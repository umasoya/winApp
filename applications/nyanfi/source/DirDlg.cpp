//----------------------------------------------------------------------//
// NyanFi																//
//  登録ディレクトリ													//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <memory>
#include "UserFunc.h"
#include "UserMdl.h"
#include "Global.h"
#include "MainFrm.h"
#include "DirDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TRegDirDlg *RegDirDlg = NULL;

//---------------------------------------------------------------------------
__fastcall TRegDirDlg::TRegDirDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::FormCreate(TObject *Sender)
{
	ListScrPanel = new UsrScrollPanel(ListPanel, RegDirListBox, USCRPNL_FLAG_P_WP | USCRPNL_FLAG_L_WP);
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::FormShow(TObject *Sender)
{
	IniFile->LoadPosInfo(this, DialogCenter);

	set_ButtonMark(HideOptBtn, UBMK_BDOWN);
	set_ButtonMark(ShowOptBtn, UBMK_BUP);

	MoveTopCheckBox->Checked = IniFile->ReadBoolGen(_T("RegDirDlgMoveTop"));
	OptPanel->Visible		 = IniFile->ReadBoolGen(_T("RegDirDlgShowOpt"),  true);
	BlankPanel->Visible 	 = !OptPanel->Visible;

	if (ScrBarStyle==0)
		set_ControlRBCorner(RegDirListBox, BlankPanel);
	else {
		BlankPanel->Left = ListPanel->ClientWidth  - BlankPanel->Width - (ListScrPanel->VisibleV? ListScrPanel->KnobWidth : 0);
		BlankPanel->Top  = ListPanel->ClientHeight - BlankPanel->Height;
		BlankPanel->BringToFront();
	}

	InitializeListHeader(RegDirHeader, _T("キー|名前|場所"));
	THeaderSections *sp = RegDirHeader->Sections;
	sp->Items[0]->Width = IniFile->ReadInteger("RegDirGrid", "ColWidth0",	40);
	sp->Items[1]->Width = IniFile->ReadInteger("RegDirGrid", "ColWidth1",	200);
	adjust_HeaderSecWidth(RegDirHeader, 2);

	TListBox *lp = RegDirListBox;
	set_StdListBox(lp, LBTAG_OPT_FOCS);
	set_UsrScrPanel(ListScrPanel);

	lp->Items->Assign(RegDirList);
	lp->ItemIndex = IndexOfDir(CurPathName);
	if (lp->ItemIndex==-1) {
		KeyEdit->Text  = EmptyStr;
		DescEdit->Text = EmptyStr;
	}
	DirEdit->Text = CurPathName;
	lp->SetFocus();
	ListScrPanel->UpdateKnob();

	UserModule->InitializeListBox(lp, ListScrPanel);
	SelIndex = -1;
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	UserModule->UninitializeListBox();

	TListBox *lp = RegDirListBox;
	for (int i=0; i<lp->Count; i++) {
		if (is_separator(get_csv_item(lp->Items->Strings[i], 1))) lp->Items->Strings[i] = ",-,";
	}
	RegDirList->Assign(lp->Items);

	//使用項目をセパレータを考慮して先頭へ移動
	if (MoveTopCheckBox->Checked) move_top_RegDirItem(SelIndex);

	IniFile->SavePosInfo(this);
	IniFile->WriteBoolGen(_T("RegDirDlgMoveTop"),	MoveTopCheckBox);
	IniFile->WriteBoolGen(_T("RegDirDlgShowOpt"),	OptPanel->Visible);

	THeaderSections *sp = RegDirHeader->Sections;
	IniFile->WriteInteger("RegDirGrid", "ColWidth0",	sp->Items[0]->Width);
	IniFile->WriteInteger("RegDirGrid", "ColWidth1",	sp->Items[1]->Width);
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::FormDestroy(TObject *Sender)
{
	delete ListScrPanel;
}

//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::FormResize(TObject *Sender)
{
	if (ScrBarStyle==0)
		set_ControlRBCorner(RegDirListBox, BlankPanel);
	else {
		BlankPanel->Left = ListPanel->ClientWidth  - BlankPanel->Width - (ListScrPanel->VisibleV? ListScrPanel->KnobWidth : 0);
		BlankPanel->Top  = ListPanel->ClientHeight - BlankPanel->Height;
		BlankPanel->BringToFront();
	}

	RegDirListBox->Invalidate();
}

//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::RegDirHeaderDrawSection(THeaderControl *HeaderControl,
          THeaderSection *Section, const TRect &Rect, bool Pressed)
{
	draw_SortHeader(HeaderControl, Section, Rect);
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::RegDirHeaderResize(TObject *Sender)
{
	adjust_HeaderSecWidth((THeaderControl *)Sender, 2);
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::RegDirHeaderSectionResize(THeaderControl *HeaderControl, THeaderSection *Section)
{
	adjust_HeaderSecWidth(HeaderControl, 2);
	RegDirListBox->Repaint();
}

//---------------------------------------------------------------------------
int __fastcall TRegDirDlg::IndexOfDir(UnicodeString dnam)
{
	int idx = -1;
	dnam = IncludeTrailingPathDelimiter(dnam);
	TListBox *lp = RegDirListBox;
	for (int i=0; i<lp->Count && idx==-1; i++) {
		if (SameText(get_csv_item(lp->Items->Strings[i], 2), dnam)) idx = i;
	}
	return idx;
}

//---------------------------------------------------------------------------
//キー操作
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::RegDirListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	UnicodeString cmd_F  = Key_to_CmdF(KeyStr);

	TListBox *lp = RegDirListBox;

	bool handled = true;
	if (USAME_TI(KeyStr, "UP") && lp->ItemIndex==0)
		lp->ItemIndex = lp->Count - 1;
	else if (USAME_TI(KeyStr, "DOWN") && lp->ItemIndex==lp->Count-1)
		lp->ItemIndex = 0;
	else if (contained_wd_i(KeysStr_Popup, KeyStr))
		show_PopupMenu(lp);
	else if (UserModule->ListBoxOpeItem(KeyStr))
		;
	//プロパティ
	else if (USAME_TI(cmd_F, "PropertyDlg")) {
		if (lp->ItemIndex!=-1) {
			TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[lp->ItemIndex], 3, true);
			if (!is_separator(itm_buf[1])) {
				pos_ListBoxItem(lp);
				ShowPropertyDialog(itm_buf[2]);
			}
		}
	}
	else handled = false;

	if (handled) Key = 0;

	ListScrPanel->UpdateKnob();
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::RegDirListBoxKeyPress(TObject *Sender, System::WideChar &Key)
{
	UnicodeString jdir;
	bool found = false;
	SelIndex = -1;

	TListBox *lp = (TListBox*)Sender;
	if (Key==VK_RETURN) {
		if (lp->ItemIndex!=-1) {
			TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[lp->ItemIndex], 3, true);
			if (!is_separator(itm_buf[1])) {
				jdir = itm_buf[2];
				SelIndex = lp->ItemIndex;
				found = true;
			}
		}
	}
	else if (!is_KeyDown(VK_CONTROL)) {
		//該当キーの登録数をカウント
		int f_cnt = 0;
		UnicodeString k = Key;
		for (int i=0; i<lp->Count; i++) {
			TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[i], 3, true);
			if (SameText(k, itm_buf[0])) {
				jdir = itm_buf[2];
				SelIndex = i;
				f_cnt++;
			}
		}

		if		(f_cnt==0) beep_Warn();
		else if (f_cnt==1) found = true;
	}

	//確定
	if (found) {
		NyanFiForm->RecoverFileList();
		UnicodeString dnam = get_actual_path(jdir);
		if (is_dir_accessible(dnam)) {
			NyanFiForm->UpdateCurPath(dnam);
			Close();
		}
		else msgbox_ERR(SysErrorMessage(ERROR_ACCESS_DENIED));
	}
}
//---------------------------------------------------------------------------
//マウス操作
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::RegDirListBoxDblClick(TObject *Sender)
{
	WideChar key = VK_RETURN;
	RegDirListBoxKeyPress(Sender, key);
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::RegDirListBoxClick(TObject *Sender)
{
	int idx = RegDirListBox->ItemIndex;  if (idx==-1) return;
	TStringDynArray itm_buf = get_csv_array(RegDirListBox->Items->Strings[idx], 3, true);
	if (is_separator(itm_buf[1])) itm_buf[0] = itm_buf[2] = EmptyStr;
	KeyEdit->Text  = itm_buf[0];
	DescEdit->Text = itm_buf[1];
	DirEdit->Text  = itm_buf[2];
}

//---------------------------------------------------------------------------
//項目の描画
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::RegDirListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(ListFont);

	cv->Brush->Color = col_bgList;
	cv->FillRect(Rect);

	TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[Index], 3, true);
	//セパレータ
	if (is_separator(itm_buf[1]))
		draw_Separator(cv, Rect);
	//項目
	else {
		THeaderSections *sp = RegDirHeader->Sections;
		cv->Font->Color = is_SelFgCol(State)? col_fgSelItem : col_fgList;
		cv->Font->Style = cv->Font->Style << fsBold;
		int s_wd = cv->TextWidth(itm_buf[0]);
		int c_wd = sp->Items[0]->Width - 4;
		int xp = Rect.Left + 2;
		int yp = Rect.Top + get_TopMargin2(cv);
		if (s_wd<c_wd) xp += (c_wd - s_wd)/2;	//センタリング
		cv->TextOut(xp, yp, itm_buf[0]);
		xp = sp->Items[0]->Width + 1;
		cv->Font->Style = cv->Font->Style >> fsBold;
		cv->TextOut(xp, yp, itm_buf[1]);
		xp += sp->Items[1]->Width + 1;
		cv->Font->Color = col_Folder;
		PathNameOut(itm_buf[2], cv, xp, yp, RegDirHeader->Sections->Items[2]->Width);
	}

	//カーソル
	draw_ListCursor2(lp, Rect, Index, State);
}

