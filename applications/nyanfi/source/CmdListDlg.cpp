//----------------------------------------------------------------------//
// NyanFi																//
//  �R�}���h�t�@�C���ꗗ												//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <memory>
#include <algorithm>
#include <Vcl.Clipbrd.hpp>
#include <RegularExpressions.hpp>
#include "UserFunc.h"
#include "UserMdl.h"
#include "Global.h"
#include "CmdListDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TCmdFileListDlg *CmdFileListDlg;

//---------------------------------------------------------------------------
__fastcall TCmdFileListDlg::TCmdFileListDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FormCreate(TObject *Sender)
{
	GridScrPanel  = new UsrScrollPanel(GridPanel,  CmdFileGrid,	   USCRPNL_FLAG_P_WP | USCRPNL_FLAG_G_WP | USCRPNL_FLAG_HS);
	PrevScrPanel  = new UsrScrollPanel(PrevPanel,  PreviewListBox, USCRPNL_FLAG_P_WP | USCRPNL_FLAG_L_WP);
	ReferScrPanel = new UsrScrollPanel(ReferPanel, ReferListBox,   USCRPNL_FLAG_P_WP | USCRPNL_FLAG_L_WP);

	GridItemList = new TStringList();
	cmdfile_List = new TStringList();

	ToFilter = false;
	ToSelect = false;

	ReferListBox->Tag = LBTAG_OPT_FIF1;
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FormShow(TObject *Sender)
{
	IniFile->LoadPosInfo(this, DialogCenter);

	TStringGrid *gp = CmdFileGrid;
	InitializeListGrid(gp);
	InitializeListHeader(CmdFileHeader, _T("�t�@�C����|����|�T�C�Y|�^�C���X�^���v|�f�B���N�g��|�Q�ƌ�|���s��"));
	IniFile->LoadGridColWidth(gp, 7, 200,200,70,120,100,70,70);
	set_HeaderFromGrid(gp, CmdFileHeader);
	set_UsrScrPanel(GridScrPanel);

	set_FormTitle(this, ToSelect? _T("�R�}���h�t�@�C���̑I��") : _T("�R�}���h�t�@�C���ꗗ"));

	CnfExeCheckBox->Checked = IniFile->ReadBoolGen(_T("CmdFileListCnfExe"));
	CnfExeCheckBox->Visible = !ToSelect;
	OkBtn->Visible			= ToSelect;

	set_MigemoCheckBox(MigemoCheckBox, _T("CmdFileListMigemo"));

	//�v���r���[�̏�����
	PreviewPanel->Visible	 = false;
	PreviewSplitter->Visible = false;
	PreviewSplitter->Color	 = col_Splitter;
	ReferSplitter->Color	 = col_Splitter;

	PreviewListBox->Font->Assign(TxtPrvFont);
	PreviewListBox->Color		= col_bgView;
	PreviewListBox->Font->Color = col_fgView;
	set_ListBoxItemHi(PreviewListBox);
	set_UsrScrPanel(PrevScrPanel);

	ReferListBox->Font->Assign(FileInfFont);
	ReferListBox->Color		    = col_bgInf;
	ReferListBox->Font->Color   = col_fgInf;
	set_ListBoxItemHi(ReferListBox);
	set_UsrScrPanel(ReferScrPanel);

	PreviewPanel->Height = IniFile->ReadIntGen(_T("CmdFileListPrevHeight"), 200);
	ReferPanel->Width	 = IniFile->ReadIntGen(_T("CmdFileListReferWidth"), 200);

	PreviewCheckBox->Checked = IniFile->ReadBoolGen(_T("CmdFileListPreview"));
	PreviewCheckBoxClick(NULL);

	::PostMessage(Handle, WM_FORM_SHOWED, 0, 0);
}

//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::WmFormShowed(TMessage &msg)
{
	TStringGrid *gp = CmdFileGrid;
	gp->Repaint();

	if (!UnInitializing) UpdateList();

	//�w��t�@�C���ʒu�Ɉړ�
	gp->TopRow = 0;
	gp->Row    = 0;
	if (!CmdFileName.IsEmpty()) {
		for (int i=0; i<GridItemList->Count; i++) {
			file_rec *fp = (file_rec*)GridItemList->Objects[i];
			if (SameText(CmdFileName, fp->f_name)) {
				gp->Row = i;  break;
			}
		}
	}

	if (gp->Row >= gp->TopRow + gp->VisibleRowCount - 2)
		gp->TopRow = std::max(gp->Row - gp->VisibleRowCount + 2, 0);

	GridScrPanel->UpdateKnob();

	(ToFilter? (TWinControl*)FilterEdit : (TWinControl*)gp)->SetFocus();
	FilterEdit->Color = ToFilter? scl_Window : col_Invalid;
	FilterEdit->Text  = EmptyStr;
}

//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	IniFile->SavePosInfo(this);
	IniFile->SaveGridColWidth(CmdFileGrid);

	IniFile->WriteBoolGen(_T("CmdFileListMigemo"),		MigemoCheckBox);
	IniFile->WriteBoolGen(_T("CmdFileListCnfExe"),		CnfExeCheckBox);
	IniFile->WriteBoolGen(_T("CmdFileListPreview"),		PreviewCheckBox);
	IniFile->WriteIntGen(_T("CmdFileListPrevHeight"),	PreviewPanel->Height);
	IniFile->WriteIntGen(_T("CmdFileListReferWidth"),	ReferPanel->Width);

	ToSelect  = false;
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FormDestroy(TObject *Sender)
{
	clear_FileList(cmdfile_List);
	delete cmdfile_List;
	delete GridItemList;
	delete GridScrPanel;
	delete PrevScrPanel;
	delete ReferScrPanel;
}

//---------------------------------------------------------------------------
//�w�b�_�̕`��
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::CmdFileHeaderDrawSection(THeaderControl *HeaderControl,
          THeaderSection *Section, const TRect &Rect, bool Pressed)
{
	draw_SortHeader(HeaderControl, Section, Rect);
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::CmdFileHeaderResize(TObject *Sender)
{
	set_HeaderFromGrid(CmdFileGrid, CmdFileHeader);
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::CmdFileHeaderSectionResize(THeaderControl *HeaderControl,
		THeaderSection *Section)
{
	set_GridFromHeader(CmdFileHeader, CmdFileGrid);
	GridScrPanel->UpdateKnob();
}

//---------------------------------------------------------------------------
//�I��p�ɕ\��
//---------------------------------------------------------------------------
int __fastcall TCmdFileListDlg::ShowToSelect(UnicodeString fnam)
{
	ToSelect	= true;
	ToFilter	= false;
	CmdFileName = fnam;
	return ShowModal();
}

//---------------------------------------------------------------------------
//�ꗗ�̍X�V
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::UpdateList()
{
	cursor_HourGlass();

	//�R�}���h�t�@�C���̈ꗗ���擾
	clear_FileList(cmdfile_List);
	std::unique_ptr<TStringList> f_lst(new TStringList());
	get_files(ExePath, _T("*.nbt"), f_lst.get(), true);

	for (int i=0; i<f_lst->Count; i++) {
		UnicodeString fnam = f_lst->Strings[i];
		file_rec *fp = cre_new_file_rec(fnam);	if (!fp) continue;
		UnicodeString lbuf = Trim(get_top_line(fnam));
		if (!remove_top_s(lbuf, ';')) lbuf = EmptyStr;
		fp->alias = lbuf;	//����
		cmdfile_List->AddObject(fnam, (TObject*)fp);
	}

	NaturalOrder = true;
	DscPathOrder = false;
	cmdfile_List->CustomSort(SortComp_PathName);

	//�O���b�h�̍X�V
	UpdateGrid();

	cursor_Default();
}
//---------------------------------------------------------------------------
//�\���O���b�h�̍X�V
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::UpdateGrid()
{
	std::unique_ptr<TStringList> cmd_lst(new TStringList());
	TStringGrid *gp = CmdFileGrid;

	//�t�B���^�ɂ���Ē��o
	UnicodeString ptn = usr_Migemo->GetRegExPtn(MigemoCheckBox->Checked, FilterEdit->Text);
	if (!ptn.IsEmpty()) {
		GridItemList->Clear();
		TRegExOptions opt; opt << roIgnoreCase;
		UnicodeString lbuf;
		for (int i=0; i<cmdfile_List->Count; i++) {
			file_rec *fp = (file_rec*)cmdfile_List->Objects[i];
			lbuf.sprintf(_T("%s %s"), fp->n_name.c_str(), fp->alias.c_str());
			if (TRegEx::IsMatch(lbuf, ptn, opt)) GridItemList->AddObject(fp->f_name, (TObject*)fp);
		}
	}
	else
		GridItemList->Assign(cmdfile_List);

	if (GridItemList->Count>0)
		gp->RowCount = GridItemList->Count;
	else {
		gp->RowCount = 1;
		clear_GridRow(gp, 0);
	}

	CmdFileGridClick(NULL);
	gp->Repaint();
	GridScrPanel->UpdateKnob();
}
//---------------------------------------------------------------------------
int __fastcall TCmdFileListDlg::GetGridIndex()
{
	int idx = CmdFileGrid->Row;
	if (idx<0 || idx>=GridItemList->Count) idx = -1;
	return idx;
}

//---------------------------------------------------------------------------
//�ꗗ�̕`��
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::CmdFileGridDrawCell(TObject *Sender, int ACol, int ARow,
		TRect &Rect, TGridDrawState State)
{
	TStringGrid *gp = (TStringGrid*)Sender;
	TCanvas *cv = gp->Canvas;
	cv->Font->Assign(gp->Font);
	bool is_irreg = IsIrregularFont(cv->Font);

	UnicodeString cellstr;
	bool row_break = false;

	cv->Brush->Color = is_AltLnBgCol(ARow)? col_bgList2 : col_bgList;

	if (ARow>=0 && ARow<GridItemList->Count) {
		file_rec *fp0 = (ARow>0)? (file_rec*)GridItemList->Objects[ARow - 1] : NULL;
		file_rec *fp  = (file_rec*)GridItemList->Objects[ARow];
		row_break	  = fp0 && !SameText(fp0->p_name, fp->p_name);
		int lst_idx   = CmdFileList->IndexOf(fp->f_name);

		UnicodeString pnam = fp->p_name;
		TColor col_f = get_ExtColor(".nbt");

		switch (ACol) {
		case 0:	//�t�@�C����
			cellstr.sprintf(_T("%s%s%s"), fp->b_name.c_str(), (NoSpaceFExt? null_TCHAR : _T("\t")), fp->f_ext.c_str());
			cv->Font->Color = col_f;
			break;
		case 1:	//����
			cellstr = fp->alias;
			cv->Font->Color = col_fgList;
			break;
		case 2:	//�T�C�Y
			cellstr = Trim(get_size_str_G(fp->f_size, 8, SizeDecDigits));
			cv->Font->Color = get_SizeColor(fp->f_size, col_f);
			break;
		case 3:	//�^�C���X�^���v
			cellstr = FormatDateTime(TimeStampFmt, fp->f_time);
			cv->Font->Color = get_TimeColor(fp->f_time, col_f);
			break;
		case 4:	//�f�B���N�g��
			if (remove_top_text(pnam, ExePath) && pnam.IsEmpty()) pnam.USET_T(".");
			cellstr = ExcludeTrailingPathDelimiter(pnam);
			cv->Font->Color = col_Folder;
			break;
		case 5:	//�Q�ƌ�
			cellstr = get_ref_CmdFile(fp->f_name);
			cv->Font->Color = col_fgList;
			break;
		case 6:	//���s��
			if (lst_idx!=-1) {
				cmdf_rec *cp = (cmdf_rec*)CmdFileList->Objects[lst_idx];
				cellstr.sprintf(_T("%u"), cp->exe_count);
				cv->Font->Color = col_fgList;
			}
			break;
		}

		if (!SameText(cellstr, gp->Cells[ACol][ARow])) gp->Cells[ACol][ARow] = cellstr;
	}

	//�w�i
	cv->FillRect(Rect);

	int xp = Rect.Left + 4;
	int yp = Rect.Top + get_TopMargin2(cv);
	int r_mgn = get_CharWidth(cv, 1);

	//�t�@�C����
	if (ACol==0) {
		UnicodeString fnam = split_pre_tab(cellstr);
		cv->TextRect(Rect, xp, yp, fnam);
		//�g���q�𕪗��\��
		if (!cellstr.IsEmpty()) {
			xp += get_TextWidth(cv, fnam, is_irreg);
			xp = std::max<int>(xp, (Rect.Right - get_TextWidth(cv, cellstr, is_irreg) - r_mgn));
			cv->TextOut(xp, yp, cellstr);
		}
	}
	//�f�B���N�g��
	else if (ACol==4) {
		PathNameOut(cellstr, cv, xp, yp);
	}
	//���̑�
	else {
		if (ACol==2 || ACol==6) xp = Rect.Right - get_TextWidth(cv, cellstr, is_irreg) - r_mgn;
		cv->TextRect(Rect, xp, yp, cellstr);
	}

	//�J�[�\��
	draw_GridCursor(gp, Rect, ARow, State);

	//�ʃf�B���N�g���Ԃ̃Z�p���[�^
	if (row_break) draw_separateLine(cv, Rect);

	//���m��
	if (gp->RowCount==1 && ARow==0 && ACol==0 && !cellstr.IsEmpty()) {
		cv->Brush->Color = col_Cursor;
		cv->FrameRect(Rect);
	}
}

//---------------------------------------------------------------------------
//�t�B���^�ɂ��ꗗ�̍X�V
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FilterEditChange(TObject *Sender)
{
	UpdateGrid();

	//�m�葦���s
	int idx = GetGridIndex();
	if (idx!=-1) {
		TStringGrid *gp = CmdFileGrid;
		if (!ToSelect && CnfExeCheckBox->Checked && FilterEdit->Focused() && !FilterEdit->Text.IsEmpty()
			&& gp->RowCount==1)
		{
			Application->ProcessMessages();  Sleep(500);
			CmdFileName = ((file_rec*)GridItemList->Objects[idx])->f_name;
			CmdRequestList->Add(UnicodeString().sprintf(_T("ExeCommands_\"@%s\""), CmdFileName.c_str()));
			ModalResult = mrOk;
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FilterEditEnter(TObject *Sender)
{
	FilterEdit->Color = scl_Window;
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FilterEditExit(TObject *Sender)
{
	CloseIME(Handle);
	InvColIfEmpty(FilterEdit);
}

//---------------------------------------------------------------------------
//�t�B���^���ł̃L�[����
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FilterEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);

	if		(contained_wd_i(KeysStr_ToList, KeyStr))	CmdFileGrid->SetFocus();
	else if (MovGridFromFilter(CmdFileGrid, KeyStr))	;
	else if (SameText(KeyStr, KeyStr_Migemo))			MigemoCheckBox->Checked = !MigemoCheckBox->Checked;
	else return;

	CmdFileGrid->Invalidate();
	Key = 0;
}
//---------------------------------------------------------------------------
//�t�B���^����Enter�L�[�őI�����ڂ����s
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FilterEditKeyPress(TObject *Sender, System::WideChar &Key)
{
	if (is_KeyPress_CtrlNotCV(Key))
		Key = 0;
	else if (Key==VK_RETURN && !ToSelect) {
		int idx = GetGridIndex();
		if (idx!=-1) {
			CmdFileName = ((file_rec*)GridItemList->Objects[idx])->f_name;
			CmdRequestList->Add(UnicodeString().sprintf(_T("ExeCommands_\"@%s\""), CmdFileName.c_str()));
			ModalResult = mrOk;
		}
		Key = 0;
	}
}

//---------------------------------------------------------------------------
//�ꗗ�ł̃L�[����
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::CmdFileGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	TStringGrid *gp = (TStringGrid*)Sender;
	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	UnicodeString CmdStr = get_CmdStr(Key_to_CmdF(KeyStr));
	int cmd_id = idx_of_word_i(_T("ReturnList|IncSearch|RenameDlg|Delete|FileEdit|TextViewer"), CmdStr);

	int idx = GetGridIndex();
	file_rec *cfp = (idx!=-1)? (file_rec*)GridItemList->Objects[idx] : NULL;

	try {
		//����
		if		(cmd_id==0) ModalResult = mrCancel;
		//�t�B���^��
		else if (cmd_id==1) FilterEdit->SetFocus();
		//�J�[�\���ړ�
		else if (GridCursorMove(CmdStr, gp))	;
		//�������T�[�`
		else if (is_IniSeaKey(KeyStr)) {	//KeyStr �ɐ��K�\���p�^�[�����Ԃ�
			TRegExOptions opt; opt << roIgnoreCase;
			int idx0=-1, idx1=-1;
			for (int i=0; i<GridItemList->Count && idx1==-1; i++) {
				if (i<=idx && idx0!=-1) continue;
				file_rec *fp = (file_rec*)GridItemList->Objects[i];
				if (TRegEx::IsMatch(fp->b_name, KeyStr, opt)) ((i<=idx)? idx0 : idx1) = i;
			}
			int rn = (idx1!=-1)? idx1 : idx0;
			if (rn!=-1) gp->Row = rn;
		}
		else {
			if (!cfp) Abort();
			CmdFileName = cfp->f_name;
			UnicodeString tmp;
			//�m��
			if (equal_ENTER(KeyStr)) {
				if (!ToSelect) CmdRequestList->Add(tmp.sprintf(_T("ExeCommands_\"@%s\""), CmdFileName.c_str()));
				ModalResult = mrOk;
			}
			//����
			else if (cmd_id==2) {
				UnicodeString org_name = cfp->b_name;
				UnicodeString new_name = inputbox_ex(LoadUsrMsg(USTR_Rename).c_str(), _T("�t�@�C�����啔"), org_name);
				if (!new_name.IsEmpty() && !SameStr(org_name, new_name)) {
					new_name = cfp->p_name + new_name + cfp->f_ext;
					if (move_FileT(cfp->f_name, new_name)) {
						RenCmdFileList->Add(tmp.sprintf(_T("%s\t%s"), cfp->f_name.c_str(), new_name.c_str()));
						cfp->f_name = new_name;
						RenameOptCmdFile();
						UpdateList();
					}
					else throw EAbort(tmp.sprintf(_T("�����Ɏ��s���܂���\r\n%s"), get_LogErrMsg(EmptyStr, false).c_str()));
				}
			}
			//�폜
			else if (cmd_id==3) {
				UnicodeString msg;
				if (msgbox_Sure(LoadUsrMsg(USTR_DeleteQ, cfp->n_name),  SureDelete)) {
					StartLog(LoadUsrMsg(USTR_BeginDelete, cfp->p_name));
					msg = make_LogHdr(_T("DELETE"), cfp);
					UnicodeString del_nam = cfp->f_name;
					try {
						if (ForceDel && !set_FileWritable(del_nam))	Abort();
						if (!delete_File(del_nam, DelUseTrash))		Abort();
						AddLog(msg);
						EndLog(_T("�폜"));
						UpdateList();
					}
					catch (EAbort &e) {
						set_LogErrMsg(msg);
						AddLog(msg);
						UserAbort(USTR_FaildDelete);
					}
				}
			}
			else if (!ToSelect) {
				//�ҏW�E�{��
				if (cmd_id==4 || cmd_id==5) {
					CmdRequestList->Add(tmp.sprintf(_T("%s_\"%s\""), CmdStr.c_str(), cfp->f_name.c_str()));
					ModalResult = mrOk;
				}
			}
		}
	}
	catch (EAbort &e) {
		UnicodeString msg = e.Message;
		if (USTARTS_TI("Abort", msg)) msg = EmptyStr;
		if (!msg.IsEmpty()) msgbox_ERR(msg); else beep_Warn();
	}

	if (!is_DialogKey(Key)) Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::CmdFileGridDblClick(TObject *Sender)
{
	perform_Key_RETURN((TControl*)Sender);
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::CmdFileGridClick(TObject *Sender)
{
	int idx = GetGridIndex();
	if (idx!=-1) {
		file_rec *fp = (file_rec*)GridItemList->Objects[idx];
		//�v���r���[
		try {
			PreviewListBox->Items->LoadFromFile(fp->f_name);
			PrevScrPanel->UpdateKnob();
		}
		catch (...) {
			;
		}
		//�t�@�C�����
		std::unique_ptr<TStringList> inf_lst(new TStringList());
		get_FileInfList(fp, inf_lst.get());
		ReferListBox->Items->Assign(inf_lst.get());
		ReferScrPanel->UpdateKnob();
	}
}

//---------------------------------------------------------------------------
//�v���r���[�̕`��
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::PreviewListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Brush->Color = State.Contains(odSelected)? col_selItem : col_bgView;
	cv->FillRect(Rect);

	PrvTextOut(lp, Index, cv, Rect,
		is_SelFgCol(State)? col_fgSelItem : col_fgView,
		4, NULL, "*.nbt");
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::PreviewCheckBoxClick(TObject *Sender)
{
	bool sw = PreviewCheckBox->Checked;
	if (sw) {
		PreviewSplitter->Align = alTop;
		PreviewPanel->Align    = alBottom;
		PreviewSplitter->Align = alBottom;
	}
	PreviewPanel->Visible	 = sw;
	PreviewSplitter->Visible = sw;
}
//---------------------------------------------------------------------------
//�v���r���[/�Q�Ə��ł̃L�[����
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::PreviewListBoxKeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	if		(SameText(KeyStr, KeyStr_Copy))			 EditCopyAction->Execute();
	else if (contained_wd_i(KeysStr_ToList, KeyStr)) CmdFileGrid->SetFocus();
}

//---------------------------------------------------------------------------
//�Q�Ə��̕`��
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::ReferListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	draw_InfListBox((TListBox*)Control, Rect, Index, State);
}

//---------------------------------------------------------------------------
//�I���̊m��
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::OkBtnClick(TObject *Sender)
{
	int idx = GetGridIndex();
	if (idx!=-1) {
		CmdFileName = ((file_rec*)GridItemList->Objects[idx])->f_name;
		ModalResult = mrOk;
	}
}

//---------------------------------------------------------------------------
//�R�s�[
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::EditCopyActionExecute(TObject *Sender)
{
	TListBox *lp = PreviewListBox->Focused() ? PreviewListBox :
					 ReferListBox->Focused() ? ReferListBox : NULL;
	ExeCmdListBox(lp, _T("ClipCopy"));
}
//---------------------------------------------------------------------------
//���ׂđI��
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::EditSelectAllActionExecute(TObject *Sender)
{
	ListBoxSelectAll(PreviewListBox->Focused() ? PreviewListBox :
					   ReferListBox->Focused() ? ReferListBox : NULL);
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::EditCopyActionUpdate(TObject *Sender)
{
	TListBox *lp = PreviewListBox->Focused() ? PreviewListBox :
				     ReferListBox->Focused() ? ReferListBox : NULL;
	((TAction*)Sender)->Enabled = lp && lp->SelCount>0;
}

//---------------------------------------------------------------------------
//�ҏW
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::ReqEditActionExecute(TObject *Sender)
{
	int idx = GetGridIndex();
	if (idx!=-1) {
		file_rec *cfp = (file_rec*)GridItemList->Objects[idx];
		if (cfp) {
			CmdRequestList->Add(UnicodeString().sprintf(_T("FileEdit_\"%s\""), cfp->f_name.c_str()));
			ModalResult = mrOk;
		}
	}
	if (ModalResult!=mrOk) beep_Warn();
}
//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::ReqEditActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !ToSelect;
}

//---------------------------------------------------------------------------
void __fastcall TCmdFileListDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	SpecialKeyProc(this, Key, Shift);
}
//---------------------------------------------------------------------------