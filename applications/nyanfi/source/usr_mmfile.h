//----------------------------------------------------------------------//
// メモリマップドファイルクラス											//
//																		//
//----------------------------------------------------------------------//
#ifndef UsrMMFileH
#define UsrMMFileH

//---------------------------------------------------------------------------
#define MAX_MEMMAP_SIZE	1073741824L		//最大マップサイズ		   (1GB)
#define FAILED_BUF_SIZE	8388608L		//マップ失敗時の読込サイズ (8MB)

//---------------------------------------------------------------------------
class MemMapFile
{
private:
	HANDLE hFile;
	HANDLE hMap;
	BYTE *pPointer;		//マップ用ポインタ

	TBytes ByteBuff;	//通常読込用バッファ

	BYTE __fastcall Get(unsigned int Index);
	void Initialize();

public:
	__property BYTE Bytes[unsigned int Index] = {read=Get};
	__int64 FileSize;		//ファイルサイズ
	unsigned int BuffSize;	//バッファ(マップ)サイズ
	bool MapEnabled;		//マップ有効
	bool isMaped;			//マップされている
	UnicodeString ErrMsg;	//エラーメッセージ

	MemMapFile();
	~MemMapFile();

	bool OpenRO(UnicodeString fnam, __int64 top_adr, unsigned int max_size);
	void Close();
};
//---------------------------------------------------------------------------
#endif
