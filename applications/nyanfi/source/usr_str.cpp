//----------------------------------------------------------------------//
// �����񑀍�															//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <algorithm>
#include <memory>
#include <System.StrUtils.hpp>
#include <System.Masks.hpp>
#include <RegularExpressions.hpp>
#include "usr_str.h"

//---------------------------------------------------------------------------
const _TCHAR *null_TCHAR = _T("");

UnicodeString	KeyStr_Shift = "Shift+";
UnicodeString	KeyStr_Ctrl  = "Ctrl+";
UnicodeString	KeyStr_Alt	 = "Alt+";

//---------------------------------------------------------------------------
//���R���\�[�g�ɑΉ�������r
//---------------------------------------------------------------------------
int __fastcall comp_NaturalOrder(TStringList *List, int Index1, int Index2)
{
	return StrCmpLogicalW(List->Strings[Index1].c_str(), List->Strings[Index2].c_str());
}

//---------------------------------------------------------------------------
//���R���� ������ (�J���}�Ή�/ �����ꍇ�� Objects �Ŕ�r)
//---------------------------------------------------------------------------
int __fastcall comp_AscendOrder(TStringList *List, int Index1, int Index2)
{
	UnicodeString s1  = List->Strings[Index1];
	UnicodeString s2  = List->Strings[Index2];
	UnicodeString ns1 = extract_top_int_str(s1);
	UnicodeString ns2 = extract_top_int_str(s2);

	int res = (!ns1.IsEmpty() && !ns2.IsEmpty())? StrCmpLogicalW(ns1.c_str(), ns2.c_str()) : 0;
	if (res==0) res = StrCmpLogicalW(s1.c_str(), s2.c_str());
	if (res==0) res = (int)List->Objects[Index1] - (int)List->Objects[Index2];
	return res;
}
//---------------------------------------------------------------------------
//���R�~�� ������ (�J���}�Ή�/ �����ꍇ�� Objects �Ŕ�r)
//---------------------------------------------------------------------------
int __fastcall comp_DescendOrder(TStringList *List, int Index1, int Index2)
{
	UnicodeString s1  = List->Strings[Index1];
	UnicodeString s2  = List->Strings[Index2];
	UnicodeString ns1 = extract_top_int_str(s1);
	UnicodeString ns2 = extract_top_int_str(s2);

	int res = (!ns1.IsEmpty() && !ns2.IsEmpty())? StrCmpLogicalW(ns2.c_str(), ns1.c_str()) : 0;
	if (res==0) res = StrCmpLogicalW(s2.c_str(), s1.c_str());
	if (res==0) res = (int)List->Objects[Index2] - (int)List->Objects[Index1];
	return res;
}
//---------------------------------------------------------------------------
//���� Objects
//---------------------------------------------------------------------------
int __fastcall comp_ObjectsOrder(TStringList *List, int Index1, int Index2)
{
	return (int)List->Objects[Index1] - (int)List->Objects[Index2];
}

//---------------------------------------------------------------------------
//CSV�p��r�֐�
//---------------------------------------------------------------------------
int  USR_CsvCol 	 = 0;		//�\�[�g�Ώۗ�
int  USR_CsvSortMode = 1;		//�\�[�g���[�h 1:����/ -1�~��
bool USR_CsvTopIsHdr = false;	//�擪�s�͍��ږ�

//---------------------------------------------------------------------------
//CSV ���R���� (�����ꍇ�� Objects �Ŕ�r)
//---------------------------------------------------------------------------
int __fastcall comp_CsvNaturalOrder(TStringList *List, int Index1, int Index2)
{
	if (USR_CsvTopIsHdr) {	//�擪�s�͍��ږ�
		if ((int)List->Objects[Index1]==0) return -1;
		if ((int)List->Objects[Index2]==0) return  1;
	}

	UnicodeString s1  = get_csv_item(List->Strings[Index1], USR_CsvCol);
	UnicodeString s2  = get_csv_item(List->Strings[Index2], USR_CsvCol);
	UnicodeString ns1 = extract_top_int_str(s1);
	UnicodeString ns2 = extract_top_int_str(s2);

	int res = (!ns1.IsEmpty() && !ns2.IsEmpty())? StrCmpLogicalW(ns1.c_str(), ns2.c_str()) : 0;
	if (res==0) res = StrCmpLogicalW(s1.c_str(), s2.c_str());
	if (res==0) res = (int)List->Objects[Index1] - (int)List->Objects[Index2];
	res *= USR_CsvSortMode;
	return res;
}
//---------------------------------------------------------------------------
//TSV ���R���� (�����ꍇ�� Objects �Ŕ�r)
//---------------------------------------------------------------------------
int __fastcall comp_TsvNaturalOrder(TStringList *List, int Index1, int Index2)
{
	if (USR_CsvTopIsHdr) {	//�擪�s�͍��ږ�
		if ((int)List->Objects[Index1]==0) return -1;
		if ((int)List->Objects[Index2]==0) return  1;
	}

	TStringDynArray itm1 = SplitString(List->Strings[Index1], "\t");
	TStringDynArray itm2 = SplitString(List->Strings[Index2], "\t");
	UnicodeString s1  = (USR_CsvCol<itm1.Length)? itm1[USR_CsvCol] : EmptyStr;
	UnicodeString s2  = (USR_CsvCol<itm1.Length)? itm2[USR_CsvCol] : EmptyStr;
	UnicodeString ns1 = extract_top_int_str(s1);
	UnicodeString ns2 = extract_top_int_str(s2);

	int res = (!ns1.IsEmpty() && !ns2.IsEmpty())? StrCmpLogicalW(ns1.c_str(), ns2.c_str()) : 0;
	if (res==0) res = StrCmpLogicalW(s1.c_str(), s2.c_str());
	if (res==0) res = (int)List->Objects[Index1] - (int)List->Objects[Index2];
	res *= USR_CsvSortMode;
	return res;
}

//---------------------------------------------------------------------------
//�w��Z�p���[�^�̑O�̕�������擾
//  �Z�p���[�^��������Ȃ��ꍇ�́A����������̂܂ܕԂ�
//---------------------------------------------------------------------------
UnicodeString get_tkn(UnicodeString s, UnicodeString sp)
{
	int p = s.Pos(sp);
	return (p==0)? s : s.SubString(1, p - 1);
}
//---------------------------------------------------------------------------
UnicodeString get_tkn(UnicodeString s, const _TCHAR *sp)
{
	int p = s.Pos(sp);
	return (p==0)? s : s.SubString(1, p - 1);
}
//---------------------------------------------------------------------------
UnicodeString get_tkn(UnicodeString s, WideChar sp)
{
	int p = s.Pos(sp);
	return (p==0)? s : s.SubString(1, p - 1);
}

//---------------------------------------------------------------------------
//�w��Z�p���[�^�̌�̕�������擾
//  �Z�p���[�^��������Ȃ��ꍇ�́AEmptyStr ��Ԃ�
//---------------------------------------------------------------------------
UnicodeString get_tkn_r(UnicodeString s, UnicodeString sp)
{
	int p = s.Pos(sp);
	return (p==0)? EmptyStr : s.SubString(p + sp.Length(),  s.Length() - (p - sp.Length() + 1));
}
//---------------------------------------------------------------------------
UnicodeString get_tkn_r(UnicodeString s, const _TCHAR *sp)
{
	int p = s.Pos(sp);
	return (p==0)? EmptyStr : s.SubString(p + _tcslen(sp),  s.Length() - (p - _tcslen(sp) + 1));
}
//---------------------------------------------------------------------------
UnicodeString get_tkn_r(UnicodeString s, WideChar sp)
{
	int p = s.Pos(sp);
	return (p==0)? EmptyStr : s.SubString(p + 1,  s.Length() - p);
}

//---------------------------------------------------------------------------
//�w��Z�p���[�^�Ԃ̕�������擾
//  ��Z�p���[�^��������Ȃ��ꍇ�́A�O�Z�p���[�^�ȍ~�̕������Ԃ�
//---------------------------------------------------------------------------
UnicodeString get_tkn_m(UnicodeString s, const _TCHAR *sp1, const _TCHAR *sp2)
{
	return get_tkn(get_tkn_r(s, sp1), sp2);
}
//---------------------------------------------------------------------------
UnicodeString get_tkn_m(UnicodeString s, WideChar sp1, WideChar sp2)
{
	return get_tkn(get_tkn_r(s, sp1), sp2);
}

