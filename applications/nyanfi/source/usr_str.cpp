//----------------------------------------------------------------------//
// 文字列操作															//
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
//自然順ソートに対応した比較
//---------------------------------------------------------------------------
int __fastcall comp_NaturalOrder(TStringList *List, int Index1, int Index2)
{
	return StrCmpLogicalW(List->Strings[Index1].c_str(), List->Strings[Index2].c_str());
}

//---------------------------------------------------------------------------
//自然昇順 文字列 (カンマ対応/ 同じ場合は Objects で比較)
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
//自然降順 文字列 (カンマ対応/ 同じ場合は Objects で比較)
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
//昇順 Objects
//---------------------------------------------------------------------------
int __fastcall comp_ObjectsOrder(TStringList *List, int Index1, int Index2)
{
	return (int)List->Objects[Index1] - (int)List->Objects[Index2];
}

//---------------------------------------------------------------------------
//CSV用比較関数
//---------------------------------------------------------------------------
int  USR_CsvCol 	 = 0;		//ソート対象列
int  USR_CsvSortMode = 1;		//ソートモード 1:昇順/ -1降順
bool USR_CsvTopIsHdr = false;	//先頭行は項目名

//---------------------------------------------------------------------------
//CSV 自然昇順 (同じ場合は Objects で比較)
//---------------------------------------------------------------------------
int __fastcall comp_CsvNaturalOrder(TStringList *List, int Index1, int Index2)
{
	if (USR_CsvTopIsHdr) {	//先頭行は項目名
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
//TSV 自然昇順 (同じ場合は Objects で比較)
//---------------------------------------------------------------------------
int __fastcall comp_TsvNaturalOrder(TStringList *List, int Index1, int Index2)
{
	if (USR_CsvTopIsHdr) {	//先頭行は項目名
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
//指定セパレータの前の文字列を取得
//  セパレータが見つからない場合は、文字列をそのまま返す
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
//指定セパレータの後の文字列を取得
//  セパレータが見つからない場合は、EmptyStr を返す
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
//指定セパレータ間の文字列を取得
//  後セパレータが見つからない場合は、前セパレータ以降の文字列を返す
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
//タブの前の文字列を取得
//  タブがない場合は、文字列をそのまま返す
//---------------------------------------------------------------------------
UnicodeString get_pre_tab(UnicodeString s)
{
	return get_tkn(s, '\t');
}
//---------------------------------------------------------------------------
//タブの後の文字列を取得
//  タブがない場合は、EmptyStr を返す
//---------------------------------------------------------------------------
UnicodeString get_post_tab(UnicodeString s)
{
	return get_tkn_r(s, '\t');
}

//---------------------------------------------------------------------------
//文字列中の (～) 内を取得
//---------------------------------------------------------------------------
UnicodeString get_in_paren(UnicodeString s)
{
	int p1 = s.Pos('(');
	int p2 = s.Pos(')');
	if (p1==0 || p2==0 || p1>=p2) return EmptyStr;
	return s.SubString(p1 + 1,  p2 - p1 - 1);
}
//---------------------------------------------------------------------------
//文字列中の (～) 内を取得、元文字列は (～) の後続文字列に
//---------------------------------------------------------------------------
UnicodeString split_in_paren(UnicodeString &s)
{
	return get_tkn_r(split_tkn(s, ')'), '(');
}

//---------------------------------------------------------------------------
//前後にタブを含まない文字列を取得
//  複数行の場合、空でない最初の文字列
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
//指定セパレータ前の文字列を分離して取得
//  セパレータが見つからない場合は、文字列をそのまま返し、元文字列は空に
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
//タブ前の文字列を分離して取得
//  タブがない場合は、文字列をそのまま返し、元文字列は空に
//---------------------------------------------------------------------------
UnicodeString split_pre_tab(UnicodeString &s)
{
	return split_tkn(s, '\t');
}

//---------------------------------------------------------------------------
//文字列から説明部分(^:～:)を分離し取得
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
//文字列から先頭文字を分離し取得
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
//コマンドラインをパラメータに分解
//---------------------------------------------------------------------------
int split_cmd_line(UnicodeString s, TStringList *lst)
{
	bool in_qut = false;
	UnicodeString lbuf;
	for (int i=1; i<=s.Length(); i++) {
		WideChar c = s[i];
		//引用符外
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
		//引用符内
		else if (c=='\"') {
			in_qut = false;
		}

		lbuf.cat_sprintf(_T("%c"), c);
	}

	if (!lbuf.IsEmpty()) lst->Add(lbuf);

	return lst->Count;
}

//---------------------------------------------------------------------------
//コマンドラインからファイル名を分離
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
//文字列を指定区切りで分割
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
//文字列から指定語を削除
//  戻り値: ture=削除した/ false=語が含まれていない
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
//文字列の先頭に指定語があったら削除 (大小文字を無視)
//  戻り値: ture=削除した/ false=先頭に語がない
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
//文字列の先頭に指定語があったら削除 (大小文字を区別)
//  戻り値:  ture=削除した/ false=先頭に語がない
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
//文字列の先頭に @ があったら削除
//  戻り値: ture=先頭の @ を削除した
//---------------------------------------------------------------------------
bool remove_top_AT(UnicodeString &s)
{
	return remove_top_s(s, '@');
}
//---------------------------------------------------------------------------
//文字列の先頭に $ があったら削除
//  戻り値: ture=先頭の $ を削除した
//---------------------------------------------------------------------------
bool remove_top_Dollar(UnicodeString &s)
{
	return remove_top_s(s, '$');
}

//---------------------------------------------------------------------------
//文字列の末尾に指定語があったら削除 (大小文字を無視)
//  戻り値: ture=末尾語を削除した
//---------------------------------------------------------------------------
bool remove_end_text(UnicodeString &s, UnicodeString w)
{
	if (!EndsText(w, s)) return false;
	s.Delete(s.Length() - w.Length() + 1, w.Length());
	return true;
}
//---------------------------------------------------------------------------
//文字列の末尾に指定語があったら削除 (大小文字を区別)
//  戻り値: ture=末尾語を削除した
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
//先頭を除外した文字列を取得
//---------------------------------------------------------------------------
UnicodeString exclude_top(UnicodeString s)
{
	return s.SubString(2, s.Length() - 1);
}
//---------------------------------------------------------------------------
//先頭と末尾を除外した文字列を取得
//---------------------------------------------------------------------------
UnicodeString exclude_top_end(UnicodeString s)
{
	return s.SubString(2, s.Length() - 2);
}

//---------------------------------------------------------------------------
//全角スペースも考慮したトリミング
//---------------------------------------------------------------------------
UnicodeString trim_ex(UnicodeString s)
{
	//先頭
	for (;;) {
		if (remove_top_s(s, "　")) continue;
		if (remove_top_s(s, ' '))  continue;
		if (remove_top_s(s, '\t')) continue;
		break;
	}
	//末尾
	for (;;) {
		if (remove_end_s(s, "　")) continue;
		if (remove_end_s(s, ' '))  continue;
		if (remove_end_s(s, '\t')) continue;
		break;
	}

	return Trim(s);
}

//---------------------------------------------------------------------------
//文字列の置換
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
//正規表現による置換
//---------------------------------------------------------------------------
UnicodeString replace_regex(UnicodeString s, const _TCHAR *o, const _TCHAR *r)
{
	return TRegEx::Replace(s, o, r);
}

//---------------------------------------------------------------------------
//リストにしたがって文字列を置換
//  リスト書式: 検索文字列 TAB 置換文字列
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
//文字列を末尾に追加したものを取得
//---------------------------------------------------------------------------
UnicodeString append_str(UnicodeString s, const _TCHAR *t)
{
	return s + UnicodeString(t);
}

//---------------------------------------------------------------------------
//; 区切りで文字列を追加
//---------------------------------------------------------------------------
void cat_str_semicolon(UnicodeString &s, UnicodeString t)
{
	if (EndsStr(";", t)) t.Delete(t.Length(), 1);
	if (t.IsEmpty()) return;

	if (!s.IsEmpty() && !EndsStr(";", s)) s.UCAT_T(";");
	s += t;
}

//---------------------------------------------------------------------------
//指定長になるまで先頭に空白を挿入
//---------------------------------------------------------------------------
UnicodeString ins_spc_length(UnicodeString s, int len)
{
	int n = len - s.Length();
	if (n>0) s.Insert(StringOfChar(_T(' '), n), 1);
	return s;
}

//---------------------------------------------------------------------------
//文字列が空ならデフォルト文字列を取得
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
//文字列から整数値を抽出
//！失敗したら例外を送出
//---------------------------------------------------------------------------
int extract_int(UnicodeString s)
{
	int i = 1;
	while (i<=s.Length()) if (!iswdigit(s[i])) s.Delete(i, 1); else i++;
	return s.ToInt();
}
//---------------------------------------------------------------------------
//文字列から整数値を抽出
//無効の場合はデフォルト値を返す
//---------------------------------------------------------------------------
int extract_int_def(UnicodeString s, int def)
{
	int i = 1;
	while (i<=s.Length()) if (!iswdigit(s[i])) s.Delete(i, 1); else i++;
	return s.ToIntDef(def);
}

//---------------------------------------------------------------------------
//文字列の先頭から数値部分を取得(カンマは無視)
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
//大小文字を区別せずに文字列を検索
//---------------------------------------------------------------------------
int pos_i(
	UnicodeString wd,	//検索語
	UnicodeString s)	//対象文字列
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
//文字列が最後に現れる位置を取得
//---------------------------------------------------------------------------
int pos_r(
	UnicodeString wd,	//検索語
	UnicodeString s)	//対象文字列
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
//大小文字を区別せずに文字列が最後に現れる位置を取得
//---------------------------------------------------------------------------
int pos_r_i(
	UnicodeString wd,	//検索語
	UnicodeString s)	//対象文字列
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
//引用符外で、文字列が最後に現れる位置を取得
//---------------------------------------------------------------------------
int pos_r_q(
	UnicodeString wd,	//検索語( \v は除く)
	UnicodeString s)	//対象文字列
{
	if (wd.IsEmpty()) return 0;
	UnicodeString ss;
	bool q_flag = false;
	for (int i=1; i<=s.Length(); i++) {
		WideChar c = s[i];
		//引用符外
		if (!q_flag) {
			if (c=='\"') q_flag = true;
		}
		//引用符内
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
//引用符外で、: が最後に現れる位置を取得 (:\\ は除く)
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
// | 区切りリストに指定語が含まれているか？
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
// | 区切りリストの語を含んでいるか？
//---------------------------------------------------------------------------
bool contains_wd_i(UnicodeString s, const _TCHAR *lst)
{
	TStringDynArray w_lst = SplitString(lst, "|");
	for (int i=0; i<w_lst.Length; i++) if (ContainsText(s, w_lst[i])) return true;
	return false;
}

//---------------------------------------------------------------------------
// | 区切りリストから指定インデックスの語を取得
//---------------------------------------------------------------------------
UnicodeString get_word_i_idx(const _TCHAR *lst, int idx)
{
	TStringDynArray w_lst = SplitString(lst, "|");
	return (idx>=0 && idx<w_lst.Length)? w_lst[idx] : EmptyStr;
}
//---------------------------------------------------------------------------
// | 区切りリストから指定語のインデックスを取得
//---------------------------------------------------------------------------
int idx_of_word_i(const _TCHAR *lst, UnicodeString wd)
{
	TStringDynArray w_lst = SplitString(lst, "|");
	int idx = -1;
	for (int i=0; i<w_lst.Length && idx==-1; i++) if (SameText(w_lst[i], wd)) idx = i;
	return idx;
}

//---------------------------------------------------------------------------
//ワイルドカードを含むパターンによる文字列一致
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
//正規表現パターンのチェック
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
//正規表現によるマッチ
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
// ; 区切り文字列から正規表現パターン(/～/)を抽出
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

	//該当パラメータがあったら削除
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
//文字列に指定語(またはパターン)が含まれているか？
//  戻り値: マッチした部分の文字列
//---------------------------------------------------------------------------
UnicodeString ptn_match_str(
	UnicodeString ptn,	//キーワード (;で区切って複数指定可)
						//  /～/ なら正規表現パターン
	UnicodeString s)	//対象文字列
{
	if (ptn.IsEmpty() || s.IsEmpty()) return EmptyStr;

	TStringDynArray p_lst = SplitString(ptn, ";");
	TRegExOptions opt; opt << roIgnoreCase;
	try {
		UnicodeString ret_str;
		for (int i=0; i<p_lst.Length && ret_str.IsEmpty(); i++) {
			UnicodeString kwd = p_lst[i];	if (kwd.IsEmpty()) continue;
			//正規表現
			if (is_regex_slash(kwd)) {
				TMatch mt = TRegEx::Match(s, exclude_top_end(kwd), opt);
				ret_str = mt.Success? mt.Value : EmptyStr;
			}
			//通常
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
//文字列に指定語(またはパターン)で始まるか？
//---------------------------------------------------------------------------
bool starts_ptn(
	UnicodeString ptn,	//開始文字列 (/～/ なら正規表現パターン)
	UnicodeString s)	//対象文字列
{
	try {
		bool res;
		//正規表現
		if (is_regex_slash(ptn)) {
			TRegExOptions opt; opt << roIgnoreCase;
			TMatch mt = TRegEx::Match(s, exclude_top_end(ptn), opt);
			res = mt.Success && (mt.Index==1);
		}
		//通常
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
//検索語リスト作成
//---------------------------------------------------------------------------
void get_find_wd_list(UnicodeString wd, TStringList *lst)
{
	for (;;) {
		if (wd.IsEmpty()) break;
		int ph = wd.Pos(' ');
		int pw = wd.Pos('　');
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

	//空白やタブを変換
	for (int i=0; i<lst->Count; i++) lst->Strings[i] = conv_esc_char(lst->Strings[i]);
}

//---------------------------------------------------------------------------
//複数の文字列を OR/AND 検索
//---------------------------------------------------------------------------
bool find_mlt(
	UnicodeString wd,	//検索語 (複数の場合は半角または全角空白で区切る）
	UnicodeString s,	//対象文字列
	bool and_sw,		//AND検索			(default = false)
	bool not_sw,		//NOT検索			(default = false)
	bool case_sw)		//大小文字を区別	(default = false)
{
	if (wd.IsEmpty()) return false;

	//検索語リスト作成
	std::unique_ptr<TStringList> wlst(new TStringList());
	get_find_wd_list(wd, wlst.get());

	//検索
	bool ret = and_sw;
	for (int i=0; i<wlst->Count; i++) {
		int p = case_sw? s.Pos(wlst->Strings[i]) : pos_i(wlst->Strings[i], s);
		if (!not_sw) {
			//含む
			if (p>0) {
				//いずれかを
				if (!and_sw) { ret = true; break; }
			}
			else {
				//全てを
				if (and_sw) { ret = false; break; }
			}
		}
		else {
			//含まない
			if (p==0) {
				//いずれかを
				if (!and_sw) { ret = true; break; }
			}
			else {
				//全てを
				if (and_sw) { ret = false; break; }
			}
		}
	}

	return ret;
}

//---------------------------------------------------------------------------
//複数の文字列を検索し、マッチした語のリストを取得する
//---------------------------------------------------------------------------
bool find_mlt_str(
	UnicodeString wd,	//検索語 (複数の場合は半角または全角空白で区切る）
	UnicodeString s,	//対象文字列
	TStringList *lst,	//[o] 実際にマッチした語のリスト
	bool case_sw)		//大小文字を区別	(default = false)
{
	lst->Clear();
	if (wd.IsEmpty()) return false;

	//検索語リスト作成
	std::unique_ptr<TStringList> wlst(new TStringList());
	get_find_wd_list(wd, wlst.get());

	//検索
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
//行数を取得
//---------------------------------------------------------------------------
int get_line_count(UnicodeString s)
{
	std::unique_ptr<TStringList> lst(new TStringList());
	lst->Text = s;
	return lst->Count;
}

//---------------------------------------------------------------------------
//TStringDynArray に文字列を追加
//---------------------------------------------------------------------------
void add_dyn_array(TStringDynArray &lst, UnicodeString s)
{
	int len    = lst.Length;
	lst.Length = len + 1;
	lst[len]   = s;
}

//---------------------------------------------------------------------------
//CSV項目内容取得
//---------------------------------------------------------------------------
TStringDynArray get_csv_array(UnicodeString src, int size, 
	bool force_size)	//サイズを強制的に設定  (default = false)
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
//CSV項目を作成
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
//CSVのリストから p 番目の項目が s のインデックスを取得
//---------------------------------------------------------------------------
int indexof_csv_list(TStringList *lst, UnicodeString s, int p)
{
	int idx = -1;
	for (int i=0; i<lst->Count && idx==-1; i++)
		if (SameText(s, get_csv_item(lst->Strings[i], p))) idx = i;
	return idx;
}
//---------------------------------------------------------------------------
//CSVのリストから p 番目の項目が s のレコードを取得
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
//エスケープシーケンス文字の変換
//---------------------------------------------------------------------------
UnicodeString conv_esc_char(UnicodeString s)
{
	if (s.IsEmpty()) return EmptyStr;

	s = REPLACE_TS(s, "\\\\", "\f");	//\\ を一時的に \f に変えておいて
	s = REPLACE_TS(s, "\\t",  "\t");	//タブ
	s = REPLACE_TS(s, "\\n",  "\r\n");	//改行
	s = REPLACE_TS(s, "\\s",  " ");		//NyanFi 固有
	s = REPLACE_TS(s, "\f",   "\\");	//\\(\f) を \ に変換
	return s;
}

//---------------------------------------------------------------------------
//置換 \\ → /
//---------------------------------------------------------------------------
UnicodeString yen_to_slash(UnicodeString s)
{
	return REPLACE_TS(s, "\\", "/");
}
//---------------------------------------------------------------------------
//置換 / → \\
//---------------------------------------------------------------------------
UnicodeString slash_to_yen(UnicodeString s)
{
	return REPLACE_TS(s, "/", "\\");
}

//---------------------------------------------------------------------------
//文字列を大小文字を区別せずに比較
//---------------------------------------------------------------------------
bool same_ut_i(UnicodeString s, const _TCHAR *t)
{
	return (_tcsicmp(s.c_str(), t)==0);
}
//---------------------------------------------------------------------------
//文字列を大小文字を区別してに比較
//---------------------------------------------------------------------------
bool same_ut_s(UnicodeString s, const _TCHAR *t)
{
	return (_tcscmp(s.c_str(), t)==0);
}

//---------------------------------------------------------------------------
//文字列=="1" ?
//---------------------------------------------------------------------------
bool equal_1(UnicodeString s)
{
	return (_tcscmp(s.c_str(), _T("1"))==0);
}
//---------------------------------------------------------------------------
//文字列=="0" ?
//---------------------------------------------------------------------------
bool equal_0(UnicodeString s)
{
	return (_tcscmp(s.c_str(), _T("0"))==0);
}
//---------------------------------------------------------------------------
//文字列=="ENTER" ?
//---------------------------------------------------------------------------
bool equal_ENTER(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("ENTER"))==0);
}
//---------------------------------------------------------------------------
//文字列=="ESC" ?
//---------------------------------------------------------------------------
bool equal_ESC(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("ESC"))==0);
}
//---------------------------------------------------------------------------
//文字列=="TAB" ?
//---------------------------------------------------------------------------
bool equal_TAB(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("TAB"))==0);
}
//---------------------------------------------------------------------------
//文字列=="DEL" ?
//---------------------------------------------------------------------------
bool equal_DEL(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("DEL"))==0);
}
//---------------------------------------------------------------------------
//文字列=="LEFT" ?
//---------------------------------------------------------------------------
bool equal_LEFT(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("LEFT"))==0);
}
//---------------------------------------------------------------------------
//文字列=="RIGHT" ?
//---------------------------------------------------------------------------
bool equal_RIGHT(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("RIGHT"))==0);
}
//---------------------------------------------------------------------------
//文字列=="RIGHT" ?
//---------------------------------------------------------------------------
bool equal_F1(UnicodeString s)
{
	return (_tcsicmp(s.c_str(), _T("F1"))==0);
}

//---------------------------------------------------------------------------
//セパレータ(=="-") ?
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
// @ で始まるか ?
//---------------------------------------------------------------------------
bool starts_AT(UnicodeString s)
{
	return starts_t_s('@', s);
}
//---------------------------------------------------------------------------
// $ で始まるか ?
//---------------------------------------------------------------------------
bool starts_Dollar(UnicodeString s)
{
	return starts_t_s('$', s);
}

//---------------------------------------------------------------------------
// \ で終わる ?
//---------------------------------------------------------------------------
bool ends_PathDlmtr(UnicodeString s)
{
	return ends_t_s('\\', s);
}
//---------------------------------------------------------------------------
// \ を含む ?
//---------------------------------------------------------------------------
bool contains_PathDlmtr(UnicodeString s)
{
	return contains_s(s, '\\');
}
//---------------------------------------------------------------------------
// / を含む ?
//---------------------------------------------------------------------------
bool contains_Slash(UnicodeString s)
{
	return contains_s(s, '/');
}

//---------------------------------------------------------------------------
// /～/ (正規表現) ?
//---------------------------------------------------------------------------
bool is_regex_slash(UnicodeString s)
{
	return (s.Length()>=2 && starts_t_s('/', s) && ends_t_s('/', s));
}

//---------------------------------------------------------------------------
//引用符で囲まれているか?
//---------------------------------------------------------------------------
bool is_quot(UnicodeString s)
{
	return (s.Length()>=2 && ((starts_t_s('\"', s) && ends_t_s('\"', s)) || ((starts_t_s('\'', s) && ends_t_s('\'', s)))));
}
//---------------------------------------------------------------------------
//空白を含んでいる場合、引用符で囲む
//---------------------------------------------------------------------------
UnicodeString add_quot_if_spc(UnicodeString s)
{
	if (ContainsStr(s, " ") || ContainsStr(s, "　")) s = "\"" + s + "\"";
	return s;
}

//---------------------------------------------------------------------------
//引用符を外す
//---------------------------------------------------------------------------
UnicodeString exclude_quot(UnicodeString s)
{
	if (is_quot(s)) s = exclude_top_end(s);
	return s;
}

//---------------------------------------------------------------------------
//TStringList Vlues[name] の整数値を取得
//---------------------------------------------------------------------------
int get_ListIntVal(TStringList *lst, const _TCHAR *name, int def)
{
	return lst->Values[name].ToIntDef(def);
}
//---------------------------------------------------------------------------
//TStringList Vlues[name] が空か?
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
//m秒から時間文字列へ
//---------------------------------------------------------------------------
UnicodeString __fastcall mSecToTStr(
	unsigned int ms,
	bool cs)			//1/100秒を表示 (default = true)
{
	int scnt = ms/1000;	//秒
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
//パラメータをm秒に変換
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
//取得幅と表示幅が一致しない例外フォント(Ricty Diminished ～)か?
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
//文字列の表示幅を取得 (例外フォントに対応)
//※Ricty Diminished は、半角文字の実際の表示幅が取得値+1になる場合がある
//---------------------------------------------------------------------------
int get_TextWidth(
	TCanvas *cv,
	UnicodeString s,	//文字列
	bool is_irreg)		//true = 例外フォント(Ricty Diminished ～)
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
//指定文字数の表示幅を取得
//---------------------------------------------------------------------------
int get_CharWidth(
	TCanvas *cv,
	int n,			//文字数	(半角単位)
	int mgn)		//余裕		(default = 0);
{
	TEXTMETRIC tm;
	::GetTextMetrics(cv->Handle , &tm);
	return (tm.tmAveCharWidth * n + mgn);
}

//---------------------------------------------------------------------------
// 文字列が TPanel の幅いっぱいに収まるように、指定位置を空白を挿入
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
//半角換算の文字列長を取得
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
					r_u.UCAT_T("？");
				else
					r_u += s_u[i];
			}
		}
		s_a = r_u;
	}
	return s_a.Length();
}

