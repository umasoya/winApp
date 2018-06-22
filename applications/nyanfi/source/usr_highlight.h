//----------------------------------------------------------------------//
// 構文強調表示															//
//																		//
//----------------------------------------------------------------------//
#ifndef UsrHighlightH
#define UsrHighlightH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <tchar.h>
#include "UIniFile.h"

//---------------------------------------------------------------------------
#define FEXT_C_SRC	_T(".c.cc.cpp.cxx")
#define FEXT_C_HDR	_T(".h.hh.hpp.hxx")
#define FEXT_C_SH	FEXT_C_SRC FEXT_C_HDR
#define FEXT_PASCAL	_T(".pas.dpr.dpk.inc")

#define FEXT_PROGRAM	FEXT_C_SH FEXT_PASCAL _T(".ahk.as.asm.awk.bas.bsh.cgi.cs.d.fs.go.hs.hsp.inc.java.js.jsx.kt.kts.lisp.lsp.lua.mac.php.pl.pm.ps1.psm1.py.qml.rb.rc.drc.rs.sh.swift.scala.sql.st.vb.vbs.v.vhd.PspScript")
															//プログラム

#define FEXT_HTML	_T(".htm.html.sht.shtm.shtml.hta.xhtml.xht.hhc.hhk.php")
															//HTML文書
#define FEXT_XML	_T(".xml.xsd.xsl.xslt.dtd.svg.rdf.manifest.library-ms.cdxml.ps1xml.ptxml.csproj.vbproj.vcxproj.cbproj.xaml")
															//XML

#define FEXT_NRM_REM	FEXT_C_SH _T(".as.cs.css.d.go.idl.java.js.jsx.kt.qml.rc.rs.scala.swift.v")	//コメントが // or /*～*/
#define FEXT_SMCL_REM	_T(".as.asm.h51.h32.hsp.nbt.qbt")	//コメントが ;

//---------------------------------------------------------------------------
// 構文強調表示定義ファイルのクラス
//---------------------------------------------------------------------------
class HighlightFile : public UsrIniFile
{
private:
	TDateTime HighlightTime;	//現在のタイムスタンプ

	void CheckValues();

public:
	UnicodeString CurSection;	//使用中のセクション
	TStringList   *ErrorList;

	HighlightFile(UnicodeString fnam);
	~HighlightFile();

	UnicodeString ReadKeyStr(const _TCHAR *key);
	int			  ReadKeyInt(const _TCHAR *key, int def = 0);
	bool		  ReadKeyBool(const _TCHAR *key, bool def = false);
	UnicodeString ReadRexExPtn(const _TCHAR *key);
	TColor		  ReadColorRGB6H(const _TCHAR *key, TColor def = clBlack);

	bool Recycle();

	bool GetSection(UnicodeString fnam,
			bool is_clip = false, bool is_log = false, bool is_h2t = false);

	bool GetCommentList(UnicodeString fnam,
			TStringList *ln_lst, TStringList *bgn_lst, TStringList *end_lst,
			bool is_h2t = false, bool only_def = false);

	int  GetCommentPos(UnicodeString fnam, TListBox *lp, int idx, bool en_mlt = true);

	UnicodeString GetHeadlinePtn(UnicodeString fnam, TStringList *hl_lst = NULL, bool only_def = false);
};
//---------------------------------------------------------------------------
extern HighlightFile *UserHighlight;

//---------------------------------------------------------------------------
UnicodeString GetDefReservedPtn(UnicodeString fext, bool &sns_case, bool is_xml, bool is_log, bool is_h2t);
UnicodeString GetDefNumericPtn(UnicodeString fext);
UnicodeString GetDefSymbolChars(UnicodeString fext, bool is_xml, bool is_h2t);
UnicodeString GetDefQuotChars(UnicodeString fext, bool &use_esc, bool is_xml, bool is_ini, bool is_h2t);
UnicodeString GetDefFunctionPtn(UnicodeString fext, UnicodeString &name_ptn, bool is_h2t);
bool GetDefaultHighlight(UnicodeString fext, TStringList *lst, TStringList *hl_lst = NULL);
//---------------------------------------------------------------------------
#endif
