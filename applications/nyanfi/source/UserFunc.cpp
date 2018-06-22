//----------------------------------------------------------------------//
// �ėp���ʊ֐�															//
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
//�V�X�e���F
TColor scl_Window;
TColor scl_WindowText;
TColor scl_Highlight;
TColor scl_HighlightText;
TColor scl_BtnFace;
TColor scl_BtnText;
TColor scl_Menu;
TColor scl_MenuText;

//---------------------------------------------------------------------------
//�R�[�h�y�[�W���X�g
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
//�V�X�e���F�̏�����
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
//�}�E�X�J�[�\���ʒu�̃E�B���h�E���擾
//---------------------------------------------------------------------------
HWND get_window_from_pos()
{
	POINT p;
	::GetCursorPos(&p);
	return ::WindowFromPoint(p);
}

//---------------------------------------------------------------------------
//�R���g���[���ɉE�N���b�N���j���[��\��
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
//ListBox �Ƀe�L�X�g��ݒ�
//---------------------------------------------------------------------------
void set_ListBoxText(TListBox *lp, const _TCHAR *s)
{
	lp->Items->Text = s;
}
//---------------------------------------------------------------------------
//ComboBox �Ƀe�L�X�g��ݒ�
//---------------------------------------------------------------------------
void set_ComboBoxText(TComboBox *cp, const _TCHAR *s)
{
	cp->Items->Text = s;
}

//---------------------------------------------------------------------------
//ComboBox �ɗ�����ǉ�
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
				lp->Insert(0, kwd);	//�擪�ɒǉ�
			}
			cp->Text = kwd;
		}
	}
}
//---------------------------------------------------------------------------
//ComboBox ���痚�����폜
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
//IME�����
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
//���X�g�{�b�N�X�̍��ڈʒu�Ƀ}�E�X�J�[�\�����ړ�
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
//���[�U��`���b�Z�[�W�� EAbort �𑗏o
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
//�G���[���b�Z�[�W
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
//�x�����b�Z�[�W
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
//���b�Z�[�W
//---------------------------------------------------------------------------
void msgbox_OK(UnicodeString msg, UnicodeString tit)
{
	Application->MessageBox(msg.c_str(),
		def_if_empty(tit, _T("�m�F")).c_str(), MB_OK);
}
//---------------------------------------------------------------------------
//�m�F���b�Z�[�W
//---------------------------------------------------------------------------
bool msgbox_Y_N(UnicodeString msg, UnicodeString tit)
{
	int res = Application->MessageBox(msg.c_str(),
				def_if_empty(tit, _T("�m�F")).c_str(), MB_YESNO|MB_ICONQUESTION);
	return (res==IDYES);
}
//---------------------------------------------------------------------------
int msgbox_Y_N_C(UnicodeString msg, UnicodeString tit)
{
	return Application->MessageBox(msg.c_str(),
		def_if_empty(tit, _T("�m�F")).c_str(), MB_YESNOCANCEL|MB_ICONQUESTION);
}

