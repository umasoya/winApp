//----------------------------------------------------------------------//
// INIファイル処理クラス												//
//																		//
//----------------------------------------------------------------------//
#ifndef DriveLogFileH
#define DriveLogFileH

//----------------------------------------------------------------------
class DriveLogFile
{
private:
	TStringList *LogList;

public:
	UnicodeString FileName;

	DriveLogFile();
	~DriveLogFile();

	bool UpdateFile();
};
//----------------------------------------------------------------------
extern DriveLogFile  *DriveLog;

//----------------------------------------------------------------------
#endif
