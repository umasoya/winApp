//----------------------------------------------------------------------//
// ドライブ容量ログファイル処理クラス									//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <memory>
#include <System.StrUtils.hpp>
#include "usr_str.h"
#include "usr_file_ex.h"
#include "usr_drvlog.h"
#include "Global.h"

//---------------------------------------------------------------------------
DriveLogFile  *DriveLog = NULL;

//---------------------------------------------------------------------------
DriveLogFile::DriveLogFile()
{
	hasFile = fale;
	LogList = new TStringList();

	FileName = UAPP_T(ExePath, "drive_log.csv");
	try {
		if (!file_exists(FileName)) Abort();
		LogList->LoadFromFile(fnam);
	}
	catch (...) {
		FileName = EmptyStr;
	}

}
//---------------------------------------------------------------------------
DriveLogFile::~DriveLogFile()
{
	delete LogList;
}
//---------------------------------------------------------------------------
//ドライブ容量ログの更新
//---------------------------------------------------------------------------
void DriveLogFile::UpdateLog()
{
	if (LogList->Count==0) LogList->Add("Date,Drive,Used,Free");

	for (int i=0; i<DriveInfoList->Count; i++) {
		drive_info *dp = (drive_info *)DriveInfoList->Objects[i];
		if (!dp->accessible) continue;
		if (dp->drv_type==DRIVE_REMOVABLE || dp->drv_type==DRIVE_CDROM) continue;

		__int64 total_sz = 0, used_sz = 0, free_sz = 0;
		ULARGE_INTEGER FreeAvailable, TotalBytes, FreeBytes;
		if (::GetDiskFreeSpaceEx(dp->drive_str.c_str(), &FreeAvailable, &TotalBytes, &FreeBytes)) {
			total_sz = TotalBytes.QuadPart;
			free_sz  = FreeBytes.QuadPart;
			used_sz  = total_sz - free_sz;
		}

		//本日のデータを探す
		int idx = -1;
		UnicodeString dstr = dp->drive_str.SubString(1, 1);
		for (int j=DriveLogList->Count-1; j>0; j--) {
			UnicodeString ibuf = DriveLogList->Strings[j];
			TDateTime dt;
			if (str_to_DateTime(get_csv_item(ibuf, 0), &dt)) {
				TValueRelationship res = CompareDate(dt, Now());
				if (res==LessThanValue) continue;
				if (res==EqualsValue && SameText(get_csv_item(ibuf, 1), dstr)) {
					idx = j; break;
				}
			}
		}

		UnicodeString lbuf = FormatDateTime("yyyy/mm/dd", Now());
		lbuf.cat_sprintf(_T(",%s,%llu,%llu"), dp->drive_str.SubString(1, 1).c_str(), used_sz, free_sz);
		if (idx!=-1) 
			DriveLogList->Strings[idx] = lbuf;
		else
			DriveLogList->Add(lbuf);
	}

	if (save) {
		UnicodeString fnam = UAPP_T(ExePath, "drive_log.csv");
		saveto_TextFile(fnam, DriveLogList);
	}
}

//---------------------------------------------------------------------------
bool DriveLogFile::UpdateFile()
{
	if (!FileName.IsEmpty()) {
		try {
			LogList->SaveToFile(FileName);
		}
		catch (...) {
			return false;
		}
	}
}
//---------------------------------------------------------------------------