//---------------------------------------------------------------------------
//�w�蕝�Ɏ��܂�悤�Ƀp�X���𒲐�
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
				if (!UENDS_TS("�c", dnam) && dnam.Length()>4) {
					plst[i] = dnam.SubString(1, dnam.Length() - 3).UCAT_T("�c");
					changed = true;
					break;
				}
				if (UENDS_TS("�c", dnam) && dnam.Length()>2) {
					plst[i].Delete(dnam.Length() - 1, 1);
					changed = true;
					break;
				}
				if (i>0 && i<plst.Length-1 && UENDS_TS("�c", dnam) && dnam.Length()==2) {
					plst[i] = "�c";
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
//�w��L�����o�X�̃t�H���g���������f�B���O�������Ă��邩�H
//---------------------------------------------------------------------------
bool has_Leading(TCanvas *cv)
{
	return (cv->TextHeight("Q")>abs(cv->Font->Height));
}

//---------------------------------------------------------------------------
//�w��t�H���g�̕`�捂���擾
//---------------------------------------------------------------------------
int get_FontHeight(TFont *font, int mgn, int min_mgn)
{
	std::unique_ptr<Graphics::TBitmap> bmp(new Graphics::TBitmap());
	TCanvas *cv = bmp->Canvas;
	cv->Font->Assign(font);
	int fh = cv->TextHeight("Q");
	if (mgn>0) {
		//�������f�B���O�Ƒ��E
		int ld = fh - abs(cv->Font->Height);
		if (ld>0) mgn -= ld;
		if (mgn<min_mgn) mgn = min_mgn;
		fh += mgn;
	}
	return fh;
}
//---------------------------------------------------------------------------
//�w��t�H���g n ����(���p�P��)�̕`�敝���擾
//---------------------------------------------------------------------------
int get_CharWidth_Font(TFont *font, int n)
{
	std::unique_ptr<Graphics::TBitmap> bmp(new Graphics::TBitmap());
	TCanvas *cv = bmp->Canvas;
	cv->Font->Assign(font);
	return get_CharWidth(cv, n);
}

//---------------------------------------------------------------------------
//���͗��̕�����𐔒l��
//---------------------------------------------------------------------------
int EditToInt(TLabeledEdit *ep,
	int def)	//�f�t�H���g�l (default = 0);
{
	return ep->Text.ToIntDef(def);
}
//---------------------------------------------------------------------------
int EditToInt(TEdit *ep, int def)
{
	return ep->Text.ToIntDef(def);
}

//---------------------------------------------------------------------------
//�T�C�Y�̐��l�𕶎����(TB�܂�)
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
//�T�C�Y�̐��l�𕶎����(GB�܂�)
//---------------------------------------------------------------------------
UnicodeString get_size_str_G(
	__int64 sz,
	int wd,		//���w�� default = 10
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
	int wd)		//���w�� default = 14
{
	return ins_spc_length(((sz>=0)? FormatFloat(",0", (double)sz) : EmptyStr), wd);
}
//---------------------------------------------------------------------------
UnicodeString get_size_str_C(int sz)
{
	return FormatFloat(",0", (double)sz);
}

//---------------------------------------------------------------------------
//������� TDateTime �ɕϊ�
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
//������������擾
//---------------------------------------------------------------------------
UnicodeString format_DateTime(
	TDateTime dt,	//����
	bool omit)		//�����Ȃ���t���ȗ� (default = false)
{
	if (omit && IsToday(dt))
		return FormatDateTime("hh:nn:ss", dt);
	else
		return FormatDateTime("yyyy/mm/dd hh:nn:ss", dt);
}
//---------------------------------------------------------------------------
//���t��������擾
//---------------------------------------------------------------------------
UnicodeString format_Date(TDateTime dt)
{
	return FormatDateTime("yyyy/mm/dd", dt);
}

//---------------------------------------------------------------------------
//������������擾 (FormatDateTime �̊g��)
// $EN ������ꍇ���P�[���� en-US �ɂ���
//---------------------------------------------------------------------------
UnicodeString format_DateTimeEx(UnicodeString fmt, TDateTime dt)
{
    TFormatSettings fs = remove_text(fmt, _T("$EN"))? TFormatSettings::Create("en-US") : TFormatSettings::Create();
	return FormatDateTime(fmt, dt, fs);
}

//---------------------------------------------------------------------------
//���t�������擾
//�@���Ύw��: {<|=|>}-n{D|M|Y}�@�@(D:���AM:���AY:�N)
//�@��Ύw��: {<|=|>}yyyy/mm/dd
//
//  �߂�l : 1: < ���Â�/ 2: = ����/ 3: > ���V����/ 0: �����Ȃ�
//			-1: �G���[
//---------------------------------------------------------------------------
int get_DateCond(UnicodeString prm, TDateTime &dt)
{
	try {
		int cnd = 0;

		if (!prm.IsEmpty()) {
			cnd = UnicodeString("<=>").Pos(prm[1]);
			if (cnd>0) {
				prm.Delete(1, 1);
				//��Ύw��
				if (prm.Length()==10 && prm[5]=='/' && prm[8]=='/') {
					dt = TDate(prm);
				}
				//���Ύw��
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
//�� �~ �� ��������擾
//---------------------------------------------------------------------------
UnicodeString get_wd_x_hi_str(int w, int h)
{
	return UnicodeString().sprintf(_T("%u �~ %u"), w, h);
}

//---------------------------------------------------------------------------
//�摜�T�C�Y��񕶎�����擾
//---------------------------------------------------------------------------
UnicodeString get_img_size_str(int w, int h, UnicodeString name)
{
	if (name.IsEmpty()) name = "�摜�T�C�Y";
	UnicodeString infstr = make_PropLine(name, get_wd_x_hi_str(w, h));
	if (w>0 && h>0) infstr.cat_sprintf(_T("  (%s)"), get_AspectStr(w, h).c_str());	//�A�X�y�N�g��
	return infstr;
}

//---------------------------------------------------------------------------
//�R�[�h�y�[�W�̖��O���擾
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
//���O�̃R�[�h�y�[�W���擾
//---------------------------------------------------------------------------
int get_CodePageOfName(UnicodeString code_name)
{
	std::unique_ptr<TStringList> lst(new TStringList());
	lst->Text = CodePageListX;
	return lst->Values[code_name].ToIntDef(0);
}

//---------------------------------------------------------------------------
//���ʃJ�E���g�𕶎����
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
//���ʃ��X�g�̐��`
//  "�t�@�C����\t���e" --> "�t�@�C����   ���e"
//---------------------------------------------------------------------------
int format_res_list(TStringList *lst, int mgn, int min_wd)
{
	//�t�@�C�����̍ő啝���擾
	int max_len = 0;
	for (int i=0; i<lst->Count; i++) max_len_half(max_len, get_pre_tab(lst->Strings[i]));
	max_len += mgn;
	if (max_len<min_wd) max_len = min_wd;

	//���ʂ𐮌`
	for (int i=0; i<lst->Count; i++) {
		UnicodeString lbuf = lst->Strings[i];
		lst->Strings[i] = align_l_str(get_pre_tab(lbuf) ,max_len, get_post_tab(lbuf));
	}

	return max_len;
}

//---------------------------------------------------------------------------
//�t�@�C���̃v���p�e�B��\��
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
//���݈ʒu�̃��X�g���ڂ��폜
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
//���X�g�{�b�N�X�̍��ڂɌr����`�� (�w�i�̔��]�F)
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
//�[�_���t���b�g�Ȑ���`�� (w=1 �̏ꍇ�͂��̂܂�)
//---------------------------------------------------------------------------
void draw_Line(
	TCanvas *cv,
	int x0, int y0, int x1, int y1,
	int w,				//����
	TColor col, 		//�F
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
//�w�i�F(Brush)�̋P�x���l�����ăZ�p���[�^��`��
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
//�w�i�F(Brush)���l�����ĕ�������`��
//---------------------------------------------------------------------------
void draw_separateLine(TCanvas *cv, TRect rc,
	int pos)	//�ʒu 0=���/ 1=����/ 2=���� (default = 0)
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
//�\�[�g�����}�[�N��`��
//---------------------------------------------------------------------------
void draw_SortMark(TCanvas *cv, int x, int y,
	bool is_asc,	//����
	TColor fg)		//�}�[�N�F
{
	TPoint mrk[3];
	//��
	if (is_asc) {
		mrk[0] = Point(x,	  y + 3);
		mrk[1] = Point(x + 6, y + 3);
		mrk[2] = Point(x + 3, y);
	}
	//��
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
//�{�^���Ƀ}�[�N��ݒ�
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
//�k���䗦���擾
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
		//�c���֌W������
		r = std::max(std::min(1.0 * vw/ih, 1.0 * vh/iw), std::min(1.0 * vw/iw, 1.0 * vh/ih));
	else
		//�c���֌W���Ⴄ
		r = std::min(std::min(1.0 * vw/ih, 1.0 * vh/iw), std::min(1.0 * vw/iw, 1.0 * vh/ih));

	if (r>limit) r = 1.0;

	return r;
}

//---------------------------------------------------------------------------
//�L�[���X�g�̍쐬
//---------------------------------------------------------------------------
void make_KeyList(TStrings *lst)
{
	lst->Clear();

	UnicodeString itmbuf;
	for (int i=0; i<26; i++) lst->Add(itmbuf.sprintf(_T("%c"), 'A'+i));		//�p��
	for (int i=0; i<10; i++) lst->Add(itmbuf.sprintf(_T("%c"), '0'+i));		//����

	for (int i=0; i<12; i++) lst->Add(itmbuf.sprintf(_T("F%u"), i + 1));	//F�L�[
	lst->AddStrings(
		SplitString("Del|Ins|BkSp|Left|Right|UP|Down|PgUp|PgDn|Home|End|Pause|Tab|Esc|Enter|Space|App|-", "|"));
	lst->Add((::GetKeyboardType(0)==7)? "^" : "'");
	lst->AddStrings(SplitString("\\|@|[|;|:|]|,|.|/|�_", "|"));
	for (int i=0; i<10; i++) lst->Add(itmbuf.sprintf(_T("10Key_%u"), i));	//10�L�[
	lst->AddStrings(SplitString("10Key_*|10Key_+|10Key_-|10Key_/|10Key_.", "|"));
}

//---------------------------------------------------------------------------
//�L�[��������Ă��邩?
//---------------------------------------------------------------------------
bool is_KeyDown(WORD key)
{
	return (HIBYTE(::GetAsyncKeyState(key)) != 0);
}
//---------------------------------------------------------------------------
//Ctrl+C,V �ȊO�� Ctrl+�` �������ꂽ�H
//---------------------------------------------------------------------------
bool is_KeyPress_CtrlNotCV(WORD key)
{
	return (is_KeyDown(VK_CONTROL) && key!=0x03 && key!=0x16);
}
//---------------------------------------------------------------------------
//Ctrl+C,V,X �ȊO�� Ctrl+�` �������ꂽ�H
//---------------------------------------------------------------------------
bool is_KeyPress_CtrlNotCVX(WORD key)
{
	return (is_KeyDown(VK_CONTROL) && key!=0x03 && key!=0x16 && key!=0x18);
}

//---------------------------------------------------------------------------
//�_�C�A���O�L�[���H
//---------------------------------------------------------------------------
bool is_DialogKey(WORD Key)
{
	return (Key==VK_LEFT || Key==VK_RIGHT || Key==VK_UP || Key==VK_DOWN ||
			Key==VK_PRIOR || Key==VK_NEXT || Key==VK_HOME || Key==VK_END || Key==VK_TAB);
}

//---------------------------------------------------------------------------
//WPARAM ����V�t�g��Ԃ��擾
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
//HTML�����̃w�b�_��������擾
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
//HTML�����̃w�b�_���(charset, TITLE, META)���擾
//---------------------------------------------------------------------------
void get_HtmlInf(
	UnicodeString fnam,		//HTML�t�@�C����
	TStringList *lst)		//[o] ��񃊃X�g(���O=�l �̌`���Ŋi�[)
{
	if (!file_exists(fnam)) return;

	//�w�b�_�������擾
	UnicodeString headstr = get_html_header(fnam);
	if (!headstr.IsEmpty()) {
		UnicodeString itmbuf;

		//�����^�錾
		UnicodeString dtd_str = get_tkn(get_tkn_r(headstr, _T("<!DOCTYPE ")), '>');
		int p = pos_i(_T("//DTD "),  dtd_str);
		if (p>0) dtd_str = get_tkn_m(dtd_str, _T("//DTD "), _T("//"));
		if (!dtd_str.IsEmpty()) {
			if (USAME_TI(dtd_str, "html")) dtd_str = "HTML5";
			lst->Add(itmbuf.sprintf(_T("%*s: %s"), FPRP_NAM_WD, _T("DOCTYPE"), dtd_str.c_str()));
		}

		//charset ���`�F�b�N
		UnicodeString charset;
		p = pos_i(_T("charset"), headstr);
		if (p>0) {
			UnicodeString tmp = headstr.SubString(p, headstr.Length() - p + 1);
			tmp = get_tkn(get_tkn_m(tmp, '=', '>'), '/');
			charset = Trim(REPLACE_TS(tmp, "\"", ""));
			if (!charset.IsEmpty())
				lst->Add(itmbuf.sprintf(_T("%*s: %s"), FPRP_NAM_WD, _T("charset"), charset.c_str()));
		}

		//�G���R�[�f�B���O���w�肵�čēǂݍ���
		if (!charset.IsEmpty()) {
			int code_page = get_CodePageOfName(charset);
			if (code_page>0) headstr = get_html_header(fnam, code_page);
		}

		//�R�����g�������폜
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

		//�^�C�g��
		p0 = pos_i(_T("<TITLE>"),  headstr);
		p1 = pos_i(_T("</TITLE>"), headstr);
		if (p0>0 && p1>p0) {
			UnicodeString tit = headstr.SubString(p0 + 7, p1 - p0 - 7);
			if (!tit.IsEmpty()) lst->Add(itmbuf.sprintf(_T("%*s: %s"), FPRP_NAM_WD, _T("title"), tit.c_str()));
		}

		//���^���
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
//Jpeg ���(�`���A�J���[�Ȃ�)���擾
//---------------------------------------------------------------------------
void get_JpgExInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst)		//[o] ��񃊃X�g
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
				add_PropLine(_T("�`��"), ((mkr==0xffc0)? "�x�[�X���C��" : "�v���O���b�V�u"), lst);
				fs->Seek(5, soFromCurrent);

				BYTE n;	//�\���v�f��
				if (fs->Read(&n, 1)!=1) Abort();
				//�O���[�X�P�[��
				if (n==1) {
					add_PropLine(_T("�J���["), "�O���[�X�P�[��", lst);
				}
				//YCbCr
				else if (n==3) {
					BYTE hn[3] = {0};	//�����T���v�����O�l
					BYTE vn[3] = {0};	//�����T���v�����O�l
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

					//��CbCr��11�łȂ��ꍇ�ɑΉ�(�����͂�����Ɖ�����)
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
					add_PropLine(_T("�J���["), "CMYK", lst);
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
//Exif���̎擾
//---------------------------------------------------------------------------
bool get_ExifInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst,		//[o] ��񃊃X�g
	int *ori,				//[o] ����	(default = NULL)
	unsigned int *i_wd,		//[o] ��	(default = NULL)
	unsigned int *i_hi)		//[o] ����	(default = NULL)
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
		"271=���[�J�[��\n"
		"272=���f��\n"
		"36867=�B�e����\n"
		"33437=�i��l\n"
		"33434U=�I�o����\n"
		"37380=�I���␳\n"
		"34850L=�I�o�v���O����\n"
		"34855=ISO���x\n"
		"37383=��������\n"
		"37386=�œ_����\n"
		"NK:5=�v�a\n"
		"CN:4.7=�v�a\n"
		"37385=�t���b�V��\n"
		"274L=�摜����\n"
		"NK:7=�t�H�[�J�X\n"
		"42036=�����Y\n"
		"305=�\�t�g��\n"
		"FOVbVer=FOVb Ver.\n"
		"GPS:2=GPS�ܓx\n"
		"GPS:4=GPS�o�x\n";

	for (int i=0; i<d_lst->Count; i++)
		add_PropLine_if(d_lst->ValueFromIndex[i].c_str(), i_lst->Values[d_lst->Names[i]], lst);

	if (ori) *ori = get_ListIntVal(i_lst.get(), _T("274"));
	return true;
}

//---------------------------------------------------------------------------
//Exif��񕶎���(�ȗ�)�̎擾
//---------------------------------------------------------------------------
UnicodeString get_ExifInfStr(
	UnicodeString fnam,		//�t�@�C����
	int *ori,				//[o] ����	(default = NULL)
	unsigned int *i_wd,		//[o] ��	(default = NULL)
	unsigned int *i_hi)		//[o] ����	(default = NULL)
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
//Exif��񕶎���(�T���l�C���p)�̎擾
//---------------------------------------------------------------------------
UnicodeString get_ExifInfThumb(
	UnicodeString fnam,		//�t�@�C����
	int *ori)				//[o] ����
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

	//����
	if (ori) *ori = get_ListIntVal(i_lst.get(), _T("274"));

	return ret_str;
}

//---------------------------------------------------------------------------
//GPS���(�ܓx�A�o�x)���擾
//---------------------------------------------------------------------------
bool get_GpsInf(
	UnicodeString fnam,		//�t�@�C����
	double *lat,			//[o] �ܓx
	double *lng)			//[o] �o�x
{
	std::unique_ptr<TStringList> xlst(new TStringList());
	if (!EXIF_GetInf(fnam, xlst.get())) return false;
	UnicodeString lat_str = Trim(get_tkn(xlst->Values["GPS:2"], '('));	//�ܓx
	UnicodeString lng_str = Trim(get_tkn(xlst->Values["GPS:4"], '('));	//�o�x
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
//�r�b�O�G���f�B�A�����烊�g���G���f�B�A���֕ϊ�
//---------------------------------------------------------------------------
unsigned int big_to_little(BYTE *p)
{
	return (unsigned int)((p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]);
}

//---------------------------------------------------------------------------
//�t�@�C���̌��݈ʒu�Ɏw�蕶���񂪂��邩�H
//  ����Β���ʒu�ցA������Ό��݈ʒu�̂܂�
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
//�t�@�C���̌��݈ʒu����`�����N������
//  ����Β���ʒu(�T�C�Y)��
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
//�t�@�C���̌��݈ʒu����`�����N�����擾
//  �߂�l: ���ʎq		EmptyStr: �G���[
//---------------------------------------------------------------------------
UnicodeString get_chunk_hdr(
	TFileStream *fs,
	unsigned int *size)		//[o] �`�����N�T�C�Y
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
//�A�C�R���t�@�C���̏����擾
//---------------------------------------------------------------------------
void get_IconInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst)		//[o] ��񃊃X�g
{
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		fs->Seek(4, soFromBeginning);

		unsigned short ico_n;
		if (fs->Read(&ico_n, 2)<2) UserAbort(USTR_IllegalFormat);
		add_PropLine(_T("�A�C�R����"), ico_n, lst);

		for (int i=0; i<ico_n; i++) {
			//�T�C�Y
			unsigned char b_buf;
			if (fs->Read(&b_buf, 1)!=1) UserAbort(USTR_IllegalFormat);
			int wd = (b_buf==0)? 256 : b_buf;
			if (fs->Read(&b_buf, 1)!=1) UserAbort(USTR_IllegalFormat);
			int hi = (b_buf==0)? 256 : b_buf;
			UnicodeString lbuf;
			lbuf.sprintf(_T("%*u: %s"), FPRP_NAM_WD, i + 1, get_wd_x_hi_str(wd, hi).c_str());
			//�F
			if (fs->Read(&b_buf, 1)!=1) UserAbort(USTR_IllegalFormat);
			if (b_buf==0)
		 		lbuf.UCAT_T("  256�F�ȏ�");
			else
		 		lbuf.cat_sprintf(_T("  %u�F"), b_buf);
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
//WAV�t�@�C���̏����擾
//---------------------------------------------------------------------------
void get_WavInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst)		//[o] ��񃊃X�g
{
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));

		if (!USAME_TS(get_chunk_hdr(fs.get()), "RIFF") || !check_read(fs.get(), "WAVE")) UserAbort(USTR_IllegalFormat);
		if (!sea_chunk(fs.get(),  "fmt "))	UserAbort(USTR_IllegalFormat);

		DWORD dwBuf;
		if (fs->Read(&dwBuf, 4)<4)			UserAbort(USTR_IllegalFormat);
		int p_data = fs->Seek(0, soFromCurrent) + dwBuf;

		//�t�H�[�}�b�g�����擾
		PCMWAVEFORMAT pwf;
		fs->Read(&pwf, sizeof(PCMWAVEFORMAT));
		if (pwf.wf.wFormatTag==WAVE_FORMAT_PCM || pwf.wf.wFormatTag==WAVE_FORMAT_EXTENSIBLE) {
			if (pwf.wf.nChannels==0) TextAbort(_T("�s���ȃ`�����l�����ł��B"));
			int chn    = pwf.wf.nChannels;
			int s_rate = pwf.wf.nSamplesPerSec;
			int s_byte = pwf.wf.nBlockAlign/chn;
			int s_bit  = s_byte * 8;

			fs->Seek(p_data, soFromBeginning);
			if (!sea_chunk(fs.get(), "data")) TextAbort(_T("data�`�����N��������܂���B"));
			fs->Read(&dwBuf, 4);
			int length = (int)(dwBuf/(s_byte*chn)/(s_rate/1000.0));

			UnicodeString lbuf = get_PropTitle(_T("�`��")).cat_sprintf(_T("%uHz %ubit "), s_rate, s_bit);
			if (chn<=2)
				lbuf.cat_sprintf(_T("%s"), (chn==2)? _T("�X�e���I") : _T("���m"));
			else
				lbuf.cat_sprintf(_T("%uch"), chn);
			lst->Add(lbuf);
			add_PropLine(_T("����"), mSecToTStr(length), lst);
		}
		else if (pwf.wf.wFormatTag==WAVE_FORMAT_MPEGLAYER3)
			add_PropLine(_T("�`��"), "MPEG 3 Layer 1", lst);
		else
			lst->Add(get_PropTitle(_T("�`��")).cat_sprintf(_T("���̑�(0x%04X)"), pwf.wf.wFormatTag));
	}
	catch (EAbort &e) {
		add_list_errmsg(lst, e.Message);
	}
	catch (...) {
		add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}
}

//---------------------------------------------------------------------------
//Opus�t�@�C���̏����擾
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
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst)		//[o] ��񃊃X�g
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
		add_PropLine(_T("�`�����l����"), n, lst);
		fs->Seek(2, soFromCurrent);		//pre-skip
		DWORD i_rate;
		if (fs->Read(&i_rate, 4)!=4) Abort();
		add_PropLine(_T("�T���v�����[�g"), UnicodeString().sprintf(_T("%uHz"), i_rate), lst);

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
//CDA�t�@�C���̏����擾
//---------------------------------------------------------------------------
bool get_CdaInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst,		//[o] ��񃊃X�g
	int *len)				//[o] ����
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
		//�g���b�N��
		WORD wBuf;
		fs->Read(&wBuf, 2);
		if (lst) add_PropLine(_T("�g���b�N��"), wBuf, lst);
		fs->Seek(4, soFromCurrent);
		//�J�n�ʒu
		fs->Read(&dwBuf, 4);
		int t = dwBuf*1000/75;	//= �Z�N�^��*1000/75 (75=1�b�Ԃ̃Z�N�^��)
		if (lst) add_PropLine(_T("�J�n�ʒu"), mSecToTStr(t), lst);
		//����
		fs->Read(&dwBuf, 4);
		t = dwBuf*1000/75;		//= �Z�N�^��*1000/75 (75=1�b�Ԃ̃Z�N�^��)
		if (len) *len = t;
		if (lst) add_PropLine(_T("����"), mSecToTStr(t), lst);
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
//���^�t�@�C���̏����擾
//---------------------------------------------------------------------------
void get_MetafileInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst)		//[o] ��񃊃X�g
{
	try {
		std::unique_ptr<TMetafile> mbuf(new TMetafile());
		mbuf->LoadFromFile(fnam);
		lst->Add(get_img_size_str(mbuf->Width, mbuf->Height, "�T�C�Y"));
		add_PropLine_if(_T("�쐬��"), mbuf->CreatedBy,   lst);
		add_PropLine_if(_T("����"),   mbuf->Description, lst);
	}
	catch (...) {
		add_list_errmsg(lst, LoadUsrMsg(USTR_FileNotOpen));
	}
}