//---------------------------------------------------------------------------
//サロゲートペアを考慮して文字長を取得
//---------------------------------------------------------------------------
int str_len_unicode(UnicodeString s)
{
	int n = 0;
	for (int i=1; i<=s.Length(); i++) if (!s.IsTrailSurrogate(i)) n++;
	return n;
}

//---------------------------------------------------------------------------
//最大幅を更新
//---------------------------------------------------------------------------
void max_len_half(int &w, UnicodeString s)
{
	w = std::max(w, str_len_half(s));
}

//---------------------------------------------------------------------------
//指定幅(半角単位)内で文字列が右詰になるように空白を挿入
//---------------------------------------------------------------------------
UnicodeString align_r_str(UnicodeString s, int wd, UnicodeString post_s)
{
	int n = str_len_half(s);
	if (n<wd) s.Insert(StringOfChar(_T(' '), wd - n), 1);
	return s + post_s;
}
//---------------------------------------------------------------------------
//指定幅(半角単位)内で文字列が左詰になるように空白を挿入
//---------------------------------------------------------------------------
UnicodeString align_l_str(UnicodeString s, int wd, UnicodeString post_s)
{
	int n = str_len_half(s);
	if (n<wd) s += StringOfChar(_T(' '), wd - n);
	return s + post_s;
}

