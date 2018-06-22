//----------------------------------------------------------------------//
// NyanFi																//
//  UNCパスの存在チェック・スレッド										//
//----------------------------------------------------------------------//
#ifndef TCheckPathThreadH
#define TCheckPathThreadH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>

//---------------------------------------------------------------------------
class TCheckPathThread : public TThread
{
private:
	void __fastcall Execute();

public:
	UnicodeString PathName;
	bool isOk;

	__fastcall TCheckPathThread(bool CreateSuspended);
};
//---------------------------------------------------------------------------
#endif
