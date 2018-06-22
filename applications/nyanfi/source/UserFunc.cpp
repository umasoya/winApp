//----------------------------------------------------------------------//
// 汎用共通関数															//
//																		//
//----------------------------------------------------------------------//
#include <windows.h>
#include <setupapi.h>
#include <cfgmgr32.h>

#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <algorithm>
#include <memory>
#include <math.h>
#include <mmreg.h>
#include <mmsystem.h>
#include <utilcls.h>
#include <imagehlp.h>
#include <wininet.h>
#include <System.StrUtils.hpp>
#include <System.IOUtils.hpp>
#include <System.DateUtils.hpp>
#include <Vcl.FileCtrl.hpp>
#include <RegularExpressions.hpp>
#include <IdURI.hpp>
#include "usr_file_ex.h"
#include "usr_shell.h"
#include "usr_exif.h"
#include "htmconv.h"
#include "UserFunc.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

#pragma link "win32\\release\\psdk\\wininet.lib"

//---------------------------------------------------------------------------
//システム色
TColor scl_Window;
TColor scl_WindowText;
TColor scl_Highlight;
TColor scl_HighlightText;
TColor scl_BtnFace;
TColor scl_BtnText;
TColor scl_Menu;
TColor scl_MenuText;

//---------------------------------------------------------------------------
//コードページリスト
//---------------------------------------------------------------------------
const UnicodeString CodePageList =
	"Shift_JIS=932\n"
	"ISO-8859-1=1252\n"
	"UTF-16=1200\n"
	"UTF-16BE=1201\n"
	"US-ASCII=20127\n"
	"EUC-JP=20932\n"
	"ISO-2022-JP=50220\n"
	"UTF-7=65000\n"
	"UTF-8=65001\n";

const UnicodeString CodePageListX =
	"Shift_JIS=932\n"
	"Shift-JIS=932\n"
	"X-SJIS=932\n"
	"ISO-8859-1=1252\n"
	"UTF-16=1200\n"
	"UTF-16BE=1201\n"
	"US-ASCII=20127\n"
	"EUC-JP=20932\n"
	"X-EUC-JP=20932\n"
	"ISO-2022-JP=50220\n"
	"UTF-7=65000\n"
	"UTF-8=65001\n";


//---------------------------------------------------------------------------
//システム色の初期化
//---------------------------------------------------------------------------
void InitializeSysColor()
{
	scl_Window		  = clWindow;
	scl_WindowText	  = clWindowText;
	scl_Highlight	  = clHighlight;
	scl_HighlightText = clHighlightText;
	scl_BtnFace		  = clBtnFace;
	scl_BtnText		  = clBtnText;
	scl_Menu		  = clMenu;
	scl_MenuText	  = clMenuText;
}

//---------------------------------------------------------------------------
//マウスカーソル位置のウィンドウを取得
//---------------------------------------------------------------------------
HWND get_window_from_pos()
{
	POINT p;
	::GetCursorPos(&p);
	return ::WindowFromPoint(p);
}

//---------------------------------------------------------------------------
//コントロールに右クリックメニューを表示
//---------------------------------------------------------------------------
void show_PopupMenu(TPopupMenu *mp, TControl *cp)
{
	if (mp && cp) {
		TPoint p = cp->ClientToScreen(Point(16, 16));
		mp->Popup(p.x, p.y);
	}
}
//---------------------------------------------------------------------------
void show_PopupMenu(TListBox *lp)
{
	show_PopupMenu(lp->PopupMenu, lp);
}
//---------------------------------------------------------------------------
void show_PopupMenu(TStringGrid *gp)
{
	show_PopupMenu(gp->PopupMenu, gp);
}

//---------------------------------------------------------------------------
//ListBox にテキストを設定
//---------------------------------------------------------------------------
void set_ListBoxText(TListBox *lp, const _TCHAR *s)
{
	lp->Items->Text = s;
}
//---------------------------------------------------------------------------
//ComboBox にテキストを設定
//---------------------------------------------------------------------------
void set_ComboBoxText(TComboBox *cp, const _TCHAR *s)
{
	cp->Items->Text = s;
}

//---------------------------------------------------------------------------
//ComboBox に履歴を追加
//---------------------------------------------------------------------------
void add_ComboBox_history(TComboBox *cp, UnicodeString kwd)
{
	if (cp) {
		if (kwd.IsEmpty()) kwd = cp->Text;
		if (!kwd.IsEmpty()) {
			TStrings *lp = cp->Items;
			int idx = lp->IndexOf(kwd);
			if (idx!=0) {
				if (idx>0) lp->Delete(idx);
				lp->Insert(0, kwd);	//先頭に追加
			}
			cp->Text = kwd;
		}
	}
}
//---------------------------------------------------------------------------
//ComboBox から履歴を削除
//---------------------------------------------------------------------------
int del_ComboBox_history(TComboBox *cp, UnicodeString kwd)
{
	int idx = -1;
	if (cp) {
		if (kwd.IsEmpty()) kwd = cp->Text;
		if (!kwd.IsEmpty()) {
			TStrings *lp = cp->Items;
			idx = lp->IndexOf(kwd);
			if (idx!=-1) lp->Delete(idx);
			cp->Text = EmptyStr;
		}
	}
	return idx;
}

//---------------------------------------------------------------------------
//IMEを閉じる
//---------------------------------------------------------------------------
void CloseIME(HWND hWnd)
{
	HIMC hIMC = ::ImmGetContext(hWnd);
	if (hIMC) {
		if (::ImmGetOpenStatus(hIMC)) ::ImmSetOpenStatus(hIMC, FALSE);
		::ImmReleaseContext(hWnd, hIMC);
	}
}

//---------------------------------------------------------------------------
//リストボックスの項目位置にマウスカーソルを移動
//---------------------------------------------------------------------------
TPoint get_ListItemPos(TListBox *lp)
{
	return lp->ClientToScreen(Point(120, (lp->ItemIndex - lp->TopIndex) * lp->ItemHeight + lp->ItemHeight/2));
}

//---------------------------------------------------------------------------
void pos_ListBoxItem(TListBox *lp)
{
	Mouse->CursorPos = get_ListItemPos(lp);;
}

//---------------------------------------------------------------------------
//ユーザ定義メッセージの EAbort を送出
//---------------------------------------------------------------------------
void UserAbort(unsigned id)
{
	throw EAbort(LoadUsrMsg(id));
}
//---------------------------------------------------------------------------
void SysErrAbort(unsigned id)
{
	throw EAbort(SysErrorMessage(id));
}
//---------------------------------------------------------------------------
void LastErrAbort()
{
	throw EAbort(SysErrorMessage(GetLastError()));
}
//---------------------------------------------------------------------------
void TextAbort(const _TCHAR *msg)
{
	throw EAbort(UnicodeString(msg));
}
//---------------------------------------------------------------------------
void SkipAbort()
{
	throw EAbort("SKIP");
}

