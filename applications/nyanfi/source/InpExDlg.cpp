//----------------------------------------------------------------------//
// NyanFi																//
//  拡張入力ボックス													//
//  カレントリストの中央に表示											//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <algorithm>
#include "UserFunc.h"
#include "UserMdl.h"
#include "InpExDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TInputExDlg *InputExDlg;

//---------------------------------------------------------------------------
__fastcall TInputExDlg::TInputExDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TInputExDlg::FormCreate(TObject *Sender)
{
	CustomWidth = 0;

	for (int i=0; i<MAX_SAVE_CODEPAGES; i++) CodePageComboBox->Items->Add(SaveCodePages[i].name);
	CodePageComboBox->ItemIndex = 0;
}
//---------------------------------------------------------------------------
void __fastcall TInputExDlg::FormShow(TObject *Sender)
{
	HelpTopic = EmptyStr;

	int c_wd = ScaledInt((CustomWidth>0)? CustomWidth : InputEdit->NumbersOnly? 260 : 400);
	CustomWidth = 0;
	int l_wd = Canvas->TextWidth(InputEdit->EditLabel->Caption);
	ClientWidth = std::max(c_wd, l_wd + ScaledInt(70));
	InputEdit->Left  = l_wd + ScaledInt(12);
	InputEdit->Width = ClientWidth - InputEdit->Left - ScaledInt(8);

	//表示位置を調整
	TPoint p;
	TRect tmp_rc = BoundsRect; tmp_rc.SetHeight(ScaledInt(120));	//***
	if (Screen->ActiveForm==Application->MainForm && USTARTS_TI("ファンクションキー", Caption)) {
		int idx = (extract_int_def(InputEdit->EditLabel->Caption, 1) - 1) / 4;
		TControl *cp = Application->MainForm;
		p = cp->ClientToScreen(cp->ClientRect.CenteredRect(tmp_rc).Location);
		p.y += cp->ClientHeight/4;
		if		(idx==0) p.x -= cp->ClientWidth/3;
		else if (idx==2) p.x += cp->ClientWidth/3;
	}
	else {
		TControl *cp = (Screen->ActiveForm!=Application->MainForm)? (TControl*)Screen->ActiveForm :
										 	 (ScrMode==SCMD_FLIST)? (TControl*)FileListBox[CurListTag] :
																	(TControl*)Application->MainForm;
		p = cp->ClientToScreen(cp->ClientRect.CenteredRect(tmp_rc).Location);
	}

	TRect main_rc = Application->MainForm->BoundsRect;
	if (p.x<main_rc.Left)			 p.x = main_rc.Left + 8;
	if ((p.x + Width)>main_rc.Right) p.x = main_rc.Right - Width - 8;

	Left = p.x;
	Top  = p.y;

	InputEdit->SetFocus();

	NewTextPanel->Visible  = false;
	CreDirPanel->Visible   = false;
	CreTestPanel->Visible  = false;
	IniSttPanel->Visible   = false;
	NotationPanel->Visible = false;

	is_Clone  = USAME_TI(Caption, "クローン作成");
	is_CreDir = SameText(Caption, LoadUsrMsg(USTR_CreateDir));

	int hi = InputPanel->Height;
	if (USAME_TI(Caption, "新規テキストの作成")) {
		CodePageComboBox->ItemIndex = IniFile->ReadIntGen(_T("NewTextCodePage"));
		ClipCheckBox->Checked = IniFile->ReadBoolGen(_T("NewTextClip"));
		EditCheckBox->Checked = EditNewText;
		NewTextPanel->Visible = true;
		hi += NewTextPanel->Height;
		HelpTopic.cat_sprintf(_T("%s"), _T(HELPTOPIC_FL) _T("#NewTextFile"));
	}
	else if (is_CreDir) {
		DirChgCheckBox->Checked = CreDirChg;
		CnvChCheckBox->Checked	= CreDirCnvChar;
		CreDirPanel->Visible	= true;
		hi += CreDirPanel->Height;
		if (!InputEdit->Text.IsEmpty()) {
			SelDefCheckBox->Checked = IniFile->ReadBoolGen(_T("CreDirIniSel"));
			IniSttPanel->Visible	= true;
		}
		HelpTopic.cat_sprintf(_T("%s"), _T(HELPTOPIC_FL) _T("#CreateDir"));
	}
	else if (is_Clone) {
		SelDefCheckBox->Checked = IniFile->ReadBoolGen(_T("CloneIniSel"));
		IniSttPanel->Visible	= true;
		HelpTopic.cat_sprintf(_T("%s"), _T(HELPTOPIC_FL) _T("#Clone"));
	}
	else if (USAME_TI(Caption, "テストファイルの作成")) {
		InputEdit->Text    = IniFile->ReadStrGen(_T("CreTestFileName"));
		TestSizeEdit->Text = IniFile->ReadStrGen(_T("CreTestFileSize"), "1M");
		TestCntEdit->Text  = "1";
		CreTestPanel->Visible = true;
		hi += CreTestPanel->Height;
		HelpTopic.cat_sprintf(_T("%s"), _T(HELPTOPIC_FL) _T("#CreateTestFile"));
	}
	else if (contains_s(Caption, _T("アドレス"))) {
		if (IniFile->ReadBoolGen(_T("HexAddress"), true)) HexRadioBtn->Checked = true; else DecRadioBtn->Checked = true;
		NotationPanel->Visible = true;
	}

	hi += BottomPanel->Height;
	ClientHeight = hi;

	::PostMessage(Handle, WM_FORM_SHOWED, 0, 0);
}