//---------------------------------------------------------------------------
//PNG�t�@�C���̏����擾
//---------------------------------------------------------------------------
bool get_PngInf(
	UnicodeString fnam, 	//�t�@�C����
	TStringList *lst,		//[o] ��񃊃X�g
	unsigned int *i_wd,		//[o] ��	(default = NULL)
	unsigned int *i_hi)		//[o] ����	(default = NULL)
{
	bool ret = false;
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		std::unique_ptr<BYTE[]> buf(new BYTE[16]);
		//���ʕ�
		if (fs->Read(buf.get(), 1)<1 || buf[0]!=0x89)	UserAbort(USTR_IllegalFormat);
		if (!check_read(fs.get(), "PNG\r\n\x1a\n"))		UserAbort(USTR_IllegalFormat);
		//�`�����N���
		for (;;) {
			if (fs->Read(buf.get(), 4)<4) break;
			int size = big_to_little(buf.get());
			if (fs->Read(buf.get(), 4)!=4) break;
			buf[4] = 0;
			UnicodeString name = (char*)buf.get();
			if (USAME_TS(name, "IEND")) break;
			//�C���[�W�w�b�_
			if (USAME_TS(name, "IHDR")) {
				//�T�C�Y
				if (fs->Read(buf.get(), 4)<4) break;
				int iw = big_to_little(buf.get());
				if (fs->Read(buf.get(), 4)<4) break;
				int ih = big_to_little(buf.get());
				lst->Add(get_img_size_str(iw, ih));
				if (i_wd && i_hi) { *i_wd = iw; *i_hi = ih; }
				//�r�b�g�[�x
				if (fs->Read(buf.get(), 1)<1) break;
				add_PropLine(_T("�r�b�g�[�x"), (int)buf[0], lst);
				//�J���[�^�C�v
				if (fs->Read(buf.get(), 1)<1) break;
				UnicodeString coltyp = get_PropTitle(_T("�J���[�^�C�v"));
				switch (buf[0]) {
				case 0: coltyp.UCAT_T("�O���[�X�P�[��");			break;
				case 2: coltyp.UCAT_T("�g�D���[�J���[");			break;
				case 3: coltyp.UCAT_T("�C���f�b�N�X�J���[");		break;
				case 4: coltyp.UCAT_T("�O���[�X�P�[�� + �A���t�@");	break;
				case 6: coltyp.UCAT_T("�g�D���[�J���[ + �A���t�@");	break;
				}
				lst->Add(coltyp);
				fs->Seek(3, soFromCurrent);
			}
			//�e�L�X�g
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
			//�C���[�W�K���}
			else if (USAME_TS(name, "gAMA")) {
				if (fs->Read(buf.get(), 4)<4) break;
				lst->Add(get_PropTitle(_T("�C���[�W�K���}")).cat_sprintf(_T("%-7.6g"), big_to_little(buf.get())/100000.0));
			}
			//ICC�v���t�@�C��
			else if (USAME_TS(name, "iCCP")) {
				std::unique_ptr<BYTE[]> sbuf(new BYTE[size]);
				if (fs->Read(sbuf.get(), size)!=size) break;
				add_PropLine(_T("ICC�v���t�@�C��"), (char*)&sbuf[0], lst);
			}
			else fs->Seek(size, soFromCurrent);
			//CRC ���X�L�b�v
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
//PNG��񕶎���(�ȗ�)�̎擾
//---------------------------------------------------------------------------
UnicodeString get_PngInfStr(
	UnicodeString fnam,		//�t�@�C����
	unsigned int *i_wd,		//[o] ��	(default = NULL)
	unsigned int *i_hi)		//[o] ����	(default = NULL)
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
		if		(contains_s(lbuf, _T("�J���[�^�C�v: "))) col_s = Trim(get_tkn_r(lbuf, ':'));
		else if (contains_s(lbuf, _T("�r�b�g�[�x: ")))	 bit_s = "  �[�x:" + Trim(get_tkn_r(lbuf, ':'));
	}

	ret_str += (col_s + bit_s);
	return ret_str;
}

