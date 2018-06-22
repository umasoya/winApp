//----------------------------------------------------------------------//
// タグ名の検索															//
//																		//
//----------------------------------------------------------------------//
#ifndef FindTagH
#define FindTagH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Menus.hpp>

//---------------------------------------------------------------------------
class TFindTagForm : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TAction *CopyAction;
	TAction *SelAllAction;
	TAction *TagJumpAction;
	TAction *TagViewAction;
	TActionList *ActionList1;
	TListBox *ResListBox;
	TMenuItem *CopyItem;
	TMenuItem *SelAllItem;
	TMenuItem *Sep_1;
	TMenuItem *TagJumpItem;
	TMenuItem *TagViewItem;
	TPaintBox *InpPaintBox;
	TPanel *InpPanel;
	TPanel *ListPanel;
	TPopupMenu *PopupMenu1;

	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall ResListBoxDblClick(TObject *Sender);
	void __fastcall ResListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall ResListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall ResListBoxKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall ResListBoxData(TWinControl *Control, int Index, UnicodeString &Data);
	void __fastcall TagViewActionExecute(TObject *Sender);
	void __fastcall PopupItemUpdate(TObject *Sender);
	void __fastcall TagJumpActionExecute(TObject *Sender);
	void __fastcall CopyActionExecute(TObject *Sender);
	void __fastcall SelAllActionExecute(TObject *Sender);
	void __fastcall InpPaintBoxPaint(TObject *Sender);

private:	// ユーザー宣言
	TStringList *TagList;
	TStringList *ResListBuf;

	UnicodeString IncSeaWord;
	bool IsMigemo;

	void __fastcall UpdateList();

public:		// ユーザー宣言
	UnicodeString TagFile;
	UnicodeString TagName;
	UnicodeString TagCmd;
	UnicodeString FileName;

	UsrScrollPanel *ListScrPanel;	//シンプルスクロールバー

	__fastcall TFindTagForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFindTagForm *FindTagForm;
//---------------------------------------------------------------------------
#endif