//---------------------------------------------------------------------------
//�^�u�̑O�̕�������擾
//  �^�u���Ȃ��ꍇ�́A����������̂܂ܕԂ�
//---------------------------------------------------------------------------
UnicodeString get_pre_tab(UnicodeString s)
{
	return get_tkn(s, '\t');
}
//---------------------------------------------------------------------------
//�^�u�̌�̕�������擾
//  �^�u���Ȃ��ꍇ�́AEmptyStr ��Ԃ�
//---------------------------------------------------------------------------
UnicodeString get_post_tab(UnicodeString s)
{
	return get_tkn_r(s, '\t');
}

//---------------------------------------------------------------------------
//�����񒆂� (�`) �����擾
//---------------------------------------------------------------------------
UnicodeString get_in_paren(UnicodeString s)
{
	int p1 = s.Pos('(');
	int p2 = s.Pos(')');
	if (p1==0 || p2==0 || p1>=p2) return EmptyStr;
	return s.SubString(p1 + 1,  p2 - p1 - 1);
}
//---------------------------------------------------------------------------
//�����񒆂� (�`) �����擾�A��������� (�`) �̌㑱�������
//---------------------------------------------------------------------------
UnicodeString split_in_paren(UnicodeString &s)
{
	return get_tkn_r(split_tkn(s, ')'), '(');
}

//---------------------------------------------------------------------------
//�O��Ƀ^�u���܂܂Ȃ���������擾
//  �����s�̏ꍇ�A��łȂ��ŏ��̕�����
//---------------------------------------------------------------------------
UnicodeString get_norm_str(UnicodeString s)
{
	UnicodeString ret_str;
	std::unique_ptr<TStringList> lst(new TStringList());
	lst->Text = s;
	for (int i=0; i<lst->Count; i++) {
		ret_str = lst->Strings[i];
		ret_str = replace_regex(ret_str, _T("^\\t+"), null_TCHAR);
		ret_str = replace_regex(ret_str, _T("\\t+$"), null_TCHAR);
		if (!ret_str.IsEmpty()) break;
	}
	return ret_str;
}

//---------------------------------------------------------------------------
//�w��Z�p���[�^�O�̕�����𕪗����Ď擾
//  �Z�p���[�^��������Ȃ��ꍇ�́A����������̂܂ܕԂ��A��������͋��
//---------------------------------------------------------------------------
UnicodeString split_tkn(UnicodeString &s, UnicodeString sp)
{
	UnicodeString r;
	int p = s.Pos(sp);
	if (p==0) {
		r = s;
		s = EmptyStr;
	}
	else {
		r = s.SubString(1, p - 1);
		s = s.SubString(p + sp.Length(), s.Length() - (p - sp.Length() + 1));
	}
	return r;
}
//---------------------------------------------------------------------------
UnicodeString split_tkn(UnicodeString &s, const _TCHAR *sp)
{
	return split_tkn(s, UnicodeString(sp));
}
//---------------------------------------------------------------------------
UnicodeString split_tkn(UnicodeString &s, WideChar sp)
{
	UnicodeString r;
	int p = s.Pos(sp);
	if (p==0) {
		r = s;
		s = EmptyStr;
	}
	else {
		r = s.SubString(1, p - 1);
		s = s.SubString(p + 1, s.Length() - p);
	}
	return r;
}

//---------------------------------------------------------------------------
//�^�u�O�̕�����𕪗����Ď擾
//  �^�u���Ȃ��ꍇ�́A����������̂܂ܕԂ��A��������͋��
//---------------------------------------------------------------------------
UnicodeString split_pre_tab(UnicodeString &s)
{
	return split_tkn(s, '\t');
}

//---------------------------------------------------------------------------
//�����񂩂��������(^:�`:)�𕪗����擾
//---------------------------------------------------------------------------
UnicodeString split_dsc(UnicodeString &s)
{
	UnicodeString dsc;
	if (!StartsStr("::{", s) && StartsStr(":", s)) {
		s.Delete(1, 1);
		dsc = split_tkn(s, ':');
		s = Trim(s);
	}
	return dsc;
}

//---------------------------------------------------------------------------
//�����񂩂�擪�����𕪗����擾
//---------------------------------------------------------------------------
UnicodeString split_top_ch(UnicodeString &s)
{
	UnicodeString c;
	if (!s.IsEmpty()) {
		c = s.SubString(1, 1);
		s.Delete(1, 1);
	}
	return c;
}
//---------------------------------------------------------------------------
WideChar split_top_wch(UnicodeString &s)
{
	WideChar c = '\0';
	if (!s.IsEmpty()) {
		c = s[1];
		s.Delete(1, 1);
	}
	return c;
}

//---------------------------------------------------------------------------
//�R�}���h���C�����p�����[�^�ɕ���
//---------------------------------------------------------------------------
int split_cmd_line(UnicodeString s, TStringList *lst)
{
	bool in_qut = false;
	UnicodeString lbuf;
	for (int i=1; i<=s.Length(); i++) {
		WideChar c = s[i];
		//���p���O
		if (!in_qut) {
			if (c=='\"')
				in_qut = true;
			else if (c==' ') {
				if (!lbuf.IsEmpty()) {
					lst->Add(lbuf); 
					lbuf = EmptyStr;
				}
				continue;
			}
		}
		//���p����
		else if (c=='\"') {
			in_qut = false;
		}

		lbuf.cat_sprintf(_T("%c"), c);
	}

	if (!lbuf.IsEmpty()) lst->Add(lbuf);

	return lst->Count;
}

//---------------------------------------------------------------------------
//�R�}���h���C������t�@�C�����𕪗�
//---------------------------------------------------------------------------
UnicodeString split_file_param(UnicodeString &s)
{
	UnicodeString ret_str;
	s = Trim(s);
	if (remove_top_s(s, '\"')) {
		ret_str = split_tkn(s, '\"');
		s = Trim(s);
	}
	else
		ret_str = split_tkn(s, ' ');
	return ret_str;
}

//---------------------------------------------------------------------------
//��������w���؂�ŕ���
//---------------------------------------------------------------------------
TStringDynArray split_strings_tab(UnicodeString s)
{
	return SplitString(s, "\t");
}
//---------------------------------------------------------------------------
TStringDynArray split_strings_semicolon(UnicodeString s)
{
	return SplitString(s, ";");
}

//---------------------------------------------------------------------------
//�����񂩂�w�����폜
//  �߂�l: ture=�폜����/ false=�ꂪ�܂܂�Ă��Ȃ�
//---------------------------------------------------------------------------
bool remove_text(UnicodeString &s, UnicodeString w)
{
	if (ContainsText(s, w)) {
		s = ReplaceText(s, w, EmptyStr);
		return true;
	}
	else
		return false;
}
//---------------------------------------------------------------------------
bool remove_text(UnicodeString &s, const _TCHAR *w)
{
	return remove_text(s, UnicodeString(w));
}

//---------------------------------------------------------------------------
//������̐擪�Ɏw��ꂪ��������폜 (�召�����𖳎�)
//  �߂�l: ture=�폜����/ false=�擪�Ɍꂪ�Ȃ�
//---------------------------------------------------------------------------
bool remove_top_text(UnicodeString &s, UnicodeString w)
{
	if (w.IsEmpty() || !StartsText(w, s)) return false;
	s.Delete(1, w.Length());
	return true;
}
//---------------------------------------------------------------------------
bool remove_top_text(UnicodeString &s, const _TCHAR *w)
{
	return remove_top_text(s, UnicodeString(w));
}

//---------------------------------------------------------------------------
//������̐擪�Ɏw��ꂪ��������폜 (�召���������)
//  �߂�l:  ture=�폜����/ false=�擪�Ɍꂪ�Ȃ�
//---------------------------------------------------------------------------
bool remove_top_s(UnicodeString &s, UnicodeString w)
{
	if (!StartsStr(w, s)) return false;
	s.Delete(1, w.Length());
	return true;
}
//---------------------------------------------------------------------------
bool remove_top_s(UnicodeString &s, const _TCHAR *w)
{
	if (!StartsStr(w, s)) return false;
	s.Delete(1, _tcslen(w));
	return true;
}
//---------------------------------------------------------------------------
bool remove_top_s(UnicodeString &s, WideChar w)
{
	if (!StartsStr(w, s)) return false;
	s.Delete(1, 1);
	return true;
}