//---------------------------------------------------------------------------
//Gif�t�@�C���̏����擾
//---------------------------------------------------------------------------
bool get_GifInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList  *lst,		//[o] ��񃊃X�g	(NULL�ł���)
	unsigned int *i_wd,		//[o] ��			(default = NULL)
	unsigned int *i_hi,		//[o] ����			(default = NULL)
	int          *i_cnt)	//[o] �t���[����	(default = NULL)
{
	bool ret = false;
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		if (fs->Size<14) UserAbort(USTR_IllegalFormat);
		std::unique_ptr<BYTE[]> buf(new BYTE[16]);
		BYTE bt;
		//�o�[�W����
		fs->Read(buf.get(), 6);
		buf[6] = '\0';
		UnicodeString vstr = (char*)buf.get();
		if (!USTARTS_TS("GIF", vstr)) UserAbort(USTR_IllegalFormat);
		//�T�C�Y
		fs->Read(buf.get(), 4);
		unsigned int iw = (buf[1] << 8) + buf[0];
		unsigned int ih = (buf[3] << 8) + buf[2];
		if (i_wd && i_hi) { *i_wd = iw; *i_hi = ih; }
		//�o���b�g���
		fs->Read(&bt, 1);
		bool has_ct = (bt & 0x80);
		int ct_size = 2 << (bt & 0x07);
		if (lst) {
			add_PropLine(_T("�o�[�W����"), vstr, lst);
			lst->Add(get_img_size_str(iw, ih));	//"�摜�T�C�Y"
			lst->Add(get_PropTitle(_T("�J���[�e�[�u��")).cat_sprintf(_T("%s"), has_ct? _T("����") : _T("�Ȃ�")));
			add_PropLine(_T("�r�b�g�̐[��"), ((bt >> 4) & 0x07) + 1, lst);
			add_PropLine(_T("�F��"), ct_size, lst);
		}
		fs->Seek(2, soFromCurrent);
		if (has_ct) fs->Seek(ct_size * 3, soFromCurrent);

		//�u���b�N
		int img_cnt = 0;
		int t_cnt = 0;
		while (fs->Read(&bt, 1)==1) {
			BYTE blk_sz;
			//�g���u���b�N
			if (bt==0x21) {
				fs->Read(&bt, 1);
				if (bt==0xf9) {	//Graphic Control Extension
					if (fs->Read(&blk_sz, 1)!=1 || blk_sz!=4) UserAbort(USTR_IllegalFormat);
					if (fs->Read(buf.get(), 4)!=4)  UserAbort(USTR_IllegalFormat);
					t_cnt += ((buf[2] << 8) + buf[1]);
				}
				while (fs->Read(&blk_sz, 1)==1 && blk_sz>0) fs->Seek(blk_sz, soFromCurrent);
			}
			//�f�[�^�u���b�N
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
			if (img_cnt>1) add_PropLine(_T("�t���[����"), img_cnt, lst);
			if (t_cnt>0)   lst->Add(get_PropTitle(_T("����")).cat_sprintf(_T("%.2f �b"), t_cnt/100.0));
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
//WebP�t�@�C���̏����擾
//  �Q�l: https://developers.google.com/speed/webp/docs/riff_container
//---------------------------------------------------------------------------
bool get_WebpInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst,		//[o] ��񃊃X�g
	unsigned int *i_wd,		//[o] ��
	unsigned int *i_hi)		//[o] ����
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
				fmt.USET_T("���b�V�[");
				fs->Seek(6, soFromCurrent);
				if (iw==0 && fs->Read(&ui_buf, 4)==4) {
					iw = (ui_buf & 0x3fff);
					ih = ((ui_buf >> 16) & 0x3fff);
				}
			}
			else if (USAME_TI(chunk, "VP8L")) {
				fmt.USET_T("���X���X");
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
				lst->Add(get_PropTitle(_T("ICCP")).UCAT_T("�L��"));
			}
			else if (USAME_TI(chunk, "ALPH")) {
				has_alph = true;
				/* ���d�l�ƍ���Ȃ�?
				if (fs->Read(buf.get(), 4)!=4) UserAbort(USTR_IllegalFormat);
				tmp = ((buf[0]>>2) & 0x01)? "Level Reduction" : "None";
				add_PropLine(_T("���O����"), tmp, lst);

				switch ((buf[0]>>4) & 0x03) {
				case  1: tmp = "Horizontal"; break;
				case  2: tmp = "Vertical"; break;
				case  3: tmp = "Gradient"; break;
				default: tmp = "None";
				}
				add_PropLine(_T("���t�B���^"), tmp, lst);

				tmp = ((buf[0]>>6) & 0x01)? "Lossless" : "None";
				add_PropLine(_T("�����k"), tmp, lst);
				*/
			}
			else if (USAME_TI(chunk, "ANIM")) {
				fmt.USET_T("�A�j���[�V����");
				if (fs->Read(buf.get(), 4)==4) {
					lst->Add(get_PropTitle(_T("�w�i�F")).cat_sprintf(_T("R%u G%u B%u A%u"), buf[2], buf[1], buf[0], buf[3]));
					if (fs->Read(&us_buf, 2)==2) {
						tmp = get_PropTitle(_T("���[�v��"));
						if (us_buf==0)
							lst->Add(tmp.UCAT_T("����"));
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

		//�T�C�Y
		if (iw>0 && ih>0) {
			tmp = get_img_size_str(iw, ih);
			if (lst_top==lst->Count) lst->Add(tmp); else lst->Insert(lst_top, tmp);
			lst_top++;
			if (i_wd && i_hi) { *i_wd = iw; *i_hi = ih; }
		}
		//�`��
		if (!fmt.IsEmpty()) {
			tmp = make_PropLine(_T("�`��"), fmt);
			if (has_alph) tmp.UCAT_T(" + �A���t�@");
			if (lst_top==lst->Count) lst->Add(tmp); else lst->Insert(lst_top, tmp);
		}
		//�t���[����
		if (i_cnt>0) {
			add_PropLine(_T("�t���[����"), i_cnt, lst);
			lst->Add(get_PropTitle(_T("����")).cat_sprintf(_T("%.2f �b"), t_cnt/1000.0));
		}
		//���^�f�[�^
		add_PropLine_if(_T("���^�f�[�^"), meta, lst);
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
//WebP��񕶎���(�ȗ�)�̎擾
//---------------------------------------------------------------------------
UnicodeString get_WebpInfStr(
	UnicodeString fnam,		//�t�@�C����
	unsigned int *i_wd,		//[o] ��
	unsigned int *i_hi)		//[o] ����
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
		if (contains_s(lbuf, _T("�`��: "))) {
			ret_str += Trim(get_tkn_r(lbuf, ':'));
			break;
		}
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//�t�@�C����CRC32��������擾
//---------------------------------------------------------------------------
UnicodeString get_CRC32_str(UnicodeString fnam)
{
	static uint32_t table[256];
	static bool has_tbl = false;
 
	//�e�[�u���̏�����
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
//�t�@�C���̃n�b�V����������擾
//---------------------------------------------------------------------------
UnicodeString get_HashStr(
	UnicodeString fnam,		//�t�@�C����
	ALG_ID algid)			//�A���S���Y�� CALG_MD5/ CALG_SHA1/ CALG_SHA_256/384/512
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
//�A�v���P�[�V���������擾
//---------------------------------------------------------------------------
void get_AppInf(
	UnicodeString fnam,	//�t�@�C����
	TStringList *lst,	//[o] ��񃊃X�g
	bool check)			//�w�b�_���`�F�b�N�A�}�V�����擾 (default = true)
{
	//�Ή��}�V��
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
			add_PropLine(_T("�}�V��"), vstr, lst);
		}
		catch (...) {
			return;
		}
	}

	//�A�v���P�[�V�������
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
				"FileDescription=����\n"
				"FileVersion=�t�@�C��Ver.\n"
				"ProductName=���i��\n"
				"ProductVersion=���iVer.\n"
				"LegalCopyright=���쌠\n"
				"CompanyName=��Ж�\n"
				"LegalTrademarks=���W\n"
				"Comments=�R�����g\n"
				"OriginalFilename=���̃t�@�C����\n"
				"InternalName=������\n";

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
					//�����񂪂Ȃ�
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

			//�擾�ł��Ȃ���΃v���p�e�B����
			if (l_cnt==0) usr_SH->get_PropInf(fnam, lst);
		}
	}
	::HeapFree(::GetProcessHeap(), 0, lpBuffer);
}

