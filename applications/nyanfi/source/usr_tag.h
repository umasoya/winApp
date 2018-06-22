//----------------------------------------------------------------------//
// タグ管理ユニット														//
//																		//
//----------------------------------------------------------------------//
#ifndef UsrTagManagerH
#define UsrTagManagerH

#include <vcl.h>
#include <Vcl.CheckLst.hpp>

//---------------------------------------------------------------------------
class TagManager
{
private:
	TDateTime TagDataTime;	//現在のタイムスタンプ
	bool Modified;

	int  GetIndex(UnicodeString fnam);

public:
	UnicodeString FileName;
	TStringList *TagDataList;
	TStringList *TagNameList;
	TMultiReadExclusiveWriteSynchronizer *TagRWLock;	//TagDataList の保護用

	TagManager(UnicodeString fnam);
	~TagManager();

	bool UpdateFile();
	bool Recycle();
	void MakeTagNameList();
	int  TrimData();

	UnicodeString GetTags(UnicodeString fnam);
	UnicodeString NormTags(UnicodeString tags, bool sw_add = true);
	void DelItem(UnicodeString fnam);
	void SetTags(UnicodeString fnam, UnicodeString tags);
	void AddTags(UnicodeString fnam, UnicodeString tags);
	bool Rename(UnicodeString old_nam, UnicodeString new_nam);
	int  RenTag(UnicodeString old_tag, UnicodeString new_tag);
	int  DelTagData(UnicodeString tag);

	int IniCheckList(TCheckListBox *lp, UnicodeString tnam = EmptyStr);
	UnicodeString CheckToTags(TCheckListBox *lp, UnicodeString tags);
	void CountTags(TCheckListBox *lp, bool clr_sw = false);
};
//---------------------------------------------------------------------------
extern TagManager *usr_TAG;

//---------------------------------------------------------------------------
bool move_FileT(UnicodeString old_nam, UnicodeString new_nam);

#endif
