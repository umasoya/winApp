//----------------------------------------------------------------------//
// 汎用共通関数															//
//																		//
//----------------------------------------------------------------------//
#ifndef UsrFuncH
#define UsrFuncH

//---------------------------------------------------------------------------
#include <vcl.h>
#include <shlobj.h>
#include <System.StrUtils.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.CheckLst.hpp>
#include "usr_str.h"
#include "UserMdl.h"

//---------------------------------------------------------------------------
#define URL_MATCH_PTN	_T("h?ttps?://[\\w/:%#$&?()~.=+-]+[\\w/]")		//URL用正規表現
#define MAIL_MATCH_PTN	_T("mailto:[a-zA-Z0-9]+[\\w.-]*@[\\w.-]+")		//メール用正規表現
#define LOCAL_FILE_PTN	_T("file:///[^/*?\"<>|）]+\\.[a-zA-Z0-9]+")		//ローカルファイルの正規表現

#define LISTBOX_SCRMGN	2

#define FILE_RBUF_SIZE	32768		//ファイル読み込み用バッファサイズ(32KB)

//---------------------------------------------------------------------------
extern TColor scl_Window;
extern TColor scl_WindowText;
extern TColor scl_Highlight;
extern TColor scl_HighlightText;
extern TColor scl_BtnFace;
extern TColor scl_BtnText;
extern TColor scl_Menu;
extern TColor scl_MenuText;

//---------------------------------------------------------------------------
extern const UnicodeString CodePageList;
extern const UnicodeString CodePageListX;

//---------------------------------------------------------------------------
void InitializeSysColor();

HWND get_window_from_pos();
void show_PopupMenu(TPopupMenu *mp, TControl *cp);
void show_PopupMenu(TListBox *cp);
void show_PopupMenu(TStringGrid *gp);

void set_ListBoxText(TListBox *lp, const _TCHAR *s);
void set_ComboBoxText(TComboBox *cp, const _TCHAR *s);

void add_ComboBox_history(TComboBox *cp, UnicodeString kwd = EmptyStr);
int  del_ComboBox_history(TComboBox *cp, UnicodeString kwd = EmptyStr);

void CloseIME(HWND hWnd);

TPoint get_ListItemPos(TListBox *lp);
void pos_ListBoxItem(TListBox *lp);

void UserAbort(unsigned id);
void SysErrAbort(unsigned id);
void LastErrAbort();
void TextAbort(const _TCHAR *msg);
void SkipAbort();

void msgbox_ERR(  UnicodeString msg);
void msgbox_ERR(  unsigned id);
void msgbox_WARN( UnicodeString msg);
void msgbox_WARN( unsigned id);

void msgbox_OK(   UnicodeString msg, UnicodeString tit = EmptyStr);
bool msgbox_Y_N(  UnicodeString msg, UnicodeString tit = EmptyStr);
int  msgbox_Y_N_C(UnicodeString msg, UnicodeString tit = EmptyStr);

//---------------------------------------------------------------------------
bool has_Leading(TCanvas *cv);
int  get_FontHeight(TFont *font, int mgn = 0, int min_mgn = 2);
int  get_CharWidth_Font(TFont *font, int n);

UnicodeString get_MiniPathName(UnicodeString pnam, int wd, TFont *font);

int  EditToInt(TLabeledEdit *ep, int def = 0);
int  EditToInt(TEdit *ep, int def = 0);

UnicodeString get_size_str_T(__int64 sz, int dg);
UnicodeString get_size_str_G(__int64 sz, int wd = 10, int dg = 1);
UnicodeString get_size_str_G(__int64 sz, int wd, int dg, int fmt_mode);
UnicodeString get_size_str_B(__int64 sz, int wd = 14);
UnicodeString get_size_str_C(int sz);

bool str_to_DateTime(UnicodeString s, TDateTime *dt);
UnicodeString format_DateTime(TDateTime dt, bool omit = false);
UnicodeString format_Date(TDateTime dt);
UnicodeString format_DateTimeEx(UnicodeString fmt, TDateTime dt);

int get_DateCond(UnicodeString prm, TDateTime &dt);

