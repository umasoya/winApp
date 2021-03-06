//----------------------------------------------------------------------//
// NyanFi																//
//  コマンドライン実行													//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "UserFunc.h"
#include "UserMdl.h"
#include "ExeDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TExeCmdDlg *ExeCmdDlg = NULL;

//---------------------------------------------------------------------------
__fastcall TExeCmdDlg::TExeCmdDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TExeCmdDlg::FormCreate(TObject *Sender)
{
	set_ComboBox_AutoComp(this);

	ExeComboBox->Tag = CBTAG_HISTORY;
}
//---------------------------------------------------------------------------
void __fastcall TExeCmdDlg::FormShow(TObject *Sender)
{
	Constraints->MinHeight = ScaledInt(220);
	Constraints->MaxHeight = ScaledInt(220);
	Constraints->MinWidth  = ScaledInt(500);

	IniFile->LoadPosInfo(this, DialogCenter);
	IniFile->LoadComboBoxItems(ExeComboBox,	_T("CmdLnHistory"));

	LogStdOutCheckBox->Checked	= IniFile->ReadBoolGen(_T("ExeDlgLogStdOut"));
	CopyStdOutCheckBox->Checked = IniFile->ReadBoolGen(_T("ExeDlgCopyStdOut"));
	SaveStdOutCheckBox->Checked = IniFile->ReadBoolGen(_T("ExeDlgSaveStdOut"));
	ListStdOutCheckBox->Checked = IniFile->ReadBoolGen(_T("ExeDlgListStdOut"));
	SaveNameEdit->Text			= IniFile->ReadStrGen(_T("ExeDlgSaveName"));
	RunAsCheckBox->Checked		= IniFile->ReadBoolGen(_T("ExeDlgRunAs"));
	UacDlgCheckBox->Checked		= IniFile->ReadBoolGen(_T("ExeDlgUacDlg"));

	int p = 0;
	//カーソル位置のファイル名を入力
	if (USAME_TI(ActPrm, "FN")) {
		UnicodeString fnam = add_quot_if_spc(FileRec->n_name);
		//実行可能ファイル
		if (test_FileExt(FileRec->f_ext, FExtExeFile)) {
			ExeComboBox->Text = fnam.UCAT_T(" ");
			p = ExeComboBox->Text.Length();
		}
		//その他のファイル
		else {
			ExeComboBox->Text = " " + fnam;
			p = 0;
		}
	}
	ExeComboBox->SetFocus();
	ExeComboBox->SelStart = p;
}
//---------------------------------------------------------------------------
void __fastcall TExeCmdDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	IniFile->SavePosInfo(this);
	IniFile->SaveComboBoxItems(ExeComboBox,	_T("CmdLnHistory"));

	IniFile->WriteBoolGen(_T("ExeDlgLogStdOut"),	LogStdOutCheckBox);
	IniFile->WriteBoolGen(_T("ExeDlgCopyStdOut"),	CopyStdOutCheckBox);
	IniFile->WriteBoolGen(_T("ExeDlgSaveStdOut"),	SaveStdOutCheckBox);
	IniFile->WriteBoolGen(_T("ExeDlgListStdOut"),	ListStdOutCheckBox);
	IniFile->WriteStrGen(_T("ExeDlgSaveName"),		SaveNameEdit);
	IniFile->WriteBoolGen(_T("ExeDlgRunAs"),		RunAsCheckBox);
	IniFile->WriteBoolGen(_T("ExeDlgUacDlg"),		UacDlgCheckBox);
}

//---------------------------------------------------------------------------
//項目の削除
//---------------------------------------------------------------------------
void __fastcall TExeCmdDlg::DelItemActionExecute(TObject *Sender)
{
	del_ComboBox_history(ExeComboBox);
}
//---------------------------------------------------------------------------
void __fastcall TExeCmdDlg::DelItemActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !ExeComboBox->Text.IsEmpty();
}
//---------------------------------------------------------------------------
//履歴のクリア
//---------------------------------------------------------------------------
void __fastcall TExeCmdDlg::ClrHistoryItemClick(TObject *Sender)
{
	ExeComboBox->Clear();
}

//---------------------------------------------------------------------------
//確定
//---------------------------------------------------------------------------
void __fastcall TExeCmdDlg::OkActionExecute(TObject *Sender)
{
	add_ComboBox_history(ExeComboBox, ExeComboBox->Text);
}
//---------------------------------------------------------------------------
void __fastcall TExeCmdDlg::OkActionUpdate(TObject *Sender)
{
	TAction *ap = (TAction*)Sender;
	if (!ExeComboBox->Text.IsEmpty())
		ap->Enabled = SaveStdOutCheckBox->Checked? !SaveNameEdit->Text.IsEmpty() : true;
	else
		ap->Enabled = false;

	for (int i=0; i<OptGroupBox->ControlCount; i++)
		OptGroupBox->Controls[i]->Enabled = !RunAsCheckBox->Checked && !UacDlgCheckBox->Checked;
}

//---------------------------------------------------------------------------
void __fastcall TExeCmdDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	SpecialKeyProc(this, Key, Shift);
}
//---------------------------------------------------------------------------
