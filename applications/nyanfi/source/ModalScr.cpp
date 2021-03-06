//----------------------------------------------------------------------//
// モーダル表示効果用スクリーン											//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop

#include "Global.h"
#include "ModalScr.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TModalScrForm *ModalScrForm;

//---------------------------------------------------------------------------
__fastcall TModalScrForm::TModalScrForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TModalScrForm::FormShow(TObject *Sender)
{
	TRect mon_rc = Application->MainForm->Monitor->BoundsRect;	mon_rc.Inflate(-1, -1);
	TRect app_rc = Application->MainForm->BoundsRect;
	BoundsRect = app_rc.Contains(mon_rc)? mon_rc : app_rc;
}
//---------------------------------------------------------------------------
void __fastcall TModalScrForm::FormActivate(TObject *Sender)
{
	HWND hWnd = get_ModalWnd();
	if (hWnd) ::SetForegroundWindow(hWnd);
}
//---------------------------------------------------------------------------
