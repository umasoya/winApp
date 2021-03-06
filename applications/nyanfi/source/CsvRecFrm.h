//----------------------------------------------------------------------//
// CSV/TSVレコード														//
//																		//
//----------------------------------------------------------------------//
#ifndef CsvRecFrmH
#define CsvRecFrmH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <System.Actions.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.Menus.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ActnList.hpp>
#include <Vcl.Buttons.hpp>
#include "usr_scrpanel.h"

//---------------------------------------------------------------------------
class TCsvRecForm : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TCheckBox *TopIsHeaderCheckBox;
	THeaderControl *RecordHeader;
	TMenuItem *CopyItem;
	TPanel *GridPanel;
	TPanel *OptPanel;
	TPopupMenu *PopupMenu1;
	TStringGrid *RecordGrid;

	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormHide(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall RecordHeaderSectionResize(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall RecordGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect, TGridDrawState State);
	void __fastcall TopIsHeaderCheckBoxClick(TObject *Sender);
	void __fastcall CopyItemClick(TObject *Sender);
	void __fastcall FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall RecordHeaderDrawSection(THeaderControl *HeaderControl, THeaderSection *Section,
          const TRect &Rect, bool Pressed);
	void __fastcall RecordHeaderResize(TObject *Sender);

private:	// ユーザー宣言
	UnicodeString HeaderStr;
	UnicodeString RecordStr;
	TStringList  *DataList;

	bool IsCSV;
	int  RecIndex;
	int  DatIndex;

	void __fastcall WmFormShowed(TMessage &msg);
	int __fastcall UpdateItemName();

public:		// ユーザー宣言
	UsrScrollPanel *GridScrPanel;	//シンプルスクロールバー

	__fastcall TCsvRecForm(TComponent* Owner);
	void __fastcall UpdateRecord(TStringList *lst = NULL, int idx = -1);

	BEGIN_MESSAGE_MAP
		VCL_MESSAGE_HANDLER(WM_FORM_SHOWED,	TMessage,		WmFormShowed)
	END_MESSAGE_MAP(TForm)
};
//---------------------------------------------------------------------------
extern PACKAGE TCsvRecForm *CsvRecForm;
//---------------------------------------------------------------------------
#endif
