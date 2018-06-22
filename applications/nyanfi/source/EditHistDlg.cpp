//----------------------------------------------------------------------//
// NyanFi																//
//  最近編集/閲覧したファイル一覧/ マーク一覧/ ダイレクトタグジャンプ	//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <algorithm>
#include <RegularExpressions.hpp>
#include "UserFunc.h"
#include "UserMdl.h"
#include "Global.h"
#include "EditHistDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditHistoryDlg *EditHistoryDlg;

//---------------------------------------------------------------------------
__fastcall TEditHistoryDlg::TEditHistoryDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FormCreate(TObject *Sender)
{
	GridScrPanel = new UsrScrollPanel(GridPanel, EditHistGrid, USCRPNL_FLAG_P_WP | USCRPNL_FLAG_G_WP | USCRPNL_FLAG_HS);

	isView = isMark = isTags = isTagPtn = false;
	OptVisible = false;
	ToFilter   = false;

	HistoryList = NULL;
	HistBufList = new TStringList();
	TagJumpList = new TStringList();
}
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FormShow(TObject *Sender)
{
	TStringGrid *gp = EditHistGrid;
	InitializeListGrid(gp);

	set_ButtonMark(HideOptBtn,  UBMK_BDOWN);
	set_ButtonMark(ShowOptBtn,  UBMK_BUP);

	if		(isMark) this->Name = "MarkListDlg";
	else if (isTags) this->Name = "TagJumpDlg";
	IniFile->LoadPosInfo(this, DialogCenter);
	this->Name = "EditHistoryDlg";

	if (isMark) {
		gp->Name = "MarkListGrid";
		gp->ColCount = 7;

		//v10.54 での変更に伴う初期化
		if (IniFile->ReadString(gp->Name, "ColWidth6").IsEmpty()) IniFile->EraseSection(gp->Name);

		IniFile->LoadGridColWidth(gp, 7, 24,200,50,100,200,100,100);

		MarkSortMode   = IniFile->ReadIntGen(_T("MarkListSortMode"));
		MarkSortOdr[0] = IniFile->ReadIntGen(_T("MarkListSortOdr0"),  1);
		MarkSortOdr[1] = IniFile->ReadIntGen(_T("MarkListSortOdr1"),  1);
		MarkSortOdr[2] = IniFile->ReadIntGen(_T("MarkListSortOdr2"), -1);
		MarkSortOdr[3] = IniFile->ReadIntGen(_T("MarkListSortOdr3"),  1);
		MarkSortOdr[4] = IniFile->ReadIntGen(_T("MarkListSortOdr4"),  1);
		MarkSortOdr[5] = IniFile->ReadIntGen(_T("MarkListSortOdr5"), -1);
	}
	else if (isTags) {
		gp->Name = "TagJumpGrid";
		gp->ColCount = 6;
		IniFile->LoadGridColWidth(gp, 6, 24,200,100,200,100,100);
	}
	else {
		gp->ColCount = 4;
		IniFile->LoadGridColWidth(gp, 4, 24,200,100,200);
	}
	gp->Name = "EditHistGrid";

	OptPanel->Visible = isTags? false : IniFile->ReadBoolGen(isMark? _T("MarkListShowOpt") : _T("EditHistShowOpt"), true);
	if (ToFilter) OptPanel->Visible = true;
	OptVisible = OptPanel->Visible;
	BlankPanel->Visible = !OptVisible && !isTags;

	HelpContext = isTags? 83 : isMark? 70 : 58;

	EditHistHeader->Style	 = isMark? hsButtons : hsFlat;
	OpenCheckBox->Checked	 = IniFile->ReadBoolGen(_T("MarkListCnfOpen"));

	OptRadioGroup->Enabled	 = false;
	OptRadioGroup->ItemIndex = IniFile->ReadIntGen(_T("EditHistOptMode"));
	OptRadioGroup->Enabled	 = true;

	UnicodeString tit;
	EditHistHeader->Sections->Clear();
	if (isMark) {
		tit.USET_T("マーク一覧");
		EditHistHeader->Style = hsButtons;
		for (int i=0; i<gp->ColCount+1; i++) EditHistHeader->Sections->Add();
		InitializeListHeader(EditHistHeader, _T(" |名前|種類|更新日時|場所|メモ|設定日時"));
		EditHistGrid->PopupMenu = MarkPopupMenu;
	}
	else if (isTags) {
		tit.sprintf(_T("ダイレクトタグジャンプ [%s] - %s"), TagName.c_str(), (isView? _T("閲覧") : _T("編集")));
		EditHistHeader->Style = hsFlat;
		for (int i=0; i<gp->ColCount+1; i++) EditHistHeader->Sections->Add();
		UnicodeString inam;
		if (TagJumpList->Count>0) {
			UnicodeString nptn = get_post_tab(get_post_tab(TagJumpList->Strings[0]));
			isTagPtn = (starts_t_s('/', nptn) || starts_t_s('?', nptn));
			inam = isTagPtn? "パターン" : "行番号";
		}
		UnicodeString hdr;
		hdr.sprintf(_T(" |ファイル名|更新日時|場所|%s|備考"), inam.c_str());
		InitializeListHeader(EditHistHeader, hdr.c_str());
		EditHistGrid->PopupMenu = NULL;
	}
	else {
		tit.sprintf(_T("最近%sしたファイル一覧"), isView? _T("閲覧") : _T("編集"));
		EditHistHeader->Style = hsFlat;
		for (int i=0; i<gp->ColCount+1; i++) EditHistHeader->Sections->Add();
		InitializeListHeader(EditHistHeader, _T(" |ファイル名|更新日時|場所"));
		EditHistGrid->PopupMenu = HistPopupMenu;
	}

	for (int i=0; i<EditHistHeader->Sections->Count; i++)
		EditHistHeader->Sections->Items[i]->Style = hsOwnerDraw;

	set_HeaderFromGrid(gp, EditHistHeader);
	set_UsrScrPanel(GridScrPanel);

	Caption = tit;

	RadioPanel->Visible = !isMark;
	MarkPanel->Width = isMark? (OpenCheckBox->Width + MemoBtn->Width + 8) : 4;
	OpenCheckBox->Visible  = isMark;
	MemoBtn->Visible	   = isMark;

	set_MigemoCheckBox(MigemoCheckBox, _T("EditHistMigemo"));

	HistoryList = (isMark || isTags)? NULL : isView? TextViewHistory : TextEditHistory;
	//存在しないファイルの履歴を削除
	if (HistoryList) {
		int i = 0;
		while (i<HistoryList->Count)
			if (!file_exists(get_csv_item(HistoryList->Strings[i], 0))) HistoryList->Delete(i); else i++;
	}

	UpdateList();
	gp->Row = isMark? std::min(IniFile->ReadIntGen(_T("MarkListLastIdx")), gp->RowCount - 1) : 0;
	GridScrPanel->UpdateKnob();
	SetOptBtn();

	EditFileName = EmptyStr;
	CmdStr		 = EmptyStr;
	TagJumpInf	 = EmptyStr;

	(ToFilter? (TWinControl*)FilterEdit : (TWinControl*)gp)->SetFocus();
	FilterEdit->Color = ToFilter? scl_Window : col_Invalid;
	FilterEdit->Text  = EmptyStr;
}
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	TStringGrid *gp = EditHistGrid;

	if		(isMark) this->Name = "MarkListDlg";
	else if (isTags) this->Name = "TagJumpDlg";
	IniFile->SavePosInfo(this);
	this->Name = "EditHistoryDlg";

	if		(isMark) gp->Name = "MarkListGrid";
	else if (isTags) gp->Name = "TagJumpGrid";
	IniFile->SaveGridColWidth(gp);
	gp->Name = "EditHistGrid";

	IniFile->WriteBoolGen(isMark? _T("MarkListShowOpt") : _T("EditHistShowOpt"),	OptPanel->Visible);
	if (isMark) IniFile->WriteIntGen(_T("MarkListLastIdx"),	gp->Row);

	IniFile->WriteBoolGen(_T("EditHistMigemo"),		MigemoCheckBox);
	IniFile->WriteBoolGen(_T("MarkListCnfOpen"),	OpenCheckBox);
	IniFile->WriteIntGen(_T("EditHistOptMode"),		OptRadioGroup);

	if (isMark) {
		IniFile->WriteIntGen(_T("MarkListSortMode"),	MarkSortMode);
		IniFile->WriteIntGen(_T("MarkListSortOdr0"),	MarkSortOdr[0]);
		IniFile->WriteIntGen(_T("MarkListSortOdr1"),	MarkSortOdr[1]);
		IniFile->WriteIntGen(_T("MarkListSortOdr2"),	MarkSortOdr[2]);
		IniFile->WriteIntGen(_T("MarkListSortOdr3"),	MarkSortOdr[3]);
		IniFile->WriteIntGen(_T("MarkListSortOdr4"),	MarkSortOdr[4]);
		IniFile->WriteIntGen(_T("MarkListSortOdr5"),	MarkSortOdr[5]);
	}

	clear_FileList(HistBufList);
	TagJumpList->Clear();
	isView = isMark = isTags = isTagPtn = false;
	ToFilter = false;
}
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FormDestroy(TObject *Sender)
{
	clear_FileList(HistBufList);
	delete HistBufList;

	delete TagJumpList;

	delete GridScrPanel;
}

