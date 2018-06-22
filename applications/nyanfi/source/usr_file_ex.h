//----------------------------------------------------------------------//
// ファイル操作 (260文字以上のパスに対応)								//
//																		//
//----------------------------------------------------------------------//
#ifndef UsrFileExH
#define UsrFileExH

#include <vcl.h>

//---------------------------------------------------------------------------
extern UnicodeString ExePath;

//---------------------------------------------------------------------------
UnicodeString cv_ex_filename(UnicodeString fnam);
UnicodeString cv_env_var(UnicodeString s);
UnicodeString cv_env_str(UnicodeString s);
UnicodeString get_actual_path(UnicodeString pnam);
UnicodeString get_actual_name(UnicodeString fnam);
UnicodeString exclude_env_path(UnicodeString fnam);
UnicodeString to_relative_name(UnicodeString fnam);
UnicodeString rel_to_absdir(UnicodeString fnam, UnicodeString rnam = EmptyStr);
bool is_same_file(UnicodeString fnam1, UnicodeString fnam2, UnicodeString rnam = EmptyStr);

UnicodeString get_root_name(UnicodeString pnam);
UnicodeString get_case_name(UnicodeString fnam);

bool is_root_dir(UnicodeString dnam);
UnicodeString exclede_delimiter_if_root(UnicodeString dnam);

UnicodeString get_drive_str(UnicodeString dnam);
bool drive_exists(UnicodeString drv);
UINT get_drive_type(UnicodeString dnam);

UnicodeString get_volume_info(UnicodeString dnam, UnicodeString *fsys = NULL);
int get_ClusterSize(UnicodeString dnam);

UnicodeString get_base_name(UnicodeString fnam);
UnicodeString get_extension(UnicodeString fnam);
UnicodeString get_extension_if_file(UnicodeString fnam);

bool test_FileExt(UnicodeString fext, UnicodeString list);
bool test_FileExt(UnicodeString fext, const _TCHAR *list);

UnicodeString to_path_name(UnicodeString dnam);
UnicodeString get_dir_name(UnicodeString dnam);
UnicodeString get_parent_path(UnicodeString dnam);
TStringDynArray split_path(UnicodeString pnam, UnicodeString dlmt = "\\");

bool is_computer_name(UnicodeString pnam);

bool is_dir_accessible(UnicodeString dnam);
bool is_drive_accessible(UnicodeString dnam);
bool is_drive_protected(UnicodeString dnam);

int  file_GetAttr(UnicodeString fnam);
bool file_SetAttr(UnicodeString fnam, int attr);
bool dir_CopyAttr(UnicodeString src_nam, UnicodeString dst_nam, bool remove_ro = false);
bool set_FileWritable(UnicodeString fnam);
bool is_SymLink(UnicodeString fnam);
int  get_HardLinkCount(UnicodeString fnam);

bool file_exists(UnicodeString fnam);
bool file_exists_x(UnicodeString fnam);
bool dir_exists(UnicodeString dnam);

TDateTime utc_to_DateTime(FILETIME *utc);
TDateTime get_file_age(UnicodeString fnam);
__int64 get_file_size(UnicodeString fnam);

UnicodeString chk_cre_dir(UnicodeString dnam);

bool is_EmptyDir(UnicodeString dnam, bool no_file = false);

int get_available_drive_list(TStringList *lst);
UnicodeString get_file_attr_str(int atr);

void get_files(UnicodeString pnam, const _TCHAR *mask, TStrings *lst, bool subSW = false, bool empSW = false);

int  get_all_files_ex(UnicodeString pnam, UnicodeString mask, TStrings *lst,
	bool subSW = false, int subn = 99, UnicodeString skip_dir = EmptyStr,
	bool empSW = false, bool inc_hide = true, bool inc_sys = true);

void get_dirs(UnicodeString pnam, TStrings *lst);

bool set_file_age(UnicodeString fnam, TDateTime dt, bool force = false);
bool create_EmptyFile(UnicodeString fnam);
bool create_Dir(UnicodeString dnam);
bool create_ForceDirs(UnicodeString dnam);
bool delete_Dir(UnicodeString dnam);
bool delete_Dirs(UnicodeString pnam);
bool move_File(UnicodeString old_nam, UnicodeString new_nam);
bool copy_File(UnicodeString src_nam, UnicodeString dst_nam);
bool rename_File(UnicodeString old_nam, UnicodeString new_nam);
bool rename_Path(UnicodeString old_nam, UnicodeString new_nam);

//---------------------------------------------------------------------------
#ifndef REPARSE_DATA_BUFFER
typedef struct _REPARSE_DATA_BUFFER {
	ULONG  ReparseTag;
	USHORT ReparseDataLength;
	USHORT Reserved;

	union {
		struct {
		  USHORT SubstituteNameOffset;
		  USHORT SubstituteNameLength;
		  USHORT PrintNameOffset;
		  USHORT PrintNameLength;
		  ULONG  Flags;
		  WCHAR  PathBuffer[1];
		} SymbolicLinkReparseBuffer;

		struct {
		  USHORT SubstituteNameOffset;
		  USHORT SubstituteNameLength;
		  USHORT PrintNameOffset;
		  USHORT PrintNameLength;
		  WCHAR  PathBuffer[1];
		} MountPointReparseBuffer;

		struct {
		  UCHAR DataBuffer[1];
		} GenericReparseBuffer;
	};
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;
#endif
//---------------------------------------------------------------------------
UnicodeString get_ReparsePointTarget(UnicodeString mnam, unsigned int &tag);

//---------------------------------------------------------------------------
#endif
