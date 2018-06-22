//----------------------------------------------------------------------//
// NyanFi																//
//  登録ディレクトリ													//
//----------------------------------------------------------------------//
#ifndef DirDlgH
#define DirDlgH

//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include "usr_scrpanel.h"
#include <Vcl.Menus.hpp>

//---------------------------------------------------------------------------
class TRegDirDlg : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TAction *AddItemAction;
	TAction *EditItemAction;
	TAction *PropertyAction;
	TActionList *ActionList1;
	TButton *AddButton;
	TButton *DelButton;
	TButton *DowButton;
	TButton *EditButton;
	TButton *HiddenCanBtn;
	TButton *RefDirButton;
	TButton *UpButton;
	TCheckBox *MoveTopCheckBox;
	TEdit *DescEdit;
	TEdit *DirEdit;
	TEdit *KeyEdit;
	THeaderControl *RegDirHeader;
	TListBox *RegDirListBox;
	TMenuItem *FitSizePosItem;
	TMenuItem *PropertyItem;
	TMenuItem *Sep_1;
	TPanel *BlankPanel;
	TPanel *ListPanel;
	TPanel *OptPanel;
	TPopupMenu *PopupMenu1;
	TSpeedButton *HideOptBtn;
	TSpeedButton *ShowOptBtn;

	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall RegDirHeaderResize(TObject *Sender);
	void __fastcall RegDirHeaderSectionResize(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall RegDirListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall RegDirListBoxClick(TObject *Sender);
	void __fastcall RegDirListBoxDblClick(TObject *Sender);
	void __fastcall RegDirListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall RegDirListBoxKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall EditItemActionExecute(TObject *Sender);
	void __fastcall EditItemActionUpdate(TObject *Sender);
	void __fastcall AddItemActionExecute(TObject *Sender);
	void __fastcall AddItemActionUpdate(TObject *Sender);
	void __fastcall RefDirButtonClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ChgOptBtnClick(TObject *Sender);
	void __fastcall FormResize(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall RegDirHeaderDrawSection(THeaderControl *HeaderControl, THeaderSection *Section,
          const TRect &Rect, bool Pressed);
	void __fastcall PropertyActionExecute(TObject *Sender);
	void __fastcall PropertyActionUpdate(TObject *Sender);

private:	// ユーザー宣言
	int  SelIndex;

	int  __fastcall IndexOfDir(UnicodeString dnam);
	void __fastcall ChangeItemActionExecute(bool add);

public:		// ユーザー宣言
	UsrScrollPanel *ListScrPanel;	//シンプルスクロールバー

	__fastcall TRegDirDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TRegDirDlg *RegDirDlg;
//---------------------------------------------------------------------------
#endif
