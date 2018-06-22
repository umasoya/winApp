//----------------------------------------------------------------------//
// NyanFi																//
//  タグ設定/検索														//
//----------------------------------------------------------------------//
#ifndef TagDlgH
#define TagDlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.CheckLst.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.Buttons.hpp>

//---------------------------------------------------------------------------
class TTagManDlg : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TAction *CountTagAction;
	TAction *DelTagAction;
	TAction *RenTagAction;
	TAction *TrimDataAction;
	TActionList *ActionList1;
	TButton *CanButton;
	TButton *OkButton;
	TCheckBox *AndCheckBox;
	TCheckBox *HideCheckBox;
	TCheckBox *ResLinkCheckBox;
	TCheckListBox *TagCheckListBox;
	TEdit *TagEdit;
	TMenuItem *CountTagItem;
	TMenuItem *DelTagItem;
	TMenuItem *RenTagItem;
	TMenuItem *Sep_1;
	TMenuItem *TrimDataItem;
	TPanel *BlankPanel;
	TPanel *FindOptPanel;
	TPanel *InpOptPanel;
	TPanel *InpPanel;
	TPanel *ListPanel;
	TPanel *OptPanel;
	TPopupMenu *PopupMenu1;
	TSpeedButton *HideOptBtn;
	TSpeedButton *ShowOptBtn;
	TAction *ShowTagCountAction;
	TMenuItem *U1;
	TMenuItem *Sep_2;

	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall TagCheckListBoxClickCheck(TObject *Sender);
	void __fastcall TagEditChange(TObject *Sender);
	void __fastcall RenTagActionExecute(TObject *Sender);
	void __fastcall SelActionUpdate(TObject *Sender);
	void __fastcall TrimDataActionExecute(TObject *Sender);
	void __fastcall DelTagActionExecute(TObject *Sender);
	void __fastcall TagCheckListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall CountTagActionExecute(TObject *Sender);
	void __fastcall CountTagActionUpdate(TObject *Sender);
	void __fastcall HideCheckBoxClick(TObject *Sender);
	void __fastcall TagCheckListBoxClick(TObject *Sender);
	void __fastcall TagEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall TagEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall TagCheckListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall OptCheckBoxClick(TObject *Sender);
	void __fastcall TagCheckListBoxKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall ChgOptBtnClick(TObject *Sender);
	void __fastcall ShowTagCountActionExecute(TObject *Sender);
	void __fastcall ShowTagCountActionUpdate(TObject *Sender);

private:	// ユーザー宣言
	bool Initialized;
	int  MaxTagWidth;
	bool InhEdit;

	void __fastcall WmFormShowed(TMessage &msg);
	void __fastcall SetCtrlFocus();

public:		// ユーザー宣言
	UnicodeString CmdStr;

	__fastcall TTagManDlg(TComponent* Owner);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_FORM_SHOWED,	TMessage,	WmFormShowed)
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TTagManDlg *TagManDlg;
//---------------------------------------------------------------------------
#endif
