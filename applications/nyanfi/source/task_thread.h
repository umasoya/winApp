//----------------------------------------------------------------------//
// NyanFi																//
//  �^�X�N�����X���b�h													//
//----------------------------------------------------------------------//
#ifndef TaskThreadH
#define TaskThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>

//---------------------------------------------------------------------------
#define MIN_INTERVAL	1
#define WAIT_INTERVAL	50
#define SLOW_INTERVAL	500

//---------------------------------------------------------------------------
class TaskConfig
{
public:
	int  CopyMode;
	bool CopyAll;
	UnicodeString CopyFmt;
	int  DstPosMode;
	bool KeepTime;

	TDateTime FilterTime;
	int  FilterMode;

	UnicodeString CmdStr;
	UnicodeString InfStr;

	UnicodeString CvImg_f_ext;
	int  CvImg_quality;
	int  CvImg_ycrcb;
	int  CvImg_cmp_mode;
	int  CvImg_scale_mode;
	int  CvImg_scale_prm1;
	int  CvImg_scale_prm2;
	int  CvImg_scale_opt;
	bool CvImg_not_use_prvw;
	bool CvImg_grayscale;
	TColor CvImg_mgn_color;
	int  CvImg_chg_name_mode;
	UnicodeString CvImg_chg_name_str;

	UnicodeString Bakup_inc_mask;
	UnicodeString Bakup_exc_mask;
	UnicodeString Bakup_skip_dir;
	bool Bakup_sub_sw;
	bool Bakup_mirror;

	TStringList *TaskList;
	TStringList *UpdDirList;

	TaskConfig();
	~TaskConfig();

	void Assign(TaskConfig *cp);
};

//---------------------------------------------------------------------------
//�^�X�N�ݒ胊�X�g (TList ����p��)
//---------------------------------------------------------------------------
class TaskConfigList : public TList
{
private:
	TaskConfig * __fastcall Get(int Index);
	void __fastcall Put(int Index, TaskConfig *Item);

public:
	__fastcall TaskConfigList();
	__fastcall ~TaskConfigList();

	void __fastcall ClearAll();

	__property TaskConfig * Items[int Index] = {read=Get, write=Put};
};


//---------------------------------------------------------------------------
// �^�X�N�����X���b�h
//---------------------------------------------------------------------------
class TTaskThread : public TThread
{
private:
	static DWORD CALLBACK ProgressRoutine(
		LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred,
		LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred,
		DWORD dwStreamNumber, DWORD dwCallbackReason,
		HANDLE hSourceFile, HANDLE hDestinationFile, LPVOID lpData);

	DWORD __fastcall ProgressCore(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred);

	void __fastcall FinishTask();
	void __fastcall Execute();

	int  __fastcall GetFiles(UnicodeString pnam, UnicodeString mask, TStrings *lst, bool empSW = false, bool symSW = false);

	int  __fastcall GetFilesEx(UnicodeString pnam, UnicodeString inc_mask, UnicodeString exc_mask,
						TStrings *lst, bool subSW = false, UnicodeString skip_dir = EmptyStr, TStringList *d_lst = NULL, 
						bool symSW = false, bool dateSW = false);

	void __fastcall GetDirs(UnicodeString pnam, TStrings *lst);

	void __fastcall CPY_core(UnicodeString fnam, UnicodeString dst_path,
								bool mov_sw = false, bool remove_ro = false, TStringList *skip_list = NULL);
	void __fastcall Task_CPY(UnicodeString prm, bool mov_sw = false);

	void __fastcall AddDebugLog(UnicodeString msg, UnicodeString info = EmptyStr);
	bool __fastcall EX_file_exists(UnicodeString fnam);
	bool __fastcall EX_dir_exists(UnicodeString dnam);
	bool __fastcall EX_set_writable(UnicodeString fnam);
	bool __fastcall EX_wait_file_ready(UnicodeString fnam);
	bool __fastcall EX_delete_File(UnicodeString fnam, bool use_trash = false);
	bool __fastcall EX_delete_Dir(UnicodeString dnam);

	void __fastcall DEL_core(UnicodeString fnam, bool use_trash);
	void __fastcall DEL_dir_trash(UnicodeString dnam);
	void __fastcall DEL_dirs(UnicodeString pnam, TStringList *skip_list = NULL);
	void __fastcall Task_DEL(UnicodeString prm);

	void __fastcall CMPDEL_core(UnicodeString fnam);
	void __fastcall Task_CMPDEL(UnicodeString prm);

	void __fastcall Task_CPYDIR(UnicodeString prm);

	void __fastcall Task_CVIMG(UnicodeString prm);
	void __fastcall Task_DLEXIF(UnicodeString prm);
	void __fastcall Task_EXTCHM(UnicodeString prm);

	void __fastcall Task_BACKUP(UnicodeString prm);

	TDateTime __fastcall SetDirTime(UnicodeString dnam);
	void __fastcall Task_DIRTIME(UnicodeString prm);

	void __fastcall AddLog(UnicodeString msg, bool must_log = false);

public:
	int  Tag;
	int  ID;
	bool TaskReady;
	bool TaskCancel;			//���f
	bool TaskPause;				//��U��~
	bool TaskFinished;			//�^�X�N���I��
	bool TaskAskSame;			//�����t�@�C��������v��
	bool TaskIsFast;			//�������s��
	bool ReqTaskSlow;			//�ᑬ���s��v��
	bool DirDeleted;			//�f�B���N�g�����폜���ꂽ
	bool LnkDeleted;			//�n�[�h�����N���폜���ꂽ

	int  TaskTime;
	int  PreCount, SubCount, OkCount, SkipCount, RenCount, ErrCount;
	int  StartCount;

	LARGE_INTEGER LastTransferred;
	__int64 SmplSize;
	int  LastCount;
	int  Speed;					//�]�����x(byte/ms)
	int  RemCount;				//�c�莞��(ms)
	__int64 CurTotalSize;

	UnicodeString CurFileName;
	double CurProgress;			//���݃R�s�[/�ړ����t�@�C���̐i����
	bool   IsRemote;			//�R�s�[/�ړ����E�悪�l�b�g���[�N�h���C�u

	UnicodeString CurSrcName;	//�R�s�[/�ړ���
	UnicodeString CurDstName;	//�R�s�[/�ړ���

	UnicodeString FirstDstName;	//�ŏ��ɃR�s�[/�ړ��悵���t�@�C����
	UnicodeString LastDstName;	//���̎��_�ōŌ�ɃR�s�[/�ړ��悵���t�@�C����

	TaskConfig *Config;

	bool IsSamePath;
	UnicodeString CopyName;
	UnicodeString CloneName;

	UnicodeString CmdName;

	__fastcall TTaskThread(bool CreateSuspended);
	bool __fastcall TaskStart();
};
//---------------------------------------------------------------------------
extern int TaskIdCount;

#endif
