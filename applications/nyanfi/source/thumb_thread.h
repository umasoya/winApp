//----------------------------------------------------------------------//
// NyanFi																//
//  サムネイル取得スレッド												//
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

	TStringList *ThumbnailList;	//サムネイルリスト

	bool ReqClear;				//リストのクリア要求
	bool ReqStart;				//取得スタート要求
	bool ReqMake;				//個別作成要求
	int  MakeIndex;
	bool TaskBusy;				//処理中
	bool isEmpty;				//サムネイル未取得

	__fastcall TThumbnailThread(bool CreateSuspended);
	bool __fastcall FitSize(int *wd, int *hi);
	void __fastcall MakeThumbnail(int idx);
};
//---------------------------------------------------------------------------
#endif