//---------------------------------------------------------------------------
//������̐擪�� @ ����������폜
//  �߂�l: ture=�擪�� @ ���폜����
//---------------------------------------------------------------------------
bool remove_top_AT(UnicodeString &s)
{
	return remove_top_s(s, '@');
}
//---------------------------------------------------------------------------
//������̐擪�� $ ����������폜
//  �߂�l: ture=�擪�� $ ���폜����
//---------------------------------------------------------------------------
bool remove_top_Dollar(UnicodeString &s)
{
	return remove_top_s(s, '$');
}

//---------------------------------------------------------------------------
//������̖����Ɏw��ꂪ��������폜 (�召�����𖳎�)
//  �߂�l: ture=��������폜����
//---------------------------------------------------------------------------
bool remove_end_text(UnicodeString &s, UnicodeString w)
{
	if (!EndsText(w, s)) return false;
	s.Delete(s.Length() - w.Length() + 1, w.Length());
	return true;
}
//---------------------------------------------------------------------------
//������̖����Ɏw��ꂪ��������폜 (�召���������)
//  �߂�l: ture=��������폜����
//---------------------------------------------------------------------------
bool remove_end_s(UnicodeString &s, UnicodeString w)
{
	if (!EndsStr(w, s)) return false;
	s.Delete(s.Length() - w.Length() + 1, w.Length());
	return true;
}
//---------------------------------------------------------------------------
bool remove_end_s(UnicodeString &s, WideChar w)
{
	if (!EndsStr(w, s)) return false;
	s.Delete(s.Length(), 1);
	return true;
}

//---------------------------------------------------------------------------
void delete_end(UnicodeString &s)
{
	s.Delete(s.Length(), 1);
}

//---------------------------------------------------------------------------
//�擪�����O������������擾
//---------------------------------------------------------------------------
UnicodeString exclude_top(UnicodeString s)
{
	return s.SubString(2, s.Length() - 1);
}
//---------------------------------------------------------------------------
//�擪�Ɩ��������O������������擾
//---------------------------------------------------------------------------
UnicodeString exclude_top_end(UnicodeString s)
{
	return s.SubString(2, s.Length() - 2);
}

//---------------------------------------------------------------------------
//�S�p�X�y�[�X���l�������g���~���O
//---------------------------------------------------------------------------
UnicodeString trim_ex(UnicodeString s)
{
	//�擪
	for (;;) {
		if (remove_top_s(s, "�@")) continue;
		if (remove_top_s(s, ' '))  continue;
		if (remove_top_s(s, '\t')) continue;
		break;
	}
	//����
	for (;;) {
		if (remove_end_s(s, "�@")) continue;
		if (remove_end_s(s, ' '))  continue;
		if (remove_end_s(s, '\t')) continue;
		break;
	}

	return Trim(s);
}

//---------------------------------------------------------------------------
//������̒u��
//---------------------------------------------------------------------------
UnicodeString replace_i(UnicodeString s, const _TCHAR *o, const _TCHAR *r)
{
	return ReplaceText(s, o, r);
}
//---------------------------------------------------------------------------
UnicodeString replace_s(UnicodeString s, const _TCHAR *o, const _TCHAR *r)
{
	return ReplaceStr(s, o, r);
}

//---------------------------------------------------------------------------
//���K�\���ɂ��u��
//---------------------------------------------------------------------------
UnicodeString replace_regex(UnicodeString s, const _TCHAR *o, const _TCHAR *r)
{
	return TRegEx::Replace(s, o, r);
}

//---------------------------------------------------------------------------
//���X�g�ɂ��������ĕ������u��
//  ���X�g����: ���������� TAB �u��������
//---------------------------------------------------------------------------
UnicodeString replace_str_by_list(UnicodeString s, TStringList *lst)
{
	for (int i=0; i<lst->Count; i++) {
		UnicodeString t_s = lst->Strings[i];
		UnicodeString f_s = split_pre_tab(t_s);
		s = ReplaceStr(s, f_s, t_s);
	}
	return s;
}

//---------------------------------------------------------------------------
//������𖖔��ɒǉ��������̂��擾
//---------------------------------------------------------------------------
UnicodeString append_str(UnicodeString s, const _TCHAR *t)
{
	return s + UnicodeString(t);
}

//---------------------------------------------------------------------------
//; ��؂�ŕ������ǉ�
//---------------------------------------------------------------------------
void cat_str_semicolon(UnicodeString &s, UnicodeString t)
{
	if (EndsStr(";", t)) t.Delete(t.Length(), 1);
	if (t.IsEmpty()) return;

	if (!s.IsEmpty() && !EndsStr(";", s)) s.UCAT_T(";");
	s += t;
}

//---------------------------------------------------------------------------
//�w�蒷�ɂȂ�܂Ő擪�ɋ󔒂�}��
//---------------------------------------------------------------------------
UnicodeString ins_spc_length(UnicodeString s, int len)
{
	int n = len - s.Length();
	if (n>0) s.Insert(StringOfChar(_T(' '), n), 1);
	return s;
}

//---------------------------------------------------------------------------
//�����񂪋�Ȃ�f�t�H���g��������擾
//---------------------------------------------------------------------------
UnicodeString def_if_empty(UnicodeString s, UnicodeString def)
{
	return s.IsEmpty()? def : s;
}
//---------------------------------------------------------------------------
UnicodeString def_if_empty(UnicodeString s, const _TCHAR *def)
{
	return s.IsEmpty()? UnicodeString(def) : s;
}

//---------------------------------------------------------------------------
//�����񂩂琮���l�𒊏o
//�I���s�������O�𑗏o
//---------------------------------------------------------------------------
int extract_int(UnicodeString s)
{
	int i = 1;
	while (i<=s.Length()) if (!iswdigit(s[i])) s.Delete(i, 1); else i++;
	return s.ToInt();
}
//---------------------------------------------------------------------------
//�����񂩂琮���l�𒊏o
//�����̏ꍇ�̓f�t�H���g�l��Ԃ�
//---------------------------------------------------------------------------
int extract_int_def(UnicodeString s, int def)
{
	int i = 1;
	while (i<=s.Length()) if (!iswdigit(s[i])) s.Delete(i, 1); else i++;
	return s.ToIntDef(def);
}

//---------------------------------------------------------------------------
//������̐擪���琔�l�������擾(�J���}�͖���)
//---------------------------------------------------------------------------
UnicodeString extract_top_int_str(UnicodeString s)
{
	UnicodeString ret_str;
	s = Trim(s);
	for (int i=1; i<=s.Length(); i++) {
		WideChar c = s[i];
		if (c==',') {
			if (i==1) break;
		}
		else if (iswdigit(c)) ret_str.cat_sprintf(_T("%c"), c);
		else break;
	}
	return ret_str;
}

//---------------------------------------------------------------------------
//�召��������ʂ����ɕ����������
//---------------------------------------------------------------------------
int pos_i(
	UnicodeString wd,	//������
	UnicodeString s)	//�Ώە�����
{
	if (wd.IsEmpty()) return 0;
	return s.UpperCase().Pos(wd.UpperCase());
}
//---------------------------------------------------------------------------
int pos_i(const _TCHAR *wd, UnicodeString s)
{
	return pos_i(UnicodeString(wd), s);
}

//---------------------------------------------------------------------------
//�����񂪍Ō�Ɍ����ʒu���擾
//---------------------------------------------------------------------------
int pos_r(
	UnicodeString wd,	//������
	UnicodeString s)	//�Ώە�����
{
	if (wd.IsEmpty()) return 0;

	int p  = 0;
	int p0 = 1;
	for (;;) {
		int p1 = PosEx(wd, s, p0);
		if (p1==0) break;
		p  = p1;
		p0 = p + 1;
	}

	return p;
}
//---------------------------------------------------------------------------
int pos_r(const _TCHAR *wd, UnicodeString s)
{
	return pos_r(UnicodeString(wd), s);
}

