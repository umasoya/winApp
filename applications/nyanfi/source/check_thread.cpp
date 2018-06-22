//----------------------------------------------------------------------//
// NyanFi																//
//  UNC�p�X�̑��݃`�F�b�N�E�X���b�h										//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include "check_thread.h"

#pragma package(smart_init)

//---------------------------------------------------------------------------
//�X���b�h�̏�����
//---------------------------------------------------------------------------
__fastcall TCheckPathThread::TCheckPathThread(bool CreateSuspended) : TThread(CreateSuspended)
{
	Priority		= tpLower;
	FreeOnTerminate = true;

	PathName = EmptyStr;
	isOk	 = false;
}

//---------------------------------------------------------------------------
void __fastcall TCheckPathThread::Execute()
{
	while (!Terminated) {
		if (!PathName.IsEmpty()) {
			isOk	 = ::PathIsDirectory(PathName.c_str());
			PathName = EmptyStr;
		}
		Sleep(50);
	}
}
//---------------------------------------------------------------------------
