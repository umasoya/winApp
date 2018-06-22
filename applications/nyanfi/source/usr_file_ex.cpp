//----------------------------------------------------------------------//
// ファイル操作 (260文字以上のパスに対応)								//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <winioctl.h>
#include <memory>
#include <System.StrUtils.hpp>
#include "usr_str.h"
#include "usr_file_ex.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
UnicodeString ExePath;		//起動パス

//---------------------------------------------------------------------------
//260文字以上のパス名に対応するためのファイル名変換
//---------------------------------------------------------------------------
UnicodeString cv_ex_filename(UnicodeString fnam)
{
	if (fnam.Length()>=MAX_PATH) {
		if (USTARTS_TS("\\\\", fnam)) fnam.Insert("?\\UNC\\", 3); else fnam.Insert("\\\\?\\", 1);
	}
	return fnam;
}

//---------------------------------------------------------------------------
//環境変数を変換
//---------------------------------------------------------------------------
UnicodeString cv_env_var(UnicodeString s)
{
	for (;;) {
		if (!contains_s(s, _T('%'))) break;
		UnicodeString estr = get_tkn_m(s, '%', '%');
		if (!ContainsText(s, "%" + estr + "%")) break;
		UnicodeString eval = GetEnvironmentVariable(estr);  if (eval.IsEmpty()) break;
		s = ReplaceText(s, "%" + estr + "%", eval);
	}

	return s;
}

//---------------------------------------------------------------------------
//環境変数を含む文字列を変換
//---------------------------------------------------------------------------
UnicodeString cv_env_str(UnicodeString s)
{
	s = ReplaceText(s, "%ExePath%", ExePath);
	s = cv_env_var(s);
	return s;
}

//---------------------------------------------------------------------------
//環境変数や書式文字列を含む文字列から実際のパス名を取得
//---------------------------------------------------------------------------
UnicodeString get_actual_path(UnicodeString pnam)
{
	pnam = cv_env_str(pnam);

	if (contains_s(pnam, _T('$'))) {
		pnam = REPLACE_TS(pnam, "$$", "\f");	//$$ を一時的に \f に変えておいて
		pnam = ReplaceStr(pnam, "$X", ExcludeTrailingPathDelimiter(ExePath));
		pnam = ReplaceStr(pnam, "$D", ExtractFileDrive(ExePath));
		pnam = REPLACE_TS(pnam, "\f", "$");		//$$(\f) を $ に変換
	}
	return pnam;
}

//---------------------------------------------------------------------------
//パスが通っている場合、パス無しファイル名をパス付きに変換
//---------------------------------------------------------------------------
UnicodeString get_actual_name(UnicodeString fnam)
{
	fnam = cv_env_str(fnam);

	if (!fnam.IsEmpty() && fnam.Pos('\\')==0) {
		bool chk_ext = get_extension(fnam).IsEmpty();
		TStringDynArray plst = split_strings_semicolon(GetEnvironmentVariable("PATH"));
		TStringDynArray xlst = split_strings_semicolon(GetEnvironmentVariable("PATHEXT"));
		UnicodeString rnam;
		for (int i=0; i<plst.Length && rnam.IsEmpty(); i++) {
			UnicodeString pnam = IncludeTrailingPathDelimiter(plst[i]);
			if (chk_ext) {
				for (int j=0; j<xlst.Length && rnam.IsEmpty(); j++) {
					UnicodeString anam = pnam + fnam + xlst[j];
					if (file_exists(anam)) rnam = anam;
				}
			}
			else if (file_exists(pnam + fnam))
				rnam = pnam + fnam;
		}
		if (!rnam.IsEmpty()) fnam = rnam;
	}

	return fnam;
}

//---------------------------------------------------------------------------
//パスが通っている場合、ファイル名からパスと拡張子を取り除く
//---------------------------------------------------------------------------
UnicodeString exclude_env_path(UnicodeString fnam)
{
	if (fnam.IsEmpty() || ExtractFileDrive(fnam).IsEmpty()) return fnam;

	UnicodeString fext = get_extension(fnam);
	if (fext.IsEmpty()) return fnam;

	//拡張子をチェック
	TStringDynArray xlst = split_strings_semicolon(GetEnvironmentVariable("PATHEXT"));
	bool ok = false;
	for (int i=0; i<xlst.Length && !ok; i++) ok = SameText(fext, xlst[i]);

	//パスをチェック
	if (ok) {
		TStringDynArray plst = split_strings_semicolon(GetEnvironmentVariable("PATH"));
		UnicodeString pnam = ExtractFilePath(fnam);
		for (int i=0; i<plst.Length; i++) {
			if (SameText(IncludeTrailingPathDelimiter(plst[i]), pnam)) {
				fnam = ChangeFileExt(ExtractFileName(fnam), EmptyStr);
				break;
			}
		}
	}

	return fnam;
}