//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FormResize(TObject *Sender)
{
	SetOptBtn();
}

//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::WmNyanFiFlIcon(TMessage &msg)
{
	EditHistGrid->Invalidate();
}

//---------------------------------------------------------------------------
//ヘッダの描画
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::EditHistHeaderDrawSection(THeaderControl *HeaderControl,
		THeaderSection *Section, const TRect &Rect, bool Pressed)
{
	int mk_mode = 0;
	if (isMark && Section->Index>0) {
		int idx = Section->Index - 1;
		if (idx==MarkSortMode) mk_mode = MarkSortOdr[idx];
	}

	draw_SortHeader(HeaderControl, Section, Rect, mk_mode);
}
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::EditHistHeaderResize(TObject *Sender)
{
	set_HeaderFromGrid(EditHistGrid, EditHistHeader);
}
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::EditHistHeaderSectionResize(THeaderControl *HeaderControl,
		THeaderSection *Section)
{
	set_GridFromHeader(EditHistHeader, EditHistGrid);
	GridScrPanel->UpdateKnob();
	SetOptBtn();
}

//---------------------------------------------------------------------------
UnicodeString __fastcall TEditHistoryDlg::get_CurFileName()
{
	TStringGrid *gp = EditHistGrid;
	int idx = gp->Row;
	return (idx>=0 && idx<HistBufList->Count)? HistBufList->Strings[idx] : EmptyStr;
}