//---------------------------------------------------------------------------
//Borland �v���W�F�N�g�����擾
//---------------------------------------------------------------------------
void get_BorlandInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst)		//[o] ��񃊃X�g
{
	if (!file_exists(fnam)) return;

	UnicodeString fext = get_extension(fnam);

	//�v���W�F�N�g
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
	//�\�[�X
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
//tags �t�@�C�������擾
//---------------------------------------------------------------------------
void get_TagsInf(
	UnicodeString fnam,		//�t�@�C����
	TStringList *lst)		//[o] ��񃊃X�g
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
//DLL�̃G�N�X�|�[�g�֐��ꗗ���擾
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
//���X�g�ɃG���[���b�Z�[�W("�G���[: ���b�Z�[�W") ��ǉ�
//---------------------------------------------------------------------------
void add_list_errmsg(TStringList *lst, UnicodeString msg)
{
	add_PropLine_if(_T("�G���["), msg, lst);
}

//---------------------------------------------------------------------------
//�P�x���擾(0�`1.0)
//---------------------------------------------------------------------------
double GetLuminance(TColor col)
{
	int c = ColorToRGB(col);
	return ((GetRValue(c)*0.3 + GetGValue(c)*0.59 + GetBValue(c)*0.11) / 255.0);
}

//---------------------------------------------------------------------------
//�J���[�ɔ{����������
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
//�P�x�Ɋ�Â��ăO���[�X�P�[����
//---------------------------------------------------------------------------
TColor GrayCol(TColor col)
{
	if (col==clNone) col = clBlack;
	BYTE v = (BYTE)(GetLuminance(col) * 255);
	return TColor(RGB(v, v, v));
}

//---------------------------------------------------------------------------
//RGB��HSL �ϊ�
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
//RGB��HSV �ϊ�
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
//�w�i�̋P�x���甒�܂��͍���I���E����
//---------------------------------------------------------------------------
TColor SelectWorB(
	TColor col,		//�w�i�F
	float  rt)		//�����w��  (default = 1.0)
{
	double v = GetLuminance(col);
	if (rt<1.0)
		return (v>0.5)? RatioCol(clWhite, 1.0 - rt) : RatioCol(clWhite, rt);
	else
		return (v>0.5)? clBlack : clWhite;
}
//---------------------------------------------------------------------------
//�P�x�Ɋ�Â��Ė��Â�����
//---------------------------------------------------------------------------
TColor AdjustColor(
	TColor col,		//���̐F
	int adj)		//�����l	0�`255
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
//�����񂩂�J���[��ݒ�
//---------------------------------------------------------------------------
void str_to_Color(TColor &col, UnicodeString s)
{
	TColor c = (TColor)s.ToIntDef((int)clNone);
	if (c!=clNone) col = c;
}

//---------------------------------------------------------------------------
//�n�C���C�g�F�̐ݒ�
//---------------------------------------------------------------------------
void SetHighlight(TCanvas *cv, bool hl)
{
	cv->Brush->Color = hl? scl_Highlight : scl_Window;
	cv->Font->Color  = hl? scl_HighlightText : scl_WindowText;
}

//---------------------------------------------------------------------------
//�O���b�h�̃Z�������w�b�_�ɍ��킹��
//---------------------------------------------------------------------------
void set_GridFromHeader(THeaderControl *hp, TStringGrid *gp)
{
	int cnt = std::min(hp->Sections->Count, gp->ColCount);
	int wd = 0;
	for (int i=0; i<cnt; i++) {
		gp->ColWidths[i] = hp->Sections->Items[i]->Width - gp->GridLineWidth;
		wd += hp->Sections->Items[i]->Width;
	}

	//�]��̃Z�N�V�����͎c�蕝�ɍ��킹��
	if (hp->Sections->Count>gp->ColCount)
		hp->Sections->Items[gp->ColCount]->Width = std::max(hp->Width - wd, 0);
}
//---------------------------------------------------------------------------
//�w�b�_�̗񕝂��O���b�h�̃Z�����ɍ��킹��
//---------------------------------------------------------------------------
void set_HeaderFromGrid(TStringGrid *gp, THeaderControl *hp)
{
	int cnt = std::min(hp->Sections->Count, gp->ColCount);
	int wd = 0;
	for (int i=0; i<cnt; i++) {
		hp->Sections->Items[i]->Width = gp->ColWidths[i] + gp->GridLineWidth + ((i==0)? 1 : 0);
		wd += hp->Sections->Items[i]->Width;
	}

	//�]��̃Z�N�V�����͎c�蕝�ɍ��킹��
	if (hp->Sections->Count>gp->ColCount)
		hp->Sections->Items[gp->ColCount]->Width = std::max(hp->Width - wd, 0);
}

//---------------------------------------------------------------------------
//�w�b�_�̗񕝂�ݒ�(�Œ�)
//---------------------------------------------------------------------------
void set_HeaderSecWidth(THeaderControl *hp, int cnt, ...)
{
	THeaderSections *sp = hp->Sections;

	//��U�A�Œ������
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

	//�����񂪖��w��Ȃ�c�蕝�ɐݒ�
	if (cnt == sp->Count-1) {
		int w = hp->ClientWidth;
		for (int i=0; i<sp->Count-1; i++) w -= sp->Items[i]->Width;
		sp->Items[sp->Count - 1]->Width = std::max(w, 0);
	}

	//�񕝂��Œ�
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
//�X�e�[�^�X�o�[�̃p�l�����𕶎�������ݒ�
//---------------------------------------------------------------------------
int set_SttBarPanelWidth(TStatusBar *sp, int idx,
	int n)	//������(���p�P��)
{
	TCanvas *cv = sp->Canvas;
	cv->Font->Assign(sp->Font);
	int wd = (n>0)? get_CharWidth(cv, n, 16) : 0;
	sp->Panels->Items[idx]->Width = wd;
	return wd;
}

//---------------------------------------------------------------------------
//�O���b�h�̍s���N���A
//---------------------------------------------------------------------------
void clear_GridRow(TStringGrid *gp, int row, int col)
{
	if (row>=0 && row<gp->RowCount) {
		for (int i=col; i<gp->ColCount; i++) gp->Cells[i][row] = EmptyStr;
	}
}

//---------------------------------------------------------------------------
//�O���b�h�̌��݈ʒu���C���f�b�N�X��
//---------------------------------------------------------------------------
int get_GridIndex(TStringGrid *gp, int max_count)
{
	int idx = (gp->ColCount * gp->Row) + gp->Col;
	if (idx>=max_count) idx = -1;
	return idx;
}
//---------------------------------------------------------------------------
//�C���f�b�N�X�ɂ���ăO���b�h�ʒu��ݒ�
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
//ListBox �̃J�[�\���ړ�
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
		//���̈�O��������X�N���[��
		int pn = lp->ClientHeight/lp->ItemHeight;
		if (lp->ItemIndex<lp->TopIndex)
			lp->TopIndex = std::max(lp->ItemIndex - LISTBOX_SCRMGN, 0);
		else if (lp->ItemIndex > lp->TopIndex + pn - 1)
			lp->TopIndex = std::max(lp->ItemIndex - pn + 1 + LISTBOX_SCRMGN, 0);
	}
	else {
		if (idx>=lp->Count) idx = lp->Count - 1;
		//���
		if (idx<lp->ItemIndex) {
			if (idx < lp->TopIndex+LISTBOX_SCRMGN)
				lp->TopIndex = std::max(idx - LISTBOX_SCRMGN, 0);
		}
		//����
		else {
			int pn = lp->ClientHeight/lp->ItemHeight;
			if (idx > (lp->TopIndex+pn - 1 - LISTBOX_SCRMGN))
				lp->TopIndex = std::max(idx - pn + 1 + LISTBOX_SCRMGN, 0);
		}
		lp->ItemIndex = idx;
	}
}
//---------------------------------------------------------------------------
//ListBox �̃J�[�\�������̈��
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
//ListBox �̃X�N���[��
//---------------------------------------------------------------------------
void ListBoxScrollDown(TListBox *lp, int n,
	bool move_csr)	//�J�[�\�����ړ� (default = false)
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
	bool move_csr)	//�J�[�\�����ړ� (default = false)
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
//ListBox �ł��ׂĂ̍��ڂ�I��
//---------------------------------------------------------------------------
void ListBoxSelectAll(TListBox *lp, bool sw)
{
	if (lp) lp->Perform(LB_SETSEL, (BOOL)sw, -1);
}

