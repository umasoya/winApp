//----------------------------------------------------------------------//
// NyanFi																//
//  �f�B���N�g������/ �X�^�b�N											//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <memory>
#include <RegularExpressions.hpp>
#include "UserFunc.h"
#include "UserMdl.h"
#include "Global.h"
#include "MainFrm.h"
#include "HistDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDirHistoryDlg *DirHistoryDlg = NULL;

//---------------------------------------------------------------------------
__fastcall TDirHistoryDlg::TDirHistoryDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::FormCreate(TObject *Sender)
{
	ListBuff = new TStringList();

	ListScrPanel = new UsrScrollPanel(ListPanel, DirHistListBox, USCRPNL_FLAG_P_WP | USCRPNL_FLAG_L_WP);

	IsDirStack = IsAllDirHist = IsFindDirHist = false;
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::FormShow(TObject *Sender)
{
	IniFile->LoadPosInfo(this, DialogCenter);
	IsMigemo = IniFile->ReadBoolGen(_T("DirHistoryMigemo"));

	RefListTag	 = CurListTag;
	WorkListName = EmptyStr;
	IncSeaWord	 = EmptyStr;

	FindBusy = FindAborted = false;
	HintPanel->Visible = false;

	InpPanel->Font->Assign(ListFont);
	InpPanel->ClientHeight = get_FontHeight(ListFont, 4, 4);
	InpPanel->Color 	   = col_bgList;
	InpPanel->Visible	   = IsFindDirHist;

	set_StdListBox(DirHistListBox);
	set_UsrScrPanel(ListScrPanel);

	int *h_ptr = (IsDirStack || IsAllDirHist)? NULL : get_DirHistPtr(CurTabIndex, CurListTag);
	UpdateListBox(h_ptr? *h_ptr : 0);
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (FindBusy) FindAborted = true;

	IsDirStack = IsAllDirHist = IsFindDirHist = false;

	IniFile->SavePosInfo(this);
	IniFile->WriteBoolGen(_T("DirHistoryMigemo"), IsMigemo);
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::FormDestroy(TObject *Sender)
{
	delete ListBuff;
	delete ListScrPanel;
}

//---------------------------------------------------------------------------
//�ꗗ�̍X�V
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::UpdateListBox(
	int idx)	//�C���f�b�N�X	(default = 0)
{
	if (IsFindDirHist) InpPaintBox->Repaint();

	ListBuff->Clear();

	//�t�B���^�������[�h
	if (IsFindDirHist) {
		std::unique_ptr<TStringList> ptn_lst(new TStringList());
		TStringDynArray wd_lst = SplitString(slash_to_yen(Trim(IncSeaWord)), " ");
		for (int i=0; i<wd_lst.Length; i++) {
			UnicodeString ptn = IsMigemo? usr_Migemo->GetRegExPtn(true, wd_lst[i]) : wd_lst[i];
			if (!ptn.IsEmpty()) ptn_lst->Add(ptn);
		}

		if (ptn_lst->Count>0) {
			std::unique_ptr<TStringList> f_lst(new TStringList());
			TRegExOptions opt; opt << roIgnoreCase;
			try {
				for (int i=0; i<AllDirHistory->Count; i++) {
					UnicodeString lbuf = AllDirHistory->Strings[i];
					UnicodeString fnam = get_csv_item(lbuf, 0);
					bool ok = true;
					if (IsMigemo) {
						for (int j=0; j<ptn_lst->Count && ok; j++) ok = TRegEx::IsMatch(fnam, ptn_lst->Strings[j], opt);
					}
					else {
						for (int j=0; j<ptn_lst->Count && ok; j++) {
							UnicodeString swd = ptn_lst->Strings[j];
							bool or_ok = false;
							do {
								UnicodeString s = split_tkn(swd, '|');
								if (!s.IsEmpty()) {
									if ((s.Pos('*') || s.Pos('?')) && str_match(s, fnam))
										or_ok = true;
									else if (ContainsText(fnam, s))
										or_ok = true;
								}
							} while (!or_ok && !swd.IsEmpty());
							ok = or_ok;
						}
					}

					if (ok) f_lst->Add(lbuf);
				}
			}
			catch (...) {
				f_lst->Clear();
			}
			ListBuff->Assign(f_lst.get());
		}
		else
			ListBuff->Assign(AllDirHistory);
	}
	//�ʏ탂�[�h
	else {
		TStringList *h_lst = IsDirStack? DirStack : IsAllDirHist? AllDirHistory : get_DirHistory(CurTabIndex, RefListTag);
		if (h_lst) ListBuff->Assign(h_lst);
	}

	//���X�g�{�b�N�X�Ɋ��蓖��(���z)
	TListBox *lp = DirHistListBox;
	int top = lp->TopIndex;
	lp->Count = ListBuff->Count;
	lp->TopIndex = top;
	if (lp->Count>0) lp->ItemIndex = (idx<lp->Count)? idx : lp->Count - 1;
	lp->SetFocus();
	ListScrPanel->UpdateKnob();

	//�^�C�g��
	UnicodeString tit;
	if (IsDirStack)
		tit.USET_T("�f�B���N�g���E�X�^�b�N");
	else if (IsFindDirHist)
		tit.sprintf(_T("�f�B���N�g������ - �S�̌��� (%u/%u)"), ListBuff->Count, AllDirHistory->Count);
	else if (IsAllDirHist)
		tit.USET_T("�f�B���N�g������ - �S��");
	else {
		tit.sprintf(_T("�f�B���N�g������ - %s"), get_LRUD_str(RefListTag).c_str());
		if (RefListTag!=CurListTag) tit.UCAT_T(" (���Α����Q��)");
	}
	Caption = tit;
}

//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::DirHistListBoxData(TWinControl *Control, int Index, UnicodeString &Data)
{
	Data = (Index>=0 && Index<ListBuff->Count)? ListBuff->Strings[Index] : EmptyStr;
}

//---------------------------------------------------------------------------
//���ڂ̕`��
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::DirHistListBoxDrawItem(TWinControl *Control, int Index,
	TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas *cv = lp->Canvas;
	cv->Font->Assign(ListFont);
	cv->Brush->Color = col_bgList;
	cv->FillRect(Rect);

	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin2(cv);

	if (!IsFindDirHist) {
		if (Index<10) {
			cv->Font->Color = col_fgList;
			cv->Font->Style = cv->Font->Style << fsUnderline;
			cv->TextOut(xp, yp, UnicodeString().sprintf(_T("%u"), (Index + 1)%10));
		}
		cv->Font->Style = cv->Font->Style >> fsUnderline;
		xp += get_CharWidth(cv, 2);
	}

	UnicodeString lbuf = get_csv_item(lp->Items->Strings[Index], 0);
	if (UncToNetDrive) lbuf = UNC_to_NetDriveName(lbuf);
	if (DispRegName)   lbuf = get_RegDirName(lbuf);

	cv->Font->Color = col_Folder;
	if (IsFindDirHist && !IncSeaWord.IsEmpty()) {
		std::unique_ptr<TStringList> wlist(new TStringList());
		get_MatchWordList(lbuf, slash_to_yen(IncSeaWord), IsMigemo, false, wlist.get());
		PathNameOut(lbuf, wlist.get(), cv, xp, yp);
	}
	else {
		PathNameOut(lbuf, NULL, cv, xp, yp);
	}

	//�J�[�\��
	draw_ListCursor2(lp, Rect, Index, State);
}

//---------------------------------------------------------------------------
//���͗��̕`��
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::InpPaintBoxPaint(TObject *Sender)
{
	draw_InputPaintBox((TPaintBox*)Sender, (IsMigemo? "Migemo: " : "") + IncSeaWord);
}

//---------------------------------------------------------------------------
//�L�[����
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::DirHistListBoxKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);	if (KeyStr.IsEmpty()) return;

	UnicodeString cmd_F  = Key_to_CmdF(KeyStr);
	UnicodeString cmd_S  = IsFindDirHist? KeyFuncList->Values["S:" + KeyStr] : EmptyStr;
	bool handled = true;

	try {
		bool is_all = (IsAllDirHist || IsFindDirHist);
		TStringList *h_lst = is_all? AllDirHistory :
						 IsDirStack? DirStack : get_DirHistory(CurTabIndex, RefListTag);
		int *h_ptr = (IsDirStack || IsAllDirHist || IsFindDirHist)? NULL : get_DirHistPtr(CurTabIndex, CurListTag);

		TListBox *lp = (TListBox*)Sender;
		UnicodeString cur_dnam = (lp->ItemIndex!=-1)? get_csv_item(lp->Items->Strings[lp->ItemIndex], 0) : EmptyStr;

		//�m��
		if ((!IsFindDirHist && is_Num0to9(KeyStr)) || equal_ENTER(KeyStr)) {
			int idx = is_Num0to9(KeyStr)? ((KeyStr.ToIntDef(0) + 9)%10) : lp->ItemIndex;
			if (idx<0 || idx>=lp->Count) SkipAbort();
			UnicodeString dnam = get_csv_item(lp->Items->Strings[idx], 0);
			//�f�B���N�g��
			if (ends_PathDlmtr(dnam)) {
				if (!is_drive_accessible(dnam)) TextAbort(_T("�h���C�u�������ł�"));

				if (dir_exists(dnam)) {
					bool is_cur = (RefListTag==CurListTag);
					if (!is_all && is_cur && h_ptr) *h_ptr = idx;
					NyanFiForm->UpdateCurPath(dnam, -1, is_cur);
					ModalResult = mrOk;
				}
				else {
					msgbox_WARN(USTR_NotFound);
					//���݂��Ȃ��f�B���N�g�����폜
					if (h_lst) {
						int i = 0;
						while (i<h_lst->Count) {
							if (StartsText(dnam, get_csv_item(h_lst->Strings[i], 0))) h_lst->Delete(i); else i++;
						}
						UpdateListBox(idx);
					}
				}
			}
			//���[�N���X�g
			else if (test_NwlExt(get_extension(dnam))) {
				WorkListName = dnam;
				ModalResult  = mrOk;
			}
		}
		//�t�B���^����
		else if (IsFindDirHist && update_IncSeaWord(IncSeaWord, KeyStr)) {
			UpdateListBox();
		}
		else if (USAME_TI(cmd_S, "ClearIncKeyword")) {
			IncSeaWord = EmptyStr;
			UpdateListBox();
		}
		else if (contained_wd_i(_T("MigemoMode|NormalMode"), cmd_S)) {
			IsMigemo = USAME_TI(cmd_S, "MigemoMode")? (!IsMigemo && usr_Migemo->DictReady) : false;
			UpdateListBox();
		}
		//�J�[�\���ړ�
		else if (USAME_TI(cmd_S, "IncSearchUp"))	ListBoxCursorUp(lp);
		else if (USAME_TI(cmd_S, "IncSearchDown"))	ListBoxCursorDown(lp);
		else if (MovListBoxFromFilter(lp, KeyStr))	;
		else if	(ExeCmdListBox(lp, cmd_F))			;
		//�������ڂ̍폜
		else if (equal_DEL(KeyStr) || USAME_TI(cmd_F, "Delete")) {
			if (lp->ItemIndex==-1) SkipAbort();
			if (IsFindDirHist) {
				for (;;) {
					int idx = indexof_csv_list(AllDirHistory, cur_dnam, 0);
					if (idx!=-1) AllDirHistory->Delete(idx); else break;
				}
			}
			else {
				if (h_lst) h_lst->Delete(lp->ItemIndex);
			}
			UpdateListBox(lp->ItemIndex);
		}
		//���Α��̗������Q��
		else if (!IsFindDirHist && (equal_RIGHT(KeyStr) || contained_wd_i(_T("ToRight|ToParentOnRight"), cmd_F))) {
			if (h_ptr && RefListTag==0) {
				RefListTag = 1;
				int *h_p = get_DirHistPtr(CurTabIndex, RefListTag);
				if (h_p) UpdateListBox(*h_p);
			}
		}
		else if (!IsFindDirHist && (equal_LEFT(KeyStr) || contained_wd_i(_T("ToLeft|ToParentOnLeft"), cmd_F))) {
			if (h_ptr && RefListTag==1) {
				RefListTag = 0;
				int *h_p = get_DirHistPtr(CurTabIndex, RefListTag);
				if (h_p) UpdateListBox(*h_p);
			}
		}
		//�v���p�e�B
		else if (USAME_TI(cmd_F, "PropertyDlg")) {
			if (!cur_dnam.IsEmpty()) {
				pos_ListBoxItem(lp);
				ShowPropertyDialog(cur_dnam);
			}
		}
		//�E�N���b�N���j���[
		else if (contained_wd_i(KeysStr_Popup, KeyStr)) show_PopupMenu(lp);
		//����
		else if (USAME_TI(cmd_F, "ReturnList")) ModalResult = mrCancel;
		else handled = false;
	}
	catch (EAbort &e) {
		UnicodeString msg = e.Message;
		if (USAME_TI(msg, "SKIP") || USTARTS_TI("Abort", msg)) msg = EmptyStr;
		if (!msg.IsEmpty()) msgbox_WARN(msg); else beep_Warn();
	}

	if (!is_DialogKey(Key) || handled) Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::DirHistListBoxKeyPress(TObject *Sender, System::WideChar &Key)
{
	//�C���N�������^���T�[�`�����
	if (_istalnum(Key) || is_KeyDown(VK_CONTROL)) Key = 0;
}
//---------------------------------------------------------------------------
//�}�E�X����
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::DirHistListBoxDblClick(TObject *Sender)
{
	perform_Key_RETURN((TControl*)Sender);
}

//---------------------------------------------------------------------------
//�w��f�B���N�g���ȉ���S�̗����ɒǉ�
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::AddDirsCore(
	UnicodeString pnam,
	TStrings *lst,
	TStringDynArray ex_lst)	//���O���X�g
{
	if (pnam.IsEmpty() || !dir_exists(pnam)) return;

	pnam = IncludeTrailingPathDelimiter(pnam);
	TSearchRec sr;
	if (FindFirst(cv_ex_filename(UAPP_T(pnam, "*")), faAnyFile, sr)==0) {
		do {
			if ((sr.Attr & faDirectory)==0 || ContainsStr("..", sr.Name)) continue;
			UnicodeString snam = pnam + sr.Name;

			//���O
			for (int i=0; i<ex_lst.Length && !snam.IsEmpty(); i++) {
				if (ex_lst[i].IsEmpty()) continue;
				if (ContainsText(snam, ex_lst[i])) snam = EmptyStr;
			}
			if (snam.IsEmpty()) continue;

			lst->Add(snam);
			HintLabel2->Caption = lst->Count;
			HintLabel2->Invalidate();

			AddDirsCore(snam, lst, ex_lst);
			Application->ProcessMessages();
			if (is_KeyDown(VK_ESCAPE)) FindAborted = true;	//���f
		} while(!FindAborted && FindNext(sr)==0);
		FindClose(sr);
	}
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::AddDirsActionExecute(TObject *Sender)
{
	UnicodeString dnam = CurPathName;
	if (UserModule->SelectDirEx(_T("�S�̗����ɒǉ�����f�B���N�g����I��"), dnam)) {
		FindBusy	= true;
		FindAborted = false;

		//�������q���g�\���̐ݒ�
		HintLabel1->Font->Assign(HintFont);
		HintLabel2->Font->Assign(HintFont);
		HintLabel1->Caption   = "������...�@ESC�L�[�Œ��f";
		HintLabel2->Caption   = EmptyStr;
		HintPanel->Color	  = col_bgHint;
		HintPanel->BoundsRect = DirHistListBox->ClientRect.CenteredRect(HintPanel->ClientRect);
		HintPanel->Visible	  = true;
		HintPanel->Repaint();
		cursor_HourGlass();

		TStringDynArray ex_lst = split_strings_semicolon(NoDirHistPath);
		std::unique_ptr<TStringList> sbuf(new TStringList());
		sbuf->Add(IncludeTrailingPathDelimiter(dnam));
		AddDirsCore(dnam, sbuf.get(), ex_lst);

		HintLabel1->Caption = "�ǉ���...�@ESC�L�[�Œ��f";
		HintLabel2->Caption = "0";
		int cnt = 0;
		for (int i=0; !FindAborted && i<sbuf->Count; i++) {
			UnicodeString pnam = IncludeTrailingPathDelimiter(sbuf->Strings[i]);
			if (indexof_csv_list(AllDirHistory, pnam, 0)==-1) {
				AllDirHistory->Add(UnicodeString().sprintf(_T("\"%s\",0"), pnam.c_str()));
				cnt++;
				HintLabel2->Caption = cnt;
				HintLabel2->Invalidate();
			}
			Application->ProcessMessages();
			if (is_KeyDown(VK_ESCAPE)) FindAborted = true;		//���f
		}

		AllDirHistory->Sort();
		UpdateListBox();

		cursor_Default();
		HintPanel->Visible = false;
		FindBusy = false;

		if (FindAborted)
			msgbox_WARN(USTR_Canceled);
		else
			msgbox_OK(UnicodeString().sprintf(_T("%u�̃f�B���N�g����ǉ����܂����B"), cnt));
	}
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::AddDirsActionUpdate(TObject *Sender)
{
	TAction *ap = (TAction*)Sender;
	ap->Visible = IsFindDirHist;
	ap->Enabled = ExtSaveDirHist && !FindBusy;
}

//---------------------------------------------------------------------------
//���������ׂč폜
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::ClearAllActionExecute(TObject *Sender)
{
	if (IsAllDirHist || IsFindDirHist) {
		if (msgbox_Sure(LoadUsrMsg(USTR_DelHistoryQ, _T("�S��"))))
			AllDirHistory->Clear();
	}
	else if (!IsDirStack) {
		if (msgbox_Sure(LoadUsrMsg(USTR_DelHistoryQ, get_LRUD_str().UCAT_T("��"))))
			clear_DirHistory(CurTabIndex, CurListTag);
	}
	UpdateListBox();
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::ClearAllActionUpdate(TObject *Sender)
{
	TAction *ap = (TAction*)Sender;
	ap->Enabled = !IsDirStack && !FindBusy;
	ap->Caption = (IsAllDirHist || IsFindDirHist)? "�S�̗��������ׂč폜(&A)" : "�J�����g���̗��������ׂč폜(&A)";
}
//---------------------------------------------------------------------------
//�i�荞�ݕ\�����̗������폜
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::ClearFltActionExecute(TObject *Sender)
{
	if (msgbox_Sure(LoadUsrMsg(USTR_DelHistoryQ, _T("�i�荞�ݕ\����")))) {
		cursor_HourGlass();
		for (int i=0; i<ListBuff->Count; i++) {
			UnicodeString dnam = get_csv_item(ListBuff->Strings[i], 0);
			int idx = indexof_csv_list(AllDirHistory, dnam, 0);
			if (idx!=-1) AllDirHistory->Delete(idx);
			Application->ProcessMessages();
		}
		UpdateListBox();
		cursor_Default();
	}
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::ClearFltActionUpdate(TObject *Sender)
{
	TAction *ap = (TAction*)Sender;
	ap->Visible = IsFindDirHist;
	ap->Enabled = !FindBusy && ListBuff->Count>0 && ListBuff->Count<AllDirHistory->Count;
}

//---------------------------------------------------------------------------
//�ꗗ���R�s�[
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::CopyAllActionExecute(TObject *Sender)
{
	std::unique_ptr<TStringList> lst(new TStringList());
	for (int i=0; i<ListBuff->Count; i++)
		lst->Add(get_csv_item(ListBuff->Strings[i], 0));

	copy_to_Clipboard(lst->Text);
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::CopyAllActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !FindBusy && ListBuff->Count>0;
}

//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (!FindBusy && Key==VK_ESCAPE)
		Close();
	else
		SpecialKeyProc(this, Key, Shift);
}

//---------------------------------------------------------------------------
//�f�B���N�g���̃v���p�e�B
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::PropertyActionExecute(TObject *Sender)
{
	TListBox *lp = DirHistListBox;
	if (lp->ItemIndex!=-1) {
		UnicodeString dnam = get_csv_item(lp->Items->Strings[lp->ItemIndex], 0);
		ShowPropertyDialog(dnam);
	}
}
//---------------------------------------------------------------------------
void __fastcall TDirHistoryDlg::PropertyActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = DirHistListBox->ItemIndex!=-1;
}
//---------------------------------------------------------------------------