//---------------------------------------------------------------------------
//一覧内容の更新
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::UpdateList()
{
	cursor_HourGlass();

	UnicodeString cur_fnam = get_CurFileName();

	clear_FileList(HistBufList);

	UnicodeString ptn = usr_Migemo->GetRegExPtn(MigemoCheckBox->Checked, FilterEdit->Text);
	TRegExOptions opt; opt << roIgnoreCase;

	//マーク
	if (isMark) {
		for (int i=0; i<IniFile->MarkIdxList->Count; i++) {
			UnicodeString dnam = IniFile->MarkIdxList->Strings[i];
			if (!is_drive_accessible(dnam)) continue;
			bool is_arc = !ends_PathDlmtr(dnam);	//アーカイブ
			TStringList *klist = (TStringList*)(IniFile->MarkIdxList->Objects[i]);
			for (int k=0; k<klist->Count; k++) {
				UnicodeString lbuf = klist->Strings[k];

				//絞り込み
				if (!ptn.IsEmpty()) {
					TStringDynArray sbuf = split_strings_tab(lbuf);
					UnicodeString s;
					if (sbuf.Length>0) s = sbuf[0];
					if (sbuf.Length>1) s.cat_sprintf(_T("\t%s"), sbuf[1].c_str());
					if (!TRegEx::IsMatch(s, ptn, opt)) continue;
				}

				UnicodeString fnam = split_pre_tab(lbuf);
				file_rec *fp;
				//アーカイブ
				if (is_arc) {
					fp = cre_new_file_rec();
					fp->is_virtual = true;
					fp->arc_name   = dnam;
					fp->f_name.sprintf(_T("%s/%s"), dnam.c_str(), ExcludeTrailingPathDelimiter(fnam).c_str());
					fp->is_dir	   = ends_PathDlmtr(fnam);
					if (fp->is_dir)
						fp->b_name = ExtractFileName(ExcludeTrailingPathDelimiter(fnam));
					else {
						fp->f_ext  = get_extension(fp->f_name);
						fp->b_name = get_base_name(fnam);
					}
					fnam = ExcludeTrailingPathDelimiter(fnam);
					if (contains_PathDlmtr(fnam)) dnam.cat_sprintf(_T("/%s"), ExtractFileDir(fnam).c_str());
					fp->p_name = dnam;
					fp->f_time = get_file_age(fp->arc_name);
				}
				//通常
				else {
					UnicodeString pnam = dnam + fnam;
					if (dir_exists(pnam)) pnam = IncludeTrailingPathDelimiter(pnam);
					fp = cre_new_file_rec(pnam);
				}

				if (fp) {
					fp->memo = lbuf;
					HistBufList->AddObject(fp->f_name, (TObject*)fp);
				}
			}
		}
		sort_MarkList();
	}
	//ダイレクトタグジャンプ
	else if (isTags) {
		for (int i=0; i<TagJumpList->Count; i++) {
			UnicodeString lbuf = TagJumpList->Strings[i];
			UnicodeString fnam = get_post_tab(lbuf);
			UnicodeString nptn = get_post_tab(fnam);
			fnam = TagPath + get_pre_tab(fnam);
			file_rec *fp = cre_new_file_rec(fnam);
			if (fp) {
				fp->alias = get_tkn(lbuf, ";\"");
				fp->memo  = Trim(get_tkn_r(lbuf, ";\""));
				HistBufList->AddObject(fp->f_name, (TObject*)fp);
			}
		}
	}
	//履歴
	else if (HistoryList) {
		for (int i=0; i<HistoryList->Count; i++) {
			UnicodeString fnam = get_csv_item(HistoryList->Strings[i], 0); 
			if (OptRadioGroup->ItemIndex==0 ||
			    (OptRadioGroup->ItemIndex==1 && StartsText(CurPathName, fnam)) ||
				(OptRadioGroup->ItemIndex==2 && SameText(CurPathName, ExtractFilePath(fnam))))
			{
				if (!ptn.IsEmpty() && !TRegEx::IsMatch(ExtractFileName(fnam), ptn, opt)) continue;
				file_rec *fp = cre_new_file_rec(fnam);
				if (fp) HistBufList->AddObject(fp->f_name, (TObject*)fp);
			}
		}
	}

	UpdateGrid();

	//カーソル位置を復元
	TStringGrid *gp = EditHistGrid;
	int idx = std::max(HistBufList->IndexOf(cur_fnam), 0);
	if (idx<gp->RowCount) gp->Row = idx;

	GridScrPanel->UpdateKnob();

	cursor_Default();
}