//---------------------------------------------------------------------------
//エラーメッセージ
//---------------------------------------------------------------------------
void msgbox_ERR(UnicodeString msg)
{
	if (msg.IsEmpty()) return;
	MessageDlg(msg, mtError, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void msgbox_ERR(unsigned id)
{
	msgbox_ERR(LoadUsrMsg(id));
}

//---------------------------------------------------------------------------
//警告メッセージ
//---------------------------------------------------------------------------
void msgbox_WARN(UnicodeString msg)
{
	if (msg.IsEmpty()) return;
	MessageDlg(msg, mtWarning, TMsgDlgButtons() << mbOK, 0);
}
//---------------------------------------------------------------------------
void msgbox_WARN(unsigned id)
{
	msgbox_WARN(LoadUsrMsg(id));
}

//---------------------------------------------------------------------------
//メッセージ
//---------------------------------------------------------------------------
void msgbox_OK(UnicodeString msg, UnicodeString tit)
{
	Application->MessageBox(msg.c_str(),
		def_if_empty(tit, _T("確認")).c_str(), MB_OK);
}
//---------------------------------------------------------------------------
//確認メッセージ
//---------------------------------------------------------------------------
bool msgbox_Y_N(UnicodeString msg, UnicodeString tit)
{
	int res = Application->MessageBox(msg.c_str(),
				def_if_empty(tit, _T("確認")).c_str(), MB_YESNO|MB_ICONQUESTION);
	return (res==IDYES);
}
//---------------------------------------------------------------------------
int msgbox_Y_N_C(UnicodeString msg, UnicodeString tit)
{
	return Application->MessageBox(msg.c_str(),
		def_if_empty(tit, _T("確認")).c_str(), MB_YESNOCANCEL|MB_ICONQUESTION);
}

//---------------------------------------------------------------------------
//指定幅に収まるようにパス名を調整
//---------------------------------------------------------------------------
UnicodeString get_MiniPathName(UnicodeString pnam, int wd, TFont *font)
{
	HWND hWnd = Application->ActiveFormHandle;
	HDC hDc = ::GetDC(hWnd);
	if (hDc) {
		std::unique_ptr<TCanvas> cv(new TCanvas());
		cv->Handle = hDc;
		cv->Font->Assign(font);
		bool is_irreg = IsIrregularFont(cv->Font);
		bool ends_dlmt = EndsStr("\\", pnam);
		while (get_TextWidth(cv.get(), pnam, is_irreg)>wd) {
			TStringDynArray plst = split_path(pnam);
			bool changed = false;
			for (int i=0; i<plst.Length; i++) {
				UnicodeString dnam = plst[i];
				if (USTARTS_TS('<', dnam)) continue;
				if (!UENDS_TS("…", dnam) && dnam.Length()>4) {
					plst[i] = dnam.SubString(1, dnam.Length() - 3).UCAT_T("…");
					changed = true;
					break;
				}
				if (UENDS_TS("…", dnam) && dnam.Length()>2) {
					plst[i].Delete(dnam.Length() - 1, 1);
					changed = true;
					break;
				}
				if (i>0 && i<plst.Length-1 && UENDS_TS("…", dnam) && dnam.Length()==2) {
					plst[i] = "…";
					changed = true;
					break;
				}
			}
			if (!changed) break;
			pnam = EmptyStr;
			for (int i=0; i<plst.Length; i++) pnam += UAPP_T(plst[i], "\\");
			if (!ends_dlmt) pnam = ExcludeTrailingPathDelimiter(pnam);
		}
		::ReleaseDC(hWnd, hDc);
	}
	return pnam;
}

//---------------------------------------------------------------------------
//指定キャンバスのフォントが内部レディングを持っているか？
//---------------------------------------------------------------------------
bool has_Leading(TCanvas *cv)
{
	return (cv->TextHeight("Q")>abs(cv->Font->Height));
}

//---------------------------------------------------------------------------
//指定フォントの描画高を取得
//---------------------------------------------------------------------------
int get_FontHeight(TFont *font, int mgn, int min_mgn)
{
	std::unique_ptr<Graphics::TBitmap> bmp(new Graphics::TBitmap());
	TCanvas *cv = bmp->Canvas;
	cv->Font->Assign(font);
	int fh = cv->TextHeight("Q");
	if (mgn>0) {
		//内部レディングと相殺
		int ld = fh - abs(cv->Font->Height);
		if (ld>0) mgn -= ld;
		if (mgn<min_mgn) mgn = min_mgn;
		fh += mgn;
	}
	return fh;
}
//---------------------------------------------------------------------------
//指定フォント n 文字(半角単位)の描画幅を取得
//---------------------------------------------------------------------------
int get_CharWidth_Font(TFont *font, int n)
{
	std::unique_ptr<Graphics::TBitmap> bmp(new Graphics::TBitmap());
	TCanvas *cv = bmp->Canvas;
	cv->Font->Assign(font);
	return get_CharWidth(cv, n);
}

//---------------------------------------------------------------------------
//入力欄の文字列を数値に
//---------------------------------------------------------------------------
int EditToInt(TLabeledEdit *ep,
	int def)	//デフォルト値 (default = 0);
{
	return ep->Text.ToIntDef(def);
}
//---------------------------------------------------------------------------
int EditToInt(TEdit *ep, int def)
{
	return ep->Text.ToIntDef(def);
}

//---------------------------------------------------------------------------
//サイズの数値を文字列に(TBまで)
//---------------------------------------------------------------------------
UnicodeString get_size_str_T(__int64 sz, int dg)
{
	UnicodeString szstr;
	float sz_k = sz/1024.0;
	float sz_m = sz_k/1024.0;
	float sz_g = sz_m/1024.0;

	if      (sz_g>1024) szstr.sprintf(_T("%.*f TB"), dg, sz_g/1024.0);
	else if (sz_m>1024) szstr.sprintf(_T("%.*f GB"), dg, sz_m/1024.0);
	else if (sz_k>1024) szstr.sprintf(_T("%.*f MB"), dg, sz_k/1024.0);
	else				szstr.sprintf(_T("%.*f KB"), dg, sz_k);
	return szstr;
}
//---------------------------------------------------------------------------
//サイズの数値を文字列に(GBまで)
//---------------------------------------------------------------------------
UnicodeString get_size_str_G(
	__int64 sz,
	int wd,		//桁指定 default = 10
	int dg)
{
	UnicodeString szstr;
	if (sz>=0) {
		if		(sz>=1073741824ull)	szstr.sprintf(_T("%5.*f GB"), dg, 1.0*sz/1073741824ull);
		else if (sz>=1048576ul)		szstr.sprintf(_T("%5.*f MB"), dg, 1.0*sz/1048576ul);
		else						szstr = FormatFloat(",0", (double)sz);
	}
	return ins_spc_length(szstr, wd);
}
//---------------------------------------------------------------------------
UnicodeString get_size_str_G(__int64 sz, int wd, int dg, int fmt_mode)
{
	UnicodeString szstr;
	if (sz>=0) {
		if		(sz>=1073741824ull)	szstr.sprintf(_T("%5.*f GB"), dg, 1.0*sz/1073741824ull);
		else if (sz>=1048576ul)		szstr.sprintf(_T("%5.*f MB"), dg, 1.0*sz/1048576ul);
		else {
			if ((fmt_mode==1 && sz>=1024) || fmt_mode==2)
				szstr.sprintf(_T("%5.*f KB"), dg, 1.0*sz/1024);
			else
				szstr = FormatFloat(",0", (double)sz);
		}
	}
	return ins_spc_length(szstr, wd);
}

//---------------------------------------------------------------------------
UnicodeString get_size_str_B(
	__int64 sz,
	int wd)		//桁指定 default = 14
{
	return ins_spc_length(((sz>=0)? FormatFloat(",0", (double)sz) : EmptyStr), wd);
}
//---------------------------------------------------------------------------
UnicodeString get_size_str_C(int sz)
{
	return FormatFloat(",0", (double)sz);
}

//---------------------------------------------------------------------------
//文字列を TDateTime に変換
//---------------------------------------------------------------------------
bool str_to_DateTime(UnicodeString s, TDateTime *dt)
{
	try {
		*dt = VarToDateTime(to_HalfWidth(Trim(s)));
		return true;
	}
	catch (...) {
		return false;
	}
}

//---------------------------------------------------------------------------
//日時文字列を取得
//---------------------------------------------------------------------------
UnicodeString format_DateTime(
	TDateTime dt,	//日時
	bool omit)		//同日なら日付を省略 (default = false)
{
	if (omit && IsToday(dt))
		return FormatDateTime("hh:nn:ss", dt);
	else
		return FormatDateTime("yyyy/mm/dd hh:nn:ss", dt);
}
//---------------------------------------------------------------------------
//日付文字列を取得
//---------------------------------------------------------------------------
UnicodeString format_Date(TDateTime dt)
{
	return FormatDateTime("yyyy/mm/dd", dt);
}

//---------------------------------------------------------------------------
//日時文字列を取得 (FormatDateTime の拡張)
// $EN がある場合ロケールを en-US にする
//---------------------------------------------------------------------------
UnicodeString format_DateTimeEx(UnicodeString fmt, TDateTime dt)
{
    TFormatSettings fs = remove_text(fmt, _T("$EN"))? TFormatSettings::Create("en-US") : TFormatSettings::Create();
	return FormatDateTime(fmt, dt, fs);
}

//---------------------------------------------------------------------------
//日付条件を取得
//　相対指定: {<|=|>}-n{D|M|Y}　　(D:日、M:月、Y:年)
//　絶対指定: {<|=|>}yyyy/mm/dd
//
//  戻り値 : 1: < より古い/ 2: = 同じ/ 3: > より新しい/ 0: 条件なし
//			-1: エラー
//---------------------------------------------------------------------------
int get_DateCond(UnicodeString prm, TDateTime &dt)
{
	try {
		int cnd = 0;

		if (!prm.IsEmpty()) {
			cnd = UnicodeString("<=>").Pos(prm[1]);
			if (cnd>0) {
				prm.Delete(1, 1);
				//絶対指定
				if (prm.Length()==10 && prm[5]=='/' && prm[8]=='/') {
					dt = TDate(prm);
				}
				//相対指定
				else {
					UnicodeString ut_str = prm.SubString(prm.Length(), 1).UpperCase();
					if (!ContainsText("DMY", ut_str)) Abort();
					delete_end(prm);
					dt = Date();
					int dn = prm.ToIntDef(0);
					if (dn!=0) {
						switch (idx_of_word_i(_T("D|M|Y"), ut_str)) {
						case 0: dt = IncDay(  dt, dn); break;
						case 1: dt = IncMonth(dt, dn); break;
						case 2: dt = IncYear( dt, dn); break;
						}
					}
				}
			}
			else cnd = -1;
		}

		return cnd;
	}
	catch (...) {
		return -1;
	}
}

//---------------------------------------------------------------------------
//幅 × 高 文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_wd_x_hi_str(int w, int h)
{
	return UnicodeString().sprintf(_T("%u × %u"), w, h);
}

//---------------------------------------------------------------------------
//画像サイズ情報文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_img_size_str(int w, int h, UnicodeString name)
{
	if (name.IsEmpty()) name = "画像サイズ";
	UnicodeString infstr = make_PropLine(name, get_wd_x_hi_str(w, h));
	if (w>0 && h>0) infstr.cat_sprintf(_T("  (%s)"), get_AspectStr(w, h).c_str());	//アスペクト比
	return infstr;
}

//---------------------------------------------------------------------------
//コードページの名前を取得
//---------------------------------------------------------------------------
UnicodeString get_NameOfCodePage(int code_page)
{
	UnicodeString ret_str;
	if (code_page>0) {
		std::unique_ptr<TStringList> lst(new TStringList());
		lst->Text = CodePageList;
		for (int i=0; i<lst->Count; i++) {
			if (lst->ValueFromIndex[i].ToIntDef(0)==code_page) {
				ret_str = lst->Names[i];  break;
			}
		}
		if (ret_str.IsEmpty()) ret_str.sprintf(_T("%u"), code_page);
	}
	return ret_str;
}
//---------------------------------------------------------------------------
//名前のコードページを取得
//---------------------------------------------------------------------------
int get_CodePageOfName(UnicodeString code_name)
{
	std::unique_ptr<TStringList> lst(new TStringList());
	lst->Text = CodePageListX;
	return lst->Values[code_name].ToIntDef(0);
}

//---------------------------------------------------------------------------
//結果カウントを文字列に
//---------------------------------------------------------------------------
UnicodeString get_res_cnt_str(int ok_cnt, int er_cnt, int sk_cnt, int ng_cnt)
{
	UnicodeString ret_str;
	if (ok_cnt>0) ret_str.cat_sprintf(_T("  OK:%u"),	ok_cnt);
	if (ng_cnt>0) ret_str.cat_sprintf(_T("  NG:%u"),	ng_cnt);
	if (er_cnt>0) ret_str.cat_sprintf(_T("  ERR:%u"),	er_cnt);
	if (sk_cnt>0) ret_str.cat_sprintf(_T("  SKIP:%u"),	sk_cnt);
	return ret_str;
}

//---------------------------------------------------------------------------
//結果リストの整形
//  "ファイル名\t内容" --> "ファイル名   内容"
//---------------------------------------------------------------------------
int format_res_list(TStringList *lst, int mgn, int min_wd)
{
	//ファイル名の最大幅を取得
	int max_len = 0;
	for (int i=0; i<lst->Count; i++) max_len_half(max_len, get_pre_tab(lst->Strings[i]));
	max_len += mgn;
	if (max_len<min_wd) max_len = min_wd;

	//結果を整形
	for (int i=0; i<lst->Count; i++) {
		UnicodeString lbuf = lst->Strings[i];
		lst->Strings[i] = align_l_str(get_pre_tab(lbuf) ,max_len, get_post_tab(lbuf));
	}

	return max_len;
}

//---------------------------------------------------------------------------
//ファイルのプロパティを表示
//---------------------------------------------------------------------------
void ShowPropertyDialog(UnicodeString fnam)
{
	SHELLEXECUTEINFO sei;
	::ZeroMemory(&sei, sizeof(sei));
	sei.cbSize = sizeof(sei);
	sei.lpFile = fnam.c_str();
	sei.lpVerb = _T("properties");
	sei.fMask  = SEE_MASK_INVOKEIDLIST | SEE_MASK_NOCLOSEPROCESS;
	ShellExecuteEx(&sei);
}

//---------------------------------------------------------------------------
//現在位置のリスト項目を削除
//---------------------------------------------------------------------------
void delete_ListItem(TCustomListBox *lp)
{
	if (lp) {
		int idx = lp->ItemIndex;
		if (idx!=-1) {
			lp->Items->Delete(idx);
			lp->ItemIndex = (idx<lp->Count)? idx : lp->Count - 1;
		}
	}
}

//---------------------------------------------------------------------------
//リストボックスの項目に罫線を描画 (背景の反転色)
//---------------------------------------------------------------------------
void draw_ListItemLine(TCustomListBox *lp, int idx)
{
	TPenMode org_md = lp->Canvas->Pen->Mode;
	int		 org_wd = lp->Canvas->Pen->Width;
	lp->Canvas->Pen->Mode  = pmNot;
	lp->Canvas->Pen->Width = 2;
	if (idx!=-1) {
		TRect r = lp->ItemRect(idx);
		int   y = (lp->ItemIndex<idx)? r.Bottom : r.Top;
		lp->Canvas->MoveTo(0, y);
		lp->Canvas->LineTo(lp->Width, y);
	}
	lp->Canvas->Pen->Mode  = org_md;
	lp->Canvas->Pen->Width = org_wd;
}

//---------------------------------------------------------------------------
//端点がフラットな線を描画 (w=1 の場合はそのまま)
//---------------------------------------------------------------------------
void draw_Line(
	TCanvas *cv,
	int x0, int y0, int x1, int y1,
	int w,				//線幅
	TColor col, 		//色
	TPenStyle style)	// (default = psSolid)
{
	if (w>1) {
		int c = ColorToRGB(col);
		LOGBRUSH l_brush;
		l_brush.lbStyle = BS_SOLID;
		l_brush.lbColor = RGB(GetRValue(c), GetGValue(c), GetBValue(c));
		l_brush.lbHatch = 0;
		DWORD p_style = PS_GEOMETRIC|PS_ENDCAP_FLAT|PS_JOIN_MITER;

		switch (style) {
		case psDot:  p_style |= PS_DOT;  break;
		case psDash: p_style |= PS_DASH; break;
		default:     p_style |= PS_SOLID;
		}

		HPEN hPen = ::ExtCreatePen(p_style, w, &l_brush, 0, 0);
		HPEN hOldPen = (HPEN)::SelectObject(cv->Handle, hPen);
		cv->Pen->Handle = hPen;
		cv->MoveTo(x0, y0);  cv->LineTo(x1, y1);
		::SelectObject(cv->Handle, hOldPen);
		::DeleteObject(hPen);
	}
	else {
		cv->Pen->Mode  = pmCopy;
		cv->Pen->Style = style;
		cv->Pen->Color = col;
		cv->Pen->Width = w;
		cv->MoveTo(x0, y0);  cv->LineTo(x1, y1);
	}
}

//---------------------------------------------------------------------------
//背景色(Brush)の輝度を考慮してセパレータを描画
//---------------------------------------------------------------------------
void draw_Separator(TCanvas *cv, TRect rc, TColor bg)
{
	int yp = rc.Top + rc.Height()/2;
	int x0 = rc.Left  + 4;
	int x1 = rc.Right - 4;

	double v  = GetLuminance((bg==clNone)? cv->Brush->Color : bg);
	TColor c0 = TColor((v>0.5)? RGB(0x77, 0x77, 0x77) : RGB(0x33, 0x33, 0x33));	//***
	TColor c1 = TColor((v>0.5)? RGB(0xdd, 0xdd, 0xdd) : RGB(0x99, 0x99, 0x99));	//***

	cv->Pen->Style = psSolid;
	cv->Pen->Width = 1;
	cv->Pen->Color = c0;	cv->MoveTo(x0, yp);  cv->LineTo(x1, yp++);
	cv->Pen->Color = c1;	cv->MoveTo(x0, yp);  cv->LineTo(x1, yp);
}
//---------------------------------------------------------------------------
//背景色(Brush)を考慮して分割線を描画
//---------------------------------------------------------------------------
void draw_separateLine(TCanvas *cv, TRect rc,
	int pos)	//位置 0=上辺/ 1=中央/ 2=下辺 (default = 0)
{
	cv->Pen->Color = SelectWorB(cv->Brush->Color, 0.25);	//***
	cv->Pen->Style = psSolid;
	cv->Pen->Width = 1;

	switch (pos) {
	case 1:
		cv->MoveTo(rc.Left  + 2, rc.Top + rc.Height()/2);
		cv->LineTo(rc.Right - 2, rc.Top + rc.Height()/2);
		break;
	case 2:
		cv->MoveTo(rc.Left  + 2, rc.Bottom - 1);
		cv->LineTo(rc.Right - 2, rc.Bottom - 1);
		break;
	default:
		cv->MoveTo(rc.Left,      rc.Top);
		cv->LineTo(rc.Right + 1, rc.Top);
	}
}

//---------------------------------------------------------------------------
//ソート方向マークを描画
//---------------------------------------------------------------------------
void draw_SortMark(TCanvas *cv, int x, int y,
	bool is_asc,	//昇順
	TColor fg)		//マーク色
{
	TPoint mrk[3];
	//▲
	if (is_asc) {
		mrk[0] = Point(x,	  y + 3);
		mrk[1] = Point(x + 6, y + 3);
		mrk[2] = Point(x + 3, y);
	}
	//▼
	else {
		mrk[0] = Point(x,	  y);
		mrk[1] = Point(x + 6, y);
		mrk[2] = Point(x + 3, y + 3);
	}
	cv->Pen->Style   = psSolid;
	cv->Pen->Color	 = fg;
	cv->Brush->Style = bsSolid;
	cv->Brush->Color = fg;
	cv->Polygon(mrk, 2);
}

//---------------------------------------------------------------------------
//ボタンにマークを設定
//---------------------------------------------------------------------------
void set_ButtonMark(TSpeedButton *bp, int id, TColor fg)
{
	int size;
	switch (id) {
	case UBMK_HTOP: case UBMK_HEND: case UBMK_LEFT: case UBMK_RIGHT:
		size = 16;	break;
	default:
		size = 9;
	}

	Graphics::TBitmap *bmp = bp->Glyph;
	TCanvas *cv = bmp->Canvas;
	bmp->SetSize(size, size);
	bmp->Transparent = true;
	cv->Brush->Color = scl_BtnFace;
	cv->FillRect(TRect(0, 0, size, size));
	cv->Pen->Style = psSolid;
	cv->Pen->Color = fg;

	switch (id) {
	case UBMK_UP:
		draw_SortMark(cv, 0, 2, true,  fg);	break;
	case UBMK_DOWN:
		draw_SortMark(cv, 0, 2, false, fg);	break;

	case UBMK_BUP:
		cv->Pen->Width = 2;
		cv->MoveTo(1, 5);  cv->LineTo(4, 2);
		cv->MoveTo(7, 5);  cv->LineTo(4, 2);
		break;
	case UBMK_BDOWN:
		cv->Pen->Width = 2;
		cv->MoveTo(1, 2);  cv->LineTo(4, 5);
		cv->MoveTo(7, 2);  cv->LineTo(4, 5);
		break;

	case UBMK_VUP:
		cv->Pen->Width = 2;
		cv->MoveTo(1, 7);  cv->LineTo(4, 4);
		cv->MoveTo(7, 7);  cv->LineTo(4, 4);
		break;
	case UBMK_VDOWN:
		cv->Pen->Width = 2;
		cv->MoveTo(1, 2);  cv->LineTo(4, 5);
		cv->MoveTo(7, 2);  cv->LineTo(4, 5);
		break;

	case UBMK_VTOP:
		cv->Pen->Width = 1;
		cv->MoveTo(0, 2);  cv->LineTo(9, 2);
		cv->MoveTo(0, 3);  cv->LineTo(9, 3);
		cv->Pen->Width = 2;
		cv->MoveTo(1, 7);  cv->LineTo(4, 4);
		cv->MoveTo(7, 7);  cv->LineTo(4, 4);
		break;
	case UBMK_VEND:
		cv->Pen->Width = 1;
		cv->MoveTo(0, 7);  cv->LineTo(9, 7);
		cv->MoveTo(0, 6);  cv->LineTo(9, 6);
		cv->Pen->Width = 2;
		cv->MoveTo(1, 2);  cv->LineTo(4, 5);
		cv->MoveTo(7, 2);  cv->LineTo(4, 5);
		break;

	case UBMK_HTOP:
		cv->Pen->Width = 1;
		cv->MoveTo(4, 1);  cv->LineTo( 4, 14);
		cv->Pen->Width = 2;
		cv->MoveTo(6, 7);  cv->LineTo(11,  2);
		cv->MoveTo(6, 7);  cv->LineTo(11, 12);
		break;
	case UBMK_HEND:
		cv->Pen->Width = 1;
		cv->MoveTo(11, 1);  cv->LineTo(11, 14);
		cv->Pen->Width = 2;
		cv->MoveTo(9, 7);  cv->LineTo(4,  2);
		cv->MoveTo(9, 7);  cv->LineTo(4, 12);
		break;
	case UBMK_LEFT:
		cv->Pen->Width = 2;
		cv->MoveTo(6, 7);  cv->LineTo(11,  2);
		cv->MoveTo(6, 7);  cv->LineTo(11, 12);
		break;
	case UBMK_RIGHT:
		cv->Pen->Width = 2;
		cv->MoveTo(9, 7);  cv->LineTo(4,  2);
		cv->MoveTo(9, 7);  cv->LineTo(4, 12);
		break;
	}
}

//---------------------------------------------------------------------------
//縮小比率を取得
//---------------------------------------------------------------------------
double get_ZoomRatio(int vw, int vh, int iw, int ih, int mgn, double limit)
{
	if (iw==0 || ih==0) return 1.0;

	if (mgn>0) {
		vw -= mgn*2;
		vh -= mgn*2;
	}

	double r;
	bool v_is_v = (vh>vw);
	bool i_is_v = (ih>iw);
	if ((v_is_v && i_is_v) || (!v_is_v && !i_is_v))
		//縦横関係が同じ
		r = std::max(std::min(1.0 * vw/ih, 1.0 * vh/iw), std::min(1.0 * vw/iw, 1.0 * vh/ih));
	else
		//縦横関係が違う
		r = std::min(std::min(1.0 * vw/ih, 1.0 * vh/iw), std::min(1.0 * vw/iw, 1.0 * vh/ih));

	if (r>limit) r = 1.0;

	return r;
}

//---------------------------------------------------------------------------
//キーリストの作成
//---------------------------------------------------------------------------
void make_KeyList(TStrings *lst)
{
	lst->Clear();

	UnicodeString itmbuf;
	for (int i=0; i<26; i++) lst->Add(itmbuf.sprintf(_T("%c"), 'A'+i));		//英字
	for (int i=0; i<10; i++) lst->Add(itmbuf.sprintf(_T("%c"), '0'+i));		//数字

	for (int i=0; i<12; i++) lst->Add(itmbuf.sprintf(_T("F%u"), i + 1));	//Fキー
	lst->AddStrings(
		SplitString("Del|Ins|BkSp|Left|Right|UP|Down|PgUp|PgDn|Home|End|Pause|Tab|Esc|Enter|Space|App|-", "|"));
	lst->Add((::GetKeyboardType(0)==7)? "^" : "'");
	lst->AddStrings(SplitString("\\|@|[|;|:|]|,|.|/|＼", "|"));
	for (int i=0; i<10; i++) lst->Add(itmbuf.sprintf(_T("10Key_%u"), i));	//10キー
	lst->AddStrings(SplitString("10Key_*|10Key_+|10Key_-|10Key_/|10Key_.", "|"));
}

//---------------------------------------------------------------------------
//キーが押されているか?
//---------------------------------------------------------------------------
bool is_KeyDown(WORD key)
{
	return (HIBYTE(::GetAsyncKeyState(key)) != 0);
}
//---------------------------------------------------------------------------
//Ctrl+C,V 以外の Ctrl+～ が押された？
//---------------------------------------------------------------------------
bool is_KeyPress_CtrlNotCV(WORD key)
{
	return (is_KeyDown(VK_CONTROL) && key!=0x03 && key!=0x16);
}
//---------------------------------------------------------------------------
//Ctrl+C,V,X 以外の Ctrl+～ が押された？
//---------------------------------------------------------------------------
bool is_KeyPress_CtrlNotCVX(WORD key)
{
	return (is_KeyDown(VK_CONTROL) && key!=0x03 && key!=0x16 && key!=0x18);
}

//---------------------------------------------------------------------------
//ダイアログキーか？
//---------------------------------------------------------------------------
bool is_DialogKey(WORD Key)
{
	return (Key==VK_LEFT || Key==VK_RIGHT || Key==VK_UP || Key==VK_DOWN ||
			Key==VK_PRIOR || Key==VK_NEXT || Key==VK_HOME || Key==VK_END || Key==VK_TAB);
}

//---------------------------------------------------------------------------
//WPARAM からシフト状態を取得
//---------------------------------------------------------------------------
int get_shift_from_wparam(WPARAM prm)
{
	int kmd = 0;
	int key = GET_KEYSTATE_WPARAM(prm);
	if (key & MK_CONTROL) kmd |= 1;
	if (key & MK_SHIFT)   kmd |= 2;
	return kmd;
}

//---------------------------------------------------------------------------
//HTML文書のヘッダ文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_html_header(UnicodeString fnam, int code_page)
{
	UnicodeString headstr;

	try {
		std::unique_ptr<TStringList> fbuf(new TStringList());
		if (code_page>0) {
			std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
			fbuf->LoadFromFile(fnam, enc.get());
		}
		else
			fbuf->LoadFromFile(fnam);

		int p = pos_i(_T("</HEAD>"), fbuf->Text);
		if (p>0) headstr = fbuf->Text.SubString(1, p + 6);
	}
	catch (...) {
		headstr = EmptyStr;
	}

	return headstr;
}

//---------------------------------------------------------------------------
//HTML文書のヘッダ情報(charset, TITLE, META)を取得
//---------------------------------------------------------------------------
void get_HtmlInf(
	UnicodeString fnam,		//HTMLファイル名
	TStringList *lst)		//[o] 情報リスト(名前=値 の形式で格納)
{
	if (!file_exists(fnam)) return;

	//ヘッダ部分を取得
	UnicodeString headstr = get_html_header(fnam);
	if (!headstr.IsEmpty()) {
		UnicodeString itmbuf;

		//文書型宣言
		UnicodeString dtd_str = get_tkn(get_tkn_r(headstr, _T("<!DOCTYPE ")), '>');
		int p = pos_i(_T("//DTD "),  dtd_str);
		if (p>0) dtd_str = get_tkn_m(dtd_str, _T("//DTD "), _T("//"));
		if (!dtd_str.IsEmpty()) {
			if (USAME_TI(dtd_str, "html")) dtd_str = "HTML5";
			lst->Add(itmbuf.sprintf(_T("%*s: %s"), FPRP_NAM_WD, _T("DOCTYPE"), dtd_str.c_str()));
		}

		//charset をチェック
		UnicodeString charset;
		p = pos_i(_T("charset"), headstr);
		if (p>0) {
			UnicodeString tmp = headstr.SubString(p, headstr.Length() - p + 1);
			tmp = get_tkn(get_tkn_m(tmp, '=', '>'), '/');
			charset = Trim(REPLACE_TS(tmp, "\"", ""));
			if (!charset.IsEmpty())
				lst->Add(itmbuf.sprintf(_T("%*s: %s"), FPRP_NAM_WD, _T("charset"), charset.c_str()));
		}

		//エンコーディングを指定して再読み込み
		if (!charset.IsEmpty()) {
			int code_page = get_CodePageOfName(charset);
			if (code_page>0) headstr = get_html_header(fnam, code_page);
		}

		//コメント部分を削除
		int p0, p1;
		p0 = headstr.Pos("<!--");
		while (p0>0) {
			p1 = headstr.Pos("-->");
			if (p1>p0) {
				headstr.Delete(p0, p1 - p0 + 3);
				p0 = headstr.Pos("<!--");
			}
			else {
				headstr = headstr.SubString(1, p1 - 1);  break;
			}
		}

		//タイトル
		p0 = pos_i(_T("<TITLE>"),  headstr);
		p1 = pos_i(_T("</TITLE>"), headstr);
		if (p0>0 && p1>p0) {
			UnicodeString tit = headstr.SubString(p0 + 7, p1 - p0 - 7);
			if (!tit.IsEmpty()) lst->Add(itmbuf.sprintf(_T("%*s: %s"), FPRP_NAM_WD, _T("title"), tit.c_str()));
		}

		//メタ情報
		std::unique_ptr<HtmConv> htmcnv(new HtmConv());
		UnicodeString tmp = headstr;
		while (!tmp.IsEmpty()) {
			int p = tmp.Pos('<');  if (p==0) break;
			tmp.Delete(1, p - 1);
			UnicodeString tstr = tmp;
			p = tstr.Pos('>');  if (p==0) break;
			tstr = tstr.SubString(1, p);
			UnicodeString nstr = htmcnv->GetTagAtr(tstr, "META", _T("NAME"));
			if (!nstr.IsEmpty()) {
				UnicodeString cstr = htmcnv->GetTagAtr(tstr, "META", _T("CONTENT"));
				if (!cstr.IsEmpty()) lst->Add(itmbuf.sprintf(_T("%*s: %s"), FPRP_NAM_WD, nstr.c_str(), cstr.c_str()));
			}
			tmp.Delete(1, tstr.Length());
		}
	}
}

//---------------------------------------------------------------------------
//Jpeg 情報(形式、カラーなど)を取得
//---------------------------------------------------------------------------
void get_JpgExInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst)		//[o] 情報リスト
{
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		std::unique_ptr<BYTE[]> buf(new BYTE[8]);
		for (int i=0; ; i++) {
			unsigned short mkr = EXIF_get_data_2(fs.get(), true);
			int p = fs->Position;
			if ((mkr & 0xff00)!=0xff00) Abort();
			if (i==0) {
				if (mkr==0xffd8) continue; else Abort();
			}
			if (mkr==0xffd9)  Abort();
			int slen = EXIF_get_data_2(fs.get(), true);
			//SOF0
			if (mkr==0xffc0 || mkr==0xffc2) {
				add_PropLine(_T("形式"), ((mkr==0xffc0)? "ベースライン" : "プログレッシブ"), lst);
				fs->Seek(5, soFromCurrent);

				BYTE n;	//構成要素数
				if (fs->Read(&n, 1)!=1) Abort();
				//グレースケール
				if (n==1) {
					add_PropLine(_T("カラー"), "グレースケール", lst);
				}
				//YCbCr
				else if (n==3) {
					BYTE hn[3] = {0};	//水平サンプリング値
					BYTE vn[3] = {0};	//垂直サンプリング値
					for (int j=0; j<3; j++) {
						if (fs->Read(buf.get(), 3)!=3) Abort();
						int id = buf[0];	if (id<1 || id>3) Abort();
						hn[id - 1] = (buf[1]>>4) & 0x0f;
						vn[id - 1] = buf[1] & 0x0f;
					}

					UnicodeString lbuf;
					lbuf.UCAT_T(" (");
					for (int j=0; j<3; j++) lbuf.cat_sprintf(_T("%*ux%u"), ((j>0)? 2 : 1), hn[j], vn[j]);
					lbuf.UCAT_T(")");

					//※CbCrが11でない場合に対応(ここはちょっと怪しい)
					if (hn[0]%2==0 && hn[1]%2==0 && hn[2]%2==0) for (int j=0; j<3; j++) hn[j] /= 2;
					if (vn[0]%2==0 && vn[1]%2==0 && vn[2]%2==0) for (int j=0; j<3; j++) vn[j] /= 2;

					if (hn[1]==1 && hn[2]==1 && vn[1]==1 && vn[2]==1) {
						switch ((hn[0] << 4) | vn[0]) {
						case 0x11: lbuf.Insert("444", 1); break;
						case 0x12: lbuf.Insert("440", 1); break;
						case 0x14: lbuf.Insert("441", 1); break;
						case 0x21: lbuf.Insert("422", 1); break;
						case 0x22: lbuf.Insert("420", 1); break;
						case 0x24: lbuf.Insert("421", 1); break;
						case 0x41: lbuf.Insert("411", 1); break;
						case 0x42: lbuf.Insert("410", 1); break;
						default: Abort();
						}
					}

					add_PropLine(_T("YCbCr"), lbuf, lst);
				}
				//CMYK
				else if (n==4) {
					add_PropLine(_T("カラー"), "CMYK", lst);
				}
				else Abort();
				break;
			}
			fs->Seek(p + slen, soFromBeginning);
		}
	}
	catch (...) {
		;
	}
}

