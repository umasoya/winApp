//----------------------------------------------------------------------//
// NyanFi																//
//  ExeCommands �p�֐�													//
//----------------------------------------------------------------------//
#ifndef UsrExeCmdsH
#define UsrExeCmdsH
//---------------------------------------------------------------------------
#include "Global.h"

//---------------------------------------------------------------------------
#define MAX_REPEAT_LEVEL 8	//Repeat���̍ő命�d���x��

//---------------------------------------------------------------------------
class ExeCmdsList
{
public:
	UnicodeString FileName;
	cmdf_rec    *CmdRec;
	TStringList *CmdList;
	int PC;
	int ListCount;
	int LineNo;
	UnicodeString CmdStr;
	bool EndOfCmds;

	int RepLevel;					//Repeat ���x�� -1:�Ȃ�
	int RepCnt[MAX_REPEAT_LEVEL];	//  �J��Ԃ���  -1:YN/ -2:����
	int RepTop[MAX_REPEAT_LEVEL];	//  �J��Ԃ��擪�ʒu
	int IfCnt;						//If����

	UnicodeString ErrMsg;

	ExeCmdsList(UnicodeString cmds);
	~ExeCmdsList();

	bool Preproc(UnicodeString &cmd, UnicodeString &prm);
	bool IncPC();
	bool proc_Repeat(UnicodeString cmd, UnicodeString prm);
	bool search_EndRepeat();
	bool proc_Goto(UnicodeString label);
	bool proc_IfCnd(bool cnd, UnicodeString prm);
	bool proc_IfElseEnd(UnicodeString cmd);
};

//---------------------------------------------------------------------------
//�⏕�R�}���h���X�g
extern const UnicodeString XCMD_SubCmds;
#define XCMDID_Add				0
#define XCMDID_AppendBuffer		1
#define XCMDID_CallCommands		2
#define XCMDID_Debug			3
#define XCMDID_Download			4
#define XCMDID_Echo				5
#define XCMDID_EchoOff			6
#define XCMDID_EchoOn			7
#define XCMDID_EchoT			8
#define XCMDID_FileExists		9
#define XCMDID_FilterBuffer		10
#define XCMDID_FlashWin			11
#define XCMDID_Format			12
#define XCMDID_FormatDI			13
#define XCMDID_FormatDT			14
#define XCMDID_FormatFN			15
#define XCMDID_GrepSetMask		16
#define XCMDID_GrepStart		17
#define XCMDID_Hint				18
#define XCMDID_Input			19
#define XCMDID_Ins				20
#define XCMDID_JumpCommands		21
#define XCMDID_ListBuffer		22
#define XCMDID_ListVar			23
#define XCMDID_LoadBuffer		24
#define XCMDID_MatchBuffer		25
#define XCMDID_MatchDir			26
#define XCMDID_MatchExt			27
#define XCMDID_MatchName		28
#define XCMDID_Mid				29
#define XCMDID_MsgBox			30
#define XCMDID_MsgBoxYN			31
#define XCMDID_MsgBoxYNC		32
#define XCMDID_MsgOff			33
#define XCMDID_Nop				34
#define XCMDID_PlaySound		35
#define XCMDID_PopIndex			36
#define XCMDID_PopTopIndex		37
#define XCMDID_PopupMenu		38
#define XCMDID_PopVar			39
#define XCMDID_PushIndex		40
#define XCMDID_PushTopIndex		41
#define XCMDID_PushVar			42
#define XCMDID_Random			43
#define XCMDID_ReadINI			44
#define XCMDID_Redraw			45
#define XCMDID_ReplaceBuffer	46
#define XCMDID_ReturnCommands	47
#define XCMDID_Right			48
#define XCMDID_SaveBuffer		49
#define XCMDID_SearchDown		50
#define XCMDID_SearchDownD		51
#define XCMDID_SearchDownF		52
#define XCMDID_SearchUp			53
#define XCMDID_SearchUpD		54
#define XCMDID_SearchUpF		55
#define XCMDID_SendCommands		56
#define XCMDID_Set				57
#define XCMDID_SetClipboard		58
#define XCMDID_SetCopyMode		59
#define XCMDID_SetExeDir		60
#define XCMDID_SetExeFile		61
#define XCMDID_SetExeOpt		62
#define XCMDID_SetExeParam		63
#define XCMDID_SetFileTime		64
#define XCMDID_SetIndex			65
#define XCMDID_ShellExecute		66
#define XCMDID_Sleep			67
#define XCMDID_Test				68
#define XCMDID_Timer			69
#define XCMDID_Trim				70
#define XCMDID_WaitForTask		71
#define XCMDID_WaitForTask2		72
#define XCMDID_Warn				73
#define XCMDID_WriteINI			74

//---------------------------------------------------------------------------
extern bool XCMD_IsBusy;
extern bool XCMD_Aborted;
extern bool XCMD_MsgOff;
extern bool XCMD_BufChanged;
extern bool XCMD_FileChanged;