//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::UpdateGrid()
{
	TStringGrid *gp = EditHistGrid;
	gp->Canvas->Font->Assign(ListFont);

	if (HistBufList->Count>0) {
		//拡張子の最大幅を取得
		MaxFextWd = 0;
		if (!isMark) {
			bool is_irreg = IsIrregularFont(ListFont);
			for (int i=0; i<HistBufList->Count; i++) {
				file_rec *fp = (file_rec*)HistBufList->Objects[i];
				MaxFextWd = std::max(get_TextWidth(gp->Canvas, fp->f_ext, is_irreg), MaxFextWd);
			}
		}

		gp->RowCount = HistBufList->Count;

		for (int i=0; i<HistBufList->Count; i++) {
			file_rec *fp = (file_rec*)HistBufList->Objects[i];
			int col = 0;
			gp->Cells[col++][i] = (i<10)? UnicodeString().sprintf(_T("%u"), (i + 1)%10) : EmptyStr;
			//名前
			if (isMark) {
				gp->Cells[col++][i] = fp->b_name;
				gp->Cells[col++][i] = fp->f_ext;
			}
			else
				gp->Cells[col++][i] = fp->b_name + fp->f_ext;
			//更新日時
			gp->Cells[col++][i] = FormatDateTime(TimeStampFmt, fp->f_time);

			//場所
			UnicodeString lbuf = fp->p_name;
			if (isTags) {
				remove_top_text(lbuf, TagPath);
				if (lbuf.IsEmpty()) lbuf = ".\\";
			}
			else {
				if (UncToNetDrive) lbuf = UNC_to_NetDriveName(lbuf);
				if (DispRegName)   lbuf = get_RegDirName(lbuf);
			}
			gp->Cells[col++][i] = lbuf;

			//メモ
			if (isMark) {
				gp->Cells[col++][i] = get_pre_tab(fp->memo);
				TDateTime dt;
				if (!str_to_DateTime(get_post_tab(fp->memo), &dt)) dt = fp->f_time;
				gp->Cells[col][i] = FormatDateTime(TimeStampFmt, dt);
			}
			//行番号/パターン, 備考
			else if (isTags) {
				lbuf = get_post_tab(get_post_tab(fp->alias));
				gp->Cells[col++][i] = REPLACE_TS(lbuf, "\t", " ");
				gp->Cells[col][i]	= REPLACE_TS(fp->memo, "\t", " ");
			}
		}
	}
	else {
		gp->RowCount = 1;
		clear_GridRow(gp, 0);
	}
	gp->Invalidate();
}

