//----------------------------------------------------------------------//
// ƒo[ƒWƒ‡ƒ“î•ñ														//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "Global.h"
#include "About.h"

//---------------------------------------------------------------------
#pragma resource "*.dfm"
TAboutBox *AboutBox = NULL;

//---------------------------------------------------------------------
__fastcall TAboutBox::TAboutBox(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------------
void __fastcall TAboutBox::FormCreate(TObject *Sender)
{
	ProgramIcon->Picture->Assign(Application->Icon);
}
//---------------------------------------------------------------------------
void __fastcall TAboutBox::SupportURLClick(TObject *Sender)
{
	Execute_ex(SupportURL->Caption);
}
//---------------------------------------------------------------------------
