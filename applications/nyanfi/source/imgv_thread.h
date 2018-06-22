//----------------------------------------------------------------------//
// NyanFi																//
//  画像ビュー処理スレッド												//
//----------------------------------------------------------------------//
#ifndef ImgViewThreadH
#define ImgViewThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>

//---------------------------------------------------------------------------
class TImgViewThread : public TThread
{
private:
	UnicodeString Img_f_name;
	__int64		  Img_f_size;
	TDateTime	  Img_f_time;
	UnicodeString Img_rot_str;	//メタファイルの回転・反転処理履歴

	UnicodeString MsgStr;
	bool MsgIsErr;

	void __fastcall ClearImage();
	void __fastcall DrawImage();
	void __fastcall DrawViewBuff();
	void __fastcall DrawMessage();
	void __fastcall Execute();

public:
	UnicodeString FileName;	//画像ファイル名
	UnicodeString FileName2;

	bool TaskBusy;			//処理中
	bool ReqRedraw;			//再描画を要求
	bool ReqReload;			//リロードを要求
	bool ReqChange;			//ビットマップ変更を要求
	bool ReqClear;			//画面クリアを要求
	bool ReqVideo;			//動画(サムネイル表示を要求)
	int  Rotation;			//回転/反転を要求 (1,3,4,5)
	int  ExifOri;			//Exif の方向情報
	int  ZoomRatio;			//ズーム倍率(%)
	double ZoomRatioF;

	bool Fitted;			//フィット表示
	bool GrayScaled;		//グレースケール表示
	bool ShowGrid;			//グリッドを表示
	bool UseViewBuff;		//メイン側で設定されたバッファ内容を使用

	bool ImgIsWmf;

	Graphics::TBitmap *ImgBuff;		//画像バッファ(元サイズ)
	Graphics::TBitmap *ViewBuff;	//表示バッファ
	Graphics::TBitmap *ChgBuff;
	TMetafile *MetaBuff;			//メタファイル用バッファ

	__fastcall TImgViewThread(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
