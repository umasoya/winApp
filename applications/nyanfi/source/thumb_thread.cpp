//----------------------------------------------------------------------//
// NyanFi																//
//  サムネイル取得スレッド												//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <memory>
#include <algorithm>
#include "usr_wic.h"
#include "usr_shell.h"
#include "UserFunc.h"
#include "Global.h"
#include "thumb_thread.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
//スレッドの初期化
//---------------------------------------------------------------------------
__fastcall TThumbnailThread::TThumbnailThread(bool CreateSuspended) : TThread(CreateSuspended)
{
	Priority		= tpLower;
	FreeOnTerminate = true;

	CallbackWnd = NULL;
	ReqClear	= false;
	ReqStart	= false;
	ReqMake		= false;
	MakeIndex	= -1;
	ReqMake		= false;
	TaskBusy	= false;
	isEmpty 	= true;

	ThumbnailList = new TStringList();
}

//---------------------------------------------------------------------------
bool __fastcall TThumbnailThread::FitSize(int *wd, int *hi)
{
	if (*wd>ThumbnailSize || *hi>ThumbnailSize) {
		//横の方が長い
		if (*wd>*hi) {
			float r = 1.0 * *hi / *wd;
			*wd = ThumbnailSize;
			*hi = *wd * r;
		}
		//縦の方が長い
		else {
			float r = 1.0 * *wd / *hi;
			*hi = ThumbnailSize;
			*wd = *hi * r;
		}
		return true;
	}
	else
		return false;
}

//---------------------------------------------------------------------------
void __fastcall TThumbnailThread::MakeThumbnail(int idx)
{
	if (idx>=0 && idx<ThumbnailList->Count) {
		Graphics::TBitmap *bp = (Graphics::TBitmap*)ThumbnailList->Objects[idx];
		if (!bp->Empty) return;

		UnicodeString fnam = ThumbnailList->Strings[idx];	if (fnam.IsEmpty()) return;
		UnicodeString fext = get_extension(fnam);
		UnicodeString tmp;

		while (ImgViewThread->TaskBusy) Sleep(50);

		try {
			if (!file_exists(fnam)) throw Exception(EmptyStr);

			//メタファイル
			if (test_MetaExt(fext)) {
				std::unique_ptr<TMetafile> mf(new TMetafile());
				mf->LoadFromFile(fnam);
				int i_wd = mf->Width;
				int i_hi = mf->Height;
				FitSize(&i_wd, &i_hi);
				bp->SetSize(i_wd, i_hi);
				bp->Canvas->Lock();
				{
					bp->Canvas->Brush->Color = col_bgWMF;
					TRect rc = Rect(0, 0, i_wd, i_hi);
					bp->Canvas->FillRect(rc);
					bp->Canvas->StretchDraw(rc, mf.get());
				}
				bp->Canvas->Unlock();

				ThumbnailList->Strings[idx]
					= tmp.sprintf(_T("%s\t%s"), fnam.c_str(), get_wd_x_hi_str(mf->Width, mf->Height).c_str());
			}
			//ビットマップ
			else {
				int img_ori = 0;
				//アイコン
				if (test_FileExt(fext, FEXT_ICONVIEW)) {
					int size = 256;
					for (int i=0; i<3; i++) {
						HICON hIcon = usr_SH->get_Icon(fnam, size);
						if (hIcon) {
							bp->Canvas->Lock();
							{
								bp->SetSize(size, size);
								bp->Canvas->Brush->Color = col_bgImage;
								bp->Canvas->FillRect(Rect(0, 0, size, size));
								::DrawIconEx(bp->Canvas->Handle, 0, 0, hIcon, size, size, 0, NULL, DI_NORMAL);
								::DestroyIcon(hIcon);
								bp->PixelFormat = pf24bit;
							}
							bp->Canvas->Unlock();
							break;
						}
						if (size==256) size = 48; else if (size==48) size = 32;
					}
					int ixn = (int)::ExtractIcon(HInstance, fnam.c_str(), -1);
					if (ixn>0) ThumbnailList->Strings[idx] = tmp.sprintf(_T("%s\t%u Icons"), fnam.c_str(), ixn);
				}
				//画像
				else {
					int res = load_ImageFile(fnam, bp, WICIMG_THUMBNAIL, col_bgImage);
					if (res==0) throw Exception(EmptyStr);
					//Exif情報の取得
					UnicodeString exif_inf;
					//Exifを取得できる場合はそれを設定
					if (test_ExifExt(fext)) exif_inf = get_ExifInfThumb(fnam, &img_ori);
					//取得できない場合はサイズ情報を設定
					if (exif_inf.IsEmpty() && !bp->Empty) exif_inf = get_wd_x_hi_str(bp->Width, bp->Height);
					if (!exif_inf.IsEmpty())
						ThumbnailList->Strings[idx] = tmp.sprintf(_T("%s\t%s"), fnam.c_str(), exif_inf.c_str());
					if (res==LOADED_BY_WIC) img_ori = 0;
				}

				//リサイズ
				int i_wd = bp->Width;
				int i_hi = bp->Height;
				if (FitSize(&i_wd, &i_hi)) {
					std::unique_ptr<Graphics::TBitmap> r_bp(new Graphics::TBitmap());
					if (!WIC_resize_image(bp, r_bp.get(), 0.0, i_wd, i_hi, 0)) throw Exception(EmptyStr);
					bp->Handle = r_bp->ReleaseHandle();
				}

				//回転
				if (RotViewImg && img_ori>0) {
					if      (img_ori==6) WIC_rotate_image(bp, 1);
					else if (img_ori==8) WIC_rotate_image(bp, 3);
				}
			}
		}
		catch (...) {
			ThumbnailList->Strings[idx] = EmptyStr;
		}

		//メインに通知
		if (CallbackWnd) ::SendMessage(CallbackWnd, WM_NYANFI_THUMBNAIL,(WPARAM)0, (LPARAM)idx);
	}
}

//---------------------------------------------------------------------------
void __fastcall TThumbnailThread::Execute()
{
	::CoInitialize(NULL);

	while (!Terminated) {
		//サムネイルリストの作成
		if (ReqStart) {
			TaskBusy = true;
			isEmpty  = true;
			for (int i=0; !ReqClear && i<ThumbnailList->Count; i++) MakeThumbnail(i);
			ReqStart = false;
			isEmpty  = !ReqClear;
			TaskBusy = false;
		}
		//個別作成
		else if (ReqMake) {
			TaskBusy = true;
			if (MakeIndex>=0 && MakeIndex<ThumbnailList->Count) MakeThumbnail(MakeIndex);
			ReqMake  = false;
			TaskBusy = false;
		}

		//リストをクリア
		if (ReqClear) {
			TaskBusy = true;
			for (int i=0; i<ThumbnailList->Count; i++) delete (Graphics::TBitmap*)ThumbnailList->Objects[i];
			ThumbnailList->Clear();
			ReqStart = false;
			ReqClear = false;
			isEmpty  = true;
			TaskBusy = false;
		}
		Sleep(50);
	}

	delete ThumbnailList;

	::CoUninitialize();
}
//---------------------------------------------------------------------------