//---------------------------------------------------------------------------
//�召��������ʂ����ɕ����񂪍Ō�Ɍ����ʒu���擾
//---------------------------------------------------------------------------
int pos_r_i(
	UnicodeString wd,	//������
	UnicodeString s)	//�Ώە�����
{
	if (wd.IsEmpty()) return 0;

	UnicodeString ss = s;
	int p = 0;
	for (;;) {
		int p1 = pos_i(wd, ss);
		if (p1==0) break;
		ss.Delete(1, p1);
		p += p1;
	}

	return p;
}

//---------------------------------------------------------------------------
//���p���O�ŁA�����񂪍Ō�Ɍ����ʒu���擾
//---------------------------------------------------------------------------
int pos_r_q(
	UnicodeString wd,	//������( \v �͏���)
	UnicodeString s)	//�Ώە�����
{
	if (wd.IsEmpty()) return 0;
	UnicodeString ss;
	bool q_flag = false;
	for (int i=1; i<=s.Length(); i++) {
		WideChar c = s[i];
		//���p���O
		if (!q_flag) {
			if (c=='\"') q_flag = true;
		}
		//���p����
		else {
			if (c=='\"')
				q_flag = false;
			else
				c = '\v';
		}
		ss += c;
	}
	return pos_r(wd, ss);
}

//---------------------------------------------------------------------------
//���p���O�ŁA: ���Ō�Ɍ����ʒu���擾 (:\\ �͏���)
//---------------------------------------------------------------------------
int pos_r_q_colon(UnicodeString s)
{
	return pos_r_q(":", REPLACE_TS(s, ":\\", "\f\f"));
}

//---------------------------------------------------------------------------
bool contains_i(UnicodeString s, const _TCHAR *w)
{
	return ContainsText(s, w);
}
//---------------------------------------------------------------------------
bool contains_i(UnicodeString s, WideChar w)
{
	return ContainsText(s, w);
}
//---------------------------------------------------------------------------
bool contains_s(UnicodeString s, const _TCHAR *w)
{
	return ContainsStr(s, w);
}
//---------------------------------------------------------------------------
bool contains_s(UnicodeString s, WideChar w)
{
	return ContainsStr(s, w);
}

//---------------------------------------------------------------------------
// | ��؂胊�X�g�Ɏw��ꂪ�܂܂�Ă��邩�H
//---------------------------------------------------------------------------
bool contained_wd_i(UnicodeString lst, UnicodeString wd)
{
	if (wd.IsEmpty()) return false;

	if (!StartsStr("|", lst)) lst.Insert("|" ,1);
	if (!EndsStr("|", lst))   lst.UCAT_T("|");
	return ContainsText(lst, "|" + wd + "|");
}
//---------------------------------------------------------------------------
bool contained_wd_i(const _TCHAR *lst, UnicodeString wd)
{
	return contained_wd_i(UnicodeString(lst), wd);
}

//---------------------------------------------------------------------------
// | ��؂胊�X�g�̌���܂�ł��邩�H
//---------------------------------------------------------------------------
bool contains_wd_i(UnicodeString s, const _TCHAR *lst)
{
	TStringDynArray w_lst = SplitString(lst, "|");
	for (int i=0; i<w_lst.Length; i++) if (ContainsText(s, w_lst[i])) return true;
	return false;
}

//---------------------------------------------------------------------------
// | ��؂胊�X�g����w��C���f�b�N�X�̌���擾
//---------------------------------------------------------------------------
UnicodeString get_word_i_idx(const _TCHAR *lst, int idx)
{
	TStringDynArray w_lst = SplitString(lst, "|");
	return (idx>=0 && idx<w_lst.Length)? w_lst[idx] : EmptyStr;
}
//---------------------------------------------------------------------------
// | ��؂胊�X�g����w���̃C���f�b�N�X���擾
//---------------------------------------------------------------------------
int idx_of_word_i(const _TCHAR *lst, UnicodeString wd)
{
	TStringDynArray w_lst = SplitString(lst, "|");
	int idx = -1;
	for (int i=0; i<w_lst.Length && idx==-1; i++) if (SameText(w_lst[i], wd)) idx = i;
	return idx;
}

//---------------------------------------------------------------------------
//���C���h�J�[�h���܂ރp�^�[���ɂ�镶�����v
//---------------------------------------------------------------------------
bool str_match(UnicodeString ptn, UnicodeString s)
{
	UnicodeString c = ptn.SubString(1, 1);
	UnicodeString next_str = s.SubString(2, s.Length() - 1);
	UnicodeString next_ptn = ptn.SubString(2, ptn.Length() - 1);

	if (c.IsEmpty())
		return s.IsEmpty();
	else if (SameStr(c, "*"))
		return str_match(next_ptn, s) || (!s.IsEmpty() && str_match(ptn, next_str));
	else if (SameStr(c, "?"))
		return !s.IsEmpty() && str_match(next_ptn, next_str);
	else {
		return (CompareText(c, s.SubString(1, 1))==0) && str_match(next_ptn, next_str);
	}
}

//---------------------------------------------------------------------------
//���K�\���p�^�[���̃`�F�b�N
//---------------------------------------------------------------------------
bool chk_RegExPtn(UnicodeString ptn)
{
	if (ptn.IsEmpty()) return false;

	try {
		TRegEx x(ptn, TRegExOptions() << roCompiled);
		return true;
	}
	catch (...) {
		return false;
	}
}
//---------------------------------------------------------------------------
//���K�\���ɂ��}�b�`
//---------------------------------------------------------------------------
bool is_match_regex(UnicodeString s, const _TCHAR *ptn)
{
	return TRegEx::IsMatch(s, ptn);
}
//---------------------------------------------------------------------------
bool is_match_regex_i(UnicodeString s, const _TCHAR *ptn)
{
	TRegExOptions opt; opt << roIgnoreCase;
	return TRegEx::IsMatch(s, ptn, opt);
}

