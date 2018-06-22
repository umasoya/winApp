//----------------------------------------------------------------------//
// シンプルスクロールバー・パネル										//
//																		//
//  TListBox、TStringGrid、TScrollBar の何れかに関連付け可能			//
//----------------------------------------------------------------------//
#ifndef UsrScrollPanelH
#define UsrScrollPanelH

//---------------------------------------------------------------------------
#include <Vcl.Grids.hpp>

//---------------------------------------------------------------------------
//Flag
#define USCRPNL_FLAG_FL		0x0001	//ファイルリスト
#define USCRPNL_FLAG_GL		0x0002	//一覧リスト

#define USCRPNL_FLAG_P_WP	0x0020	//親パネルの WindowProc 入れ替え
#define USCRPNL_FLAG_L_WP	0x0040	//リストボックスの WindowProc 入れ替え
#define USCRPNL_FLAG_G_WP	0x0080	//グリッドの WindowProc 入れ替え
#define USCRPNL_FLAG_HS		0x0100	//水平スクロールバー付

//---------------------------------------------------------------------------
#define WM_NYANFI_USCROLL	(WM_APP + 500)	//ファイルリストのノブ処理用

//---------------------------------------------------------------------------
class UsrScrollPanel
{
private:
	TPanel     *ScrPanelV;			//垂直スクロールパネル
	TPaintBox  *ScrPaintBoxV;		//描画領域

	TPanel     *ScrPanelH;			//水平スクロールパネル
	TPaintBox  *ScrPaintBoxH;		//描画領域

	TRect ScrKnobRectV;				//ノブの長方形
	TRect ScrKnobRectH;
	int   ScrKnobMaxY;				//最大ノブ位置
	int   ScrKnobMaxX;

	bool  ScrCatchKnob;				//ノブをつかんでいる
	int   ScrCatchYp;
	int   ScrCatchXp;
	int   ScrPage;

	TWndMethod org_ParentPanelWndProc;
	void __fastcall ParentPanelWndProc(TMessage &msg);

	TWndMethod org_AssoListWndProc;
	void __fastcall AssoListWndProc(TMessage &msg);

	TWndMethod org_AssoGridWndProc;
	void __fastcall AssoGridWndProc(TMessage &msg);

	TWndMethod org_ScrPanelVWndProc;
	void __fastcall ScrPanelVWndProc(TMessage &msg);

	TWndMethod org_ScrPanelHWndProc;
	void __fastcall ScrPanelHWndProc(TMessage &msg);

	void __fastcall ScrPaintBoxPaint(TObject *Sender);
	int  __fastcall GetKnobPosH();

	int  __fastcall get_GridTotalWidth();
	void __fastcall set_GridLefCol(int pos);
	int  __fastcall get_GridTotalHeight();
	void __fastcall set_GridTopRow(int pos);

public:
	TPanel      *ParentPanel;			//親パネル
	TListBox    *AssoListBox;			//関連リストボックス
	TStringGrid *AssoStrGrid;			//関連グリッド
	TScrollBar  *AssoScrollBar;			//関連スクロールバー

	int Flag;							//設定フラグ
	int KnobWidth;

	Graphics::TBitmap *KnobImgBuffV;	//垂直ノブ画像
	Graphics::TBitmap *KnobImgBuffH;	//水平ノブ画像

	TColor Color;						//背景色
	TColor KnobColor;					//ノブ色
	TColor KnobBdrColor;				//ノブ輪郭色

	bool ListCsrVisible;				//リストボックスのカーソルを常に可視領域に

	TMouseEvent OnRButtonUp;			//右クリックイベント

	bool FVisible;
	void __fastcall SetVisible(bool Value);
	__property bool Visible = {read = FVisible,  write = SetVisible};

	bool VisibleV, VisibleH;			//スクロールバーの表示状態

	UsrScrollPanel(TPanel *pp, TListBox *lp,    int flag);
	UsrScrollPanel(TPanel *pp, TStringGrid *gp, int flag);
	UsrScrollPanel(TPanel *pp, TScrollBar *sp,  int flag);
	~UsrScrollPanel();

	void __fastcall InitializePanel();
	void __fastcall UpdateKnob();
	void __fastcall Repaint();

	void __fastcall ScrPaintBoxMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall ScrPaintBoxMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall ScrPaintBoxMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
};
//---------------------------------------------------------------------------
#endif
