//----------------------------------------------------------------------//
// NyanFi																//
//  同名ファイルの比較ダイアログ										//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop

#include "UserFunc.h"
#include "Global.h"
#include "CompDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFileCompDlg *FileCompDlg = NULL;

//---------------------------------------------------------------------------
__fastcall TFileCompDlg::TFileCompDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFileCompDlg::FormCreate(TObject *Sender)
{
	AllDirHasSize = false;
}
//---------------------------------------------------------------------------
void __fastcall TFileCompDlg::FormShow(TObject *Sender)
{
	IniFile->LoadPosInfo(this, DialogCenter);

	TimeRadioGroup->ItemIndex = IniFile->ReadIntGen(_T("CompModeT"));
	SizeRadioGroup->ItemIndex = IniFile->ReadIntGen(_T("CompModeS"));
	HashRadioGroup->ItemIndex = IniFile->ReadIntGen(_T("CompModeH"));
	AlgRadioGroup->ItemIndex  = IniFile->ReadIntGen(_T("CompAlg"));
	CmpDirCheckBox->Checked   = IniFile->ReadBoolGen(_T("CompDir"));
	SelOppCheckBox->Checked   = IniFile->ReadBoolGen(_T("CompSelOpp"));
	ReverseCheckBox->Checked  = IniFile->ReadBoolGen(_T("CompReverse"));
	SelMaskCheckBox->Checked  = IniFile->ReadBoolGen(_T("CompSelMask"));
}
//---------------------------------------------------------------------------
void __fastcall TFileCompDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	IniFile->SavePosInfo(this);

	IniFile->WriteIntGen(_T("CompModeT"),	 TimeRadioGroup);
	IniFile->WriteIntGen(_T("CompModeS"),	 SizeRadioGroup);
	IniFile->WriteIntGen(_T("CompModeH"),	 HashRadioGroup);
	IniFile->WriteIntGen(_T("CompAlg"),		 AlgRadioGroup);
	IniFile->WriteBoolGen(_T("CompDir"),	 CmpDirCheckBox);
	IniFile->WriteBoolGen(_T("CompSelOpp"),	 SelOppCheckBox);
	IniFile->WriteBoolGen(_T("CompReverse"), ReverseCheckBox);
	IniFile->WriteBoolGen(_T("CompSelMask"), SelMaskCheckBox);
}
//---------------------------------------------------------------------------
void __fastcall TFileCompDlg::OkActionUpdate(TObject *Sender)
{
	SizeRadioGroup->Enabled = !CmpDirCheckBox->Checked || AllDirHasSize;
	HashRadioGroup->Enabled = !CmpDirCheckBox->Checked && SizeRadioGroup->ItemIndex==2 && !CurStt->is_FTP && !OppStt->is_FTP;
	AlgRadioGroup->Enabled	= HashRadioGroup->Enabled;

	TimePanel->Color = (TimeRadioGroup->ItemIndex==0)? col_Invalid : scl_BtnFace;
	SizePanel->Color = (!SizeRadioGroup->Enabled || SizeRadioGroup->ItemIndex==0)? col_Invalid : scl_BtnFace;
	HashPanel->Color = (!HashRadioGroup->Enabled || HashRadioGroup->ItemIndex==0)? col_Invalid : scl_BtnFace;

	((TAction*)Sender)->Enabled = true;
}

//---------------------------------------------------------------------------
void __fastcall TFileCompDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	SpecialKeyProc(this, Key, Shift, _T(HELPTOPIC_FL) _T("#CompareDlg"));
}
//---------------------------------------------------------------------------