//---------------------------------------------------------------------------
// ; ��؂蕶���񂩂琳�K�\���p�^�[��(/�`/)�𒊏o
//---------------------------------------------------------------------------
UnicodeString extract_prm_RegExPtn(UnicodeString &s)
{
	UnicodeString ret_str;

	TStringDynArray lst = split_strings_semicolon(s);
	int idx = -1;
	for (int i=0; i<lst.Length && idx==-1; i++) {
		if (is_regex_slash(lst[i])) {
			ret_str = lst[i];
			idx = i;
		}
	}

	//�Y���p�����[�^����������폜
	if (idx!=-1) {
		UnicodeString lbuf;
		for (int i=0; i<lst.Length; i++) {
			if (i==idx) continue;
			if (!lbuf.IsEmpty()) lbuf.UCAT_T(";");
			lbuf += lst[i];
		}
		s = lbuf;
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//������Ɏw���(�܂��̓p�^�[��)���܂܂�Ă��邩�H
//  �߂�l: �}�b�`���������̕�����
//---------------------------------------------------------------------------
UnicodeString ptn_match_str(
	UnicodeString ptn,	//�L�[���[�h (;�ŋ�؂��ĕ����w���)
						//  /�`/ �Ȃ琳�K�\���p�^�[��
	UnicodeString s)	//�Ώە�����
{
	if (ptn.IsEmpty() || s.IsEmpty()) return EmptyStr;

	TStringDynArray p_lst = SplitString(ptn, ";");
	TRegExOptions opt; opt << roIgnoreCase;
	try {
		UnicodeString ret_str;
		for (int i=0; i<p_lst.Length && ret_str.IsEmpty(); i++) {
			UnicodeString kwd = p_lst[i];	if (kwd.IsEmpty()) continue;
			//���K�\��
			if (is_regex_slash(kwd)) {
				TMatch mt = TRegEx::Match(s, exclude_top_end(kwd), opt);
				ret_str = mt.Success? mt.Value : EmptyStr;
			}
			//�ʏ�
			else {
				int p = pos_i(kwd, s);
				ret_str = (p>0)? s.SubString(p, kwd.Length()) : EmptyStr;
			}
		}
		return ret_str;
	}
	catch (...) {
		return EmptyStr;
	}
}
//---------------------------------------------------------------------------
//������Ɏw���(�܂��̓p�^�[��)�Ŏn�܂邩�H
//---------------------------------------------------------------------------
bool starts_ptn(
	UnicodeString ptn,	//�J�n������ (/�`/ �Ȃ琳�K�\���p�^�[��)
	UnicodeString s)	//�Ώە�����
{
	try {
		bool res;
		//���K�\��
		if (is_regex_slash(ptn)) {
			TRegExOptions opt; opt << roIgnoreCase;
			TMatch mt = TRegEx::Match(s, exclude_top_end(ptn), opt);
			res = mt.Success && (mt.Index==1);
		}
		//�ʏ�
		else {
			res = StartsText(ptn, s);
		}
		return res;
	}
	catch (...) {
		return false;
	}
}

//---------------------------------------------------------------------------
//�����ꃊ�X�g�쐬
//---------------------------------------------------------------------------
void get_find_wd_list(UnicodeString wd, TStringList *lst)
{
	for (;;) {
		if (wd.IsEmpty()) break;
		int ph = wd.Pos(' ');
		int pw = wd.Pos('�@');
		int p = (ph && !pw)? ph : (!ph && pw)? pw : (ph && pw)? std::min(ph, pw) : 0;
		if (p==1)
			wd.Delete(1, 1);
		else if (p>1) {
			lst->Add(wd.SubString(1, p - 1));
			wd.Delete(1, p);
		}
		else {
			lst->Add(wd); break;
		}
	}

	//�󔒂�^�u��ϊ�
	for (int i=0; i<lst->Count; i++) lst->Strings[i] = conv_esc_char(lst->Strings[i]);
}

//---------------------------------------------------------------------------
//�����̕������ OR/AND ����
//---------------------------------------------------------------------------
bool find_mlt(
	UnicodeString wd,	//������ (�����̏ꍇ�͔��p�܂��͑S�p�󔒂ŋ�؂�j
	UnicodeString s,	//�Ώە�����
	bool and_sw,		//AND����			(default = false)
	bool not_sw,		//NOT����			(default = false)
	bool case_sw)		//�召���������	(default = false)
{
	if (wd.IsEmpty()) return false;

	//�����ꃊ�X�g�쐬
	std::unique_ptr<TStringList> wlst(new TStringList());
	get_find_wd_list(wd, wlst.get());

	//����
	bool ret = and_sw;
	for (int i=0; i<wlst->Count; i++) {
		int p = case_sw? s.Pos(wlst->Strings[i]) : pos_i(wlst->Strings[i], s);
		if (!not_sw) {
			//�܂�
			if (p>0) {
				//�����ꂩ��
				if (!and_sw) { ret = true; break; }
			}
			else {
				//�S�Ă�
				if (and_sw) { ret = false; break; }
			}
		}
		else {
			//�܂܂Ȃ�
			if (p==0) {
				//�����ꂩ��
				if (!and_sw) { ret = true; break; }
			}
			else {
				//�S�Ă�
				if (and_sw) { ret = false; break; }
			}
		}
	}

	return ret;
}

//---------------------------------------------------------------------------
//�����̕�������������A�}�b�`������̃��X�g���擾����
//---------------------------------------------------------------------------
bool find_mlt_str(
	UnicodeString wd,	//������ (�����̏ꍇ�͔��p�܂��͑S�p�󔒂ŋ�؂�j
	UnicodeString s,	//�Ώە�����
	TStringList *lst,	//[o] ���ۂɃ}�b�`������̃��X�g
	bool case_sw)		//�召���������	(default = false)
{
	lst->Clear();
	if (wd.IsEmpty()) return false;

	//�����ꃊ�X�g�쐬
	std::unique_ptr<TStringList> wlst(new TStringList());
	get_find_wd_list(wd, wlst.get());

	//����
	for (int i=0; i<wlst->Count; i++) {
		UnicodeString wd = wlst->Strings[i];
		int p = case_sw? s.Pos(wd) : pos_i(wd, s);
		if (p>0) {
			UnicodeString fwd = s.SubString(p, wd.Length());
			if (lst->IndexOf(fwd)==-1) lst->Add(fwd);
		}
	}

	return (lst->Count>0);
}

//---------------------------------------------------------------------------
//�s�����擾
//---------------------------------------------------------------------------
int get_line_count(UnicodeString s)
{
	std::unique_ptr<TStringList> lst(new TStringList());
	lst->Text = s;
	return lst->Count;
}

//---------------------------------------------------------------------------
//TStringDynArray �ɕ������ǉ�
//---------------------------------------------------------------------------
void add_dyn_array(TStringDynArray &lst, UnicodeString s)
{
	int len    = lst.Length;
	lst.Length = len + 1;
	lst[len]   = s;
}

//---------------------------------------------------------------------------
//CSV���ړ��e�擾
//---------------------------------------------------------------------------
TStringDynArray get_csv_array(UnicodeString src, int size, 
	bool force_size)	//�T�C�Y�������I�ɐݒ�  (default = false)
{
	TStringDynArray ret_array;
	ret_array.Length = size;
	std::unique_ptr<TStringList> rec(new TStringList());
	rec->Delimiter		 = ',';
	rec->QuoteChar		 = '\"';
	rec->StrictDelimiter = true;
	rec->DelimitedText	 = src;
	for (int i=0; i<rec->Count && i<size; i++) ret_array[i] = rec->Strings[i];
	if (!force_size) ret_array.Length = rec->Count;
	return ret_array;
}
//---------------------------------------------------------------------------
UnicodeString get_csv_item(UnicodeString src, int idx)
{
	std::unique_ptr<TStringList> rec(new TStringList());
	rec->Delimiter		 = ',';
	rec->QuoteChar		 = '\"';
	rec->StrictDelimiter = true;
	rec->DelimitedText	 = src;
	return (idx<rec->Count)? rec->Strings[idx] : EmptyStr;
}

//---------------------------------------------------------------------------
UnicodeString get_tsv_item(UnicodeString src, int idx)
{
	TStringDynArray rec = SplitString(src, "\t");
	return (idx<rec.Length)? rec[idx] : EmptyStr;
}

//---------------------------------------------------------------------------
//CSV���ڂ��쐬
//---------------------------------------------------------------------------
UnicodeString make_csv_str(UnicodeString s)
{
	return UnicodeString().sprintf(_T("\"%s\""), REPLACE_TS(s, "\"", "\"\"").c_str());
}
//---------------------------------------------------------------------------
UnicodeString make_csv_str(bool sw)
{
	return UnicodeString().sprintf(_T("\"%u\""), sw? 1 : 0);
}

//---------------------------------------------------------------------------
UnicodeString make_csv_rec_str(TStringDynArray lst)
{
	UnicodeString lbuf;
	for (int i=0; i<lst.Length; i++) {
		if (i>0) lbuf.UCAT_T(",");
		lbuf += make_csv_str(lst[i]);
	}
	return lbuf;
}

//---------------------------------------------------------------------------
//CSV�̃��X�g���� p �Ԗڂ̍��ڂ� s �̃C���f�b�N�X���擾
//---------------------------------------------------------------------------
int indexof_csv_list(TStringList *lst, UnicodeString s, int p)
{
	int idx = -1;
	for (int i=0; i<lst->Count && idx==-1; i++)
		if (SameText(s, get_csv_item(lst->Strings[i], p))) idx = i;
	return idx;
}
//---------------------------------------------------------------------------
//CSV�̃��X�g���� p �Ԗڂ̍��ڂ� s �̃��R�[�h���擾
//---------------------------------------------------------------------------
TStringDynArray record_of_csv_list(TStringList *lst, UnicodeString s, int p, int size)
{
	TStringDynArray ret_array;
	if (lst) {
		int idx = indexof_csv_list(lst, s, p);
		if (idx!=-1) ret_array = get_csv_array(lst->Strings[idx], size, true);
	}
	return ret_array;
}

//---------------------------------------------------------------------------
//�G�X�P�[�v�V�[�P���X�����̕ϊ�
//---------------------------------------------------------------------------
UnicodeString conv_esc_char(UnicodeString s)
{
	if (s.IsEmpty()) return EmptyStr;

	s = REPLACE_TS(s, "\\\\", "\f");	//\\ ���ꎞ�I�� \f �ɕς��Ă�����
	s = REPLACE_TS(s, "\\t",  "\t");	//�^�u
	s = REPLACE_TS(s, "\\n",  "\r\n");	//���s
	s = REPLACE_TS(s, "\\s",  " ");		//NyanFi �ŗL
	s = REPLACE_TS(s, "\f",   "\\");	//\\(\f) �� \ �ɕϊ�
	return s;
}

//---------------------------------------------------------------------------
//�u�� \\ �� /
//---------------------------------------------------------------------------
UnicodeString yen_to_slash(UnicodeString s)
{
	return REPLACE_TS(s, "\\", "/");
}
//---------------------------------------------------------------------------
//�u�� / �� \\
//---------------------------------------------------------------------------
UnicodeString slash_to_yen(UnicodeString s)
{
	return REPLACE_TS(s, "/", "\\");
}

//---------------------------------------------------------------------------
//�������召��������ʂ����ɔ�r
//---------------------------------------------------------------------------
bool same_ut_i(UnicodeString s, const _TCHAR *t)
{
	return (_tcsicmp(s.c_str(), t)==0);
}
//---------------------------------------------------------------------------
//�������召��������ʂ��Ăɔ�r
//---------------------------------------------------------------------------
bool same_ut_s(UnicodeString s, const _TCHAR *t)
{
	return (_tcscmp(s.c_str(), t)==0);
}

//---------------------------------------------------------------------------
//������=="1" ?
//---------------------------------------------------------------------------
bool equal_1(UnicodeString s)
{
	return (_tcscmp(s.c_str(), _T("1"))==0);
}
//---------------------------------------------------------------------------
//������=="0" ?
//---------------------------------------------------------------------------
bool equal_0(UnicodeString s)
{
	return (_tcscmp(s.c_str(), _T("0"))==0);
}
//---------------------------------------------------------------------------
//������=="ENTER" ?
//---------------------------------------------------------------------------
bool equal_ENTER(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("ENTER"))==0);
}
//---------------------------------------------------------------------------
//������=="ESC" ?
//---------------------------------------------------------------------------
bool equal_ESC(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("ESC"))==0);
}
//---------------------------------------------------------------------------
//������=="TAB" ?
//---------------------------------------------------------------------------
bool equal_TAB(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("TAB"))==0);
}
//---------------------------------------------------------------------------
//������=="DEL" ?
//---------------------------------------------------------------------------
bool equal_DEL(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("DEL"))==0);
}
//---------------------------------------------------------------------------
//������=="LEFT" ?
//---------------------------------------------------------------------------
bool equal_LEFT(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("LEFT"))==0);
}
//---------------------------------------------------------------------------
//������=="RIGHT" ?
//---------------------------------------------------------------------------
bool equal_RIGHT(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("RIGHT"))==0);
}
//---------------------------------------------------------------------------
//������=="RIGHT" ?
//---------------------------------------------------------------------------
bool equal_F1(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("F1"))==0);
}