//---------------------------------------------------------------------------
//Exif情報の取得
//---------------------------------------------------------------------------
bool get_ExifInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst,		//[o] 情報リスト
	int *ori,				//[o] 方向	(default = NULL)
	unsigned int *i_wd,		//[o] 幅	(default = NULL)
	unsigned int *i_hi)		//[o] 高さ	(default = NULL)
{
	std::unique_ptr<TStringList> i_lst(new TStringList());
	if (!EXIF_GetInf(fnam, i_lst.get())) return false;

	UnicodeString fext = get_extension(fnam);
	unsigned int iw, ih;
	if (Exif_GetImgSize(i_lst.get(), fext, &iw, &ih)) {
		lst->Add(get_img_size_str(iw, ih));
		if (i_wd && i_hi) { *i_wd = iw; *i_hi = ih; }
	}

	std::unique_ptr<TStringList> d_lst(new TStringList());
	d_lst->Text =
		"271=メーカー名\n"
		"272=モデル\n"
		"36867=撮影日時\n"
		"33437=絞り値\n"
		"33434U=露出時間\n"
		"37380=露光補正\n"
		"34850L=露出プログラム\n"
		"34855=ISO感度\n"
		"37383=測光方式\n"
		"37386=焦点距離\n"
		"NK:5=ＷＢ\n"
		"CN:4.7=ＷＢ\n"
		"37385=フラッシュ\n"
		"274L=画像方向\n"
		"NK:7=フォーカス\n"
		"42036=レンズ\n"
		"305=ソフト名\n"
		"FOVbVer=FOVb Ver.\n"
		"GPS:2=GPS緯度\n"
		"GPS:4=GPS経度\n";

	for (int i=0; i<d_lst->Count; i++)
		add_PropLine_if(d_lst->ValueFromIndex[i].c_str(), i_lst->Values[d_lst->Names[i]], lst);

	if (ori) *ori = get_ListIntVal(i_lst.get(), _T("274"));
	return true;
}

//---------------------------------------------------------------------------
//Exif情報文字列(簡略)の取得
//---------------------------------------------------------------------------
UnicodeString get_ExifInfStr(
	UnicodeString fnam,		//ファイル名
	int *ori,				//[o] 方向	(default = NULL)
	unsigned int *i_wd,		//[o] 幅	(default = NULL)
	unsigned int *i_hi)		//[o] 高さ	(default = NULL)
{
	std::unique_ptr<TStringList> i_lst(new TStringList());
	if (!EXIF_GetInf(fnam, i_lst.get())) return EmptyStr;

	UnicodeString fext = get_extension(fnam);
	if (i_wd && i_hi) Exif_GetImgSize(i_lst.get(), fext, i_wd, i_hi);

	struct {
		const _TCHAR *id;
		const _TCHAR *fmt;
	} inf_list[] = {
		{_T("33437"), _T(" F%s")}, {_T("33434U"), _T(" %s")}, {_T("34855"), _T(" ISO%s")},
		{_T("37380"), _T(" EV%s")}, {_T("37386"), _T(" %s")}
	};

	UnicodeString ret_str, vstr;
	int cnt = sizeof(inf_list)/sizeof(inf_list[0]);
	for (int i=0; i<cnt; i++) {
		vstr = i_lst->Values[inf_list[i].id];
		if (!vstr.IsEmpty()) ret_str.cat_sprintf(inf_list[i].fmt, vstr.c_str());
	}

	if (ori) *ori = get_ListIntVal(i_lst.get(), _T("274"));
	return ret_str;
}

//---------------------------------------------------------------------------
//Exif情報文字列(サムネイル用)の取得
//---------------------------------------------------------------------------
UnicodeString get_ExifInfThumb(
	UnicodeString fnam,		//ファイル名
	int *ori)				//[o] 方向
{
	std::unique_ptr<TStringList> i_lst(new TStringList());
	if (!EXIF_GetInf(fnam, i_lst.get())) return EmptyStr;

	struct {
		const _TCHAR *id;
		const _TCHAR *fmt;
	} inf_list[] = {
		{_T("33437"), _T("F%s")}, {_T("33434"), _T(" %s")}, {_T("34855"), _T(" %s")}, {_T("37380"), _T(" %s")}
	};

	UnicodeString ret_str, vstr;
	int cnt = sizeof(inf_list)/sizeof(inf_list[0]);
	for (int i=0; i<cnt; i++) {
		vstr = i_lst->Values[inf_list[i].id];
		if (!vstr.IsEmpty()) ret_str.cat_sprintf(inf_list[i].fmt, vstr.c_str());
	}

	//方向
	if (ori) *ori = get_ListIntVal(i_lst.get(), _T("274"));

	return ret_str;
}

//---------------------------------------------------------------------------
//GPS情報(緯度、経度)を取得
//---------------------------------------------------------------------------
bool get_GpsInf(
	UnicodeString fnam,		//ファイル名
	double *lat,			//[o] 緯度
	double *lng)			//[o] 経度
{
	std::unique_ptr<TStringList> xlst(new TStringList());
	if (!EXIF_GetInf(fnam, xlst.get())) return false;
	UnicodeString lat_str = Trim(get_tkn(xlst->Values["GPS:2"], '('));	//緯度
	UnicodeString lng_str = Trim(get_tkn(xlst->Values["GPS:4"], '('));	//経度
	if (lat_str.IsEmpty() || lng_str.IsEmpty()) return false;

	try {
		*lat = lat_str.ToDouble();
		*lng = lng_str.ToDouble();
		return true;
	}
	catch (...) {
		return false;
	}
}

//---------------------------------------------------------------------------
//ビッグエンディアンからリトルエンディアンへ変換
//---------------------------------------------------------------------------
unsigned int big_to_little(BYTE *p)
{
	return (unsigned int)((p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]);
}

//---------------------------------------------------------------------------
//ファイルの現在位置に指定文字列があるか？
//  あれば直後位置へ、無ければ現在位置のまま
//---------------------------------------------------------------------------
bool check_read(TFileStream *fs, const char *s)
{
	int size = strlen(s);
	if (size==0) return false;

	int p0 = fs->Seek(0, soFromCurrent);
	try {
		std::unique_ptr<BYTE[]> buf(new BYTE[size]);
		if (fs->Read(buf.get(), size)!=size) Abort();
		for (int i=0; i<size; i++) if (s[i]!=buf[i]) Abort();
		return true;
	}
	catch (EAbort &e) {
		fs->Seek(p0, soFromBeginning);
		return false;
	}
}

//---------------------------------------------------------------------------
//ファイルの現在位置からチャンクを検索
//  あれば直後位置(サイズ)へ
//---------------------------------------------------------------------------
bool sea_chunk(TFileStream *fs, const char *s)
{
	int size = strlen(s);
	if (size!=4) return false;

	try {
		std::unique_ptr<BYTE[]> buf(new BYTE[size]);

		for (;;) {
			if (fs->Read(buf.get(), size)!=size) Abort();
			bool ok = true;
			for (int i=0; i<size && ok; i++) {
				if (!isprint(buf[i])) Abort();
				if (s[i]!=buf[i]) ok = false;
			}
			if (ok) break;

			DWORD dwLen;
			if (fs->Read(&dwLen, 4)<4) Abort();
			if (dwLen<2) Abort();
			int p = fs->Seek(0, soFromCurrent) + dwLen;
			fs->Seek(p, soFromBeginning);
		}
		return true;
	}
	catch (EAbort &e) {
		return false;
	}
}

//---------------------------------------------------------------------------
//ファイルの現在位置からチャンク情報を取得
//  戻り値: 識別子		EmptyStr: エラー
//---------------------------------------------------------------------------
UnicodeString get_chunk_hdr(
	TFileStream *fs,
	unsigned int *size)		//[o] チャンクサイズ
{
	UnicodeString ret_str;

	char sbuf[8];
	::ZeroMemory(&sbuf, sizeof(sbuf));
	if (fs->Read(&sbuf, 4)!=4)	return EmptyStr;
	ret_str = UnicodeString(sbuf);
	if (ret_str.Length()!=4)	return EmptyStr;

	BYTE buf[8];
	if (fs->Read(&buf, 4)!=4) return EmptyStr;
	if (size) *size = (unsigned int)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24));

	return ret_str;
}

//---------------------------------------------------------------------------
//アイコンファイルの情報を取得
//---------------------------------------------------------------------------
void get_IconInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst)		//[o] 情報リスト
{
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		fs->Seek(4, soFromBeginning);

		unsigned short ico_n;
		if (fs->Read(&ico_n, 2)<2) UserAbort(USTR_IllegalFormat);
		add_PropLine(_T("アイコン数"), ico_n, lst);

		for (int i=0; i<ico_n; i++) {
			//サイズ
			unsigned char b_buf;
			if (fs->Read(&b_buf, 1)!=1) UserAbort(USTR_IllegalFormat);
			int wd = (b_buf==0)? 256 : b_buf;
			if (fs->Read(&b_buf, 1)!=1) UserAbort(USTR_IllegalFormat);
			int hi = (b_buf==0)? 256 : b_buf;
			UnicodeString lbuf;
			lbuf.sprintf(_T("%*u: %s"), FPRP_NAM_WD, i + 1, get_wd_x_hi_str(wd, hi).c_str());
			//色
			if (fs->Read(&b_buf, 1)!=1) UserAbort(USTR_IllegalFormat);
			if (b_buf==0)
		 		lbuf.UCAT_T("  256色以上");
			else
		 		lbuf.cat_sprintf(_T("  %u色"), b_buf);
			lst->Add(lbuf);
			fs->Seek(13, soFromCurrent);
		}
	}
	catch(EAbort &e) {
		add_list_errmsg(lst, e.Message);
	}
	catch (...) {
		add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}
}