//---------------------------------------------------------------------------
//実行ファイルのディレクトリからの相対ファイル名に変換
//---------------------------------------------------------------------------
UnicodeString to_relative_name(UnicodeString fnam)
{
	remove_top_text(fnam, ExePath);
	return fnam;
}

//---------------------------------------------------------------------------
//相対パスから絶対パスへの変換
//  ドライブ名が指定されていたら、変換せずにそのままパスを返す
//---------------------------------------------------------------------------
UnicodeString rel_to_absdir(
	UnicodeString fnam,		//変換対象ファイル名
	UnicodeString rnam)		//基準ディレクトリ名 (default = EmptyStr/ ExePath);
{
	if (fnam.IsEmpty() || !ExtractFileDrive(fnam).IsEmpty()) return fnam;

	if (rnam.IsEmpty()) rnam = ExePath;

	for (;;) {
		if (remove_top_text(fnam, _T(".\\"))) continue;
		if (!remove_top_text(fnam, _T("..\\"))) break;
		rnam = get_parent_path(rnam);
	}

	return contains_PathDlmtr(rnam)? rnam + fnam : EmptyStr;
}

//---------------------------------------------------------------------------
//同一ファイルか?
//---------------------------------------------------------------------------
bool is_same_file(UnicodeString fnam1, UnicodeString fnam2,
	UnicodeString rnam)		//基準ファイル名 (default = EmptyStr/ ExePath);
{
	fnam1 = rel_to_absdir(exclude_quot(fnam1), rnam);
	fnam2 = rel_to_absdir(exclude_quot(fnam2), rnam);
	return SameText(fnam1, fnam2);
}

//---------------------------------------------------------------------------
//ルート名(ドライブ名またはUNCのコンピュータ名)を取得
//---------------------------------------------------------------------------
UnicodeString get_root_name(UnicodeString pnam)
{
	UnicodeString rstr = pnam;
	if (remove_top_text(rstr, _T("\\\\")))
		rstr = "\\\\" + get_tkn(rstr, '\\');
	else
		rstr = ExtractFileDrive(rstr);
	return IncludeTrailingPathDelimiter(rstr);
}

//---------------------------------------------------------------------------
//実際の大小文字でファイル名を取得
//---------------------------------------------------------------------------
UnicodeString get_case_name(UnicodeString fnam)
{
	UnicodeString drv_nam = IncludeTrailingPathDelimiter(ExtractFileDrive(fnam));
	UnicodeString ret_str = drv_nam;
	UnicodeString pnam;
	TStringDynArray plst = split_path(fnam);
	for (int i=0; i<plst.Length-1; i++) {
		pnam += plst[i] + "\\";
		if (USAME_TS(pnam, "\\") || USAME_TS(pnam, "\\\\")) continue;
		UnicodeString sea_str = pnam + plst[i + 1];
		if (SameText(IncludeTrailingPathDelimiter(sea_str), drv_nam)) continue;
		if (sea_str.Length()>=MAX_PATH) sea_str.Insert("\\\\?\\", 1);
		TSearchRec sr;
		if (FindFirst(sea_str, faAnyFile, sr)==0) {
			ret_str += sr.Name;
			if (sr.Attr & faDirectory) ret_str = IncludeTrailingPathDelimiter(ret_str);
			FindClose(sr);
		}
		else {
			//見つからなかったらそのまま返す
			ret_str = fnam;
			break;
		}
	}
	return ret_str;
}

//---------------------------------------------------------------------------
//ルートディレクトリか?
//---------------------------------------------------------------------------
bool is_root_dir(UnicodeString dnam)
{
	if (remove_top_s(dnam, _T("\\\\")))
		return !contains_PathDlmtr(ExcludeTrailingPathDelimiter(get_tkn_r(dnam, '\\')));
	else
		return !contains_PathDlmtr(ExcludeTrailingPathDelimiter(dnam));
}

//---------------------------------------------------------------------------
//ディレクトリがルートの場合、末尾の \ を取り除く
//---------------------------------------------------------------------------
UnicodeString exclede_delimiter_if_root(UnicodeString dnam)
{
	if (!dnam.IsEmpty()) {
		dnam = is_root_dir(dnam) ? ExcludeTrailingPathDelimiter(dnam)
								 : IncludeTrailingPathDelimiter(dnam);
	}
	return dnam;
}

//---------------------------------------------------------------------------
//ドライブ名(大文字、\付き)を取得
//---------------------------------------------------------------------------
UnicodeString get_drive_str(UnicodeString dnam)
{
	return IncludeTrailingPathDelimiter(ExtractFileDrive(dnam)).UpperCase();
}

