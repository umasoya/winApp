//----------------------------------------------------------------------//
// タグ名の検索															//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <memory>
#include <RegularExpressions.hpp>
#include "UserFunc.h"
#include "Global.h"
#include "FindTag.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFindTagForm *FindTagForm;

//---------------------------------------------------------------------------
__fastcall TFindTagForm::TFindTagForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::FormCreate(TObject *Sender)
{
	TagList    = new TStringList();
	ResListBuf = new TStringList();

	ListScrPanel = new UsrScrollPanel(ListPanel,  ResListBox, USCRPNL_FLAG_P_WP | USCRPNL_FLAG_L_WP);
}
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::FormShow(TObject *Sender)
{
	IniFile->LoadPosInfo(this, DialogCenter);
	IsMigemo = IniFile->ReadBoolGen(_T("FindTagMigemo"));

	TListBox *lp = ResListBox;
	set_StdListBox(lp, LBTAG_OPT_LCPY);
	set_UsrScrPanel(ListScrPanel);

	UnicodeString tit = "タグ名検索";
	if (!FileName.IsEmpty()) tit.cat_sprintf(_T(" - %s"), ExtractFileName(FileName).c_str());
	Caption = tit;

	IncSeaWord = EmptyStr;
	InpPanel->Font->Assign(ListFont);
	InpPanel->ClientHeight = get_FontHeight(ListFont, 4, 4);
	InpPanel->Color 	   = col_bgList;

	TagList->Clear();
	std::unique_ptr<TStringList> fbuf(new TStringList());
	if (load_text_ex(TagFile, fbuf.get())!=0) {
		fbuf->Sort();
		UnicodeString lst_tag;
		UnicodeString rnam = ExtractFilePath(TagFile);
		for (int i=0; i<fbuf->Count; i++) {
			UnicodeString lbuf = fbuf->Strings[i];
			if (StartsStr("!", lbuf)) continue;
			if (!FileName.IsEmpty() && !SameText(FileName, rel_to_absdir(get_tkn(get_tkn_r(lbuf, '\t'), '\t'), rnam))) continue;

			UnicodeString tnam = get_tkn(lbuf, "\t");
			if (!SameStr(lst_tag, tnam)) {
				TagList->Add(tnam);
				lst_tag = tnam;
			}
		}

		UpdateList();
	}
}
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::FormClose(TObject *Sender, TCloseAction &Action)
{
	ResListBuf->Clear();
	ResListBox->Count = 0;

	IniFile->SavePosInfo(this);
	IniFile->WriteBoolGen(_T("FindTagMigemo"), IsMigemo);
}
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::FormDestroy(TObject *Sender)
{
	delete TagList;
	delete ResListBuf;
	delete ListScrPanel;
}

//---------------------------------------------------------------------------
void __fastcall TFindTagForm::UpdateList()
{
	InpPaintBox->Repaint();

	cursor_HourGlass();
	filter_List(TagList, ResListBuf, IncSeaWord, IsMigemo, true);	//AND/OR検索有効
	cursor_Default();

	//リストボックスに割り当て(仮想)
	ResListBox->Count = ResListBuf->Count;
	if (ResListBox->Count>0) ResListBox->ItemIndex = 0;
	ResListBox->SetFocus();
	ListScrPanel->UpdateKnob();
}

//---------------------------------------------------------------------------
//一覧の描画
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::ResListBoxDrawItem(TWinControl *Control, int Index,
          TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox *)Control;
	TCanvas  *cv = lp->Canvas;

	cv->Font->Assign(ListFont);
	cv->Brush->Color = State.Contains(odSelected)? col_selItem : col_bgList;
	cv->FillRect(Rect);

	int xp = Rect.Left + 4;
	int yp = Rect.Top + get_TopMargin2(cv);
	cv->Font->Color = (State.Contains(odSelected) && col_fgSelItem!=clNone)? col_fgSelItem : col_fgList;

	UnicodeString lbuf = lp->Items->Strings[Index];
	if (!IncSeaWord.IsEmpty()) {
		std::unique_ptr<TStringList> wlist(new TStringList());
		get_MatchWordList(lbuf, IncSeaWord, IsMigemo, false, wlist.get());
		EmphasisTextOut(lbuf, wlist.get(), cv, xp, yp);
	}
	else {
		cv->TextOut(xp, yp, lbuf);
	}

	//カーソル
	draw_ListCursor(lp, Rect, Index, State);
}
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::ResListBoxData(TWinControl *Control, int Index, UnicodeString &Data)
{
	Data = (Index>=0 && Index<ResListBuf->Count)? ResListBuf->Strings[Index] : EmptyStr;
}

