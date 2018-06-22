//----------------------------------------------------------------------//
// �^�O�Ǘ����j�b�g														//
//																		//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <memory>
#include <algorithm>
#include <System.StrUtils.hpp>
#include "usr_str.h"
#include "usr_file_ex.h"
#include "usr_tag.h"

//---------------------------------------------------------------------------
TagManager *usr_TAG = NULL;

//---------------------------------------------------------------------------
TagManager::TagManager(UnicodeString fnam)
{
	TagRWLock	= new TMultiReadExclusiveWriteSynchronizer();
	TagDataList = new TStringList();
	TagNameList = new TStringList();

	FileName = fnam;
	if (FileExists(FileName)) {
		TagDataTime = get_file_age(FileName);

		TagRWLock->BeginWrite();
		TagDataList->LoadFromFile(FileName);
		TagRWLock->EndWrite();

		Modified = false;
		MakeTagNameList();
	}
}
//---------------------------------------------------------------------------
TagManager::~TagManager()
{
	delete TagRWLock;
	delete TagDataList;
	delete TagNameList;
}

//---------------------------------------------------------------------------
//�t�@�C���E�C���f�b�N�X���擾
//�ITagRWLock ���Ŏg�p���邱��
//---------------------------------------------------------------------------
int TagManager::GetIndex(UnicodeString fnam)
{
	UnicodeString f_str = fnam + "\t";
	int idx = -1;

	for (int i=0; i<TagDataList->Count; i++) {
		if (StartsText(f_str, TagDataList->Strings[i])) {
			idx = i; break;
		}
	}

	return idx;
}

//---------------------------------------------------------------------------
//�t�@�C�����X�V
//---------------------------------------------------------------------------
bool TagManager::UpdateFile()
{
	try {
		if (Modified) {
			TagRWLock->BeginRead();
			TagDataList->SaveToFile(FileName, TEncoding::UTF8);
			TagRWLock->EndRead();

			TagDataTime = get_file_age(FileName);
			Modified = false;
		}
		return true;
	}
	catch (...) {
		return false;
	}
}

//---------------------------------------------------------------------------
//�^�O�����X�g�쐬
//---------------------------------------------------------------------------
void TagManager::MakeTagNameList()
{
	TagRWLock->BeginRead();
	for (int i=0; i<TagDataList->Count; i++) {
		UnicodeString lbuf = TagDataList->Strings[i];
		TStringDynArray tag_lst = split_strings_semicolon(get_post_tab(lbuf));
		for (int j=0; j<tag_lst.Length; j++) {
			UnicodeString tag = tag_lst[j];
			if (TagNameList->IndexOf(tag)==-1) TagNameList->Add(tag);
		}
	}
	TagRWLock->EndRead();

	TagNameList->Sort();
}

//---------------------------------------------------------------------------
//�^�C���X�^���v���ω����Ă�����ēǂݍ���
//---------------------------------------------------------------------------
bool TagManager::Recycle()
{
	if (!FileExists(FileName)) return false;

	TDateTime ft = get_file_age(FileName);
	if (TagDataTime!=ft) {
		TagDataTime = ft;

		TagRWLock->BeginWrite();
		TagDataList->LoadFromFile(FileName);
		TagRWLock->EndWrite();

		Modified = false;
		MakeTagNameList();
		return true;
	}
	else
		return false;
}

//---------------------------------------------------------------------------
//���݂��Ȃ����ڂ̃^�O�f�[�^�𐮗�
//---------------------------------------------------------------------------
int TagManager::TrimData()
{
	int cnt = 0;
	int i = 0;

	TagRWLock->BeginWrite();
	while (i<TagDataList->Count) {
		if (!file_exists(get_pre_tab(TagDataList->Strings[i]))) {
			TagDataList->Delete(i); cnt++;
		}
		else i++;
	}
	TagRWLock->EndWrite();

	if (cnt>0) Modified = true;
	UpdateFile();

	return cnt;
}

//---------------------------------------------------------------------------
//�^�O���擾
//---------------------------------------------------------------------------
UnicodeString TagManager::GetTags(UnicodeString fnam)
{
	UnicodeString ret_str;

	TagRWLock->BeginRead();
	int idx = GetIndex(fnam);
	if (idx!=-1) ret_str = get_post_tab(TagDataList->Strings[idx]);
	TagRWLock->EndRead();

	return ret_str;
}

//---------------------------------------------------------------------------
//�^�O�̑召�����������̂��̂ɍ��킹�A��^�O������
//�����ɂȂ���Βǉ�
//---------------------------------------------------------------------------
UnicodeString TagManager::NormTags(
	UnicodeString tags,
	bool sw_add)			//������Βǉ�	(default = true)
{
	UnicodeString ret_str;

	for (;;) {
		UnicodeString tnam = split_tkn(tags, ';');
		if (!tnam.IsEmpty()) {
			int idx = TagNameList->IndexOf(tnam);
			if (idx==-1) {
				if (sw_add) TagNameList->Add(tnam);
			}
			else
				tnam = TagNameList->Strings[idx];
			cat_str_semicolon(ret_str, tnam);
		}
		if (tags.IsEmpty()) break;
	}

	if (sw_add) TagNameList->Sort();

	return ret_str;
}

