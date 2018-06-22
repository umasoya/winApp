//----------------------------------------------------------------------//
// NyanFi																//
//  タグ設定/検索														//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <algorithm>
#include <memory>
#include <RegularExpressions.hpp>
#include "UserFunc.h"
#include "UserMdl.h"
#include "Global.h"
#include "TagDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TTagManDlg *TagManDlg = NULL;

//---------------------------------------------------------------------------
__fastcall TTagManDlg::TTagManDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::FormCreate(TObject *Sender)
{
	Initialized = false;
	MaxTagWidth = 0;
	InhEdit = false;
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::FormShow(TObject *Sender)
{
	Constraints->MinWidth  = 0;

	IniFile->LoadPosInfo(this, DialogCenter);

	set_StdListBox(TagCheckListBox);
	MaxTagWidth = usr_TAG->IniCheckList(TagCheckListBox);

	set_ButtonMark(HideOptBtn,  UBMK_BDOWN);
	set_ButtonMark(ShowOptBtn,  UBMK_BUP);

	FindOptPanel->Visible = false;
	OptPanel->Height	  = FindOptPanel->Height + InpOptPanel->Height;

	if (SameText(CmdStr, "FindTag")) {
		Caption = "タグ検索";
		HideCheckBox->Checked	 = IniFile->ReadBoolGen(_T("FindTagDlgHide"));
		FindOptPanel->Visible	 = true;
		AndCheckBox->Checked	 = IniFile->ReadBoolGen(_T("FindTagDlgAnd"),	true);
		ResLinkCheckBox->Checked = IniFile->ReadBoolGen(_T("FindTagResLink"));
		OptPanel->Visible		 = IniFile->ReadBoolGen(_T("FindTagShowOpt"),	true);
	}
	else if (SameText(CmdStr, "SetTag")) {
		Caption = "タグの設定";
		HideCheckBox->Checked = IniFile->ReadBoolGen(_T("SetTagDlgHide"));
		OptPanel->Height	  = InpOptPanel->Height;
		OptPanel->Visible	  = IniFile->ReadBoolGen(_T("SetTagShowOpt"),	true);

		TStringDynArray org_lst = split_strings_semicolon(TagEdit->Text);
		for (int i=0; i<org_lst.Length; i++) {
			int idx = TagCheckListBox->Items->IndexOf(org_lst[i]);
			if (idx!=-1) TagCheckListBox->Checked[idx] = true;
		}
	}
	else {
		Caption = "タグの追加";
		HideCheckBox->Checked = IniFile->ReadBoolGen(_T("AddTagDlgHide"));
		OptPanel->Height	  = InpOptPanel->Height;
		OptPanel->Visible	  = IniFile->ReadBoolGen(_T("AddTagShowOpt"),	true);
	}

	ShowTagCountAction->Checked = IniFile->ReadBoolGen(_T("TagDlgShowCount"));

	BlankPanel->Visible   = !OptPanel->Visible;
	Constraints->MinWidth = OptPanel->Visible? 320 : 160;

	InpPanel->Visible = !HideCheckBox->Checked;

	::PostMessage(Handle, WM_FORM_SHOWED, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::WmFormShowed(TMessage &msg)
{
	if (ShowTagCountAction->Checked) {
		Repaint();
		usr_TAG->CountTags(TagCheckListBox);
	}

	TagEdit->Hint = LoadUsrMsg(USTR_HintMltSepSC);

	SetCtrlFocus();
	Initialized = true;
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	IniFile->SavePosInfo(this);

	if (SameText(CmdStr, "FindTag")) {
		IniFile->WriteBoolGen(_T("FindTagDlgHide"),		HideCheckBox);
		IniFile->WriteBoolGen(_T("FindTagDlgAnd"),		AndCheckBox);
		IniFile->WriteBoolGen(_T("FindTagResLink"),		ResLinkCheckBox);
		IniFile->WriteBoolGen(_T("FindTagShowOpt"),		OptPanel->Visible);
	}
	else if (SameText(CmdStr, "SetTag")) {
		IniFile->WriteBoolGen(_T("SetTagDlgHide"),		HideCheckBox);
		IniFile->WriteBoolGen(_T("SetTagShowOpt"),		OptPanel->Visible);
	}
	else {
		IniFile->WriteBoolGen(_T("AddTagDlgHide"),		HideCheckBox);
		IniFile->WriteBoolGen(_T("AddTagShowOpt"),		OptPanel->Visible);
	}

	IniFile->WriteBoolGen(_T("TagDlgShowCount"),	ShowTagCountAction->Checked);

	Initialized = false;
	CmdStr = EmptyStr;
}

//---------------------------------------------------------------------------
void __fastcall TTagManDlg::SetCtrlFocus()
{
	if (InpPanel->Visible) {
		TagEdit->SetFocus();
		TagEdit->SelStart = TagEdit->Text.Length();
	}
	else {
		TagCheckListBox->SetFocus();
		if (TagCheckListBox->Count>0) {
			if (TagCheckListBox->ItemIndex==-1) TagCheckListBox->ItemIndex = 0;
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TTagManDlg::HideCheckBoxClick(TObject *Sender)
{
	InpPanel->Visible = !HideCheckBox->Checked;
	SetCtrlFocus();
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::OptCheckBoxClick(TObject *Sender)
{
	SetCtrlFocus();
}

//---------------------------------------------------------------------------
//オプションの開閉
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::ChgOptBtnClick(TObject *Sender)
{
	OptPanel->Visible	  = !OptPanel->Visible;
	BlankPanel->Visible   = !OptPanel->Visible;
	Constraints->MinWidth = OptPanel->Visible? 320 : 160;
}

//---------------------------------------------------------------------------
//入力欄での操作
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::TagEditChange(TObject *Sender)
{
	if (!Initialized || InhEdit) return;

	TStringDynArray org_lst = split_strings_semicolon(TagEdit->Text);
	std::unique_ptr<TStringList> lst(new TStringList());
	for (int i=0; i<org_lst.Length; i++)
		if (!org_lst[i].IsEmpty()) lst->Add(org_lst[i]);

	for (int i=0; i<TagCheckListBox->Count; i++)
		TagCheckListBox->Checked[i] = (lst->IndexOf(TagCheckListBox->Items->Strings[i])!=-1);

	TagCheckListBoxClickCheck(NULL);
	TagCheckListBox->Invalidate();

	if (InpPanel->Visible) {
		TagEdit->SetFocus();
		TagEdit->SelStart = TagEdit->Text.Length();
	}
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::TagEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	if (KeyStr.IsEmpty()) return;

	TCheckListBox *lp = TagCheckListBox;
	if (contained_wd_i(KeysStr_ToList, KeyStr)) {
		if (lp->Count>0) {
			lp->SetFocus();
			if (lp->ItemIndex==-1) lp->ItemIndex = 0;
		}
	}
	else return;
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::TagEditKeyPress(TObject *Sender, System::WideChar &Key)
{
	if (is_KeyPress_CtrlNotCV(Key) || Key==VK_RETURN || Key==VK_ESCAPE) Key = 0;
}

//---------------------------------------------------------------------------
//タグ一覧での操作
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::TagCheckListBoxClickCheck(TObject *Sender)
{
	if (!Initialized) return;

	UnicodeString lbuf = usr_TAG->CheckToTags(TagCheckListBox, TagEdit->Text);
	if (!SameStr(lbuf, TagEdit->Text)) {
		InhEdit = true;
		TagEdit->Text = lbuf;
		InhEdit = false;
	}
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::TagCheckListBoxClick(TObject *Sender)
{
	TagCheckListBox->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::TagCheckListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	UnicodeString CmdStr = Key_to_CmdF(KeyStr);

	TCheckListBox *lp = TagCheckListBox;

	if		(contained_wd_i(KeysStr_CsrDown, KeyStr))	Key = VK_DOWN;
	else if (contained_wd_i(KeysStr_CsrUp, KeyStr))		Key = VK_UP;
	//全チェック/解除
	else if (lp->Count>0 && USTARTS_TI("SelAll", CmdStr)) {
		int cnt = 0;
		for (int i=0; i<lp->Count; i++) if (lp->Checked[i]) cnt++;
		lp->CheckAll((cnt>0)? cbUnchecked : cbChecked);
		lp->Invalidate();
		TagCheckListBoxClickCheck(NULL);
	}
	//頭文字サーチ
	else if (is_IniSeaKey(KeyStr)) {
		int idx = lp->ItemIndex;
		int idx0=-1, idx1=-1;
		TRegExOptions opt; opt << roIgnoreCase;
		for (int i=0; i<lp->Count && idx1==-1; i++) {
			if (i<=idx && idx0!=-1) continue;
			if (TRegEx::IsMatch(lp->Items->Strings[i], KeyStr, opt)) ((i<=idx)? idx0 : idx1) = i;
		}
		idx = (idx1!=-1)? idx1 : idx0;
		if (idx!=-1) {
			lp->ItemIndex = idx;
			lp->Invalidate();
		}
	}

	if (!is_DialogKey(Key)) Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::TagCheckListBoxKeyPress(TObject *Sender, System::WideChar &Key)
{
	//インクリメンタルサーチを回避
	Key = 0;
}

//---------------------------------------------------------------------------
//一覧の描画
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::TagCheckListBoxDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State)
{
	TCheckListBox *lp = (TCheckListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);
	cv->Brush->Color = lp->Checked[Index]? col_selItem : col_bgList;
	cv->FillRect(Rect);

	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);

	cv->Font->Color = is_SelFgCol(lp->Checked[Index])? col_fgSelItem : col_fgTagNam;
	cv->TextOut(xp, yp, lp->Items->Strings[Index]);

	//使用数
	int n = (int)lp->Items->Objects[Index];
	if (n>0) {
		cv->Font->Color = col_fgList;
		UnicodeString lbuf = n;
		xp += (MaxTagWidth + cv->TextWidth("9999999") - cv->TextWidth(lbuf));
		cv->TextOut(xp, yp, lbuf);
	}

	//カーソル
	if (lp->Focused() && Index==lp->ItemIndex) {
		int lw = std::max(CursorWidth, 1);
		int yp = Rect.Bottom - lw;
		draw_Line(lp->Canvas, Rect.Left, yp, Rect.Right, yp, lw, col_Cursor, psSolid);
	}

	if (State.Contains(odFocused)) lp->Canvas->DrawFocusRect(Rect);
}
//---------------------------------------------------------------------------
//タグ名の変更
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::RenTagActionExecute(TObject *Sender)
{
	int idx = TagCheckListBox->ItemIndex;
	if (idx==-1) return;

	UnicodeString old_tag = TagCheckListBox->Items->Strings[idx];
	UnicodeString new_tag = old_tag;
	if (input_query_ex(_T("タグ名の変更"), _T("タグ名"), &new_tag)
		&& !new_tag.IsEmpty() && !SameStr(old_tag, new_tag))
	{
		int cnt = usr_TAG->RenTag(old_tag, new_tag);
		MaxTagWidth = usr_TAG->IniCheckList(TagCheckListBox, new_tag);
		if (cnt>0) msgbox_OK(UnicodeString().sprintf(_T("%u項目のタグ設定を更新しました。"), cnt));
	}
}
//---------------------------------------------------------------------------
//タグ名の削除
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::DelTagActionExecute(TObject *Sender)
{
	int idx = TagCheckListBox->ItemIndex;
	if (idx==-1) return;

	UnicodeString tag = TagCheckListBox->Items->Strings[idx];
	UnicodeString msg; msg.sprintf(_T("[%s] をタグデータから削除しますか?"), tag.c_str());
	if (msgbox_Sure(msg)) {
		int cnt = usr_TAG->DelTagData(tag);
		MaxTagWidth = usr_TAG->IniCheckList(TagCheckListBox);
		TagEdit->Text = EmptyStr;
		if (cnt>0) msgbox_OK(UnicodeString().sprintf(_T("%u項目のタグを削除しました。"), cnt));
	}
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::SelActionUpdate(TObject *Sender)
{
	((TAction *)Sender)->Enabled = (TagCheckListBox->ItemIndex!=-1);
}

//---------------------------------------------------------------------------
//存在しない項目のデータ削除
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::TrimDataActionExecute(TObject *Sender)
{
	if (msgbox_Sure(LoadUsrMsg(USTR_DeleteQ, _T("存在しない項目のデータ")), SureDelete)) {
		cursor_HourGlass();
		int cnt = usr_TAG->TrimData();
		cursor_Default();

		UnicodeString msg;
		if (cnt>0)
			msg.sprintf(_T("%u 個のデータを削除しました。"), cnt);
		else
			msg.USET_T("存在しない項目のデータはありませんでした。");
		msgbox_OK(msg);
	}
}

//---------------------------------------------------------------------------
//タグの使用数をカウント
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::CountTagActionExecute(TObject *Sender)
{
	usr_TAG->CountTags(TagCheckListBox);
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::CountTagActionUpdate(TObject *Sender)
{
	((TAction *)Sender)->Enabled = (TagCheckListBox->Count>0 && !ShowTagCountAction->Checked);
}

//---------------------------------------------------------------------------
//タグの使用数を表示
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::ShowTagCountActionExecute(TObject *Sender)
{
	TAction *ap = (TAction *)Sender;
	ap->Checked = !ap->Checked;
	usr_TAG->CountTags(TagCheckListBox, !ap->Checked);
}
//---------------------------------------------------------------------------
void __fastcall TTagManDlg::ShowTagCountActionUpdate(TObject *Sender)
{
	((TAction *)Sender)->Enabled = (TagCheckListBox->Count>0);
}

//---------------------------------------------------------------------------
void __fastcall TTagManDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);

	if (equal_ENTER(KeyStr))
		ModalResult = mrOk;
	else if (equal_ESC(KeyStr))
		ModalResult = mrCancel;
	else {
		UnicodeString topic = HELPTOPIC_FL;
		topic.cat_sprintf(_T("#%s"), CmdStr.c_str());
		SpecialKeyProc(this, Key, Shift, topic.c_str());
	}
}
//---------------------------------------------------------------------------

