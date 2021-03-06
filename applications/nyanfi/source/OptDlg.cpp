//----------------------------------------------------------------------//
// NyanFi																//
//  オプション設定														//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <memory>
#include <algorithm>
#include <mmsystem.h>
#include <Vcl.FileCtrl.hpp>
#include <RegularExpressions.hpp>
#include "usr_shell.h"
#include "usr_xd2tx.h"
#include "usr_arc.h"
#include "UserFunc.h"
#include "UserMdl.h"
#include "Global.h"
#include "CmdListDlg.h"
#include "OptDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOptionDlg *OptionDlg = NULL;

//---------------------------------------------------------------------------
__fastcall TOptionDlg::TOptionDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormCreate(TObject *Sender)
{
	cursor_HourGlass();
	set_ComboBox_AutoComp(this);

	FindMarkList = new MarkList(this);
	ColBufList	 = new TStringList();

	set_ButtonMark(FindDownBtn, UBMK_BDOWN);
	set_ButtonMark(FindUpBtn,   UBMK_BUP);

	for (int i=0; i<MAX_KEYTABS; i++) KeyListBuf[i] = new TStringList();
	FKeyLabelBuf = new TStringList();

	set_ComboBoxText(LayoutComboBox,
		_T("ファイルリストが上\nファイルリストが左\nファイルリストが右\n"));

	set_ComboBoxText(SizeFmtComboBox,
		_T("GB/MB/バイト\nGB/MB/KB/バイト\nGB/MB/KB\n"));

	set_ComboBoxText(TabStyleComboBox,
		_T("タブ\nタブ(斜)\nボタン\nフラット\n"));

	set_ComboBoxText(ScrBarStyleComboBox,
		_T("標準\nシンプル\nシンプル(3/4幅)\nシンプル(1/2幅)\nシンプル(画像)\n"));

	for (int i=0; i<FontList->Count-1; i++) FontComboBox->Items->Add(FontList->ValueFromIndex[i]);

	set_ComboBoxText(SizeDecDgtComboBox, _T("0\n1\n2\n3\n"));
	set_ComboBoxText(MaxTasksComboBox,   _T("1\n2\n3\n4\n"));

	OptColListBox->Items->Text =
		"bgList=リストの背景色\n"
		"bgList2=リストの背景色(交互)\n"
		"fgList=リストの文字色\n"
		"bgArc=仮想ディレクトリの背景色\n"
		"bgFind=検索結果リストの背景色\n"
		"bgWork=ワークリストの背景色\n"
		"bgFTP=FTPリモート側の背景色\n"
		"selItem=|選択項目の背景色\n"
		"oppItem=反対パス選択項目の背景色\n"
		"fgSelItem=選択項目の文字色\n"
		"bgMark=マーク項目の背景色\n"
		"matchItem=INC.サーチのマッチ項目背景色\n"
		"Folder=ディレクトリの文字色\n"
		"SymLink=ディレクトリ(シンボリック)の文字色\n"
		"Protect=ディレクトリ(削除制限)の文字色\n"
		"ReadOnly=読み込み専用属性の文字色\n"
		"Hidden=隠し属性の文字色\n"
		"System=システム属性の文字色\n"
		"fgTagNam=タグ名の文字色\n"
		"InvItem=無効な項目の文字色\n"
		"Cursor=ラインカーソルの色\n"
		"bgScrBar=|シンプルスクロールバーの背景色\n"
		"bgScrKnob=シンプルスクロールノブの色\n"
		"frScrKnob=シンプルスクロールノブの輪郭色\n"
		"Splitter=分割境界線\n"
		"bgTlBar1=ツールバーのグラデーション開始色\n"
		"bgTlBar2=ツールバーのグラデーション終了色\n"
		"fgTlBar=ツールバーの文字色\n"
		"htTlBar=ツールバーのホットトラッキング色\n"
		"bgTabBar=|タブバーの背景色\n"
		"bgActTab=アクティブなタブの背景色\n"
		"bgInAcTab=非アクティブなタブの背景色\n"
		"frmTab=タブの輪郭線\n"
		"fgTab=タブの文字色\n"
		"bgListHdr=|リストヘッダの背景色\n"
		"fgListHdr=リストヘッダの文字色\n"
		"bgDirInf=ディレクトリ情報の背景色\n"
		"fgDirInf=ディレクトリ情報の文字色\n"
		"bgDirRel=ディレクトリ関係の背景色\n"
		"fgDirRel=ディレクトリ関係の文字色\n"
		"bgDrvInf=ドライブ情報の背景色\n"
		"fgDrvInf=ドライブ情報の文字色\n"
		"bgInf=|ファイル情報の背景色\n"
		"fgInf=ファイル情報の文字色\n"
		"fgInfNam=ファイル情報の項目名文字色\n"
		"bgTxtPrv=テキストプレビューの背景色\n"
		"fgTxtPrv=テキストプレビューの文字色\n"
		"bgLog=ログの背景色\n"
		"fgLog=ログの文字色\n"
		"bgTask=タスク状態表示の背景色\n"
		"fgPrgBar=タスク進捗バー色\n"
		"bgPrgBar=タスク進捗背景色\n"
		"Error=エラー/注意の文字色\n"
		"bgSttBar=|ステータスバーの背景色\n"
		"fgSttBar=ステータスバーの文字色\n"
		"bgInfHdr=情報ヘッダの背景色\n"
		"fgInfHdr=情報ヘッダの文字色\n"
		"bgView=|テキストビュアーの背景色\n"
		"fgView=テキストビュアーの文字色\n"
		"Margin=テキストビュアーの余白白\n"
		"bgRuler=ルーラの背景色\n"
		"fgRuler=ルーラの目盛色\n"
		"bgLineNo=行番号背景色\n"
		"LineNo=行番号文字色\n"
		"Mark=行マーク\n"
		"bdrLine=行番号境界線\n"
		"bdrFold=折り返し境界線\n"
		"bdrFixed=固定長表示の縦罫線\n"
		"Comment=|コメントの文字色\n"
		"Strings=文字列の文字色\n"
		"Reserved=予約語の文字色\n"
		"Symbol=シンボルの文字色\n"
		"Numeric=数値の文字色\n"
		"fgEmpBin1=バイナリ強調文字色1\n"
		"fgEmpBin2=バイナリ強調文字色2\n"
		"fgEmpBin3=バイナリ強調文字色3\n"
		"Headline=見出しの文字色\n"
		"Ruby=ルビ\n"
		"URL=URLの文字色\n"
		"LocalLink=ローカルファイルへのリンク\n"
		"fgEmp=強調文字色\n"
		"bgEmp=強調背景色\n"
		"TAB=タブ表示色\n"
		"CR=改行表示色\n"
		"HR=罫線の色\n"
		"Ctrl=コントロールコード\n"
		"bgImage=|画像の背景色\n"
		"bgDblPg=見開き表示の余白色\n"
		"bgWMF=メタファイルの背景色\n"
		"bdrThumb=サムネイルの境界線\n"
		"ThumbExif=サムネイルのExif情報\n"
		"ImgGrid=画像分割グリッド\n"
		"bgHint=|ヒント表示の背景色\n"
		"fgHint=ヒント表示の文字色\n"
		"bgWarn=警告表示の背景色\n"
		"Invalid=無効な項目の背景色\n"
		"Illegal=不正な入力項目の背景色\n"
		"OptFind=オプション設定の検索結果\n";

	TimColListBox->Items->Text =
		"Tim1H=1時間以内\n"
		"Tim3H=3時間以内\n"
		"Tim1D=1日以内\n"
		"Tim3D=3日以内\n"
		"Tim7D=7日以内\n"
		"Tim1M=1ヶ月以内\n"
		"Tim3M=3ヶ月以内\n"
		"Tim6M=6ヶ月以内\n"
		"Tim1Y=1年以内\n"
		"Tim3Y=3年以内\n"
		"TimOld=それより前\n"
		"Size4G=|4GB以上\n"
		"Size1G=1GB以上\n"
		"Size1M=1MB以上\n"
		"Size1K=1KB以上\n"
		"SizeLT=1KB未満\n"
		"Size0=0\n";

	set_ComboBoxText(BgImgModeComboBox,
		_T("非表示\n")
		_T("2画面に渡って表示\n")
		_T("それぞれに表示\n")
		_T("デスクトップ背景を表示\n"));
	set_ComboBoxText(BgImgSubModeComboBox,
		_T("中央に表示\n")
		_T("並べて表示\n")
		_T("表示サイズにストレッチ\n")
		_T("表示サイズに合わせて切り出し\n"));

	set_ComboBoxText(ScaleOptComboBox,
		_T("ニアレストネイバー\nバイリニア\nバイキュービック\nファントリサンプリング\n補間しない\n"));

	//マウスホイール
	set_ComboBoxText(WheelCmdF0ComboBox,
		_T("ScrollUp/ScrollDown\n")
		_T("PageUp/PageDown\n")
		_T("CursorUp/CursorDown\n")
		_T("PrevDrive/NextDrive\n")
		_T("PrevTab/NextTab\n")
		_T("BackDirHist/ForwardDirHist\n")
		_T("BorderLeft/BorderRight\n")
		_T("ScrollUpLog/ScrollDownLog\n")
		_T("ScrollUpText/ScrollDownText\n")
		_T("SetSubSize_-16/SetSubSize_+16\n")
		_T("ZoomIn/ZoomOut\n")
		_T("PrevNyanFi/NextNyanFi\n"));
	WheelCmdF1ComboBox->Items->Assign(WheelCmdF0ComboBox->Items);
	WheelCmdF2ComboBox->Items->Assign(WheelCmdF0ComboBox->Items);
	WheelCmdF3ComboBox->Items->Assign(WheelCmdF0ComboBox->Items);

	set_ComboBoxText(WheelCmdV0ComboBox,
		_T("ScrollUp/ScrollDown\n")
		_T("ScrollCursorUp/ScrollCursorDown\n")
		_T("CursorUp/CursorDown\n")
		_T("PageUp/PageDown\n")
		_T("ZoomIn/ZoomOut\n")
		_T("PrevFile/NextFile\n")
		_T("PrevNyanFi/NextNyanFi\n"));
	WheelCmdV1ComboBox->Items->Assign(WheelCmdV0ComboBox->Items);
	WheelCmdV2ComboBox->Items->Assign(WheelCmdV0ComboBox->Items);
	WheelCmdV3ComboBox->Items->Assign(WheelCmdV0ComboBox->Items);

	set_ComboBoxText(WheelCmdI0ComboBox,
		_T("PrevFile/NextFile\n")
		_T("PrevPage/NextPage\n")
		_T("ScrollUp/ScrollDown\n")
		_T("ScrollLeft/ScrollRight\n")
		_T("ZoomIn/ZoomOut\n")
		_T("RotateLeft/RotateRight\n")
		_T("PrevNyanFi/NextNyanFi\n"));
	WheelCmdI1ComboBox->Items->Assign(WheelCmdI0ComboBox->Items);
	WheelCmdI2ComboBox->Items->Assign(WheelCmdI0ComboBox->Items);
	WheelCmdI3ComboBox->Items->Assign(WheelCmdI0ComboBox->Items);

	//マウスボタン
	set_ComboBoxText(WheelBtnF0ComboBox,
		_T("ToRoot\n")
		_T("ToParent\n")
		_T("BackDirHist\n")
		_T("ForwardDirHist\n")
		_T("EqualListWidth\n")
		_T("HomeWorkList\n")
		_T("ZoomReset\n"));
	WheelBtnF1ComboBox->Items->Assign(WheelBtnF0ComboBox->Items);
	WheelBtnF2ComboBox->Items->Assign(WheelBtnF0ComboBox->Items);
	WheelBtnF3ComboBox->Items->Assign(WheelBtnF0ComboBox->Items);

	set_ComboBoxText(X1BtnFComboBox,
		_T("BackDirHist\n")
		_T("ForwardDirHist\n")
		_T("ToParent\n")
		_T("PrevDrive\n")
		_T("NextDrive\n")
		_T("PrevTab\n")
		_T("NextTab\n")
		_T("PageUp\n")
		_T("PageDown\n")
		_T("NextNyanFi\n")
		_T("PrevNyanFi\n"));
	X2BtnFComboBox->Items->Assign(X1BtnFComboBox->Items);

	set_ComboBoxText(WheelBtnVComboBox,
		_T("ZoomReset\n")
		_T("TextTop\n")
		_T("TextEnd\n")
		_T("Close\n"));

	set_ComboBoxText(X1BtnVComboBox,
		_T("BackViewHist\n")
		_T("PrevFile\n")
		_T("NextFile\n")
		_T("PageUp\n")
		_T("PageDown\n")
		_T("NextNyanFi\n")
		_T("PrevNyanFi\n"));
	X2BtnVComboBox->Items->Assign(X1BtnVComboBox->Items);

	set_ComboBoxText(WheelBtnIComboBox,
		_T("FittedSize\n")
		_T("FullScreen\n")
		_T("TopFile\n")
		_T("EndFile\n")
		_T("Close\n"));

	set_ComboBoxText(X1BtnIComboBox,
		_T("PrevFile\n")
		_T("NextFile\n")
		_T("PrevPage\n")
		_T("NextPage\n")
		_T("Histogram\n")
		_T("Loupe\n")
		_T("Thumbnail\n")
		_T("ThumbnailEx\n")
		_T("WarnHighlight\n")
		_T("NextNyanFi\n")
		_T("PrevNyanFi\n"));
	X2BtnIComboBox->Items->Assign(X1BtnIComboBox->Items);

	set_ComboBoxText(MenuCmdComboBox,
		_T("MaskSel      マスクに該当するファイルを選択\n")
		_T("MaskDel      マスクに該当するファイルを削除\n")
		_T("MaskCopy     マスクに該当するファイルをコピー\n")
		_T("MaskMove     マスクに該当するファイルを移動\n")
		_T("MaskPack     マスクに該当するファイルを圧縮\n")
		_T("ExeCommands  指定したコマンドを実行\n"));
	RefMenuPrmBtn->Enabled = false;

	make_KeyList(KeyComboBox->Items);
	make_KeyList(Key2ComboBox->Items);
	Key2ComboBox->Items->Insert(0, EmptyStr);
	Key2ComboBox->Enabled = false;

	make_KeyList(HotKeyComboBox->Items);
	HotKeyComboBox->Items->Insert(0, EmptyStr);
	make_KeyList(AppKeyComboBox->Items);
	AppKeyComboBox->Items->Insert(0, EmptyStr);
	set_ComboBoxText(AppPrmComboBox,
		_T("\n")
		_T("FA : 一覧側\n")
		_T("FL : ランチャー側\n")
		_T("FI : INC.サーチ\n")
		_T("AO : 一覧のみ表示\n")
		_T("LO : ランチャーのみ表示\n")
		_T("LI : ランチャーのみ(INC.)\n"));

	set_ComboBoxText(OpenByModeComboBox, _T("そのまま\nWindowsの関連付けで開く\n標準の Enter キー動作\n"));

	set_ComboBoxText(DblClickFlComboBox,
		_T("標準の Enter キー動作 (OpenStandard)\n")
		_T("Windowsの関連付けで開く (OpenByWin)\n")
		_T("独自の関連付けで開く (OpenByApp)\n"));

	set_ComboBoxText(DefDropComboBox, _T("Windows 標準\n常にコピー\n常に移動\n"));

	set_ComboBoxText(StdCmdComboBox,
		_T("Windowsの関連付けで開く (OpenByWin)\n")
		_T("独自の関連付けで開く (OpenByApp)\n")
		_T("テキストビュアーで開く (TextViewer)\n")
		_T("エディタで開く (FileEdit)\n")
		_T("バイナリエディタで開く (BinaryEdit)\n")
		_T("コマンドファイルとして実行 (ExeCommands)\n")
		_T("指定コマンドを実行 (ExeCommands_)\n"));

	LayoutChanged  = false;
	inh_KeySet	   = 0;

	UnicodeString hntstr   = LoadUsrMsg(USTR_HintMltSepSC);
	NoWatchPathEdit->Hint  = hntstr;
	NoInfPathEdit->Hint    = hntstr;
	NoImgPrvPathEdit->Hint = hntstr;
	InsHrClsEdit->Hint	   = hntstr;
	NoDirHistPathEdit->Hint= hntstr;
	MarkPathEdit->Hint	   = hntstr;

	hntstr = LoadUsrMsg(USTR_HintMltFExt);
	ExtColorEdit->Hint	   = hntstr;
	FExtGetInfEdit->Hint   = hntstr;
	FExtNoInfEdit->Hint    = hntstr;
	FExtImgPrvEdit->Hint   = hntstr;
	FExtNoImgPrvEdit->Hint = hntstr;
	FExtNoIViewEdit->Hint  = hntstr;
	MarkFextEdit->Hint	   = hntstr;
	FExt7zDllEdit->Hint    = hntstr;
	FExtImgEdit->Hint	   = hntstr;
	FExtEtcEdit->Hint	   = hntstr;
	ViewTab4Edit->Hint	   = hntstr;
	FExtExeFileEdit->Hint  = hntstr;
	FExtStdEdit->Hint	   = hntstr;

	//コントロールのタグに、対応する変数のポインタを設定
	//UnicodeString
	L_IniPatEdit->Tag			= (int)&InitialPath[0];
	R_IniPatEdit->Tag			= (int)&InitialPath[1];
	TextEditorEdit->Tag 		= (int)&TextEditor;
	TextEditorFrmtEdit->Tag 	= (int)&TextEditorFrmt;
	ExtTxViewerEdit->Tag		= (int)&ExtTxViewer;
	ExtTxViewerFmtEdit->Tag 	= (int)&ExtTxViewerFrmt;
	ImageEditorEdit->Tag		= (int)&ImageEditor;
	FExtImgEdit->Tag			= (int)&FExtImgEidt;
	BinaryEditorEdit->Tag		= (int)&BinaryEditor;
	SndWarnEdit->Tag			= (int)&SoundWarning;
	SndTaskFinEdit->Tag 		= (int)&SoundTaskFin;
	SndFindFinEdit->Tag 		= (int)&SoundFindFin;
	SndWatchEdit->Tag			= (int)&SoundWatch;
	DirBraEdit->Tag 			= (int)&DirBraStr;
	DirKetEdit->Tag 			= (int)&DirKetStr;
	DrvInfFmtREdit->Tag 		= (int)&DrvInfFmtR;
	DrvInfFmtSEdit->Tag 		= (int)&DrvInfFmtS;
	DrvInfFmtNEdit->Tag 		= (int)&DrvInfFmtN;
	SttBarFmtEdit->Tag			= (int)&SttBarFmt;
	ClockFmtEdit->Tag			= (int)&SttClockFmt;
	TmStampFmtEdit->Tag 		= (int)&TimeStampFmt;
	ViewTab4Edit->Tag			= (int)&FExtViewTab4;
	EmpBin1Edit->Tag			= (int)&EmpBinPtn1;
	EmpBin2Edit->Tag			= (int)&EmpBinPtn2;
	EmpBin3Edit->Tag			= (int)&EmpBinPtn3;
	InsHrClsEdit->Tag			= (int)&HtmInsHrCls;
	HdrStrEdit->Tag				= (int)&HtmHdrStr;
	FExtGetInfEdit->Tag 		= (int)&FExtGetInf;
	FExtNoInfEdit->Tag			= (int)&FExtNoInf;
	NoWatchPathEdit->Tag		= (int)&NoWatchPath;
	NoInfPathEdit->Tag			= (int)&NoInfPath;
	NoImgPrvPathEdit->Tag		= (int)&NoImgPrvPath;
	FExtImgPrvEdit->Tag 		= (int)&FExtImgPrv;
	FExtNoImgPrvEdit->Tag		= (int)&FExtNoImgPrv;
	FExtNoIViewEdit->Tag		= (int)&FExtNoIView;
	MarkPathEdit->Tag			= (int)&MarkImgPath;
	MarkFextEdit->Tag			= (int)&MarkImgFExt;
	MarkMemoEdit->Tag			= (int)&MarkImgMemo;
	FExt7zDllEdit->Tag			= (int)&FExt7zDll;
	AutoRenFmtEdit->Tag			= (int)&AutoRenFmt;
	NoDirHistPathEdit->Tag		= (int)&NoDirHistPath;
	FExtExeFileEdit->Tag		= (int)&FExtExeFile;
	HomeWorkListEdit->Tag		= (int)&HomeWorkList;
	WebSeaUrlEdit->Tag			= (int)&WebSeaUrl;
	BgImg1Edit->Tag				= (int)&BgImgName[0];
	BgImg2Edit->Tag				= (int)&BgImgName[1];
	SpImgEdit->Tag				= (int)&BgImgName[2];
	SpImg2Edit->Tag				= (int)&BgImgName[3];
	KnobImgEdit->Tag			= (int)&BgImgName[4];
	TimerCnd1Edit->Tag			= (int)&Timer_Condition[0];
	TimerCmd1Edit->Tag			= (int)&OnTimerEvent[0];
	TimerCnd2Edit->Tag			= (int)&Timer_Condition[1];
	TimerCmd2Edit->Tag			= (int)&OnTimerEvent[1];

	//int (NumbersOnly = true)
	BatLowLimitEdit->Tag		= (int)&BatLowerLimit;
	MaxLogLinesEdit->Tag		= (int)&MaxLogLines;
	MaxLogFilesEdit->Tag		= (int)&MaxLogFiles;
	FwCntWarnEdit->Tag			= (int)&FlashCntWarning;
	FwTmWarnEdit->Tag			= (int)&FlashTimeWarning;
	FwCntTaskFinEdit->Tag		= (int)&FlashCntTaskfin;
	FwTmTaskFinEdit->Tag		= (int)&FlashTimeTaskfin;
	NotifyTaskTimeEdit->Tag		= (int)&NotifyTaskTime;
	FrmMgnEdit->Tag				= (int)&ImgFrameMargin;
	FitMaxZoomEdit->Tag			= (int)&ImgFitMaxZoom;
	MinShowTimeEdit->Tag		= (int)&MinShowTime;
	IniWinLeftEdit->Tag			= (int)&IniWinLeft;
	IniWinTopEdit->Tag			= (int)&IniWinTop;
	MsgHintTimeEdit->Tag		= (int)&MsgHintTime;
	KeyHintTimeEdit->Tag		= (int)&KeyHintTime;
	ImgGridHEdit->Tag			= (int)&ImgGridHorzN;
	ImgGridVEdit->Tag			= (int)&ImgGridVertN;
	ThumbSizeEdit->Tag	 		= (int)&ThumbnailSize;
	ThumbBdrEdit->Tag			= (int)&ThumbBdrWidth;
	IntrPgEdit->Tag				= (int)&ImgDblMargin;
	BdrMovWdLabeledEdit->Tag	= (int)&BorderMoveWidth;
	ListInterLnEdit->Tag		= (int)&ListInterLn;
	CursorWidthEdit->Tag		= (int)&CursorWidth;
	CursorAlphaEdit->Tag		= (int)&CursorAlpha;
	ListPercentEdit->Tag		= (int)&ListPercent;
	TabWidthEdit->Tag			= (int)&FlTabWidth;
	TextInterLnEdit->Tag		= (int)&ViewTxtInterLn;
	LeftMarginEdit->Tag 		= (int)&ViewLeftMargin;
	FoldWdEdit->Tag 			= (int)&ViewFoldWidth;
	FixedLimitEdit->Tag			= (int)&ViewFixedLimit;
	ViewWheelScrLnEdit->Tag 	= (int)&ViewWheelScrLn;
	ListWheelScrLnEdit->Tag 	= (int)&ListWheelScrLn;
	PrvTxtInfLnEdit->Tag		= (int)&PrvTxtInfLn;
	PrvTxtTailLnEdit->Tag		= (int)&PrvTxtTailLn;
	PrvActTailLnEdit->Tag		= (int)&PrvActTailLn;
	WatchIntervalEdit->Tag		= (int)&WatchInterval;
	LimitSndEdit->Tag			= (int)&PlaySndLimitTime;
	LogIntervalEdit->Tag		= (int)&LogInterval;
	IniWinWdEdit->Tag			= (int)&IniWinWidth;
	IniWinHiEdit->Tag			= (int)&IniWinHeight;
	DlgMoveEdit->Tag			= (int)&DlgMovePrm;
	DlgSizeEdit->Tag			= (int)&DlgSizePrm;
	CmpDelOwCntEdit->Tag		= (int)&CmpDelOwCnt;
	MigemoMinEdit->Tag			= (int)&IncSeaMigemoMin;
	ToleranceEdit->Tag 			= (int)&TimeTolerance;
	RemoteWaitTimeEdit->Tag 	= (int)&RemoteWaitTime;
	NormalWaitTimeEdit->Tag 	= (int)&NormalWaitTime;
	NopDtctTimeEdit->Tag		= (int)&NopDtctTime;
	ModalAlphaEdit->Tag 		= (int)&ModalScrAlpha;
	BgColAlphaEdit->Tag 		= (int)&BgColAlpha;
	HideTimeEdit->Tag			= (int)&BgHideTime;
	TileIfSzEdit->Tag			= (int)&BgTileSize;
	SplitWidthEdit->Tag 		= (int)&SplitterWidth;
	SplitWidth2Edit->Tag 		= (int)&SplitterWidth2;
	IconCacheEdit->Tag			= (int)&IconCache;
	FindPathWdEdit->Tag 		= (int)&FindPathWidth;

	//bool
	DivFlistUDCheckBox->Tag 	= (int)&DivFileListUD;
	DivDirInfUDCheckBox->Tag	= (int)&DivDirInfUD;
	HideAtrCheckBox->Tag		= (int)&ShowHideAtr;
	SysAtrCheckBox->Tag 		= (int)&ShowSystemAtr;
	DispRegNameCheckBox->Tag	= (int)&DispRegName;
	UncToNetDriveCheckBox->Tag	= (int)&UncToNetDrive;
	PathInTitleBarCheckBox->Tag = (int)&PathInTitleBar;
	UseIndIconCheckBox->Tag 	= (int)&UseIndIcon;
	OmitEndOfNameCheckBox->Tag	= (int)&OmitEndOfName;
	DirJumboCheckBox->Tag		= (int)&ShowDirJumboIco;
	InZipImgCheckBox->Tag		= (int)&ShowInZipImg;
	AniGifCheckBox->Tag 		= (int)&PreviewAniGif;
	PrevTxtToInfCheckBox->Tag	= (int)&ShowTextPreview;
	PrevTailCheckBox->Tag		= (int)&ShowTailPreview;
	ViewArcInfCheckBox->Tag 	= (int)&ViewArcInf;
	ReloadActCheckBox->Tag		= (int)&ReloadOnActive;
	OpenAddDrvCheckBox->Tag 	= (int)&OpenAddedDrive;
	CheckUncCheckBox->Tag		= (int)&CheckUnc;
	ForceDelCheckBox->Tag		= (int)&ForceDel;
	RemoveCdRoCheckBox->Tag 	= (int)&RemoveCdReadOnly;
	CopyNoBuffCheckBox->Tag 	= (int)&CopyNoBuffering;
	ShowArcCopyPrgCheckBox->Tag = (int)&ShowArcCopyProg;
	DelUseTrsCheckBox->Tag		= (int)&DelUseTrash;
	NotShowNoTaskCheckBox->Tag	= (int)&NotShowNoTask;
	ShowMsgHintCheckBox->Tag	= (int)&ShowMsgHint;
	ShowKeyHintCheckBox->Tag	= (int)&ShowKeyHint;
	ShowNoKeyHintCheckBox->Tag	= (int)&ShowNoKeyHint;
	ShowCopyHintCheckBox->Tag	= (int)&ShowCopyHint;
	ShowDuplNotifyCheckBox->Tag = (int)&ShowDuplNotify;
	LogErrOnlyCheckBox->Tag 	= (int)&LogErrOnly;
	LogErrMsgCheckBox->Tag		= (int)&LogErrMsg;
	LogDebugInfCheckBox->Tag	= (int)&LogDebugInf;
	LogHideSkipCheckBox->Tag	= (int)&LogHideSkip;
	LogFullPathCheckBox->Tag	= (int)&LogFullPath;
	SaveLogCheckBox->Tag		= (int)&SaveLog;
	AppendLogCheckBox->Tag		= (int)&AppendLog;
	NotSortWLCheckBox->Tag		= (int)&NotSortWorkList;
	AutoDelWLCheckBox->Tag		= (int)&AutoDelWorkList;
	DirHistCsrPosCheckBox->Tag	= (int)&DirHistCsrPos;
	DelDplDirHistCheckBox->Tag	= (int)&DelDplDirHist;
	WorkToDirHistCheckBox->Tag  = (int)&WorkToDirHist;
	NoChkDirHistCheckBox->Tag	= (int)&NoCheckDirHist;
	ExtSavDirHistCheckBox->Tag	= (int)&ExtSaveDirHist;
	LoopFlCsrCheckBox->Tag		= (int)&LoopFilerCursor;
	FlCsrVisiCheckBox->Tag		= (int)&FlCursorVisible;
	AutoCompCheckBox->Tag		= (int)&AutoCompComboBox;
	AutoHotkeyCheckBox->Tag 	= (int)&MenuAutoHotkey;
	EscHelpCheckBox->Tag		= (int)&EscapeHelp;
	InhAltMenuCheckBox->Tag		= (int)&InhbitAltMenu;
	SelColDrvInfCheckBox->Tag	= (int)&SelColDrvInf;
	PermitCmdsCheckBox->Tag 	= (int)&PermitDotCmds;
	InheritDotCheckBox->Tag 	= (int)&InheritDotNyan;
	DotPerUserCheckBox->Tag 	= (int)&DotNyanPerUser;
	SelByMouseCheckBox->Tag 	= (int)&SelectByMouse;
	SelBaseOnlyCheckBox->Tag	= (int)&SelectBaseOnly;
	SelIconCheckBox->Tag		= (int)&SelectIconSngl;
	PriFExtColCheckBox->Tag 	= (int)&PriorFExtCol;
	ColOnlyFExtCheckBox->Tag	= (int)&ColorOnlyFExt;
	ShowIconCheckBox->Tag		= (int)&ShowIcon;
	ShowByteSizeCheckBox->Tag	= (int)&ShowByteSize;
	ShowImgPrvCheckBox->Tag 	= (int)&ShowImgPreview;
	ShowFilePropCheckBox->Tag	= (int)&ShowProperty;
	HideScrBarCheckBox->Tag 	= (int)&HideScrBar;
	ShowHeaderCheckBox->Tag 	= (int)&ShowHeader;
	FixListCheckBox->Tag		= (int)&FixListWidth;
	KeepOnResizeCheckBox->Tag	= (int)&KeepOnResize;
	KeepCurListWdCheckBox->Tag	= (int)&KeepCurListWidth;
	NoSpcFExtCheckBox->Tag		= (int)&NoSpaceFExt;
	NoClsTabBtnCheckBox->Tag	= (int)&NoClsTabBtn;
	ShowMainMenuCheckBox->Tag	= (int)&ShowMainMenu;
	HideTitMenuCheckBox->Tag	= (int)&HideTitleMenu;
	FlatInfPanelCheckBox->Tag	= (int)&FlatInfPanel;
	GrayScaleCheckBox->Tag		= (int)&BgImgGray;
	HideScrCheckBox->Tag		= (int)&BgImgHideScr;
	TileIfCheckBox->Tag 		= (int)&BgImgTileIf;
	ModalScrCheckBox->Tag		= (int)&ModalScreen;
	DlgCenterCheckBox->Tag		= (int)&DialogCenter;
	ShowLnNoCheckBox->Tag		= (int)&ShowLineNo;
	ShowLnCsrCheckBox->Tag		= (int)&ShowLineCursor;
	ShowTabCheckBox->Tag		= (int)&ShowTAB;
	ShowCrCheckBox->Tag 		= (int)&ShowCR;
	ShowRulerCheckBox->Tag		= (int)&ShowTextRuler;
	TxtSttIsBtmCheckBox->Tag	= (int)&TxtSttIsBottom;
	UseXd2txCheckBox->Tag 		= (int)&UseXd2tx;
	UseXd2txCheckBox->Enabled	= xd2tx_Available;
	AozoraCheckBox->Tag 		= (int)&ChkAozora;
	ClickableUrlCheckBox->Tag	= (int)&ClickableUrl;
	RstViewLnCheckBox->Tag		= (int)&RestoreViewLine;
	TvCsrVisiCheckBox->Tag		= (int)&TvCursorVisible;
	LimitBinCsrCheckBox->Tag	= (int)&LimitBinCsr;
	ColorHintCheckBox->Tag		= (int)&TxtColorHint;
	BinMemMapedCheckBox->Tag	= (int)&BinMemMaped;
	EmRemCheckBox->Tag			= (int)&EmpComment;
	EmStrCheckBox->Tag			= (int)&EmpStrings;
	EmRsvCheckBox->Tag			= (int)&EmpReserved;
	EmSymCheckBox->Tag			= (int)&EmpSymbol;
	EmNumCheckBox->Tag			= (int)&EmpNumeric;
	EmHdlnCheckBox->Tag 		= (int)&EmpHeadline;
	EmRubyCheckBox->Tag 		= (int)&EmpRuby;
	InsHrSctCheckBox->Tag		= (int)&HtmInsHrSct;
	InsHrArtCheckBox->Tag		= (int)&HtmInsHrArt;
	InsHrNavCheckBox->Tag		= (int)&HtmInsHrNav;
	MarkdownCheckBox->Tag		= (int)&ToMarkdown;
	RotViewImgCheckBox->Tag 	= (int)&RotViewImg;
	KeepZoomCheckBox->Tag		= (int)&KeepZoomRatio;
	ThumbNameCheckBox->Tag		= (int)&ShowThumbName;
	ThumbExifCheckBox->Tag		= (int)&ShowThumbExif;
	ThumbFExtCheckBox->Tag		= (int)&ShowThumbFExt;
	NotThumbIfArcCheckBox->Tag	= (int)&NotThumbIfArc;
	LoopCurCheckBox->Tag		= (int)&LoopViewCursor;
	HintTopEndCheckBox->Tag 	= (int)&HintTopEndImg;
	BeepTopEndCheckBox->Tag 	= (int)&BeepTopEndImg;
	ImgSttIsBtmCheckBox->Tag	= (int)&ImgSttIsBottom;
	ToolBarISideCheckBox->Tag	= (int)&ToolBarISide;
	SeekBindDirCheckBox->Tag	= (int)&SeekBindDir;
	SeekSwapNPCheckBox->Tag		= (int)&SeekSwapNxtPrv;
	OneStpInDblCheckBox->Tag	= (int)&OneStepInDblPg;
	HideCsrFullCheckBox->Tag	= (int)&HideCsrInFull;
	HideThumbCheckBox->Tag		= (int)&HideThumbInFull;
	AnimateGifCheckBox->Tag 	= (int)&AnimateGif;
	ThumbScrCheckBox->Tag		= (int)&ShowThumbScroll;
	MarkImgCheckBox->Tag		= (int)&MarkImgClose;
	UseSpiFirstCheckBox->Tag	= (int)&UseSpiFirst;
	SureCopyCheckBox->Tag		= (int)&SureCopy;
	SureMoveCheckBox->Tag		= (int)&SureMove;
	SureDeleteCheckBox->Tag 	= (int)&SureDelete;
	SureUnPackCheckBox->Tag 	= (int)&SureUnPack;
	SureExecCheckBox->Tag		= (int)&SureExec;
	SureOtherCheckBox->Tag		= (int)&SureOther;
	SureExitCheckBox->Tag		= (int)&SureExit;
	SureCmpDelCheckBox->Tag 	= (int)&SureCmpDel;
	SureCancelCheckBox->Tag 	= (int)&SureCancel;
	SureDefNoCheckBox->Tag		= (int)&SureDefNo;
	WarnPowFailCheckBox->Tag	= (int)&WarnPowerFail;
	WarnLowBatheckBox->Tag		= (int)&WarnLowBattery;
	WarnDisconCheckBox->Tag 	= (int)&WarnDisconnect;
	PrtSubCheckBox->Tag			= (int)&ProtectSubDir;
	PrtFileCheckBox->Tag		= (int)&ProtectFile;
	ImgEditSglCheckBox->Tag		= (int)&ImageEditSgl;

	AppMinCheckBox->Tag			= (int)&AppListChgMin;
	OpenCmdPromptCheckBox->Tag	= (int)&OpenByCmdPrompt;
	OpenDirByStdCheckBox->Tag	= (int)&OpenDirByStd;
	AppOnlyCurCheckBox->Tag 	= (int)&OpenOnlyCurApp;
	WinOnlyCurCheckBox->Tag 	= (int)&OpenOnlyCurWin;
	DowAfterStdCheckBox->Tag	= (int)&DownAfterOpenStd;
	OpnStdResCheckBox->Tag		= (int)&OpenStdOnResList;
	IncSeaCaseCheckBox->Tag 	= (int)&IncSeaCaseSens;
	IncSeaLoopCheckBox->Tag 	= (int)&IncSeaLoop;
	IncSea1ExitCheckBox->Tag	= (int)&IncSeaMatch1Exit;
	LimitRarCheckBox->Tag		= (int)&LimitGetRarInf;
	IniPathToTab1CheckBox->Tag	= (int)&IniPathToTab1;
	IniTabHomeAllCheckBox->Tag	= (int)&IniTabHomeAll;
	SplashCheckBox->Tag 		= (int)&ShowSplash;
	FixWinCheckBox->Tag 		= (int)&FixWinPos;
	IniSeaByNumCheckBox->Tag	= (int)&IniSeaByNum;
	IniSeaBySignCheckBox->Tag	= (int)&IniSeaBySign;
	MulInstCheckBox->Tag		= (int)&MultiInstance;
	CloseOtherCheckBox->Tag 	= (int)&CloseOthers;
	TaskTrayCheckBox->Tag		= (int)&StoreTaskTray;
	FindPathColCheckBox->Tag	= (int)&FindPathColumn;

	//UnicodeString (Style = csDropDown)
	L_IniMaskComboBox->Tag		= (int)&InitialMask[0];
	R_IniMaskComboBox->Tag		= (int)&InitialMask[1];
	WheelCmdF0ComboBox->Tag 	= (int)&WheelCmdF[0];
	WheelCmdF1ComboBox->Tag 	= (int)&WheelCmdF[1];
	WheelCmdF2ComboBox->Tag 	= (int)&WheelCmdF[2];
	WheelCmdF3ComboBox->Tag 	= (int)&WheelCmdF[3];
	WheelCmdV0ComboBox->Tag 	= (int)&WheelCmdV[0];
	WheelCmdV1ComboBox->Tag 	= (int)&WheelCmdV[1];
	WheelCmdV2ComboBox->Tag 	= (int)&WheelCmdV[2];
	WheelCmdV3ComboBox->Tag 	= (int)&WheelCmdV[3];
	WheelCmdI0ComboBox->Tag 	= (int)&WheelCmdI[0];
	WheelCmdI1ComboBox->Tag 	= (int)&WheelCmdI[1];
	WheelCmdI2ComboBox->Tag 	= (int)&WheelCmdI[2];
	WheelCmdI3ComboBox->Tag 	= (int)&WheelCmdI[3];
	WheelBtnF0ComboBox->Tag 	= (int)&WheelBtnCmdF[0];
	WheelBtnF1ComboBox->Tag 	= (int)&WheelBtnCmdF[1];
	WheelBtnF2ComboBox->Tag 	= (int)&WheelBtnCmdF[2];
	WheelBtnF3ComboBox->Tag 	= (int)&WheelBtnCmdF[3];
	WheelBtnVComboBox->Tag		= (int)&WheelBtnCmdV;
	WheelBtnIComboBox->Tag		= (int)&WheelBtnCmdI;
	X1BtnFComboBox->Tag 		= (int)&X1BtnCmdF;
	X2BtnFComboBox->Tag 		= (int)&X2BtnCmdF;
	X1BtnVComboBox->Tag 		= (int)&X1BtnCmdV;
	X2BtnVComboBox->Tag 		= (int)&X2BtnCmdV;
	X1BtnIComboBox->Tag 		= (int)&X1BtnCmdI;
	X2BtnIComboBox->Tag 		= (int)&X2BtnCmdI;

	//int (Style = csDropDownList)
	LayoutComboBox->Tag 		= (int)&LayoutMode;
	SizeFmtComboBox->Tag		= (int)&SizeFormatMode;
	TabStyleComboBox->Tag		= (int)&FlTabStyle;
	ScrBarStyleComboBox->Tag	= (int)&ScrBarStyle;
	BgImgModeComboBox->Tag		= (int)&BgImgMode;
	BgImgSubModeComboBox->Tag	= (int)&BgImgSubMode;

	ScaleOptComboBox->Tag		= (int)&WicScaleOpt;
	OpenByModeComboBox->Tag 	= (int)&OpenByMode;
	DblClickFlComboBox->Tag 	= (int)&DblClickFlMode;
	DefDropComboBox->Tag		= (int)&DefDropMode;
	SizeDecDgtComboBox->Tag 	= (int)&SizeDecDigits;

	IniModeIRadioGroup->Tag 	= (int)&InitialModeI;
	L_IniSortRadioGroup->Tag	= (int)&IniSortMode[0];
	R_IniSortRadioGroup->Tag	= (int)&IniSortMode[1];

	PrtDirRadioGroup->Tag		= (int)&ProtectDirMode;

	cursor_Default();

	//ドロップターゲットを設定
	usr_SH->AddTargetList(this, ToolExeEdit);
	usr_SH->AddTargetList(this, TextEditorEdit);
	usr_SH->AddTargetList(this, ImageEditorEdit);
	usr_SH->AddTargetList(this, BinaryEditorEdit);
	usr_SH->AddTargetList(this, EtcEditorEdit);
	usr_SH->AddTargetList(this, ExtTxViewerEdit);
	usr_SH->AddTargetList(this, AsoAppComboBox);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormShow(TObject *Sender)
{
	DlgInitialized = false;
	cursor_HourGlass();

	//INIファイルがデフォルトと異なる場合、ファイル名を表示
	UnicodeString inam = IniFile->FileName;
	UnicodeString tit = "オプション設定";
	if (!SameText(inam, ChangeFileExt(Application->ExeName, ".INI"))) tit.cat_sprintf(_T(" - %s"), to_relative_name(inam).c_str());
	Caption = tit;

	set_ListBoxItemHi(ExtColListBox);
	set_ListBoxItemHi(SpiListBox);
	set_ListBoxItemHi(EtcEditorListBox);
	set_ListBoxItemHi(AssociateListBox);
	set_ListBoxItemHi(ExtMenuListBox);
	set_ListBoxItemHi(ExtToolListBox);
	set_ListBoxItemHi(KeyListBox);
	set_ListBoxItemHi(EventListBox);
	set_ListBoxItemHi(PrtDirListBox);

	InitializeListHeader(ExtMenuHeader, _T("キャプション|エイリアス|設定"), Font);
	THeaderSections *sp = ExtMenuHeader->Sections;
	sp->Items[0]->Width = IniFile->ReadIntGen(_T("ExtMenuColWidth0"),	200);
	sp->Items[1]->Width = IniFile->ReadIntGen(_T("ExtMenuColWidth1"),	120);
	adjust_HeaderSecWidth(ExtMenuHeader, 2);

	InitializeListHeader(ExtToolHeader, _T("キャプション|エイリアス|設定"), Font);
	sp = ExtToolHeader->Sections;
	sp->Items[0]->Width = IniFile->ReadIntGen(_T("ExtToolColWidth0"),	200);
	sp->Items[1]->Width = IniFile->ReadIntGen(_T("ExtToolColWidth1"),	120);
	adjust_HeaderSecWidth(ExtToolHeader, 2);

	L_IniMaskComboBox->Clear();
	for (int i=0; i<PathMaskList->Count; i++) {
		TStringDynArray itm_buf = get_csv_array(PathMaskList->Strings[i], 3, true);
		if (itm_buf[2].IsEmpty() || USAME_TS(itm_buf[2], "*")) continue;
		UnicodeString lbuf;
		if (!itm_buf[1].IsEmpty()) lbuf.sprintf(_T(":%s:"), itm_buf[1].c_str());
		L_IniMaskComboBox->Items->Add(lbuf + itm_buf[2]);
	}
	R_IniMaskComboBox->Items->Assign(L_IniMaskComboBox->Items);

	FindEdit->Text = EmptyStr;

	HelpContext = PageControl1->ActivePage->HelpContext;

	//タグを用いてコントロールに値を設定
	BringOptionByTag(this);

	//タグを用いない設定
	TempDirEdit->Text	= TempPath;
	MigemoDirEdit->Text	= MigemoPath;
	SusieDirEdit->Text	= SpiDir;
	LimitTxtEdit->Text	= ViewTxtLimitSize/1024;
	LimitBinEdit->Text	= ViewBinLimitSize/1048576;
	LimitRarEdit->Text	= GetInfLimitSize/1048576;
	MaxTasksComboBox->ItemIndex = MaxTasks - 1;
	AppPrmComboBox->ItemIndex	= idx_of_word_i(_T("|FA|FL|FI|AO|LO|LI"), AppListHotPrm);

	ColBufList->Assign(ColorList);
	ModalColorBox->Selected = (TColor)get_ListIntVal(ColBufList, _T("ModalScr"), clBlack);

	AssociateListBox->Items->Assign(AssociateList);
	EtcEditorListBox->Items->Assign(EtcEditorList);
	ExtColListBox->Items->Assign(ExtColList);
	StdCmdListBox->Items->Assign(OpenStdCmdList);
	PrtDirListBox->Items->Assign(ProtectDirList);

	TCheckListBox *lp = ExtMenuListBox;
	lp->Items->Assign(ExtMenuList);
	for (int i=0; i<lp->Count; i++) lp->Checked[i] = !equal_0(get_csv_item(lp->Items->Strings[i], 4));

	lp = ExtToolListBox;
	lp->Items->Assign(ExtToolList);
	for (int i=0; i<lp->Count; i++) lp->Checked[i] = !equal_0(get_csv_item(lp->Items->Strings[i], 5));

	//キー一覧のヘッダを設定
	InitializeListHeader(KeyHeaderControl, _T("キー|コマンド|説明"), Font);
	TCanvas *cv = KeyListBox->Canvas;
	cv->Font->Assign(KeyListBox->Font);
	set_HeaderSecWidth(KeyHeaderControl, 2, cv->TextWidth("Shift+Ctrl+Alt+Space "), get_CharWidth(cv, 40));

	KeySortMode = IniFile->ReadIntGen(_T("OptKeySortMode"),	0);
	KeyFuncList->CustomSort(KeyComp_Key);

	for (int i=0; i<MAX_KEYTABS; i++) {
		KeyListBuf[i]->Clear();
		UnicodeString modstr = GetCmdModeStr(i + 1);
		for (int j=0; j<KeyFuncList->Count; j++){
			UnicodeString lbuf = KeyFuncList->Strings[j];
			if (StartsText(modstr, lbuf)) KeyListBuf[i]->Add(lbuf);
		}
	}
	FKeyLabelBuf->Assign(FKeyLabelList);

	PageControl1Change(NULL);

	(ViewFoldFitWin? FitFoldRadioBtn : SetFoldRadioBtn)->Checked = true;

	switch (IniPathMode[0]) {
	case 1:  L_IniPatMod1RadioBtn->Checked = true; break;
	case 2:  L_IniPatMod2RadioBtn->Checked = true; break;
	default: L_IniPatMod0RadioBtn->Checked = true;
	}

	switch (IniPathMode[1]) {
	case 1:  R_IniPatMod1RadioBtn->Checked = true; break;
	case 2:  R_IniPatMod2RadioBtn->Checked = true; break;
	default: R_IniPatMod0RadioBtn->Checked = true;
	}

	((IniWorkMode==1)? IniWork1RadioBtn : IniWork0RadioBtn)->Checked = true;
	((IniWinMode==1)?  IniWin1RadioBtn  : IniWin0RadioBtn)->Checked  = true;

	set_MigemoCheckBox(MigemoCheckBox, _T("OptKeyMigemo"));

	UnRegCmdCheckBox->Checked = IniFile->ReadBoolGen(_T("OptUnregCmd"));
	SetCmdCombo();

	AsoSortCheckBox->Checked  = IniFile->ReadBoolGen(_T("OptAsoSort"));

	DlgMoveShiftCheckBox->Checked = ContainsText(DlgMoveShift, KeyStr_Shift);
	DlgMoveCtrlCheckBox->Checked  = ContainsText(DlgMoveShift, KeyStr_Ctrl);
	DlgMoveAltCheckBox->Checked   = ContainsText(DlgMoveShift, KeyStr_Alt);
	DlgSizeShiftCheckBox->Checked = ContainsText(DlgSizeShift, KeyStr_Shift);
	DlgSizeCtrlCheckBox->Checked  = ContainsText(DlgSizeShift, KeyStr_Ctrl);
	DlgSizeAltCheckBox->Checked   = ContainsText(DlgSizeShift, KeyStr_Alt);

	UnicodeString kstr = CallHotKey;
	HotShiftCheckBox->Checked	  = remove_text(kstr, KeyStr_Shift);
	HotCtrlCheckBox->Checked	  = remove_text(kstr, KeyStr_Ctrl);
	HotAltCheckBox->Checked 	  = remove_text(kstr, KeyStr_Alt);
	HotWinCheckBox->Checked 	  = remove_text(kstr, _T("Win+"));
	HotKeyComboBox->ItemIndex	  = HotKeyComboBox->Items->IndexOf(kstr);

	kstr = AppListHotKey;
	AppShiftCheckBox->Checked	  = remove_text(kstr, KeyStr_Shift);
	AppCtrlCheckBox->Checked	  = remove_text(kstr, KeyStr_Ctrl);
	AppAltCheckBox->Checked 	  = remove_text(kstr, KeyStr_Alt);
	AppWinCheckBox->Checked 	  = remove_text(kstr, _T("Win+"));
	AppKeyComboBox->ItemIndex	  = AppKeyComboBox->Items->IndexOf(kstr);

	IniSeaShiftCheckBox->Checked  = ContainsText(IniSeaShift, KeyStr_Shift);
	IniSeaCtrlCheckBox->Checked   = ContainsText(IniSeaShift, KeyStr_Ctrl);
	IniSeaAltCheckBox->Checked	  = ContainsText(IniSeaShift, KeyStr_Alt);

	UpdateSpiListBox();

	//イベント
	EventListBox->Clear();
	for (int i=0; i<MAX_EVENT_CMD; i++) {
		EventListBox->Items->Add(
			UnicodeString().sprintf(_T("%s\t%s"), EventCmdList[i].dsc, EventCmdList[i].sp->c_str()));
	}
	EventCmdsEdit->Text = EmptyStr;

	LayoutChanged	= false;
	WinSizeChanged	= false;
	TlBarColChanged = false;

	KeyWd = EmptyStr;

	FExt7zDllEdit->Enabled = usr_ARC->Use7zDll;

	UpdateMaxItemWidth();

	cursor_Default();
	DlgInitialized = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (ScrMode==SCMD_FLIST) OptionPageIndex = PageControl1->ActivePageIndex;

	IconImgListP->Clear();
	UserModule->UninitializeListBox();

	THeaderSections *sp = ExtMenuHeader->Sections;
	IniFile->WriteIntGen(_T("ExtMenuColWidth0"),	sp->Items[0]->Width);
	IniFile->WriteIntGen(_T("ExtMenuColWidth1"),	sp->Items[1]->Width);
	sp = ExtToolHeader->Sections;
	IniFile->WriteIntGen(_T("ExtToolColWidth0"),	sp->Items[0]->Width);
	IniFile->WriteIntGen(_T("ExtToolColWidth1"),	sp->Items[1]->Width);

	IniFile->WriteIntGen(_T("OptKeySortMode"),		KeySortMode);
	IniFile->WriteBoolGen(_T("OptKeyMigemo"),		MigemoCheckBox);
	IniFile->WriteBoolGen(_T("OptUnregCmd"),		UnRegCmdCheckBox);
	IniFile->WriteBoolGen(_T("OptAsoSort"),			AsoSortCheckBox);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormDestroy(TObject *Sender)
{
	delete FindMarkList;
	delete ColBufList;

	for (int i=0; i<MAX_KEYTABS; i++) delete KeyListBuf[i];
	delete FKeyLabelBuf;
}

//---------------------------------------------------------------------------
//ロップ受け入れ
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::WmDropped(TMessage &msg)
{
	TWinControl *cp = (TWinControl *)msg.WParam;
	if (cp==ToolExeEdit)
		SetExtToolExeName(DroppedList->Strings[0]);
	else
		SetExtNameToCtrl(DroppedList->Strings[0], cp, cp==AsoAppComboBox);
}

//---------------------------------------------------------------------------
//現在の対象リストボックスを取得
//---------------------------------------------------------------------------
TCustomListBox* __fastcall TOptionDlg::GetCurListBox()
{
	if (PageControl1->ActivePage==ExtMenuSheet) return ExtMenuListBox;
	if (PageControl1->ActivePage==ExtToolSheet) return ExtToolListBox;
	if (PageControl1->ActivePage==FontColSheet) return ExtColListBox;
	if (PageControl1->ActivePage==EditorSheet)  return EtcEditorListBox;
	if (PageControl1->ActivePage==AssoSheet)    return AssociateListBox;
	if (PageControl1->ActivePage==CommadSheet)  return StdCmdListBox;
	if (PageControl1->ActivePage==NotifySheet)  return PrtDirListBox;
												return NULL;
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::UpdateMaxItemWidth()
{
	TCanvas *cv = AssociateListBox->Canvas;
	cv->Font->Assign(DialogFont);
	MaxWd_AssExt = 0;
	for (int i=0; i<AssociateListBox->Count; i++)
		MaxWd_AssExt = std::max(cv->TextWidth(AssociateListBox->Items->Names[i]), MaxWd_AssExt);
	MaxWd_AssExt += ScaledInt(20);

	cv = EventListBox->Canvas;
	cv->Font->Assign(DialogFont);
	MaxWd_Ev = 0;
	for (int i=0; i<MAX_EVENT_CMD; i++)
		MaxWd_Ev = std::max(cv->TextWidth(EventCmdList[i].dsc), MaxWd_Ev);
	MaxWd_Ev += ScaledInt(20);
}

//---------------------------------------------------------------------------
//キーリストの作成
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::make_KeyList(TStrings *lst)
{
	lst->Clear();

	UnicodeString itmbuf;
	for (int i=0; i<26; i++) lst->Add(itmbuf.sprintf(_T("%c"), 'A' + i));	//英字
	for (int i=0; i<10; i++) lst->Add(itmbuf.sprintf(_T("%c"), '0' + i));	//数字

	for (int i=0; i<12; i++) lst->Add(itmbuf.sprintf(_T("F%u"), i + 1));	//Fキー
	lst->AddStrings(
		SplitString("Del|Ins|BkSp|Left|Right|UP|Down|PgUp|PgDn|Home|End|Pause|Tab|Esc|Enter|Space|App", "|"));

	if (is_JpKeybd())
		lst->AddStrings(SplitString("-|^|\\|@|[|;|:|]|,|.|/|＼", "|"));		//JP
	else
		lst->AddStrings(SplitString("`|-|＝|[|]|\\|;|'|,|.|/", "|"));		//US

	for (int i=0; i<10; i++) lst->Add(itmbuf.sprintf(_T("10Key_%u"), i));	//10キー
	lst->AddStrings(SplitString("10Key_*|10Key_+|10Key_-|10Key_/|10Key_.", "|"));
}

//---------------------------------------------------------------------------
UnicodeString __fastcall TOptionDlg::GetCmdModeStr(int idx)
{
	if (idx==0) idx = KeyTabControl->TabIndex + 1;
	return (idx>=1 && idx<=MAX_KEYTABS)? ScrModeIdStr.SubString(idx, 1).UCAT_T(":") : EmptyStr;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TOptionDlg::GetCmdKeyStr()
{
	UnicodeString kstr = GetCmdModeStr();
	if (SelCheckBox->Checked)   kstr += KeyStr_SELECT;
	if (ShiftCheckBox->Checked) kstr += KeyStr_Shift;
	if (CtrlCheckBox->Checked)  kstr += KeyStr_Ctrl;
	if (AltCheckBox->Checked)   kstr += KeyStr_Alt;
	kstr += KeyComboBox->Text;
	if (!Key2ComboBox->Text.IsEmpty()) kstr.cat_sprintf(_T("~%s"), Key2ComboBox->Text.c_str());
	return kstr;
}

//---------------------------------------------------------------------------
//2ストローク開始キーか?
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::IsFirstCmdKey()
{
	bool res = false;

	UnicodeString key = get_tkn_r(GetCmdKeyStr(), ':');
	if (!contains_s(key, _T('~'))) {
		TStrings *sp = KeyListBox->Items;
		for (int i=0; i<sp->Count; i++) {
			UnicodeString fkey = sp->Names[i];
			if (contains_s(fkey, _T('~'))) {
				fkey = get_tkn_m(fkey, ':', '~');
				if (SameText(key, fkey)) { res = true; break; }
			}
		}
	}

	return res;
}

//---------------------------------------------------------------------------
//タブを設定
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SetSheet(UnicodeString prm)
{
	int idx = idx_of_word_i(_T("GN|G2|MO|DS|FC|TV|IV|ED|KY|AC|XM|XT|ST|NT|CM|EV"), prm);
	if (idx!=-1)
		PageControl1->ActivePageIndex = idx;
	else {
		if		(ScrMode==SCMD_TVIEW)	PageControl1->ActivePage = TxtViewerSheet;
		else if (ScrMode==SCMD_IVIEW)	PageControl1->ActivePage = ImgViewerSheet;
		else 							PageControl1->ActivePageIndex = OptionPageIndex;
	}
}

//---------------------------------------------------------------------------
//一時ディレクトリの参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefTmpDirBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("一時ディレクトリ"), dnam, true)) TempDirEdit->Text = dnam;
}

//---------------------------------------------------------------------------
//Migemoディレクトリの参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefMigemoBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("Migemo ディレクトリ"), dnam, true)) MigemoDirEdit->Text = dnam;
}

//---------------------------------------------------------------------------
//その他のエディタ一覧の描画
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EtcEditorListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);
	int xp = Rect.Left + 4;
	int yp = Rect.Top + get_TopMargin(cv);

	int w_x = 0;
	for (int i=0; i<lp->Count; i++) w_x = std::max(cv->TextWidth(lp->Items->Names[i]), w_x);

	UnicodeString etc_fext = lp->Items->Names[Index];
	UnicodeString etc_edtr = exclude_quot(lp->Items->ValueFromIndex[Index]);

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);
	cv->TextOut(xp, yp, etc_fext);
	cv->TextOut(xp + w_x + 20, yp, etc_edtr);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EtcEditorListBoxClick(TObject *Sender)
{
	TListBox *lp = (TListBox*)Sender;
	int idx = lp->ItemIndex;
	if (idx!=-1) {
		FExtEtcEdit->Text	= lp->Items->Names[idx];
		EtcEditorEdit->Text = exclude_quot(lp->Items->ValueFromIndex[idx]);
	}
}