//---------------------------------------------------------------------------
//�w��t�@�C���̃^�O�����폜
//---------------------------------------------------------------------------
void TagManager::DelItem(UnicodeString fnam)
{
	TagRWLock->BeginWrite();
	int idx = GetIndex(fnam);
	if (idx!=-1) {
		TagDataList->Delete(idx);
		Modified = true;
	}
	TagRWLock->EndWrite();
}

//---------------------------------------------------------------------------
//�^�O�̐ݒ�
//---------------------------------------------------------------------------
void TagManager::SetTags(
	UnicodeString fnam,		//�t�@�C��
	UnicodeString tags)		//�ǉ��^�O(;��؂�)
{
	fnam = ExcludeTrailingPathDelimiter(fnam);

	TagRWLock->BeginWrite();
	int idx = GetIndex(fnam);
	if (idx!=-1) {
		if (!tags.IsEmpty())
			TagDataList->Strings[idx] = fnam + "\t" + tags;
		else
			TagDataList->Delete(idx);
	}
	else if (!tags.IsEmpty()) {
		TagDataList->Add(fnam + "\t" + tags);
	}
	TagRWLock->EndWrite();

	Modified = true;
}

//---------------------------------------------------------------------------
//�^�O�̒ǉ�
//---------------------------------------------------------------------------
void TagManager::AddTags(
	UnicodeString fnam,		//�t�@�C��
	UnicodeString tags)		//�ǉ��^�O(;��؂�)
{
	if (tags.IsEmpty()) return;

	fnam = ExcludeTrailingPathDelimiter(fnam);

	TagRWLock->BeginWrite();
	int idx = GetIndex(fnam);
	if (idx!=-1) {
		UnicodeString lbuf = TagDataList->Strings[idx];
		UnicodeString   tag_str = get_post_tab(lbuf);
		TStringDynArray tag_lst = split_strings_semicolon(tag_str);
		TStringDynArray add_lst = split_strings_semicolon(tags);
		for (int j=0; j<add_lst.Length; j++) {
			if (add_lst[j].IsEmpty()) continue;
			bool found = false;
			for (int k=0; k<tag_lst.Length && !found; k++) {
				found = SameText(add_lst[j], tag_lst[k]);
			}
			if (!found) cat_str_semicolon(tag_str, add_lst[j]);
		}
		TagDataList->Strings[idx] = fnam + "\t" + tag_str;
	}
	else {
		TagDataList->Add(fnam + "\t" + tags);
	}
	TagRWLock->EndWrite();

	Modified = true;
}

//---------------------------------------------------------------------------
//�t�@�C�����̕ύX
//---------------------------------------------------------------------------
bool TagManager::Rename(UnicodeString old_nam, UnicodeString new_nam)
{
	old_nam = ExcludeTrailingPathDelimiter(old_nam);
	new_nam = ExcludeTrailingPathDelimiter(new_nam);

	TagRWLock->BeginWrite();
	int idx = GetIndex(old_nam);
	if (idx!=-1) {
		TagDataList->Strings[idx] = new_nam + "\t" + get_post_tab(TagDataList->Strings[idx]);
		Modified = true;
	}
	TagRWLock->EndWrite();

	return (idx!=-1);
}

//---------------------------------------------------------------------------
//�^�O�̉���
//---------------------------------------------------------------------------
int TagManager::RenTag(UnicodeString old_tag, UnicodeString new_tag)
{
	int idx = TagNameList->IndexOf(old_tag);
	if (idx!=-1) TagNameList->Strings[idx] = new_tag;

	TagRWLock->BeginWrite();
	int total = 0;
	for (int i=0; i<TagDataList->Count; i++) {
		UnicodeString lbuf = TagDataList->Strings[i];
		UnicodeString fnam = get_pre_tab(lbuf);
		TStringDynArray tag_lst = split_strings_semicolon(get_post_tab(lbuf));
		UnicodeString new_tags;
		int cnt = 0;
		for (int j=0; j<tag_lst.Length; j++) {
			if (SameText(old_tag, tag_lst[j])) {
				tag_lst[j] = new_tag; cnt++;
			}
			cat_str_semicolon(new_tags, tag_lst[j]);
		}

		if (cnt>0) {
			TagDataList->Strings[i] = fnam + "\t" + new_tags;
			total += cnt;
		}
	}
	TagRWLock->EndWrite();

	if (total>0) Modified = true;
	UpdateFile();

	return total;
}

