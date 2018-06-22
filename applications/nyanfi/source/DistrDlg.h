//----------------------------------------------------------------------//
// NyanFi																//
//  振り分けダイアログ													//
//----------------------------------------------------------------------//
#ifndef DistrDlgH
#define DistrDlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.CheckLst.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include "usr_spbar.h"
#include <Vcl.Menus.hpp>

//---------------------------------------------------------------------------
class TDistributionDlg : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TAction *AddRegAction;
	TAction *ChgRegAction;
	TAction *DelRegAction;
	TAction *ExeCopyAction;
	TAction *ExeMoveAction;
	TAction *FindDownAction;
	TAction *FindUpAction;
	TAction *MakeFileAction;
	TActionList *ActionList1;
	TBevel *Bevel1;
	TButton *AddItemBtn;
	TButton *CheckBtn;
	TButton *ChgItemBtn;
	TButton *CopyBtn;
	TButton *DelItemBtn;
	TButton *DowItemBtn;
	TButton *HideCanBtn;
	TButton *MoveBtn;
	TButton *RefDirButton;
	TButton *RefListBtn;
	TButton *UpItemBtn;
	TCheckBox *CreDistrDirCheckBox;
	TCheckListBox *RegListBox;
	TComboBox *SameNameComboBox;
	THeaderControl *PrvListHeader;
	TLabel *Label1;
	TLabeledEdit *DistrDirEdit;
	TLabeledEdit *DistrMaskEdit;
	TLabeledEdit *FindEdit;
	TLabeledEdit *TitleEdit;
	TListBox *PrvListBox;
	TMenuItem *SaveRegItem;
	TPanel *ExePanel;
	TPanel *MainPanel;
	TPanel *PrvPanel;
	TPanel *RegPanel;
	TPopupMenu *PopupMenu1;
	TSpeedButton *FindDownBtn;
	TSpeedButton *FindUpBtn;
	TSplitter *PrvSplitter;
	TStatusBar *StatusBar1;

	void __fastcall ExeCopyActionExecute(TObject *Sender);
	void __fastcall ExeMoveActionExecute(TObject *Sender);
	void __fastcall ExeActionUpdate(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall AddRegActionExecute(TObject *Sender);
	void __fastcall ChgRegActionExecute(TObject *Sender);
	void __fastcall ChgRegActionUpdate(TObject *Sender);
	void __fastcall DelRegActionExecute(TObject *Sender);
	void __fastcall RegListBoxClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall AddRegActionUpdate(TObject *Sender);
	void __fastcall RegListBoxClickCheck(TObject *Sender);
	void __fastcall RegListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall PrvListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall CheckBtnClick(TObject *Sender);
	void __fastcall RegListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall PrvListBoxData(TWinControl *Control, int Index, UnicodeString &Data);
	void __fastcall PrvListHeaderDrawSection(THeaderControl *HeaderControl, THeaderSection *Section,
			const TRect &Rect, bool Pressed);
	void __fastcall PrvListHeaderSectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall PrvListHeaderSectionResize(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall RefDirButtonClick(TObject *Sender);
	void __fastcall FindDownActionExecute(TObject *Sender);
	void __fastcall FindDownActionUpdate(TObject *Sender);
	void __fastcall FindUpActionExecute(TObject *Sender);
	void __fastcall FindUpActionUpdate(TObject *Sender);
	void __fastcall FindEditChange(TObject *Sender);
	void __fastcall FindEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall CreDistrDirCheckBoxClick(TObject *Sender);
	void __fastcall RefListBtnClick(TObject *Sender);
	void __fastcall MakeFileActionExecute(TObject *Sender);
	void __fastcall PrvListBoxDblClick(TObject *Sender);

private:	// ユーザー宣言
	SttProgressBar *SttPrgBar;		//プログレスバー
	int PrvSortMode;
	UnicodeString LastDistDir;
	UnicodeString DistrFile;
	bool RegEnabled;				//登録内容の変更を許可

	void __fastcall WmFormShowed(TMessage &msg);
	bool __fastcall SaveDistrFile();
	void __fastcall UpdatePreview(bool upd = true);
	UnicodeString __fastcall MakeRegItem(int idx = -1);
	bool __fastcall MatchRegItem(int idx);
	bool __fastcall MatchRegMask(UnicodeString mask, UnicodeString nnam);

public:		// ユーザー宣言
	TStringList *ItemList;			// IN 対象リスト
	TStringList *DistrList;			//OUT 振り分けリスト(ファイル名 \t 振り分け先)
	UnicodeString OppPath;			//反対パス
	bool IsMove;					//true=移動/ false=コピー
	bool ImmediateExe;				//確認無しで直ちに実行
	int  SkipCount;					//振り分け先不在によるスキップ数

	__fastcall TDistributionDlg(TComponent* Owner);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_FORM_SHOWED,	TMessage,	WmFormShowed)
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TDistributionDlg *DistributionDlg;
//---------------------------------------------------------------------------
#endif
