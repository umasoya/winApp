//----------------------------------------------------------------------//
// NyanFi																//
//  ファイル情報														//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <memory>
#include <Vcl.Clipbrd.hpp>
#include "UserFunc.h"
#include "Global.h"
#include "MainFrm.h"
#include "FileInfDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFileInfoDlg *FileInfoDlg;

//---------------------------------------------------------------------------
__fastcall TFileInfoDlg::TFileInfoDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::FormCreate(TObject *Sender)
{
	FileRec    = NULL;
	CmdStr	   = EmptyStr;
	isPlayFile = isAppInfo = false;

	ListScrPanel = new UsrScrollPanel(ListPanel, InfListBox, USCRPNL_FLAG_P_WP | USCRPNL_FLAG_L_WP);

	InfListBox->Tag = LBTAG_FIF_LIST | LBTAG_OPT_FIF2 | LBTAG_OPT_ZOOM;
}
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::FormShow(TObject *Sender)
{
	CmdStr = EmptyStr;

	IniFile->LoadPosInfo(this, DialogCenter);
	InfListBox->Color = col_bgInf;
	InfListBox->Font->Assign(FileInfFont);
	set_ListBoxItemHi(InfListBox);
	set_UsrScrPanel(ListScrPanel);

	UpdateInfo();
}
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	IniFile->SavePosInfo(this);

	FileRec    = NULL;
	isPlayFile = isAppInfo = false;
}
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::FormDestroy(TObject *Sender)
{
	delete ListScrPanel;
}

//---------------------------------------------------------------------------
//一覧の更新
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::UpdateInfo()
{
	if (isAppInfo) {
		set_FormTitle(this, _T("アプリケーション情報"));
		ListScrPanel->UpdateKnob();
		return;
	}

	if (!FileRec) FileRec = NyanFiForm->GetCurFrecPtr(false, true);

	if (FileRec) {
		cursor_HourGlass();

		Caption = UnicodeString().sprintf(_T("%s の情報"), (FileRec->is_up? FileRec->p_name : FileRec->n_name).c_str());

		if (!FileRec->is_dir && FileRec->is_virtual && !FileRec->failed) FileRec->failed = !SetTmpFile(FileRec);
		if (!FileRec->is_dummy) GetFileInfList(FileRec, true);
		std::unique_ptr<TStringList> ilst(new TStringList());
		ilst->Assign(FileRec->inf_list);
		if (ilst->Count>=3) {
			int idx = 0;
			if (!FileRec->is_up) {
				ilst->Strings[idx] = make_PropLine(FileRec->is_dir? _T("ディレクトリ名") : _T("ファイル名"), ilst->Strings[idx]);
				idx++;
				ilst->Strings[idx] = make_PropLine(_T("場所"), ilst->Strings[idx]);
			}
			else {
				ilst->Delete(0);
				ilst->Strings[idx] = make_PropLine(_T("ディレクトリ名"), ilst->Strings[idx]);
			}
			idx++;
			ilst->Strings[idx++] = make_PropLine(FileRec->is_ftp? "パーミッション" : "属性", FileRec->attr_str);

			//ファイルサイズ
			if (!FileRec->is_dir) {
				//サイズ
				if (FileRec->f_size>=0) {
					ilst->Insert(idx++,
						get_PropTitle(_T("サイズ")).cat_sprintf(_T("%s (%s)"),
							get_size_str_G(FileRec->f_size, 10, SizeDecDigits, 1).c_str(), 
							get_size_str_B(FileRec->f_size, 5).c_str()));
				}
				//占有サイズ
				if (!FileRec->is_virtual && !FileRec->is_ftp) {
					int clu_sz = get_ClusterSize(FileRec->p_name);	//クラスタサイズ
					if (clu_sz>0) {
						__int64 o_size;
						if (FileRec->is_dir)
							o_size = FileRec->o_size;
						else {
							__int64 clu_n = FileRec->f_size/clu_sz;
							if (FileRec->f_size%clu_sz>0) clu_n++;
							o_size = (clu_n * clu_sz);
						}
						if (o_size>=0) {
							ilst->Insert(idx++,
								get_PropTitle(_T("占有サイズ")).cat_sprintf(_T("%s (%s)"),
									get_size_str_G(o_size, 10, SizeDecDigits, 1).c_str(), 
									get_size_str_B(o_size, 5).c_str()));
						}
					}
				}
			}

			//日時
			ilst->Insert(idx++, make_PropLine(_T("更新日時"), format_DateTime(FileRec->f_time)));
			if (!FileRec->is_virtual && !FileRec->is_ftp) {
				HANDLE hFile = ::CreateFile(cv_ex_filename(FileRec->f_name).c_str(),
					GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
				if (hFile!=INVALID_HANDLE_VALUE) {
					FILETIME c_tm, a_tm, w_tm, f_tm;
					if (::GetFileTime(hFile, &c_tm, &a_tm, &w_tm)) {
						ilst->Insert(idx++,
							make_PropLine(_T("作成日時"), format_DateTime(utc_to_DateTime(&c_tm))));
						ilst->Insert(idx++,
							make_PropLine(_T("アクセス日時"), format_DateTime(utc_to_DateTime(&a_tm))));
					}
					::CloseHandle(hFile);
				}
			}
			ilst->Insert(idx, EmptyStr);
		}
		InfListBox->Items->Assign(ilst.get());

		cursor_Default();
	}
	else {
		set_FormTitle(this, _T("ファイル情報"));
		InfListBox->Clear();
	}

	ListScrPanel->UpdateKnob();
}

//---------------------------------------------------------------------------
//一覧の描画
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::InfListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	draw_InfListBox(lp, Rect, Index, State);
	draw_ListCursor2(lp, Rect, Index, State);
}