//---------------------------------------------------------------------------
//ドライブが有効か?
//---------------------------------------------------------------------------
bool drive_exists(UnicodeString drv)
{
	drv = get_tkn(drv, ':');	if (drv.IsEmpty())	 return false;
	int d_n = drv[1] - _T('A');	if (d_n<0 || d_n>25) return false;
	DWORD d_flag = 0x00000001 << d_n;
	DWORD d_bit = ::GetLogicalDrives();	//利用可能なドライブをビットマスク形式で取得
	return (d_bit & d_flag);
}

//---------------------------------------------------------------------------
//ドライブの種類を取得
//---------------------------------------------------------------------------
UINT get_drive_type(UnicodeString dnam)
{
	return ::GetDriveType(get_drive_str(dnam).c_str());
}

//---------------------------------------------------------------------------
//ボリューム情報を取得
//戻り値 ボリューム名
//---------------------------------------------------------------------------
UnicodeString get_volume_info(UnicodeString dnam,
	UnicodeString *fsys)	//[o] ファイルシステム
{
	UnicodeString ret_str;
	_TCHAR vol_nam[MAX_PATH];
	_TCHAR fil_sys[MAX_PATH];
	DWORD VolSerialNo, MaxCompLen, Flags;
	if (::GetVolumeInformation(get_drive_str(dnam).c_str(),
		vol_nam, sizeof(vol_nam), &VolSerialNo, &MaxCompLen, &Flags, fil_sys, sizeof(fil_sys)))
	{
		ret_str = vol_nam;
		if (fsys) *fsys = fil_sys;
	}
	return ret_str;
}

//---------------------------------------------------------------------------
//クラスタサイズを取得
//---------------------------------------------------------------------------
int get_ClusterSize(UnicodeString dnam)
{
	DWORD SecPerCl, BytePerSec, FreeCl, TotalCl;
	if (::GetDiskFreeSpace(get_drive_str(dnam).c_str(), &SecPerCl, &BytePerSec, &FreeCl, &TotalCl))
		return (BytePerSec * SecPerCl);
	else
		return 0;
}

//---------------------------------------------------------------------------
//ファイルのベース名を取得
//---------------------------------------------------------------------------
UnicodeString get_base_name(UnicodeString fnam)
{
	fnam = ExtractFileName(fnam);
	if (fnam.IsEmpty()) return EmptyStr;
	return USTARTS_TS('.', fnam)? "." + ChangeFileExt(exclude_top(fnam), EmptyStr) : ChangeFileExt(fnam, EmptyStr);
}
//---------------------------------------------------------------------------
//拡張子を取得 (ドットファイルを考慮)
//---------------------------------------------------------------------------
UnicodeString get_extension(UnicodeString fnam)
{
	fnam = ExtractFileName(fnam);
	if (fnam.IsEmpty()) return EmptyStr;
	return ExtractFileExt(USTARTS_TS('.', fnam)? exclude_top(fnam) : fnam);
}
//---------------------------------------------------------------------------
//ファイルなら拡張子を取得
//---------------------------------------------------------------------------
UnicodeString get_extension_if_file(UnicodeString fnam)
{
	return dir_exists(fnam)? EmptyStr : get_extension(fnam);
}

//---------------------------------------------------------------------------
//拡張子のチェック
//---------------------------------------------------------------------------
bool test_FileExt(UnicodeString fext, UnicodeString list)
{
	if (list.IsEmpty() || fext.IsEmpty() || USAME_TS(fext, ".")) return false;
	if (USAME_TS(list, "*") || USAME_TS(list, ".*")) return true;
	if (!StartsStr(".", fext)) fext.Insert(".", 1);
	if (!EndsStr('.', fext))   fext.UCAT_T(".");
	if (!StartsStr(".", list)) list.Insert(".", 1);
	if (!EndsStr('.', list))   list.UCAT_T(".");
	return ContainsText(list, fext);
}
//---------------------------------------------------------------------------
bool test_FileExt(UnicodeString fext, const _TCHAR *list)
{
	return test_FileExt(fext, UnicodeString(list));
}

//---------------------------------------------------------------------------
//ディレクトリ名の末尾に \\ を (EmptyStr の場合は EmptyStr を返す)
//---------------------------------------------------------------------------
UnicodeString to_path_name(UnicodeString dnam)
{
	return (!dnam.IsEmpty()? IncludeTrailingPathDelimiter(dnam) : EmptyStr);
}

//---------------------------------------------------------------------------
//ディレクトリ名(末尾の \\ 無し)を取得
//---------------------------------------------------------------------------
UnicodeString get_dir_name(UnicodeString dnam)
{
	return ExtractFileName(ExcludeTrailingPathDelimiter(dnam));
}

//---------------------------------------------------------------------------
//親ディレクトリを取得
//---------------------------------------------------------------------------
UnicodeString get_parent_path(UnicodeString dnam)
{
	return ExtractFilePath(ExcludeTrailingPathDelimiter(dnam));
}

