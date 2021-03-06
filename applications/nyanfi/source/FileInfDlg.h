//----------------------------------------------------------------------//
// NyanFi																//
//  ファイル情報														//
//----------------------------------------------------------------------//
#ifndef FileInfDlgH
#define FileInfDlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "usr_scrpanel.h"
#include "Global.h"

//---------------------------------------------------------------------------
class TFileInfoDlg : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TAction *CopyAction;
	TAction *CopyInfoAction;
	TAction *OpenUrlAction;
	TAction *PropertyDlgAction;
	TAction *SelAllAction;
	TActionList *ActionList1;
	TListBox *InfListBox;
	TMenuItem *InfEditCopyItem;
	TMenuItem *InfEditSelectAllItem;
	TMenuItem *PopCopyFileInfoItm;
	TMenuItem *PropertyDlgItem;
	TMenuItem *Sep_i_1;
	TMenuItem *Sep_i_2;
	TMenuItem *Sep_i_3;
	TMenuItem *URLW1;
	TPanel *ListPanel;
	TPopupMenu *InfPopupMenu;

	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall InfListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
	void __fastcall InfListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall CopyInfoActionExecute(TObject *Sender);
	void __fastcall CopyActionExecute(TObject *Sender);
	void __fastcall CopyActionUpdate(TObject *Sender);
	void __fastcall SelAllActionExecute(TObject *Sender);
	void __fastcall PropertyDlgActionUpdate(TObject *Sender);
	void __fastcall PropertyDlgActionExecute(TObject *Sender);
	void __fastcall InfListBoxKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall OpenUrlActionExecute(TObject *Sender);
	void __fastcall OpenUrlActionUpdate(TObject *Sender);
	void __fastcall InfListBoxDblClick(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);

private:	// ユーザー宣言
	void __fastcall UpdateInfo();

public:		// ユーザー宣言
	UsrScrollPanel *ListScrPanel;	//シンプルスクロールバー

	file_rec *FileRec;
	UnicodeString CmdStr;
	bool isPlayFile;
	bool isAppInfo;

	__fastcall TFileInfoDlg(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFileInfoDlg *FileInfoDlg;
//---------------------------------------------------------------------------
#endif