UnicodeString get_wd_x_hi_str(int w, int h);
UnicodeString get_img_size_str(int w, int h, UnicodeString name = EmptyStr);

UnicodeString get_NameOfCodePage(int code_page);
int get_CodePageOfName(UnicodeString code_name);

UnicodeString get_res_cnt_str(int ok_cnt = 0, int er_cnt = 0, int sk_cnt = 0, int ng_cnt = 0);

int  format_res_list(TStringList *lst, int mgn = 2, int min_wd = 16);

void ShowPropertyDialog(UnicodeString fnam);

void delete_ListItem(TCustomListBox *lp);
void draw_ListItemLine(TCustomListBox *lp, int idx);

void draw_Line(TCanvas *cv, int x0, int y0, int x1, int y1, int w, TColor col, TPenStyle = psSolid);
void draw_Separator(TCanvas *cv, TRect rc, TColor bg = Graphics::clNone);
void draw_separateLine(TCanvas *cv, TRect rc, int pos = 0);
void draw_SortMark(TCanvas *cv, int x, int y, bool is_asc, TColor fg = Graphics::clNone);

#define UBMK_UP		1
#define UBMK_DOWN	2
#define UBMK_BUP	3
#define UBMK_BDOWN	4
#define UBMK_VUP	5
#define UBMK_VDOWN	6
#define UBMK_VTOP	7
#define UBMK_VEND	8
#define UBMK_HTOP	9
#define UBMK_HEND	10
#define UBMK_LEFT	11
#define UBMK_RIGHT	12

void set_ButtonMark(TSpeedButton *bp, int id = UBMK_DOWN, TColor fg = clBtnText);

double get_ZoomRatio(int vw, int vh, int iw, int ih, int mgn = 0, double limit = 1.0);

void make_KeyList(TStrings *lst);
bool is_KeyDown(WORD key);
bool is_KeyPress_CtrlNotCV(WORD key);
bool is_KeyPress_CtrlNotCVX(WORD key);
bool is_DialogKey(WORD Key);
int  get_shift_from_wparam(WPARAM prm);

UnicodeString get_html_header(UnicodeString fnam, int code_page = 0);
void get_HtmlInf(UnicodeString fnam, TStringList *lst);

void get_JpgExInf(UnicodeString fnam, TStringList *lst);

bool get_ExifInf(UnicodeString fnam, TStringList *lst, int *ori = NULL,
	unsigned int *i_wd = NULL, unsigned int *i_hi = NULL);

UnicodeString get_ExifInfStr(UnicodeString fnam, int *ori = NULL, unsigned int *i_wd = NULL, unsigned int *i_hi = NULL);
UnicodeString get_ExifInfThumb(UnicodeString fnam, int *ori);

bool get_GpsInf(UnicodeString fnam, double *lat, double *lng);

unsigned int big_to_little(BYTE *p);
bool check_read(TFileStream *fs, const char *s);
bool sea_chunk(TFileStream *fs, const char *s);
UnicodeString get_chunk_hdr(TFileStream *fs, unsigned int *size = NULL);

void get_WavInf(UnicodeString fnam, TStringList *lst);
void get_OpusInf(UnicodeString fnam, TStringList *lst);
bool get_CdaInf(UnicodeString fnam, TStringList *lst, int *len = NULL);
void get_IconInf(UnicodeString fnam, TStringList* lst);
void get_MetafileInf(UnicodeString fnam, TStringList *lst);
bool get_PngInf(UnicodeString fnam, TStringList *lst, unsigned int *i_wd = NULL, unsigned int *i_hi = NULL);
UnicodeString get_PngInfStr(UnicodeString fnam, unsigned int *i_wd = NULL, unsigned int *i_hi = NULL);
bool get_GifInf(UnicodeString fnam, TStringList *lst, unsigned int *i_wd = NULL, unsigned int *i_hi = NULL, int *i_cnt = NULL);
bool get_WebpInf(UnicodeString fnam, TStringList *lst, unsigned int *i_wd = NULL, unsigned int *i_hi = NULL);
UnicodeString get_WebpInfStr(UnicodeString fnam, unsigned int *i_wd = NULL, unsigned int *i_hi = NULL);
void get_AppInf(UnicodeString fnam, TStringList *lst, bool check = true);
void get_BorlandInf(UnicodeString fnam, TStringList *lst);
void get_TagsInf(UnicodeString fnam, TStringList *lst);
bool get_DllExpFunc(UnicodeString fnam, TStringList *lst);