//---------------------------------------------------------------------------
//WAVファイルの情報を取得
//---------------------------------------------------------------------------
void get_WavInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst)		//[o] 情報リスト
{
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));

		if (!USAME_TS(get_chunk_hdr(fs.get()), "RIFF") || !check_read(fs.get(), "WAVE")) UserAbort(USTR_IllegalFormat);
		if (!sea_chunk(fs.get(),  "fmt "))	UserAbort(USTR_IllegalFormat);

		DWORD dwBuf;
		if (fs->Read(&dwBuf, 4)<4)			UserAbort(USTR_IllegalFormat);
		int p_data = fs->Seek(0, soFromCurrent) + dwBuf;

		//フォーマット情報を取得
		PCMWAVEFORMAT pwf;
		fs->Read(&pwf, sizeof(PCMWAVEFORMAT));
		if (pwf.wf.wFormatTag==WAVE_FORMAT_PCM || pwf.wf.wFormatTag==WAVE_FORMAT_EXTENSIBLE) {
			if (pwf.wf.nChannels==0) TextAbort(_T("不正なチャンネル数です。"));
			int chn    = pwf.wf.nChannels;
			int s_rate = pwf.wf.nSamplesPerSec;
			int s_byte = pwf.wf.nBlockAlign/chn;
			int s_bit  = s_byte * 8;

			fs->Seek(p_data, soFromBeginning);
			if (!sea_chunk(fs.get(), "data")) TextAbort(_T("dataチャンクが見つかりません。"));
			fs->Read(&dwBuf, 4);
			int length = (int)(dwBuf/(s_byte*chn)/(s_rate/1000.0));

			UnicodeString lbuf = get_PropTitle(_T("形式")).cat_sprintf(_T("%uHz %ubit "), s_rate, s_bit);
			if (chn<=2)
				lbuf.cat_sprintf(_T("%s"), (chn==2)? _T("ステレオ") : _T("モノ"));
			else
				lbuf.cat_sprintf(_T("%uch"), chn);
			lst->Add(lbuf);
			add_PropLine(_T("長さ"), mSecToTStr(length), lst);
		}
		else if (pwf.wf.wFormatTag==WAVE_FORMAT_MPEGLAYER3)
			add_PropLine(_T("形式"), "MPEG 3 Layer 1", lst);
		else
			lst->Add(get_PropTitle(_T("形式")).cat_sprintf(_T("その他(0x%04X)"), pwf.wf.wFormatTag));
	}
	catch (EAbort &e) {
		add_list_errmsg(lst, e.Message);
	}
	catch (...) {
		add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}
}

//---------------------------------------------------------------------------
//Opusファイルの情報を取得
//---------------------------------------------------------------------------
unsigned int get_Ogg_seg_size(TFileStream *fs)
{
	unsigned int seg_sz = 0;
	BYTE seg_n, seg_d;
	if (fs->Read(&seg_n, 1)==1) {
		std::unique_ptr<BYTE[]> sbuf(new BYTE[seg_n]);
		if (fs->Read(sbuf.get(), seg_n)==seg_n) {
			for (int i=0; i<seg_n; i++) seg_sz += sbuf[i];
		}
	}
	return seg_sz;
}
//---------------------------------------------------------------------------
UnicodeString get_OpusStr(TFileStream *fs)
{
	UnicodeString ret_str;
	DWORD s_len;
	if (fs->Read(&s_len, 4)==4) {
		std::unique_ptr<BYTE[]> sbuf(new BYTE[s_len + 2]);
		if (fs->Read(sbuf.get(), (int)s_len)==(int)s_len) {
			TBytes b;  b.Length = s_len;
			for (DWORD i=0; i<s_len; i++) b[i] = sbuf[i];
			ret_str = TEncoding::UTF8->GetString(b, 0, b.Length);
		}
	}
	return Trim(ret_str);
}
//---------------------------------------------------------------------------
void get_OpusInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst)		//[o] 情報リスト
{
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));

		if (!check_read(fs.get(), "OggS"))	Abort();
		fs->Seek(22, soFromCurrent);

		int seg_sz = get_Ogg_seg_size(fs.get());  if (seg_sz==0) Abort();
		int p = fs->Seek(0, soFromCurrent);

		if (!check_read(fs.get(), "OpusHead")) Abort();
		fs->Seek(1, soFromCurrent);		//version
		BYTE n;
		if (fs->Read(&n, 1)!=1) Abort();
		add_PropLine(_T("チャンネル数"), n, lst);
		fs->Seek(2, soFromCurrent);		//pre-skip
		DWORD i_rate;
		if (fs->Read(&i_rate, 4)!=4) Abort();
		add_PropLine(_T("サンプルレート"), UnicodeString().sprintf(_T("%uHz"), i_rate), lst);

		fs->Seek(p + seg_sz, soFromBeginning);
		if (check_read(fs.get(), "OggS")) fs->Seek(22, soFromCurrent);

		seg_sz = get_Ogg_seg_size(fs.get());  if (seg_sz==0) Abort();
		if (!check_read(fs.get(), "OpusTags")) Abort();

		//Vendor String
		UnicodeString s = get_OpusStr(fs.get());
		if (!s.IsEmpty()) add_PropLine(_T("Vendor"), s, lst);

		//User Comment
		DWORD l_len;
		if (fs->Read(&l_len, 4)<4) Abort();
		for (DWORD i=0; i<l_len; i++) {
			UnicodeString lbuf = get_OpusStr(fs.get());
			UnicodeString tnam = split_tkn(lbuf, '=');
			if (!lbuf.IsEmpty()) {
				TStringDynArray s_lst = SplitString(tnam, "_");
				tnam = EmptyStr;
				for (int i=0; i<s_lst.Length; i++) {
					if (s_lst[i].IsEmpty()) continue;
					tnam += s_lst[i].SubString(1, 1).UpperCase() + exclude_top(s_lst[i]).LowerCase();
				}
				add_PropLine(tnam, lbuf, lst);
			}
		}
	}
	catch (EAbort &e) {
		lst->Clear();
	}
	catch (...) {
		add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}
}

//---------------------------------------------------------------------------
//CDAファイルの情報を取得
//---------------------------------------------------------------------------
bool get_CdaInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst,		//[o] 情報リスト
	int *len)				//[o] 長さ
{
	bool ret = false;
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));

		if (!USAME_TS(get_chunk_hdr(fs.get()), "RIFF") || !check_read(fs.get(), "CDDA")) Abort();
		if (!sea_chunk(fs.get(), "fmt ")) Abort();

		DWORD dwBuf;
		if (fs->Read(&dwBuf, 4)<4)	Abort();
		if (dwBuf!=24)				Abort();

		fs->Seek(2, soFromCurrent);
		//トラック＃
		WORD wBuf;
		fs->Read(&wBuf, 2);
		if (lst) add_PropLine(_T("トラック＃"), wBuf, lst);
		fs->Seek(4, soFromCurrent);
		//開始位置
		fs->Read(&dwBuf, 4);
		int t = dwBuf*1000/75;	//= セクタ数*1000/75 (75=1秒間のセクタ数)
		if (lst) add_PropLine(_T("開始位置"), mSecToTStr(t), lst);
		//長さ
		fs->Read(&dwBuf, 4);
		t = dwBuf*1000/75;		//= セクタ数*1000/75 (75=1秒間のセクタ数)
		if (len) *len = t;
		if (lst) add_PropLine(_T("長さ"), mSecToTStr(t), lst);
		ret = true;
	}
	catch (EAbort &e) {
		if (lst) add_list_errmsg(lst, LoadUsrMsg(USTR_IllegalFormat));
	}
	catch (...) {
		if (lst) add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}
	return ret;
}

//---------------------------------------------------------------------------
//メタファイルの情報を取得
//---------------------------------------------------------------------------
void get_MetafileInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst)		//[o] 情報リスト
{
	try {
		std::unique_ptr<TMetafile> mbuf(new TMetafile());
		mbuf->LoadFromFile(fnam);
		lst->Add(get_img_size_str(mbuf->Width, mbuf->Height, "サイズ"));
		add_PropLine_if(_T("作成者"), mbuf->CreatedBy,   lst);
		add_PropLine_if(_T("説明"),   mbuf->Description, lst);
	}
	catch (...) {
		add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}
}

//---------------------------------------------------------------------------
//PNGファイルの情報を取得
//---------------------------------------------------------------------------
bool get_PngInf(
	UnicodeString fnam, 	//ファイル名
	TStringList *lst,		//[o] 情報リスト
	unsigned int *i_wd,		//[o] 幅	(default = NULL)
	unsigned int *i_hi)		//[o] 高さ	(default = NULL)
{
	bool ret = false;
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		std::unique_ptr<BYTE[]> buf(new BYTE[16]);
		//識別部
		if (fs->Read(buf.get(), 1)<1 || buf[0]!=0x89)	UserAbort(USTR_IllegalFormat);
		if (!check_read(fs.get(), "PNG\r\n\x1a\n"))		UserAbort(USTR_IllegalFormat);
		//チャンク解析
		for (;;) {
			if (fs->Read(buf.get(), 4)<4) break;
			int size = big_to_little(buf.get());
			if (fs->Read(buf.get(), 4)!=4) break;
			buf[4] = 0;
			UnicodeString name = (char*)buf.get();
			if (USAME_TS(name, "IEND")) break;
			//イメージヘッダ
			if (USAME_TS(name, "IHDR")) {
				//サイズ
				if (fs->Read(buf.get(), 4)<4) break;
				int iw = big_to_little(buf.get());
				if (fs->Read(buf.get(), 4)<4) break;
				int ih = big_to_little(buf.get());
				lst->Add(get_img_size_str(iw, ih));
				if (i_wd && i_hi) { *i_wd = iw; *i_hi = ih; }
				//ビット深度
				if (fs->Read(buf.get(), 1)<1) break;
				add_PropLine(_T("ビット深度"), (int)buf[0], lst);
				//カラータイプ
				if (fs->Read(buf.get(), 1)<1) break;
				UnicodeString coltyp = get_PropTitle(_T("カラータイプ"));
				switch (buf[0]) {
				case 0: coltyp.UCAT_T("グレースケール");			break;
				case 2: coltyp.UCAT_T("トゥルーカラー");			break;
				case 3: coltyp.UCAT_T("インデックスカラー");		break;
				case 4: coltyp.UCAT_T("グレースケール + アルファ");	break;
				case 6: coltyp.UCAT_T("トゥルーカラー + アルファ");	break;
				}
				lst->Add(coltyp);
				fs->Seek(3, soFromCurrent);
			}
			//テキスト
			else if (USAME_TS(name, "tEXt")) {
				std::unique_ptr<BYTE[]> sbuf(new BYTE[size + 2]);
				if (fs->Read(sbuf.get(), size)!=size) break;
				int p = -1;
				for (int i=0; i<size && p==-1; i++) if (sbuf[i]==0) p = i + 1;
				if (p>0 && p<size) {
					sbuf[size] = 0;
					std::unique_ptr<TStringList> vlst(new TStringList());
					vlst->Text = (char*)&sbuf[p];
					int i = 0;
					while (i<vlst->Count) if (Trim(vlst->Strings[i]).IsEmpty()) vlst->Delete(i); else i++;
					add_PropLine((char*)&sbuf[0], Trim(vlst->Text), lst);
				}
			}
			//イメージガンマ
			else if (USAME_TS(name, "gAMA")) {
				if (fs->Read(buf.get(), 4)<4) break;
				lst->Add(get_PropTitle(_T("イメージガンマ")).cat_sprintf(_T("%-7.6g"), big_to_little(buf.get())/100000.0));
			}
			//ICCプロファイル
			else if (USAME_TS(name, "iCCP")) {
				std::unique_ptr<BYTE[]> sbuf(new BYTE[size]);
				if (fs->Read(sbuf.get(), size)!=size) break;
				add_PropLine(_T("ICCプロファイル"), (char*)&sbuf[0], lst);
			}
			else fs->Seek(size, soFromCurrent);
			//CRC をスキップ
			fs->Seek(4, soFromCurrent);
		}

		ret = true;
	}
	catch (EAbort &e) {
		add_list_errmsg(lst, e.Message);
	}
	catch (...) {
		add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}
	return ret;
}
//---------------------------------------------------------------------------
//PNG情報文字列(簡略)の取得
//---------------------------------------------------------------------------
UnicodeString get_PngInfStr(
	UnicodeString fnam,		//ファイル名
	unsigned int *i_wd,		//[o] 幅	(default = NULL)
	unsigned int *i_hi)		//[o] 高さ	(default = NULL)
{
	UnicodeString ret_str;

	std::unique_ptr<TStringList> i_lst(new TStringList());
	unsigned int iw, ih;
	if (!get_PngInf(fnam, i_lst.get(), &iw, &ih)) return EmptyStr;

	if (i_wd && i_hi) {
		*i_wd = iw;
		*i_hi = ih;
	}
	else
		ret_str = get_wd_x_hi_str(iw, ih).UCAT_T(" ");

	UnicodeString col_s, bit_s;
	for (int i=0; i<i_lst->Count; i++) {
		UnicodeString lbuf = i_lst->Strings[i];
		if		(contains_s(lbuf, _T("カラータイプ: "))) col_s = Trim(get_tkn_r(lbuf, ':'));
		else if (contains_s(lbuf, _T("ビット深度: ")))	 bit_s = "  深度:" + Trim(get_tkn_r(lbuf, ':'));
	}

	ret_str += (col_s + bit_s);
	return ret_str;
}

//---------------------------------------------------------------------------
//Gifファイルの情報を取得
//---------------------------------------------------------------------------
bool get_GifInf(
	UnicodeString fnam,		//ファイル名
	TStringList  *lst,		//[o] 情報リスト	(NULLでも可)
	unsigned int *i_wd,		//[o] 幅			(default = NULL)
	unsigned int *i_hi,		//[o] 高さ			(default = NULL)
	int          *i_cnt)	//[o] フレーム数	(default = NULL)
{
	bool ret = false;
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		if (fs->Size<14) UserAbort(USTR_IllegalFormat);
		std::unique_ptr<BYTE[]> buf(new BYTE[16]);
		BYTE bt;
		//バージョン
		fs->Read(buf.get(), 6);
		buf[6] = '\0';
		UnicodeString vstr = (char*)buf.get();
		if (!USTARTS_TS("GIF", vstr)) UserAbort(USTR_IllegalFormat);
		//サイズ
		fs->Read(buf.get(), 4);
		unsigned int iw = (buf[1] << 8) + buf[0];
		unsigned int ih = (buf[3] << 8) + buf[2];
		if (i_wd && i_hi) { *i_wd = iw; *i_hi = ih; }
		//バレット情報
		fs->Read(&bt, 1);
		bool has_ct = (bt & 0x80);
		int ct_size = 2 << (bt & 0x07);
		if (lst) {
			add_PropLine(_T("バージョン"), vstr, lst);
			lst->Add(get_img_size_str(iw, ih));	//"画像サイズ"
			lst->Add(get_PropTitle(_T("カラーテーブル")).cat_sprintf(_T("%s"), has_ct? _T("あり") : _T("なし")));
			add_PropLine(_T("ビットの深さ"), ((bt >> 4) & 0x07) + 1, lst);
			add_PropLine(_T("色数"), ct_size, lst);
		}
		fs->Seek(2, soFromCurrent);
		if (has_ct) fs->Seek(ct_size * 3, soFromCurrent);

		//ブロック
		int img_cnt = 0;
		int t_cnt = 0;
		while (fs->Read(&bt, 1)==1) {
			BYTE blk_sz;
			//拡張ブロック
			if (bt==0x21) {
				fs->Read(&bt, 1);
				if (bt==0xf9) {	//Graphic Control Extension
					if (fs->Read(&blk_sz, 1)!=1 || blk_sz!=4) UserAbort(USTR_IllegalFormat);
					if (fs->Read(buf.get(), 4)!=4)  UserAbort(USTR_IllegalFormat);
					t_cnt += ((buf[2] << 8) + buf[1]);
				}
				while (fs->Read(&blk_sz, 1)==1 && blk_sz>0) fs->Seek(blk_sz, soFromCurrent);
			}
			//データブロック
			else if (bt==0x2c) {
				img_cnt++;
				fs->Seek(8, soFromCurrent);
				fs->Read(&bt, 1);
				if (bt & 0x80) fs->Seek((2 << (bt & 0x07)) * 3, soFromCurrent);
				fs->Seek(1, soFromCurrent);
				while (fs->Read(&blk_sz, 1)==1 && blk_sz>0) fs->Seek(blk_sz, soFromCurrent);
			}
			else break;
		}

		if (lst) {
			if (img_cnt>1) add_PropLine(_T("フレーム数"), img_cnt, lst);
			if (t_cnt>0)   lst->Add(get_PropTitle(_T("長さ")).cat_sprintf(_T("%.2f 秒"), t_cnt/100.0));
		}

		if (i_cnt) *i_cnt = img_cnt;
		ret = true;
	}
	catch (EAbort &e) {
		if (lst) add_list_errmsg(lst, e.Message);
	}
	catch (...) {
		if (lst) add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}

	return ret;
}

