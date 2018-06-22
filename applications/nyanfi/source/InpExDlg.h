//----------------------------------------------------------------------//
// NyanFi																//
//  拡張入力ボックス													//
//----------------------------------------------------------------------//
#ifndef InpExDlgH
#define InpExDlgH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include "Global.h"

//---------------------------------------------------------------------------
class TInputExDlg : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TButton *CancelBtn;
	TButton *OkBtn;
	TCheckBox *ClipCheckBox;
	TCheckBox *CnvChCheckBox;
	TCheckBox *DirChgCheckBox;
	TCheckBox *EditCheckBox;
	TCheckBox *SelDefCheckBox;
	TComboBox *CodePageComboBox;
	TLabel *CodePageLabel;
	TLabeledEdit *InputEdit;
	TLabeledEdit *TestCntEdit;
	TLabeledEdit *TestSizeEdit;
	TPanel *BottomPanel;
	TPanel *CreDirPanel;
	TPanel *CreTestPanel;
	TPanel *IniSttPanel;
	TPanel *InputPanel;
	TPanel *NewTextPanel;
	TPanel *NotationPanel;
	TRadioButton *DecRadioBtn;
	TRadioButton *HexRadioBtn;

	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall OptionClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall InputEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall InputEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall SelDefCheckBoxClick(TObject *Sender);

private:	// ユーザー宣言
	UnicodeString HelpTopic;

	bool is_Clone;
	bool is_CreDir;

	void __fastcall WmFormShowed(TMessage &msg);

public:		// ユーザー宣言
	int  CustomWidth;

	__fastcall TInputExDlg(TComponent* Owner);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_FORM_SHOWED,	TMessage,	WmFormShowed)
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TInputExDlg *InputExDlg;
//---------------------------------------------------------------------------
#endif