//---------------------------------------------------------------------------
//項目の追加・変更
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::ChangeItemActionExecute(bool add)
{
	if (add && IndexOfDir(DirEdit->Text)!=-1) {
		msgbox_WARN(USTR_Registered);
		return;
	}

	if (is_separator(DescEdit->Text) || dir_exists(get_actual_path(DirEdit->Text))) {
		UnicodeString lbuf;
		if (is_separator(DescEdit->Text))
			lbuf.USET_T(",-,");
		else {
			lbuf = KeyEdit->Text.SubString(1, 1).UpperCase();
			lbuf.cat_sprintf(_T(",%s,%s"),
				make_csv_str(DescEdit->Text).c_str(),
				make_csv_str(IncludeTrailingPathDelimiter(DirEdit->Text)).c_str());
		}

		TListBox *lp = RegDirListBox;
		if (add) {
			lp->Items->Add(lbuf);
			lp->ItemIndex = lp->Count - 1;
		}
		else {
			if (lp->ItemIndex==-1) return;
			lp->Items->Strings[lp->ItemIndex] = lbuf;
		}

		lp->SetFocus();
		ListScrPanel->UpdateKnob();
	}
	else msgbox_WARN(USTR_DirNotFound);
}

//---------------------------------------------------------------------------
//変更
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::EditItemActionExecute(TObject *Sender)
{
	ChangeItemActionExecute(false);
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::EditItemActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = (RegDirListBox->ItemIndex!=-1);
}
//---------------------------------------------------------------------------
//追加
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::AddItemActionExecute(TObject *Sender)
{
	ChangeItemActionExecute(true);
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::AddItemActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !DescEdit->Text.IsEmpty();
}