//---------------------------------------------------------------------------
//文字列が指定幅に収まるように調整
//---------------------------------------------------------------------------
UnicodeString minimize_str(
	UnicodeString s,	//対象文字列
	TCanvas *cv,
	int  wd,			//制限幅
	bool omit_end)		//末尾を省略 (default = false : 中間を省略)
{
	bool is_irreg = IsIrregularFont(cv->Font);

	int ww = get_TextWidth(cv, s, is_irreg);
	if (ww > wd) {
		if (omit_end) {
			s.UCAT_T("…");
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
			s.Insert("…", p);	p += 1;
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
	UnicodeString s,	//対象文字列
	TCanvas *cv,
	int  wd,			//制限幅
	bool omit_end)		//末尾を省略 (default = false : 中間を省略)
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
//全角/半角変換
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
//全角に変換
//---------------------------------------------------------------------------
UnicodeString to_FullWidth(UnicodeString s)
{
	return to_Full_or_Half(s, true);
}
//---------------------------------------------------------------------------
//半角に変換
//---------------------------------------------------------------------------
UnicodeString to_HalfWidth(UnicodeString s)
{
	return to_Full_or_Half(s, false);
}

//---------------------------------------------------------------------------
//罫線行か？
//---------------------------------------------------------------------------
int is_RuledLine(UnicodeString s)
{
	return SameStr(s, StringOfChar(_T('─'), s.Length())) ? 1 :
			(SameStr(s, StringOfChar(_T('━'), s.Length()))? 2 : 0);
}

//---------------------------------------------------------------------------
//罫線行文字列を取得 (---- -------- ...)
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
//アドレス文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_AddrStr(__int64 adr,
	int w)		//表示桁 (default = 0)
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
//アスペクト比文字列を取得
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
//日本語キーボードか?
//---------------------------------------------------------------------------
bool is_JpKeybd()
{
	return (::GetKeyboardType(0)==7);
}

//---------------------------------------------------------------------------
//シフト状態を取得
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
//シフト状態を文字列に
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
//キーを文字列に変換
//  JP/USキーボード対応
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
	case VK_OEM_PLUS:   keystr.sprintf(_T("%s"), is_JpKeybd()? _T(";") : _T("＝")); break;
	case VK_OEM_COMMA:  keystr.USET_T(",");		break;
	case VK_OEM_PERIOD: keystr.USET_T(".");		break;
	case VK_OEM_102:    keystr.USET_T("＼");	break;

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
//キーとシフト状態を文字列に
//---------------------------------------------------------------------------
UnicodeString get_KeyStr(WORD Key, TShiftState Shift)
{
	UnicodeString keystr = get_KeyStr(Key);
	return !keystr.IsEmpty()? get_ShiftStr(Shift) + keystr : EmptyStr;
}
//---------------------------------------------------------------------------