//---------------------------------------------------------------------------
//エディタ/ビュアーの参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefEditorBtnClick(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;
	UnicodeString tit = get_word_i_idx(
		_T("テキストエディタ|イメージエディタ|バイナリエディタ|その他のエディタ|外部テキストビュアー"), tag);
	UserModule->PrepareOpenDlg(tit.c_str(), F_FILTER_EXE2);
	UnicodeString fnam;
	if (UserModule->OpenDlgToStr(fnam)) {
		switch (tag) {
		case 0: SetExtNameToCtrl(fnam, TextEditorEdit);		break;
		case 1: SetExtNameToCtrl(fnam, ImageEditorEdit);	break;
		case 2: SetExtNameToCtrl(fnam, BinaryEditorEdit);	break;
		case 3: SetExtNameToCtrl(fnam, EtcEditorEdit);		break;
		case 4: SetExtNameToCtrl(fnam, ExtTxViewerEdit);	break;
		}
	}
}

//---------------------------------------------------------------------------
//タスク終了時の通知音の参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefSoundBtnClick(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;

	UnicodeString inidir = def_if_empty(ExtractFileDir(rel_to_absdir((tag==0)? SndWarnEdit->Text : SndTaskFinEdit->Text)), ExePath);
	UserModule->PrepareOpenDlg(_T("サウンドファイルの指定"), F_FILTER_WAV, NULL, inidir);

	UnicodeString fnam;
	if (UserModule->OpenDlgToStr(fnam, true)) {
		switch (tag) {
		case 0: SndWarnEdit->Text    = fnam;	break;
		case 1: SndTaskFinEdit->Text = fnam;	break;
		case 2: SndFindFinEdit->Text = fnam;	break;
		case 3: SndWatchEdit->Text	 = fnam;	break;
		}
		play_sound(UserModule->OpenDlg->FileName);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::TestSoundBtnClick(TObject *Sender)
{
	switch (((TComponent*)Sender)->Tag) {
	case 0: play_sound(SndWarnEdit->Text);		break;
	case 1: play_sound(SndTaskFinEdit->Text);	break;
	case 2: play_sound(SndFindFinEdit->Text);	break;
	case 3: play_sound(SndWatchEdit->Text);		break;
	}
}

//---------------------------------------------------------------------------
//全体履歴ファイル設定の変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtSavDirHistCheckBoxClick(TObject *Sender)
{
	if (!DlgInitialized) return;

	if (ExtSavDirHistCheckBox->Checked)	{
		UnicodeString fnam = UAPP_T(ExePath, DIR_HIST_FILE);
		if (file_exists(fnam) && msgbox_Sure(_T("既存の履歴ファイルを読み込みますか？"))) {
			std::unique_ptr<UsrIniFile> hst_file(new UsrIniFile(fnam));
			hst_file->LoadListItems("AllDirHistory", AllDirHistory, 0, false);
		}
	}
}

//---------------------------------------------------------------------------
//レイアウト変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::LayoutChange(TObject *Sender)
{
	LayoutChanged = true;
}

//---------------------------------------------------------------------------
//背景画像の参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefBgImgBtnClick(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;
	TLabeledEdit *ep = (tag==0)? BgImg1Edit : (tag==1)? BgImg2Edit : (tag==2)? SpImgEdit : SpImg2Edit;
	UserModule->OpenImgDlg->Title = "背景画像の指定";
	UserModule->SetOpenImgFilter(ep->Text);
	UserModule->OpenImgDlg->InitialDir = rel_to_absdir(ExtractFilePath(ep->Text));
	UserModule->OpenImgDlg->FileName   = EmptyStr;
	UserModule->OpenImgDlgToEdit(ep, true);
}
//---------------------------------------------------------------------------
//シンプルスクロールノブ画像の参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ScrBarStyleComboBoxClick(TObject *Sender)
{
	if (ScrBarStyleComboBox->ItemIndex==4) {
		UserModule->OpenImgDlg->Title = "ノブ画像の指定";
		UserModule->SetOpenImgFilter(KnobImgEdit->Text);
		UserModule->OpenImgDlg->InitialDir = rel_to_absdir(ExtractFilePath(KnobImgEdit->Text));
		UserModule->OpenImgDlg->FileName   = EmptyStr;
		if (!UserModule->OpenImgDlgToEdit(KnobImgEdit, true)) {
			ScrBarStyleComboBox->ItemIndex = 0;
			KnobImgEdit->Text = EmptyStr;
		}
	}
	else
		KnobImgEdit->Text = EmptyStr;
}