void add_list_errmsg(TStringList *lst, UnicodeString msg);

UnicodeString get_CRC32_str(UnicodeString fnam);
UnicodeString get_HashStr(UnicodeString fnam, ALG_ID algid);
UnicodeString get_HashStr(UnicodeString fnam, UnicodeString idstr);

double GetLuminance(TColor col);
TColor RatioCol(TColor col, float rt);
TColor GrayCol(TColor col);
void RgbToHsl(TColor col, int *h, int *s, int *l);
void RgbToHsv(TColor col, int *h, int *s, int *v);
TColor SelectWorB(TColor col, float rt = 1.0);
TColor AdjustColor(TColor col, int adj);
void str_to_Color(TColor &col, UnicodeString s);

void SetHighlight(TCanvas *cv, bool hl);

void set_GridFromHeader(THeaderControl *hp, TStringGrid *gp);
void set_HeaderFromGrid(TStringGrid *gp, THeaderControl *hp);
void set_HeaderSecWidth(THeaderControl *hp, int cnt, ...);
void adjust_HeaderSecWidth(THeaderControl *hp, int idx);

int  set_SttBarPanelWidth(TStatusBar *sp, int idx, int n);

void clear_GridRow(TStringGrid *gp, int row, int col = 0);
int  get_GridIndex(TStringGrid *gp, int max_count);
void set_GridIndex(TStringGrid *gp, int idx, int max_count);

void ListBoxCursorDown(TListBox *lp);
void ListBoxCursorDown(TListBox *lp, int n);
void ListBoxCursorUp(TListBox *lp);
void ListBoxCursorUp(TListBox *lp, int n);
void ListBoxPageDown(TListBox *lp);
void ListBoxPageUp(TListBox *lp);
void ListBoxTop(TListBox *lp);
void ListBoxEnd(TListBox *lp);
void ListBoxSetIndex(TListBox *lp, int idx);
bool ListBoxCsrToVisible(TListBox *lp);
void ListBoxScrollDown(TListBox *lp, int n, bool move_csr = false);
void ListBoxScrollDown(TListBox *lp, UnicodeString prm);
void ListBoxScrollUp(TListBox *lp, int n, bool move_csr = false);
void ListBoxScrollUp(TListBox *lp, UnicodeString prm);
void ListBoxSelectAll(TListBox *lp, bool sw = true);
int  ListBoxInitialSearch(TListBox *lp, UnicodeString kptn, bool fnam_sw = false);
UnicodeString ListBoxGetURL(TListBox *lp);
void set_ListBox_ItemNo(TListBox *lp);

void GridCursorLeft(TStringGrid *gp);
void GridCursorRight(TStringGrid *gp);
void GridCursorDown(TStringGrid *gp);
void GridCursorUp(TStringGrid *gp);
void GridPageDown(TStringGrid *gp);
void GridPageUp(TStringGrid *gp);
bool GridCursorMove(UnicodeString cmd , TStringGrid *gp);

int up_StrListItem(TStringList *lst, int idx);
int down_StrListItem(TStringList *lst, int idx);

void perform_Key(TControl *cp, int key);
void perform_Key_RETURN(TControl *cp);

int get_IndexFromAccKey(TStringList *lst, UnicodeString key);

void ClearKeyBuff(bool key_only = false);

UnicodeString extract_URL(UnicodeString s);
UnicodeString get_EncodedUrl(UnicodeString url);
bool InternetConnected();
int  get_OnlineFile(UnicodeString url, UnicodeString fnam, bool *cancel = NULL, TProgressBar *prg_bar = NULL);

void set_SplitterWidht(TSplitter *sp, int sp_wd);
void set_PanelAlign(TPanel *pp, TSplitter *sp, int mode,  int sp_wd);
void set_PanelAlign(TPanel *pp, TSplitter *sp, TAlign al, int sp_wd);