//---------------------------------------------------------------------------
//パスを分割
//---------------------------------------------------------------------------
TStringDynArray split_path(
	UnicodeString pnam,
	UnicodeString dlmt)		//区切り	(default = "\\")
{
	pnam = ExcludeTrailingPathDelimiter(pnam);
	UnicodeString top_s = remove_top_s(pnam, dlmt + dlmt)? dlmt + dlmt : EmptyStr;

	TStringDynArray ret_array = SplitString(pnam, dlmt);
	if (!top_s.IsEmpty() && ret_array.Length>0) ret_array[0] = top_s + ret_array[0];

	return ret_array;
}

//---------------------------------------------------------------------------
//コンピュータ名か?
//---------------------------------------------------------------------------
bool is_computer_name(UnicodeString pnam)
{
	if (!remove_top_s(pnam, _T("\\\\"))) return false;
	return (ExcludeTrailingPathDelimiter(pnam).Pos('\\')==0);
}

//---------------------------------------------------------------------------
//ディレクトリがアクセス可能か？
//---------------------------------------------------------------------------
bool is_dir_accessible(UnicodeString dnam)
{
	if (!is_drive_accessible(dnam)) return false;
	if (is_root_dir(dnam)) return true;

	if (!contains_PathDlmtr(ExcludeTrailingPathDelimiter(dnam))) dnam = to_path_name(dnam);
	if (dnam.IsEmpty()) return false;

	if (dnam.Length()>=248) {
		if (USTARTS_TS("\\\\", dnam)) dnam.Insert("?\\UNC\\", 3); else dnam.Insert("\\\\?\\", 1);
	}
	TSearchRec sr;
	if (FindFirst(UAPP_T(dnam, "*.*"), faAnyFile, sr)==0) {
		FindClose(sr);
		return true;
	}
	else
		return false;
}