//---------------------------------------------------------------------------
//WebPファイルの情報を取得
//  参考: https://developers.google.com/speed/webp/docs/riff_container
//---------------------------------------------------------------------------
bool get_WebpInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst,		//[o] 情報リスト
	unsigned int *i_wd,		//[o] 幅
	unsigned int *i_hi)		//[o] 高さ
{
	bool ret = false;
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));

		if (!USAME_TS(get_chunk_hdr(fs.get()), "RIFF") || !check_read(fs.get(), "WEBP")) UserAbort(USTR_IllegalFormat);

		std::unique_ptr<BYTE[]> buf(new BYTE[16]);
		unsigned int iw = 0, ih = 0, i_cnt = 0, t_cnt = 0;
		bool has_alph = false;
		int  lst_top  = lst->Count;
		unsigned int   ui_buf;
		unsigned short us_buf;
		UnicodeString tmp, fmt, meta;

		while (fs->Position < fs->Size) {
			unsigned int  chunk_sz;
			UnicodeString chunk = get_chunk_hdr(fs.get(), &chunk_sz);
			if (chunk.IsEmpty()) UserAbort(USTR_IllegalFormat);
			int p = fs->Seek(0, soFromCurrent);

			if (USAME_TI(chunk, "VP8 ")) {
				fmt.USET_T("ロッシー");
				fs->Seek(6, soFromCurrent);
				if (iw==0 && fs->Read(&ui_buf, 4)==4) {
					iw = (ui_buf & 0x3fff);
					ih = ((ui_buf >> 16) & 0x3fff);
				}
			}
			else if (USAME_TI(chunk, "VP8L")) {
				fmt.USET_T("ロスレス");
				if (fs->Read(buf.get(), 1)!=1 || buf[0]!=0x2f) UserAbort(USTR_IllegalFormat);
				if (iw==0 && fs->Read(&ui_buf, 4)==4) {
					iw = (ui_buf & 0x3fff) + 1;
					ih = ((ui_buf >> 14) & 0x3fff) + 1;
				}
			}
			else if (USAME_TI(chunk, "VP8X")) {
				fs->Seek(4, soFromCurrent);
				if (iw==0 && fs->Read(buf.get(), 6)==6) {
					iw = (buf[0] | (buf[1] << 8) | (buf[2] << 16)) + 1;
					ih = (buf[3] | (buf[4] << 8) | (buf[5] << 16)) + 1;
				}
			}
			else if (USAME_TI(chunk, "ICCP")) {
				lst->Add(get_PropTitle(_T("ICCP")).UCAT_T("有り"));
			}
			else if (USAME_TI(chunk, "ALPH")) {
				has_alph = true;
				/* ※仕様と合わない?
				if (fs->Read(buf.get(), 4)!=4) UserAbort(USTR_IllegalFormat);
				tmp = ((buf[0]>>2) & 0x01)? "Level Reduction" : "None";
				add_PropLine(_T("α前処理"), tmp, lst);

				switch ((buf[0]>>4) & 0x03) {
				case  1: tmp = "Horizontal"; break;
				case  2: tmp = "Vertical"; break;
				case  3: tmp = "Gradient"; break;
				default: tmp = "None";
				}
				add_PropLine(_T("αフィルタ"), tmp, lst);

				tmp = ((buf[0]>>6) & 0x01)? "Lossless" : "None";
				add_PropLine(_T("α圧縮"), tmp, lst);
				*/
			}
			else if (USAME_TI(chunk, "ANIM")) {
				fmt.USET_T("アニメーション");
				if (fs->Read(buf.get(), 4)==4) {
					lst->Add(get_PropTitle(_T("背景色")).cat_sprintf(_T("R%u G%u B%u A%u"), buf[2], buf[1], buf[0], buf[3]));
					if (fs->Read(&us_buf, 2)==2) {
						tmp = get_PropTitle(_T("ループ回数"));
						if (us_buf==0)
							lst->Add(tmp.UCAT_T("無限"));
						else
							lst->Add(tmp.cat_sprintf(_T("%u"), us_buf));
					}
				}
			}
			else if (USAME_TI(chunk, "ANMF")) {
				fs->Seek(12, soFromCurrent);
				if (fs->Read(buf.get(), 3)==3) {
					t_cnt += (buf[0] | (buf[1] << 8) | (buf[2] << 16));
					i_cnt++;
				}
			}
			else if (USAME_TI(chunk, "EXIF")) {
				if (!meta.IsEmpty()) meta.UCAT_T(",");
				meta.UCAT_T("EXIF");
			}
			else if (USAME_TI(chunk, "XMP ")) {
				if (!meta.IsEmpty()) meta.UCAT_T(",");
				meta.UCAT_T("XMP");
	 		}

			fs->Seek(p, soFromBeginning);
			fs->Seek((int)(chunk_sz + (chunk_sz%2)), soFromCurrent);
		}

		//サイズ
		if (iw>0 && ih>0) {
			tmp = get_img_size_str(iw, ih);
			if (lst_top==lst->Count) lst->Add(tmp); else lst->Insert(lst_top, tmp);
			lst_top++;
			if (i_wd && i_hi) { *i_wd = iw; *i_hi = ih; }
		}
		//形式
		if (!fmt.IsEmpty()) {
			tmp = make_PropLine(_T("形式"), fmt);
			if (has_alph) tmp.UCAT_T(" + アルファ");
			if (lst_top==lst->Count) lst->Add(tmp); else lst->Insert(lst_top, tmp);
		}
		//フレーム数
		if (i_cnt>0) {
			add_PropLine(_T("フレーム数"), i_cnt, lst);
			lst->Add(get_PropTitle(_T("長さ")).cat_sprintf(_T("%.2f 秒"), t_cnt/1000.0));
		}
		//メタデータ
		add_PropLine_if(_T("メタデータ"), meta, lst);
		ret = true;
	}
	catch (EAbort &e) {
		add_list_errmsg(lst, e.Message);
	}
	catch (...) {
		add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}
	return ret;
}
//---------------------------------------------------------------------------
//WebP情報文字列(簡略)の取得
//---------------------------------------------------------------------------
UnicodeString get_WebpInfStr(
	UnicodeString fnam,		//ファイル名
	unsigned int *i_wd,		//[o] 幅
	unsigned int *i_hi)		//[o] 高さ
{
	UnicodeString ret_str;

	std::unique_ptr<TStringList> i_lst(new TStringList());
	unsigned int iw, ih;
	if (!get_WebpInf(fnam, i_lst.get(), &iw, &ih)) return EmptyStr;
	if (i_wd && i_hi) {
		*i_wd = iw;
		*i_hi = ih;
	}
	else
		ret_str = get_wd_x_hi_str(iw, ih).UCAT_T(" ");

	UnicodeString col_s, bit_s;
	for (int i=0; i<i_lst->Count; i++) {
		UnicodeString lbuf = i_lst->Strings[i];
		if (contains_s(lbuf, _T("形式: "))) {
			ret_str += Trim(get_tkn_r(lbuf, ':'));
			break;
		}
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//ファイルのCRC32文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_CRC32_str(UnicodeString fnam)
{
	static uint32_t table[256];
	static bool has_tbl = false;
 
	//テーブルの初期化
	if (!has_tbl) {
		for (int i=0; i<256; i++) {
			uint32_t rem = i;
			for (int j=0; j<8; j++) {
				if (rem & 1) {
					rem >>= 1;
					rem ^= 0xedb88320;
				}
				else
					rem >>= 1;
			}
			table[i] = rem;
		}
		has_tbl = true;
	}

	HANDLE hFile = ::CreateFile(fnam.c_str(), GENERIC_READ,
				FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE==hFile) return EmptyStr;

	UnicodeString ret_str;
	try {
		try {
			std::unique_ptr<BYTE[]> fbuf(new BYTE[FILE_RBUF_SIZE]);
			uint32_t crc32 = 0xFFFFFFFF;
			DWORD dwSize;
			for (;;) {
				if (!::ReadFile(hFile, fbuf.get(), FILE_RBUF_SIZE, &dwSize, NULL)) Abort();
				if (dwSize==0) break;
				char *bp = (char*)fbuf.get();
				for (DWORD i=0; i<dwSize; i++,bp++)
					crc32 = (crc32 >> 8) ^ table[(crc32 & 0xff) ^ (uint8_t)(*bp)];
			}
			crc32 ^= 0xFFFFFFFF;
			ret_str.sprintf(_T("%08x"), crc32);
		}
		catch (...) {
			ret_str = EmptyStr;
		}
	}
	__finally {
		::CloseHandle(hFile);
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//ファイルのハッシュ文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_HashStr(
	UnicodeString fnam,		//ファイル名
	ALG_ID algid)			//アルゴリズム CALG_MD5/ CALG_SHA1/ CALG_SHA_256/384/512
{
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;

	HANDLE hFile = ::CreateFile(fnam.c_str(), GENERIC_READ,
				FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (INVALID_HANDLE_VALUE==hFile) return EmptyStr;

	if (!::CryptAcquireContext(&hProv, NULL, NULL,
		(algid==CALG_SHA_256 || algid==CALG_SHA_384 || algid==CALG_SHA_512)? PROV_RSA_AES : PROV_RSA_FULL,
			CRYPT_VERIFYCONTEXT))
				return EmptyStr;
	if (!::CryptCreateHash(hProv, algid, 0, 0, &hHash)) {
		::CryptReleaseContext(hProv, 0);
		return EmptyStr;
	}

	UnicodeString ret_str;
	try {
		try {
			std::unique_ptr<BYTE[]> fbuf(new BYTE[FILE_RBUF_SIZE]);
			DWORD dwSize;
			for (;;) {
				if (!::ReadFile(hFile, fbuf.get(), FILE_RBUF_SIZE, &dwSize, NULL)) Abort();
				if (dwSize==0) break;
				if (!::CryptHashData(hHash, fbuf.get(), dwSize, 0)) Abort();
			}
			DWORD dwHashLen;
			if (!::CryptGetHashParam(hHash, HP_HASHVAL, NULL, &dwHashLen, 0)) Abort();
			std::unique_ptr<BYTE[]> pbHash(new BYTE[dwHashLen]);
			if (!::CryptGetHashParam(hHash, HP_HASHVAL, pbHash.get(), &dwHashLen, 0)) Abort();
			for (DWORD i=0; i<dwHashLen; i++) ret_str.cat_sprintf(_T("%02x"), pbHash[i]);
		}
		catch (...) {
			ret_str = EmptyStr;
		}
	}
	__finally {
		::CryptDestroyHash(hHash);
		::CryptReleaseContext(hProv, 0);
		::CloseHandle(hFile);
	}

	return ret_str;
}
//---------------------------------------------------------------------------
UnicodeString get_HashStr(UnicodeString fnam, UnicodeString idstr)
{
	switch (idx_of_word_i(_T("MD5|SHA1|SHA256|SHA384|SHA512|CRC32"), idstr)) {
	case  0: return get_HashStr(fnam, CALG_MD5);
	case  1: return get_HashStr(fnam, CALG_SHA1);
	case  2: return get_HashStr(fnam, CALG_SHA_256);
	case  3: return get_HashStr(fnam, CALG_SHA_384);
	case  4: return get_HashStr(fnam, CALG_SHA_512);
	case  5: return get_CRC32_str(fnam);
	default: return EmptyStr;
	}
}

//---------------------------------------------------------------------------
//アプリケーション情報を取得
//---------------------------------------------------------------------------
void get_AppInf(
	UnicodeString fnam,	//ファイル名
	TStringList *lst,	//[o] 情報リスト
	bool check)			//ヘッダをチェック、マシン情報取得 (default = true)
{
	//対応マシン
	if (check) {
		try {
			std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
			unsigned short us_buf;
			unsigned int   ui_buf;
			if (fs->Read(&us_buf, 2)!=2 || us_buf!=0x5a4d) Abort();
			fs->Seek((int)0x3c, soFromBeginning);
			if (fs->Read(&ui_buf, 4)!=4) Abort();
			fs->Seek((int)ui_buf, soFromBeginning);
			if (fs->Read(&ui_buf, 4)!=4 || ui_buf!=0x4550) Abort();
			//Machine
			if (fs->Read(&us_buf, 2)!=2) Abort();
			UnicodeString vstr =
				(us_buf==IMAGE_FILE_MACHINE_I386)?  "x86" :
				(us_buf==IMAGE_FILE_MACHINE_AMD64)? "x64" :
				(us_buf==IMAGE_FILE_MACHINE_IA64)?  "Intel Itanium":
				(us_buf==0x01c0)? "arm" :
				(us_buf==0x01c2)? "thumb" :
				(us_buf==0x01c4)? "armnt" :
				(us_buf==0xaa64)? "arm64" : "???";
			//Characteristics
			fs->Seek(16, soFromCurrent);
			if ((fs->Read(&us_buf, 2)==2) && USAME_TS(vstr, "x86") && (us_buf & 0x20))
				vstr.UCAT_T(" (Large Address Aware)");
			add_PropLine(_T("マシン"), vstr, lst);
		}
		catch (...) {
			return;
		}
	}

	//アプリケーション情報
	WORD cpag_id[] = {0, 932, 949, 950, 1200, 1250, 1251, 1252, 1253, 1254, 1255, 1256};
	WORD lang_id[] = {
			0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407, 0x0408,
			0x0409, 0x040A, 0x040B, 0x040C, 0x040D, 0x040E, 0x040F, 0x0410,
			0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, 0x0418,
			0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F, 0x0420, 0x0421,
			0x0804, 0x0807, 0x0809, 0x080A, 0x080C, 0x0810, 0x0813, 0x0814, 0x0816, 0x081A, 0x0C0C, 0x100C
		};

	DWORD dwReserved;
	DWORD dwSize	= ::GetFileVersionInfoSize(fnam.c_str(), &dwReserved);
	if (dwSize==0) return;
	LPVOID lpBuffer = ::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize);
	if (!lpBuffer) return;

	if (::GetFileVersionInfo(fnam.c_str(), NULL, dwSize, lpBuffer)) {
		LPVOID lpLang = NULL;
		UINT dwLength;
		if (::VerQueryValue(lpBuffer, _T("\\VarFileInfo\\Translation"), &lpLang, &dwLength)) {
			WORD *lpTranslate = (WORD*)lpLang;
			UnicodeString substr;
			if (*lpTranslate!=0) substr.sprintf(_T("\\StringFileInfo\\%04x%04x\\"), *lpTranslate, *(lpTranslate + 1));

			std::unique_ptr<TStringList> s_lst(new TStringList());
			s_lst->Text =
				"FileDescription=説明\n"
				"FileVersion=ファイルVer.\n"
				"ProductName=製品名\n"
				"ProductVersion=製品Ver.\n"
				"LegalCopyright=著作権\n"
				"CompanyName=会社名\n"
				"LegalTrademarks=商標\n"
				"Comments=コメント\n"
				"OriginalFilename=元のファイル名\n"
				"InternalName=内部名\n";

			int l_cnt = 0;
			for (int j=0; j<s_lst->Count; j++) {
				UnicodeString nstr = s_lst->Names[j];
				UnicodeString subblock, vstr;
				LPVOID lpStr;
				if (!substr.IsEmpty()) {
					subblock = substr + nstr;
					if (::VerQueryValue(lpBuffer, subblock.c_str(), &lpStr, &dwLength))
						vstr.cat_sprintf(_T("%s"), lpStr);
				}
				else {
					//言語情報がない
					int c_cnt = sizeof(cpag_id)/sizeof(WORD);
					int i_cnt = sizeof(lang_id)/sizeof(WORD);
					for (int c_i=0; c_i<c_cnt && vstr.IsEmpty(); c_i++) {
						for (int l_i=0; l_i<i_cnt && vstr.IsEmpty(); l_i++) {
							substr.sprintf(_T("\\StringFileInfo\\%04x%04x\\"), lang_id[l_i], cpag_id[c_i]);
							subblock = substr + nstr;
							if (::VerQueryValue(lpBuffer, subblock.c_str(), &lpStr, &dwLength))
								vstr.cat_sprintf(_T("%s"), lpStr);
						}
					}
				}
				vstr = Trim(vstr);
				if (!vstr.IsEmpty()) {
					add_PropLine(s_lst->ValueFromIndex[j], vstr, lst);
					l_cnt++;
				}
			}

			//取得できなければプロパティから
			if (l_cnt==0) usr_SH->get_PropInf(fnam, lst);
		}
	}
	::HeapFree(::GetProcessHeap(), 0, lpBuffer);
}

//---------------------------------------------------------------------------
//Borland プロジェクト情報を取得
//---------------------------------------------------------------------------
void get_BorlandInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst)		//[o] 情報リスト
{
	if (!file_exists(fnam)) return;

	UnicodeString fext = get_extension(fnam);

	//プロジェクト
	if (test_FileExt(fext, _T(".cbproj.dproj"))) {
		std::unique_ptr<TStringList> fbuf(new TStringList());
		fbuf->LoadFromFile(fnam);
		bool flag = false;
		lst->Add(EmptyStr);
		for (int i=0; i<fbuf->Count; i++) {
			UnicodeString lbuf = Trim(fbuf->Strings[i]);
			if (!flag) {
				if (contains_i(lbuf, _T("<PropertyGroup>"))) flag = true;
			}
			else {
				if (contains_i(lbuf, _T("</PropertyGroup>"))) break;

				UnicodeString vstr = get_tkn_m(lbuf, _T(">"), _T("</"));
				if (contains_i(lbuf, _T("<FrameworkType>")))
					add_PropLine(_T("FrameworkType"),	vstr, lst);
				else if (contains_i(lbuf, _T("<AppType>")))
					add_PropLine(_T("AppType"),			vstr, lst);
				else if (contains_i(lbuf, _T("<MainSource>")))
					add_PropLine(_T("MainSource"),		vstr, lst);
				else if (contains_i(lbuf, _T("<Platform Condition=")))
					add_PropLine(_T("Platform"),		vstr, lst);
				else if (contains_i(lbuf, _T("<Config Condition=")) || contains_i(lbuf, _T("Configuration Condition=")))
					add_PropLine(_T("Config"),			vstr, lst);
				else if (contains_i(lbuf, _T("<ProjectType>")))
					add_PropLine(_T("ProjectType"),		vstr, lst);
			}
		}
		lst->Add(EmptyStr);
	}
	//ソース
	else if (test_FileExt(fext, _T(".cpp.pas.dfm.fmx.h"))) {
		UnicodeString fm_nam = ChangeFileExt(fnam, ".dfm");
		if (!file_exists(fm_nam)) fm_nam = ChangeFileExt(fnam, ".fmx");
		if (file_exists(fm_nam)) {
			std::unique_ptr<TStringList> fbuf(new TStringList());
			fbuf->LoadFromFile(fm_nam);

			std::unique_ptr<TStringList> ibuf(new TStringList());
			for (int i=0; i<fbuf->Count; i++) {
				UnicodeString lbuf = Trim(fbuf->Strings[i]);
				if (USTARTS_TI("object", lbuf)) {
					if (i>0) break;
					add_PropLine(_T("Form Name"), get_tkn_m(lbuf, ' ', ':'), ibuf.get());
				}
				else {
					UnicodeString vnam = split_tkn(lbuf, _T(" = "));
					UnicodeString vstr = exclude_quot(lbuf);
					if (USAME_TI(vnam, "Caption")) {
						UnicodeString cap_str;
						while (!vstr.IsEmpty()) {
							if (remove_top_s(vstr, '\''))
								cap_str += split_tkn(vstr, '\'');
							else if (vstr[1]=='#') {
								UnicodeString tmp = split_tkn(vstr, '\'');
								if (!vstr.IsEmpty()) vstr.Insert("'", 1);
								tmp.Delete(1, 1);
								TStringDynArray c_lst = SplitString(tmp, "#");
								UnicodeString tstr;
								for (int j=0; j<c_lst.Length; j++) tstr += _TCHAR(c_lst[j].ToIntDef(0));
								cap_str += tstr;
							}
							else {
								cap_str += vstr;  break;
							}
						}
						vstr = cap_str;
					}
					else if (!contained_wd_i(_T("BorderStyle|ClientWidth|ClientHeight|Position"), vnam)) continue;

					add_PropLine(vnam, vstr, ibuf.get());
				}
			}

			if (ibuf->Count>0) {
				lst->Add(EmptyStr);
				lst->AddStrings(ibuf.get());
				lst->Add(EmptyStr);
			}
		}
	}
}

//---------------------------------------------------------------------------
//tags ファイル情報を取得
//---------------------------------------------------------------------------
void get_TagsInf(
	UnicodeString fnam,		//ファイル名
	TStringList *lst)		//[o] 情報リスト
{
	if (!file_exists(fnam)) return;

	std::unique_ptr<TStringList> fbuf(new TStringList());
	fbuf->LoadFromFile(fnam);
	bool flag = false;
	for (int i=0; i<fbuf->Count; i++) {
		UnicodeString lbuf = fbuf->Strings[i];
		if (remove_top_s(lbuf, "!_TAG_")) {
			if (!flag) lst->Add(EmptyStr);
			flag = true;
			if (remove_top_s(lbuf, "PROGRAM_") || remove_top_s(lbuf, "FILE_")) {
				UnicodeString vnam = split_pre_tab(lbuf);
				if (!USAME_TI(vnam, "ENCODING")) {
					UnicodeString vstr = REPLACE_TS(REPLACE_TS(lbuf, "\t//", ""), "\t"," ");
					add_PropLine(vnam, vstr, lst);
				}
			}
		}
		else {
			if (flag) lst->Add(EmptyStr);
			break;
		}
	}
}

//---------------------------------------------------------------------------
//DLLのエクスポート関数一覧を取得
//---------------------------------------------------------------------------
bool get_DllExpFunc(UnicodeString fnam, TStringList *lst)
{
	bool loaded = false;
	HMODULE hModule = ::GetModuleHandle(ExtractFileName(fnam).c_str());
	if (!hModule) {
		hModule = ::LoadLibrary(fnam.c_str());
		loaded = true;
	}
	if (!hModule) return false;

	ULONG nSize;
	IMAGE_EXPORT_DIRECTORY *pImgExpDir
		= (IMAGE_EXPORT_DIRECTORY*)::ImageDirectoryEntryToData(
			(PVOID)hModule, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &nSize);

	if (pImgExpDir) {
		UnicodeString tmp = fnam;
		if (!loaded) tmp.UCAT_T("  (Linked)");
		lst->Add(tmp);
		lst->Add("Ordinal        Finction");
		UnicodeString hr_str = make_RuledLine(2, 14, 25);
		lst->Add(hr_str);
		char **ppszFunctionName  = (char**)((ULONG)pImgExpDir->AddressOfNames + (ULONG)hModule);
		WORD  *pwFunctionOrdinal = (WORD*)((ULONG)pImgExpDir->AddressOfNameOrdinals + (ULONG)hModule);
		int f_cnt = 0;
		for (unsigned int i = 0; i<pImgExpDir->NumberOfFunctions; i++) {
			UnicodeString fnc_nam;
			for (unsigned int j=0; j<pImgExpDir->NumberOfNames; j++) {
				if (pwFunctionOrdinal[j]==(WORD)i) {
					fnc_nam = UnicodeString((char*)((ULONG)ppszFunctionName[j] + (ULONG)hModule));
					break;
				}
			}
			if (!fnc_nam.IsEmpty()) {
				unsigned int ordn = pImgExpDir->Base + i;
				lst->Add(tmp.sprintf(_T("%4d (0x%04x)  %s"), ordn, ordn, fnc_nam.c_str()));
				f_cnt++;
			}
		}
		lst->Add(hr_str);
		lst->Add(tmp.sprintf(_T("        Total  %4d functions"), f_cnt));
	}

	if (loaded) ::FreeLibrary(hModule);
	return true;
}

//---------------------------------------------------------------------------
//リストにエラーメッセージ("エラー: メッセージ") を追加
//---------------------------------------------------------------------------
void add_list_errmsg(TStringList *lst, UnicodeString msg)
{
	add_PropLine_if(_T("エラー"), msg, lst);
}

//---------------------------------------------------------------------------
//輝度を取得(0～1.0)
//---------------------------------------------------------------------------
double GetLuminance(TColor col)
{
	int c = ColorToRGB(col);
	return ((GetRValue(c)*0.3 + GetGValue(c)*0.59 + GetBValue(c)*0.11) / 255.0);
}

//---------------------------------------------------------------------------
//カラーに倍率をかける
//---------------------------------------------------------------------------
TColor RatioCol(TColor col, float rt)
{
	if (rt>1.0) rt = 1.0; else if (rt<0.0) rt = 0.0;
	int c = ColorToRGB(col);
	int r = GetRValue(c) * rt;
	int g = GetGValue(c) * rt;
	int b = GetBValue(c) * rt;
	return TColor(RGB(r, g, b));
}
//---------------------------------------------------------------------------
//輝度に基づいてグレースケール化
//---------------------------------------------------------------------------
TColor GrayCol(TColor col)
{
	if (col==clNone) col = clBlack;
	BYTE v = (BYTE)(GetLuminance(col) * 255);
	return TColor(RGB(v, v, v));
}

//---------------------------------------------------------------------------
//RGB→HSL 変換
//---------------------------------------------------------------------------
void RgbToHsl(TColor col, int *h, int *s, int *l)
{
	int cref = ColorToRGB(col);
	float r = GetRValue(cref)/255.0;
	float g = GetGValue(cref)/255.0;
	float b = GetBValue(cref)/255.0;

	float maxv = std::max(r, std::max(g, b));
	float minv = std::min(r, std::min(g, b));
	float h_f, s_f;
	float l_f = (maxv + minv)/2.0;

	if (maxv==minv) {
		h_f = s_f = 0.0;
	}
	else {
		float d = maxv - minv;
		if		(maxv==r) h_f = (g - b)/d;
		else if (maxv==g) h_f = (b - r)/d + 2.0;
		else			  h_f = (r - g)/d + 4.0;
		h_f *= 60.0;
		if (h_f<0.0) h_f += 360.0;

		s_f = ((l_f<0.5)? d/(maxv + minv) : d/(2.0 - maxv - minv)) * 100.0;
	}

	l_f *= 100.0;

	*h = (int)(h_f + 0.5);
	*s = (int)(s_f + 0.5);
	*l = (int)(l_f + 0.5);
}
//---------------------------------------------------------------------------
//RGB→HSV 変換
//---------------------------------------------------------------------------
void RgbToHsv(TColor col, int *h, int *s, int *v)
{
	int cref = ColorToRGB(col);
	float r = GetRValue(cref)/255.0;
	float g = GetGValue(cref)/255.0;
	float b = GetBValue(cref)/255.0;

	float maxv = std::max(r, std::max(g, b));
	float minv = std::min(r, std::min(g, b));
	float h_f, s_f;
	float v_f = maxv * 100;

	if (maxv==minv) {
		h_f = s_f = 0.0;
	}
	else {
		float d = maxv - minv;
		if		(maxv==r) h_f = (g - b)/d;
		else if (maxv==g) h_f = (b - r)/d + 2.0;
		else			  h_f = (r - g)/d + 4.0;
		h_f *= 60.0;
		if (h_f<0.0) h_f += 360.0;

		s_f = d/maxv * 100.0;
	}

	*h = (int)(h_f + 0.5);
	*s = (int)(s_f + 0.5);
	*v = (int)(v_f + 0.5);
}

//---------------------------------------------------------------------------
//背景の輝度から白または黒を選択・加減
//---------------------------------------------------------------------------
TColor SelectWorB(
	TColor col,		//背景色
	float  rt)		//加減指定  (default = 1.0)
{
	double v = GetLuminance(col);
	if (rt<1.0)
		return (v>0.5)? RatioCol(clWhite, 1.0 - rt) : RatioCol(clWhite, rt);
	else
		return (v>0.5)? clBlack : clWhite;
}
//---------------------------------------------------------------------------
//輝度に基づいて明暗を加減
//---------------------------------------------------------------------------
TColor AdjustColor(
	TColor col,		//元の色
	int adj)		//加減値	0～255
{
	if (GetLuminance(col)>0.5) adj = -adj;

	int cref = ColorToRGB(col);
	int r = GetRValue(cref);
	int g = GetGValue(cref);
	int b = GetBValue(cref);

	r += adj;  if (r>255) r = 255; else if (r<0) r = 0;
	g += adj;  if (g>255) g = 255; else if (g<0) g = 0;
	b += adj;  if (b>255) b = 255; else if (b<0) b = 0;

	return TColor(RGB(r, g, b));
}

//---------------------------------------------------------------------------
//文字列からカラーを設定
//---------------------------------------------------------------------------
void str_to_Color(TColor &col, UnicodeString s)
{
	TColor c = (TColor)s.ToIntDef((int)clNone);
	if (c!=clNone) col = c;
}

//---------------------------------------------------------------------------
//ハイライト色の設定
//---------------------------------------------------------------------------
void SetHighlight(TCanvas *cv, bool hl)
{
	cv->Brush->Color = hl? scl_Highlight : scl_Window;
	cv->Font->Color  = hl? scl_HighlightText : scl_WindowText;
}

//---------------------------------------------------------------------------
//グリッドのセル幅をヘッダに合わせる
//---------------------------------------------------------------------------
void set_GridFromHeader(THeaderControl *hp, TStringGrid *gp)
{
	int cnt = std::min(hp->Sections->Count, gp->ColCount);
	int wd = 0;
	for (int i=0; i<cnt; i++) {
		gp->ColWidths[i] = hp->Sections->Items[i]->Width - gp->GridLineWidth;
		wd += hp->Sections->Items[i]->Width;
	}

	//余りのセクションは残り幅に合わせる
	if (hp->Sections->Count>gp->ColCount)
		hp->Sections->Items[gp->ColCount]->Width = std::max(hp->Width - wd, 0);
}
//---------------------------------------------------------------------------
//ヘッダの列幅をグリッドのセル幅に合わせる
//---------------------------------------------------------------------------
void set_HeaderFromGrid(TStringGrid *gp, THeaderControl *hp)
{
	int cnt = std::min(hp->Sections->Count, gp->ColCount);
	int wd = 0;
	for (int i=0; i<cnt; i++) {
		hp->Sections->Items[i]->Width = gp->ColWidths[i] + gp->GridLineWidth + ((i==0)? 1 : 0);
		wd += hp->Sections->Items[i]->Width;
	}

	//余りのセクションは残り幅に合わせる
	if (hp->Sections->Count>gp->ColCount)
		hp->Sections->Items[gp->ColCount]->Width = std::max(hp->Width - wd, 0);
}

//---------------------------------------------------------------------------
//ヘッダの列幅を設定(固定)
//---------------------------------------------------------------------------
void set_HeaderSecWidth(THeaderControl *hp, int cnt, ...)
{
	THeaderSections *sp = hp->Sections;

	//一旦、固定を解除
	for (int i=0; i<sp->Count; i++) {
		sp->Items[i]->MinWidth = 0;
		sp->Items[i]->MaxWidth = 10000;
	}

	va_list ap;
	va_start(ap, cnt);
	for (int i=0; i<cnt; i++) {
		int w = va_arg(ap, int);
		if (i<sp->Count) sp->Items[i]->Width = w;
	}
	va_end(ap);

	//末尾列が未指定なら残り幅に設定
	if (cnt == sp->Count-1) {
		int w = hp->ClientWidth;
		for (int i=0; i<sp->Count-1; i++) w -= sp->Items[i]->Width;
		sp->Items[sp->Count - 1]->Width = std::max(w, 0);
	}

	//列幅を固定
	for (int i=0; i<sp->Count; i++) {
		int w = sp->Items[i]->Width;
		sp->Items[i]->MinWidth = w;
		sp->Items[i]->MaxWidth = w;
	}
}
//---------------------------------------------------------------------------
void adjust_HeaderSecWidth(THeaderControl *hp, int idx)
{
	THeaderSections *sp = hp->Sections;
	int wd = hp->Width;
	for (int i=0; i<sp->Count; i++) if (i!=idx) wd -= sp->Items[i]->Width;
	sp->Items[idx]->Width = wd;
}

//---------------------------------------------------------------------------
//ステータスバーのパネル幅を文字数から設定
//---------------------------------------------------------------------------
int set_SttBarPanelWidth(TStatusBar *sp, int idx,
	int n)	//文字数(半角単位)
{
	TCanvas *cv = sp->Canvas;
	cv->Font->Assign(sp->Font);
	int wd = (n>0)? get_CharWidth(cv, n, 16) : 0;
	sp->Panels->Items[idx]->Width = wd;
	return wd;
}

//---------------------------------------------------------------------------
//グリッドの行をクリア
//---------------------------------------------------------------------------
void clear_GridRow(TStringGrid *gp, int row, int col)
{
	if (row>=0 && row<gp->RowCount) {
		for (int i=col; i<gp->ColCount; i++) gp->Cells[i][row] = EmptyStr;
	}
}

//---------------------------------------------------------------------------
//グリッドの現在位置をインデックス化
//---------------------------------------------------------------------------
int get_GridIndex(TStringGrid *gp, int max_count)
{
	int idx = (gp->ColCount * gp->Row) + gp->Col;
	if (idx>=max_count) idx = -1;
	return idx;
}
//---------------------------------------------------------------------------
//インデックスによってグリッド位置を設定
//---------------------------------------------------------------------------
void set_GridIndex(TStringGrid *gp, int idx, int max_count)
{
	if (gp->Visible) gp->SetFocus();

	if (idx<0) idx = 0;
	if (idx>=max_count) idx = (max_count>0)? max_count - 1 : 0;
	int c0 = gp->Col;
	int c1 = idx%gp->ColCount;
	int r1 = idx/gp->ColCount;

	try {
		if ((r1*gp->ColCount + c0) >= max_count) {
			gp->Col = c1;
			gp->Row = r1;
		}
		else {
			gp->Row = r1;
			gp->Col = c1;
		}
	}
	catch (...) {
		gp->Col = 0; gp->Row = 0;
	}

	if (gp->Visible) gp->Invalidate();
}

//---------------------------------------------------------------------------
//ListBox のカーソル移動
//---------------------------------------------------------------------------
void ListBoxCursorDown(TListBox *lp)
{
	int idx = lp->ItemIndex + 1;
	if (idx<lp->Count) ListBoxSetIndex(lp, idx);
}
//---------------------------------------------------------------------------
void ListBoxCursorDown(TListBox *lp, int n)
{
	ListBoxSetIndex(lp, std::min(lp->ItemIndex + n, lp->Count - 1));
}
//---------------------------------------------------------------------------
void ListBoxCursorUp(TListBox *lp)
{
	int idx = lp->ItemIndex - 1;
	if (idx>=0) ListBoxSetIndex(lp, idx);
}
//---------------------------------------------------------------------------
void ListBoxCursorUp(TListBox *lp, int n)
{
	ListBoxSetIndex(lp, std::max(lp->ItemIndex - n, 0));
}
//---------------------------------------------------------------------------
void ListBoxPageDown(TListBox *lp)
{
	ListBoxCursorDown(lp, lp->ClientHeight/lp->ItemHeight - 1);
}
//---------------------------------------------------------------------------
void ListBoxPageUp(TListBox *lp)
{
	ListBoxCursorUp(lp, lp->ClientHeight/lp->ItemHeight - 1);
}
//---------------------------------------------------------------------------
void ListBoxTop(TListBox *lp)
{
	ListBoxSetIndex(lp, 0);
}
//---------------------------------------------------------------------------
void ListBoxEnd(TListBox *lp)
{
	if (lp->Count>0) ListBoxSetIndex(lp, lp->Count - 1);
}

//---------------------------------------------------------------------------
void ListBoxSetIndex(TListBox *lp, int idx)
{
	if (lp->Count==0) return;

	if (idx<0)
		lp->ItemIndex = 0;
	else if (idx==lp->ItemIndex) {
		//可視領域外だったらスクロール
		int pn = lp->ClientHeight/lp->ItemHeight;
		if (lp->ItemIndex<lp->TopIndex)
			lp->TopIndex = std::max(lp->ItemIndex - LISTBOX_SCRMGN, 0);
		else if (lp->ItemIndex > lp->TopIndex + pn - 1)
			lp->TopIndex = std::max(lp->ItemIndex - pn + 1 + LISTBOX_SCRMGN, 0);
	}
	else {
		if (idx>=lp->Count) idx = lp->Count - 1;
		//上へ
		if (idx<lp->ItemIndex) {
			if (idx < lp->TopIndex+LISTBOX_SCRMGN)
				lp->TopIndex = std::max(idx - LISTBOX_SCRMGN, 0);
		}
		//下へ
		else {
			int pn = lp->ClientHeight/lp->ItemHeight;
			if (idx > (lp->TopIndex+pn - 1 - LISTBOX_SCRMGN))
				lp->TopIndex = std::max(idx - pn + 1 + LISTBOX_SCRMGN, 0);
		}
		lp->ItemIndex = idx;
	}
}
//---------------------------------------------------------------------------
//ListBox のカーソルを可視領域へ
//---------------------------------------------------------------------------
bool ListBoxCsrToVisible(TListBox *lp)
{
	int lst_idx = lp->ItemIndex;
	if (lp->ItemIndex < lp->TopIndex)
		lp->ItemIndex = lp->TopIndex;
	else {
		int idx_btm = lp->TopIndex + lp->ClientHeight/lp->ItemHeight - 1;
		if (lp->ItemIndex > idx_btm) lp->ItemIndex = idx_btm;
	}
	return (lst_idx != lp->ItemIndex);
}
//---------------------------------------------------------------------------
//ListBox のスクロール
//---------------------------------------------------------------------------
void ListBoxScrollDown(TListBox *lp, int n,
	bool move_csr)	//カーソルも移動 (default = false)
{
	int idx = lp->TopIndex;
	lp->TopIndex = idx + n;
	n = lp->TopIndex - idx;
	if (move_csr) lp->ItemIndex += n;
}
//---------------------------------------------------------------------------
void ListBoxScrollDown(TListBox *lp, UnicodeString prm)
{
	if (!prm.IsEmpty()) {
		int pn = lp->ClientHeight/lp->ItemHeight;

		switch (idx_of_word_i(_T("HP|FP|ED"), prm)) {
		case 0:  ListBoxScrollDown(lp, pn/2);					break;
		case 1:  ListBoxScrollDown(lp, pn);						break;
		case 2:  lp->TopIndex = std::max(lp->Count - pn, 0);	break;
		default: ListBoxScrollDown(lp, prm.ToIntDef(1));
		}
	}
}

//---------------------------------------------------------------------------
void ListBoxScrollUp(TListBox *lp, int n,
	bool move_csr)	//カーソルも移動 (default = false)
{
	int idx = lp->TopIndex;
	if (n>lp->TopIndex) lp->TopIndex = 0; else lp->TopIndex = idx - n;
	n = lp->TopIndex - idx;
	if (move_csr) lp->ItemIndex += n;
}
//---------------------------------------------------------------------------
void ListBoxScrollUp(TListBox *lp, UnicodeString prm)
{
	if (!prm.IsEmpty()) {
		int pn = lp->ClientHeight/lp->ItemHeight;

		switch (idx_of_word_i(_T("HP|FP|TP"), prm)) {
		case 0:  ListBoxScrollUp(lp, pn/2);	break;
		case 1:  ListBoxScrollUp(lp, pn);	break;
		case 2:  lp->TopIndex = 0;			break;
		default: ListBoxScrollUp(lp, prm.ToIntDef(1));
		}
	}
}

//---------------------------------------------------------------------------
//ListBox ですべての項目を選択
//---------------------------------------------------------------------------
void ListBoxSelectAll(TListBox *lp, bool sw)
{
	if (lp) lp->Perform(LB_SETSEL, (BOOL)sw, -1);
}

//---------------------------------------------------------------------------
//ListBox で頭文字サーチ
//---------------------------------------------------------------------------
int ListBoxInitialSearch(
	TListBox *lp,
	UnicodeString kptn,		//検索パターン
	bool fnam_sw)			//ファイル名部分に対して適用 (default = false)
{
	int idx0 = -1, idx1 = -1;
	try {
		TRegExOptions opt; opt << roIgnoreCase;
		for (int i=0; i<lp->Count && idx1==-1; i++) {
			if (i<=lp->ItemIndex && idx0!=-1) continue;
			UnicodeString lbuf = lp->Items->Strings[i];
			if (fnam_sw) lbuf = ExtractFileName(lbuf);
			if (TRegEx::IsMatch(lbuf, kptn, opt)) ((i<=lp->ItemIndex)? idx0 : idx1) = i;
		}
	}
	catch (...) {
		;
	}
	int idx = (idx1!=-1)? idx1 : idx0;
	if (idx!=-1) lp->ItemIndex = idx;
	return idx;
}

//---------------------------------------------------------------------------
//ListBox の現在行からURLを取得
//---------------------------------------------------------------------------
UnicodeString ListBoxGetURL(TListBox *lp)
{
	return (lp->ItemIndex!=-1)? extract_URL(lp->Items->Strings[lp->ItemIndex]) : EmptyStr;
}


//---------------------------------------------------------------------------
//ListBox の Objects を整数値としてチェックし、項目連番を設定
//---------------------------------------------------------------------------
void set_ListBox_ItemNo(TListBox *lp)
{
	int itm_no	 = 0;
	int last_tag = -1;
	for (int i=0; i<lp->Count; i++) {
		int tag = (int)lp->Items->Objects[i];
		if (last_tag!=tag) {
			itm_no++;
			last_tag = tag;
		}
		lp->Items->Objects[i] = (TObject*)itm_no;
	}

	lp->Invalidate();
}

//---------------------------------------------------------------------------
//TStringGrid でのカーソル移動
//---------------------------------------------------------------------------
void GridCursorLeft(TStringGrid *gp)
{
	if (gp->Col>0) gp->Col = gp->Col - 1;
}
//---------------------------------------------------------------------------
void GridCursorRight(TStringGrid *gp)
{
	if (gp->Col < gp->ColCount - 1) gp->Col = gp->Col + 1;
}
//---------------------------------------------------------------------------
void GridCursorDown(TStringGrid *gp)
{
	if (gp->Row < gp->RowCount - 1) gp->Row = gp->Row + 1;
}
//---------------------------------------------------------------------------
void GridCursorUp(TStringGrid *gp)
{
	if (gp->Row>gp->FixedRows) gp->Row = gp->Row - 1;
}
//---------------------------------------------------------------------------
void GridPageDown(TStringGrid *gp)
{
	int pn  = gp->VisibleRowCount - 1;
	int row = std::min(gp->Row + pn, gp->RowCount - 1);
	gp->Row = row;
}
//---------------------------------------------------------------------------
void GridPageUp(TStringGrid *gp)
{
	int pn  = gp->VisibleRowCount - 1;
	int row = std::max(gp->Row - pn, gp->FixedRows);
	gp->Row = row;
}
//---------------------------------------------------------------------------
bool GridCursorMove(UnicodeString cmd , TStringGrid *gp)
{
	switch (idx_of_word_i(_T("CursorDown|CursorUp|PageDown|PageUp"), cmd)) {
	case  0: GridCursorDown(gp);	break;
	case  1: GridCursorUp(gp);		break;
	case  2: GridPageDown(gp);		break;
	case  3: GridPageUp(gp);		break;
	default:						return false;
	}

	return true;
}

//---------------------------------------------------------------------------
//TStringList の項目を１つ上へ移動
//---------------------------------------------------------------------------
int up_StrListItem(TStringList *lst, int idx)
{
	if (idx>=1 && idx<lst->Count) {
		lst->Move(idx, idx - 1);
		idx--;
	}
	return idx;
}
//---------------------------------------------------------------------------
//TStringList の項目を１つ下へ移動
//---------------------------------------------------------------------------
int down_StrListItem(TStringList *lst, int idx)
{
	if (idx>=0 && idx<lst->Count - 1) {
		lst->Move(idx, idx + 1);
		idx++;
	}
	return idx;
}

//---------------------------------------------------------------------------
//コントロールでキーを押す
//---------------------------------------------------------------------------
void perform_Key(TControl *cp, int key)
{
	cp->Perform(WM_KEYDOWN,	key, 0);
	cp->Perform(WM_KEYUP,	key, 0);
}
//---------------------------------------------------------------------------
void perform_Key_RETURN(TControl *cp)
{
	perform_Key(cp, VK_RETURN);
}

//---------------------------------------------------------------------------
//アクセラレータキーの一致するリスト項目のインデックスを取得
//---------------------------------------------------------------------------
int get_IndexFromAccKey(TStringList *lst, UnicodeString key)
{
	if (key.Length()!=1) return -1;
	key.Insert("&", 1);
	int idx = -1;
	for (int i=0; i<lst->Count; i++)
		if (ContainsText(get_csv_item(lst->Strings[i], 0), key)) { idx = i; break; }
	return idx;
}

//---------------------------------------------------------------------------
//キー・マウスバッファをクリア
//---------------------------------------------------------------------------
void ClearKeyBuff(
	bool key_only)	//true = キーバッファのみクリア (default = false)
{
	MSG msg;
	if (key_only) {
		do {
			Sleep(0);
		} while (::PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE));
	}
	else {
		do {
			Sleep(50);
		} while (
			::PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE) ||
			::PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE)
		);
	}
}