void set_ControlRBCorner(TControl *cp0, TControl *cp1);

UnicodeString get_BatteryPercentStr();
UnicodeString get_BatteryTimeStr();

bool make_url_file(UnicodeString fnam, UnicodeString url);

bool EjectDrive(UnicodeString drvnam, bool eject);
bool EjectDrive2(UnicodeString drvnam, bool eject);

bool class_is_LabeledEdit(TObject *op);
bool class_is_MaskEdit(TObject *op);
bool class_is_Edit(TObject *op);
bool class_is_CheckBox(TObject *op);
bool class_is_ComboBox(TObject *op);
bool class_is_ListBox(TObject *op);
bool class_is_CheckListBox(TObject *op);
bool class_is_RadioGroup(TObject *op);

void cursor_HourGlass();
void cursor_Default();;

void reduction_MenuLine(TMenuItem *mp);

//---------------------------------------------------------------------------
//ユーザメッセージ
//---------------------------------------------------------------------------
#define USTR_NotFound		1001
#define USTR_OpeNotSuported	1002
#define USTR_CantOperate	1003
#define USTR_FmtNotSuported	1004
#define USTR_FaildTmpUnpack	1005
#define USTR_SameMoveDest	1006
#define USTR_SameCopyDest	1007
#define USTR_IncludeDir		1008
#define USTR_IllegalDate	1009
#define USTR_IllegalParam	1010
#define USTR_IllegalFormat	1011
#define USTR_IllegalAddress	1012
#define USTR_FaildExec		1013
#define USTR_FaildProc		1014
#define USTR_FaildLoad		1015
#define USTR_FaildSave		1016
#define USTR_FaildDelete	1017
#define USTR_FaildCopy		1018
#define USTR_FaildMenu		1019
#define USTR_Canceled		1020
#define USTR_WlistCantOpen	1021
#define USTR_ProcBusy		1022
#define USTR_NoObject		1023
#define USTR_IllegalRegEx	1024
#define USTR_InputFmtStr	1025
#define USTR_Registered		1026
#define USTR_SearchingESC	1027
#define USTR_CalculatingESC 1028
#define USTR_WaitForReady	1029
#define USTR_FaildListFile	1030
#define USTR_CantMakeTmpDir	1031
#define USTR_SyntaxError	1032
#define USTR_BadStatmet		1033
#define USTR_NoParameter	1034
#define USTR_AppNotFound	1035
#define USTR_ImgNotReady	1036
#define USTR_FileNotOpen	1037
#define USTR_DirNotFound	1038
#define USTR_InvalidCmd		1039
#define USTR_HintMltFExt	1040
#define USTR_HintMltSepSC	1041
#define USTR_NoEditor		1042
#define USTR_CantPlay		1043
#define USTR_SaveAs			1044
#define USTR_CantCreDir		1045
#define USTR_CopyQ			1046
#define USTR_DeleteQ		1047
#define USTR_CompDeleteQ	1048
#define USTR_DelHistoryQ	1049
#define USTR_ClrAllMarkQ	1050
#define USTR_CancelCmdQ		1051
#define USTR_ArcNotOpen		1052
#define USTR_NotText		1053
#define USTR_CreateDir		1054
#define USTR_Extract		1055
#define USTR_CantGetInfo	1056
#define USTR_DuplicateName	1057
#define USTR_OverwriteQ		1058
#define USTR_UploadQ		1059
#define USTR_DownloadQ		1060
#define USTR_BeginDelete	1061
#define USTR_SameName		1062
#define USTR_Rename			1063
#define USTR_NgSameName		1064
#define USTR_IllegalDtCond	1065
#define USTR_WorkFiltered	1066

#define USTR_SelectedItem	1100
#define USTR_List			1101
#define USTR_Upload			1102
#define USTR_Download		1103

UnicodeString LoadUsrMsg(int id, UnicodeString s = EmptyStr);
UnicodeString LoadUsrMsg(int id, const _TCHAR *s);
UnicodeString LoadUsrMsg(int id, int id_s);

//---------------------------------------------------------------------------
#endif