//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::SetOptBtn()
{
	if (ScrBarStyle==0)
		set_ControlRBCorner(EditHistGrid, BlankPanel);
	else {
		BlankPanel->Left = GridPanel->ClientWidth  - BlankPanel->Width  - (GridScrPanel->VisibleV? GridScrPanel->KnobWidth : 0);
		BlankPanel->Top  = GridPanel->ClientHeight - BlankPanel->Height - (GridScrPanel->VisibleH? GridScrPanel->KnobWidth : 0);
		BlankPanel->BringToFront();
	}
}

//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::OptRadioGroupClick(TObject *Sender)
{
	if (!OptRadioGroup->Enabled) return;

	UpdateList();

	TStringGrid *gp = EditHistGrid;
	gp->Row = 0;
	gp->SetFocus();
}

//---------------------------------------------------------------------------
//指定インデックスの履歴項目を削除/マーク解除
//---------------------------------------------------------------------------
bool __fastcall TEditHistoryDlg::del_HistItem()
{
	if (!isMark && !HistoryList) return false;

	int idx = EditHistGrid->Row;
	UnicodeString fnam = get_CurFileName();
	if (fnam.IsEmpty()) return false;

	if (isMark)
		IniFile->FileMark(fnam, 0);
	else {
		int i = 0;
		while (i<HistoryList->Count)
			if (SameText(fnam, get_csv_item(HistoryList->Strings[i], 0))) HistoryList->Delete(i); else i++;
	}

	del_FileListItem(HistBufList, idx);
	UpdateGrid();
	return true;
}