extern bool XCMD_Debugging;
extern bool XCMD_Debug_Step, XCMD_Debug_Go, XCMD_Debug_List, XCMD_Debug_Exit, XCMD_Debug_Quit, XCMD_Debug_Help;

extern TStringList *XCMD_XList;
extern TStringList *XCMD_WatchList;
extern TStringList *XCMD_IdxStack;
extern TStringList *XCMD_TopIdxStack;
extern TStringList *XCMD_VarStack;

extern ExeCmdsList *XCMD_xlp;
extern UnicodeString XCMD_cmd, XCMD_prm;

extern int XCMD_last_task_id;
extern int XCMD_tim_cnt;
extern int XCMD_start_cnt;
extern TDateTime XCMD_tim_t;
extern bool XCMD_echo_on;
extern bool XCMD_echo_lno;
extern bool XCMD_view_log;
extern bool XCMD_view_clip;
extern file_rec *XCMD_cfp;

extern UnicodeString XCMD_cur_f_name, XCMD_cur_r_name;
extern UnicodeString XCMD_cur_fext, XCMD_cur_bnam, XCMD_cur_path;
extern bool XCMD_is_dir, XCMD_is_file, XCMD_selected, XCMD_is_virtual, XCMD_is_img, XCMD_is_log;
extern bool XCMD_is_bin, XCMD_is_htm;
extern bool XCMD_matched, XCMD_found, XCMD_marked;
extern bool XCMD_is_top, XCMD_is_end, XCMD_is_sel;
extern bool XCMD_fromGrep;
extern int  XCMD_box_res;
extern __int64   XCMD_f_size;
extern TDateTime XCMD_f_time;

//---------------------------------------------------------------------------
void XCMD_Initialize(UnicodeString &opt);
void XCMD_Uninitialize();

void XCMD_set_Var(const _TCHAR *name, UnicodeString v);
void XCMD_set_Var(const _TCHAR *name, int v);

ExeCmdsList *XCMD_AddCmdsList(UnicodeString cmds, bool is_call = true);
ExeCmdsList *XCMD_Return();

file_rec *XCMD_set_cfp(UnicodeString fnam, UnicodeString cnam, file_rec *cfp);
void XCMD_upd_Var();

void XCMD_EchoLn();
void XCMD_ShowDebugInf(UnicodeString err = EmptyStr);

bool XCMD_EvalCnd(TStringDynArray lst);
bool XCMD_Control();

void XCMD_match_Str(UnicodeString prm, UnicodeString s);
void XCMD_match_Buffer(UnicodeString prm);
void XCMD_MatchExt(UnicodeString prm, UnicodeString fext);

void XCMD_Set(UnicodeString prm);
void XCMD_Add(UnicodeString prm);
void XCMD_Ins(UnicodeString prm);
void XCMD_Mid(UnicodeString prm);
void XCMD_Right(UnicodeString prm);
void XCMD_Trim(UnicodeString prm);
void XCMD_Input(UnicodeString prm);
void XCMD_Random(UnicodeString prm);
bool XCMD_Test(UnicodeString prm);

int XCMD_GetCodePage();
UnicodeString XCMD_GetBuffer();
void XCMD_SetBuffer(UnicodeString s);

void XCMD_FilterBuffer(UnicodeString prm);
void XCMD_ReplaceBuffer(UnicodeString prm);
void XCMD_LoadBuffer(UnicodeString prm);
void XCMD_SaveBuffer(UnicodeString prm);
void XCMD_AppendBuffer(UnicodeString prm);

int  XCMD_MsgBox(UnicodeString cmd, UnicodeString prm);
void XCMD_PlaySound(UnicodeString prm);
void XCMD_FlashWin(UnicodeString prm);
void XCMD_get_UsrVarList(TStringList *lst);
void XCMD_ListVar();

void XCMD_Sleep(UnicodeString prm);
void XCMD_Timer(UnicodeString prm);

void XCMD_SearchDown(UnicodeString prm, TListBox *lp, TStringList *lst, bool file_only = false, bool dir_only = false);
void XCMD_SearchUp(UnicodeString prm, TListBox *lp, TStringList *lst, bool file_only = false, bool dir_only = false);
void XCMD_PushIndex(TListBox *lp);
void XCMD_PopIndex(TListBox *lp);
void XCMD_PushTopIndex(TListBox *lp);
void XCMD_PopTopIndex(TListBox *lp);
void XCMD_PushVar(UnicodeString prm);
void XCMD_PopVar(UnicodeString prm);
void XCMD_SetFileTime(UnicodeString prm);
void XCMD_ReadINI(UnicodeString prm);
void XCMD_WriteINI(UnicodeString prm);

bool XCMD_ShellExe(UnicodeString cmd, UnicodeString prm = EmptyStr, UnicodeString wdir = EmptyStr, UnicodeString opt = EmptyStr);

void XCMD_Redraw(UnicodeString prm);
//---------------------------------------------------------------------------
#endif