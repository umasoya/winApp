//----------------------------------------------------------------------//
// ルーペ																//
//----------------------------------------------------------------------//
#ifndef LoupeFrmH
#define LoupeFrmH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.Buttons.hpp>
#include <Vcl.ToolWin.hpp>

//---------------------------------------------------------------------------
#define MINHI_LOUPE 42

//---------------------------------------------------------------------------
class TLoupeForm : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TImage *LoupeImage;
	TPanel *ImgPanel;
	TPanel *OptPanel;
	TToolBar *MagToolBar;
	TToolButton *ToolButton1;
	TToolButton *ToolButton2;
	TToolButton *ToolButton3;
	TToolButton *ToolButton4;

	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall LoupeImageMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall ToolBtnClick(TObject *Sender);

private:	// ユーザー宣言
	void __fastcall WmSysCommand(TWMSysCommand & SysCom);

public:		// ユーザー宣言
	__fastcall TLoupeForm(TComponent* Owner);

	void __fastcall SaveOption();
	void __fastcall DrawImage(Graphics::TBitmap *bmp = NULL, int x = 0, int y = 0);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_SYSCOMMAND,	TWMSysCommand,	WmSysCommand)
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TLoupeForm *LoupeForm;
//---------------------------------------------------------------------------
#endif