//---------------------------------------------------------------------------
//�Z�p���[�^(=="-") ?
//---------------------------------------------------------------------------
bool is_separator(UnicodeString s)
{
	return (_tcscmp(s.c_str(), _T("-"))==0);
}

//---------------------------------------------------------------------------
bool starts_t_i(const _TCHAR *w, UnicodeString s)
{
	return StartsText(w, s);
}
//---------------------------------------------------------------------------
bool starts_t_s(const _TCHAR *w, UnicodeString s)
{
	return StartsStr(w, s);
}
//---------------------------------------------------------------------------
bool starts_t_s(WideChar w, UnicodeString s)
{
	return StartsStr(w, s);
}

//---------------------------------------------------------------------------
bool ends_t_i(const _TCHAR *w, UnicodeString s)
{
	return EndsText(w, s);
}
//---------------------------------------------------------------------------
bool ends_t_s(const _TCHAR *w, UnicodeString s)
{
	return EndsStr(w, s);
}
//---------------------------------------------------------------------------
bool ends_t_s(WideChar w, UnicodeString s)
{
	return EndsStr(w, s);
}

//---------------------------------------------------------------------------
bool starts_tchs(const _TCHAR *lst, UnicodeString s)
{
	return s.IsDelimiter(lst, 1);
}
//---------------------------------------------------------------------------
bool ends_tchs(const _TCHAR *lst, UnicodeString s)
{
	return s.IsDelimiter(lst, s.Length());
}

//---------------------------------------------------------------------------
// @ �Ŏn�܂邩 ?
//---------------------------------------------------------------------------
bool starts_AT(UnicodeString s)
{
	return starts_t_s('@', s);
}
//---------------------------------------------------------------------------
// $ �Ŏn�܂邩 ?
//---------------------------------------------------------------------------
bool starts_Dollar(UnicodeString s)
{
	return starts_t_s('$', s);
}

//---------------------------------------------------------------------------
// \ �ŏI��� ?
//---------------------------------------------------------------------------
bool ends_PathDlmtr(UnicodeString s)
{
	return ends_t_s('\\', s);
}
//---------------------------------------------------------------------------
// \ ���܂� ?
//---------------------------------------------------------------------------
bool contains_PathDlmtr(UnicodeString s)
{
	return contains_s(s, '\\');
}
//---------------------------------------------------------------------------
// / ���܂� ?
//---------------------------------------------------------------------------
bool contains_Slash(UnicodeString s)
{
	return contains_s(s, '/');
}

//---------------------------------------------------------------------------
// /�`/ (���K�\��) ?
//---------------------------------------------------------------------------
bool is_regex_slash(UnicodeString s)
{
	return (s.Length()>=2 && starts_t_s('/', s) && ends_t_s('/', s));
}

//---------------------------------------------------------------------------
//���p���ň͂܂�Ă��邩?
//---------------------------------------------------------------------------
bool is_quot(UnicodeString s)
{
	return (s.Length()>=2 && ((starts_t_s('\"', s) && ends_t_s('\"', s)) || ((starts_t_s('\'', s) && ends_t_s('\'', s)))));
}
//---------------------------------------------------------------------------
//�󔒂��܂�ł���ꍇ�A���p���ň͂�
//---------------------------------------------------------------------------
UnicodeString add_quot_if_spc(UnicodeString s)
{
	if (ContainsStr(s, " ") || ContainsStr(s, "�@")) s = "\"" + s + "\"";
	return s;
}

//---------------------------------------------------------------------------
//���p�����O��
//---------------------------------------------------------------------------
UnicodeString exclude_quot(UnicodeString s)
{
	if (is_quot(s)) s = exclude_top_end(s);
	return s;
}

//---------------------------------------------------------------------------
//TStringList Vlues[name] �̐����l���擾
//---------------------------------------------------------------------------
int get_ListIntVal(TStringList *lst, const _TCHAR *name, int def)
{
	return lst->Values[name].ToIntDef(def);
}
//---------------------------------------------------------------------------
//TStringList Vlues[name] ����?
//---------------------------------------------------------------------------
bool ListVal_is_empty(TStringList *lst, const _TCHAR *name)
{
	return lst->Values[name].IsEmpty();
}
//---------------------------------------------------------------------------
//TStringList Vlues[name]=="1"?
//---------------------------------------------------------------------------
bool ListVal_equal_1(TStringList *lst, const _TCHAR *name)
{
	return (_tcscmp(lst->Values[name].c_str(), _T("1"))==0);
}

//---------------------------------------------------------------------------
//m�b���玞�ԕ������
//---------------------------------------------------------------------------
UnicodeString __fastcall mSecToTStr(
	unsigned int ms,
	bool cs)			//1/100�b��\�� (default = true)
{
	int scnt = ms/1000;	//�b
	int c = (int)(ms/10.0 + 0.5) % 100;
	int s = scnt % 60;	scnt /= 60;
	int m = scnt % 60;
	int h = scnt / 60;

	UnicodeString ret_str;
	if (cs)
		ret_str.sprintf(_T("%02u:%02u:%02u.%02u"), h, m, s, c);
	else {
		if (c>50) s++;
		ret_str.sprintf(_T("%02u:%02u:%02u"), h, m, s);
	}
	return ret_str;
}
//---------------------------------------------------------------------------
//�p�����[�^��m�b�ɕϊ�
//---------------------------------------------------------------------------
int param_to_mSec(UnicodeString prm)
{
	int k;
	if		(remove_end_text(prm, "S")) k = 1000;
	else if (remove_end_text(prm, "M")) k = (60*1000);
	else if (remove_end_text(prm, "H")) k = (60*60*1000);
	else								k = 1;
	return prm.ToIntDef(0) * k;
}