//---------------------------------------------------------------------------
//文字列からURL(ローカルファイルを含む)を抽出
//---------------------------------------------------------------------------
UnicodeString extract_URL(UnicodeString s)
{
	if (s.Pos(':')==0) return EmptyStr;

	UnicodeString url;
	TMatch mt = TRegEx::Match(s, URL_MATCH_PTN);
	if (mt.Success) {
		url = mt.Value;
		if (!USTARTS_TS('h', url)) url.Insert("h", 1);	//先頭に h がなかったら補う
	}
	else {
		mt = TRegEx::Match(s, MAIL_MATCH_PTN);
		if (mt.Success)
			url = mt.Value;
		else {
			mt = TRegEx::Match(s, LOCAL_FILE_PTN);
			if (mt.Success) url = mt.Value;
		}
	}
	return url;
}

//---------------------------------------------------------------------------
//URLをエンコード
//---------------------------------------------------------------------------
UnicodeString get_EncodedUrl(UnicodeString url)
{
	try {
		return TIdURI::URLEncode(url, IndyTextEncoding_UTF8());
	}
	catch (Exception &exception) {
		return EmptyStr;
	}
}

//---------------------------------------------------------------------------
//インターネット接続があるか?
//---------------------------------------------------------------------------
bool InternetConnected()
{
	DWORD flag;
	return InternetGetConnectedState(&flag, 0);
}