//---------------------------------------------------------------------------
//ディレクトリの参照
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::RefDirButtonClick(TObject *Sender)
{
	UnicodeString dnam = DirEdit->Text;
	if (UserModule->SelectDirEx(_T("登録ディレクトリ"), dnam)) DirEdit->Text = to_path_name(dnam);
}

//---------------------------------------------------------------------------
//ディレクトリのプロパティ
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::PropertyActionExecute(TObject *Sender)
{
	TListBox *lp = RegDirListBox;
	if (lp->ItemIndex!=-1) {
		TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[lp->ItemIndex], 3, true);
		if (!is_separator(itm_buf[1])) {
			ShowPropertyDialog(itm_buf[2]);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::PropertyActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = (RegDirListBox->ItemIndex!=-1);
}

//---------------------------------------------------------------------------
//オプションの開閉
//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::ChgOptBtnClick(TObject *Sender)
{
	OptPanel->Visible	= !OptPanel->Visible;
	BlankPanel->Visible = !OptPanel->Visible;
	ListScrPanel->UpdateKnob();

	if (ScrBarStyle==0)
		set_ControlRBCorner(RegDirListBox, BlankPanel);
	else {
		BlankPanel->Left = ListPanel->ClientWidth  - BlankPanel->Width - (ListScrPanel->VisibleV? ListScrPanel->KnobWidth : 0);
		BlankPanel->Top  = ListPanel->ClientHeight - BlankPanel->Height;
		BlankPanel->BringToFront();
	}
}

//---------------------------------------------------------------------------
void __fastcall TRegDirDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	SpecialKeyProc(this, Key, Shift);
}
//---------------------------------------------------------------------------