//---------------------------------------------------------------------------
//セルの描画
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::EditHistGridDrawCell(TObject *Sender, int ACol, int ARow,
		TRect &Rect, TGridDrawState State)
{
	TStringGrid *gp = (TStringGrid*)Sender;
	TCanvas *cv = gp->Canvas;
	cv->Font->Assign(gp->Font);
	bool is_irreg = IsIrregularFont(cv->Font);

	int idx = ARow;
	if (idx<HistBufList->Count) {
		file_rec *fp = (file_rec*)HistBufList->Objects[ARow];
		cv->Brush->Color = fp->is_virtual? col_bgArc : is_AltLnBgCol(ARow)? col_bgList2 : col_bgList;
		cv->FillRect(Rect);

		UnicodeString lbuf = gp->Cells[ACol][ARow];

		TColor col_fg = col_fgList;
		//マーク一覧
		if (isMark) {
			switch (ACol) {
			case 0: cv->Font->Style = cv->Font->Style << fsUnderline;	break;
			case 2: col_fg = get_ExtColor(fp->f_ext);					break;
			case 3: col_fg = get_TimeColor(fp->f_time, col_fgList);		break;
			case 4: col_fg = col_Folder;								break;
			case 6: {
					TDateTime dt;
					if (str_to_DateTime(lbuf, &dt)) col_fg = get_TimeColor(dt, col_fgList);
				}
				break;
			}
		}
		//その他の一覧
		else {
			switch (ACol) {
			case 0: cv->Font->Style = cv->Font->Style << fsUnderline;	break;
			case 2: col_fg = get_TimeColor(fp->f_time, col_fgList);		break;
			case 3: col_fg = col_Folder;								break;
			}
		}
		cv->Font->Color = col_fg;

		int xp = Rect.Left + ScaledInt(4);
		int yp = Rect.Top + get_TopMargin2(cv);

		TColor col_f, col_x;

		//ファイル名の共通処理
		if (ACol==1) {
			//アイコン
			if (ShowIcon) {
				draw_SmallIcon(fp, cv, xp, std::max(yp + (cv->TextHeight("Q") - SIcoSize)/2, 0));
				xp += get_IcoWidth();
			}

			col_x = get_ExtColor(fp->f_ext);
			col_f = get_FileColor(fp, col_x);
		}

		//マーク一覧
		if (isMark) {
			//ファイル名
			if (ACol==1) {
				int mgn = ScaledInt(4);
				//ディレクトリ
				cv->Font->Color = col_f;
				UnicodeString bnam;
				if (fp->is_dir) {
					bnam = (fp->is_dir && !ShowIcon)? (DirBraStr + fp->b_name + DirKetStr) : fp->b_name;
					bnam = minimize_str(bnam, cv, Rect.Right - xp - mgn, OmitEndOfName);
					cv->TextOut(xp, yp, bnam);
				}
				//ファイル
				else {
					bnam = minimize_str(fp->b_name, cv, Rect.Right - xp - mgn, OmitEndOfName);
					cv->TextOut(xp, yp, bnam);
				}
			}
			//拡張子
			else if (ACol==2) {
				if (!fp->is_dir) cv->TextOut(xp, yp, lbuf);
			}
			//場所
			else if (ACol==4) {
				PathNameOut(lbuf, cv, xp, yp, gp->ColWidths[ACol]);
			}
			else {
				cv->TextRect(Rect, xp, yp, lbuf);
			}
		}
		//その他の一覧
		else {
			//ファイル名
			if (ACol==1) {
				int mgn = get_CharWidth(cv, 1);
				//ディレクトリ or 拡張子非分離
				cv->Font->Color = col_f;
				UnicodeString bnam;
				if (NoSpaceFExt || fp->is_dir) {
					bnam = (fp->is_dir && !ShowIcon)? (DirBraStr + fp->b_name + DirKetStr) : fp->b_name;
					bnam = minimize_str(bnam, cv, Rect.Right - xp - get_TextWidth(cv, fp->f_ext, is_irreg) - mgn, OmitEndOfName);
					cv->TextOut(xp, yp, bnam);
					xp += get_TextWidth(cv, bnam, is_irreg);
				}
				//拡張子分離
				else {
					int x_fx = Rect.Right - MaxFextWd - mgn;
					bnam = minimize_str(fp->b_name, cv, x_fx - xp, OmitEndOfName);
					cv->TextOut(xp, yp, bnam);
					xp = x_fx;
				}

				//拡張子
				if (!fp->is_dir) {
					if (PriorFExtCol || ColorOnlyFExt) cv->Font->Color = col_x;
					cv->TextOut(xp, yp, fp->f_ext);
				}
			}
			//場所
			else if (ACol==3) {
				PathNameOut(lbuf, cv, xp, yp, gp->ColWidths[ACol]);
			}
			else {
				if (isTagPtn && ACol==4) lbuf = minimize_str(lbuf, cv, Rect.Width() - 8, true);
				cv->TextRect(Rect, xp, yp, lbuf);
			}
		}
	}
	else {
		cv->Brush->Color = col_bgList;
		cv->FillRect(Rect);
	}

	//カーソル
	draw_GridCursor(gp, Rect, ARow, State);
}

//---------------------------------------------------------------------------
bool __fastcall TEditHistoryDlg::set_FileName(int idx)
{
	if (idx>=0 && idx<HistBufList->Count) {
		file_rec *fp = (file_rec*)HistBufList->Objects[idx];
		UnicodeString fnam = fp->f_name;
		if (fp->is_virtual && fp->is_dir) fnam = IncludeTrailingPathDelimiter(fnam);
		EditFileName = fnam;
		if (isTags) TagJumpInf = fp->alias;
	}
	else
		EditFileName = EmptyStr;
	return !EditFileName.IsEmpty();
}