//---------------------------------------------------------------------------
//�擾���ƕ\��������v���Ȃ���O�t�H���g(Ricty Diminished �`)��?
//---------------------------------------------------------------------------
bool IsIrregularFont(TFont *fnt)
{
	if (!StartsStr("Ricty Diminished", fnt->Name)) return false;

	switch (fnt->Size) {
	case 11: case 13: case 14: case 16: case 17: case 19: case 20:
	case 32: case 34: case 35: case 37: case 38: case 40: case 41:
	case 53: case 55: case 56: case 58: case 59: case 61: case 62:
		// ((fsz>=11 && fsz<=20) || (fsz>=32 && fsz<=41) || (fsz>=53 && fsz<=62)) && Size*96%72!=0
		return true;
	default:
		return false;
	}
}

//---------------------------------------------------------------------------
//������̕\�������擾 (��O�t�H���g�ɑΉ�)
//��Ricty Diminished �́A���p�����̎��ۂ̕\�������擾�l+1�ɂȂ�ꍇ������
//---------------------------------------------------------------------------
int get_TextWidth(
	TCanvas *cv,
	UnicodeString s,	//������
	bool is_irreg)		//true = ��O�t�H���g(Ricty Diminished �`)
{
	if (is_irreg) {
		int wd = 0;
		for (int i=1; i<=s.Length(); i++) {
			wd += cv->TextWidth(s[i]);
			if (str_len_half(s[i])==1) wd ++;
		}
		return wd;
	}
	else
		return cv->TextWidth(s);
}

//---------------------------------------------------------------------------
//�w�蕶�����̕\�������擾
//---------------------------------------------------------------------------
int get_CharWidth(
	TCanvas *cv,
	int n,			//������	(���p�P��)
	int mgn)		//�]�T		(default = 0);
{
	TEXTMETRIC tm;
	::GetTextMetrics(cv->Handle , &tm);
	return (tm.tmAveCharWidth * n + mgn);
}

//---------------------------------------------------------------------------
// ������ TPanel �̕������ς��Ɏ��܂�悤�ɁA�w��ʒu���󔒂�}��
//---------------------------------------------------------------------------
UnicodeString fit_StrToPanel(UnicodeString s, int p, TPanel *pp)
{
	HDC hDc = ::GetDC(pp->Handle);
	if (hDc) {
		std::unique_ptr<TCanvas> cv(new TCanvas());
		cv->Handle = hDc;
		cv->Font->Assign(pp->Font);
		while (cv->TextWidth(UAPP_T(s, "  "))<pp->ClientWidth) s.Insert(" ", p);
		::ReleaseDC(pp->Handle, hDc);
	}
	return s;
}

//---------------------------------------------------------------------------
//���p���Z�̕����񒷂��擾
//---------------------------------------------------------------------------
int str_len_half(UnicodeString s)
{
	AnsiString    s_a = s;
	UnicodeString s_u = s_a;
	if (s!=s_u && s.Length()==s_u.Length()) {
		UnicodeString r_u;
		for (int i=1; i<=s.Length(); i++) {
			if (!s.IsTrailSurrogate(i)) {
				if (s[i]!=s_u[i] && s_u[i]=='?')
					r_u.UCAT_T("�H");
				else
					r_u += s_u[i];
			}
		}
		s_a = r_u;
	}
	return s_a.Length();
}

//---------------------------------------------------------------------------
//�T���Q�[�g�y�A���l�����ĕ��������擾
//---------------------------------------------------------------------------
int str_len_unicode(UnicodeString s)
{
	int n = 0;
	for (int i=1; i<=s.Length(); i++) if (!s.IsTrailSurrogate(i)) n++;
	return n;
}

//---------------------------------------------------------------------------
//�ő啝���X�V
//---------------------------------------------------------------------------
void max_len_half(int &w, UnicodeString s)
{
	w = std::max(w, str_len_half(s));
}

//---------------------------------------------------------------------------
//�w�蕝(���p�P��)���ŕ����񂪉E�l�ɂȂ�悤�ɋ󔒂�}��
//---------------------------------------------------------------------------
UnicodeString align_r_str(UnicodeString s, int wd, UnicodeString post_s)
{
	int n = str_len_half(s);
	if (n<wd) s.Insert(StringOfChar(_T(' '), wd - n), 1);
	return s + post_s;
}
//---------------------------------------------------------------------------
//�w�蕝(���p�P��)���ŕ����񂪍��l�ɂȂ�悤�ɋ󔒂�}��
//---------------------------------------------------------------------------
UnicodeString align_l_str(UnicodeString s, int wd, UnicodeString post_s)
{
	int n = str_len_half(s);
	if (n<wd) s += StringOfChar(_T(' '), wd - n);
	return s + post_s;
}

//---------------------------------------------------------------------------
//�����񂪎w�蕝�Ɏ��܂�悤�ɒ���
//---------------------------------------------------------------------------
UnicodeString minimize_str(
	UnicodeString s,	//�Ώە�����
	TCanvas *cv,
	int  wd,			//������
	bool omit_end)		//�������ȗ� (default = false : ���Ԃ��ȗ�)
{
	bool is_irreg = IsIrregularFont(cv->Font);

	int ww = get_TextWidth(cv, s, is_irreg);
	if (ww > wd) {
		if (omit_end) {
			s.UCAT_T("�c");
			int p = s.Length() - 1;
			while (p>0) {
				s.Delete(p--, 1);
				if (get_TextWidth(cv, s, is_irreg) <= wd) break;
			}
		}
		else {
			int p = 0;
			for (int i=1; i<s.Length(); i++) {
				if (get_TextWidth(cv, s.SubString(1, i), is_irreg) > wd/2) {
					p = i - 1; break;
				}
			}
			if (p==0) p = 1;
			s.Insert("�c", p);	p += 1;
			while (s.Length()>=p) {
				s.Delete(p, 1);
				if (get_TextWidth(cv, s, is_irreg) <= wd) break;
			}
		}
	}
	return s;
}

//---------------------------------------------------------------------------
UnicodeString fit_str(
	UnicodeString s,	//�Ώە�����
	TCanvas *cv,
	int  wd,			//������
	bool omit_end)		//�������ȗ� (default = false : ���Ԃ��ȗ�)
{
	if (cv->TextWidth(s) > wd)
		s = minimize_str(s, cv, wd, omit_end);
	else {
		for (;;) {
			if (cv->TextWidth(s + " ")>wd) break;
			s.UCAT_T(" ");
		}
	}
	return s;
}

//---------------------------------------------------------------------------
//�S�p/���p�ϊ�
//---------------------------------------------------------------------------
UnicodeString to_Full_or_Half(UnicodeString s, bool to_w)
{
	const int size_s = s.Length() + 1;
	int size_d = ::LCMapString(::GetUserDefaultLCID(), (to_w? LCMAP_FULLWIDTH : LCMAP_HALFWIDTH), s.c_str(), size_s, NULL, 0);
	std::unique_ptr<_TCHAR[]> sbuf(new _TCHAR[size_d]);
	::ZeroMemory(sbuf.get(), size_d * sizeof(_TCHAR));
	::LCMapString(::GetUserDefaultLCID(), (to_w? LCMAP_FULLWIDTH : LCMAP_HALFWIDTH), s.c_str(), size_s, sbuf.get(), size_d);
	return UnicodeString(sbuf.get());
}
//---------------------------------------------------------------------------
//�S�p�ɕϊ�
//---------------------------------------------------------------------------
UnicodeString to_FullWidth(UnicodeString s)
{
	return to_Full_or_Half(s, true);
}
//---------------------------------------------------------------------------
//���p�ɕϊ�
//---------------------------------------------------------------------------
UnicodeString to_HalfWidth(UnicodeString s)
{
	return to_Full_or_Half(s, false);
}

//---------------------------------------------------------------------------
//�r���s���H
//---------------------------------------------------------------------------
int is_RuledLine(UnicodeString s)
{
	return SameStr(s, StringOfChar(_T('��'), s.Length())) ? 1 :
			(SameStr(s, StringOfChar(_T('��'), s.Length()))? 2 : 0);
}

//---------------------------------------------------------------------------
//�r���s��������擾 (---- -------- ...)
//---------------------------------------------------------------------------
UnicodeString make_RuledLine(int cnt, ...)
{
	UnicodeString ret_str;

	va_list ap;
	va_start(ap, cnt);
	for (int i=0; i<cnt; i++) {
		int w = va_arg(ap, int);
		if (w>0) {
			if (!ret_str.IsEmpty()) ret_str.UCAT_T(" ");
			ret_str += StringOfChar(_T('-'), w);
		}
	}
	va_end(ap);

	return ret_str;
}