//---------------------------------------------------------------------------
//フォント
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FontComboBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TComboBox *cp = (TComboBox*)Control;
	TCanvas   *cv = cp->Canvas;
	cv->Font->Assign(cp->Font);
	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);
	int fh = abs(cv->Font->Height);

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);
	cv->TextOut(xp, yp, cp->Items->Strings[Index]);
	xp += fh*10;

	if (Index<FontList->Count) {
		TFont *f = (TFont*)FontList->Objects[Index];
		//サイズ
		UnicodeString szstr = f->Size;
		cv->TextOut(xp + get_CharWidth(cv, 2) - cv->TextWidth(szstr), yp, szstr);
		xp += fh*2;
		//名前
		cv->Font->Assign(f);
		cv->Font->Size = cp->Font->Size;
		SetHighlight(cv, State.Contains(odSelected));
		yp = Rect.Top + get_TopMargin(cv);
		cv->TextOut(xp, yp, f->Name);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefFontBtnClick(TObject *Sender)
{
	int idx = FontComboBox->ItemIndex;
	if (idx>=0 && idx<FontComboBox->Items->Count && idx<FontList->Count) {
		TFont *f = (TFont*)FontList->Objects[idx];
		UserModule->FontDlg->Options.Clear();
		if (USAME_TI(FontList->ValueFromIndex[idx], "テキストビュアー"))
			UserModule->FontDlg->Options << fdFixedPitchOnly;	//テキストビュアーは等幅

		if (UserModule->FontDlgToFont(f)) {
			//ダイアログ
			if (USAME_TI(FontList->ValueFromIndex[idx], "ダイアログ")) {
				if (f->Size>10) f->Size = 10;
				Application->DefaultFont->Assign(f);
				UpdateMaxItemWidth();
			}
			//その他
			else FontComboBox->Repaint();
		}
	}
}

//---------------------------------------------------------------------------
//配色
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OptColListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	draw_ColorListBox((TListBox*)Control, Rect, Index, State, ColBufList);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefColBtnClick(TObject *Sender)
{
	TListBox *lp = (((TButton*)Sender)->Tag==1)? TimColListBox : OptColListBox;
	int idx = lp->ItemIndex;	if (idx==-1) return;

	UnicodeString col_nam = lp->Items->Names[idx];
	UserModule->ColorDlg->Color = (TColor)ColBufList->Values[col_nam].ToIntDef(clBlack);
	if (UserModule->ColorDlg->Execute()) {
		ColBufList->Values[col_nam] = IntToStr(UserModule->ColorDlg->Color);
		lp->Repaint();
		if (contained_wd_i(_T("bgTlBar1|bgTlBar2"), col_nam)) TlBarColChanged = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ModalColorBoxChange(TObject *Sender)
{
	ColBufList->Values["ModalScr"] = IntToStr(ModalColorBox->Selected);
}

//---------------------------------------------------------------------------
//配色の無効化
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DisableColActionExecute(TObject *Sender)
{
	TListBox *lp = OptColListBox;
	if (lp->ItemIndex!=-1) {
		ColBufList->Values[lp->Items->Names[lp->ItemIndex]] = IntToStr(clNone);
		lp->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DisableColActionUpdate(TObject *Sender)
{
	TListBox *lp = OptColListBox;
	UnicodeString col_id = (lp->ItemIndex!=-1)? lp->Items->Names[lp->ItemIndex] : EmptyStr;

	((TAction*)Sender)->Enabled
		= contained_wd_i(_T("bgList2|fgSelItem|Protect|frScrKnob|frmTab|bdrLine|bdrFold|bdrFixed"), col_id);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DisableCol2ActionExecute(TObject *Sender)
{
	TListBox *lp = TimColListBox;
	if (lp->ItemIndex!=-1) {
		ColBufList->Values[lp->Items->Names[lp->ItemIndex]] = IntToStr(clNone);
		lp->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DisableCol2ActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = TimColListBox->ItemIndex!=-1;
}

//---------------------------------------------------------------------------
//スポイト
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SpuitImageMouseDown(TObject *Sender, TMouseButton Button,
		TShiftState Shift, int X, int Y)
{
	if (Button!=mbLeft) return;

	TImage *ip = (TImage*)Sender;
	SpuitTag = ip->Tag;
	if (SpuitTag!=2) {
		TListBox *cp = (SpuitTag==1)? TimColListBox : OptColListBox;
		if (cp->ItemIndex==-1) return;
	}

	UserModule->BeginSpuit((SpuitTag==2)? SpuitPanel3 : (SpuitTag==1)? SpuitPanel2 : SpuitPanel, ip);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SpuitImageMouseUp(TObject *Sender, TMouseButton Button,
		TShiftState Shift, int X, int Y)
{
	if (UserModule->SpuitEnabled()) {
		TColor col = UserModule->EndSpuit();
		if (SpuitTag==2)
			RefExtColPanel->Color = col;
		else {
			TListBox *lp = (SpuitTag==1)? TimColListBox : OptColListBox;
			if (lp->ItemIndex!=-1) {
				UnicodeString col_nam = lp->Items->Names[lp->ItemIndex];
				ColBufList->Values[col_nam] = IntToStr(col);
				lp->Repaint();
				if (contained_wd_i(_T("bgTlBar1|bgTlBar2"), col_nam)) TlBarColChanged = true;
			}
		}
	}
}
//---------------------------------------------------------------------------
//透明度制限
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ModalAlphaEditChange(TObject *Sender)
{
	TLabeledEdit *ep = (TLabeledEdit*)Sender;
	if (EditToInt(ep, 0)>255) ep->Text = 255;
}

//---------------------------------------------------------------------------
//拡張子別配色
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtColListBoxClick(TObject *Sender)
{
	int idx = ExtColListBox->ItemIndex;  if (idx==-1) return;
	UnicodeString lbuf = ExtColListBox->Items->Strings[idx];
	RefExtColPanel->Color = (TColor)split_tkn(lbuf, ',').ToIntDef(clNone);
	ExtColorEdit->Text	  = lbuf;
}

//---------------------------------------------------------------------------
//拡張子別配色一覧の描画
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtColListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);
	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);

	UnicodeString lbuf = lp->Items->Strings[Index];
	cv->Font->Color = (TColor)split_tkn(lbuf, ',').ToIntDef(clNone);
	bool is_dot = USAME_TS(lbuf, ".");
	UnicodeString ext = REPLACE_TS(lbuf, ".", " .");
	cv->Brush->Color = col_bgList;

	UnicodeString smpl_str;
	smpl_str.sprintf(_T("%s"), is_dot? _T("hogehoge") : _T("hoge.???"));
	int smpl_wd = cv->TextWidth(smpl_str) + 10;
	TRect rc = Rect;  rc.Right = rc.Left + smpl_wd;
	cv->FillRect(rc);
	cv->TextOut(xp + 4, yp, smpl_str);

	SetHighlight(cv, State.Contains(odSelected));
	rc = Rect;  rc.Left += smpl_wd;  xp += smpl_wd;
	cv->FillRect(rc);
	cv->TextOut(xp + 4, yp, ext);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefExtColBtnClick(TObject *Sender)
{
	UserModule->ColorDlg->Color = RefExtColPanel->Color;
	if (UserModule->ColorDlg->Execute()) RefExtColPanel->Color = UserModule->ColorDlg->Color;
}
//---------------------------------------------------------------------------
//拡張子別色の追加
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddExtColActionExecute(TObject *Sender)
{
	UnicodeString s;
	s.sprintf(_T("0x%06x,%s"), (int)RefExtColPanel->Color, ExtColorEdit->Text.c_str());
	ExtColListBox->Items->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddExtColActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !ExtColorEdit->Text.IsEmpty();
}
//---------------------------------------------------------------------------
//拡張子別色の変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgExtColActionExecute(TObject *Sender)
{
	int idx = ExtColListBox->ItemIndex;  if (idx==-1) return;
	UnicodeString s;
	s.sprintf(_T("0x%06x,%s"), (int)RefExtColPanel->Color, ExtColorEdit->Text.c_str());
	ExtColListBox->Items->Strings[idx] = s;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgExtColActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !ExtColorEdit->Text.IsEmpty() && ExtColListBox->ItemIndex!=-1;
}

//---------------------------------------------------------------------------
//リストでのキー操作
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OptListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	TControl *cp = (TControl*)Sender;
	UnicodeString KeyStr = get_KeyStr(Key, Shift);

	if (cp==KeyListBox) {
		if (equal_DEL(KeyStr)) DelKeyAction->Execute(); else return;
	}
	else if (cp==PrtDirListBox) {
		if (equal_DEL(KeyStr)) UserModule->DelListItemAction->Execute(); else return;
	}
	else {
		if (equal_DEL(KeyStr)) {
			if (cp==EventListBox) {
				TListBox *lp = (TListBox*)cp;
				int idx = lp->ItemIndex;
				if (idx!=-1) lp->Items->Strings[idx] = get_pre_tab(lp->Items->Strings[idx]).UCAT_T("\t");
			}
			else UserModule->DelListItemAction->Execute();
		}
		else if (!UserModule->ListBoxOpeItem(KeyStr)) return;
	}

	Key = 0;
}

//---------------------------------------------------------------------------
//Susie プラグインのディレクトリを参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefSusieDirBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("Susie プラグイン"), dnam, true)) {
		SusieDirEdit->Text = dnam;
		delete SPI;
		SPI = new SpiUnit(rel_to_absdir(SusieDirEdit->Text));
		UpdateSpiListBox();
	}
}
//---------------------------------------------------------------------------
//Suisie プラグイン一覧を更新
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::UpdateSpiListBox()
{
	SpiListBox->Clear();
	for (int i=0; i<SPI->PlgList->Count; i++) {
		spi_info *sp = SPI->PlgList->Items[i];
		SpiListBox->Items->Add(
			UnicodeString().sprintf(_T("%s \t[%s] %s"),
				ExtractFileName(sp->FileName).c_str(), sp->VerType.c_str(), sp->FileType.c_str()));
	}
}
//---------------------------------------------------------------------------
//Sunis プラグインの設定
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ConfigSpiActionExecute(TObject *Sender)
{
	int idx = SpiListBox->ItemIndex;
	if (idx!=-1) {
		spi_info *sp = SPI->PlgList->Items[idx];
		if (sp->ConfigurationDlg) sp->ConfigurationDlg(this->Handle, 1);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ConfigSpiActionUpdate(TObject *Sender)
{
	int idx = SpiListBox->ItemIndex;
	if (idx!=-1) {
		spi_info *sp = SPI->PlgList->Items[idx];
		((TAction*)Sender)->Enabled = (sp->ConfigurationDlg!=NULL);
	}
	else
		((TAction*)Sender)->Enabled = false;
}

//---------------------------------------------------------------------------
//その他のエディタの追加
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddEtcEditorActionExecute(TObject *Sender)
{
	EtcEditorListBox->Items->Add(
		UnicodeString().cat_sprintf(_T("%s=\"%s\""), FExtEtcEdit->Text.LowerCase().c_str(), EtcEditorEdit->Text.c_str()));
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddEtcEditorActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !FExtEtcEdit->Text.IsEmpty() && !EtcEditorEdit->Text.IsEmpty();
}
//---------------------------------------------------------------------------
//その他のエディタの変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgEtcEditorActionExecute(TObject *Sender)
{
	int idx = EtcEditorListBox->ItemIndex;  if (idx==-1) return;
	EtcEditorListBox->Items->Strings[idx] =
		UnicodeString().cat_sprintf(_T("%s=\"%s\""), FExtEtcEdit->Text.LowerCase().c_str(), EtcEditorEdit->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgEtcEditorActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !FExtEtcEdit->Text.IsEmpty() && !EtcEditorEdit->Text.IsEmpty();
}

//---------------------------------------------------------------------------
//エイリアス変更を反映
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::UpdateAlias(UnicodeString o_name, UnicodeString n_name)
{
	if (SameText(o_name, n_name)) return;

	UnicodeString tmp;
	//キー設定
	for (int i=0; i<MAX_KEYTABS; i++) {
		for (int j=0; j<KeyListBuf[i]->Count; j++) {
			UnicodeString vbuf = KeyListBuf[i]->ValueFromIndex[j];
			if (!remove_top_Dollar(vbuf)) continue;
			if (SameText(o_name, vbuf))
				KeyListBuf[i]->ValueFromIndex[j] = tmp.sprintf(_T("$%s"), n_name.c_str());
		}
	}
	//関連付け
	TListBox *lp = AssociateListBox;
	for (int i=0; i<lp->Count; i++) {
		UnicodeString vbuf = exclude_quot(lp->Items->ValueFromIndex[i]);
		if (!remove_top_Dollar(vbuf)) continue;
		if (SameText(o_name, vbuf))
			lp->Items->ValueFromIndex[i] = tmp.sprintf(_T("\"$%s\""), n_name.c_str());
	}
}
//---------------------------------------------------------------------------
//エイリアスに関連付けられている拡張子を取得
//---------------------------------------------------------------------------
UnicodeString __fastcall TOptionDlg::get_AliasInfo(UnicodeString alias)
{
	//キー
	UnicodeString k_inf;
	for (int i=0; i<MAX_KEYTABS && k_inf.IsEmpty(); i++) k_inf = get_Alias_KeyStr(alias, KeyListBuf[i]);

	//関連付け
	UnicodeString a_inf;
	TListBox *lp = AssociateListBox;
	for (int i=0; i<lp->Count; i++) {
		UnicodeString fext = lp->Items->Names[i];
		UnicodeString vbuf = exclude_quot(lp->Items->ValueFromIndex[i]);
		if (!remove_top_Dollar(vbuf)) continue;
		if (!SameText(alias, vbuf))   continue;
		a_inf.cat_sprintf(_T(".%s"), fext.c_str());
	}

	UnicodeString ret_str = k_inf;
	if (!a_inf.IsEmpty()) {
		if (!ret_str.IsEmpty()) ret_str.UCAT_T(" / ");
		ret_str += a_inf;
	}
	return ret_str;
}
//---------------------------------------------------------------------------
//エイリアスの重複メッセージを取得
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::check_DuplAliasMsg(
	UnicodeString alias,	//エイリアス
	TCheckListBox *lp)		//変更対象リストボックス (追加の場合は NULL)
{
	if (alias.IsEmpty()) return false;

	UnicodeString mgs;
	mgs.sprintf(_T("エイリアス \"%s\" が\r\n"), alias.c_str());

	for (int i=0; i<ExtMenuListBox->Count; i++) {
		if (lp==ExtMenuListBox && i==lp->ItemIndex) continue;
		TStringDynArray itm_buf = get_csv_array(ExtMenuListBox->Items->Strings[i], EXTMENU_CSVITMCNT, true);
		if (SameText(alias, itm_buf[3])) {
			msgbox_WARN(mgs.cat_sprintf(_T("追加メニュー \"%s\" で重複しています。"), itm_buf[0].c_str()));
			return true;
		}
	}
	for (int i=0; i<ExtToolListBox->Count; i++) {
		if (lp==ExtToolListBox && i==lp->ItemIndex) continue;
		TStringDynArray itm_buf = get_csv_array(ExtToolListBox->Items->Strings[i], EXTTOOL_CSVITMCNT, true);
		if (SameText(alias, itm_buf[4])) {
			msgbox_WARN(mgs.cat_sprintf(_T("外部ツール \"%s\" で重複しています。"), itm_buf[0].c_str()));
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//外部ツール
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtToolListBoxClick(TObject *Sender)
{
	TCheckListBox *lp = ExtToolListBox;
	if (lp->Count==0) return;
	if (!Sender) lp->ItemIndex = -1;

	if (lp->ItemIndex!=-1) {
		TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[lp->ItemIndex], EXTTOOL_CSVITMCNT, true);
		ToolTitEdit->Text	= itm_buf[0];
		ToolExeEdit->Text	= itm_buf[1];
		ToolPrmEdit->Text	= itm_buf[2];
		ToolDirEdit->Text	= itm_buf[3];
		ToolAliasEdit->Text = itm_buf[4];
		ToolAliasLabel->Caption = get_AliasInfo(itm_buf[4]);
	}
	else {
		ToolTitEdit->Text	= EmptyStr;
		ToolExeEdit->Text	= EmptyStr;
		ToolPrmEdit->Text	= EmptyStr;
		ToolDirEdit->Text	= EmptyStr;
		ToolAliasEdit->Text = EmptyStr;
		ToolAliasLabel->Caption = EmptyStr;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ToolTitEditChange(TObject *Sender)
{
	if (starts_tchs(_T("-<>"), ToolTitEdit->Text)) {
		ToolExeEdit->Text = EmptyStr;
		ToolPrmEdit->Text = EmptyStr;
		ToolDirEdit->Text = EmptyStr;
		if (starts_tchs(_T("-<"), ToolTitEdit->Text)) {
			ToolTitEdit->Text	= ToolTitEdit->Text.SubString(1, 1);
			ToolAliasEdit->Text = EmptyStr;
		}
	}
}

//---------------------------------------------------------------------------
//外部ツールの実行ファイルを設定
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SetExtToolExeName(UnicodeString fnam)
{
	if (test_LnkExt(get_extension(fnam))) {
		UnicodeString xnam, prm, fld;
		int shw;
		usr_SH->get_LnkInf(fnam, NULL, &xnam, &prm, &fld, &shw);
		if (test_FileExt(get_extension(xnam), ".exe")) {
			ToolTitEdit->Text = get_base_name(fnam);
			ToolExeEdit->Text = xnam;
			ToolPrmEdit->Text = prm;
			ToolDirEdit->Text = fld;
			ToolAliasEdit->Text = EmptyStr;
		}
		else
			msgbox_WARN("リンク先が .exe ファイルではありません");
	}
	else
		ToolExeEdit->Text = fnam;
}
//---------------------------------------------------------------------------
//外部ツールの追加・変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeExtToolList(bool add)
{
	TCheckListBox *lp = ExtToolListBox;
	if (check_DuplAliasMsg(ToolAliasEdit->Text, add? NULL : lp)) return;

	int  idx = lp->ItemIndex;
	bool chk = add? true : (idx!=-1)? lp->Checked[idx] : false;

	UnicodeString lbuf;
	lbuf.sprintf(_T("%s,%s,%s,%s,%s,%s"),
		make_csv_str(ToolTitEdit->Text).c_str(),	make_csv_str(ToolExeEdit->Text).c_str(),
		make_csv_str(ToolPrmEdit->Text).c_str(),	make_csv_str(ToolDirEdit->Text).c_str(),
		make_csv_str(ToolAliasEdit->Text).c_str(),	make_csv_str(chk).c_str());

	//追加
	if (add) {
		lp->Items->Add(lbuf);
		lp->Checked[lp->Count - 1] = true;
	}
	//変更
	else {
		if (idx==-1) return;
		UnicodeString last_alias = get_csv_item(lp->Items->Strings[idx], 4);
		lp->Items->Strings[idx] = lbuf;
		//エイリアス変更を反映
		UpdateAlias(last_alias, ToolAliasEdit->Text);
	}
}
//---------------------------------------------------------------------------
//追加
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddToolActionExecute(TObject *Sender)
{
	ChangeExtToolList(true);
}
//---------------------------------------------------------------------------
//変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgToolActionExecute(TObject *Sender)
{
	ChangeExtToolList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgToolActionUpdate(TObject *Sender)
{
	bool is_special = starts_tchs(_T("-<>"), ToolTitEdit->Text);
	((TAction*)Sender)->Enabled = is_special || (!ToolTitEdit->Text.IsEmpty() && !ToolExeEdit->Text.IsEmpty());
	ToolAliasEdit->Enabled = !starts_tchs(_T("-<"), ToolTitEdit->Text);
	ToolExeEdit->Enabled   = !is_special;
	ToolPrmEdit->Enabled   = !is_special;
	ToolDirEdit->Enabled   = !is_special;
}

//---------------------------------------------------------------------------
//外部ツールの参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefExeBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("外部ツール"), F_FILTER_EXE2);
	UnicodeString fnam;
	if (UserModule->OpenDlgToStr(fnam)) SetExtToolExeName(fnam);
}
//---------------------------------------------------------------------------
//外部ツールパラメータ書式文字列の参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefToolFmtBtnClick(TObject *Sender)
{
	TPoint p = ExtToolPanel->ClientToScreen(Point(RefToolFmtBtn->Left + 8, RefToolFmtBtn->Top + 8));
	ToolPopupMenu->Popup(p.x, p.y);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FrmtItemClick(TObject *Sender)
{
	ToolPrmEdit->SelText = get_tkn(((TMenuItem*)Sender)->Caption, ' ');
}
//---------------------------------------------------------------------------
//外部ツールの作業ディレクトリの参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefDirBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("作業ディレクトリ"), dnam)) ToolDirEdit->Text = dnam;
}

//---------------------------------------------------------------------------
//追加メニュー
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtMenuListBoxClick(TObject *Sender)
{
	TCheckListBox *lp = ExtMenuListBox;
	if (lp->Count==0) return;
	if (!Sender) lp->ItemIndex = -1;

	if (lp->ItemIndex!=-1) {
		TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[lp->ItemIndex], EXTMENU_CSVITMCNT, true);
		MenuTitEdit->Text = itm_buf[0];
		if (starts_tchs(_T("-<"), MenuTitEdit->Text)) {
			MenuAliasEdit->Text 	   = EmptyStr;
			MenuAliasLabel->Caption    = EmptyStr;
			MenuCmdComboBox->ItemIndex = -1;
			MenuPrmEdit->Text		   = EmptyStr;
			MenuIconEdit->Text		   = EmptyStr;
		}
		else {
			TComboBox *cp = MenuCmdComboBox;
			for (int i=0; i<cp->Items->Count; i++) {
				if (StartsText(UAPP_T(itm_buf[1], " "), cp->Items->Strings[i])) {
					cp->ItemIndex = i;  break;
				}
			}
			MenuPrmEdit->Text		= itm_buf[2];
			MenuAliasEdit->Text 	= itm_buf[3];
			MenuAliasLabel->Caption = get_AliasInfo(itm_buf[3]);
			MenuIconEdit->Text		= itm_buf[5];
		}
	}
	else {
		MenuTitEdit->Text		   = EmptyStr;
		MenuAliasEdit->Text 	   = EmptyStr;
		MenuAliasLabel->Caption    = EmptyStr;
		MenuCmdComboBox->ItemIndex = -1;
		MenuPrmEdit->Text		   = EmptyStr;
		MenuIconEdit->Text		   = EmptyStr;
	}

	RefMenuPrmBtn->Enabled = USAME_TI(get_tkn(MenuCmdComboBox->Text, ' '), "ExeCommands");
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::MenuCmdComboBoxChange(TObject *Sender)
{
	RefMenuPrmBtn->Enabled = USAME_TI(get_tkn(MenuCmdComboBox->Text, ' '), "ExeCommands");
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::MenuTitEditChange(TObject *Sender)
{
	if (starts_tchs(_T("-<>"), MenuTitEdit->Text)) {
		MenuCmdComboBox->ItemIndex = -1;
		MenuPrmEdit->Text	= EmptyStr;
		if (starts_tchs(_T("-<"), MenuTitEdit->Text)) {
			MenuTitEdit->Text	= MenuTitEdit->Text.SubString(1, 1);
			MenuAliasEdit->Text = EmptyStr;
		}
	}
}

//---------------------------------------------------------------------------
//メニュー項目の追加・変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeExtMenuList(bool add)
{
	TCheckListBox *lp = ExtMenuListBox;
	if (check_DuplAliasMsg(MenuAliasEdit->Text, add? NULL : lp)) return;

	int  idx = lp->ItemIndex;
	bool chk = add? true : (idx!=-1)? lp->Checked[idx] : false;

	UnicodeString lbuf;
	lbuf.sprintf(_T("%s,%s,%s,%s,%s,%s"),
		make_csv_str(MenuTitEdit->Text).c_str(), make_csv_str(get_tkn(MenuCmdComboBox->Text, ' ')).c_str(),
		make_csv_str(MenuPrmEdit->Text).c_str(), make_csv_str(MenuAliasEdit->Text).c_str(),
		make_csv_str(chk).c_str(), make_csv_str(MenuIconEdit->Text).c_str());

	if (add) {
		lp->Items->Add(lbuf);
		lp->Checked[lp->Count - 1] = true;
	}
	else {
		if (idx==-1) return;
		UnicodeString last_alias = get_csv_item(lp->Items->Strings[idx], 3);
		lp->Items->Strings[idx] = lbuf;
		//エイリアス変更を反映
		UpdateAlias(last_alias, MenuAliasEdit->Text);
	}
}
//---------------------------------------------------------------------------
//追加
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddMenuActionExecute(TObject *Sender)
{
	ChangeExtMenuList(true);
}
//---------------------------------------------------------------------------
//変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgMenuActionExecute(TObject *Sender)
{
	ChangeExtMenuList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgMenuActionUpdate(TObject *Sender)
{
	bool is_special = starts_tchs(_T("-<>"), MenuTitEdit->Text);
	((TAction*)Sender)->Enabled = is_special ||
		(!MenuTitEdit->Text.IsEmpty() && !MenuCmdComboBox->Text.IsEmpty() && !MenuPrmEdit->Text.IsEmpty());
	MenuAliasEdit->Enabled   = !starts_tchs(_T("-<"), MenuTitEdit->Text);
	MenuCmdComboBox->Enabled = !is_special;
	MenuPrmEdit->Enabled	 = !is_special;
}

//---------------------------------------------------------------------------
//パラメータファイルの参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefCmdsBtnClick(TObject *Sender)
{
	UnicodeString subtit;
	if (PageControl1->ActivePage==ExtMenuSheet) {
		if (!MenuTitEdit->Text.IsEmpty()) subtit.sprintf(_T(" - [%s]"), MenuTitEdit->Text.c_str());
	}
	else if (PageControl1->ActivePage==EventSheet) {
		int idx = EventListBox->ItemIndex;
		if (idx!=-1) subtit.sprintf(_T(" - [%s]"), get_pre_tab(EventListBox->Items->Strings[idx]).c_str());
	}

	UnicodeString cmd;
	if		(PageControl1->ActivePage==ExtMenuSheet) cmd = get_CmdStr(MenuPrmEdit->Text);
	else if (PageControl1->ActivePage==EventSheet)	 cmd = get_CmdStr(EventCmdsEdit->Text);

	//通常ファイルの選択
	if (contained_wd_i(_T("ContextMenu|ExeMenuFile|FileEdit|LoadWorkList|OpenByApp|OpenByWin|TextViewer"), cmd)) {
		UnicodeString tit = "パラメータの指定" + subtit;
		UserModule->PrepareOpenDlg(tit.c_str(), F_FILTER_ALL, NULL, RefParamPath);
		UnicodeString fnam;
		if (UserModule->OpenDlgToStr(fnam)) {
			RefParamPath = ExtractFilePath(fnam);
			if (contained_wd_i(_T("ContextMenu|ExeMenuFile|LoadWorkList"), cmd))
				fnam = to_relative_name(fnam);
			else if (remove_top_text(fnam, ExePath))
				fnam.Insert("%ExePath%", 1);
			cmd.cat_sprintf(_T("_\"%s\""), fnam.c_str());

			if (PageControl1->ActivePage==ExtMenuSheet)
				MenuPrmEdit->Text = cmd;
			else if (PageControl1->ActivePage==EventSheet) {
				EventCmdsEdit->Text = cmd;
				int idx = EventListBox->ItemIndex;
				if (idx!=-1)
					EventListBox->Items->Strings[idx] = get_pre_tab(EventListBox->Items->Strings[idx]) + "\t" + cmd;
			}
		}
	}
	//コマンドファイルの選択
	else if (CmdFileListDlg->ShowToSelect()==mrOk) {
		UnicodeString fnam = CmdFileListDlg->CmdFileName;
		fnam = "@" + to_relative_name(fnam);
		if (PageControl1->ActivePage==ExtMenuSheet)
			MenuPrmEdit->Text = fnam;
		else if (PageControl1->ActivePage==EventSheet) {
			switch (((TComponent*)Sender)->Tag) {
			case 100:
				EventCmdsEdit->Text = fnam;
				if (EventListBox->ItemIndex!=-1)
					EventListBox->Items->Strings[EventListBox->ItemIndex]
						 = get_pre_tab(EventListBox->Items->Strings[EventListBox->ItemIndex]) + "\t" + fnam;
				break;
			case 101: TimerCmd1Edit->Text = fnam;	break;
			case 102: TimerCmd2Edit->Text = fnam;	break;
			}
		}
	}
}
//---------------------------------------------------------------------------
//アイコンの参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefMenuIconBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("アイコンの指定"), F_FILTER_ICO, NULL, IconFilePath);
	if (UserModule->OpenDlgToEdit(MenuIconEdit, true)) IconFilePath = ExtractFilePath(rel_to_absdir(MenuIconEdit->Text));
}

//---------------------------------------------------------------------------
//関連付け
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AssociateListBoxClick(TObject *Sender)
{
	TListBox *lp = AssociateListBox;
	if (!Sender) lp->ItemIndex = -1;

	if (lp->ItemIndex!=-1) {
		AsoExtEdit->Text	 = AssociateListBox->Items->Names[lp->ItemIndex];
		AsoAppComboBox->Text = exclude_quot(AssociateListBox->Items->ValueFromIndex[lp->ItemIndex]);
	}
	else {
		AsoExtEdit->Text	 = EmptyStr;
		AsoAppComboBox->Text = EmptyStr;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AssociateListBoxMouseUp(TObject *Sender, TMouseButton Button,
		TShiftState Shift, int X, int Y)
{
	if (Button!=mbRight) return;
	TListBox *lp = AssociateListBox;
	int idx = lp->ItemIndex;  if (idx==-1) return;

	TStringDynArray app_lst;
	UnicodeString fext = lp->Items->Names[idx];
	for (int i=0; i<lp->Count; i++) {
		if (test_FileExt(fext, lp->Items->Names[i]))
			add_dyn_array(app_lst, exclude_quot(lp->Items->ValueFromIndex[i]));
	}

	if (app_lst.Length>1) {
		std::unique_ptr<TStringList> m_lst(new TStringList());
		make_AssoMenuList(app_lst, m_lst.get());
		//テストメニューを作成
		TestPopupMenu->Items->Clear();
		TestPopupMenu->AutoHotkeys = AutoHotkeyCheckBox->Checked? maAutomatic : maManual;
		IconImgListP->Clear();
		for (int i=0; i<m_lst->Count; i++) {
			TStringDynArray m_buf = split_strings_tab(Trim(m_lst->Strings[i]));
			if (m_buf.Length>0) {
				TMenuItem *mp  = new TMenuItem(TestPopupMenu);
				mp->Caption    = m_buf[0];
				mp->ImageIndex = (m_buf.Length==3)? add_IconImage(m_buf[2], IconImgListP) : -1;
				TestPopupMenu->Items->Add(mp);
			}
		}
		//テストメニューを表示
		TestPopupMenu->Popup(Mouse->CursorPos.x, Mouse->CursorPos.y);
	}
}

//---------------------------------------------------------------------------
//関連付け一覧のソート (同じ拡張子内はそのまま)
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SortAssoListBox(UnicodeString item)
{
	TListBox *lp = AssociateListBox;
	if (lp->Count<2) return;

	std::unique_ptr<TStringList> x_lst(new TStringList());
	for (int i=0; i<lp->Count; i++) {
		UnicodeString lbuf = lp->Items->Strings[i];
		UnicodeString fext = lp->Items->Names[i];
		TStringList *o_lst;
		int idx = x_lst->IndexOf(fext);
		if (idx==-1) {
			o_lst = new TStringList();
			x_lst->AddObject(fext, (TObject*)o_lst);
		}
		else
			o_lst = (TStringList*)x_lst->Objects[idx];
		o_lst->Add(lbuf);
	}
	x_lst->Sort();

	lp->Clear();
	for (int i=0; i<x_lst->Count; i++) {
		TStringList *o_lst = (TStringList*)x_lst->Objects[i];
		for (int j=0; j<o_lst->Count; j++) lp->Items->Add(o_lst->Strings[j]);
		delete o_lst;
	}

	if (!item.IsEmpty()) {
		int idx = lp->Items->IndexOf(item);
		if (idx!=-1) lp->ItemIndex = idx;
	}
}

//---------------------------------------------------------------------------
//関連付け項目の追加・変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeAssociateList(bool add)
{
	if (AsoExtEdit->Text.IsEmpty() || AsoAppComboBox->Text.IsEmpty()) return;

	UnicodeString s = AsoExtEdit->Text.LowerCase();
	remove_top_s(s, '.');
	s.cat_sprintf(_T("=\"%s\""), AsoAppComboBox->Text.c_str());

	//追加
	if (add) {
		AssociateListBox->Items->Add(s);
		if (AsoSortCheckBox->Checked) SortAssoListBox(s);
	}
	//変更
	else {
		if (AssociateListBox->ItemIndex==-1) return;
		AssociateListBox->Items->Strings[AssociateListBox->ItemIndex] = s;
		if (AsoSortCheckBox->Checked) SortAssoListBox(s);
	}

	MaxWd_AssExt = 0;
	for (int i=0; i<AssociateListBox->Count; i++)
		MaxWd_AssExt = std::max(AssociateListBox->Canvas->TextWidth(AssociateListBox->Items->Names[i]), MaxWd_AssExt);
	MaxWd_AssExt += ScaledInt(20);
}

//---------------------------------------------------------------------------
//関連付けの参照内容を設定
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AsoAppComboBoxDropDown(TObject *Sender)
{
	std::unique_ptr<TStringList> cmd_lst(new TStringList());
	for (int i=0; i<AssociateListBox->Count; i++) {
		UnicodeString lbuf = exclude_quot(AssociateListBox->Items->ValueFromIndex[i]);
		if (!USTARTS_TS('$', lbuf) && !USAME_TS(lbuf, "-")) cmd_lst->Add(lbuf);
	}
	cmd_lst->Sort();

	//重複削除
	if (cmd_lst->Count>1) {
		UnicodeString laststr = cmd_lst->Strings[0];
		int i = 1;
		while (i<cmd_lst->Count) {
			UnicodeString lbuf = cmd_lst->Strings[i];
			if (SameStr(laststr, lbuf))
				cmd_lst->Delete(i);
			else {
				laststr = lbuf;
				i++;
			}
		}
	}

	//エイリアス
	for (int i=0; i<ExtMenuListBox->Count; i++) {
		UnicodeString itm = get_csv_item(ExtMenuListBox->Items->Strings[i], 3);
	 	if (!itm.IsEmpty()) cmd_lst->Add("$" + itm);
	}
	for (int i=0; i<ExtToolListBox->Count; i++) {
		UnicodeString itm = get_csv_item(ExtToolListBox->Items->Strings[i], 4);
	 	if (!itm.IsEmpty()) cmd_lst->Add("$" + itm);
	}

	AsoAppComboBox->Items->Assign(cmd_lst.get());
}

//---------------------------------------------------------------------------
//追加
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddAssoActionExecute(TObject *Sender)
{
	ChangeAssociateList(true);
}
//---------------------------------------------------------------------------
//変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgAssoActionExecute(TObject *Sender)
{
	ChangeAssociateList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgAssoActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !AsoExtEdit->Text.IsEmpty() && !AsoAppComboBox->Text.IsEmpty();
}

//---------------------------------------------------------------------------
//関連付け実行ファイルの選択
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AsoRefBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("関連付け"), F_FILTER_EXE2 _T("|") F_FILTER_NBT);
	UnicodeString fnam;
	if (UserModule->OpenDlgToStr(fnam)) SetExtNameToCtrl(fnam, AsoAppComboBox, true);
}

//---------------------------------------------------------------------------
//一覧の描画
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OptListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);

	if (lp==EventListBox && USAME_TI(EventCmdList[Index].key, "OnNewDrive") && !State.Contains(odSelected)) {
		cv->Brush->Color = !OpenAddDrvCheckBox->Checked? col_Invalid : scl_Window;
		cv->Font->Color  = scl_WindowText;
	}
	else
		SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);

	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);
	UnicodeString lbuf = lp->Items->Strings[Index];
	bool brk = false;

	//SPI
	if (lp==SpiListBox) {
		int w_x = 0;
		for (int i=0; i<lp->Count; i++)
			w_x = std::max(cv->TextWidth(get_pre_tab(lp->Items->Strings[i])), w_x);
		cv->TextOut(xp, yp, split_pre_tab(lbuf));
		cv->TextOut(xp + w_x + 20, yp, lbuf);
		if (Index>0) {
			UnicodeString vt   = lbuf.SubString(1, 4);
			UnicodeString u_vt = get_post_tab(lp->Items->Strings[Index - 1]).SubString(1, 4);
			brk = !SameText(vt, u_vt);
		}
	}
	//イベント
	else if (lp==EventListBox) {
		UnicodeString tmp = split_pre_tab(lbuf);
		brk = remove_top_s(tmp, '|');
		cv->TextOut(xp, yp, tmp);
		cv->TextOut(xp + MaxWd_Ev, yp, lbuf);
	}
	//関連付け
	else {
		if (Index>0) brk = !SameText(lp->Items->Names[Index], lp->Items->Names[Index-1]);
		cv->TextOut(xp, yp, split_tkn(lbuf, '='));
		cv->TextOut(xp + MaxWd_AssExt, yp, exclude_quot(lbuf));
	}

	//境界線(上辺)
	if (brk) draw_separateLine(cv, Rect);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OptMenuListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TCheckListBox *lp = (TCheckListBox*)Control;
	bool is_tool = (lp==ExtToolListBox);
	THeaderSections *sp = ((THeaderControl *)(is_tool? ExtToolHeader : ExtMenuHeader))->Sections;

	TCanvas *cv  = lp->Canvas;
	cv->Font->Assign(lp->Font);
	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);

	//項目の階層を調べる
	int sub_lvl = 0;
	for (int i=0; i<=Index; i++) {
		UnicodeString itm = get_csv_item(lp->Items->Strings[i], 0);
		if (i<Index && USTARTS_TS('>', itm)) sub_lvl++;
		else if (USAME_TS(itm, "<") && sub_lvl>0) sub_lvl--;
	}

	int xp = Rect.Left + 2 + ScaledInt(18) + (sub_lvl * cv->TextWidth("W"));
	int yp = Rect.Top + get_TopMargin(cv);

	UnicodeString lbuf = lp->Items->Strings[Index];
	TStringDynArray itm_buf = get_csv_array(lbuf, (is_tool? EXTTOOL_CSVITMCNT : EXTMENU_CSVITMCNT), true);
	//セパレータ
	if (is_separator(itm_buf[0])) {
		TRect rc = Rect;  rc.Left = xp - 4;
		draw_Separator(cv, rc);
	}
	//項目
	else {
		//アイコン
		usr_SH->draw_SmallIcon(rel_to_absdir(cv_env_str(itm_buf[is_tool? 1 : 5])), cv, Rect.Left + 2, yp);
		//キャプション
		UnicodeString lbuf = minimize_str(itm_buf[0], cv, sp->Items[0]->Width - xp, true);
		cv->TextOut(xp, yp, lbuf);
		//エイリアス
		if (!starts_tchs(_T("-<"), lbuf)) {
			cv->TextOut(sp->Items[0]->Width, yp, minimize_str(itm_buf[is_tool? 4 : 3], cv, sp->Items[1]->Width, true));
		}
		//設定
		if (!starts_tchs(_T("-<>"), lbuf)) {
			lbuf = EmptyStr;
			for (int i=1; i<(is_tool? 4 : 3); i++) {
				if (i>1) lbuf.UCAT_T(", ");
				lbuf += itm_buf[i];
			}
			cv->TextOut(sp->Items[0]->Width + sp->Items[1]->Width, yp, lbuf);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtMenuHeaderSectionResize(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
	ExtMenuListBox->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtToolHeaderSectionResize(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
	ExtToolListBox->Repaint();
}

//---------------------------------------------------------------------------
//キー設定
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyTabControlDrawTab(TCustomTabControl *Control, int TabIndex,
		const TRect &Rect, bool Active)
{
	//※テーマ利用時に下部タブが正しく描画されない不具合の対策
	draw_BottomTab(Control, TabIndex, Rect, Active);
}
//---------------------------------------------------------------------------
//カテゴリー変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyTabControlChanging(TObject *Sender, bool &AllowChange)
{
	KeyListBuf[KeyTabControl->TabIndex]->Assign(KeyListBox->Items);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyTabControlChange(TObject *Sender)
{
	KeyListBox->Items->Assign(KeyListBuf[KeyTabControl->TabIndex]);

	Key2ComboBox->ItemIndex = -1;
	Key2ComboBox->Enabled	= (KeyTabControl->TabIndex==0 || KeyTabControl->TabIndex==2 || KeyTabControl->TabIndex==3);
	SelCheckBox->Enabled	= (KeyTabControl->TabIndex==0);

	KeyFilterEdit->Text = EmptyStr;
	SortKeyListBox();
	SetCmdCombo();
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyHeaderControlDrawSection(THeaderControl *HeaderControl,
		  THeaderSection *Section, const TRect &Rect, bool Pressed)
{
	draw_SortHeader(HeaderControl, Section, Rect, (Section->Index==KeySortMode)? 1 : 0, true);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyHeaderControlSectionClick(THeaderControl *HeaderControl,
		  THeaderSection *Section)
{
	KeySortMode = Section->Index;
	SortKeyListBox();
}

//---------------------------------------------------------------------------
//コマンド(CmdComboBox)の項目を設定
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SetCmdCombo(
	UnicodeString kwd)		//フィルタ文字列 (default = EmptyStr : 絞り込み無し)
{
	PrmComboBox->Text	 = EmptyStr;
	PrmComboBox->Enabled = false;

	TStrings *sp = CmdComboBox->Items;
	sp->Clear();
	if (KeyTabControl->TabIndex==-1) return;

	std::unique_ptr<TStringList> cmd_lst(new TStringList());
	UnicodeString modstr = GetCmdModeStr();
	for (int i=0; i<CmdSetList->Count; i++) {
		UnicodeString lbuf = CmdSetList->Strings[i];
		if (StartsText(modstr, lbuf)) cmd_lst->Add(get_tkn_r(lbuf, ':'));
	}
	//追加メニュー/外部ツールのエイリアス
	if (KeyTabControl->TabIndex==0 || KeyTabControl->TabIndex==2 || KeyTabControl->TabIndex==3) {
		for (int i=0; i<ExtMenuListBox->Count; i++) {
			UnicodeString itm = get_csv_item(ExtMenuListBox->Items->Strings[i], 3);
		 	if (!itm.IsEmpty()) cmd_lst->Add("$" + itm);
		}
		for (int i=0; i<ExtToolListBox->Count; i++) {
			UnicodeString itm = get_csv_item(ExtToolListBox->Items->Strings[i], 4);
		 	if (!itm.IsEmpty()) cmd_lst->Add("$" + itm);
		}
	}

	if (UnRegCmdCheckBox->Checked) {
		TListBox *lp = KeyListBox;
		//登録済コマンドを抽出
		std::unique_ptr<TStringList> reg_lst(new TStringList());
		for (int i=0; i<lp->Count; i++) {
			if (StartsText(modstr, lp->Items->Strings[i])) reg_lst->Add(lp->Items->ValueFromIndex[i]);
		}
		//未登録項目のみを残す
		int i = 0;
		while (i<cmd_lst->Count) {
			bool found = false;
			for (int j=0; j<reg_lst->Count && !found; j++)
				found = SameText(cmd_lst->Strings[i], get_CmdStr(reg_lst->Strings[j]));
			if (found) cmd_lst->Delete(i); else i++;
		}
		//現在位置の項目を追加
		int idx = lp->ItemIndex;
		if (idx!=-1) {
			cmd_lst->Add(get_CmdStr(lp->Items->ValueFromIndex[idx]));
			cmd_lst->Sort();
		}
	}

	//フィルタ
	if (!kwd.IsEmpty()) {
		std::unique_ptr<TStringList> lst(new TStringList());
		lst->Assign(cmd_lst.get());
		UnicodeString ptn = usr_Migemo->GetRegExPtn(MigemoCheckBox->Checked, kwd);
		if (!ptn.IsEmpty()) {
			TRegExOptions opt; opt << roIgnoreCase;
			int i=0;
			while (i<lst->Count) {
				UnicodeString lbuf = get_CmdDesc(lst->Strings[i], false, ExtMenuListBox->Items, ExtToolListBox->Items);
				if (!TRegEx::IsMatch(lbuf, ptn, opt)) lst->Delete(i); else i++;
			}
		}
		cmd_lst->Assign(lst.get());
	}

	//ドロップダウン項目を割り当て
	for (int i=0; i<cmd_lst->Count; i++)
		sp->Add(get_CmdDesc(cmd_lst->Strings[i], false, ExtMenuListBox->Items, ExtToolListBox->Items));
}

//---------------------------------------------------------------------------
//コマンドコンボボックスの描画
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::CmdComboBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,
		TOwnerDrawState State)
{
	TComboBox *cp = (TComboBox*)Control;
	TCanvas   *cv = cp->Canvas;
	int yp = Rect.Top + get_TopMargin(cv);
	int xp = Rect.Left + 4;

	UnicodeString lbuf = cp->Items->Strings[Index];
	UnicodeString cmd  = split_tkn(lbuf, ' ');

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);
	cv->TextOut(xp, yp, cmd);
	xp += cv->TextHeight("Q")*10;
	cv->TextOut(xp, yp, Trim(lbuf));
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::CmdComboBoxChange(TObject *Sender)
{
	UnicodeString cmd = get_tkn(CmdComboBox->Text," ");

	int tab_idx = KeyTabControl->TabIndex;
	UnicodeString params;

	PrmComboBox->Clear();
	PrmComboBox->Style	  = csDropDownList;
	PrmComboBox->Text	  = EmptyStr;
	PrmComboBox->Enabled  = false;
	RefCmdPrmBtn->Enabled = false;

	if (contained_wd_i(
	 	_T("AddTag|CalcDirSize|CalcDirSizeAll|Calculator|ChangeDir|ChangeOppDir|Clone|CloneToCurr|ContextMenu|ConvertHtm2Txt|")
		_T("Copy|CopyTo|CountLines|CreateDir|DateSelect|DebugCmdFile|ExeCommands|ExeMenuFile|FileEdit|FileRun|Filter|")
		_T("FindFileDlg|FindFileDirDlg|FindTag|FTPChmod|Grep|IncSearch|JumpTo|JumpIndex|PropertyDlg|MaskFind|MaskSelect|")
		_T("MatchSelect|ListArchive|ListDuration|ListExpFunc|ListTail|ListText|LoadBgImage|LoadWorkList|MoveTo|NewTextFile|")
		_T("OpenByApp|OpenByExp|OpenByWin|Pack|PackToCurr|PlayList|RegExChecker|")
		_T("ScrollUpLog|ScrollUpText|ScrollDownLog|ScrollDownText|SetDirTime|")
		_T("SetPathMask|SetSttBarFmt|SetSubSize|SetTag|SubDirList|TagJumpDirect|TagViewDirect|ToTab|TextViewer|ImageViewer|")
		_T("ViewTail|WatchTail|WidenCurList|WinPos|CursorUp|CursorDown|FindDown|FindUp|JumpLine|OpenURL|")
		_T("ScrollCursorDown|ScrollCursorUp|ScrollDown|ScrollUp|")
		_T("SetColor|SetMargin|SetTab|SetWidth|SetUserDefStr|SetFontSize|HelpCurWord|Restart"), cmd))
	{
		PrmComboBox->Enabled = true;
		PrmComboBox->Style = USAME_TI(cmd, "ImageViewer")? csDropDownList : csDropDown;
		//ファイル/ディレクトリ参照ボタン
		RefCmdPrmBtn->Enabled = contained_wd_i(
			_T("CalcDirSize|CalcDirSizeAll|ChangeDir|ChangeOppDir|ContextMenu|CopyTo|DebugCmdFile|ExeCommands|ExeMenuFile|")
			_T("FileEdit|FileRun|JumpTo|ListArchive|ListDuration|ListExpFunc|ListTail|ListText|LoadBgImage|LoadWorkList|")
			_T("MoveTo|OpenByApp|OpenByExp|OpenByWin|PropertyDlg|")
			_T("PlayList|TextViewer|SetColor|SubDirList|HelpCurWord|Restart"), cmd);
	}

	//パラメータ項目を設定
	if (USAME_TI(cmd, "AlphaBlend")) {
		PrmComboBox->Style = csDropDown;
		params.USET_T("\r\nIN : 透明度を入力\r\n");
	}
	else if (USAME_TI(cmd, "AppList")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("FA : 一覧側にフォーカス\n")
			_T("FL : ランチャー側にフォーカス\n")
			_T("FI : ランチャー側にフォーカス(INC.サーチ)\n")
			_T("AO : 一覧のみ表示\n")
			_T("LO : ランチャーのみ表示\n")
			_T("LI : ランチャーのみ表示(INC.サーチ)\n"));
	}
	else if (USAME_TI(cmd, "BgImgMode")) {
		params.sprintf(_T("%s"),
			_T("OFF : 非表示\n")
			_T("1 : 2画面にわたって表示\n")
			_T("2 : それぞれに表示\n")
			_T("3 : デスクトップ背景を表示\n")
			_T("^1 : 2画面にわたって表示/非表示\n")
			_T("^2 : それぞれに表示/非表示\n")
			_T("^3 : デスクトップ背景を表示/非表示\n"));
	}
	else if (contained_wd_i(_T("CursorUp|CursorDown|ScrollCursorDown|ScrollCursorUp|ScrollDown|ScrollUp"), cmd) && tab_idx!=4) {
		params.USET_T("\nHP : 半ページ分\nFP : 1ページ分\n");
	}
	else if (contained_wd_i(_T("CalcDirSize|CalcDirSizeAll"), cmd)) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("LO : 結果をログに出力\n")
			_T("CC : 結果をクリップボードにコピー\n")
			_T("LS : 結果を一覧表示\n")
			_T("FC : ファイル数、サブディレクトリ数を表示\n"));
		if (USAME_TI(cmd, "CalcDirSizeAll")) {
			params.UCAT_T("SG : グラフ表示(対カレント)\nDG : グラフ表示(対ドライブ)\n");
		}
	}
	else if (USAME_TI(cmd, "Calculator")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("CB : クリップボードを介して計算\n"));
	}
	else if (USAME_TI(cmd, "ChangeCodePage")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("932 :   Shift_JIS\n")
			_T("50220 : JIS(ISO-2022-JP)\n")
			_T("20932 : EUC-JP\n")
			_T("1252 :  Latin-1\n")
			_T("65001 : UTF-8\n")
			_T("1200 :  UTF-16\n"));
	}
	else if (contained_wd_i(_T("ChangeDrive|DriveGraph|EjectDrive"), cmd)) {
		for (int i=0; i<26; i++) params.cat_sprintf(_T("%c\n"), 'A'+i);	//英字
		if (USAME_TI(cmd, "EjectDrive")) params.UCAT_T(". : カレントドライブ\n");
		if (USAME_TI(cmd, "DriveGraph")) params = "\n" + params;
	}
	else if (contained_wd_i(_T("ChangeRegDir|ChangeOppRegDir"), cmd)) {
		for (int i=0; i<RegDirList->Count; i++) {
			TStringDynArray itm_buf = get_csv_array(RegDirList->Strings[i], 3);
			if (itm_buf.Length!=3 || itm_buf[0].IsEmpty()) continue;
			params.cat_sprintf(_T("%s : %s\n"), itm_buf[0].UpperCase().c_str(), itm_buf[1].c_str());
		}
	}
	else if (USAME_TI(cmd, "CheckUpdate")) {
		params.USET_T("\nNC : 保存場所の選択、確認なし\n");
	}
	else if (USAME_TI(cmd, "ClearAll")) {
		params.USET_T("\nAL : 左右すべての選択状態を解除\n");
	}
	else if (USAME_TI(cmd, "ClearMark")) {
		params.USET_T("\nAC : すべての場所のすべてのマークを解除\n");
	}
	else if (USAME_TI(cmd, "ClipCopy")) {
		if (tab_idx==2) {
			params.USET_T("\nAD : 現在の内容に追加\n");
		}
		else if (tab_idx==3) {
			params.USET_T("\nVI : 表示されている状態でコピー\n");
		}
	}
	else if (USAME_TI(cmd, "Close") && tab_idx==2) {
		params.USET_T("\nAL : すべての別ウィンドウを閉じる\n");
	}
	else if (USAME_TI(cmd, "CompareDlg")) {
		params.USET_T("\nNC : ダイアログを出さず、名前のみ比較\n");
	}
	else if (contained_wd_i(_T("CompareHash|GetHash"), cmd)) {
		params.sprintf(_T("%s"),
			_T("MD5\n")
			_T("SHA1\n")
			_T("SHA256\n")
			_T("SHA384\n")
			_T("SHA512\n")
			_T("CRC32\n"));
	}
	else if (USAME_TI(cmd, "ConvertDoc2Txt")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("SJ : Shift_JIS で出力(デフォルト)\n")
			_T("IJ : ISO-2022-JP で出力\n")
			_T("EJ : EUC-JP で出力\n")
			_T("U8 : UTF-8 で出力\n")
			_T("UL : UTF-16 で出力\n")
			_T("UB : UTF-16BE で出力\n"));
	}
	else if (USAME_TI(cmd, "ConvertHtm2Txt")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("HD : ヘッダ情報を挿入\n")
			_T("MD : Markdown記法に変換\n")
			_T("TX : 通常テキストに変換\n"));
	}
	else if (USAME_TI(cmd, "CopyDir")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("TO : コピー先を入力\n")
			_T("KT : タイムスタンプを維持\n")
			_T("CC : ディレクトリ名をクリップボードにコピー\n")
			_T("LS : ディレクトリ名を一覧表示\n"));
	}
	else if (contained_wd_i(_T("CopyFileName|ListFileName"), cmd)) {
		PrmComboBox->Style = csDropDown;
		params.USET_T("\nFN : ファイル名部分のみ\n");
	}
	else if (USAME_TI(cmd, "CreateDir")) {
		params.USET_T("\r\nIN : デフォルト名を指定して入力\r\n");
	}
	else if (contained_wd_i(_T("CsrDirToOpp|LinkToOpp|SwapLR"), cmd)) {
		params.USET_T("\nTO : 反対側へ移動\n");
	}
	else if (contained_wd_i(_T("CurrFromOpp|CurrToOpp"), cmd)) {
		params.USET_T("\nSL : 選択状態を反映\n");
		if (USAME_TI(cmd, "CurrToOpp")) params.UCAT_T("TO : 反対側へ移動\n");
	}
	else if (USAME_TI(cmd, "DelJpgExif")) {
		params.USET_T("\nKT : タイムスタンプを維持\n");
	}
	else if (USAME_TI(cmd, "DirHistory")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("GA : 全体履歴を表示\n")
			_T("GS : 全体履歴を表示(ソート/重複削除)\n")
			_T("FM : 全体履歴をフィルタ検索\n")
			_T("AC : カレント側の履歴をすべて削除\n")
			_T("GC : 全体履歴をすべて削除\n"));
	}
	else if (USAME_TI(cmd, "DistributionDlg")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("XC : 確認なしで直ちにコピー\n")
			_T("XM : 確認なしで直ちに移動\n")
			_T("SN : ファイルリストからマスクと振分先を設定\n"));
	}
	else if (USAME_TI(cmd, "Duplicate")) {
		params.USET_T("\nRA : 管理者として二重起動\n");
	}
	else if (contained_wd_i(_T("EditHistory|ViewHistory"), cmd)) {
		params.USET_T("\nFF : フィルタ欄にフォーカス\nAC : 履歴をすべて消去\n");
	}
	else if (USAME_TI(cmd, "Eject")) {
		params.USET_T("\n");
		for (int i=0; i<DriveInfoList->Count; i++) {
			drive_info *dp = (drive_info *)DriveInfoList->Objects[i];
			if (dp->drv_type==DRIVE_CDROM)
				params.cat_sprintf(_T("%s\n"), get_tkn(dp->drive_str, ':').c_str());
		}
	}
	else if (USAME_TI(cmd, "ExeCommandLine")) {
		params.USET_T("\nFN : カーソル位置のファイル名を入力\n");
	}
	else if (contained_wd_i(_T("ExeExtMenu|ExeExtTool"), cmd)) {
		TStringList *lst= USAME_TI(cmd, "ExeExtMenu")? ExtMenuList : ExtToolList;
		for (int i=0; i<lst->Count; i++) {
			UnicodeString itm = get_csv_item(lst->Strings[i], 0);
			if (!contains_s(itm, _T('&'))) continue;
			UnicodeString ak = get_tkn_r(itm, '&').SubString(1, 1);
			if (ak.IsEmpty() || !_istalnum(ak[1])) continue;
			params.cat_sprintf(_T("%s : %s\n"), ak.UpperCase().c_str(), itm.c_str());
		}
	}
	else if (USAME_TI(cmd, "ExeToolBtn")) {
		TStringList *lst= ToolBtnList;
		for (int i=0; i<lst->Count; i++) {
			TStringDynArray itm_buf = get_csv_array(lst->Strings[i], 2, true);
			if (is_separator(itm_buf[0])) continue;
			params.cat_sprintf(_T("%u : %s\n"), i + 1, (!itm_buf[0].IsEmpty()? itm_buf[0] : itm_buf[1]).c_str());
		}
	}
	else if (USAME_TI(cmd, "ExtractIcon")) {
		PrmComboBox->Style = csDropDown;
		params.USET_T("\nSI : スモールアイコンを抽出\n");
	}
	else if (USAME_TI(cmd, "Filter")) {
		params.USET_T("\nCS : 大小文字を区別\nCA : 実行前に選択マスクを解除\n");
	}
	else if (contained_wd_i(_T("FindFileDlg|FindFileDirDlg"), cmd)) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("NM : マスク欄を非表示\n")
			_T("FK : 検索語欄にフォーカス\n")
			_T("R0 : 「サブディレクトリも検索」オフ\n")
			_T("R1 : 「サブディレクトリも検索」オン\n"));
		if (USAME_TI(cmd, "FindFileDlg")) {
			params.cat_sprintf(_T("%s"),
				_T("X0 : 「拡張検索」オフ\n")
				_T("X1 : 「拡張検索」オン\n")
				_T("A0 : 「アーカイブ内も検索」オフ\n")
				_T("A1 : 「アーカイブ内も検索」オン\n"));
		}
	}
	else if (USAME_TI(cmd, "FindHardLink")) {
		params.USET_T("\nOP : 結果リストから反対側へ反映\n");
	}
	else if (USAME_TI(cmd, "FindMark")) {
		params.USET_T("\nAL : すべてのマーク項目を検索\n");
	}
	else if (USAME_TI(cmd, "FindTagName")) {
		params.USET_T("\nEJ : テキストエディタでダイレクトタグジャンプ\n");
		if (tab_idx==2) params.UCAT_T("CO : 現在のファイルのみを検索\n");
	}
	else if (contained_wd_i(_T("FindSelDown|FindSelUp"), cmd)) {
		params.USET_T("\nEM : マッチ語を強調表示\n");
	}
	else if (USAME_TI(cmd, "HelpContents")) {
		params.USET_T("\nCI : コマンドの索引\nFI : コマンドの機能別索引\nCH : 変更履歴\n");
	}
	else if (USAME_TI(cmd, "IncSearch")) {
		params.USET_T("\nMM : Migomoモード\nNM : 通常モード\n");
		if (tab_idx==0) params.UCAT_T("FM : フィルタマスク・モード\nCA : 実行前に選択マスクを解除\n");
	}
	else if (USAME_TI(cmd, "InputDir")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("ND : ダイアログを表示しないで入力\n")
			_T("SD : フォルダ参照ダイアログを表示\n"));
	}
	else if (USAME_TI(cmd, "DriveList")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("ND : ポップアップメニューで選択\n")
			_T("NS : ポップアップメニューで選択(空き容量非表示)\n"));
	}
	else if (USAME_TI(cmd, "InputCommands")) {
		params.USET_T("\nEL : エコー、コマンドファイルの行番号表示\n");
	}
	else if (contained_wd_i(_T("CmdFileList|ListClipboard|MarkList|KeyList|FunctionList|UserDefList"), cmd)) {
		params.USET_T("\nFF : フィルタ欄にフォーカス\n");
	}
	else if (contained_wd_i(_T("Exit|Close"), cmd)) {
		params.USET_T("\nNS : INIファイルを保存しない\nNX : 他のNyanFiを終了させない\n");
	}
	else if (USAME_TI(cmd, "ExPopupMenu")) {
		params.USET_T("\nMN : 追加メニューのみ表示\nTL : 外部ツールのみ表示\n");
	}
	else if (USAME_TI(cmd, "Library")) {
		params.USET_T("\nSD : 選択ダイアログを表示\n* : 選択メニューを表示\n");
		std::unique_ptr<TStringList> lst(new TStringList());
		get_files(LibraryPath, _T("*.library-ms"), lst.get());
		for (int i=0; i<lst->Count; i++)
			params.cat_sprintf(_T("%s\n"), get_base_name(lst->Strings[i]).c_str());
	}
	else if (contained_wd_i(_T("CountLines|ListArchive|ListDuration|ListExpFunc|ListNyanFi"), cmd)) {
		params.USET_T("\nCC : 結果をクリップボードにコピー\nLS : 結果を一覧表示\n");
	}
	else if (USAME_TI(cmd, "ListLog")) {
		params.USET_T("\nEO : エラー箇所の絞り込み表示\n");
	}
	else if (USAME_TI(cmd, "ListTail")) {
		params.USET_T("\nTE : 最後尾に移動r\nFF : フィルタ欄にフォーカス\n");
	}
	else if (USAME_TI(cmd, "ListText")) {
		params.USET_T("\nFF : フィルタ欄にフォーカス\nEO : エラー箇所の絞り込み表示\n");
	}
	else if (USAME_TI(cmd, "Mark") && (tab_idx==0 || tab_idx==3)) {
		params.USET_T("\nND : カーソルを移動しない\nIM : メモを入力\n");
	}
	else if (USAME_TI(cmd, "MoveTab")) {
		params.USET_T("\nTP : 先頭に先頭\nED : 最後に移動\nPR : １つ前に移動\n");
	}
	else if (USAME_TI(cmd, "PageBind")) {
		params.USET_T("\nR : 右綴じ\nL : 左綴じ\n");
	}
	else if (USAME_TI(cmd, "NextNyanFi")) {
		params.USET_T("\nDN : なければ二重起動\n");
	}
	else if (USAME_TI(cmd, "PlayList")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("RP : リピート再生\n")
			_T("SF : シャッフル再生\n")
			_T("SR : シャッフル・リピート再生\n")
			_T("NX : 次の曲へ\n")
			_T("PR : 前の曲へ\n")
			_T("PS : 一時停止\n")
			_T("RS : 再開\n")
			_T("FI : ファイル情報を表示\n")
			_T("LS : プレイリストを表示\n"));
	}
	else if (contained_wd_i(_T("RegDirDlg|PathMaskDlg"), cmd)) {
		params.USET_T("\nND : ポップアップメニューで選択\n");
	}
	else if (contained_wd_i(_T("RegDirPopup|PushDir|PopDir"), cmd)) {
		params.USET_T("\nOP : 反対側で実行\n");
	}
	else if (USAME_TI(cmd, "ReloadList")) {
		params.USET_T("\nCO : カレントのみ更新\nHL : ハードリンクのタイムスタンプ更新\nOFF : 更新禁止\n");
	}
	else if (USAME_TI(cmd, "Restart")) {
		params.USET_T("\nNS : INIファイルを保存しない\nRA : 管理者として再起動\nDM : 管理者から降格して再起動\n");
	}
	else if (USAME_TI(cmd, "SaveAsWorkList")) {
		params.USET_T("\nFL : カレントの内容をワークリストとして保存\n");
	}
	else if (contained_wd_i(_T("ScrollUpLog|ScrollUpText|ScrollDownLog|ScrollDownText"), cmd)) {
		params.USET_T("\nHP : 半ページ分\nFP : 1ページ分\n");
		params.cat_sprintf(_T("%s\n"), contains_i(cmd, _T("Down"))? _T("ED : 最後へ") : _T("TP : 先頭へ"));
	}
	else if (USAME_TI(cmd, "Select") && (tab_idx==0 || tab_idx==3)) {
		PrmComboBox->Style = csDropDown;
		params.USET_T("\n");
		if (tab_idx==0) params.UCAT_T("IN : 繰り返し回数を入力\n");
		params.UCAT_T("ND : カーソルを移動しない\n");
	}
	else if (USAME_TI(cmd, "SelectFile")) {
		params.USET_T("\nNX : 次のファイルを表示\n");
	}
	else if (USAME_TI(cmd, "SelEmptyDir")) {
		params.USET_T("\nNF : ファイルが含まれていなければ選択\n");
	}
	else if (contained_wd_i(_T("MarkMask|SelMask"), cmd)) {
		params.USET_T("\nCA : マスクを解除\n");
	}
	else if (USAME_TI(cmd, "SetColor")) {
		params.USET_T("\nRS : 配色をリセット\n");
	}
	else if (USAME_TI(cmd, "SetPathMask")) {
		params.USET_T("\nEX : カーソル位置の拡張子でマスク\n");
	}
	else if (USAME_TI(cmd, "SetTopAddr")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("TP : 先頭アドレスを先頭に\n")
			_T("NX : 先頭アドレスを後続部に\n")
			_T("PR : 先頭アドレスを先行部に\n")
			_T("ED : 終端アドレスを最後に\n"));
	}
	else if (USAME_TI(cmd, "ShowFileInfo")) {
		params.USET_T("\nSD : ダイアログで表示\n");
	}
	else if (USAME_TI(cmd, "Sort")) {
		params.USET_T("\nAO : 昇順\nDO : 降順\n");
	}
	else if (USAME_TI(cmd, "SortDlg")) {
		params.sprintf(_T("%s"),
		 	_T("\n")
			_T("F : 名前順\n")
			_T("E : 拡張子順\n")
			_T("D : 更新日時順\n")
			_T("S : サイズ順\n")
			_T("A : 属性順\n")
			_T("L : 場所順(結果リスト)\n")
			_T("FE : 名前/拡張子順トグル切り替え\n")
			_T("FD : 名前/更新日時順トグル切り替え\n")
			_T("FS : 名前/サイズ順トグル切り替え\n")
			_T("ED : 拡張子/更新日時順トグル切り替え\n")
			_T("ES : 拡張子/サイズ順トグル切り替え\n")
			_T("DS : 更新日時順/サイズ順トグル切り替え\n")
			_T("IV : 現在のソート方法を逆順に\n")
			_T("IA : すべてのソート方法を逆順に\n")
			_T("XN : ディレクトリ - ファイルと同じ\n")
			_T("XF : ディレクトリ - 名前\n")
			_T("XD : ディレクトリ - 更新日時\n")
			_T("XS : ディレクトリ - サイズ\n")
			_T("XA : ディレクトリ - 属性\n")
			_T("XX : ディレクトリを区別しない\n")
			_T("XNX : ファイルと同じ/ディレクトリを区別しないトグル切り替え\n"));
	}
	else if (USAME_TI(cmd, "SubDirList")) {
		params.USET_T("\nND : ポップアップメニューで選択\n");
	}
	else if (USAME_TI(cmd, "SwapName")) {
		params.USET_T("\nLR : 左右で入れ替え\n");
	}
	else if (USAME_TI(cmd, "TabHome")) {
		params.USET_T("\nAL : すべてのタブに適用\nCO : カレントのみに適用\n");
	}
	else if (contained_wd_i("TagJump|TagView", cmd)) {
		params.USET_T("\nDJ : 見つからなければダイレクトタグジャンプ\n");
	}
	else if (contained_wd_i(_T("TextViewer|ImageViewer"), cmd)) {
		params.USET_T("\nCB : クリップボードの内容を表示\nNN : 次のNyanFiで表示\n");
		if (USAME_TI(cmd, "TextViewer")) params.UCAT_T("XW : 別ウィンドウで表示\n");
	}
	else if (USAME_TI(cmd, "ToOppSameItem")) {
		params.USET_T("\nNO : 反対側へ移動しない\n");
	}
	else if (contained_wd_i(_T("ToParent|ToParentOnLeft|ToParentOnRight"), cmd)) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("DL : ルートならドライブ/共有フォルダ一覧を表示\n")
			_T("DP : ルートならドライブ選択メニューを表示\n"));
	}
	else if (USAME_TI(cmd, "ToTab")) {
		params.USET_T("\n");
		for (int i=0; i<TabList->Count; i++) {
			UnicodeString s = get_csv_item(TabList->Strings[i], 2);
			if (!s.IsEmpty()) params.cat_sprintf(_T("%s\n"), s.c_str());
		}
	}
	else if (contained_wd_i(_T("UnPack|UnPackToCurr"), cmd)) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("CD : ディレクトリを作成してその中に\n")
			_T("CD2 : ルートに複数の対象があったらディレクトリ作成\n"));
	}
	else if (USAME_TI(cmd, "UpdateFromArc")) {
		params.USET_T("\nUN : 新しいアーカイブを探して更新\n");
	}
	else if (contained_wd_i("ViewIniFile|ViewLog", cmd)) {
		params.UCAT_T("\nXW : 別ウィンドウで表示\n");
	}
	else if (USAME_TI(cmd, "WatchTail")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("ST : 監視内容を表示\n")
			_T("CC : カーソル位置ファイルの監視を中止\n")
			_T("AC : すべての監視を中止\n"));
	}
	if (USAME_TI(cmd, "WebMap")) {
		params.USET_T("\nIN : 緯度,経度を入力\n");
	}
	else if (USAME_TI(cmd, "WebSearch")) {
		params.USET_T("\nCB : クリップボードの内容を検索\n");
		if (tab_idx==0) params.UCAT_T("FN : カーソル位置のファイル名を検索\n");
	}
	else if (USAME_TI(cmd, "WorkList") && tab_idx==0) {
		params.USET_T("\nOP : 反対側で実行\nRL : 変更内容を破棄して読み込み直す\nDI : 無効な項目を一括削除\n");
	}
	else if (contained_wd_i(_T("ZoomIn|ZoomOut"), cmd)) {
		params.USET_T("\n");
		for (int i=2; i<=12; i++) params.cat_sprintf(_T("%u\n"), i);
	}
	else if (contained_wd_i(_T("Copy|Move|Delete|CompleteDelete"), cmd)) {
		params.USET_T("\nSO : 選択項目のみ処理\n");
		if (USAME_TI(cmd, "Copy")) {
			params.cat_sprintf(_T("%s"),
				_T("OP : 反対側コピー先のカーソル位置を設定\nOP2 : コピー先のカーソル位置を逐次設定\n")
			 	_T("TO : コピー先を入力\nSD : コピー先を参照\nSS : カレントのサブディレクトリを選択\n"));
		}
		if (USAME_TI(cmd, "Move")) {
			params.cat_sprintf(_T("%s"),
				_T("OP : 反対側移動先のカーソル位置を設定\nOP2 : 移動先のカーソル位置を逐次設定\n")
			 	_T("TO : 移動先を入力\nSD : 移動先を参照\nSS : カレントのサブディレクトリを選択\n"));
		}
	}
	//トグル動作コマンド
	else if (contained_wd_i(
		_T("FileListOnly|HideSizeTime|LockTextPreview|MenuBar|MuteVolume|")
		_T("ShowByteSize|ShowFKeyBar|ShowHideAtr|ShowIcon|ShowPreview|ShowProperty|ShowStatusBar|ShowSystemAtr|ShowTabBar|")
		_T("PauseAllTask|Suspend|SyncLR|UseTrash|")
		_T("BitmapView|CharInfo|CsvRecord|Highlight|HtmlToText|Inspector|ShowCR|ShowLineNo|ShowRuby|ShowRuler|ShowTAB|FixedLen|")
		_T("DoublePage|FullScreen|GifViewer|SubViewer|GrayScale|Histogram|Loupe|ShowGrid|ShowSeekBar|Sidebar|")
		_T("Thumbnail|ThumbnailEx|WarnHighlight"),
		cmd))
	{
		params.USET_T("\nON : 表示/有効\nOFF : 非表示/無効/解除\n");
		if (USAME_TI(cmd, "SubViewer"))
			params.UCAT_T("LK : ロック/解除\nRL : 左に90度回転\nRR : 右に90度回転\nFH : 左右反転\nFV : 上下反転\n");
	}
	//オプション設定
	else if (USAME_TI(cmd, "OptionDlg")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("GN : 一般\n")
			_T("MO : マウス操作\n")
			_T("DS : デザイン\n")
			_T("TV : テキストビュアー\n")
			_T("IV : イメージビュアー\n")
			_T("ED : エディタ\n")
			_T("AC : 関連付け\n")
			_T("ST : 起動時\n")
			_T("XM : 追加メニュー\n")
			_T("XT : 外部ツール\n")
			_T("KY : キー設定\n")
			_T("NT : 通知・確認・ヒント\n")
			_T("CM : コマンド\n")
			_T("EV : イベント\n"));
	}

	if (!params.IsEmpty()) PrmComboBox->Items->Text = params;

	if (PrmComboBox->Items->Count>0) {
		PrmComboBox->Enabled   = true;
		PrmComboBox->ItemIndex = 0;
	}
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::UnRegCmdCheckBoxClick(TObject *Sender)
{
	SetCmdCombo();
	KeyListBoxClick(KeyListBox);
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyListBoxClick(TObject *Sender)
{
	inh_KeySet++;

	if (!KeyFilterEdit->Text.IsEmpty() || UnRegCmdCheckBox->Checked) {
		KeyFilterEdit->Text = EmptyStr;
		SetCmdCombo();
	}

	TListBox *lp = (TListBox *)Sender;
	int idx = lp->ItemIndex;  if (idx==-1) return;
	UnicodeString kstr = get_tkn_r(lp->Items->Names[idx], ':');
	UnicodeString fstr = lp->Items->ValueFromIndex[idx];

	ShiftCheckBox->Checked = remove_text(kstr, KeyStr_Shift);
	CtrlCheckBox->Checked  = remove_text(kstr, KeyStr_Ctrl);
	AltCheckBox->Checked   = remove_text(kstr, KeyStr_Alt);
	SelCheckBox->Checked   = remove_text(kstr, KeyStr_SELECT);

	if (contains_s(kstr, _T('~'))) {
		//2ストローク
		KeyComboBox->ItemIndex	= KeyComboBox->Items->IndexOf(get_tkn(kstr, '~'));
		Key2ComboBox->ItemIndex = Key2ComboBox->Items->IndexOf(get_tkn_r(kstr, '~'));
	}
	else {
		KeyComboBox->ItemIndex	= KeyComboBox->Items->IndexOf(kstr);
		Key2ComboBox->ItemIndex = 0;
	}

	//コマンド
	UnicodeString cmd = fstr;
	UnicodeString prm;
	if (!starts_Dollar(cmd)) {
		prm = get_PrmStr(cmd);
		cmd = get_CmdStr(cmd);
	}

	TComboBox *cp = CmdComboBox;
	for (int i=0; i<cp->Items->Count; i++) {
		if (StartsText(UAPP_T(cmd, " "), cp->Items->Strings[i])) {
			cp->ItemIndex = i;
			CmdComboBoxChange(cp);
			break;
		}
	}

	//パラメータ
	cp = PrmComboBox;
	if (cp->Enabled) {
		if (cp->Style==csDropDown)
			cp->Text = prm;
		else {
			for (int i=0; i<cp->Items->Count; i++) {
				if (SameText(prm, get_tkn(cp->Items->Strings[i], ' '))) {
					cp->ItemIndex = i; break;
				}
			}
		}
	}

	inh_KeySet--;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyListBoxDblClick(TObject *Sender)
{
	TListBox *lp = (TListBox *)Sender;
	int idx = lp->ItemIndex;  if (idx==-1) return;

	UnicodeString kstr = lp->Items->Names[idx];
	TRegExOptions opt; opt << roIgnoreCase;
	if (TRegEx::IsMatch(kstr, ":(Shift\\+|Ctrl\\+|Alt\\+)*F\\d+", opt)) {
		UnicodeString lbl  = FKeyLabelBuf->Values[kstr];
		UnicodeString tit = "ファンクションキー表示名の変更";
		tit.cat_sprintf(_T(" - %s"), KeyListBuf[KeyTabControl->TabIndex]->Values[kstr].c_str());
		UnicodeString prm;  prm.sprintf(_T("[%s]"), get_tkn_r(kstr, ":").c_str());
		if (input_query_ex(tit.c_str(), prm.c_str(), &lbl)) {
			FKeyLabelBuf->Values[kstr] = lbl;
			lp->Invalidate();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeySetClick(TObject *Sender)
{
	if (inh_KeySet>0) return;

	UnicodeString kbuf = GetCmdKeyStr().UCAT_T("=");

	TListBox *lp = KeyListBox;
	int idx = -1;
	for (int i=0; i<lp->Count; i++) {
		if (StartsText(kbuf, lp->Items->Strings[i])) { idx = i;  break; }
	}

	if (idx!=-1) {
		lp->ItemIndex = idx;
		KeyListBoxClick(lp);
	}
}

//---------------------------------------------------------------------------
//フィルタ
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyFilterEdit_Change(TObject *Sender)
{
	UnicodeString kwd = ((TEdit*)Sender)->Text;
	cursor_Default();
	set_RedrawOff(CmdComboBox);
	{
		CmdComboBox->DroppedDown = false;
		SetCmdCombo(kwd);
		if (!kwd.IsEmpty()) {
			CmdComboBox->DroppedDown = true;
			CmdComboBox->ItemIndex	 = 0;
			CmdComboBoxChange(CmdComboBox);
		}
	}
	set_RedrawOn(CmdComboBox);
	Screen->Cursor = crArrow;	//※ドロップダウン時にカーソルが消える現象への対策
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyFilterEdit_KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);

	if (contained_wd_i(KeysStr_CsrDown, KeyStr)) {
		CmdComboBox->SetFocus();
		perform_Key(CmdComboBox, VK_DOWN);
	}
	else if (equal_ENTER(KeyStr)) {
		//確定
		if (CmdComboBox->DroppedDown) {
			CmdComboBox->SetFocus();
			perform_Key_RETURN(CmdComboBox);
		}
	}
	else if	(equal_ESC(KeyStr) && !KeyFilterEdit->Text.IsEmpty())
		KeyFilterEdit->Text = EmptyStr;
	else if (SameText(KeyStr, KeyStr_Migemo))
		MigemoCheckBox->Checked = !MigemoCheckBox->Checked;
	else return;

	Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyFilterEdit_KeyPress(TObject *Sender, System::WideChar &Key)
{
	if (Key==VK_RETURN || Key==VK_ESCAPE) Key = 0;
}

//---------------------------------------------------------------------------
//キー設定の検索
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::MatchKeyItem(int idx)
{
	bool found = false;
	TListBox *lp = KeyListBox;
	UnicodeString kwd = FindKeyEdit->Text;
	if (!kwd.IsEmpty() && idx>=0 && idx<lp->Count) {
		UnicodeString dsc = get_CmdDesc(lp->Items->ValueFromIndex[idx],
								false, ExtMenuListBox->Items, ExtToolListBox->Items);
		found = ContainsText(dsc, kwd);
	}
	return found;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyEditChange(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	if (!FindKeyEdit->Text.IsEmpty()) {
		int idx = -1;
		for (int i=0; i<lp->Count && idx==-1; i++) if (MatchKeyItem(i)) idx = i;
		lp->ItemIndex = idx;
		if (idx!=-1) KeyListBoxClick(lp);
	}
	else lp->ItemIndex = -1;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (!Shift.Empty()) return;

	if		(Key==VK_UP)	{ FindKeyUpAction->Execute();	Key = 0; }
	else if (Key==VK_DOWN)	{ FindKeyDownAction->Execute();	Key = 0; }
}
//---------------------------------------------------------------------------
//下方向に検索
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyDownActionExecute(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	if (!FindKeyEdit->Text.IsEmpty() && lp->Count>0) {
		int i0 = (lp->ItemIndex!=-1)? lp->ItemIndex : 0;
		if (MatchKeyItem(i0)) i0++;
		int idx = -1;
		for (int i=i0; i<lp->Count && idx==-1; i++) if (MatchKeyItem(i)) idx = i;
		if (idx!=-1) {
			lp->ItemIndex = idx;
			KeyListBoxClick(lp);
		}
		else beep_Warn();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyDownActionUpdate(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	int idx = -1;
	for (int i=lp->Count-1; i>=0 && idx==-1; i--) if (MatchKeyItem(i)) idx = i;
	((TAction*)Sender)->Enabled = (idx!=-1 && idx>lp->ItemIndex);
}
//---------------------------------------------------------------------------
//上方向に検索
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyUpActionExecute(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	if (!FindKeyEdit->Text.IsEmpty() && lp->ItemIndex>0) {
		int i0 = lp->ItemIndex;
		if (MatchKeyItem(i0)) i0--;
		int idx = -1;
		for (int i=i0; i>=0 && idx==-1; i--) if (MatchKeyItem(i)) idx = i;
		if (idx!=-1) {
			lp->ItemIndex = idx;
			KeyListBoxClick(lp);
		}
		else beep_Warn();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyUpActionUpdate(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	int idx = -1;
	for (int i=0; i<lp->Count && idx==-1; i++) if (MatchKeyItem(i)) idx = i;
	((TAction*)Sender)->Enabled = (idx!=-1 && idx<lp->ItemIndex);
}

//---------------------------------------------------------------------------
//キー一覧の描画
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,
		TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);
	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);

	UnicodeString key = lp->Items->Names[Index];
	UnicodeString cmd = lp->Items->ValueFromIndex[Index];

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);

	cv->TextOut(xp, yp, get_tkn_r(key, ":"));
	xp += cv->TextWidth("Shift+Ctrl+Alt+Space ");

	UnicodeString dsc = get_CmdDesc(cmd, true, ExtMenuListBox->Items, ExtToolListBox->Items);
	int w_cmd = get_CharWidth(cv, 40);
	cmd = minimize_str(del_CmdDesc(cmd), cv, w_cmd - 8, true);
	cv->TextOut(xp, yp, cmd);
	xp += w_cmd;
	cv->TextOut(xp, yp, dsc);

	//ファンクションキー名
	UnicodeString lbl = FKeyLabelBuf->Values[key];
	if (!lbl.IsEmpty()) {
		xp = Rect.Left + Rect.Width() - get_CharWidth(cv, 16);
		cv->TextOut(xp, yp, lbl);
	}
}

//---------------------------------------------------------------------------
//キー割当一覧のソート
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SortKeyListBox(UnicodeString item)
{
	TListBox *lp = KeyListBox;
	std::unique_ptr<TStringList> key_lst(new TStringList());
	key_lst->Assign(lp->Items);

	switch (KeySortMode) {
	case  1:	//コマンド
		key_lst->CustomSort(KeyComp_Cmd);	break;
	case  2:	//説明
		cursor_HourGlass();
		key_lst->CustomSort(KeyComp_Dsc);
		cursor_Default();
		break;
	default:	//キー
		key_lst->CustomSort(KeyComp_Key);
	}

	lp->Items->Assign(key_lst.get());
	if (!item.IsEmpty()) {
		int idx = lp->Items->IndexOf(item);
		if (idx!=-1) lp->ItemIndex = idx;
	}

	KeyHeaderControl->Repaint();
}

//---------------------------------------------------------------------------
//キー設定の追加・変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeKeyList(bool add)
{
	if (KeyComboBox->ItemIndex!=-1 && CmdComboBox->ItemIndex!=-1) {
		try {
			UnicodeString ibuf = GetCmdKeyStr();
			UnicodeString cmd  = get_tkn(CmdComboBox->Text, ' ');
			if (PrmComboBox->Enabled && !PrmComboBox->Text.IsEmpty())
				cmd.cat_sprintf(_T("_%s"), get_tkn(PrmComboBox->Text, _T(" : ")).c_str());

			TListBox *lp = KeyListBox;
			//追加
			if (add) {
				ibuf.cat_sprintf(_T("=%s"), cmd.c_str());
				lp->Items->Add(ibuf);
			}
			//変更
			else {
				int kidx = -1;
				TStrings *sp = lp->Items;
				for (int i=0; i<sp->Count; i++) {
					if (SameText(sp->Names[i], ibuf)) { kidx = i;  break; }
				}
				int c_idx = lp->ItemIndex;
		 		if (kidx!=-1 && kidx!=c_idx) UserAbort(USTR_Registered);
				ibuf.cat_sprintf(_T("=%s"), cmd.c_str());
				lp->Items->Strings[c_idx] = ibuf;
			}
			SortKeyListBox(ibuf);

			SetCmdCombo();
			KeySetClick(NULL);
			KeyListBuf[KeyTabControl->TabIndex]->Assign(KeyListBox->Items);
			KeyListBoxClick(lp);
		}
		catch (EAbort &e) {
			msgbox_WARN(e.Message);
		}
	}
}

//---------------------------------------------------------------------------
//パラメータの参照
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefCmdPrmBtnClick(TObject *Sender)
{
	UnicodeString cmd = get_tkn(CmdComboBox->Text, ' ');
	//ディレクトリ
	if (contained_wd_i(_T("ChangeDir|ChangeOppDir|CopyTo|MoveTo|OpenByExp|PlayList|SubDirList"), cmd)) {
		UnicodeString dnam = PrmComboBox->Text;
		if (UserModule->SelectDirEx(_T("パラメータ指定 : ディレクトリ"), dnam)) PrmComboBox->Text = dnam;
	}
	//ファイル
	else {
		if (contained_wd_i(_T("DebugCmdFile|ExeCommands"), cmd)) {
			if (CmdFileListDlg->ShowToSelect()==mrOk) {
				PrmComboBox->Text = "@" + to_relative_name(CmdFileListDlg->CmdFileName);
			}
		}
		else {
			bool is_nwl = USAME_TI(cmd, "LoadWorkList");
			bool is_mnu = contained_wd_i(_T("ExeMenuFile|ContextMenu"), cmd);
			bool is_chm = USAME_TI(cmd, "HelpCurWord");
			bool is_ini = contained_wd_i(_T("Restart|SetColor"), cmd);
			bool is_run = USAME_TI(cmd, "FileRun");

			UserModule->PrepareOpenDlg(_T("パラメータ指定 : ファイル"), NULL,
				(is_nwl? _T("*.nwl") : is_chm? _T("*.chm") : is_ini? _T("*.ini") : _T("*.*")),
				(is_ini? ExePath : RefParamPath));
			UnicodeString fnam;
			if (UserModule->OpenDlgToStr(fnam)) {
				RefParamPath = ExtractFilePath(fnam);
				if (is_run) {
					TStringDynArray plst = split_strings_semicolon(GetEnvironmentVariable("PATH"));
					for (int i=0; i<plst.Length; i++) {
						if (SameText(IncludeTrailingPathDelimiter(plst[i]), RefParamPath)) {
							fnam = ExtractFileName(fnam); break;
						}
					}
				}
				else if (is_nwl || is_mnu || is_ini)
					fnam = to_relative_name(fnam);
				PrmComboBox->Text = fnam;
			}
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::CheckKeyAction(bool ok, bool add)
{
	if (ok && PrmComboBox->Enabled) {
		if (PrmComboBox->Style==csDropDownList)
			ok = (PrmComboBox->ItemIndex!=-1);
		else if (contained_wd_i(_T("ChangeDir|ChangeOppDir|ExeCommands|MaskFind"), get_tkn(CmdComboBox->Text, ' ')))
			ok = !PrmComboBox->Text.IsEmpty();
	}

	//2ストローク開始キーのチェック
	if (ok) ok = !IsFirstCmdKey();

	//既存チェック
	if (add && ok) {
		UnicodeString kbuf = GetCmdKeyStr();
		TStrings *sp = KeyListBox->Items;
		for (int i=0; i<sp->Count; i++)
			if (SameText(sp->Names[i], kbuf)) { ok = false;  break; }
	}

	return ok;
}

//---------------------------------------------------------------------------
//追加
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddKeyActionExecute(TObject *Sender)
{
	ChangeKeyList(true);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddKeyActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled =
		CheckKeyAction(KeyComboBox->ItemIndex!=-1 && CmdComboBox->ItemIndex!=-1, true);
}

//---------------------------------------------------------------------------
//変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgKeyActionExecute(TObject *Sender)
{
	ChangeKeyList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgKeyActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled =
		CheckKeyAction(KeyListBox->ItemIndex!=-1 && KeyComboBox->ItemIndex!=-1 && CmdComboBox->ItemIndex!=-1);
}

//---------------------------------------------------------------------------
//キー設定の削除
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DelKeyActionExecute(TObject *Sender)
{
	delete_ListItem(KeyListBox);
	SetCmdCombo();
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DelKeyActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = (KeyListBox->ItemIndex!=-1);
}

//---------------------------------------------------------------------------
//検索
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindEditChange(TObject *Sender)
{
	TPageControl *pp = PageControl1;
	for (int i=0; i<pp->PageCount; i++) pp->Pages[i]->Tag = 0;
	pp->Repaint();
	FindMarkList->ClearAll();
	FindMarkList->MarkColor = col_OptFind;

	UnicodeString wd = FindEdit->Text;
	if (!wd.IsEmpty()) {
		//タブシート毎に検索
		for (int i=0; i<pp->PageCount; i++) {
			TTabSheet *sp = pp->Pages[i];
			sp->Tag = (FindMarkList->SearchControl((TWinControl*)sp, wd)>0 || ContainsText(sp->Caption, wd))? 1 : 0;
			//見つかったら Tag=1 に
		}
		FindMarkList->ShowMark();
		PageControl1->Repaint();
		//見つかった項目を含むタブをアクティブに
		bool found = false;
		for (int i=0; i<pp->PageCount && !found; i++) {
			if (pp->Pages[i]->Tag==1) {
				pp->ActivePage = pp->Pages[i];
				found = true;
			}
		}
		if (!found) beep_Warn();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::PageControl1Change(TObject *Sender)
{
	HelpContext = PageControl1->ActivePage->HelpContext;

	if (!Sender || PageControl1->ActivePage==KeySetSheet) {
		KeyTabControl->TabIndex =
			LogWndListBox->Focused()? 4 :
			   (ScrMode==SCMD_FLIST)? (CurStt->is_IncSea? 1 : 0) :
			   (ScrMode==SCMD_TVIEW)? 2 :
			   (ScrMode==SCMD_IVIEW)? 3 : 0;

		KeyTabControlChange(KeyTabControl);
		KeyComboBox->ItemIndex = -1;
	}

	if (!Sender || PageControl1->ActivePage==AssoSheet) 	AssociateListBoxClick(NULL);
	if (!Sender || PageControl1->ActivePage==ExtMenuSheet)	ExtMenuListBoxClick(NULL);
	if (!Sender || PageControl1->ActivePage==ExtToolSheet)	ExtToolListBoxClick(NULL);

	UserModule->InitializeListBox(GetCurListBox());
}

//---------------------------------------------------------------------------
//タブの描画 (検索結果の強調表示)
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::PageControl1DrawTab(TCustomTabControl *Control, int TabIndex,
		const TRect &Rect, bool Active)
{
	TTabControl *tp = (TTabControl*)Control;
	//背景
	TCanvas *cv = tp->Canvas;
	cv->Brush->Color = (PageControl1->Pages[TabIndex]->Tag==0)? Color : col_OptFind;
	cv->FillRect(Rect);
	//タイトル
	cv->Font->Color = scl_WindowText;
	UnicodeString tstr = tp->Tabs->Strings[TabIndex];
	cv->TextOut(Rect.Left + (Rect.Width() - cv->TextWidth(tstr))/2, Rect.Top + 4, tstr);
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefIniPatBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("初期パス"), dnam)) {
		dnam = to_path_name(dnam);
		if (((TComponent*)Sender)->Tag==1) R_IniPatEdit->Text = dnam; else L_IniPatEdit->Text = dnam;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::L_IniPatMod1RadioBtnClick(TObject *Sender)
{
	if (R_IniPatMod1RadioBtn->Checked) R_IniPatMod0RadioBtn->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::R_IniPatMod1RadioBtnClick(TObject *Sender)
{
	if (L_IniPatMod1RadioBtn->Checked) L_IniPatMod0RadioBtn->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefHomeWorkBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("ホームワークリストの指定"), F_FILTER_NWL, _T("*.nwl"), WorkListPath);
	UserModule->OpenDlgToEdit(HomeWorkListEdit, true);
}

//---------------------------------------------------------------------------
//配色のインポート
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::InpColBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("配色のインポート"), F_FILTER_INI, _T("*.INI"));
	if (UserModule->OpenDlg->Execute()) {
		std::unique_ptr<TStringList> col_lst(new TStringList());
		std::unique_ptr<TStringList> ext_lst(new TStringList());
		std::unique_ptr<UsrIniFile> inp_file(new UsrIniFile(UserModule->OpenDlg->FileName));
		inp_file->ReadSection("Color", col_lst.get());
		inp_file->LoadListItems("ExtColList",	ext_lst.get());
		if ((col_lst->Count + ext_lst->Count)==0)
			msgbox_WARN("配色設定がありません。");
		else {
			if (col_lst->Count>0) ColBufList->Assign(col_lst.get());
			if (ext_lst->Count>0) ExtColListBox->Items->Assign(ext_lst.get());
			//表示を更新
			ExtColListBox->Repaint();
			OptColListBox->Repaint();
			TimColListBox->Repaint();
		}
	}
}
//---------------------------------------------------------------------------
//配色のエクスポート
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExpColBtnClick(TObject *Sender)
{
	UserModule->PrepareSaveDlg(_T("配色のエクスポート"), F_FILTER_INI, _T("Export_Col.INI"));
	if (UserModule->SaveDlg->Execute()) {
		std::unique_ptr<UsrIniFile> exp_file(new UsrIniFile(UserModule->SaveDlg->FileName));
		exp_file->AssignSection("Color", ColBufList);
		std::unique_ptr<TStringList> ext_lst(new TStringList());
		ext_lst->Assign(ExtColListBox->Items);
		exp_file->SaveListItems("ExtColList", ext_lst.get());
		if (!exp_file->UpdateFile(true)) msgbox_ERR(USTR_FaildSave);
	}
}

//---------------------------------------------------------------------------
//キー設定のインポート
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::InpKeyBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("キー設定のインポート"), F_FILTER_INI, _T("*.INI"));
	if (UserModule->OpenDlg->Execute()) {
		std::unique_ptr<UsrIniFile> inp_file(new UsrIniFile(UserModule->OpenDlg->FileName));
		std::unique_ptr<TStringList> key_lst(new TStringList());
		inp_file->ReadSection("KeyFuncList", key_lst.get());
		if (key_lst->Count>0) {
			for (int i=0; i<MAX_KEYTABS; i++) {
				KeyListBuf[i]->Clear();
				UnicodeString modstr = GetCmdModeStr(i + 1);
				for (int j=0; j<key_lst->Count; j++){
					UnicodeString lbuf = key_lst->Strings[j];
					if (StartsText(modstr, lbuf)) KeyListBuf[i]->Add(lbuf);
				}
			}
			inp_file->ReadSection("FKeyLabelList", FKeyLabelBuf);
			KeyTabControlChange(KeyTabControl);
			//その他のキー設定
			UnicodeString sstr = inp_file->ReadString(SCT_Option, "DlgMoveShift");
			DlgMoveShiftCheckBox->Checked = ContainsText(sstr, KeyStr_Shift);
			DlgMoveCtrlCheckBox->Checked  = ContainsText(sstr, KeyStr_Ctrl);
			DlgMoveAltCheckBox->Checked	  = ContainsText(sstr, KeyStr_Alt);
			sstr = inp_file->ReadString(SCT_Option, "DlgSizeShift");
			DlgSizeShiftCheckBox->Checked = ContainsText(sstr, KeyStr_Shift);
			DlgSizeCtrlCheckBox->Checked  = ContainsText(sstr, KeyStr_Ctrl);
			DlgSizeAltCheckBox->Checked	  = ContainsText(sstr, KeyStr_Alt);
			sstr = inp_file->ReadString(SCT_Option, "IniSeaShift", "F:Ctrl+");
			IniSeaShiftCheckBox->Checked  = ContainsText(sstr, KeyStr_Shift);
			IniSeaCtrlCheckBox->Checked   = ContainsText(sstr, KeyStr_Ctrl);
			IniSeaAltCheckBox->Checked	  = ContainsText(sstr, KeyStr_Alt);
			IniSeaByNumCheckBox->Checked  = inp_file->ReadBool(SCT_Option, "IniSeaByNum");
			IniSeaBySignCheckBox->Checked = inp_file->ReadBool(SCT_Option, "IniSeaBySign");
		}
		else msgbox_WARN("キー設定がありません。");
	}
}
//---------------------------------------------------------------------------
//キー設定のエクスポート
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExpKeyBtnClick(TObject *Sender)
{
	UserModule->PrepareSaveDlg(_T("キー設定のエクスポート"), F_FILTER_INI, _T("Export_Key.INI"));
	if (UserModule->SaveDlg->Execute()) {
		std::unique_ptr<UsrIniFile> exp_file(new UsrIniFile(UserModule->SaveDlg->FileName));
		std::unique_ptr<TStringList> key_lst(new TStringList());
		KeyListBuf[KeyTabControl->TabIndex]->Assign(KeyListBox->Items);
		for (int i=0; i<MAX_KEYTABS; i++) key_lst->AddStrings(KeyListBuf[i]);
		key_lst->CustomSort(KeyComp_Key);
		exp_file->AssignSection("KeyFuncList",	 key_lst.get());
		exp_file->AssignSection("FKeyLabelList", FKeyLabelBuf);
		//その他のキー設定
		UnicodeString sstr;
		if (DlgMoveShiftCheckBox->Checked) sstr += KeyStr_Shift;
		if (DlgMoveCtrlCheckBox->Checked)  sstr += KeyStr_Ctrl;
		if (DlgMoveAltCheckBox->Checked)   sstr += KeyStr_Alt;
		exp_file->WriteString(SCT_Option, "DlgMoveShift", sstr);
		sstr = EmptyStr;
		if (DlgSizeShiftCheckBox->Checked) sstr += KeyStr_Shift;
		if (DlgSizeCtrlCheckBox->Checked)  sstr += KeyStr_Ctrl;
		if (DlgSizeAltCheckBox->Checked)   sstr += KeyStr_Alt;
		exp_file->WriteString(SCT_Option, "DlgSizeShift", sstr);
		sstr.USET_T("F:");
		if (IniSeaShiftCheckBox->Checked) sstr += KeyStr_Shift;
		if (IniSeaCtrlCheckBox->Checked)  sstr += KeyStr_Ctrl;
		if (IniSeaAltCheckBox->Checked)   sstr += KeyStr_Alt;
		exp_file->WriteString(SCT_Option, "IniSeaShift", sstr);
		exp_file->WriteBool(SCT_Option, "IniSeaByNum",	IniSeaByNumCheckBox);
		exp_file->WriteBool(SCT_Option, "IniSeaBySign",	IniSeaBySignCheckBox);
		//書き出し
		if (!exp_file->UpdateFile(true)) msgbox_ERR(USTR_FaildSave);
	}
}
//---------------------------------------------------------------------------
//右寄せ
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AlRightBtnClick(TObject *Sender)
{
	IniWinWdEdit->Text = Screen->Width - EditToInt(IniWinLeftEdit);
	WinSizeChanged = IniWin1RadioBtn->Checked;
}
//---------------------------------------------------------------------------
//最大幅
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::MaxWdBtnClick(TObject *Sender)
{
	IniWinLeftEdit->Text = "0";
	IniWinWdEdit->Text	 = Screen->Width;
	WinSizeChanged = IniWin1RadioBtn->Checked;
}
//---------------------------------------------------------------------------
//最大高
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::MaxHiBtnClick(TObject *Sender)
{
	IniWinTopEdit->Text = "0";
	IniWinHiEdit->Text	= Screen->Height;
	WinSizeChanged = IniWin1RadioBtn->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::IniWinSizeChange(TObject *Sender)
{
	WinSizeChanged = IniWin1RadioBtn->Checked;
}

//---------------------------------------------------------------------------
//OpenStandard コマンドへの関連付け
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::StdCmdListBoxClick(TObject *Sender)
{
	int idx = StdCmdListBox->ItemIndex;  if (idx==-1) return;
	FExtStdEdit->Text = StdCmdListBox->Items->Names[idx];
	UnicodeString cmd = StdCmdListBox->Items->ValueFromIndex[idx];
	if (contains_s(cmd, _T('_'))) {
		StdCmdPrmEdit->Text = get_PrmStr(cmd);
		cmd = get_CmdStr(cmd).UCAT_T("_");
	}

	for (int i=0; i<StdCmdComboBox->Items->Count; i++) {
		if (ContainsText(StdCmdComboBox->Items->Strings[i], cmd)) {
			StdCmdComboBox->ItemIndex = i;  break;
		}
	}

	StdCmdComboBoxChange(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::StdCmdComboBoxChange(TObject *Sender)
{
	bool has_prm = contains_s(StdCmdComboBox->Text, _T("ExeCommands_"));
	StdCmdPrmEdit->Enabled	 = has_prm;
	RefStdCmdPrmBtn->Enabled = has_prm;
	if (!has_prm) StdCmdPrmEdit->Text = EmptyStr;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::StdCmdListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,
		TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);
	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);

	int w_x = 0;
	for (int i=0; i<lp->Count; i++) w_x = std::max(w_x, cv->TextWidth(lp->Items->Names[i]));
	cv->TextOut(xp, yp, lp->Items->Names[Index]);
	cv->TextOut(xp + w_x + 20, yp, lp->Items->ValueFromIndex[Index]);
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefStdCmdPrmBtnClick(TObject *Sender)
{
	if (contains_i(StdCmdComboBox->Text, _T("ExeCommands_"))) {
		if (CmdFileListDlg->ShowToSelect()==mrOk)
			StdCmdPrmEdit->Text = "@" + to_relative_name(CmdFileListDlg->CmdFileName);
	}
}

//---------------------------------------------------------------------------
//標準コマンドの追加・変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeStdCmdList(bool add)
{
	if (FExtStdEdit->Text.IsEmpty() || StdCmdComboBox->Text.IsEmpty()) return;
	UnicodeString fext = Trim(FExtStdEdit->Text.LowerCase());	if (fext.IsEmpty()) return;
	if (fext[1]!='.') fext.Insert(".", 1);
	UnicodeString cmd = get_in_paren(StdCmdComboBox->Text);
	if (contains_s(cmd, _T('_'))) cmd += StdCmdPrmEdit->Text;

	//追加
	if (add) {
		StdCmdListBox->Items->Add(fext.cat_sprintf(_T("=%s"), cmd.c_str()));
	}
	//変更
	else {
		int idx = StdCmdListBox->ItemIndex;  if (idx==-1) return;
		StdCmdListBox->Items->Strings[idx] = fext.cat_sprintf(_T("=%s"), cmd.c_str());
	}
}
//---------------------------------------------------------------------------
//追加
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddStdCmdActionExecute(TObject *Sender)
{
	ChangeStdCmdList(true);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddStdCmdActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !FExtStdEdit->Text.IsEmpty() && !StdCmdComboBox->Text.IsEmpty();
}
//---------------------------------------------------------------------------
//変更
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgStdCmdActionExecute(TObject *Sender)
{
	ChangeStdCmdList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgStdCmdActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled
		 = StdCmdListBox->ItemIndex!=-1 && !FExtStdEdit->Text.IsEmpty() && !StdCmdComboBox->Text.IsEmpty();
}

//---------------------------------------------------------------------------
//イベント
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EventListBoxClick(TObject *Sender)
{
	EventCmdsEdit->Enabled = false;
	EventCmdsEdit->Text    = get_post_tab(EventListBox->Items->Strings[EventListBox->ItemIndex]);
	EventCmdsEdit->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EventCmdsEditChange(TObject *Sender)
{
	if (!EventCmdsEdit->Enabled) return;

	int idx = EventListBox->ItemIndex;
	if (idx!=-1)
		EventListBox->Items->Strings[idx]
			= get_pre_tab(EventListBox->Items->Strings[idx]) + "\t" + EventCmdsEdit->Text;
}

//---------------------------------------------------------------------------
//コマンド/メニューファイルの編集
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EditFileActionExecute(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;
	UnicodeString fnam = get_cmds_prm_file(
		(PageControl1->ActivePage==EventSheet)? ((tag==0)? EventCmdsEdit->Text : (tag==1)? TimerCmd1Edit->Text : TimerCmd2Edit->Text)
											  : MenuPrmEdit->Text);
	if (!fnam.IsEmpty()) open_by_TextEditor(fnam);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EditFileActionUpdate(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;
	UnicodeString fnam = get_cmds_prm_file(
		(PageControl1->ActivePage==EventSheet)? ((tag==0)? EventCmdsEdit->Text : (tag==1)? TimerCmd1Edit->Text : TimerCmd2Edit->Text)
											  : MenuPrmEdit->Text);
	((TAction*)Sender)->Enabled = !fnam.IsEmpty();
}

//---------------------------------------------------------------------------
//削除制限ディレクトリの追加
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddPrtDirActionExecute(TObject *Sender)
{
	UnicodeString dnam;
	if (UserModule->SelectDirEx(_T("作業ディレクトリ"), dnam)) {
		if (PrtDirListBox->Items->IndexOf(dnam)==-1) {
			PrtDirListBox->Items->Add(dnam);
			PrtDirListBox->ItemIndex = PrtDirListBox->Items->IndexOf(dnam);
		}
	}
}

//---------------------------------------------------------------------------
//確定
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OkActionExecute(TObject *Sender)
{
	ModalResult = mrNone;

	//NyanFi ホットキー再登録
	if (IsPrimary) {
		UnicodeString kstr;
		if (!HotKeyComboBox->Text.IsEmpty()) {
			if (HotShiftCheckBox->Checked) kstr += KeyStr_Shift;
			if (HotCtrlCheckBox->Checked)  kstr += KeyStr_Ctrl;
			if (HotAltCheckBox->Checked)   kstr += KeyStr_Alt;
			if (HotWinCheckBox->Checked)   kstr.UCAT_T("Win+");
			kstr += HotKeyComboBox->Text;
		}
		if (register_HotKey(ID_CALL_HOTKEY, kstr))
			CallHotKey = kstr;
		else {
			FindEdit->Text = EmptyStr;
			FindEdit->Text = "呼び出しホットキー";
			msgbox_ERR(GlobalErrMsg);
			return;
		}
		//AppList ホットキー再登録
		kstr = EmptyStr;
		if (!AppKeyComboBox->Text.IsEmpty()) {
			if (AppShiftCheckBox->Checked) kstr += KeyStr_Shift;
			if (AppCtrlCheckBox->Checked)  kstr += KeyStr_Ctrl;
			if (AppAltCheckBox->Checked)   kstr += KeyStr_Alt;
			if (AppWinCheckBox->Checked)   kstr.UCAT_T("Win+");
			kstr += AppKeyComboBox->Text;
		}
		if (register_HotKey(ID_APP_HOTKEY, kstr))
			AppListHotKey = kstr;
		else {
			FindEdit->Text = EmptyStr;
			FindEdit->Text = "AppList";
			msgbox_ERR(GlobalErrMsg);
			return;
		}
	}

	//コントロールのタグにしたがって変更を適用
	ApplyOptionByTag(this);

	//タグを用いない設定
	TempPath		 = to_path_name(TempDirEdit->Text);
	TempPathA		 = rel_to_absdir(TempPath);
	TempPathFTP.sprintf(_T("%s" TMP_FTP_D), TempPathA.c_str());
	MaxTasks		 = MaxTasksComboBox->ItemIndex + 1;
	AppListHotPrm	 = get_word_i_idx(_T("|FA|FL|FI|AO|LO|LI"), AppPrmComboBox->ItemIndex);
	ViewTxtLimitSize = EditToInt(LimitTxtEdit) * 1024;
	ViewBinLimitSize = std::max(StrToInt64Def(LimitBinEdit->Text, 0) * 1048576, 1048576LL);
	GetInfLimitSize  = std::max(EditToInt(LimitRarEdit) * 1048576, 1048576);

	UnicodeString tmp;
	//Migemo 再初期化
	if (!SameText(MigemoPath, MigemoDirEdit->Text)) {
		MigemoPath = to_path_name(MigemoDirEdit->Text);
		delete usr_Migemo;
		usr_Migemo = new MigemoUnit(rel_to_absdir(MigemoPath));
		AddLog(tmp.sprintf(_T("  LOAD migemo.dll   %s"), usr_Migemo->DictReady? _T("OK") : _T("NONE")));
	}
	usr_Migemo->MinLength = IncSeaMigemoMin;

	//Susie プラグインの再初期化
	if (!SameText(SpiDir, SusieDirEdit->Text)) {
		SpiDir = to_path_name(SusieDirEdit->Text);
		delete SPI;
		SPI = new SpiUnit(rel_to_absdir(SpiDir));
		if (SPI->PlgList->Count>0) {
			for (int i=0; i<SPI->PlgList->Count; i++) {
				spi_info *sp = SPI->PlgList->Items[i];
				AddLog(tmp.sprintf(_T("  LOAD %-12s %s"), ExtractFileName(sp->FileName).c_str(), sp->FileType.c_str()));
			}
		}
	}

	//7z.dll対応拡張子
	usr_ARC->FExt7zDll = FExt7zDll;

	EtcEditorList->Assign(EtcEditorListBox->Items);

	ColorList->Assign(ColBufList);
	set_col_from_ColorList();

	ExtColList->Assign(ExtColListBox->Items);
	AssociateList->Assign(AssociateListBox->Items);
	OpenStdCmdList->Assign(StdCmdListBox->Items);
	ProtectDirList->Assign(PrtDirListBox->Items);

	TCheckListBox *lp = ExtMenuListBox;
	for (int i=0; i<lp->Count; i++) {
		TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[i], EXTMENU_CSVITMCNT, true);
		itm_buf[4].sprintf(_T("%s"), lp->Checked[i]? _T("1") : _T("0"));
		lp->Items->Strings[i] = make_csv_rec_str(itm_buf);
	}
	ExtMenuList->Assign(lp->Items);

	lp = ExtToolListBox;
	for (int i=0; i<lp->Count; i++) {
		TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[i], EXTTOOL_CSVITMCNT, true);
		itm_buf[5].sprintf(_T("%s"), lp->Checked[i]? _T("1") : _T("0"));
		lp->Items->Strings[i] = make_csv_rec_str(itm_buf);
	}
	ExtToolList->Assign(lp->Items);

	ViewFoldFitWin = FitFoldRadioBtn->Checked;

	//キー設定
	KeyListBuf[KeyTabControl->TabIndex]->Assign(KeyListBox->Items);
	std::unique_ptr<TStringList> key_lst(new TStringList());
	for (int i=0; i<MAX_KEYTABS; i++) key_lst->AddStrings(KeyListBuf[i]);
	key_lst->CustomSort(KeyComp_Key);
	KeyFuncList->Assign(key_lst.get());
	//ファンクションキーの表示名
	FKeyLabelList->Assign(FKeyLabelBuf);
	int i = 0;
	while (i<FKeyLabelList->Count) {
		if (KeyFuncList->IndexOfName(FKeyLabelList->Names[i])==-1) FKeyLabelList->Delete(i); else i++;
	}

	IniPathMode[0] = L_IniPatMod1RadioBtn->Checked? 1 : L_IniPatMod2RadioBtn->Checked? 2 : 0;
	IniPathMode[1] = R_IniPatMod1RadioBtn->Checked? 1 : R_IniPatMod2RadioBtn->Checked? 2 : 0;
	IniWorkMode    = IniWork1RadioBtn->Checked? 1 : 0;
	IniWinMode	   = IniWin1RadioBtn->Checked? 1 : 0;

	DlgMoveShift = EmptyStr;
	if (DlgMoveShiftCheckBox->Checked) DlgMoveShift += KeyStr_Shift;
	if (DlgMoveCtrlCheckBox->Checked)  DlgMoveShift += KeyStr_Ctrl;
	if (DlgMoveAltCheckBox->Checked)   DlgMoveShift += KeyStr_Alt;
	DlgSizeShift = EmptyStr;
	if (DlgSizeShiftCheckBox->Checked) DlgSizeShift += KeyStr_Shift;
	if (DlgSizeCtrlCheckBox->Checked)  DlgSizeShift += KeyStr_Ctrl;
	if (DlgSizeAltCheckBox->Checked)   DlgSizeShift += KeyStr_Alt;

	//頭文字サーチ
	IniSeaShift.USET_T("F:");
	if (IniSeaShiftCheckBox->Checked) IniSeaShift += KeyStr_Shift;
	if (IniSeaCtrlCheckBox->Checked)  IniSeaShift += KeyStr_Ctrl;
	if (IniSeaAltCheckBox->Checked)   IniSeaShift += KeyStr_Alt;

	//イベント
	for (int i=0; i<MAX_EVENT_CMD; i++)
		 *(EventCmdList[i].sp) = get_post_tab(EventListBox->Items->Strings[i]);

	SetupTimer();

	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OkActionUpdate(TObject *Sender)
{
	HotKeyComboBox->Enabled =
		(HotShiftCheckBox->Checked || HotCtrlCheckBox->Checked || HotAltCheckBox->Checked || HotWinCheckBox->Checked);

	DirBraEdit->Color = ShowIconCheckBox->Checked? col_Invalid : scl_Window;
	DirKetEdit->Color = ShowIconCheckBox->Checked? col_Invalid : scl_Window;

	bool flag = (DlgMoveShiftCheckBox->Checked || DlgMoveCtrlCheckBox->Checked || DlgMoveAltCheckBox->Checked);
	DlgMoveLabel->Font->Color = flag? scl_WindowText : col_Invalid;
	DlgMoveEdit->Color		  = flag? scl_Window	 : col_Invalid;

	flag = (DlgSizeShiftCheckBox->Checked ||DlgSizeCtrlCheckBox->Checked || DlgSizeAltCheckBox->Checked);
	DlgSizeLabel->Font->Color = flag? scl_WindowText : col_Invalid;
	DlgSizeEdit->Color		  = flag? scl_Window	 : col_Invalid;

	InvColIfEmpty(NoWatchPathEdit);
	InvColIfEmpty(NoInfPathEdit);
	InvColIfEmpty(FExtGetInfEdit);
	InvColIfEmpty(FExtNoInfEdit);
	InvColIfEmpty(NoImgPrvPathEdit);
	InvColIfEmpty(FExtImgPrvEdit);
	InvColIfEmpty(FExtNoImgPrvEdit);
	InvColIfEmpty(ExtTxViewerEdit);
	ExtTxViewerFmtEdit->Color = ExtTxViewerEdit->Color;

	HdrStrEdit->Color = MarkdownCheckBox->Checked? col_Invalid : scl_Window;

	SelBaseOnlyCheckBox->Enabled  = SelByMouseCheckBox->Checked;
	SelIconCheckBox->Enabled	  = SelByMouseCheckBox->Checked;

	PrvTxtInfLnEdit->Color		  = PrevTxtToInfCheckBox->Checked? scl_Window : col_Invalid;
	PrevTailCheckBox->Enabled	  = PrevTxtToInfCheckBox->Checked;
	flag = (PrevTxtToInfCheckBox->Checked && PrevTailCheckBox->Checked);
	PrvTxtTailLnEdit->Color		  = flag? scl_Window : col_Invalid;
	PrvActTailLnEdit->Color		  = flag? scl_Window : col_Invalid;

	AppendLogCheckBox->Enabled	  = SaveLogCheckBox->Checked;
	MaxLogFilesEdit->Enabled	  = SaveLogCheckBox->Checked;
	BeepTopEndCheckBox->Enabled   = HintTopEndCheckBox->Checked;
	SeekSwapNPCheckBox->Enabled   = SeekBindDirCheckBox->Checked;

	flag = (EditToInt(FwCntWarnEdit)==0 || EditToInt(FwTmWarnEdit)==0);
	FwCntWarnEdit->Color = flag? col_Invalid : scl_Window;
	FwTmWarnEdit->Color  = flag? col_Invalid : scl_Window;

	flag = (EditToInt(FwCntTaskFinEdit)==0 || EditToInt(FwTmTaskFinEdit)==0);
	FwCntTaskFinEdit->Color = flag? col_Invalid : scl_Window;
	FwTmTaskFinEdit->Color	= flag? col_Invalid : scl_Window;

	EventCmdsEdit->Enabled = (EventListBox->ItemIndex!=-1);
	RefCmdsBtn->Enabled	   = (EventListBox->ItemIndex!=-1);

	InvColIfEmpty(NopDtctTimeEdit);
	InvColIfEmpty(WatchIntervalEdit);
	InvColIfEmpty(FixedLimitEdit);

	ListPercentEdit->Color	 = !KeepOnResizeCheckBox->Checked? col_Invalid : scl_Window;
	FindPathWdEdit->Color	 = !FindPathColCheckBox->Checked? col_Invalid : scl_Window;

	ModalAlphaEdit->Color	 = !ModalScrCheckBox->Checked? col_Invalid : scl_Window;
	ModalColorBox->Color 	 = !ModalScrCheckBox->Checked? col_Invalid : scl_Window;

	RefFontBtn->Enabled		 = (FontComboBox->ItemIndex!=-1);
	RefColBtn->Enabled		 = (OptColListBox->ItemIndex!=-1);
	RefTimColBtn->Enabled	 = (TimColListBox->ItemIndex!=-1);

	if (!UserModule->SpuitEnabled()) {
		SpuitImage->Visible  = (OptColListBox->ItemIndex!=-1);
		SpuitImage2->Visible = (TimColListBox->ItemIndex!=-1);
		SpuitImage3->Visible = !ExtColorEdit->Text.IsEmpty();
	}

	DivDirInfUDCheckBox->Enabled = DivFlistUDCheckBox->Checked;

	int bgmod = BgImgModeComboBox->ItemIndex;
	BgImgSubModeComboBox->Enabled = (bgmod==1 || bgmod==2);
	BgImg1Edit->Color	= (bgmod==0 || bgmod==3)?		col_Invalid : scl_Window;
	BgImg2Edit->Color	= (bgmod!=2)?					col_Invalid : scl_Window;
	HideTimeEdit->Color = !HideScrCheckBox->Checked?	col_Invalid : scl_Window;
	TileIfSzEdit->Color = !TileIfCheckBox->Checked?		col_Invalid : scl_Window;
	InvColIfEmpty(SpImgEdit);
	InvColIfEmpty(SpImg2Edit);

	CloseOtherCheckBox->Enabled = !TaskTrayCheckBox->Checked;
	if (TaskTrayCheckBox->Checked) CloseOtherCheckBox->Checked = true;

	MarkPathEdit->Color = !MarkImgCheckBox->Checked? col_Invalid : scl_Window;
	MarkFextEdit->Color = !MarkImgCheckBox->Checked? col_Invalid : scl_Window;
	MarkMemoEdit->Color = !MarkImgCheckBox->Checked? col_Invalid : scl_Window;
}

//---------------------------------------------------------------------------
//キャンセル処理
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::CanButtonClick(TObject *Sender)
{
	//スポイトのキャンセル
	if (UserModule->SpuitEnabled()) UserModule->EndSpuit();
	//閉じる
	else ModalResult = mrCancel;
}

//---------------------------------------------------------------------------
//ヘルプ
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::FormHelp(WORD Command, NativeInt Data, bool &CallHelp)
{
	if (Command==HELP_CONTEXT || Command==HELP_CONTEXTPOPUP) {
		if (PageControl1->ActivePage==KeySetSheet && (CmdComboBox->Focused() || PrmComboBox->Focused())) {
			UnicodeString topic;

			switch (KeyTabControl->TabIndex) {
			case 0: topic.UCAT_T(HELPTOPIC_FL);		break;	//ファイラー
			case 1: topic.UCAT_T(HELPTOPIC_IS);		break;	//INC.サーチ
			case 2: topic.UCAT_T(HELPTOPIC_TV);		break;	//テキストビュアー
			case 3: topic.UCAT_T(HELPTOPIC_IV);		break;	//イメージビュアー
			case 4: topic.UCAT_T(HELPTOPIC_CILW);	break;	//ログ
			}

			UnicodeString kwd = get_tkn(CmdComboBox->Text, ' ');
			if (!contains_s(topic, _T('#')) && !kwd.IsEmpty() && !starts_Dollar(kwd))
				topic.cat_sprintf(_T("#%s"), kwd.c_str());

			HtmlHelpTopic(topic.c_str());
		}
		else {
			HtmlHelpContext(Data);
		}
		CallHelp = false;
	}
	return true;
}
//---------------------------------------------------------------------------