//---------------------------------------------------------------------------
//ListBox �œ������T�[�`
//---------------------------------------------------------------------------
int ListBoxInitialSearch(
	TListBox *lp,
	UnicodeString kptn,		//�����p�^�[��
	bool fnam_sw)			//�t�@�C���������ɑ΂��ēK�p (default = false)
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
//ListBox �̌��ݍs����URL���擾
//---------------------------------------------------------------------------
UnicodeString ListBoxGetURL(TListBox *lp)
{
	return (lp->ItemIndex!=-1)? extract_URL(lp->Items->Strings[lp->ItemIndex]) : EmptyStr;
}


//---------------------------------------------------------------------------
//ListBox �� Objects �𐮐��l�Ƃ��ă`�F�b�N���A���ژA�Ԃ�ݒ�
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
//TStringGrid �ł̃J�[�\���ړ�
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
//TStringList �̍��ڂ��P��ֈړ�
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
//TStringList �̍��ڂ��P���ֈړ�
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
//�R���g���[���ŃL�[������
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
//�A�N�Z�����[�^�L�[�̈�v���郊�X�g���ڂ̃C���f�b�N�X���擾
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
//�L�[�E�}�E�X�o�b�t�@���N���A
//---------------------------------------------------------------------------
void ClearKeyBuff(
	bool key_only)	//true = �L�[�o�b�t�@�̂݃N���A (default = false)
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
//�����񂩂�URL(���[�J���t�@�C�����܂�)�𒊏o
//---------------------------------------------------------------------------
UnicodeString extract_URL(UnicodeString s)
{
	if (s.Pos(':')==0) return EmptyStr;

	UnicodeString url;
	TMatch mt = TRegEx::Match(s, URL_MATCH_PTN);
	if (mt.Success) {
		url = mt.Value;
		if (!USTARTS_TS('h', url)) url.Insert("h", 1);	//�擪�� h ���Ȃ�������₤
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
//URL���G���R�[�h
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
//�C���^�[�l�b�g�ڑ������邩?
//---------------------------------------------------------------------------
bool InternetConnected()
{
	DWORD flag;
	return InternetGetConnectedState(&flag, 0);
}

//---------------------------------------------------------------------------
//�I�����C����̃t�@�C�����擾
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
//�X�v���b�^����ݒ�
//---------------------------------------------------------------------------
void set_SplitterWidht(TSplitter *sp, int sp_wd)
{
	if (sp->Align==alLeft || sp->Align==alRight) sp->Width = sp_wd; else sp->Height = sp_wd;
	sp->Repaint();
}

//---------------------------------------------------------------------------
//�p�l���̔z�u
//---------------------------------------------------------------------------
void set_PanelAlign(TPanel *pp, TSplitter *sp, int mode, int sp_wd)
{
	switch (mode) {
	case 1:	//�E
		sp->Align = alLeft;
		pp->Align = alRight;
		sp->Align = alRight;
		sp->Width = pp->Visible? sp_wd : 0;
		break;
	case 2:	//��
		sp->Align = alRight;
		pp->Align = alLeft;
		sp->Align = alLeft;
		sp->Width = pp->Visible? sp_wd : 0;
		break;
	case 3: //��
		sp->Align  = alBottom;
		pp->Align  = alTop;
		sp->Align  = alTop;
		sp->Height = pp->Visible? sp_wd : 0;
		break;
	default:	//0: ��
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
//�R���g���[�����E�����ɔz�u
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
//�o�b�e���c��(%)�̕�����擾
//---------------------------------------------------------------------------
UnicodeString get_BatteryPercentStr()
{
	SYSTEM_POWER_STATUS ps;
	::GetSystemPowerStatus(&ps);
	if (ps.BatteryLifePercent==255) return "---";
	return UnicodeString().sprintf(_T("%3u"), ps.BatteryLifePercent);
}
//---------------------------------------------------------------------------
//�o�b�e���c��(����)�̕�����擾
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
//�C���^�[�l�b�g�V���[�g�J�b�g�̍쐬
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
//�h���C�u�̎��O��
//  �߂�l: ���O����/���O���\ �̏ꍇ true
//---------------------------------------------------------------------------
bool EjectDrive(
	UnicodeString drvnam,	//�h���C�u��
	bool eject)				//true=���O��/ false=�m�F�̂�
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
				//ID=37: �u���o���v�𒼐ڎ��s
				ret = usr_SH->DriveContextMenu(Application->ActiveFormHandle, drvnam, NULL, 37).IsEmpty();
			}
			else {
				//�{�����[�������b�N
				bool locked = false;
				DWORD dwBytesReturned;
				for (int i = 0; i<20; i++) {
					if (::DeviceIoControl(hDrive, FSCTL_LOCK_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL)) {
						locked = true; break;
					}
					Sleep(10000/20);	//10�b/20�񃊃g���C
				}
				if (locked) {
					//�}�E���g����
					if (::DeviceIoControl(hDrive, FSCTL_DISMOUNT_VOLUME, NULL, 0, NULL, 0, &dwBytesReturned, NULL)) {
						//�폜��W���邩��f�o�C�X���~
						PREVENT_MEDIA_REMOVAL PMRBuffer;
						PMRBuffer.PreventMediaRemoval = FALSE;
						if (::DeviceIoControl(hDrive, IOCTL_STORAGE_MEDIA_REMOVAL,
							&PMRBuffer, sizeof(PREVENT_MEDIA_REMOVAL), NULL, 0, &dwBytesReturned, NULL))
						{
							//���O��
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
//�����[�o�u��/CD-ROM �ȊO�̎��O��
//---------------------------------------------------------------------------
bool EjectDrive2(UnicodeString drvnam, bool eject)
{
	drvnam = ExtractFileDrive(drvnam);
	if (drvnam.IsEmpty()) return false;
	drvnam = IncludeTrailingPathDelimiter(drvnam);

	//�h���C�u�̃{�����[�������擾
	_TCHAR pszVolName[MAX_PATH];
	if (!::GetVolumeNameForVolumeMountPoint(drvnam.c_str(), pszVolName, MAX_PATH)) return false;
	UnicodeString vol_name = pszVolName;

	//�{�����[�����̈�v����f�o�C�X������
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
							//���O��
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
//�R���g���[���̃N���X�`�F�b�N
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
//���j���[�̗]�v�ȃZ�p���[�^���\���ɂ���
//���ŏ��̕\���� AutoLineReduction �������Ȃ����ɑΏ�
//---------------------------------------------------------------------------
void reduction_MenuLine(TMenuItem *mp)
{
	if (!mp) return;

	bool is_sp = false;
	TMenuItem *ip_0 = NULL;		//�ŏ��̕\������
	TMenuItem *ip_1 = NULL;		//�Ō�̕\������

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
//���b�Z�[�W������̎擾
//---------------------------------------------------------------------------
UnicodeString LoadUsrMsg(int id, UnicodeString s)
{
	std::unique_ptr<TStringList> msg_lst(new TStringList());
	msg_lst->Text =
		"1001=%s��������܂���B\n"							//USTR_NotFound
		"1002=%s���̑���ɂ͑Ή����Ă��܂���B\n"				//USTR_OpeNotSuported
		"1003=���̑���͂ł��܂���B\n"							//USTR_CantOperate
		"1004=�Ή����Ă��Ȃ��`���ł��B\n"						//USTR_FmtNotSuported
		"1005=�ꎞ�𓀂Ɏ��s���܂����B\n"						//USTR_FaildTmpUnpack
		"1006=�ړ��悪�����ł��B\n"								//USTR_SameMoveDest
		"1007=�R�s�[�悪�����ł��B\n"							//USTR_SameCopyDest
		"1008=�f�B���N�g�����܂܂�Ă��܂��B\n"					//USTR_IncludeDir
		"1009=�s���ȓ��t�܂��͎����ł��B\n"						//USTR_IllegalDate
		"1010=�s���Ȏw��ł��B\n"								//USTR_IllegalParam
		"1011=�s���Ȍ`���ł��B\n"								//USTR_IllegalFormat
		"1012=�s���ȃA�h���X�ł�\n"								//USTR_IllegalAddress
		"1013=�N���Ɏ��s���܂����B\n"							//USTR_FaildExec
		"1014=�����Ɏ��s���܂����B\n"							//USTR_FaildProc
		"1015=�ǂݍ��݂Ɏ��s���܂����B\n"						//USTR_FaildLoad
		"1016=%s�̕ۑ��Ɏ��s���܂����B\n"						//USTR_FaildSave
		"1017=�폜�Ɏ��s���܂����B\n"							//USTR_FaildDelete
		"1018=�R�s�[�Ɏ��s���܂����B\n"							//USTR_FaildCopy
		"1019=���j���[�����s�ł��܂���B\n"						//USTR_FaildMenu
		"1020=���f���܂����B\n"									//USTR_Canceled
		"1021=���[�N���X�g���J���܂���B\n"						//USTR_WlistCantOpen
		"1022=�������ɂ��̑���͂ł��܂���B\n"					//USTR_ProcBusy
		"1023=�Ώۂ�����܂���B\n"								//USTR_NoObject
		"1024=���K�\���Ɍ�肪����܂��B\n"						//USTR_IllegalRegEx
		"1025=����������̓���\n"								//USTR_InputFmtStr
		"1026=���łɓo�^����Ă��܂��B\n"						//USTR_Registered
		"1027=�@������...�^�@ESC�L�[�Œ��f���܂��B\n"			//USTR_SearchingESC
		"1028=�@�v�Z��...�^�@ESC�L�[�Œ��f���܂��B\n"			//USTR_CalculatingESC
		"1029=�@������...�^�@���΂炭���������������B\n"		//USTR_WaitForReady
		"1030=���X�|���X�t�@�C���̍쐬�Ɏ��s���܂����B\n"		//USTR_FaildListFile
		"1031=�ꎞ�f�B���N�g�����쐬�ł��܂���B\n"				//USTR_CantMakeTmpDir
		"1032=�s���ȍ\���ł��B\n"								//USTR_SyntaxError
		"1033=�Ή����鐧�䕶��������܂���B\n"				//USTR_BadStatmet
		"1034=�p�����[�^���w�肳��Ă��܂���B\n"				//USTR_NoParameter
		"1035=�A�v���P�[�V������������܂���B\n"				//USTR_AppNotFound
		"1036=�摜�̏������ł��Ă��܂���\n"						//USTR_ImgNotReady
		"1037=�t�@�C�����J���܂���\n"							//USTR_FileNotOpen
		"1038=�f�B���N�g����������܂���B\n"					//USTR_DirNotFound
		"1039=�����ȃR�}���h�ł��B\n"							//USTR_InvalidCmd
		"1040=. �ŋ�؂��ĕ����w��\\n"						//USTR_HintMltFExt
		"1041=; �ŋ�؂��ĕ����w��\\n"						//USTR_HintMltSepSC
		"1042=�Ή�����G�f�B�^������܂���\n"					//USTR_NoEditor
		"1043=�Đ��ł��܂���B\n"								//USTR_CantPlay
		"1044=%s�ɖ��O��t���ĕۑ�\n"							//USTR_SaveAs
		"1045=�f�B���N�g��[%s]���쐬�ł��܂���B\n"				//USTR_CantCreDir
		"1046=%s���R�s�[���܂���?\n"							//USTR_CopyQ
		"1047=%s���폜���܂���?\n"								//USTR_DeleteQ
		"1048=%s�����S�폜���܂���?\n"							//USTR_CompDeleteQ
		"1049=%s�̗��������ׂč폜���܂���?\n"					//USTR_DelHistoryQ
		"1050=���ׂĂ̏ꏊ�̂��ׂẴ}�[�N���������܂���?\n"	//USTR_ClrAllMarkQ
		"1051=�R�}���h�𒆒f���܂���?\n"						//USTR_CancelCmdQ
		"1052=�A�[�J�C�u���J���܂���B\n"						//USTR_ArcNotOpen
		"1053=�e�L�X�g�t�@�C���ł͂���܂���B\n"				//USTR_NotText
		"1054=�f�B���N�g���̍쐬\n"								//USTR_CreateDir
		"1055=%s�𒊏o���܂���?\n"								//USTR_Extract
		"1056=%s��񂪎擾�ł��܂���B\n"						//USTR_CantGetInfo
		"1057=���O���d�����Ă��܂��B\n"							//USTR_DuplicateName
		"1058=�㏑�����Ă��悢�ł���?\n"						//USTR_OverwriteQ
		"1059=%s���A�b�v���[�h���܂���?\n"						//USTR_UploadQ
		"1060=%s���_�E�����[�h���܂���?\n"						//USTR_DownloadQ
		"1061=�폜�J�n  %s\n"									//USTR_BeginDelete
		"1062=�����t�@�C���̏��� <%s>\n"						//USTR_SameName
		"1063=���O�̕ύX\n"										//USTR_Rename
		"1064=�f�B���N�g�������̈قȂ铯���Ώۂ����݂��܂��B\n"	//USTR_NgSameName
		"1065=�s���ȓ��t�����ł��B\n"							//USTR_IllegalDtCond
		"1066=�I���}�X�N���̃��[�N���X�g�͕ύX�ł��܂���B\n"	//USTR_WorkFiltered

		"1100=�I������\n"										//USTR_SelectedItem
		"1101=�ꗗ\n"											//USTR_List
		"1102=�A�b�v���[�h\n"									//USTR_Upload
		"1103=�_�E�����[�h\n";									//USTR_Download

	UnicodeString ret_str = REPLACE_TS(msg_lst->Values[IntToStr(id)], "�^", "\r\n");
	ret_str = s.IsEmpty()? replace_regex(ret_str, _T("%s[�̂�����]?"), null_TCHAR) : ReplaceStr(ret_str, "%s", s);
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
