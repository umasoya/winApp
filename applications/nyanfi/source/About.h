//----------------------------------------------------------------------//
// バージョン情報														//
//----------------------------------------------------------------------//
#ifndef aboutH
#define aboutH
//----------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>

//----------------------------------------------------------------------------
class TAboutBox : public TForm
{
__published:
	TButton *CancelBtn;
	TButton *OKButton;
	TImage *ProgramIcon;
	TLabel *Copyright;
	TLabel *ProductName;
	TLabel *SupportURL;
	TLabel *Version;
	TPanel *Panel1;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall SupportURLClick(TObject *Sender);

private:

public:
	virtual __fastcall TAboutBox(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TAboutBox *AboutBox;
//----------------------------------------------------------------------------
#endif