//---------------------------------------------------------------------------
//�^�O�f�[�^�̍폜
//---------------------------------------------------------------------------
int TagManager::DelTagData(UnicodeString tag)
{
	int idx = TagNameList->IndexOf(tag);
	if (idx==-1) return 0;

	TagNameList->Delete(idx);

	TagRWLock->BeginWrite();
	int total = 0;
	int i = 0;
	while (i<TagDataList->Count) {
		UnicodeString lbuf = TagDataList->Strings[i];
		UnicodeString fnam = get_pre_tab(lbuf);
		TStringDynArray tag_lst = split_strings_semicolon(get_post_tab(lbuf));
		UnicodeString upd_tags;
		int cnt = 0;
		for (int j=0; j<tag_lst.Length; j++) {
			if (SameText(tag, tag_lst[j]))
				cnt++;
			else
				cat_str_semicolon(upd_tags, tag_lst[j]);
		}

		if (cnt>0) {
			total += cnt;
			if (upd_tags.IsEmpty())
				TagDataList->Delete(i);
			else {
				TagDataList->Strings[i] = fnam + "\t" + upd_tags;
				i++;
			}
		}
		else i++;
	}
	TagRWLock->EndWrite();

	if (total>0) Modified = true;
	UpdateFile();

	return total;
}

//---------------------------------------------------------------------------
//�^�O�`�F�b�N���X�g�̏�����
//�߂�l: �^�O�̍ő�\����
//---------------------------------------------------------------------------
int TagManager::IniCheckList(TCheckListBox *lp, UnicodeString tnam)
{
	lp->Items->Assign(TagNameList);

	int max_wd = 0;
	for (int i=0; i<lp->Count; i++) {
		lp->Checked[i] = false;
		max_wd = std::max(max_wd, lp->Canvas->TextWidth(lp->Items->Strings[i]));
	}

	if (!tnam.IsEmpty()) lp->ItemIndex = lp->Items->IndexOf(tnam);

	return max_wd;
}

//---------------------------------------------------------------------------
//���X�g�`�F�b�N�{�b�N�X����^�O������ւ̔��f
//---------------------------------------------------------------------------
UnicodeString TagManager::CheckToTags(TCheckListBox *lp, UnicodeString tags)
{
	bool end_sc = EndsStr(";", tags);

	TStringDynArray org_lst = split_strings_semicolon(tags);
	std::unique_ptr<TStringList> lst(new TStringList());
	for (int i=0; i<org_lst.Length; i++)
		if (!org_lst[i].IsEmpty()) lst->Add(org_lst[i]);

	for (int i=0; i<lp->Count; i++) {
		UnicodeString tnam = lp->Items->Strings[i];
		int idx = lst->IndexOf(tnam);
		if (idx==-1 && lp->Checked[i])
			lst->Add(tnam);
		else if (idx!=-1 && !lp->Checked[i])
			lst->Delete(idx);
	}

	UnicodeString ret_str, tag1;
	for (int i=0; i<lst->Count; i++) {
		UnicodeString tnam = lst->Strings[i];
		int idx = lp->Items->IndexOf(tnam);
		if (idx!=-1)
			cat_str_semicolon(ret_str, lp->Items->Strings[idx]);
		else
			cat_str_semicolon(tag1, tnam);
	}

	cat_str_semicolon(ret_str, tag1);

	if (end_sc) ret_str.UCAT_T(";");

	return ret_str;
}

//---------------------------------------------------------------------------
//�^�O�̎g�p�����J�E���g
//---------------------------------------------------------------------------
void TagManager::CountTags(TCheckListBox *lp,
	bool clr_sw)	//tru = �N���A�̂� (default = false)
{
	for (int i=0; i<lp->Count; i++) lp->Items->Objects[i] = (TObject*)0;

	if (!clr_sw) {
		Screen->Cursor = crHourGlass;
		TagRWLock->BeginRead();
		for (int i=0; i<TagDataList->Count; i++) {
			TStringDynArray tag_lst = split_strings_semicolon(get_post_tab(TagDataList->Strings[i]));
			for (int j=0; j<tag_lst.Length; j++) {
				int idx = lp->Items->IndexOf(tag_lst[j]);
				if (idx!=-1) lp->Items->Objects[idx] = (TObject*)((int)lp->Items->Objects[idx] + 1);
			}
		}
		TagRWLock->EndRead();
		Screen->Cursor = crDefault;
	}

	lp->Repaint();
}

//---------------------------------------------------------------------------
//�t�@�C���̈ړ�(�^�O���l��)
//---------------------------------------------------------------------------
bool move_FileT(UnicodeString old_nam, UnicodeString new_nam)
{
	bool res = ::MoveFileEx(cv_ex_filename(old_nam).c_str(), cv_ex_filename(new_nam).c_str(),
				MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH);

	if (res && usr_TAG) usr_TAG->Rename(old_nam, new_nam);

	return res;
}
//---------------------------------------------------------------------------