//---------------------------------------------------------------------------
//キー操作
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::EditHistGridKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	TStringGrid *gp = (TStringGrid*)Sender;

	UnicodeString KeyStr = get_KeyStr(Key, Shift);
	CmdStr = Key_to_CmdF(KeyStr);

	try {
		//数字キーによるアクセス
		if (is_Num0to9(KeyStr)) {
			if (!set_FileName((KeyStr.ToIntDef(0) + 9)%10)) Abort(); 
			if (isMark)
				CmdStr.sprintf(_T("%s"), OpenCheckBox->Checked? _T("OpenStandard") : null_TCHAR);
			else
				CmdStr.sprintf(_T("%s"), isView? _T("TextViewer") : _T("FileEdit"));
			ModalResult = mrOk;
		}
		else if (equal_ENTER(KeyStr)) {
			if (!set_FileName(gp->Row)) Abort();
			CmdStr.sprintf(_T("%s"), (isMark && OpenCheckBox->Checked)? _T("OpenStandard") : null_TCHAR);
			ModalResult = mrClose;	//移動
		}
		//フィルタへ
		else if (USTARTS_TI("IncSearch", CmdStr) && OptVisible) FilterEdit->SetFocus();
		//閉じる
		else if (USAME_TI(CmdStr, "ReturnList")) ModalResult = mrCancel;
		//カーソル移動
		else if (GridCursorMove(CmdStr, gp))	;
		//項目削除/マーク解除
		else if (equal_DEL(KeyStr) || USAME_TI(CmdStr, "Delete")) {
			if (!del_HistItem()) Abort();
		}
		//メモ入力
		else if (isMark && USAME_TI(CmdStr, "Mark_IM")) MemoItemClick(NULL);
		//コマンド
		else if (!isTags && contained_wd_i(_T("FileEdit|TextViewer|ImageViewer|OpenByApp|OpenByWin"), CmdStr)) {
			if (!set_FileName(gp->Row)) Abort();
			ModalResult = mrOk;
		}
		//右クリックメニュー
		else if (contained_wd_i(KeysStr_Popup, KeyStr) && gp->PopupMenu)
			show_PopupMenu(gp);
		//頭文字サーチ
		else if (is_IniSeaKey(KeyStr)) {	//KeyStr に正規表現パターンが返る
			TRegExOptions opt; opt << roIgnoreCase;
			int idx0 = -1, idx1 = -1;
			for (int i=0; i<HistBufList->Count && idx1==-1; i++) {
				if (i<=gp->Row && idx0!=-1) continue;
				UnicodeString fnam = HistBufList->Strings[i];
				if (contains_Slash(fnam)) fnam = get_tkn_r(fnam, '/');
				fnam = ExtractFileName(fnam);
				if (TRegEx::IsMatch(fnam, KeyStr, opt)) ((i<=gp->Row)? idx0 : idx1) = i;
			}
			int f_idx = (idx1!=-1)? idx1 : idx0;
			if (f_idx!=-1) gp->Row = f_idx;
		}
	}
	catch (EAbort &e) {
		beep_Warn();
	}

	if (!is_DialogKey(Key)) Key = 0;
}

//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::OpenCheckBoxClick(TObject *Sender)
{
	EditHistGrid->SetFocus();
}

//---------------------------------------------------------------------------
//マウス操作
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::EditHistGridDblClick(TObject *Sender)
{
	perform_Key_RETURN((TControl*)Sender);
}

//---------------------------------------------------------------------------
//ソート
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::sort_MarkList()
{
	if (!isMark) return;

	switch (MarkSortMode) {
	case 0: case 1: case 4:
		DscNameOrder = (MarkSortOdr[MarkSortMode] == -1);	break;
	case 3:
		DscPathOrder = (MarkSortOdr[MarkSortMode] == -1);	break;
	case 2: case 5:
		OldOrder	 = (MarkSortOdr[MarkSortMode] == 1);	break;
	}

	switch (MarkSortMode) {
	case 0: HistBufList->CustomSort(SortComp_Name);		break;
	case 1: HistBufList->CustomSort(SortComp_Ext);		break;
	case 2: HistBufList->CustomSort(SortComp_Time);		break;
	case 3: HistBufList->CustomSort(SortComp_PathName);	break;
	case 4: HistBufList->CustomSort(SortComp_Memo);		break;
	case 5: HistBufList->CustomSort(SortComp_MarkTime);	break;
	}
}

//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::EditHistHeaderSectionClick(THeaderControl *HeaderControl,
		THeaderSection *Section)
{
	if (!isMark || Section->Index<1) return;

	int idx = Section->Index - 1;
	if (idx==MarkSortMode)
		MarkSortOdr[idx] = -MarkSortOdr[idx];
	else
		MarkSortMode = idx;

	EditHistHeader->Invalidate();

	UnicodeString fnam = get_CurFileName();
	sort_MarkList();
	UpdateGrid();

	TStringGrid *gp = EditHistGrid;
	idx = std::max(HistBufList->IndexOf(fnam), 0);
	if (idx<gp->RowCount) gp->Row = idx;
	gp->Invalidate();
	gp->SetFocus();
}

//---------------------------------------------------------------------------
//オプションの開閉
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::ChgOptBtnClick(TObject *Sender)
{
	BlankPanel->Visible = !BlankPanel->Visible;
	OptPanel->Visible	= !OptPanel->Visible;
	OptVisible = OptPanel->Visible;
	SetOptBtn();
}