//---------------------------------------------------------------------------
//キー操作
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::InfListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	UnicodeString cmd_F  = Key_to_CmdF(KeyStr);
	UnicodeString cmd_V  = Key_to_CmdV(KeyStr);
	TListBox *lp = (TListBox*)Sender;

	bool handled = true;
	if		(ExeCmdListBox(lp, cmd_F) || ExeCmdListBox(lp, cmd_V))	;
	//前後のファイルへ切り替え
	else if (FileRec && !FileRec->is_ftp && !isPlayFile && contained_wd_i(_T("PrevFile|NextFile"), cmd_V)) {
		CmdStr = cmd_V;
		this->Perform(WM_SETREDRAW, 0, 0);	//画面を消さずに残す
		ModalResult = mrRetry;
	}
	else if (USAME_TI(cmd_F, "PropertyDlg"))		PropertyDlgAction->Execute();
	else if (SameText(KeyStr, KeyStr_Copy))			CopyAction->Execute();
	else if (contained_wd_i(KeysStr_Popup, KeyStr))	show_PopupMenu(lp);
	else if	(equal_ESC(KeyStr))						ModalResult = mrOk;
	else handled = false;

	if (handled) Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::InfListBoxKeyPress(TObject *Sender, System::WideChar &Key)
{
	//インクリメンタルサーチを回避
	if (_istalnum(Key) || Key==VK_SPACE) Key = 0;
}

//---------------------------------------------------------------------------
//ファイル情報をコピー
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::CopyInfoActionExecute(TObject *Sender)
{
	copy_to_Clipboard(InfListBox->Items->Text);
}

//---------------------------------------------------------------------------
//コピー
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::CopyActionExecute(TObject *Sender)
{
	ExeCmdListBox(InfListBox, _T("ClipCopy"));
}
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::CopyActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = (InfListBox->SelCount>0);
}
//---------------------------------------------------------------------------
//すべて選択
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::SelAllActionExecute(TObject *Sender)
{
	ListBoxSelectAll(InfListBox);
}
//---------------------------------------------------------------------------
//URLを開く
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::OpenUrlActionExecute(TObject *Sender)
{
	Execute_ex(ListBoxGetURL(InfListBox));
}
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::OpenUrlActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !ListBoxGetURL(InfListBox).IsEmpty();
}
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::InfListBoxDblClick(TObject *Sender)
{
	OpenUrlAction->Execute();
}

//---------------------------------------------------------------------------
//プロパティ
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::PropertyDlgActionExecute(TObject *Sender)
{
	ShowPropertyDialog(FileRec->f_name);
}
//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::PropertyDlgActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = (FileRec && !FileRec->is_virtual);
}

//---------------------------------------------------------------------------
void __fastcall TFileInfoDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	SpecialKeyProc(this, Key, Shift, _T(HELPTOPIC_FL) _T("#ShowFileInfo"));
}
//---------------------------------------------------------------------------
