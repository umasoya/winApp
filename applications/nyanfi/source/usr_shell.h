//----------------------------------------------------------------------//
// Shell 関連															//
//																		//
//----------------------------------------------------------------------//
#ifndef UserShellH
#define UserShellH

//---------------------------------------------------------------------------
#include <shlobj.h>
#include <System.Math.hpp>

//---------------------------------------------------------------------------
#define WM_FORM_DROPPED  (WM_APP + 10)

//---------------------------------------------------------------------------
typedef HRESULT (WINAPI *FUNC_SHGetKnownFolderPath)(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR*);

//---------------------------------------------------------------------------
#define WICIMG_FRAME		0
#define WICIMG_PREVIEW		1
#define WICIMG_THUMBNAIL	2

//---------------------------------------------------------------------------
#define FPRP_NAM_WD		15		//最大表示幅(半角単位)

UnicodeString get_WndText(HWND hWnd);
UnicodeString get_WndClassName(HWND hWnd);

UnicodeString get_PropTitle(UnicodeString s);
UnicodeString get_PropTitle(const _TCHAR *s);
UnicodeString make_PropLine(UnicodeString tit, UnicodeString str);
UnicodeString make_PropLine(const _TCHAR *tit, UnicodeString str);
UnicodeString make_PropLine(UnicodeString tit, int n);
UnicodeString make_PropLine(const _TCHAR *tit, int n);

void add_PropLine(UnicodeString tit, UnicodeString str, TStrings *lst);
void add_PropLine(const _TCHAR *tit, UnicodeString str, TStrings *lst);
void add_PropLine(UnicodeString tit, int n, TStrings *lst);
void add_PropLine(const _TCHAR *tit, int n, TStrings *lst);

void add_PropLine_if(const _TCHAR *tit, UnicodeString str, TStrings *lst);

//---------------------------------------------------------------------------
extern HWND TargetHandle;
extern int  DragStartTag;
extern int  DroppedTag;
extern int  DroppedMode;
extern int  DefDropMode;
extern UnicodeString DragDrive;
extern bool DropRefused;

extern TStringList *DroppedList;
extern TListBox *L_FileListBox;
extern TListBox *R_FileListBox;

extern IContextMenu2 *ContextMenu2;
extern IContextMenu3 *ContextMenu3;

//---------------------------------------------------------------------------
struct drop_target_rec {
	TForm *TargetForm;
	TWinControl *TargetWinCtrl;
	HWND hTargetWnd;
};

//---------------------------------------------------------------------------
// TDropSource クラス
//---------------------------------------------------------------------------
class TDropSource : public IDropSource
{
public:
	TDropSource() : FRefCount(0) { }
	HRESULT __stdcall QueryInterface(REFIID riid, void **ppv);
	ULONG   __stdcall AddRef();
	ULONG   __stdcall Release();
	HRESULT __stdcall QueryContinueDrag(BOOL fEsc, DWORD grfKeyState);
	HRESULT __stdcall GiveFeedback(DWORD dwEffect);

private:
	LONG FRefCount;
};

//---------------------------------------------------------------------------
// TDropTarget クラス
//---------------------------------------------------------------------------
class TDropTargetBase : public IDropTarget
{
protected:
	unsigned int refCount;

	TDropTargetBase();
	~TDropTargetBase() { }

	virtual HRESULT __stdcall QueryInterface(const IID& iid, void **ppv);
	virtual ULONG __stdcall AddRef();
	virtual ULONG __stdcall Release();
};

//---------------------------------------------------------------------------
// TDropTarget クラス
//---------------------------------------------------------------------------
class TDropTarget : public TDropTargetBase
{
public:
	static void CreateInstance(IDropTarget **pp);

private:
	virtual HRESULT __stdcall DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	virtual HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
	virtual HRESULT __stdcall DragLeave();
	virtual HRESULT __stdcall Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect);
};