//---------------------------------------------------------------------------
//メモの入力
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::MemoItemClick(TObject *Sender)
{
	int idx = EditHistGrid->Row;
	if (idx<0 || idx>=HistBufList->Count) return;

	file_rec *cfp = (file_rec*)HistBufList->Objects[idx];
	UnicodeString mstr = get_pre_tab(cfp->memo);
	UnicodeString fnam = HistBufList->Strings[idx];
	if (input_query_ex(ExtractFileName(fnam).c_str(), _T("メモ"), &mstr, 480)) {
		IniFile->FileMark(fnam, 1, mstr);
		cfp->memo = IniFile->GetMarkMemo(fnam);
		UpdateList();
	}
	CloseIME(Handle);
}
//---------------------------------------------------------------------------
//メモの削除
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::ClrMemoItemClick(TObject *Sender)
{
	int idx = EditHistGrid->Row;
	if (idx<0 || idx>=HistBufList->Count) return;

	file_rec *cfp = (file_rec*)HistBufList->Objects[idx];
	UnicodeString fnam = HistBufList->Strings[idx];
	IniFile->FileMark(fnam, 1, EmptyStr);
	cfp->memo = IniFile->GetMarkMemo(fnam);
	UpdateList();
}
//---------------------------------------------------------------------------
//マーク解除
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::DelMarkItemClick(TObject *Sender)
{
	if (!del_HistItem()) beep_Warn();
}
//---------------------------------------------------------------------------
//すべてのマークを解除
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::ClrAllMarkItemClick(TObject *Sender)
{
	if (msgbox_Sure(USTR_ClrAllMarkQ)) {
		IniFile->ClearAllMark();
		UpdateList();
	}
}

//---------------------------------------------------------------------------
//フィルタによる一覧の更新
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FilterEditChange(TObject *Sender)
{
	UpdateList();
}
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FilterEditEnter(TObject *Sender)
{
	FilterEdit->Color = scl_Window;
}
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FilterEditExit(TObject *Sender)
{
	CloseIME(Handle);
	InvColIfEmpty(FilterEdit);
}

//---------------------------------------------------------------------------
//フィルタ欄でのキー操作
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FilterEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);

	if		(contained_wd_i(KeysStr_ToList, KeyStr))	EditHistGrid->SetFocus();
	else if (MovGridFromFilter(EditHistGrid, KeyStr))	;
	else if (SameText(KeyStr, KeyStr_Migemo))			MigemoCheckBox->Checked = !MigemoCheckBox->Checked;
	else return;

	Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FilterEditKeyPress(TObject *Sender, System::WideChar &Key)
{
	if (is_KeyPress_CtrlNotCV(Key))
		Key = 0;
	else if (Key==VK_RETURN) {
		if (!set_FileName(EditHistGrid->Row)) Abort();
		CmdStr.sprintf(_T("%s"), (isMark && OpenCheckBox->Checked)? _T("OpenStandard") : null_TCHAR);
		ModalResult = mrClose;	//移動
		Key = 0;
	}
}

//---------------------------------------------------------------------------
//履歴をすべて削除
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::ClearAllItemClick(TObject *Sender)
{
	if (HistoryList && msgbox_Sure(USTR_DelHistoryQ)) {
		HistoryList->Clear();
		UpdateList();
	}
}
//---------------------------------------------------------------------------
//履歴に入れないパスの設定
//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::NoHistItemClick(TObject *Sender)
{
	UnicodeString *p = isView? &NoViewHistPath : &NoEditHistPath;
	if (input_query_ex(_T("履歴に入れないパスの設定"), null_TCHAR, p, 0, false, LoadUsrMsg(USTR_HintMltSepSC))) {
		if (!(*p).IsEmpty()) {
			TStringDynArray path_lst = split_strings_semicolon(*p);
			int idx = 0;
			while (idx<HistoryList->Count) {
				UnicodeString pnam = ExtractFilePath(get_csv_item(HistoryList->Strings[idx], 0));
				for (int i=0; i<path_lst.Length && !pnam.IsEmpty(); i++) {
					if (path_lst[i].IsEmpty()) continue;
					if (ContainsText(pnam, path_lst[i])) pnam = EmptyStr;
				}

				if (pnam.IsEmpty()) HistoryList->Delete(idx); else idx++;
			}

			UpdateList();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TEditHistoryDlg::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (SpecialKeyProc(this, Key, Shift)) return;
}
//---------------------------------------------------------------------------