//---------------------------------------------------------------------------
//ドライブがアクセス可能か？
//---------------------------------------------------------------------------
bool is_drive_accessible(UnicodeString dnam)
{
	if (dnam.IsEmpty()) return false;
	UnicodeString dstr = get_drive_str(dnam);
	if (dstr.IsEmpty()) return false;

	UINT old_mode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
	DWORD MaxCompLen, Flags;
	bool res = (::GetVolumeInformation(dstr.c_str(), NULL, 0, NULL, &MaxCompLen, &Flags, NULL, 0)!=0);
	::SetErrorMode(old_mode);

	return res;
}
//---------------------------------------------------------------------------
//ドライブがライトプロテクトされているか？
//---------------------------------------------------------------------------
bool is_drive_protected(UnicodeString dnam)
{
	bool ret = false;
	dnam = "\\\\.\\" + ExtractFileDrive(dnam);
	HANDLE hDevice = ::CreateFile(dnam.c_str(),
		GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
	if (hDevice!=INVALID_HANDLE_VALUE) {
		DWORD dwWritten;
		if (::DeviceIoControl(hDevice, IOCTL_DISK_IS_WRITABLE, NULL, 0, NULL, 0, &dwWritten, NULL)==FALSE &&
			GetLastError()==ERROR_WRITE_PROTECT)
				ret = true;
		::CloseHandle(hDevice);
	}
	return ret;
}

//---------------------------------------------------------------------------
//ファイルの属性を取得
//---------------------------------------------------------------------------
int file_GetAttr(UnicodeString fnam)
{
	try {
		return FileGetAttr(cv_ex_filename(fnam));
	}
	catch (...) {
		return faInvalid;
	}
}
//---------------------------------------------------------------------------
//ファイルの属性を設定
//---------------------------------------------------------------------------
bool file_SetAttr(UnicodeString fnam, int attr)
{
	try {
		return (FileSetAttr(cv_ex_filename(fnam).c_str(), attr)==0);
	}
	catch (...) {
		return false;
	}
}
//---------------------------------------------------------------------------
//ディレクトリの属性をコピー
//戻り値 false = 失敗
//dst_nam は無属性とみなす(作成直後の状態)
//---------------------------------------------------------------------------
bool dir_CopyAttr(UnicodeString src_nam, UnicodeString dst_nam,
	bool remove_ro)	//読込専用属性を解除
{
	int attr = file_GetAttr(src_nam);	if (attr==faInvalid) return false;
	if (remove_ro) attr &= ~faReadOnly;
	if ((attr & (faReadOnly|faHidden|faSysFile)) == 0) return true;
	return file_SetAttr(dst_nam, attr);
}

//---------------------------------------------------------------------------
//ファイルの読み出し専用属性を解除
//---------------------------------------------------------------------------
bool set_FileWritable(UnicodeString fnam)
{
	bool ret = true;
	if (file_exists(fnam)) {
		int atr = file_GetAttr(fnam);
		if (atr!=faInvalid && (atr & faReadOnly)) {
			atr &= ~faReadOnly;
			ret = file_SetAttr(fnam, atr);
		}
	}
	return ret;
}

//---------------------------------------------------------------------------
//リパースポイント?
//---------------------------------------------------------------------------
bool is_SymLink(UnicodeString fnam)
{
	fnam = cv_ex_filename(ExcludeTrailingPathDelimiter(fnam));
	DWORD attr = ::GetFileAttributes(fnam.c_str());
	return (attr!=0xffffffff && (attr & FILE_ATTRIBUTE_REPARSE_POINT));
}

//---------------------------------------------------------------------------
//リンクカウントを取得(2以上ならハードリンク)
//---------------------------------------------------------------------------
int get_HardLinkCount(UnicodeString fnam)
{
	int cnt = 0;

	HANDLE hFile = ::CreateFile(cv_ex_filename(fnam).c_str(),
		0, FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile!=INVALID_HANDLE_VALUE) {
		BY_HANDLE_FILE_INFORMATION fi;
		if (::GetFileInformationByHandle(hFile, &fi)) cnt = fi.nNumberOfLinks;
		::CloseHandle(hFile);
	}

    return cnt;
}


//---------------------------------------------------------------------------
//ファイルの存在チェック
//---------------------------------------------------------------------------
bool file_exists(UnicodeString fnam)
{
	if (fnam.IsEmpty()) return false;
	return (file_GetAttr(fnam) != faInvalid);
}
//---------------------------------------------------------------------------
bool file_exists_x(UnicodeString fnam)
{
	if (fnam.IsEmpty()) return false;
	int attr = file_GetAttr(fnam);

	return (attr!=faInvalid && (attr & faDirectory)==0);
}

//---------------------------------------------------------------------------
//ディレクトリの存在チェック
//---------------------------------------------------------------------------
bool dir_exists(UnicodeString dnam)
{
	if (dnam.IsEmpty()) return false;
	return ::PathIsDirectory(cv_ex_filename(dnam).c_str());
}

//---------------------------------------------------------------------------
//世界協定時刻からローカルの TDateTime へ
//---------------------------------------------------------------------------
TDateTime utc_to_DateTime(FILETIME *utc)
{
	FILETIME f_tm;
	SYSTEMTIME st;
	::FileTimeToLocalFileTime(utc, &f_tm);
	::FileTimeToSystemTime(&f_tm, &st);
	return SystemTimeToDateTime(st);
}

//---------------------------------------------------------------------------
//ファイルのタイムスタンプを TDateTime 型で取得
//---------------------------------------------------------------------------
TDateTime get_file_age(UnicodeString fnam)
{
	TDateTime ft = 0;
	HANDLE hFile = ::CreateFile(cv_ex_filename(fnam).c_str(),
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile!=INVALID_HANDLE_VALUE) {
		FILETIME c_tm, a_tm, w_tm, f_tm;
		if (::GetFileTime(hFile, &c_tm, &a_tm, &w_tm)) ft = utc_to_DateTime(&w_tm);
		::CloseHandle(hFile);
	}
	return ft;
}

//---------------------------------------------------------------------------
//タイムスタンプを設定
//---------------------------------------------------------------------------
bool set_file_age(UnicodeString fnam, TDateTime dt,
	bool force)		//読込専用でも強制的に変更 (default = false)
{
	bool res = false;

	//読込専用なら一時的に解除
	int atr = file_GetAttr(fnam);
	bool chg_ro = force && atr!=faInvalid && (atr & faReadOnly);
	if (chg_ro && !set_FileWritable(fnam)) chg_ro = false;

	UnicodeString l_fnam = cv_ex_filename(fnam);
	HANDLE hFile = ::CreateFile(l_fnam.c_str(),
		GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile!=INVALID_HANDLE_VALUE) {
		SYSTEMTIME s_tm;
		FILETIME l_tm, f_tm;
		DateTimeToSystemTime(dt, s_tm);
		//システム日時 → ファイル時刻 → 世界協定時刻
		if (::SystemTimeToFileTime(&s_tm, &l_tm) && ::LocalFileTimeToFileTime(&l_tm, &f_tm)) {
			res = ::SetFileTime(hFile, NULL, NULL, &f_tm);
		}
		::CloseHandle(hFile);
	}

	//属性の元に戻す
	if (chg_ro && atr!=-1) file_SetAttr(fnam, atr);

	return res;
}

//---------------------------------------------------------------------------
//ファイルサイズを取得
//---------------------------------------------------------------------------
__int64 get_file_size(UnicodeString fnam)
{
	__int64 size = 0;
	HANDLE hFile = ::CreateFile(cv_ex_filename(fnam).c_str(),
		GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile!=INVALID_HANDLE_VALUE) {
		DWORD size_hi;
		DWORD size_lo = ::GetFileSize(hFile, &size_hi);
		size = size_hi;
		size = (size << 32) + size_lo;
		::CloseHandle(hFile);
	}
	return size;
}

//---------------------------------------------------------------------------
//指定ディレクトリが存在しない場合は作成
//---------------------------------------------------------------------------
UnicodeString chk_cre_dir(UnicodeString dnam)
{
	if (!dir_exists(dnam)) dnam = create_Dir(dnam)? IncludeTrailingPathDelimiter(dnam) : EmptyStr;
	return dnam;
}

//---------------------------------------------------------------------------
//空ディレクトリか？
//---------------------------------------------------------------------------
bool is_EmptyDir(UnicodeString dnam, 
	bool no_file)	//true = ファイルが含まれていない(サイズ=0) (default = false);
{
	bool e_flag = true;
	dnam = IncludeTrailingPathDelimiter(dnam);
	TSearchRec sr;
	if (FindFirst(cv_ex_filename(UAPP_T(dnam, "*")), faAnyFile, sr)==0) {
		do {
			if (ContainsStr("..", sr.Name)) continue;
			if (no_file && (sr.Attr & faDirectory))
				e_flag = is_EmptyDir(dnam + sr.Name, no_file);
			else
				e_flag = false;
		} while(FindNext(sr)==0 && e_flag);
		FindClose(sr);
	}
	return e_flag;
}

//---------------------------------------------------------------------------
//利用可能なドライブのリストを取得
//---------------------------------------------------------------------------
int get_available_drive_list(TStringList *lst)
{
	lst->Clear();
	DWORD d_bit = ::GetLogicalDrives();	//利用可能なドライブをビットマスク形式で取得
	DWORD d_flag = 0x00000001;
	UINT old_mode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
	for (int d_n=_T('A'); d_n <= _T('Z'); d_n++,d_flag<<=1) {
		if ((d_bit & d_flag)==0) continue;
		UnicodeString drvnam; drvnam.sprintf(_T("%c:\\"), d_n);
		if (is_drive_accessible(drvnam)) lst->Add(drvnam);
	}
	::SetErrorMode(old_mode);
	return lst->Count;
}

//---------------------------------------------------------------------------
//ファイルの属性文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_file_attr_str(int atr)
{
	UnicodeString atr_str = "____";
	if (atr!=faInvalid) {
		if (atr & faReadOnly)	atr_str[1] = 'R';
		if (atr & faHidden)		atr_str[2] = 'H';
		if (atr & faSysFile)	atr_str[3] = 'S';
		if (atr & faArchive)	atr_str[4] = 'A';
	}
	return atr_str;
}

//---------------------------------------------------------------------------
//ディレクトリ下のマスクに該当する全ファイルを取得
//---------------------------------------------------------------------------
void get_files(
	UnicodeString pnam,	//ディレクトリ名
	const _TCHAR *mask,	//マスク
	TStrings *lst,		//結果格納リスト
	bool subSW,			//サブディレクトリも	 (default = false)
	bool empSW)			//空のサブディレクトリも (default = false)
{
	if (!dir_exists(pnam)) return;

	pnam = IncludeTrailingPathDelimiter(pnam);

	UnicodeString mask_u = def_if_empty(mask, _T("*.*"));

	//サブディレクトリを検索
	TSearchRec sr;
	if (subSW) {
		if (FindFirst(cv_ex_filename(UAPP_T(pnam, "*")), faAnyFile, sr)==0) {
			do {
				if ((sr.Attr & faDirectory)==0) continue;
				if (ContainsStr("..", sr.Name)) continue;
				int scnt = get_all_files_ex(pnam + sr.Name, mask_u, lst, subSW, 99, EmptyStr, empSW);
				if (empSW && scnt==0) {
					UnicodeString snam = IncludeTrailingPathDelimiter(pnam + sr.Name);
					if (lst->IndexOf(snam)==-1) lst->Add(snam);
				}
			} while(FindNext(sr)==0);
			FindClose(sr);
		}
	}

	//ファイルを検索
	if (FindFirst(cv_ex_filename(pnam + mask_u), faAnyFile, sr)==0) {
		do {
			if (sr.Attr & faDirectory) continue;
			if (lst->IndexOf(sr.Name)==-1) lst->Add(pnam + sr.Name);
		} while(FindNext(sr)==0);
		FindClose(sr);
	}
}

//---------------------------------------------------------------------------
//ディレクトリ下のマスクに該当する全ファイルのリストを追加取得
//  戻り値: ファイル数(空ディレクトリは除く)
//---------------------------------------------------------------------------
int get_all_files_ex(
	UnicodeString pnam,		//ディレクトリ名
	UnicodeString mask,		//マスク
	TStrings *lst,			//結果を格納する TStrings
	bool subSW,				//サブディレクトリも検索			(default = fals)
	int subn,				//サブディレクトリの階層の深さ		(default = 99)
	UnicodeString skip_dir,	//除外Dirマスク(;で区切り複数可)	(default = EmptyStr)
	bool empSW,				//空のサブディレクトリも			(default = false)
	bool inc_hide,			//隠し属性も						(default = true)
	bool inc_sys)			//システム属性も					(default = true)
{
	int fcnt = 0;
	if (!dir_exists(pnam)) return fcnt;

	pnam = IncludeTrailingPathDelimiter(pnam);
	if (mask.IsEmpty()) mask = "*.*";

	//サブディレクトリを検索
	TSearchRec sr;
	if (subSW && subn>0) {
		TStringDynArray skip_dir_lst = split_strings_semicolon(skip_dir);
		if (FindFirst(cv_ex_filename(UAPP_T(pnam, "*")), faAnyFile, sr)==0) {
			do {
				if ((sr.Attr & faDirectory)==0) continue;
				if (!inc_hide && (sr.Attr & faHidden))  continue;
				if (!inc_sys  && (sr.Attr & faSysFile)) continue;
		
				UnicodeString dnam = sr.Name;
				if ((sr.Attr & faDirectory) && !ContainsStr("..", dnam)) {
					//除外ディレクトリのチェック
					bool skip = false;
					for (int i=0; i<skip_dir_lst.Length; i++) {
						UnicodeString snam = skip_dir_lst[i];
						if (str_match(skip_dir_lst[i], dnam)) { skip = true; break; }
					}

					if (!skip) {
						int scnt = get_all_files_ex(pnam + dnam, mask, lst, subSW, subn - 1, skip_dir,
													empSW, inc_hide, inc_sys);
						fcnt += scnt;
						if (empSW && scnt==0) {
							UnicodeString snam = IncludeTrailingPathDelimiter(pnam + dnam);
							if (lst->IndexOf(snam)==-1) lst->Add(snam);
						}
					}
				}
			} while(FindNext(sr)==0);
			FindClose(sr);
		}
	}

	//ファイルを検索
	if (FindFirst(cv_ex_filename(pnam + mask), faAnyFile, sr)==0) {
		do {
			if (sr.Attr & faDirectory) continue;
			if (!inc_hide && (sr.Attr & faHidden))  continue;
			if (!inc_sys  && (sr.Attr & faSysFile)) continue;

			if (lst->IndexOf(sr.Name)==-1) {
				lst->Add(pnam + sr.Name); fcnt++;
			}
		} while(FindNext(sr)==0);
		FindClose(sr);
	}

	return fcnt;
}

//---------------------------------------------------------------------------
//ディレクトリ下の全ディレクトリを取得
//---------------------------------------------------------------------------
void get_dirs(UnicodeString pnam, TStrings *lst)
{
	if (pnam.IsEmpty() || !dir_exists(pnam)) return;

	pnam = IncludeTrailingPathDelimiter(pnam);
	TSearchRec sr;
	if (FindFirst(cv_ex_filename(UAPP_T(pnam, "*")), faAnyFile, sr)==0) {
		do {
			if ((sr.Attr & faDirectory)==0 || ContainsStr("..", sr.Name)) continue;
			lst->Add(pnam + sr.Name);
			get_dirs(pnam + sr.Name, lst);
		} while(FindNext(sr)==0);
		FindClose(sr);
	}
}

//---------------------------------------------------------------------------
//空のファイルを作成
//---------------------------------------------------------------------------
bool create_EmptyFile(UnicodeString fnam)
{
	HANDLE hFile = ::CreateFile(cv_ex_filename(fnam).c_str(),
		GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile==INVALID_HANDLE_VALUE) return false;
	::CloseHandle(hFile);
	return true;
}

//---------------------------------------------------------------------------
//ディレクトリの作成
//---------------------------------------------------------------------------
bool create_Dir(UnicodeString dnam)
{
	if (dnam.Length()>=248) {
		if (USTARTS_TS("\\\\", dnam)) dnam.Insert("?\\UNC\\", 3); else dnam.Insert("\\\\?\\", 1);
	}
	return ::CreateDirectory(dnam.c_str(), NULL);
}
//---------------------------------------------------------------------------
//ディレクトリの作成(複数階層可)
//---------------------------------------------------------------------------
bool create_ForceDirs(UnicodeString dnam)
{
	TStringDynArray plst = split_path(dnam);
	UnicodeString rnam = get_root_name(dnam);
	UnicodeString pnam;
	for (int i=0; i<plst.Length; i++) {
		pnam += UAPP_T(plst[i], "\\");
		if (USAME_TS(pnam, "\\") || USAME_TS(pnam, "\\\\") || SameStr(pnam, rnam)) continue;
		if (dir_exists(pnam)) continue;
		if (!create_Dir(pnam)) return false;
	}
	return true;
}

//---------------------------------------------------------------------------
//ディレクトリの削除
//---------------------------------------------------------------------------
bool delete_Dir(UnicodeString dnam)
{
	return ::RemoveDirectory(cv_ex_filename(dnam).c_str());
}
//---------------------------------------------------------------------------
//サブディレクトリを含めたディレクトリの削除
//---------------------------------------------------------------------------
bool delete_Dirs(UnicodeString pnam)
{
	pnam = to_path_name(pnam);
	if (!dir_exists(pnam)) return false;

	//サブディレクトリを検索して削除
	TSearchRec sr;
	if (FindFirst(cv_ex_filename(UAPP_T(pnam, "*")), faAnyFile, sr)==0) {
		bool ok = true;
		do {
			if ((sr.Attr & faDirectory)==0 || ContainsStr("..", sr.Name)) continue;
			ok = delete_Dirs(pnam + sr.Name);
		} while(FindNext(sr)==0 && ok);
		FindClose(sr);
	}

	//ディレクトリを削除
	if (!set_FileWritable(pnam)) return false;
	return delete_Dir(pnam);
}

//---------------------------------------------------------------------------
//ファイルの移動
//---------------------------------------------------------------------------
bool move_File(UnicodeString old_nam, UnicodeString new_nam)
{
	return ::MoveFileEx(cv_ex_filename(old_nam).c_str(), cv_ex_filename(new_nam).c_str(),
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);
}
//---------------------------------------------------------------------------
//ファイルのコピー
//---------------------------------------------------------------------------
bool copy_File(UnicodeString src_nam, UnicodeString dst_nam)
{
	return ::CopyFile(cv_ex_filename(src_nam).c_str(), cv_ex_filename(dst_nam).c_str(), FALSE);
}

//---------------------------------------------------------------------------
//ファイル名前の変更
//---------------------------------------------------------------------------
bool rename_File(UnicodeString old_nam, UnicodeString new_nam)
{
	return ::MoveFile(cv_ex_filename(old_nam).c_str(), cv_ex_filename(new_nam).c_str());
}

//---------------------------------------------------------------------------
//パス名の変更
//---------------------------------------------------------------------------
bool rename_Path(UnicodeString old_nam, UnicodeString new_nam)
{
	if (!SameText(get_root_name(old_nam),  get_root_name(new_nam))) return false;

	TStringDynArray o_plst = split_path(old_nam);
	TStringDynArray n_plst = split_path(new_nam);
	if (o_plst.Length!=n_plst.Length) return false;

	UnicodeString cur_dir = GetCurrentDir();

	bool ok = true;
	UnicodeString o_pnam, n_pnam;
	for (int i=0; i<o_plst.Length && ok; i++) {
		o_pnam += UAPP_T(o_plst[i], "\\");
		n_pnam += UAPP_T(n_plst[i], "\\");
		if (USAME_TS(o_pnam, "\\") || USAME_TS(o_pnam, "\\\\") || USAME_TS(n_pnam, "\\") || USAME_TS(n_pnam, "\\\\")) continue;
		if (!SameText(o_plst[i], n_plst[i])) {
			SetCurrentDir(get_parent_path(o_pnam));
			ok = rename_File(o_pnam, n_pnam);
		}
		o_pnam = n_pnam;
	}

	SetCurrentDir(cur_dir);

	return ok;
}

//---------------------------------------------------------------------------
//パースポイントの参照先を取得
//---------------------------------------------------------------------------
UnicodeString get_ReparsePointTarget(UnicodeString mnam, unsigned int &tag)
{
	HANDLE hDir = ::CreateFile(mnam.c_str(), GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_OPEN_REPARSE_POINT|FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hDir==INVALID_HANDLE_VALUE) return EmptyStr;

	UnicodeString pnam;
	char dat_buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
	::ZeroMemory(dat_buf, MAXIMUM_REPARSE_DATA_BUFFER_SIZE);
	DWORD dat_sz;
	if (::DeviceIoControl(hDir, FSCTL_GET_REPARSE_POINT,
		NULL, 0, dat_buf, MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dat_sz, NULL))
	{
		REPARSE_DATA_BUFFER *rp_buf = (_REPARSE_DATA_BUFFER*)dat_buf;
		if (IsReparseTagMicrosoft(rp_buf->ReparseTag)) {
			tag = rp_buf->ReparseTag;
			switch (tag) {
			case IO_REPARSE_TAG_MOUNT_POINT:	//ジャンクション
				pnam = (WCHAR*)rp_buf->MountPointReparseBuffer.PathBuffer
						+ rp_buf->MountPointReparseBuffer.SubstituteNameOffset/sizeof(WCHAR);
				break;
			case IO_REPARSE_TAG_SYMLINK:		//シンボリックリンク
				pnam = (WCHAR*)rp_buf->SymbolicLinkReparseBuffer.PathBuffer
						+ rp_buf->SymbolicLinkReparseBuffer.SubstituteNameOffset/sizeof(WCHAR);
				break;
			}
			remove_top_s(pnam, _T("\\??\\"));
		}
	}

	::CloseHandle(hDir);

	return pnam;
}
//---------------------------------------------------------------------------
