//----------------------------------------------------------------------//
// NyanFi																//
//  汎用一覧ダイアログ													//
//----------------------------------------------------------------------//
#ifndef GenInfDlgH
#define GenInfDlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtDlgs.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.AppEvnts.hpp>
#include "usr_scrpanel.h"
#include "Global.h"

//---------------------------------------------------------------------------
class TGeneralInfoDlg : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TAction *CopyAction;
	TAction *DelDuplAction;
	TAction *EditFileAction;
	TAction *ErrOnlyAction;
	TAction *FileName1stAction;
	TAction *KeepIndexAction;
	TAction *OmitComPathAction;
	TAction *OrgOrderAction;
	TAction *ResoreListAction;
	TAction *SaveAsAction;
	TAction *SelAllAction;
	TAction *ShowFileInfoAction;
	TAction *ShowLineNoAction;
	TAction *SortAscAction;
	TAction *SortDesAction;
	TAction *ViewFileAction;
	TAction *ViewListAction;
	TActionList *ActionList1;
	TCheckBox *HighlightCheckBox;
	TCheckBox *MigemoCheckBox;
	TCheckBox *NotifyCheckBox;
	TCheckBox *WatchCheckBox;
	TLabeledEdit *FilterEdit;
	TListBox *GenListBox;
	TMenuItem *D1;
	TMenuItem *EditFileItem;
	TMenuItem *ErrOnlyItem;
	TMenuItem *FileName1stItem;
	TMenuItem *FindTextItem;
	TMenuItem *FitSizePosItem;
	TMenuItem *InfEditCopyItem;
	TMenuItem *InfEditSelectAllItem;
	TMenuItem *KeepIndexItem;
	TMenuItem *OmitComPathItem;
	TMenuItem *OrgOrderItem;
	TMenuItem *R1;
	TMenuItem *SaveAsItem;
	TMenuItem *Sep_1;
	TMenuItem *Sep_2;
	TMenuItem *Sep_3;
	TMenuItem *Sep_4;
	TMenuItem *Sep_5;
	TMenuItem *ShowFileInfoItem;
	TMenuItem *ShowLineNoItem;
	TMenuItem *SortAscItem;
	TMenuItem *SortDesItem;
	TMenuItem *SortItem;
	TMenuItem *ViewFileItem;
	TMenuItem *ViewListItem;
	TPanel *FilterPanel;
	TPanel *ListPanel;
	TPanel *MianPanel;
	TPanel *OpePanel;
	TPanel *TailPanel;
	TPopupMenu *InfPopupMenu;
	TStatusBar *StatusBar1;
	TTimer *Timer1;

	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall GenListBoxData(TWinControl *Control, int Index, UnicodeString &Data);
	void __fastcall GenListBoxDataObject(TWinControl *Control, int Index, TObject *&DataObject);
	void __fastcall GenListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall GenListBoxClick(TObject *Sender);
	void __fastcall GenListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall GenListBoxKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall StatusBar1DrawPanel(TStatusBar *StatusBar, TStatusPanel *Panel, const TRect &Rect);
	void __fastcall CopyActionExecute(TObject *Sender);
	void __fastcall CopyActionUpdate(TObject *Sender);
	void __fastcall SelAllActionExecute(TObject *Sender);
	void __fastcall SaveAsActionExecute(TObject *Sender);
	void __fastcall FilterEditChange(TObject *Sender);
	void __fastcall FilterEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FilterEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall FilterEditEnter(TObject *Sender);
	void __fastcall FilterEditExit(TObject *Sender);
	void __fastcall MigemoCheckBoxClick(TObject *Sender);
	void __fastcall HighlightCheckBoxClick(TObject *Sender);
	void __fastcall ViewListActionExecute(TObject *Sender);
	void __fastcall ShowLineNoActionExecute(TObject *Sender);
	void __fastcall ShowLineNoActionUpdate(TObject *Sender);
	void __fastcall ViewFileActionExecute(TObject *Sender);
	void __fastcall ViewListActionUpdate(TObject *Sender);
	void __fastcall EditFileActionExecute(TObject *Sender);
	void __fastcall OpenFileActionUpdate(TObject *Sender);
	void __fastcall ErrOnlyActionExecute(TObject *Sender);
	void __fastcall ErrOnlyActionUpdate(TObject *Sender);
	void __fastcall GenListBoxDblClick(TObject *Sender);
	void __fastcall GenListBoxExit(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall WatchCheckBoxClick(TObject *Sender);
	void __fastcall FindTextItemClick(TObject *Sender);
	void __fastcall SortAscActionExecute(TObject *Sender);
	void __fastcall ListActionUpdate(TObject *Sender);
	void __fastcall SortDesActionExecute(TObject *Sender);
	void __fastcall OrgOrderActionExecute(TObject *Sender);
	void __fastcall DelDuplActionExecute(TObject *Sender);
	void __fastcall ResoreListActionExecute(TObject *Sender);
	void __fastcall ShowFileInfoActionExecute(TObject *Sender);
	void __fastcall ShowFileInfoActionUpdate(TObject *Sender);
	void __fastcall KeepIndexActionExecute(TObject *Sender);
	void __fastcall KeepIndexActionUpdate(TObject *Sender);
	void __fastcall FileName1stActionExecute(TObject *Sender);
	void __fastcall FileName1stActionUpdate(TObject *Sender);
	void __fastcall OmitComPathActionExecute(TObject *Sender);
	void __fastcall OmitComPathActionUpdate(TObject *Sender);

private:	// ユーザー宣言
	int  MaxNameWidth;
	int  TabWidth;
	UnicodeString SavePath;
	bool isFiltered;
	bool show_LineNo;
	bool keep_Index;
	int  LastIndex;
	int  LastTopIndex;

	UnicodeString FirstKey;		//2ストローク開始キー
	bool Wait2ndKey;			//2ストローク目待ち状態

	__int64   FileSize;
	TDateTime FileTime;

	int  ListWidth;				//リスト幅
	int  SortMode;				//ソートモード 1:昇順/ -1:降順/ 0:解除
	bool Found;					//検索結果

	void __fastcall WmFormShowed(TMessage &msg);
	void __fastcall WmMenuChar(TMessage &msg);

	TWndMethod org_GenListWndProc;
	void __fastcall GenListWndProc(TMessage &msg);

	bool __fastcall UpdateList(bool reload = false);
	void __fastcall WmNyanFiPlayList(TMessage &msg);

	UnicodeString __fastcall TwoStrokeSeq(WORD &Key, TShiftState Shift);
	void __fastcall CancelKeySeq();

	void __fastcall SortGenList(int mode);

public:		// ユーザー宣言
	UsrScrollPanel *ListScrPanel;	//シンプルスクロールバー

	bool isVarList;		//「名前=値」形式
	bool isLog;			//ログ
	bool isFileList;	//ファイルスト
	bool isPlayList;	//プレイリスト
	bool isDirs;		//ディレクト一覧
	bool isFTP;			//FTP
	bool isTail;		//末尾表示
	bool isReverse;		//逆順表示
	bool ErrOnly;		//エラー部分のみ(ログ)
	bool ToEnd;			//最後に移動
	bool ToFilter;		//フィルタ欄にフォーカス

	bool FileName1st;	//ファイル名を先に表示

	bool OmitComPath;	//パスの共通部分を省略
	UnicodeString ComPathName;

	UnicodeString FileName;
	UnicodeString RetStr;
	int TailLine;				//末尾表示行数
	int CodePage;				//コードページ
	bool HasBOM;				//BOM
	UnicodeString LineBreakStr;	//改行コード
	UnicodeString FindWord;		//検索文字列
	UnicodeString RegExPtn;		//正規表現パターン
	UnicodeString HdrLnStr;		//見出し行開始文字列(/〜/ の場合正規表現)
	UnicodeString ErrMsg;

	TStringList *GenInfoList;
	TStringList *GenInfoBuff;
	TStringList *GenSelList;

	__fastcall TGeneralInfoDlg(TComponent* Owner);
	void __fastcall SetStatusBar(UnicodeString msg = EmptyStr);

	bool __fastcall FindText(bool down);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_FORM_SHOWED,		TMessage,	WmFormShowed)
		VCL_MESSAGE_HANDLER(WM_MENUCHAR,		TMessage,	WmMenuChar)
		VCL_MESSAGE_HANDLER(WM_NYANFI_PLAYLIST, TMessage,	WmNyanFiPlayList)
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TGeneralInfoDlg *GeneralInfoDlg;
//---------------------------------------------------------------------------
#endif