//---------------------------------------------------------------------------
void __fastcall TInputExDlg::WmFormShowed(TMessage &msg)
{
	if (!InputEdit->Text.IsEmpty()) {
		if (SameText(Caption, LoadUsrMsg(USTR_Rename))) {
			int p = pos_r(_T("."), InputEdit->Text) - 1;
			if (p<0) p = InputEdit->Text.Length();
			switch (IniFile->ReadIntGen(_T("RenameDlgIniStt"))) {
			case 1:
				InputEdit->SelStart = 0;
				break;
			case 2:
				InputEdit->SelStart  = 0;
				InputEdit->SelLength = p;
				break;
			case 3:
				InputEdit->SelectAll();
				break;
			default:
				InputEdit->SelStart = p;
			}
		}
		else {
			if (IniSttPanel->Visible && SelDefCheckBox->Checked)
				InputEdit->SelectAll();
			else
				InputEdit->SelStart = InputEdit->Text.Length();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TInputExDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	CloseIME(Handle);

	if (NewTextPanel->Visible) {
		IniFile->WriteIntGen(_T("NewTextCodePage"),	CodePageComboBox);
		IniFile->WriteBoolGen(_T("NewTextClip"),	ClipCheckBox);
		EditNewText = EditCheckBox->Checked;
	}
	else if (CreDirPanel->Visible) {
		CreDirChg	  = DirChgCheckBox->Checked;
		CreDirCnvChar = CnvChCheckBox->Checked;
	}
	else if (CreTestPanel->Visible) {
		IniFile->WriteStrGen(_T("CreTestFileName"),	InputEdit->Text);
		IniFile->WriteStrGen(_T("CreTestFileSize"),	TestSizeEdit->Text);
	}
	else if (NotationPanel->Visible) {
		IniFile->WriteBoolGen(_T("HexAddress"),		HexRadioBtn->Checked);
		//16進の場合、0x を付加
		if (HexRadioBtn->Checked) {
			UnicodeString lbuf = InputEdit->Text;
			if (!lbuf.IsEmpty()) {
				lbuf.Insert("0x", starts_tchs(_T("+-"), lbuf)? 2 : 1);
				InputEdit->Text = lbuf;
			}
		}
	}

	if (IniSttPanel->Visible) {
		if (is_Clone)
			IniFile->WriteBoolGen(_T("CloneIniSel"),	SelDefCheckBox);
		else
			IniFile->WriteBoolGen(_T("CreDirIniSel"),	SelDefCheckBox);
	}
}

//---------------------------------------------------------------------------
void __fastcall TInputExDlg::OptionClick(TObject *Sender)
{
	InputEdit->SetFocus();
}
//---------------------------------------------------------------------------
void __fastcall TInputExDlg::SelDefCheckBoxClick(TObject *Sender)
{
	InputEdit->SetFocus();

	if (SelDefCheckBox->Checked)
		InputEdit->SelectAll();
	else
		InputEdit->SelStart = InputEdit->Text.Length();
}

//---------------------------------------------------------------------------
void __fastcall TInputExDlg::InputEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	SpecialEditProc(Sender, Key, Shift);
}
//---------------------------------------------------------------------------
void __fastcall TInputExDlg::InputEditKeyPress(TObject *Sender, System::WideChar &Key)
{
	if (is_KeyPress_CtrlNotCV(Key)) Key = 0;
}

//---------------------------------------------------------------------------
void __fastcall TInputExDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (equal_F1(get_KeyStr(Key, Shift)) && !HelpTopic.IsEmpty()) {
		HtmlHelpTopic(HelpTopic.c_str());
		Key = 0;
	}
}
//---------------------------------------------------------------------------