//---------------------------------------------------------------------------
//オンライン上のファイルを取得
//---------------------------------------------------------------------------
int get_OnlineFile(UnicodeString url, UnicodeString fnam, bool *cancel, TProgressBar *prg_bar)
{
	int f_size = -1;
	if (cancel) *cancel = false;

	if (!url.IsEmpty() && InternetConnected()) {
		HINTERNET hSession = InternetOpen(
			_T("NyanFi"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
		if (hSession) {
			HINTERNET hConnect = InternetOpenUrl(hSession,
			    url.c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
			if (hConnect) {
				DWORD dwFsize = 0;
				DWORD dwBufLen = sizeof(dwFsize);
				HttpQueryInfo(hConnect, HTTP_QUERY_CONTENT_LENGTH|HTTP_QUERY_FLAG_NUMBER,
						 (LPVOID)&dwFsize, &dwBufLen, 0);

				std::unique_ptr<BYTE[]> ldbuf(new BYTE[FILE_RBUF_SIZE]);
				DWORD dwSize;
				f_size = 0;
				chk_cre_dir(ExtractFileDir(fnam));
				{
					std::unique_ptr<TFileStream> dfs(new TFileStream(fnam, fmCreate));
					for (;;) {
						if (!InternetReadFile(hConnect, ldbuf.get(), FILE_RBUF_SIZE, &dwSize)) break;
						if (dwSize==0) break;
						dfs->Write(ldbuf.get(), dwSize);
						f_size += dwSize;
						if (prg_bar && dwFsize>0) prg_bar->Position = floor(1.0 * prg_bar->Max * f_size / dwFsize);
						if (cancel) {
							Application->ProcessMessages();
							if (*cancel) break;
						}
					}
				}
				if (cancel && *cancel && file_exists(fnam)) DeleteFile(fnam);
				if (prg_bar) {
					prg_bar->Position = prg_bar->Max;
					prg_bar->Repaint(); Sleep(500);
				}
				InternetCloseHandle(hConnect);
			}
		    InternetCloseHandle(hSession);
		}
	}
	return f_size;
}

//---------------------------------------------------------------------------
//スプリッタ幅を設定
//---------------------------------------------------------------------------
void set_SplitterWidht(TSplitter *sp, int sp_wd)
{
	if (sp->Align==alLeft || sp->Align==alRight) sp->Width = sp_wd; else sp->Height = sp_wd;
	sp->Repaint();
}

//---------------------------------------------------------------------------
//パネルの配置
//---------------------------------------------------------------------------
void set_PanelAlign(TPanel *pp, TSplitter *sp, int mode, int sp_wd)
{
	switch (mode) {
	case 1:	//右
		sp->Align = alLeft;
		pp->Align = alRight;
		sp->Align = alRight;
		sp->Width = pp->Visible? sp_wd : 0;
		break;
	case 2:	//左
		sp->Align = alRight;
		pp->Align = alLeft;
		sp->Align = alLeft;
		sp->Width = pp->Visible? sp_wd : 0;
		break;
	case 3: //上
		sp->Align  = alBottom;
		pp->Align  = alTop;
		sp->Align  = alTop;
		sp->Height = pp->Visible? sp_wd : 0;
		break;
	default:	//0: 下
		sp->Align  = alTop;
		pp->Align  = alBottom;
		sp->Align  = alBottom;
		sp->Height = pp->Visible? sp_wd : 0;
	}
}
//---------------------------------------------------------------------------
void set_PanelAlign(TPanel *pp, TSplitter *sp, TAlign al, int sp_wd)
{
	switch (al) {
	case alTop:		set_PanelAlign(pp, sp, 3, sp_wd);	break;
	case alBottom:	set_PanelAlign(pp, sp, 0, sp_wd);	break;
	case alLeft:	set_PanelAlign(pp, sp, 2, sp_wd);	break;
	case alRight:	set_PanelAlign(pp, sp, 1, sp_wd);	break;
	default:
		;
	}
}

//---------------------------------------------------------------------------
//コントロールを右下隅に配置
//---------------------------------------------------------------------------
void set_ControlRBCorner(TControl *cp0, TControl *cp1)
{
	if (cp0->Visible && cp1->Visible) {
		cp1->Left = cp0->Left + cp0->ClientWidth  - cp1->Width;
		cp1->Top  = cp0->Top  + cp0->ClientHeight - cp1->Height;
		cp1->BringToFront();
	}
}

//---------------------------------------------------------------------------
//バッテリ残量(%)の文字列取得
//---------------------------------------------------------------------------
UnicodeString get_BatteryPercentStr()
{
	SYSTEM_POWER_STATUS ps;
	::GetSystemPowerStatus(&ps);
	if (ps.BatteryLifePercent==255) return "---";
	return UnicodeString().sprintf(_T("%3u"), ps.BatteryLifePercent);
}
//---------------------------------------------------------------------------
//バッテリ残量(時間)の文字列取得
//---------------------------------------------------------------------------
UnicodeString get_BatteryTimeStr()
{
	SYSTEM_POWER_STATUS ps;
	::GetSystemPowerStatus(&ps);
	UnicodeString ret;
	if (ps.ACLineStatus==1)
		ret.USET_T("ONLINE");
	else {
		int s = ps.BatteryLifeTime;
		if (s==-1)
			ret.USET_T("--:--");
		else
			ret.sprintf(_T("%u:%02u"), s/3600, (s%3600)/60);
	}
	return ret;
}

//---------------------------------------------------------------------------
//インターネットショートカットの作成
//---------------------------------------------------------------------------
bool make_url_file(UnicodeString fnam, UnicodeString url)
{
	if (fnam.IsEmpty() || url.IsEmpty()) return false;

	std::unique_ptr<TStringList> fbuf(new TStringList());
	fbuf->Text = UnicodeString().sprintf(_T("[InternetShortcut]\r\nURL=%s\r\n"), url.c_str());

	try {
		fbuf->SaveToFile(fnam, TEncoding::UTF8);
		return true;
	}
	catch (...) {
		return false;
	}
}

//---------------------------------------------------------------------------
//ドライブの取り外し
//  戻り値: 取り外せた/取り外し可能 の場合 true
//---------------------------------------------------------------------------
bool EjectDrive(
	UnicodeString drvnam,	//ドライブ名
	bool eject)				//true=取り外し/ false=確認のみ
{
	if (EjectDrive2(drvnam, eject)) return true;

	drvnam = ExtractFileDrive(drvnam);
	if (drvnam.IsEmpty()) return false;
	drvnam = IncludeTrailingPathDelimiter(drvnam);

	int typ = ::GetDriveType(drvnam.c_str());
	DWORD dwAccessFlags;
	switch(typ) {
	case DRIVE_REMOVABLE:	dwAccessFlags = GENERIC_READ|GENERIC_WRITE;	break;
	case DRIVE_CDROM:		dwAccessFlags = GENERIC_READ;				break;
	default:				return false;
	}

	bool ret = false;
	UnicodeString drv = "\\\\.\\" + ExtractFileDrive(drvnam);
	HANDLE hDrive = ::CreateFile(drv.c_str(), dwAccessFlags,
		FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hDrive!=INVALID_HANDLE_VALUE) {
		if (eject) {
			if (typ==DRIVE_CDROM) {
				//ID=37: 「取り出し」を直接実行
				ret = usr_SH->DriveContextMenu(Application->ActiveFormHandle, drvnam, NULL, 37).IsEmpty();
			}
			else {
				//ボリュームをロック
				bool locked = false;
				DWORD dwBytesReturned;
				for (int i = 0; i<20; i++) {
					if (::DeviceIoControl(hDrive, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL)) {
						locked = true; break;
					}
					Sleep(10000/20);	//10秒/20回リトライ
				}
				if (locked) {
					//マウント解除
					if (::DeviceIoControl(hDrive, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL)) {
						//削除を妨げるからデバイスを停止
						PREVENT_MEDIA_REMOVAL PMRBuffer;
						PMRBuffer.PreventMediaRemoval = FALSE;
						if (::DeviceIoControl(hDrive, IOCTL_STORAGE_MEDIA_REMOVAL,
							&PMRBuffer, sizeof(PREVENT_MEDIA_REMOVAL), NULL, 0, &dwBytesReturned, NULL))
						{
							//取り外し
							::DeviceIoControl(hDrive, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &dwBytesReturned, NULL);
						}
						ret = true;
					}
				}
			}
		}
		else ret = true;

		::CloseHandle(hDrive);
	}

	return ret;
}
//---------------------------------------------------------------------------
//リムーバブル/CD-ROM 以外の取り外し
//---------------------------------------------------------------------------
bool EjectDrive2(UnicodeString drvnam, bool eject)
{
	drvnam = ExtractFileDrive(drvnam);
	if (drvnam.IsEmpty()) return false;
	drvnam = IncludeTrailingPathDelimiter(drvnam);

	//ドライブのボリューム名を取得
	_TCHAR pszVolName[MAX_PATH];
	if (!::GetVolumeNameForVolumeMountPoint(drvnam.c_str(), pszVolName, MAX_PATH)) return false;
	UnicodeString vol_name = pszVolName;

	//ボリューム名の一致するデバイスを検索
	bool res = false;
	HDEVINFO hDevInfo = ::SetupDiGetClassDevs(NULL, 0, 0, DIGCF_PRESENT|DIGCF_ALLCLASSES);
	if (hDevInfo!=INVALID_HANDLE_VALUE) {
		bool found = false;
		SP_DEVINFO_DATA DevInfoData;
		DevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

		for (DWORD i=0; !found && SetupDiEnumDeviceInfo(hDevInfo, i, &DevInfoData); i++) {
			ULONG status  = 0;
			ULONG problem = 0;
			if (::CM_Get_DevNode_Status(&status, &problem, DevInfoData.DevInst, 0)!=CR_SUCCESS) continue;
			if ((status & DN_DISABLEABLE)==0 || (status & DN_REMOVABLE)==0) continue;

			DEVINST devInstChild;
			if (CM_Get_Child(&devInstChild, DevInfoData.DevInst, 0)==CR_SUCCESS) {
				_TCHAR pszDevInstanceId[MAX_PATH];
				if (CM_Get_Device_ID(devInstChild, pszDevInstanceId, MAX_PATH, 0)==CR_SUCCESS) {
					UnicodeString strDiList;
					ULONG ulSize;
					if (CM_Get_Device_Interface_List_Size(&ulSize, (LPGUID)&VolumeClassGuid, 
							pszDevInstanceId, 0)==CR_SUCCESS)
					{
						std::unique_ptr<_TCHAR[]> pszList(new _TCHAR[ulSize + 1]);
						if (CM_Get_Device_Interface_List((LPGUID)&VolumeClassGuid, 
							pszDevInstanceId, pszList.get(), ulSize, 0)==CR_SUCCESS)
								strDiList = UnicodeString(pszList.get());
					}
					if (strDiList.IsEmpty()) {
						if (CM_Get_Device_ID_List_Size(&ulSize, pszDevInstanceId,
								CM_GETIDLIST_FILTER_REMOVALRELATIONS )==CR_SUCCESS)
						{
							std::unique_ptr<_TCHAR[]> pDeviceIDsList(new _TCHAR[ulSize]);
							if (CM_Get_Device_ID_List(pszDevInstanceId, pDeviceIDsList.get(),
									ulSize, CM_GETIDLIST_FILTER_REMOVALRELATIONS)==CR_SUCCESS)
							{
								if (CM_Get_Device_Interface_List_Size(&ulSize, (LPGUID)&VolumeClassGuid,
										pDeviceIDsList.get(), 0)==CR_SUCCESS)
								{
									std::unique_ptr<_TCHAR[]> pDeviceInterfacesList(new _TCHAR[ulSize]);
									if (CM_Get_Device_Interface_List((LPGUID)&VolumeClassGuid, pDeviceIDsList.get(),
										 pDeviceInterfacesList.get(), ulSize, 0) == CR_SUCCESS)
											strDiList = pDeviceInterfacesList.get();
								}
							}
						}
					}
					if (!strDiList.IsEmpty()) {
						strDiList = IncludeTrailingPathDelimiter(strDiList);
						if (!GetVolumeNameForVolumeMountPoint(strDiList.c_str(), pszVolName, MAX_PATH)) continue;
						if (SameText(vol_name, pszVolName)) {
							found = true;
							//取り外し
							res = eject? (CM_Request_Device_Eject(DevInfoData.DevInst, NULL, NULL, 0, 0)==CR_SUCCESS) : true;
						}
					}
				}
			}
		}
		::SetupDiDestroyDeviceInfoList(hDevInfo);
	}
	return res;
}

//---------------------------------------------------------------------------
//コントロールのクラスチェック
//---------------------------------------------------------------------------
bool class_is_LabeledEdit(TObject *op)
{
	return op->ClassNameIs("TLabeledEdit");
}
//---------------------------------------------------------------------------
bool class_is_MaskEdit(TObject *op)
{
	return op->ClassNameIs("TMaskEdit");
}
//---------------------------------------------------------------------------
bool class_is_Edit(TObject *op)
{
	return op->ClassNameIs("TEdit");
}
//---------------------------------------------------------------------------
bool class_is_CheckBox(TObject *op)
{
	return op->ClassNameIs("TCheckBox");
}
//---------------------------------------------------------------------------
bool class_is_ComboBox(TObject *op)
{
	return op->ClassNameIs("TComboBox");
}
//---------------------------------------------------------------------------
bool class_is_ListBox(TObject *op)
{
	return op->ClassNameIs("TListBox");
}
//---------------------------------------------------------------------------
bool class_is_CheckListBox(TObject *op)
{
	return op->ClassNameIs("TCheckListBox");
}
//---------------------------------------------------------------------------
bool class_is_RadioGroup(TObject *op)
{
	return op->ClassNameIs("TRadioGroup");
}

//---------------------------------------------------------------------------
void cursor_HourGlass()
{
	Screen->Cursor = crHourGlass;
}
//---------------------------------------------------------------------------
void cursor_Default()
{
	Screen->Cursor = crDefault;
}

//---------------------------------------------------------------------------
//メニューの余計なセパレータを非表示にする
//※最初の表示で AutoLineReduction が効かない問題に対処
//---------------------------------------------------------------------------
void reduction_MenuLine(TMenuItem *mp)
{
	if (!mp) return;

	bool is_sp = false;
	TMenuItem *ip_0 = NULL;		//最初の表示項目
	TMenuItem *ip_1 = NULL;		//最後の表示項目

	for (int i=0; i<mp->Count; i++) {
		TMenuItem *ip = mp->Items[i];
		if (ip->Action) ip->Action->Update();
		if (USAME_TS(ip->Caption, "-")) {
			ip->Visible = !is_sp;
			is_sp = true;
		}
		else if (ip->Visible) {
			is_sp = false;
		}

		if (ip->Visible) {
			if (!ip_0) ip_0 = ip;
			ip_1 = ip;
		}
	}

	if (ip_0 && USAME_TS(ip_0->Caption, "-")) ip_0->Visible = false;
	if (ip_1 && USAME_TS(ip_1->Caption, "-")) ip_1->Visible = false;
}

//---------------------------------------------------------------------------
//メッセージ文字列の取得
//---------------------------------------------------------------------------
UnicodeString LoadUsrMsg(int id, UnicodeString s)
{
	std::unique_ptr<TStringList> msg_lst(new TStringList());
	msg_lst->Text =
		"1001=%sが見つかりません。\n"							//USTR_NotFound
		"1002=%sこの操作には対応していません。\n"				//USTR_OpeNotSuported
		"1003=この操作はできません。\n"							//USTR_CantOperate
		"1004=対応していない形式です。\n"						//USTR_FmtNotSuported
		"1005=一時解凍に失敗しました。\n"						//USTR_FaildTmpUnpack
		"1006=移動先が同じです。\n"								//USTR_SameMoveDest
		"1007=コピー先が同じです。\n"							//USTR_SameCopyDest
		"1008=ディレクトリが含まれています。\n"					//USTR_IncludeDir
		"1009=不正な日付または時刻です。\n"						//USTR_IllegalDate
		"1010=不正な指定です。\n"								//USTR_IllegalParam
		"1011=不正な形式です。\n"								//USTR_IllegalFormat
		"1012=不正なアドレスです\n"								//USTR_IllegalAddress
		"1013=起動に失敗しました。\n"							//USTR_FaildExec
		"1014=処理に失敗しました。\n"							//USTR_FaildProc
		"1015=読み込みに失敗しました。\n"						//USTR_FaildLoad
		"1016=%sの保存に失敗しました。\n"						//USTR_FaildSave
		"1017=削除に失敗しました。\n"							//USTR_FaildDelete
		"1018=コピーに失敗しました。\n"							//USTR_FaildCopy
		"1019=メニューが実行できません。\n"						//USTR_FaildMenu
		"1020=中断しました。\n"									//USTR_Canceled
		"1021=ワークリストを開けません。\n"						//USTR_WlistCantOpen
		"1022=処理中にこの操作はできません。\n"					//USTR_ProcBusy
		"1023=対象がありません。\n"								//USTR_NoObject
		"1024=正規表現に誤りがあります。\n"						//USTR_IllegalRegEx
		"1025=書式文字列の入力\n"								//USTR_InputFmtStr
		"1026=すでに登録されています。\n"						//USTR_Registered
		"1027=　検索中...／　ESCキーで中断します。\n"			//USTR_SearchingESC
		"1028=　計算中...／　ESCキーで中断します。\n"			//USTR_CalculatingESC
		"1029=　処理中...／　しばらくお持ちください。\n"		//USTR_WaitForReady
		"1030=レスポンスファイルの作成に失敗しました。\n"		//USTR_FaildListFile
		"1031=一時ディレクトリが作成できません。\n"				//USTR_CantMakeTmpDir
		"1032=不正な構文です。\n"								//USTR_SyntaxError
		"1033=対応する制御文が見つかりません。\n"				//USTR_BadStatmet
		"1034=パラメータが指定されていません。\n"				//USTR_NoParameter
		"1035=アプリケーションが見つかりません。\n"				//USTR_AppNotFound
		"1036=画像の準備ができていません\n"						//USTR_ImgNotReady
		"1037=ファイルを開けません\n"							//USTR_FileNotOpen
		"1038=ディレクトリが見つかりません。\n"					//USTR_DirNotFound
		"1039=無効なコマンドです。\n"							//USTR_InvalidCmd
		"1040=. で区切って複数指定可能\n"						//USTR_HintMltFExt
		"1041=; で区切って複数指定可能\n"						//USTR_HintMltSepSC
		"1042=対応するエディタがありません\n"					//USTR_NoEditor
		"1043=再生できません。\n"								//USTR_CantPlay
		"1044=%sに名前を付けて保存\n"							//USTR_SaveAs
		"1045=ディレクトリ[%s]が作成できません。\n"				//USTR_CantCreDir
		"1046=%sをコピーしますか?\n"							//USTR_CopyQ
		"1047=%sを削除しますか?\n"								//USTR_DeleteQ
		"1048=%sを完全削除しますか?\n"							//USTR_CompDeleteQ
		"1049=%sの履歴をすべて削除しますか?\n"					//USTR_DelHistoryQ
		"1050=すべての場所のすべてのマークを解除しますか?\n"	//USTR_ClrAllMarkQ
		"1051=コマンドを中断しますか?\n"						//USTR_CancelCmdQ
		"1052=アーカイブを開けません。\n"						//USTR_ArcNotOpen
		"1053=テキストファイルではありません。\n"				//USTR_NotText
		"1054=ディレクトリの作成\n"								//USTR_CreateDir
		"1055=%sを抽出しますか?\n"								//USTR_Extract
		"1056=%s情報が取得できません。\n"						//USTR_CantGetInfo
		"1057=名前が重複しています。\n"							//USTR_DuplicateName
		"1058=上書きしてもよいですか?\n"						//USTR_OverwriteQ
		"1059=%sをアップロードしますか?\n"						//USTR_UploadQ
		"1060=%sをダウンロードしますか?\n"						//USTR_DownloadQ
		"1061=削除開始  %s\n"									//USTR_BeginDelete
		"1062=同名ファイルの処理 <%s>\n"						//USTR_SameName
		"1063=名前の変更\n"										//USTR_Rename
		"1064=ディレクトリ属性の異なる同名対象が存在します。\n"	//USTR_NgSameName
		"1065=不正な日付条件です。\n"							//USTR_IllegalDtCond
		"1066=選択マスク中のワークリストは変更できません。\n"	//USTR_WorkFiltered

		"1100=選択項目\n"										//USTR_SelectedItem
		"1101=一覧\n"											//USTR_List
		"1102=アップロード\n"									//USTR_Upload
		"1103=ダウンロード\n";									//USTR_Download

	UnicodeString ret_str = REPLACE_TS(msg_lst->Values[IntToStr(id)], "／", "\r\n");
	ret_str = s.IsEmpty()? replace_regex(ret_str, _T("%s[のをがに]?"), null_TCHAR) : ReplaceStr(ret_str, "%s", s);
	return ret_str;
}
//---------------------------------------------------------------------------
UnicodeString LoadUsrMsg(int id, const _TCHAR *s)
{
	return LoadUsrMsg(id, UnicodeString(s));
}
//---------------------------------------------------------------------------
UnicodeString LoadUsrMsg(int id, int id_s)
{
	return LoadUsrMsg(id, LoadUsrMsg(id_s));
}
//---------------------------------------------------------------------------
