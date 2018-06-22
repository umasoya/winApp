//----------------------------------------------------------------------//
// NyanFi																//
//  カラーピッカー														//
//----------------------------------------------------------------------//
#ifndef ColPickerH
#define ColPickerH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>

//---------------------------------------------------------------------------
class TColorPicker : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TCheckBox *Ave9pxCheckBox;
	TCheckBox *CmpColCheckBox;
	TCheckBox *SafeColCheckBox;
	TComboBox *FmtComboBox;
	TEdit *ColEdit;
	TImage *ColImage;
	TLabel *ColLabel;
	TPanel *Col2Panel;
	TPanel *ColPanel;

	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);

public:		// ユーザー宣言
	TImage *ViewImage;

	__fastcall TColorPicker(TComponent* Owner);
	void __fastcall UpdateStt(int x = 0, int y = 0, float ratio = 1.0);
	void __fastcall CopyColor();
};
//---------------------------------------------------------------------------
extern PACKAGE TColorPicker *ColorPicker;
//---------------------------------------------------------------------------
#endif