//---------------------------------------------------------------------------
//入力欄の描画
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::InpPaintBoxPaint(TObject *Sender)
{
	draw_InputPaintBox((TPaintBox*)Sender, (IsMigemo? "Migemo: " : "") + IncSeaWord);
}

//---------------------------------------------------------------------------
//確定
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::ResListBoxDblClick(TObject *Sender)
{
	int idx = ResListBox->ItemIndex;
	if (idx!=-1) TagName = ResListBox->Items->Strings[idx];
	ModalResult = !TagName.IsEmpty()? mrOk : mrCancel;
}

//---------------------------------------------------------------------------
//一覧でのキー操作
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::ResListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);		if (KeyStr.IsEmpty()) return;
	UnicodeString cmd_F  = Key_to_CmdF(KeyStr);
	UnicodeString cmd_V  = Key_to_CmdV(KeyStr);
	UnicodeString cmd_S  = KeyFuncList->Values["S:" + KeyStr];

	TListBox *lp = (TListBox*)Sender;
	bool handled = true;
	if (equal_ENTER(KeyStr))
		ResListBoxDblClick(Sender);
	else if (equal_ESC(KeyStr))
		ModalResult = mrCancel;
	else if (update_IncSeaWord(IncSeaWord, KeyStr))
		UpdateList();
	else if (USAME_TI(cmd_S, "ClearIncKeyword")) {
		IncSeaWord = EmptyStr;
		UpdateList();
	}
	else if (USAME_TI(cmd_S, "MigemoMode")) {
		IsMigemo = (!IsMigemo && usr_Migemo->DictReady);
		UpdateList();
	}
	else if (USAME_TI(cmd_F, "TextViewer")) {
		TagCmd.USET_T("VIEW");
		ResListBoxDblClick(Sender);
	}
	else if (USAME_TI(cmd_F, "FileEdit") || USAME_TI(cmd_V, "FileEdit")) {
		TagCmd.USET_T("EDIT");
		ResListBoxDblClick(Sender);
	}
	else if (ExeCmdListBox(lp, cmd_F) || ExeCmdListBox(lp, cmd_V)) {
		if (USAME_TI(cmd_V, "ClipCopy")) {
			TagCmd.USET_T("COPY");
			ModalResult = mrOk;
		}
	}
	else if (USAME_TI(cmd_S, "IncSearchUp"))	ListBoxCursorUp(lp);
	else if (USAME_TI(cmd_S, "IncSearchDown"))	ListBoxCursorDown(lp);
	else if (MovListBoxFromFilter(lp, KeyStr))	;
	else if (contained_wd_i(KeysStr_Popup, KeyStr)) show_PopupMenu(lp);
	else handled = false;

	if (handled) Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::ResListBoxKeyPress(TObject *Sender, System::WideChar &Key)
{
	//インクリメンタルサーチを回避
	if (_istalnum(Key) || Key==VK_SPACE || Key==VK_RETURN) Key = 0;
}

//---------------------------------------------------------------------------
//ビュアーでダイレクトタグジャンプ
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::TagViewActionExecute(TObject *Sender)
{
	TagCmd.USET_T("VIEW");
	ResListBoxDblClick(Sender);
}
//---------------------------------------------------------------------------
//エディタでダイレクトタグジャンプ
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::TagJumpActionExecute(TObject *Sender)
{
	TagCmd.USET_T("EDIT");
	ResListBoxDblClick(Sender);
}
//---------------------------------------------------------------------------
//コピー
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::CopyActionExecute(TObject *Sender)
{
	ExeCmdListBox(ResListBox, _T("ClipCopy"));

	TagCmd.USET_T("COPY");
	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
//すべて選択
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::SelAllActionExecute(TObject *Sender)
{
	ListBoxSelectAll(ResListBox);
}
//---------------------------------------------------------------------------
void __fastcall TFindTagForm::PopupItemUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = (ResListBox->ItemIndex!=-1);
}

//---------------------------------------------------------------------------
void __fastcall TFindTagForm::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	SpecialKeyProc(this, Key, Shift);
}
//---------------------------------------------------------------------------