//---------------------------------------------------------------------------
//�A�h���X��������擾
//---------------------------------------------------------------------------
UnicodeString get_AddrStr(__int64 adr,
	int w)		//�\���� (default = 0)
{
	UnicodeString ret_str;
	if (adr>=0) {
		unsigned int adr_l = adr & 0xffffffffLL;
		int adr_h = adr >> 32;
		ret_str.sprintf(_T("%08X"), adr_l);
		ret_str.Insert(":", 5);
		if (adr_h>0) ret_str = IntToHex(adr_h, 1) + ":" + ret_str;
		ret_str = align_r_str(ret_str, w);
	}
	else
		ret_str.USET_T("____:____");
	return ret_str;
}

//---------------------------------------------------------------------------
//�A�X�y�N�g�䕶������擾
//---------------------------------------------------------------------------
UnicodeString get_AspectStr(int w, int h)
{
	if (w==0 || h==0) return EmptyStr;

	int w1 = w;
	int h1 = h;
	if (h1>w1) std::swap(w1, h1);
	for (;;) {
		int r = w1%h1;
		if (r==0) break;
		w1 = h1; h1 = r;
	}
	w /= h1;
	h /= h1;

	UnicodeString ret_str;
	if (w<20)
		ret_str.cat_sprintf(_T("%u : %u"), w, h);
	else {
		if (w>h)
			ret_str.cat_sprintf(_T("%5.4g : 1"), 1.0 * w/h);
		else
			ret_str.cat_sprintf(_T("1 : %5.4g"), 1.0 * h/w);
	}
	return Trim(ret_str);
}

//---------------------------------------------------------------------------
//���{��L�[�{�[�h��?
//---------------------------------------------------------------------------
bool is_JpKeybd()
{
	return (::GetKeyboardType(0)==7);
}

//---------------------------------------------------------------------------
//�V�t�g��Ԃ��擾
//---------------------------------------------------------------------------
TShiftState get_Shift()
{
	TShiftState shift;
	if (HIBYTE(::GetAsyncKeyState(VK_SHIFT)))	shift << ssShift;
	if (HIBYTE(::GetAsyncKeyState(VK_CONTROL)))	shift << ssCtrl;
	if (HIBYTE(::GetAsyncKeyState(VK_MENU)))	shift << ssAlt;
	return shift;
}

//---------------------------------------------------------------------------
//�V�t�g��Ԃ𕶎����
//---------------------------------------------------------------------------
UnicodeString get_ShiftStr(TShiftState Shift)
{
	UnicodeString sftstr;
	if (Shift.Contains(ssShift)) sftstr += KeyStr_Shift;
	if (Shift.Contains(ssCtrl))  sftstr += KeyStr_Ctrl;
	if (Shift.Contains(ssAlt))   sftstr += KeyStr_Alt;
	return  sftstr;
}
//---------------------------------------------------------------------------
UnicodeString get_ShiftStr()
{
	return get_ShiftStr(get_Shift());
}

//---------------------------------------------------------------------------
//�L�[�𕶎���ɕϊ�
//  JP/US�L�[�{�[�h�Ή�
//---------------------------------------------------------------------------
UnicodeString get_KeyStr(WORD Key)
{
	UnicodeString keystr;

	switch (Key) {
	case VK_RETURN: 	keystr.USET_T("ENTER");	break;
	case VK_ESCAPE: 	keystr.USET_T("ESC");	break;
	case VK_PAUSE:  	keystr.USET_T("PAUSE");	break;
	case VK_SPACE:  	keystr.USET_T("SPACE");	break;
	case VK_TAB:		keystr.USET_T("TAB");	break;
	case VK_LEFT:   	keystr.USET_T("LEFT");	break;
	case VK_RIGHT:  	keystr.USET_T("RIGHT");	break;
	case VK_DOWN:		keystr.USET_T("DOWN");	break;
	case VK_UP:			keystr.USET_T("UP");	break;
	case VK_PRIOR:		keystr.USET_T("PGUP");	break;
	case VK_NEXT:		keystr.USET_T("PGDN");	break;
	case VK_HOME:		keystr.USET_T("HOME");	break;
	case VK_END:		keystr.USET_T("END");	break;
	case VK_INSERT:		keystr.USET_T("INS");	break;
	case VK_DELETE: 	keystr.USET_T("DEL");	break;
	case VK_BACK: 		keystr.USET_T("BKSP");	break;
	case VK_F1:			keystr.USET_T("F1");	break;
	case VK_F2:			keystr.USET_T("F2");	break;
	case VK_F3:			keystr.USET_T("F3");	break;
	case VK_F4:			keystr.USET_T("F4");	break;
	case VK_F5:			keystr.USET_T("F5");	break;
	case VK_F6:			keystr.USET_T("F6");	break;
	case VK_F7:			keystr.USET_T("F7");	break;
	case VK_F8:			keystr.USET_T("F8");	break;
	case VK_F9:			keystr.USET_T("F9");	break;
	case VK_F10:		keystr.USET_T("F10");	break;
	case VK_F11:		keystr.USET_T("F11");	break;
	case VK_F12:		keystr.USET_T("F12");	break;
	case VK_APPS:		keystr.USET_T("APP");	break;
	case VK_OEM_1:		keystr.sprintf(_T("%s"), is_JpKeybd()? _T(":") : _T(";")); break;
	case VK_OEM_2:		keystr.USET_T("/");		break;
	case VK_OEM_3:		keystr.sprintf(_T("%s"), is_JpKeybd()? _T("@") : _T("`")); break;
	case VK_OEM_4:		keystr.USET_T("[");		break;
	case VK_OEM_5:		keystr.USET_T("\\");	break;
	case VK_OEM_6:		keystr.USET_T("]");		break;
	case VK_OEM_MINUS:  keystr.USET_T("-");		break;
	case VK_OEM_PLUS:   keystr.sprintf(_T("%s"), is_JpKeybd()? _T(";") : _T("��")); break;
	case VK_OEM_COMMA:  keystr.USET_T(",");		break;
	case VK_OEM_PERIOD: keystr.USET_T(".");		break;
	case VK_OEM_102:    keystr.USET_T("�_");	break;

	case VK_NUMPAD0:	keystr.USET_T("10Key_0");	break;
	case VK_NUMPAD1:	keystr.USET_T("10Key_1");	break;
	case VK_NUMPAD2:	keystr.USET_T("10Key_2");	break;
	case VK_NUMPAD3:	keystr.USET_T("10Key_3");	break;
	case VK_NUMPAD4:	keystr.USET_T("10Key_4");	break;
	case VK_NUMPAD5:	keystr.USET_T("10Key_5");	break;
	case VK_NUMPAD6:	keystr.USET_T("10Key_6");	break;
	case VK_NUMPAD7:	keystr.USET_T("10Key_7");	break;
	case VK_NUMPAD8:	keystr.USET_T("10Key_8");	break;
	case VK_NUMPAD9:	keystr.USET_T("10Key_9");	break;
	case VK_MULTIPLY:	keystr.USET_T("10Key_*");	break;
	case VK_ADD:		keystr.USET_T("10Key_+");	break;
	case VK_SUBTRACT:	keystr.USET_T("10Key_-");	break;
	case VK_DIVIDE:		keystr.USET_T("10Key_/");	break;
	case VK_DECIMAL:	keystr.USET_T("10Key_.");	break;
	case VK_OEM_7:	  	keystr.sprintf(_T("%s"), is_JpKeybd()? _T("^") : _T("'")); break;

	default:
		if (_istalnum(Key)) keystr = (char)Key; else keystr = EmptyStr;
	}

	return  keystr;
}
//---------------------------------------------------------------------------
//�L�[�ƃV�t�g��Ԃ𕶎����
//---------------------------------------------------------------------------
UnicodeString get_KeyStr(WORD Key, TShiftState Shift)
{
	UnicodeString keystr = get_KeyStr(Key);
	return !keystr.IsEmpty()? get_ShiftStr(Shift) + keystr : EmptyStr;
}
//---------------------------------------------------------------------------
