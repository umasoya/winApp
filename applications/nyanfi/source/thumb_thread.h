//----------------------------------------------------------------------//
// NyanFi																//
//  �T���l�C���擾�X���b�h												//
//----------------------------------------------------------------------//
#ifndef ThumbnailThreadH
#define ThumbnailThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>

//---------------------------------------------------------------------------
class TThumbnailThread : public TThread
{
private:
	void __fastcall Execute();

public:
	HWND CallbackWnd;

	TStringList *ThumbnailList;	//�T���l�C�����X�g

	bool ReqClear;				//���X�g�̃N���A�v��
	bool ReqStart;				//�擾�X�^�[�g�v��
	bool ReqMake;				//�ʍ쐬�v��
	int  MakeIndex;
	bool TaskBusy;				//������
	bool isEmpty;				//�T���l�C�����擾

	__fastcall TThumbnailThread(bool CreateSuspended);
	bool __fastcall FitSize(int *wd, int *hi);
	void __fastcall MakeThumbnail(int idx);
};
//---------------------------------------------------------------------------
#endif