//---------------------------------------------------------------------------
//ドロップターゲット・リスト
//---------------------------------------------------------------------------
class TDropTargetList : public TList
{
private:
	drop_target_rec * __fastcall Get(int Index);
	void __fastcall Put(int Index, drop_target_rec *Item);

	virtual void __fastcall Notify(void *Ptr, TListNotification Action);

public:
	__property drop_target_rec *Items[int Index] = {read=Get, write=Put};

	__fastcall TDropTargetList();
	void __fastcall AddNew(TForm *form, TWinControl *ctrl);
};

extern TDropTargetList *TargetList;
extern int TargetIndex;

//---------------------------------------------------------------------------
// UserShell クラス
//---------------------------------------------------------------------------
class UserShell
{
private:
	TStringList *PropertyList;		//プロパティの項目リスト (項目名=インデックスNo.)

	TFPUExceptionMask FpuTmpMask;	//浮動小数点例外の抑止マスク
									//※特殊な .opus ファイルでのエラー対策

	IDataObject* GetFilePathDataObject(UnicodeString dnam, TStringList *flist);

	unsigned int   msRead_uint(TMemoryStream *ms, int ofs);
	unsigned short msRead_ushort(TMemoryStream *ms, int ofs);
	UnicodeString  msRead_strdat(TMemoryStream *ms, int &ptr, bool is_uc);

public:
	UserShell(HWND hWnd);
	~UserShell();

	HWND MainHandle;
	int  PropNameWidth;		//プロパティ項目名の最大表示幅(半角単位)
	double ScrScale;

	void AddTargetList(TForm *form, TWinControl *ctrl);
	void DelTargetItem(TForm *form);

	DWORD DoFilesDragDrop(UnicodeString dnam, TStringList *flist, int tag);
	DWORD DoClipDragDrop();

	UnicodeString ShowContextMenu(HWND hWnd, UnicodeString dnam, TStringList *flist, TStringList *ex_item);
	UnicodeString DriveContextMenu(HWND hWnd, UnicodeString drvnam, TStringList *ex_item, int ex_ID = 0);
	bool InvokeMenuCmd(HWND hWnd, UnicodeString dnam, TStringList *flist, LPCSTR cmd);

	bool ShowFileRun();
	bool CreateShortCut(UnicodeString fnam, UnicodeString target,
		UnicodeString cmdprm = EmptyStr, UnicodeString wdir = EmptyStr, UnicodeString desc = EmptyStr);

	UnicodeString get_PropInf(UnicodeString fnam, TStringList *lst = NULL, UnicodeString idx_str = EmptyStr, bool lst_sw = false);
	UnicodeString get_PropStr(UnicodeString fnam, UnicodeString prp_nam);
	UnicodeString get_VerStr(UnicodeString fnam);
	UnicodeString get_PropValue(TStringList *lst, UnicodeString name);
	int  get_Duration(UnicodeString fnam);

	bool get_LnkInf(UnicodeString fnam, TStringList *lst, UnicodeString *nam = NULL,
		UnicodeString *prm = NULL, UnicodeString *fld = NULL, int *shw = NULL, UnicodeString *rem = NULL);

	UnicodeString get_FileTypeStr(UnicodeString fnam, bool lnk_sw = false);
	UnicodeString get_LnkName(UnicodeString fnam);

	HICON get_Icon(UnicodeString fnam, int &size, bool chk_sz = true);
	HICON get_SmallIcon(UnicodeString fnam);
	bool  draw_SmallIcon(UnicodeString fnam, TCanvas *cv, int x, int y);
	bool  draw_SmallIcon(UnicodeString fnam, TImage *ip, TColor bg = clBtnFace);
	bool  get_Thumbnail(UnicodeString fnam, Graphics::TBitmap *o_bmp, int o_size);

	UnicodeString KnownGuidStrToPath(UnicodeString s);
};
//---------------------------------------------------------------------------
extern UserShell *usr_SH;
//---------------------------------------------------------------------------

#endif
