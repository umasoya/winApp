//----------------------------------------------------------------------//
// NyanFi																//
//  �I�v�V�����ݒ�														//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <memory>
#include <algorithm>
#include <mmsystem.h>
#include <Vcl.FileCtrl.hpp>
#include <RegularExpressions.hpp>
#include "usr_shell.h"
#include "usr_xd2tx.h"
#include "usr_arc.h"
#include "UserFunc.h"
#include "UserMdl.h"
#include "Global.h"
#include "CmdListDlg.h"
#include "OptDlg.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TOptionDlg *OptionDlg = NULL;

//---------------------------------------------------------------------------
__fastcall TOptionDlg::TOptionDlg(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormCreate(TObject *Sender)
{
	cursor_HourGlass();
	set_ComboBox_AutoComp(this);

	FindMarkList = new MarkList(this);
	ColBufList	 = new TStringList();

	set_ButtonMark(FindDownBtn, UBMK_BDOWN);
	set_ButtonMark(FindUpBtn,   UBMK_BUP);

	for (int i=0; i<MAX_KEYTABS; i++) KeyListBuf[i] = new TStringList();
	FKeyLabelBuf = new TStringList();

	set_ComboBoxText(LayoutComboBox,
		_T("�t�@�C�����X�g����\n�t�@�C�����X�g����\n�t�@�C�����X�g���E\n"));

	set_ComboBoxText(SizeFmtComboBox,
		_T("GB/MB/�o�C�g\nGB/MB/KB/�o�C�g\nGB/MB/KB\n"));

	set_ComboBoxText(TabStyleComboBox,
		_T("�^�u\n�^�u(��)\n�{�^��\n�t���b�g\n"));

	set_ComboBoxText(ScrBarStyleComboBox,
		_T("�W��\n�V���v��\n�V���v��(3/4��)\n�V���v��(1/2��)\n�V���v��(�摜)\n"));

	for (int i=0; i<FontList->Count-1; i++) FontComboBox->Items->Add(FontList->ValueFromIndex[i]);

	set_ComboBoxText(SizeDecDgtComboBox, _T("0\n1\n2\n3\n"));
	set_ComboBoxText(MaxTasksComboBox,   _T("1\n2\n3\n4\n"));

	OptColListBox->Items->Text =
		"bgList=���X�g�̔w�i�F\n"
		"bgList2=���X�g�̔w�i�F(����)\n"
		"fgList=���X�g�̕����F\n"
		"bgArc=���z�f�B���N�g���̔w�i�F\n"
		"bgFind=�������ʃ��X�g�̔w�i�F\n"
		"bgWork=���[�N���X�g�̔w�i�F\n"
		"bgFTP=FTP�����[�g���̔w�i�F\n"
		"selItem=|�I�����ڂ̔w�i�F\n"
		"oppItem=���΃p�X�I�����ڂ̔w�i�F\n"
		"fgSelItem=�I�����ڂ̕����F\n"
		"bgMark=�}�[�N���ڂ̔w�i�F\n"
		"matchItem=INC.�T�[�`�̃}�b�`���ڔw�i�F\n"
		"Folder=�f�B���N�g���̕����F\n"
		"SymLink=�f�B���N�g��(�V���{���b�N)�̕����F\n"
		"Protect=�f�B���N�g��(�폜����)�̕����F\n"
		"ReadOnly=�ǂݍ��ݐ�p�����̕����F\n"
		"Hidden=�B�������̕����F\n"
		"System=�V�X�e�������̕����F\n"
		"fgTagNam=�^�O���̕����F\n"
		"InvItem=�����ȍ��ڂ̕����F\n"
		"Cursor=���C���J�[�\���̐F\n"
		"bgScrBar=|�V���v���X�N���[���o�[�̔w�i�F\n"
		"bgScrKnob=�V���v���X�N���[���m�u�̐F\n"
		"frScrKnob=�V���v���X�N���[���m�u�̗֊s�F\n"
		"Splitter=�������E��\n"
		"bgTlBar1=�c�[���o�[�̃O���f�[�V�����J�n�F\n"
		"bgTlBar2=�c�[���o�[�̃O���f�[�V�����I���F\n"
		"fgTlBar=�c�[���o�[�̕����F\n"
		"htTlBar=�c�[���o�[�̃z�b�g�g���b�L���O�F\n"
		"bgTabBar=|�^�u�o�[�̔w�i�F\n"
		"bgActTab=�A�N�e�B�u�ȃ^�u�̔w�i�F\n"
		"bgInAcTab=��A�N�e�B�u�ȃ^�u�̔w�i�F\n"
		"frmTab=�^�u�̗֊s��\n"
		"fgTab=�^�u�̕����F\n"
		"bgListHdr=|���X�g�w�b�_�̔w�i�F\n"
		"fgListHdr=���X�g�w�b�_�̕����F\n"
		"bgDirInf=�f�B���N�g�����̔w�i�F\n"
		"fgDirInf=�f�B���N�g�����̕����F\n"
		"bgDirRel=�f�B���N�g���֌W�̔w�i�F\n"
		"fgDirRel=�f�B���N�g���֌W�̕����F\n"
		"bgDrvInf=�h���C�u���̔w�i�F\n"
		"fgDrvInf=�h���C�u���̕����F\n"
		"bgInf=|�t�@�C�����̔w�i�F\n"
		"fgInf=�t�@�C�����̕����F\n"
		"fgInfNam=�t�@�C�����̍��ږ������F\n"
		"bgTxtPrv=�e�L�X�g�v���r���[�̔w�i�F\n"
		"fgTxtPrv=�e�L�X�g�v���r���[�̕����F\n"
		"bgLog=���O�̔w�i�F\n"
		"fgLog=���O�̕����F\n"
		"bgTask=�^�X�N��ԕ\���̔w�i�F\n"
		"fgPrgBar=�^�X�N�i���o�[�F\n"
		"bgPrgBar=�^�X�N�i���w�i�F\n"
		"Error=�G���[/���ӂ̕����F\n"
		"bgSttBar=|�X�e�[�^�X�o�[�̔w�i�F\n"
		"fgSttBar=�X�e�[�^�X�o�[�̕����F\n"
		"bgInfHdr=���w�b�_�̔w�i�F\n"
		"fgInfHdr=���w�b�_�̕����F\n"
		"bgView=|�e�L�X�g�r���A�[�̔w�i�F\n"
		"fgView=�e�L�X�g�r���A�[�̕����F\n"
		"Margin=�e�L�X�g�r���A�[�̗]����\n"
		"bgRuler=���[���̔w�i�F\n"
		"fgRuler=���[���̖ڐ��F\n"
		"bgLineNo=�s�ԍ��w�i�F\n"
		"LineNo=�s�ԍ������F\n"
		"Mark=�s�}�[�N\n"
		"bdrLine=�s�ԍ����E��\n"
		"bdrFold=�܂�Ԃ����E��\n"
		"bdrFixed=�Œ蒷�\���̏c�r��\n"
		"Comment=|�R�����g�̕����F\n"
		"Strings=������̕����F\n"
		"Reserved=�\���̕����F\n"
		"Symbol=�V���{���̕����F\n"
		"Numeric=���l�̕����F\n"
		"fgEmpBin1=�o�C�i�����������F1\n"
		"fgEmpBin2=�o�C�i�����������F2\n"
		"fgEmpBin3=�o�C�i�����������F3\n"
		"Headline=���o���̕����F\n"
		"Ruby=���r\n"
		"URL=URL�̕����F\n"
		"LocalLink=���[�J���t�@�C���ւ̃����N\n"
		"fgEmp=���������F\n"
		"bgEmp=�����w�i�F\n"
		"TAB=�^�u�\���F\n"
		"CR=���s�\���F\n"
		"HR=�r���̐F\n"
		"Ctrl=�R���g���[���R�[�h\n"
		"bgImage=|�摜�̔w�i�F\n"
		"bgDblPg=���J���\���̗]���F\n"
		"bgWMF=���^�t�@�C���̔w�i�F\n"
		"bdrThumb=�T���l�C���̋��E��\n"
		"ThumbExif=�T���l�C����Exif���\n"
		"ImgGrid=�摜�����O���b�h\n"
		"bgHint=|�q���g�\���̔w�i�F\n"
		"fgHint=�q���g�\���̕����F\n"
		"bgWarn=�x���\���̔w�i�F\n"
		"Invalid=�����ȍ��ڂ̔w�i�F\n"
		"Illegal=�s���ȓ��͍��ڂ̔w�i�F\n"
		"OptFind=�I�v�V�����ݒ�̌�������\n";

	TimColListBox->Items->Text =
		"Tim1H=1���Ԉȓ�\n"
		"Tim3H=3���Ԉȓ�\n"
		"Tim1D=1���ȓ�\n"
		"Tim3D=3���ȓ�\n"
		"Tim7D=7���ȓ�\n"
		"Tim1M=1�����ȓ�\n"
		"Tim3M=3�����ȓ�\n"
		"Tim6M=6�����ȓ�\n"
		"Tim1Y=1�N�ȓ�\n"
		"Tim3Y=3�N�ȓ�\n"
		"TimOld=������O\n"
		"Size4G=|4GB�ȏ�\n"
		"Size1G=1GB�ȏ�\n"
		"Size1M=1MB�ȏ�\n"
		"Size1K=1KB�ȏ�\n"
		"SizeLT=1KB����\n"
		"Size0=0\n";

	set_ComboBoxText(BgImgModeComboBox,
		_T("��\��\n")
		_T("2��ʂɓn���ĕ\��\n")
		_T("���ꂼ��ɕ\��\n")
		_T("�f�X�N�g�b�v�w�i��\��\n"));
	set_ComboBoxText(BgImgSubModeComboBox,
		_T("�����ɕ\��\n")
		_T("���ׂĕ\��\n")
		_T("�\���T�C�Y�ɃX�g���b�`\n")
		_T("�\���T�C�Y�ɍ��킹�Đ؂�o��\n"));

	set_ComboBoxText(ScaleOptComboBox,
		_T("�j�A���X�g�l�C�o�[\n�o�C���j�A\n�o�C�L���[�r�b�N\n�t�@���g���T���v�����O\n��Ԃ��Ȃ�\n"));

	//�}�E�X�z�C�[��
	set_ComboBoxText(WheelCmdF0ComboBox,
		_T("ScrollUp/ScrollDown\n")
		_T("PageUp/PageDown\n")
		_T("CursorUp/CursorDown\n")
		_T("PrevDrive/NextDrive\n")
		_T("PrevTab/NextTab\n")
		_T("BackDirHist/ForwardDirHist\n")
		_T("BorderLeft/BorderRight\n")
		_T("ScrollUpLog/ScrollDownLog\n")
		_T("ScrollUpText/ScrollDownText\n")
		_T("SetSubSize_-16/SetSubSize_+16\n")
		_T("ZoomIn/ZoomOut\n")
		_T("PrevNyanFi/NextNyanFi\n"));
	WheelCmdF1ComboBox->Items->Assign(WheelCmdF0ComboBox->Items);
	WheelCmdF2ComboBox->Items->Assign(WheelCmdF0ComboBox->Items);
	WheelCmdF3ComboBox->Items->Assign(WheelCmdF0ComboBox->Items);

	set_ComboBoxText(WheelCmdV0ComboBox,
		_T("ScrollUp/ScrollDown\n")
		_T("ScrollCursorUp/ScrollCursorDown\n")
		_T("CursorUp/CursorDown\n")
		_T("PageUp/PageDown\n")
		_T("ZoomIn/ZoomOut\n")
		_T("PrevFile/NextFile\n")
		_T("PrevNyanFi/NextNyanFi\n"));
	WheelCmdV1ComboBox->Items->Assign(WheelCmdV0ComboBox->Items);
	WheelCmdV2ComboBox->Items->Assign(WheelCmdV0ComboBox->Items);
	WheelCmdV3ComboBox->Items->Assign(WheelCmdV0ComboBox->Items);

	set_ComboBoxText(WheelCmdI0ComboBox,
		_T("PrevFile/NextFile\n")
		_T("PrevPage/NextPage\n")
		_T("ScrollUp/ScrollDown\n")
		_T("ScrollLeft/ScrollRight\n")
		_T("ZoomIn/ZoomOut\n")
		_T("RotateLeft/RotateRight\n")
		_T("PrevNyanFi/NextNyanFi\n"));
	WheelCmdI1ComboBox->Items->Assign(WheelCmdI0ComboBox->Items);
	WheelCmdI2ComboBox->Items->Assign(WheelCmdI0ComboBox->Items);
	WheelCmdI3ComboBox->Items->Assign(WheelCmdI0ComboBox->Items);

	//�}�E�X�{�^��
	set_ComboBoxText(WheelBtnF0ComboBox,
		_T("ToRoot\n")
		_T("ToParent\n")
		_T("BackDirHist\n")
		_T("ForwardDirHist\n")
		_T("EqualListWidth\n")
		_T("HomeWorkList\n")
		_T("ZoomReset\n"));
	WheelBtnF1ComboBox->Items->Assign(WheelBtnF0ComboBox->Items);
	WheelBtnF2ComboBox->Items->Assign(WheelBtnF0ComboBox->Items);
	WheelBtnF3ComboBox->Items->Assign(WheelBtnF0ComboBox->Items);

	set_ComboBoxText(X1BtnFComboBox,
		_T("BackDirHist\n")
		_T("ForwardDirHist\n")
		_T("ToParent\n")
		_T("PrevDrive\n")
		_T("NextDrive\n")
		_T("PrevTab\n")
		_T("NextTab\n")
		_T("PageUp\n")
		_T("PageDown\n")
		_T("NextNyanFi\n")
		_T("PrevNyanFi\n"));
	X2BtnFComboBox->Items->Assign(X1BtnFComboBox->Items);

	set_ComboBoxText(WheelBtnVComboBox,
		_T("ZoomReset\n")
		_T("TextTop\n")
		_T("TextEnd\n")
		_T("Close\n"));

	set_ComboBoxText(X1BtnVComboBox,
		_T("BackViewHist\n")
		_T("PrevFile\n")
		_T("NextFile\n")
		_T("PageUp\n")
		_T("PageDown\n")
		_T("NextNyanFi\n")
		_T("PrevNyanFi\n"));
	X2BtnVComboBox->Items->Assign(X1BtnVComboBox->Items);

	set_ComboBoxText(WheelBtnIComboBox,
		_T("FittedSize\n")
		_T("FullScreen\n")
		_T("TopFile\n")
		_T("EndFile\n")
		_T("Close\n"));

	set_ComboBoxText(X1BtnIComboBox,
		_T("PrevFile\n")
		_T("NextFile\n")
		_T("PrevPage\n")
		_T("NextPage\n")
		_T("Histogram\n")
		_T("Loupe\n")
		_T("Thumbnail\n")
		_T("ThumbnailEx\n")
		_T("WarnHighlight\n")
		_T("NextNyanFi\n")
		_T("PrevNyanFi\n"));
	X2BtnIComboBox->Items->Assign(X1BtnIComboBox->Items);

	set_ComboBoxText(MenuCmdComboBox,
		_T("MaskSel      �}�X�N�ɊY������t�@�C����I��\n")
		_T("MaskDel      �}�X�N�ɊY������t�@�C�����폜\n")
		_T("MaskCopy     �}�X�N�ɊY������t�@�C�����R�s�[\n")
		_T("MaskMove     �}�X�N�ɊY������t�@�C�����ړ�\n")
		_T("MaskPack     �}�X�N�ɊY������t�@�C�������k\n")
		_T("ExeCommands  �w�肵���R�}���h�����s\n"));
	RefMenuPrmBtn->Enabled = false;

	make_KeyList(KeyComboBox->Items);
	make_KeyList(Key2ComboBox->Items);
	Key2ComboBox->Items->Insert(0, EmptyStr);
	Key2ComboBox->Enabled = false;

	make_KeyList(HotKeyComboBox->Items);
	HotKeyComboBox->Items->Insert(0, EmptyStr);
	make_KeyList(AppKeyComboBox->Items);
	AppKeyComboBox->Items->Insert(0, EmptyStr);
	set_ComboBoxText(AppPrmComboBox,
		_T("\n")
		_T("FA : �ꗗ��\n")
		_T("FL : �����`���[��\n")
		_T("FI : INC.�T�[�`\n")
		_T("AO : �ꗗ�̂ݕ\��\n")
		_T("LO : �����`���[�̂ݕ\��\n")
		_T("LI : �����`���[�̂�(INC.)\n"));

	set_ComboBoxText(OpenByModeComboBox, _T("���̂܂�\nWindows�̊֘A�t���ŊJ��\n�W���� Enter �L�[����\n"));

	set_ComboBoxText(DblClickFlComboBox,
		_T("�W���� Enter �L�[���� (OpenStandard)\n")
		_T("Windows�̊֘A�t���ŊJ�� (OpenByWin)\n")
		_T("�Ǝ��̊֘A�t���ŊJ�� (OpenByApp)\n"));

	set_ComboBoxText(DefDropComboBox, _T("Windows �W��\n��ɃR�s�[\n��Ɉړ�\n"));

	set_ComboBoxText(StdCmdComboBox,
		_T("Windows�̊֘A�t���ŊJ�� (OpenByWin)\n")
		_T("�Ǝ��̊֘A�t���ŊJ�� (OpenByApp)\n")
		_T("�e�L�X�g�r���A�[�ŊJ�� (TextViewer)\n")
		_T("�G�f�B�^�ŊJ�� (FileEdit)\n")
		_T("�o�C�i���G�f�B�^�ŊJ�� (BinaryEdit)\n")
		_T("�R�}���h�t�@�C���Ƃ��Ď��s (ExeCommands)\n")
		_T("�w��R�}���h�����s (ExeCommands_)\n"));

	LayoutChanged  = false;
	inh_KeySet	   = 0;

	UnicodeString hntstr   = LoadUsrMsg(USTR_HintMltSepSC);
	NoWatchPathEdit->Hint  = hntstr;
	NoInfPathEdit->Hint    = hntstr;
	NoImgPrvPathEdit->Hint = hntstr;
	InsHrClsEdit->Hint	   = hntstr;
	NoDirHistPathEdit->Hint= hntstr;
	MarkPathEdit->Hint	   = hntstr;

	hntstr = LoadUsrMsg(USTR_HintMltFExt);
	ExtColorEdit->Hint	   = hntstr;
	FExtGetInfEdit->Hint   = hntstr;
	FExtNoInfEdit->Hint    = hntstr;
	FExtImgPrvEdit->Hint   = hntstr;
	FExtNoImgPrvEdit->Hint = hntstr;
	FExtNoIViewEdit->Hint  = hntstr;
	MarkFextEdit->Hint	   = hntstr;
	FExt7zDllEdit->Hint    = hntstr;
	FExtImgEdit->Hint	   = hntstr;
	FExtEtcEdit->Hint	   = hntstr;
	ViewTab4Edit->Hint	   = hntstr;
	FExtExeFileEdit->Hint  = hntstr;
	FExtStdEdit->Hint	   = hntstr;

	//�R���g���[���̃^�O�ɁA�Ή�����ϐ��̃|�C���^��ݒ�
	//UnicodeString
	L_IniPatEdit->Tag			= (int)&InitialPath[0];
	R_IniPatEdit->Tag			= (int)&InitialPath[1];
	TextEditorEdit->Tag 		= (int)&TextEditor;
	TextEditorFrmtEdit->Tag 	= (int)&TextEditorFrmt;
	ExtTxViewerEdit->Tag		= (int)&ExtTxViewer;
	ExtTxViewerFmtEdit->Tag 	= (int)&ExtTxViewerFrmt;
	ImageEditorEdit->Tag		= (int)&ImageEditor;
	FExtImgEdit->Tag			= (int)&FExtImgEidt;
	BinaryEditorEdit->Tag		= (int)&BinaryEditor;
	SndWarnEdit->Tag			= (int)&SoundWarning;
	SndTaskFinEdit->Tag 		= (int)&SoundTaskFin;
	SndFindFinEdit->Tag 		= (int)&SoundFindFin;
	SndWatchEdit->Tag			= (int)&SoundWatch;
	DirBraEdit->Tag 			= (int)&DirBraStr;
	DirKetEdit->Tag 			= (int)&DirKetStr;
	DrvInfFmtREdit->Tag 		= (int)&DrvInfFmtR;
	DrvInfFmtSEdit->Tag 		= (int)&DrvInfFmtS;
	DrvInfFmtNEdit->Tag 		= (int)&DrvInfFmtN;
	SttBarFmtEdit->Tag			= (int)&SttBarFmt;
	ClockFmtEdit->Tag			= (int)&SttClockFmt;
	TmStampFmtEdit->Tag 		= (int)&TimeStampFmt;
	ViewTab4Edit->Tag			= (int)&FExtViewTab4;
	EmpBin1Edit->Tag			= (int)&EmpBinPtn1;
	EmpBin2Edit->Tag			= (int)&EmpBinPtn2;
	EmpBin3Edit->Tag			= (int)&EmpBinPtn3;
	InsHrClsEdit->Tag			= (int)&HtmInsHrCls;
	HdrStrEdit->Tag				= (int)&HtmHdrStr;
	FExtGetInfEdit->Tag 		= (int)&FExtGetInf;
	FExtNoInfEdit->Tag			= (int)&FExtNoInf;
	NoWatchPathEdit->Tag		= (int)&NoWatchPath;
	NoInfPathEdit->Tag			= (int)&NoInfPath;
	NoImgPrvPathEdit->Tag		= (int)&NoImgPrvPath;
	FExtImgPrvEdit->Tag 		= (int)&FExtImgPrv;
	FExtNoImgPrvEdit->Tag		= (int)&FExtNoImgPrv;
	FExtNoIViewEdit->Tag		= (int)&FExtNoIView;
	MarkPathEdit->Tag			= (int)&MarkImgPath;
	MarkFextEdit->Tag			= (int)&MarkImgFExt;
	MarkMemoEdit->Tag			= (int)&MarkImgMemo;
	FExt7zDllEdit->Tag			= (int)&FExt7zDll;
	AutoRenFmtEdit->Tag			= (int)&AutoRenFmt;
	NoDirHistPathEdit->Tag		= (int)&NoDirHistPath;
	FExtExeFileEdit->Tag		= (int)&FExtExeFile;
	HomeWorkListEdit->Tag		= (int)&HomeWorkList;
	WebSeaUrlEdit->Tag			= (int)&WebSeaUrl;
	BgImg1Edit->Tag				= (int)&BgImgName[0];
	BgImg2Edit->Tag				= (int)&BgImgName[1];
	SpImgEdit->Tag				= (int)&BgImgName[2];
	SpImg2Edit->Tag				= (int)&BgImgName[3];
	KnobImgEdit->Tag			= (int)&BgImgName[4];
	TimerCnd1Edit->Tag			= (int)&Timer_Condition[0];
	TimerCmd1Edit->Tag			= (int)&OnTimerEvent[0];
	TimerCnd2Edit->Tag			= (int)&Timer_Condition[1];
	TimerCmd2Edit->Tag			= (int)&OnTimerEvent[1];

	//int (NumbersOnly = true)
	BatLowLimitEdit->Tag		= (int)&BatLowerLimit;
	MaxLogLinesEdit->Tag		= (int)&MaxLogLines;
	MaxLogFilesEdit->Tag		= (int)&MaxLogFiles;
	FwCntWarnEdit->Tag			= (int)&FlashCntWarning;
	FwTmWarnEdit->Tag			= (int)&FlashTimeWarning;
	FwCntTaskFinEdit->Tag		= (int)&FlashCntTaskfin;
	FwTmTaskFinEdit->Tag		= (int)&FlashTimeTaskfin;
	NotifyTaskTimeEdit->Tag		= (int)&NotifyTaskTime;
	FrmMgnEdit->Tag				= (int)&ImgFrameMargin;
	FitMaxZoomEdit->Tag			= (int)&ImgFitMaxZoom;
	MinShowTimeEdit->Tag		= (int)&MinShowTime;
	IniWinLeftEdit->Tag			= (int)&IniWinLeft;
	IniWinTopEdit->Tag			= (int)&IniWinTop;
	MsgHintTimeEdit->Tag		= (int)&MsgHintTime;
	KeyHintTimeEdit->Tag		= (int)&KeyHintTime;
	ImgGridHEdit->Tag			= (int)&ImgGridHorzN;
	ImgGridVEdit->Tag			= (int)&ImgGridVertN;
	ThumbSizeEdit->Tag	 		= (int)&ThumbnailSize;
	ThumbBdrEdit->Tag			= (int)&ThumbBdrWidth;
	IntrPgEdit->Tag				= (int)&ImgDblMargin;
	BdrMovWdLabeledEdit->Tag	= (int)&BorderMoveWidth;
	ListInterLnEdit->Tag		= (int)&ListInterLn;
	CursorWidthEdit->Tag		= (int)&CursorWidth;
	CursorAlphaEdit->Tag		= (int)&CursorAlpha;
	ListPercentEdit->Tag		= (int)&ListPercent;
	TabWidthEdit->Tag			= (int)&FlTabWidth;
	TextInterLnEdit->Tag		= (int)&ViewTxtInterLn;
	LeftMarginEdit->Tag 		= (int)&ViewLeftMargin;
	FoldWdEdit->Tag 			= (int)&ViewFoldWidth;
	FixedLimitEdit->Tag			= (int)&ViewFixedLimit;
	ViewWheelScrLnEdit->Tag 	= (int)&ViewWheelScrLn;
	ListWheelScrLnEdit->Tag 	= (int)&ListWheelScrLn;
	PrvTxtInfLnEdit->Tag		= (int)&PrvTxtInfLn;
	PrvTxtTailLnEdit->Tag		= (int)&PrvTxtTailLn;
	PrvActTailLnEdit->Tag		= (int)&PrvActTailLn;
	WatchIntervalEdit->Tag		= (int)&WatchInterval;
	LimitSndEdit->Tag			= (int)&PlaySndLimitTime;
	LogIntervalEdit->Tag		= (int)&LogInterval;
	IniWinWdEdit->Tag			= (int)&IniWinWidth;
	IniWinHiEdit->Tag			= (int)&IniWinHeight;
	DlgMoveEdit->Tag			= (int)&DlgMovePrm;
	DlgSizeEdit->Tag			= (int)&DlgSizePrm;
	CmpDelOwCntEdit->Tag		= (int)&CmpDelOwCnt;
	MigemoMinEdit->Tag			= (int)&IncSeaMigemoMin;
	ToleranceEdit->Tag 			= (int)&TimeTolerance;
	RemoteWaitTimeEdit->Tag 	= (int)&RemoteWaitTime;
	NormalWaitTimeEdit->Tag 	= (int)&NormalWaitTime;
	NopDtctTimeEdit->Tag		= (int)&NopDtctTime;
	ModalAlphaEdit->Tag 		= (int)&ModalScrAlpha;
	BgColAlphaEdit->Tag 		= (int)&BgColAlpha;
	HideTimeEdit->Tag			= (int)&BgHideTime;
	TileIfSzEdit->Tag			= (int)&BgTileSize;
	SplitWidthEdit->Tag 		= (int)&SplitterWidth;
	SplitWidth2Edit->Tag 		= (int)&SplitterWidth2;
	IconCacheEdit->Tag			= (int)&IconCache;
	FindPathWdEdit->Tag 		= (int)&FindPathWidth;

	//bool
	DivFlistUDCheckBox->Tag 	= (int)&DivFileListUD;
	DivDirInfUDCheckBox->Tag	= (int)&DivDirInfUD;
	HideAtrCheckBox->Tag		= (int)&ShowHideAtr;
	SysAtrCheckBox->Tag 		= (int)&ShowSystemAtr;
	DispRegNameCheckBox->Tag	= (int)&DispRegName;
	UncToNetDriveCheckBox->Tag	= (int)&UncToNetDrive;
	PathInTitleBarCheckBox->Tag = (int)&PathInTitleBar;
	UseIndIconCheckBox->Tag 	= (int)&UseIndIcon;
	OmitEndOfNameCheckBox->Tag	= (int)&OmitEndOfName;
	DirJumboCheckBox->Tag		= (int)&ShowDirJumboIco;
	InZipImgCheckBox->Tag		= (int)&ShowInZipImg;
	AniGifCheckBox->Tag 		= (int)&PreviewAniGif;
	PrevTxtToInfCheckBox->Tag	= (int)&ShowTextPreview;
	PrevTailCheckBox->Tag		= (int)&ShowTailPreview;
	ViewArcInfCheckBox->Tag 	= (int)&ViewArcInf;
	ReloadActCheckBox->Tag		= (int)&ReloadOnActive;
	OpenAddDrvCheckBox->Tag 	= (int)&OpenAddedDrive;
	CheckUncCheckBox->Tag		= (int)&CheckUnc;
	ForceDelCheckBox->Tag		= (int)&ForceDel;
	RemoveCdRoCheckBox->Tag 	= (int)&RemoveCdReadOnly;
	CopyNoBuffCheckBox->Tag 	= (int)&CopyNoBuffering;
	ShowArcCopyPrgCheckBox->Tag = (int)&ShowArcCopyProg;
	DelUseTrsCheckBox->Tag		= (int)&DelUseTrash;
	NotShowNoTaskCheckBox->Tag	= (int)&NotShowNoTask;
	ShowMsgHintCheckBox->Tag	= (int)&ShowMsgHint;
	ShowKeyHintCheckBox->Tag	= (int)&ShowKeyHint;
	ShowNoKeyHintCheckBox->Tag	= (int)&ShowNoKeyHint;
	ShowCopyHintCheckBox->Tag	= (int)&ShowCopyHint;
	ShowDuplNotifyCheckBox->Tag = (int)&ShowDuplNotify;
	LogErrOnlyCheckBox->Tag 	= (int)&LogErrOnly;
	LogErrMsgCheckBox->Tag		= (int)&LogErrMsg;
	LogDebugInfCheckBox->Tag	= (int)&LogDebugInf;
	LogHideSkipCheckBox->Tag	= (int)&LogHideSkip;
	LogFullPathCheckBox->Tag	= (int)&LogFullPath;
	SaveLogCheckBox->Tag		= (int)&SaveLog;
	AppendLogCheckBox->Tag		= (int)&AppendLog;
	NotSortWLCheckBox->Tag		= (int)&NotSortWorkList;
	AutoDelWLCheckBox->Tag		= (int)&AutoDelWorkList;
	DirHistCsrPosCheckBox->Tag	= (int)&DirHistCsrPos;
	DelDplDirHistCheckBox->Tag	= (int)&DelDplDirHist;
	WorkToDirHistCheckBox->Tag  = (int)&WorkToDirHist;
	NoChkDirHistCheckBox->Tag	= (int)&NoCheckDirHist;
	ExtSavDirHistCheckBox->Tag	= (int)&ExtSaveDirHist;
	LoopFlCsrCheckBox->Tag		= (int)&LoopFilerCursor;
	FlCsrVisiCheckBox->Tag		= (int)&FlCursorVisible;
	AutoCompCheckBox->Tag		= (int)&AutoCompComboBox;
	AutoHotkeyCheckBox->Tag 	= (int)&MenuAutoHotkey;
	EscHelpCheckBox->Tag		= (int)&EscapeHelp;
	InhAltMenuCheckBox->Tag		= (int)&InhbitAltMenu;
	SelColDrvInfCheckBox->Tag	= (int)&SelColDrvInf;
	PermitCmdsCheckBox->Tag 	= (int)&PermitDotCmds;
	InheritDotCheckBox->Tag 	= (int)&InheritDotNyan;
	DotPerUserCheckBox->Tag 	= (int)&DotNyanPerUser;
	SelByMouseCheckBox->Tag 	= (int)&SelectByMouse;
	SelBaseOnlyCheckBox->Tag	= (int)&SelectBaseOnly;
	SelIconCheckBox->Tag		= (int)&SelectIconSngl;
	PriFExtColCheckBox->Tag 	= (int)&PriorFExtCol;
	ColOnlyFExtCheckBox->Tag	= (int)&ColorOnlyFExt;
	ShowIconCheckBox->Tag		= (int)&ShowIcon;
	ShowByteSizeCheckBox->Tag	= (int)&ShowByteSize;
	ShowImgPrvCheckBox->Tag 	= (int)&ShowImgPreview;
	ShowFilePropCheckBox->Tag	= (int)&ShowProperty;
	HideScrBarCheckBox->Tag 	= (int)&HideScrBar;
	ShowHeaderCheckBox->Tag 	= (int)&ShowHeader;
	FixListCheckBox->Tag		= (int)&FixListWidth;
	KeepOnResizeCheckBox->Tag	= (int)&KeepOnResize;
	KeepCurListWdCheckBox->Tag	= (int)&KeepCurListWidth;
	NoSpcFExtCheckBox->Tag		= (int)&NoSpaceFExt;
	NoClsTabBtnCheckBox->Tag	= (int)&NoClsTabBtn;
	ShowMainMenuCheckBox->Tag	= (int)&ShowMainMenu;
	HideTitMenuCheckBox->Tag	= (int)&HideTitleMenu;
	FlatInfPanelCheckBox->Tag	= (int)&FlatInfPanel;
	GrayScaleCheckBox->Tag		= (int)&BgImgGray;
	HideScrCheckBox->Tag		= (int)&BgImgHideScr;
	TileIfCheckBox->Tag 		= (int)&BgImgTileIf;
	ModalScrCheckBox->Tag		= (int)&ModalScreen;
	DlgCenterCheckBox->Tag		= (int)&DialogCenter;
	ShowLnNoCheckBox->Tag		= (int)&ShowLineNo;
	ShowLnCsrCheckBox->Tag		= (int)&ShowLineCursor;
	ShowTabCheckBox->Tag		= (int)&ShowTAB;
	ShowCrCheckBox->Tag 		= (int)&ShowCR;
	ShowRulerCheckBox->Tag		= (int)&ShowTextRuler;
	TxtSttIsBtmCheckBox->Tag	= (int)&TxtSttIsBottom;
	UseXd2txCheckBox->Tag 		= (int)&UseXd2tx;
	UseXd2txCheckBox->Enabled	= xd2tx_Available;
	AozoraCheckBox->Tag 		= (int)&ChkAozora;
	ClickableUrlCheckBox->Tag	= (int)&ClickableUrl;
	RstViewLnCheckBox->Tag		= (int)&RestoreViewLine;
	TvCsrVisiCheckBox->Tag		= (int)&TvCursorVisible;
	LimitBinCsrCheckBox->Tag	= (int)&LimitBinCsr;
	ColorHintCheckBox->Tag		= (int)&TxtColorHint;
	BinMemMapedCheckBox->Tag	= (int)&BinMemMaped;
	EmRemCheckBox->Tag			= (int)&EmpComment;
	EmStrCheckBox->Tag			= (int)&EmpStrings;
	EmRsvCheckBox->Tag			= (int)&EmpReserved;
	EmSymCheckBox->Tag			= (int)&EmpSymbol;
	EmNumCheckBox->Tag			= (int)&EmpNumeric;
	EmHdlnCheckBox->Tag 		= (int)&EmpHeadline;
	EmRubyCheckBox->Tag 		= (int)&EmpRuby;
	InsHrSctCheckBox->Tag		= (int)&HtmInsHrSct;
	InsHrArtCheckBox->Tag		= (int)&HtmInsHrArt;
	InsHrNavCheckBox->Tag		= (int)&HtmInsHrNav;
	MarkdownCheckBox->Tag		= (int)&ToMarkdown;
	RotViewImgCheckBox->Tag 	= (int)&RotViewImg;
	KeepZoomCheckBox->Tag		= (int)&KeepZoomRatio;
	ThumbNameCheckBox->Tag		= (int)&ShowThumbName;
	ThumbExifCheckBox->Tag		= (int)&ShowThumbExif;
	ThumbFExtCheckBox->Tag		= (int)&ShowThumbFExt;
	NotThumbIfArcCheckBox->Tag	= (int)&NotThumbIfArc;
	LoopCurCheckBox->Tag		= (int)&LoopViewCursor;
	HintTopEndCheckBox->Tag 	= (int)&HintTopEndImg;
	BeepTopEndCheckBox->Tag 	= (int)&BeepTopEndImg;
	ImgSttIsBtmCheckBox->Tag	= (int)&ImgSttIsBottom;
	ToolBarISideCheckBox->Tag	= (int)&ToolBarISide;
	SeekBindDirCheckBox->Tag	= (int)&SeekBindDir;
	SeekSwapNPCheckBox->Tag		= (int)&SeekSwapNxtPrv;
	OneStpInDblCheckBox->Tag	= (int)&OneStepInDblPg;
	HideCsrFullCheckBox->Tag	= (int)&HideCsrInFull;
	HideThumbCheckBox->Tag		= (int)&HideThumbInFull;
	AnimateGifCheckBox->Tag 	= (int)&AnimateGif;
	ThumbScrCheckBox->Tag		= (int)&ShowThumbScroll;
	MarkImgCheckBox->Tag		= (int)&MarkImgClose;
	UseSpiFirstCheckBox->Tag	= (int)&UseSpiFirst;
	SureCopyCheckBox->Tag		= (int)&SureCopy;
	SureMoveCheckBox->Tag		= (int)&SureMove;
	SureDeleteCheckBox->Tag 	= (int)&SureDelete;
	SureUnPackCheckBox->Tag 	= (int)&SureUnPack;
	SureExecCheckBox->Tag		= (int)&SureExec;
	SureOtherCheckBox->Tag		= (int)&SureOther;
	SureExitCheckBox->Tag		= (int)&SureExit;
	SureCmpDelCheckBox->Tag 	= (int)&SureCmpDel;
	SureCancelCheckBox->Tag 	= (int)&SureCancel;
	SureDefNoCheckBox->Tag		= (int)&SureDefNo;
	WarnPowFailCheckBox->Tag	= (int)&WarnPowerFail;
	WarnLowBatheckBox->Tag		= (int)&WarnLowBattery;
	WarnDisconCheckBox->Tag 	= (int)&WarnDisconnect;
	PrtSubCheckBox->Tag			= (int)&ProtectSubDir;
	PrtFileCheckBox->Tag		= (int)&ProtectFile;
	ImgEditSglCheckBox->Tag		= (int)&ImageEditSgl;

	AppMinCheckBox->Tag			= (int)&AppListChgMin;
	OpenCmdPromptCheckBox->Tag	= (int)&OpenByCmdPrompt;
	OpenDirByStdCheckBox->Tag	= (int)&OpenDirByStd;
	AppOnlyCurCheckBox->Tag 	= (int)&OpenOnlyCurApp;
	WinOnlyCurCheckBox->Tag 	= (int)&OpenOnlyCurWin;
	DowAfterStdCheckBox->Tag	= (int)&DownAfterOpenStd;
	OpnStdResCheckBox->Tag		= (int)&OpenStdOnResList;
	IncSeaCaseCheckBox->Tag 	= (int)&IncSeaCaseSens;
	IncSeaLoopCheckBox->Tag 	= (int)&IncSeaLoop;
	IncSea1ExitCheckBox->Tag	= (int)&IncSeaMatch1Exit;
	LimitRarCheckBox->Tag		= (int)&LimitGetRarInf;
	IniPathToTab1CheckBox->Tag	= (int)&IniPathToTab1;
	IniTabHomeAllCheckBox->Tag	= (int)&IniTabHomeAll;
	SplashCheckBox->Tag 		= (int)&ShowSplash;
	FixWinCheckBox->Tag 		= (int)&FixWinPos;
	IniSeaByNumCheckBox->Tag	= (int)&IniSeaByNum;
	IniSeaBySignCheckBox->Tag	= (int)&IniSeaBySign;
	MulInstCheckBox->Tag		= (int)&MultiInstance;
	CloseOtherCheckBox->Tag 	= (int)&CloseOthers;
	TaskTrayCheckBox->Tag		= (int)&StoreTaskTray;
	FindPathColCheckBox->Tag	= (int)&FindPathColumn;

	//UnicodeString (Style = csDropDown)
	L_IniMaskComboBox->Tag		= (int)&InitialMask[0];
	R_IniMaskComboBox->Tag		= (int)&InitialMask[1];
	WheelCmdF0ComboBox->Tag 	= (int)&WheelCmdF[0];
	WheelCmdF1ComboBox->Tag 	= (int)&WheelCmdF[1];
	WheelCmdF2ComboBox->Tag 	= (int)&WheelCmdF[2];
	WheelCmdF3ComboBox->Tag 	= (int)&WheelCmdF[3];
	WheelCmdV0ComboBox->Tag 	= (int)&WheelCmdV[0];
	WheelCmdV1ComboBox->Tag 	= (int)&WheelCmdV[1];
	WheelCmdV2ComboBox->Tag 	= (int)&WheelCmdV[2];
	WheelCmdV3ComboBox->Tag 	= (int)&WheelCmdV[3];
	WheelCmdI0ComboBox->Tag 	= (int)&WheelCmdI[0];
	WheelCmdI1ComboBox->Tag 	= (int)&WheelCmdI[1];
	WheelCmdI2ComboBox->Tag 	= (int)&WheelCmdI[2];
	WheelCmdI3ComboBox->Tag 	= (int)&WheelCmdI[3];
	WheelBtnF0ComboBox->Tag 	= (int)&WheelBtnCmdF[0];
	WheelBtnF1ComboBox->Tag 	= (int)&WheelBtnCmdF[1];
	WheelBtnF2ComboBox->Tag 	= (int)&WheelBtnCmdF[2];
	WheelBtnF3ComboBox->Tag 	= (int)&WheelBtnCmdF[3];
	WheelBtnVComboBox->Tag		= (int)&WheelBtnCmdV;
	WheelBtnIComboBox->Tag		= (int)&WheelBtnCmdI;
	X1BtnFComboBox->Tag 		= (int)&X1BtnCmdF;
	X2BtnFComboBox->Tag 		= (int)&X2BtnCmdF;
	X1BtnVComboBox->Tag 		= (int)&X1BtnCmdV;
	X2BtnVComboBox->Tag 		= (int)&X2BtnCmdV;
	X1BtnIComboBox->Tag 		= (int)&X1BtnCmdI;
	X2BtnIComboBox->Tag 		= (int)&X2BtnCmdI;

	//int (Style = csDropDownList)
	LayoutComboBox->Tag 		= (int)&LayoutMode;
	SizeFmtComboBox->Tag		= (int)&SizeFormatMode;
	TabStyleComboBox->Tag		= (int)&FlTabStyle;
	ScrBarStyleComboBox->Tag	= (int)&ScrBarStyle;
	BgImgModeComboBox->Tag		= (int)&BgImgMode;
	BgImgSubModeComboBox->Tag	= (int)&BgImgSubMode;

	ScaleOptComboBox->Tag		= (int)&WicScaleOpt;
	OpenByModeComboBox->Tag 	= (int)&OpenByMode;
	DblClickFlComboBox->Tag 	= (int)&DblClickFlMode;
	DefDropComboBox->Tag		= (int)&DefDropMode;
	SizeDecDgtComboBox->Tag 	= (int)&SizeDecDigits;

	IniModeIRadioGroup->Tag 	= (int)&InitialModeI;
	L_IniSortRadioGroup->Tag	= (int)&IniSortMode[0];
	R_IniSortRadioGroup->Tag	= (int)&IniSortMode[1];

	PrtDirRadioGroup->Tag		= (int)&ProtectDirMode;

	cursor_Default();

	//�h���b�v�^�[�Q�b�g��ݒ�
	usr_SH->AddTargetList(this, ToolExeEdit);
	usr_SH->AddTargetList(this, TextEditorEdit);
	usr_SH->AddTargetList(this, ImageEditorEdit);
	usr_SH->AddTargetList(this, BinaryEditorEdit);
	usr_SH->AddTargetList(this, EtcEditorEdit);
	usr_SH->AddTargetList(this, ExtTxViewerEdit);
	usr_SH->AddTargetList(this, AsoAppComboBox);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormShow(TObject *Sender)
{
	DlgInitialized = false;
	cursor_HourGlass();

	//INI�t�@�C�����f�t�H���g�ƈقȂ�ꍇ�A�t�@�C������\��
	UnicodeString inam = IniFile->FileName;
	UnicodeString tit = "�I�v�V�����ݒ�";
	if (!SameText(inam, ChangeFileExt(Application->ExeName, ".INI"))) tit.cat_sprintf(_T(" - %s"), to_relative_name(inam).c_str());
	Caption = tit;

	set_ListBoxItemHi(ExtColListBox);
	set_ListBoxItemHi(SpiListBox);
	set_ListBoxItemHi(EtcEditorListBox);
	set_ListBoxItemHi(AssociateListBox);
	set_ListBoxItemHi(ExtMenuListBox);
	set_ListBoxItemHi(ExtToolListBox);
	set_ListBoxItemHi(KeyListBox);
	set_ListBoxItemHi(EventListBox);
	set_ListBoxItemHi(PrtDirListBox);

	InitializeListHeader(ExtMenuHeader, _T("�L���v�V����|�G�C���A�X|�ݒ�"), Font);
	THeaderSections *sp = ExtMenuHeader->Sections;
	sp->Items[0]->Width = IniFile->ReadIntGen(_T("ExtMenuColWidth0"),	200);
	sp->Items[1]->Width = IniFile->ReadIntGen(_T("ExtMenuColWidth1"),	120);
	adjust_HeaderSecWidth(ExtMenuHeader, 2);

	InitializeListHeader(ExtToolHeader, _T("�L���v�V����|�G�C���A�X|�ݒ�"), Font);
	sp = ExtToolHeader->Sections;
	sp->Items[0]->Width = IniFile->ReadIntGen(_T("ExtToolColWidth0"),	200);
	sp->Items[1]->Width = IniFile->ReadIntGen(_T("ExtToolColWidth1"),	120);
	adjust_HeaderSecWidth(ExtToolHeader, 2);

	L_IniMaskComboBox->Clear();
	for (int i=0; i<PathMaskList->Count; i++) {
		TStringDynArray itm_buf = get_csv_array(PathMaskList->Strings[i], 3, true);
		if (itm_buf[2].IsEmpty() || USAME_TS(itm_buf[2], "*")) continue;
		UnicodeString lbuf;
		if (!itm_buf[1].IsEmpty()) lbuf.sprintf(_T(":%s:"), itm_buf[1].c_str());
		L_IniMaskComboBox->Items->Add(lbuf + itm_buf[2]);
	}
	R_IniMaskComboBox->Items->Assign(L_IniMaskComboBox->Items);

	FindEdit->Text = EmptyStr;

	HelpContext = PageControl1->ActivePage->HelpContext;

	//�^�O��p���ăR���g���[���ɒl��ݒ�
	BringOptionByTag(this);

	//�^�O��p���Ȃ��ݒ�
	TempDirEdit->Text	= TempPath;
	MigemoDirEdit->Text	= MigemoPath;
	SusieDirEdit->Text	= SpiDir;
	LimitTxtEdit->Text	= ViewTxtLimitSize/1024;
	LimitBinEdit->Text	= ViewBinLimitSize/1048576;
	LimitRarEdit->Text	= GetInfLimitSize/1048576;
	MaxTasksComboBox->ItemIndex = MaxTasks - 1;
	AppPrmComboBox->ItemIndex	= idx_of_word_i(_T("|FA|FL|FI|AO|LO|LI"), AppListHotPrm);

	ColBufList->Assign(ColorList);
	ModalColorBox->Selected = (TColor)get_ListIntVal(ColBufList, _T("ModalScr"), clBlack);

	AssociateListBox->Items->Assign(AssociateList);
	EtcEditorListBox->Items->Assign(EtcEditorList);
	ExtColListBox->Items->Assign(ExtColList);
	StdCmdListBox->Items->Assign(OpenStdCmdList);
	PrtDirListBox->Items->Assign(ProtectDirList);

	TCheckListBox *lp = ExtMenuListBox;
	lp->Items->Assign(ExtMenuList);
	for (int i=0; i<lp->Count; i++) lp->Checked[i] = !equal_0(get_csv_item(lp->Items->Strings[i], 4));

	lp = ExtToolListBox;
	lp->Items->Assign(ExtToolList);
	for (int i=0; i<lp->Count; i++) lp->Checked[i] = !equal_0(get_csv_item(lp->Items->Strings[i], 5));

	//�L�[�ꗗ�̃w�b�_��ݒ�
	InitializeListHeader(KeyHeaderControl, _T("�L�[|�R�}���h|����"), Font);
	TCanvas *cv = KeyListBox->Canvas;
	cv->Font->Assign(KeyListBox->Font);
	set_HeaderSecWidth(KeyHeaderControl, 2, cv->TextWidth("Shift+Ctrl+Alt+Space "), get_CharWidth(cv, 40));

	KeySortMode = IniFile->ReadIntGen(_T("OptKeySortMode"),	0);
	KeyFuncList->CustomSort(KeyComp_Key);

	for (int i=0; i<MAX_KEYTABS; i++) {
		KeyListBuf[i]->Clear();
		UnicodeString modstr = GetCmdModeStr(i + 1);
		for (int j=0; j<KeyFuncList->Count; j++){
			UnicodeString lbuf = KeyFuncList->Strings[j];
			if (StartsText(modstr, lbuf)) KeyListBuf[i]->Add(lbuf);
		}
	}
	FKeyLabelBuf->Assign(FKeyLabelList);

	PageControl1Change(NULL);

	(ViewFoldFitWin? FitFoldRadioBtn : SetFoldRadioBtn)->Checked = true;

	switch (IniPathMode[0]) {
	case 1:  L_IniPatMod1RadioBtn->Checked = true; break;
	case 2:  L_IniPatMod2RadioBtn->Checked = true; break;
	default: L_IniPatMod0RadioBtn->Checked = true;
	}

	switch (IniPathMode[1]) {
	case 1:  R_IniPatMod1RadioBtn->Checked = true; break;
	case 2:  R_IniPatMod2RadioBtn->Checked = true; break;
	default: R_IniPatMod0RadioBtn->Checked = true;
	}

	((IniWorkMode==1)? IniWork1RadioBtn : IniWork0RadioBtn)->Checked = true;
	((IniWinMode==1)?  IniWin1RadioBtn  : IniWin0RadioBtn)->Checked  = true;

	set_MigemoCheckBox(MigemoCheckBox, _T("OptKeyMigemo"));

	UnRegCmdCheckBox->Checked = IniFile->ReadBoolGen(_T("OptUnregCmd"));
	SetCmdCombo();

	AsoSortCheckBox->Checked  = IniFile->ReadBoolGen(_T("OptAsoSort"));

	DlgMoveShiftCheckBox->Checked = ContainsText(DlgMoveShift, KeyStr_Shift);
	DlgMoveCtrlCheckBox->Checked  = ContainsText(DlgMoveShift, KeyStr_Ctrl);
	DlgMoveAltCheckBox->Checked   = ContainsText(DlgMoveShift, KeyStr_Alt);
	DlgSizeShiftCheckBox->Checked = ContainsText(DlgSizeShift, KeyStr_Shift);
	DlgSizeCtrlCheckBox->Checked  = ContainsText(DlgSizeShift, KeyStr_Ctrl);
	DlgSizeAltCheckBox->Checked   = ContainsText(DlgSizeShift, KeyStr_Alt);

	UnicodeString kstr = CallHotKey;
	HotShiftCheckBox->Checked	  = remove_text(kstr, KeyStr_Shift);
	HotCtrlCheckBox->Checked	  = remove_text(kstr, KeyStr_Ctrl);
	HotAltCheckBox->Checked 	  = remove_text(kstr, KeyStr_Alt);
	HotWinCheckBox->Checked 	  = remove_text(kstr, _T("Win+"));
	HotKeyComboBox->ItemIndex	  = HotKeyComboBox->Items->IndexOf(kstr);

	kstr = AppListHotKey;
	AppShiftCheckBox->Checked	  = remove_text(kstr, KeyStr_Shift);
	AppCtrlCheckBox->Checked	  = remove_text(kstr, KeyStr_Ctrl);
	AppAltCheckBox->Checked 	  = remove_text(kstr, KeyStr_Alt);
	AppWinCheckBox->Checked 	  = remove_text(kstr, _T("Win+"));
	AppKeyComboBox->ItemIndex	  = AppKeyComboBox->Items->IndexOf(kstr);

	IniSeaShiftCheckBox->Checked  = ContainsText(IniSeaShift, KeyStr_Shift);
	IniSeaCtrlCheckBox->Checked   = ContainsText(IniSeaShift, KeyStr_Ctrl);
	IniSeaAltCheckBox->Checked	  = ContainsText(IniSeaShift, KeyStr_Alt);

	UpdateSpiListBox();

	//�C�x���g
	EventListBox->Clear();
	for (int i=0; i<MAX_EVENT_CMD; i++) {
		EventListBox->Items->Add(
			UnicodeString().sprintf(_T("%s\t%s"), EventCmdList[i].dsc, EventCmdList[i].sp->c_str()));
	}
	EventCmdsEdit->Text = EmptyStr;

	LayoutChanged	= false;
	WinSizeChanged	= false;
	TlBarColChanged = false;

	KeyWd = EmptyStr;

	FExt7zDllEdit->Enabled = usr_ARC->Use7zDll;

	UpdateMaxItemWidth();

	cursor_Default();
	DlgInitialized = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormClose(TObject *Sender, TCloseAction &Action)
{
	if (ScrMode==SCMD_FLIST) OptionPageIndex = PageControl1->ActivePageIndex;

	IconImgListP->Clear();
	UserModule->UninitializeListBox();

	THeaderSections *sp = ExtMenuHeader->Sections;
	IniFile->WriteIntGen(_T("ExtMenuColWidth0"),	sp->Items[0]->Width);
	IniFile->WriteIntGen(_T("ExtMenuColWidth1"),	sp->Items[1]->Width);
	sp = ExtToolHeader->Sections;
	IniFile->WriteIntGen(_T("ExtToolColWidth0"),	sp->Items[0]->Width);
	IniFile->WriteIntGen(_T("ExtToolColWidth1"),	sp->Items[1]->Width);

	IniFile->WriteIntGen(_T("OptKeySortMode"),		KeySortMode);
	IniFile->WriteBoolGen(_T("OptKeyMigemo"),		MigemoCheckBox);
	IniFile->WriteBoolGen(_T("OptUnregCmd"),		UnRegCmdCheckBox);
	IniFile->WriteBoolGen(_T("OptAsoSort"),			AsoSortCheckBox);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FormDestroy(TObject *Sender)
{
	delete FindMarkList;
	delete ColBufList;

	for (int i=0; i<MAX_KEYTABS; i++) delete KeyListBuf[i];
	delete FKeyLabelBuf;
}

//---------------------------------------------------------------------------
//���b�v�󂯓���
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::WmDropped(TMessage &msg)
{
	TWinControl *cp = (TWinControl *)msg.WParam;
	if (cp==ToolExeEdit)
		SetExtToolExeName(DroppedList->Strings[0]);
	else
		SetExtNameToCtrl(DroppedList->Strings[0], cp, cp==AsoAppComboBox);
}

//---------------------------------------------------------------------------
//���݂̑Ώۃ��X�g�{�b�N�X���擾
//---------------------------------------------------------------------------
TCustomListBox* __fastcall TOptionDlg::GetCurListBox()
{
	if (PageControl1->ActivePage==ExtMenuSheet) return ExtMenuListBox;
	if (PageControl1->ActivePage==ExtToolSheet) return ExtToolListBox;
	if (PageControl1->ActivePage==FontColSheet) return ExtColListBox;
	if (PageControl1->ActivePage==EditorSheet)  return EtcEditorListBox;
	if (PageControl1->ActivePage==AssoSheet)    return AssociateListBox;
	if (PageControl1->ActivePage==CommadSheet)  return StdCmdListBox;
	if (PageControl1->ActivePage==NotifySheet)  return PrtDirListBox;
												return NULL;
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::UpdateMaxItemWidth()
{
	TCanvas *cv = AssociateListBox->Canvas;
	cv->Font->Assign(DialogFont);
	MaxWd_AssExt = 0;
	for (int i=0; i<AssociateListBox->Count; i++)
		MaxWd_AssExt = std::max(cv->TextWidth(AssociateListBox->Items->Names[i]), MaxWd_AssExt);
	MaxWd_AssExt += ScaledInt(20);

	cv = EventListBox->Canvas;
	cv->Font->Assign(DialogFont);
	MaxWd_Ev = 0;
	for (int i=0; i<MAX_EVENT_CMD; i++)
		MaxWd_Ev = std::max(cv->TextWidth(EventCmdList[i].dsc), MaxWd_Ev);
	MaxWd_Ev += ScaledInt(20);
}

//---------------------------------------------------------------------------
//�L�[���X�g�̍쐬
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::make_KeyList(TStrings *lst)
{
	lst->Clear();

	UnicodeString itmbuf;
	for (int i=0; i<26; i++) lst->Add(itmbuf.sprintf(_T("%c"), 'A' + i));	//�p��
	for (int i=0; i<10; i++) lst->Add(itmbuf.sprintf(_T("%c"), '0' + i));	//����

	for (int i=0; i<12; i++) lst->Add(itmbuf.sprintf(_T("F%u"), i + 1));	//F�L�[
	lst->AddStrings(
		SplitString("Del|Ins|BkSp|Left|Right|UP|Down|PgUp|PgDn|Home|End|Pause|Tab|Esc|Enter|Space|App", "|"));

	if (is_JpKeybd())
		lst->AddStrings(SplitString("-|^|\\|@|[|;|:|]|,|.|/|�_", "|"));		//JP
	else
		lst->AddStrings(SplitString("`|-|��|[|]|\\|;|'|,|.|/", "|"));		//US

	for (int i=0; i<10; i++) lst->Add(itmbuf.sprintf(_T("10Key_%u"), i));	//10�L�[
	lst->AddStrings(SplitString("10Key_*|10Key_+|10Key_-|10Key_/|10Key_.", "|"));
}

//---------------------------------------------------------------------------
UnicodeString __fastcall TOptionDlg::GetCmdModeStr(int idx)
{
	if (idx==0) idx = KeyTabControl->TabIndex + 1;
	return (idx>=1 && idx<=MAX_KEYTABS)? ScrModeIdStr.SubString(idx, 1).UCAT_T(":") : EmptyStr;
}
//---------------------------------------------------------------------------
UnicodeString __fastcall TOptionDlg::GetCmdKeyStr()
{
	UnicodeString kstr = GetCmdModeStr();
	if (SelCheckBox->Checked)   kstr += KeyStr_SELECT;
	if (ShiftCheckBox->Checked) kstr += KeyStr_Shift;
	if (CtrlCheckBox->Checked)  kstr += KeyStr_Ctrl;
	if (AltCheckBox->Checked)   kstr += KeyStr_Alt;
	kstr += KeyComboBox->Text;
	if (!Key2ComboBox->Text.IsEmpty()) kstr.cat_sprintf(_T("~%s"), Key2ComboBox->Text.c_str());
	return kstr;
}

//---------------------------------------------------------------------------
//2�X�g���[�N�J�n�L�[��?
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::IsFirstCmdKey()
{
	bool res = false;

	UnicodeString key = get_tkn_r(GetCmdKeyStr(), ':');
	if (!contains_s(key, _T('~'))) {
		TStrings *sp = KeyListBox->Items;
		for (int i=0; i<sp->Count; i++) {
			UnicodeString fkey = sp->Names[i];
			if (contains_s(fkey, _T('~'))) {
				fkey = get_tkn_m(fkey, ':', '~');
				if (SameText(key, fkey)) { res = true; break; }
			}
		}
	}

	return res;
}

//---------------------------------------------------------------------------
//�^�u��ݒ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SetSheet(UnicodeString prm)
{
	int idx = idx_of_word_i(_T("GN|G2|MO|DS|FC|TV|IV|ED|KY|AC|XM|XT|ST|NT|CM|EV"), prm);
	if (idx!=-1)
		PageControl1->ActivePageIndex = idx;
	else {
		if		(ScrMode==SCMD_TVIEW)	PageControl1->ActivePage = TxtViewerSheet;
		else if (ScrMode==SCMD_IVIEW)	PageControl1->ActivePage = ImgViewerSheet;
		else 							PageControl1->ActivePageIndex = OptionPageIndex;
	}
}

//---------------------------------------------------------------------------
//�ꎞ�f�B���N�g���̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefTmpDirBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("�ꎞ�f�B���N�g��"), dnam, true)) TempDirEdit->Text = dnam;
}

//---------------------------------------------------------------------------
//Migemo�f�B���N�g���̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefMigemoBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("Migemo �f�B���N�g��"), dnam, true)) MigemoDirEdit->Text = dnam;
}

//---------------------------------------------------------------------------
//���̑��̃G�f�B�^�ꗗ�̕`��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EtcEditorListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);
	int xp = Rect.Left + 4;
	int yp = Rect.Top + get_TopMargin(cv);

	int w_x = 0;
	for (int i=0; i<lp->Count; i++) w_x = std::max(cv->TextWidth(lp->Items->Names[i]), w_x);

	UnicodeString etc_fext = lp->Items->Names[Index];
	UnicodeString etc_edtr = exclude_quot(lp->Items->ValueFromIndex[Index]);

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);
	cv->TextOut(xp, yp, etc_fext);
	cv->TextOut(xp + w_x + 20, yp, etc_edtr);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EtcEditorListBoxClick(TObject *Sender)
{
	TListBox *lp = (TListBox*)Sender;
	int idx = lp->ItemIndex;
	if (idx!=-1) {
		FExtEtcEdit->Text	= lp->Items->Names[idx];
		EtcEditorEdit->Text = exclude_quot(lp->Items->ValueFromIndex[idx]);
	}
}

//---------------------------------------------------------------------------
//�G�f�B�^/�r���A�[�̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefEditorBtnClick(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;
	UnicodeString tit = get_word_i_idx(
		_T("�e�L�X�g�G�f�B�^|�C���[�W�G�f�B�^|�o�C�i���G�f�B�^|���̑��̃G�f�B�^|�O���e�L�X�g�r���A�["), tag);
	UserModule->PrepareOpenDlg(tit.c_str(), F_FILTER_EXE2);
	UnicodeString fnam;
	if (UserModule->OpenDlgToStr(fnam)) {
		switch (tag) {
		case 0: SetExtNameToCtrl(fnam, TextEditorEdit);		break;
		case 1: SetExtNameToCtrl(fnam, ImageEditorEdit);	break;
		case 2: SetExtNameToCtrl(fnam, BinaryEditorEdit);	break;
		case 3: SetExtNameToCtrl(fnam, EtcEditorEdit);		break;
		case 4: SetExtNameToCtrl(fnam, ExtTxViewerEdit);	break;
		}
	}
}

//---------------------------------------------------------------------------
//�^�X�N�I�����̒ʒm���̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefSoundBtnClick(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;

	UnicodeString inidir = def_if_empty(ExtractFileDir(rel_to_absdir((tag==0)? SndWarnEdit->Text : SndTaskFinEdit->Text)), ExePath);
	UserModule->PrepareOpenDlg(_T("�T�E���h�t�@�C���̎w��"), F_FILTER_WAV, NULL, inidir);

	UnicodeString fnam;
	if (UserModule->OpenDlgToStr(fnam, true)) {
		switch (tag) {
		case 0: SndWarnEdit->Text    = fnam;	break;
		case 1: SndTaskFinEdit->Text = fnam;	break;
		case 2: SndFindFinEdit->Text = fnam;	break;
		case 3: SndWatchEdit->Text	 = fnam;	break;
		}
		play_sound(UserModule->OpenDlg->FileName);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::TestSoundBtnClick(TObject *Sender)
{
	switch (((TComponent*)Sender)->Tag) {
	case 0: play_sound(SndWarnEdit->Text);		break;
	case 1: play_sound(SndTaskFinEdit->Text);	break;
	case 2: play_sound(SndFindFinEdit->Text);	break;
	case 3: play_sound(SndWatchEdit->Text);		break;
	}
}

//---------------------------------------------------------------------------
//�S�̗����t�@�C���ݒ�̕ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtSavDirHistCheckBoxClick(TObject *Sender)
{
	if (!DlgInitialized) return;

	if (ExtSavDirHistCheckBox->Checked)	{
		UnicodeString fnam = UAPP_T(ExePath, DIR_HIST_FILE);
		if (file_exists(fnam) && msgbox_Sure(_T("�����̗����t�@�C����ǂݍ��݂܂����H"))) {
			std::unique_ptr<UsrIniFile> hst_file(new UsrIniFile(fnam));
			hst_file->LoadListItems("AllDirHistory", AllDirHistory, 0, false);
		}
	}
}

//---------------------------------------------------------------------------
//���C�A�E�g�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::LayoutChange(TObject *Sender)
{
	LayoutChanged = true;
}

//---------------------------------------------------------------------------
//�w�i�摜�̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefBgImgBtnClick(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;
	TLabeledEdit *ep = (tag==0)? BgImg1Edit : (tag==1)? BgImg2Edit : (tag==2)? SpImgEdit : SpImg2Edit;
	UserModule->OpenImgDlg->Title = "�w�i�摜�̎w��";
	UserModule->SetOpenImgFilter(ep->Text);
	UserModule->OpenImgDlg->InitialDir = rel_to_absdir(ExtractFilePath(ep->Text));
	UserModule->OpenImgDlg->FileName   = EmptyStr;
	UserModule->OpenImgDlgToEdit(ep, true);
}
//---------------------------------------------------------------------------
//�V���v���X�N���[���m�u�摜�̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ScrBarStyleComboBoxClick(TObject *Sender)
{
	if (ScrBarStyleComboBox->ItemIndex==4) {
		UserModule->OpenImgDlg->Title = "�m�u�摜�̎w��";
		UserModule->SetOpenImgFilter(KnobImgEdit->Text);
		UserModule->OpenImgDlg->InitialDir = rel_to_absdir(ExtractFilePath(KnobImgEdit->Text));
		UserModule->OpenImgDlg->FileName   = EmptyStr;
		if (!UserModule->OpenImgDlgToEdit(KnobImgEdit, true)) {
			ScrBarStyleComboBox->ItemIndex = 0;
			KnobImgEdit->Text = EmptyStr;
		}
	}
	else
		KnobImgEdit->Text = EmptyStr;
}

//---------------------------------------------------------------------------
//�t�H���g
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FontComboBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TComboBox *cp = (TComboBox*)Control;
	TCanvas   *cv = cp->Canvas;
	cv->Font->Assign(cp->Font);
	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);
	int fh = abs(cv->Font->Height);

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);
	cv->TextOut(xp, yp, cp->Items->Strings[Index]);
	xp += fh*10;

	if (Index<FontList->Count) {
		TFont *f = (TFont*)FontList->Objects[Index];
		//�T�C�Y
		UnicodeString szstr = f->Size;
		cv->TextOut(xp + get_CharWidth(cv, 2) - cv->TextWidth(szstr), yp, szstr);
		xp += fh*2;
		//���O
		cv->Font->Assign(f);
		cv->Font->Size = cp->Font->Size;
		SetHighlight(cv, State.Contains(odSelected));
		yp = Rect.Top + get_TopMargin(cv);
		cv->TextOut(xp, yp, f->Name);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefFontBtnClick(TObject *Sender)
{
	int idx = FontComboBox->ItemIndex;
	if (idx>=0 && idx<FontComboBox->Items->Count && idx<FontList->Count) {
		TFont *f = (TFont*)FontList->Objects[idx];
		UserModule->FontDlg->Options.Clear();
		if (USAME_TI(FontList->ValueFromIndex[idx], "�e�L�X�g�r���A�["))
			UserModule->FontDlg->Options << fdFixedPitchOnly;	//�e�L�X�g�r���A�[�͓���

		if (UserModule->FontDlgToFont(f)) {
			//�_�C�A���O
			if (USAME_TI(FontList->ValueFromIndex[idx], "�_�C�A���O")) {
				if (f->Size>10) f->Size = 10;
				Application->DefaultFont->Assign(f);
				UpdateMaxItemWidth();
			}
			//���̑�
			else FontComboBox->Repaint();
		}
	}
}

//---------------------------------------------------------------------------
//�z�F
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OptColListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	draw_ColorListBox((TListBox*)Control, Rect, Index, State, ColBufList);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefColBtnClick(TObject *Sender)
{
	TListBox *lp = (((TButton*)Sender)->Tag==1)? TimColListBox : OptColListBox;
	int idx = lp->ItemIndex;	if (idx==-1) return;

	UnicodeString col_nam = lp->Items->Names[idx];
	UserModule->ColorDlg->Color = (TColor)ColBufList->Values[col_nam].ToIntDef(clBlack);
	if (UserModule->ColorDlg->Execute()) {
		ColBufList->Values[col_nam] = IntToStr(UserModule->ColorDlg->Color);
		lp->Repaint();
		if (contained_wd_i(_T("bgTlBar1|bgTlBar2"), col_nam)) TlBarColChanged = true;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ModalColorBoxChange(TObject *Sender)
{
	ColBufList->Values["ModalScr"] = IntToStr(ModalColorBox->Selected);
}

//---------------------------------------------------------------------------
//�z�F�̖�����
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DisableColActionExecute(TObject *Sender)
{
	TListBox *lp = OptColListBox;
	if (lp->ItemIndex!=-1) {
		ColBufList->Values[lp->Items->Names[lp->ItemIndex]] = IntToStr(clNone);
		lp->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DisableColActionUpdate(TObject *Sender)
{
	TListBox *lp = OptColListBox;
	UnicodeString col_id = (lp->ItemIndex!=-1)? lp->Items->Names[lp->ItemIndex] : EmptyStr;

	((TAction*)Sender)->Enabled
		= contained_wd_i(_T("bgList2|fgSelItem|Protect|frScrKnob|frmTab|bdrLine|bdrFold|bdrFixed"), col_id);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DisableCol2ActionExecute(TObject *Sender)
{
	TListBox *lp = TimColListBox;
	if (lp->ItemIndex!=-1) {
		ColBufList->Values[lp->Items->Names[lp->ItemIndex]] = IntToStr(clNone);
		lp->Invalidate();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DisableCol2ActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = TimColListBox->ItemIndex!=-1;
}

//---------------------------------------------------------------------------
//�X�|�C�g
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SpuitImageMouseDown(TObject *Sender, TMouseButton Button,
		TShiftState Shift, int X, int Y)
{
	if (Button!=mbLeft) return;

	TImage *ip = (TImage*)Sender;
	SpuitTag = ip->Tag;
	if (SpuitTag!=2) {
		TListBox *cp = (SpuitTag==1)? TimColListBox : OptColListBox;
		if (cp->ItemIndex==-1) return;
	}

	UserModule->BeginSpuit((SpuitTag==2)? SpuitPanel3 : (SpuitTag==1)? SpuitPanel2 : SpuitPanel, ip);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SpuitImageMouseUp(TObject *Sender, TMouseButton Button,
		TShiftState Shift, int X, int Y)
{
	if (UserModule->SpuitEnabled()) {
		TColor col = UserModule->EndSpuit();
		if (SpuitTag==2)
			RefExtColPanel->Color = col;
		else {
			TListBox *lp = (SpuitTag==1)? TimColListBox : OptColListBox;
			if (lp->ItemIndex!=-1) {
				UnicodeString col_nam = lp->Items->Names[lp->ItemIndex];
				ColBufList->Values[col_nam] = IntToStr(col);
				lp->Repaint();
				if (contained_wd_i(_T("bgTlBar1|bgTlBar2"), col_nam)) TlBarColChanged = true;
			}
		}
	}
}
//---------------------------------------------------------------------------
//�����x����
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ModalAlphaEditChange(TObject *Sender)
{
	TLabeledEdit *ep = (TLabeledEdit*)Sender;
	if (EditToInt(ep, 0)>255) ep->Text = 255;
}

//---------------------------------------------------------------------------
//�g���q�ʔz�F
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtColListBoxClick(TObject *Sender)
{
	int idx = ExtColListBox->ItemIndex;  if (idx==-1) return;
	UnicodeString lbuf = ExtColListBox->Items->Strings[idx];
	RefExtColPanel->Color = (TColor)split_tkn(lbuf, ',').ToIntDef(clNone);
	ExtColorEdit->Text	  = lbuf;
}

//---------------------------------------------------------------------------
//�g���q�ʔz�F�ꗗ�̕`��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtColListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);
	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);

	UnicodeString lbuf = lp->Items->Strings[Index];
	cv->Font->Color = (TColor)split_tkn(lbuf, ',').ToIntDef(clNone);
	bool is_dot = USAME_TS(lbuf, ".");
	UnicodeString ext = REPLACE_TS(lbuf, ".", " .");
	cv->Brush->Color = col_bgList;

	UnicodeString smpl_str;
	smpl_str.sprintf(_T("%s"), is_dot? _T("hogehoge") : _T("hoge.???"));
	int smpl_wd = cv->TextWidth(smpl_str) + 10;
	TRect rc = Rect;  rc.Right = rc.Left + smpl_wd;
	cv->FillRect(rc);
	cv->TextOut(xp + 4, yp, smpl_str);

	SetHighlight(cv, State.Contains(odSelected));
	rc = Rect;  rc.Left += smpl_wd;  xp += smpl_wd;
	cv->FillRect(rc);
	cv->TextOut(xp + 4, yp, ext);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefExtColBtnClick(TObject *Sender)
{
	UserModule->ColorDlg->Color = RefExtColPanel->Color;
	if (UserModule->ColorDlg->Execute()) RefExtColPanel->Color = UserModule->ColorDlg->Color;
}
//---------------------------------------------------------------------------
//�g���q�ʐF�̒ǉ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddExtColActionExecute(TObject *Sender)
{
	UnicodeString s;
	s.sprintf(_T("0x%06x,%s"), (int)RefExtColPanel->Color, ExtColorEdit->Text.c_str());
	ExtColListBox->Items->Add(s);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddExtColActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !ExtColorEdit->Text.IsEmpty();
}
//---------------------------------------------------------------------------
//�g���q�ʐF�̕ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgExtColActionExecute(TObject *Sender)
{
	int idx = ExtColListBox->ItemIndex;  if (idx==-1) return;
	UnicodeString s;
	s.sprintf(_T("0x%06x,%s"), (int)RefExtColPanel->Color, ExtColorEdit->Text.c_str());
	ExtColListBox->Items->Strings[idx] = s;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgExtColActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !ExtColorEdit->Text.IsEmpty() && ExtColListBox->ItemIndex!=-1;
}

//---------------------------------------------------------------------------
//���X�g�ł̃L�[����
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OptListBoxKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	TControl *cp = (TControl*)Sender;
	UnicodeString KeyStr = get_KeyStr(Key, Shift);

	if (cp==KeyListBox) {
		if (equal_DEL(KeyStr)) DelKeyAction->Execute(); else return;
	}
	else if (cp==PrtDirListBox) {
		if (equal_DEL(KeyStr)) UserModule->DelListItemAction->Execute(); else return;
	}
	else {
		if (equal_DEL(KeyStr)) {
			if (cp==EventListBox) {
				TListBox *lp = (TListBox*)cp;
				int idx = lp->ItemIndex;
				if (idx!=-1) lp->Items->Strings[idx] = get_pre_tab(lp->Items->Strings[idx]).UCAT_T("\t");
			}
			else UserModule->DelListItemAction->Execute();
		}
		else if (!UserModule->ListBoxOpeItem(KeyStr)) return;
	}

	Key = 0;
}

//---------------------------------------------------------------------------
//Susie �v���O�C���̃f�B���N�g�����Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefSusieDirBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("Susie �v���O�C��"), dnam, true)) {
		SusieDirEdit->Text = dnam;
		delete SPI;
		SPI = new SpiUnit(rel_to_absdir(SusieDirEdit->Text));
		UpdateSpiListBox();
	}
}
//---------------------------------------------------------------------------
//Suisie �v���O�C���ꗗ���X�V
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::UpdateSpiListBox()
{
	SpiListBox->Clear();
	for (int i=0; i<SPI->PlgList->Count; i++) {
		spi_info *sp = SPI->PlgList->Items[i];
		SpiListBox->Items->Add(
			UnicodeString().sprintf(_T("%s \t[%s] %s"),
				ExtractFileName(sp->FileName).c_str(), sp->VerType.c_str(), sp->FileType.c_str()));
	}
}
//---------------------------------------------------------------------------
//Sunis �v���O�C���̐ݒ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ConfigSpiActionExecute(TObject *Sender)
{
	int idx = SpiListBox->ItemIndex;
	if (idx!=-1) {
		spi_info *sp = SPI->PlgList->Items[idx];
		if (sp->ConfigurationDlg) sp->ConfigurationDlg(this->Handle, 1);
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ConfigSpiActionUpdate(TObject *Sender)
{
	int idx = SpiListBox->ItemIndex;
	if (idx!=-1) {
		spi_info *sp = SPI->PlgList->Items[idx];
		((TAction*)Sender)->Enabled = (sp->ConfigurationDlg!=NULL);
	}
	else
		((TAction*)Sender)->Enabled = false;
}

//---------------------------------------------------------------------------
//���̑��̃G�f�B�^�̒ǉ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddEtcEditorActionExecute(TObject *Sender)
{
	EtcEditorListBox->Items->Add(
		UnicodeString().cat_sprintf(_T("%s=\"%s\""), FExtEtcEdit->Text.LowerCase().c_str(), EtcEditorEdit->Text.c_str()));
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddEtcEditorActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !FExtEtcEdit->Text.IsEmpty() && !EtcEditorEdit->Text.IsEmpty();
}
//---------------------------------------------------------------------------
//���̑��̃G�f�B�^�̕ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgEtcEditorActionExecute(TObject *Sender)
{
	int idx = EtcEditorListBox->ItemIndex;  if (idx==-1) return;
	EtcEditorListBox->Items->Strings[idx] =
		UnicodeString().cat_sprintf(_T("%s=\"%s\""), FExtEtcEdit->Text.LowerCase().c_str(), EtcEditorEdit->Text.c_str());
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgEtcEditorActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !FExtEtcEdit->Text.IsEmpty() && !EtcEditorEdit->Text.IsEmpty();
}

//---------------------------------------------------------------------------
//�G�C���A�X�ύX�𔽉f
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::UpdateAlias(UnicodeString o_name, UnicodeString n_name)
{
	if (SameText(o_name, n_name)) return;

	UnicodeString tmp;
	//�L�[�ݒ�
	for (int i=0; i<MAX_KEYTABS; i++) {
		for (int j=0; j<KeyListBuf[i]->Count; j++) {
			UnicodeString vbuf = KeyListBuf[i]->ValueFromIndex[j];
			if (!remove_top_Dollar(vbuf)) continue;
			if (SameText(o_name, vbuf))
				KeyListBuf[i]->ValueFromIndex[j] = tmp.sprintf(_T("$%s"), n_name.c_str());
		}
	}
	//�֘A�t��
	TListBox *lp = AssociateListBox;
	for (int i=0; i<lp->Count; i++) {
		UnicodeString vbuf = exclude_quot(lp->Items->ValueFromIndex[i]);
		if (!remove_top_Dollar(vbuf)) continue;
		if (SameText(o_name, vbuf))
			lp->Items->ValueFromIndex[i] = tmp.sprintf(_T("\"$%s\""), n_name.c_str());
	}
}
//---------------------------------------------------------------------------
//�G�C���A�X�Ɋ֘A�t�����Ă���g���q���擾
//---------------------------------------------------------------------------
UnicodeString __fastcall TOptionDlg::get_AliasInfo(UnicodeString alias)
{
	//�L�[
	UnicodeString k_inf;
	for (int i=0; i<MAX_KEYTABS && k_inf.IsEmpty(); i++) k_inf = get_Alias_KeyStr(alias, KeyListBuf[i]);

	//�֘A�t��
	UnicodeString a_inf;
	TListBox *lp = AssociateListBox;
	for (int i=0; i<lp->Count; i++) {
		UnicodeString fext = lp->Items->Names[i];
		UnicodeString vbuf = exclude_quot(lp->Items->ValueFromIndex[i]);
		if (!remove_top_Dollar(vbuf)) continue;
		if (!SameText(alias, vbuf))   continue;
		a_inf.cat_sprintf(_T(".%s"), fext.c_str());
	}

	UnicodeString ret_str = k_inf;
	if (!a_inf.IsEmpty()) {
		if (!ret_str.IsEmpty()) ret_str.UCAT_T(" / ");
		ret_str += a_inf;
	}
	return ret_str;
}
//---------------------------------------------------------------------------
//�G�C���A�X�̏d�����b�Z�[�W���擾
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::check_DuplAliasMsg(
	UnicodeString alias,	//�G�C���A�X
	TCheckListBox *lp)		//�ύX�Ώۃ��X�g�{�b�N�X (�ǉ��̏ꍇ�� NULL)
{
	if (alias.IsEmpty()) return false;

	UnicodeString mgs;
	mgs.sprintf(_T("�G�C���A�X \"%s\" ��\r\n"), alias.c_str());

	for (int i=0; i<ExtMenuListBox->Count; i++) {
		if (lp==ExtMenuListBox && i==lp->ItemIndex) continue;
		TStringDynArray itm_buf = get_csv_array(ExtMenuListBox->Items->Strings[i], EXTMENU_CSVITMCNT, true);
		if (SameText(alias, itm_buf[3])) {
			msgbox_WARN(mgs.cat_sprintf(_T("�ǉ����j���[ \"%s\" �ŏd�����Ă��܂��B"), itm_buf[0].c_str()));
			return true;
		}
	}
	for (int i=0; i<ExtToolListBox->Count; i++) {
		if (lp==ExtToolListBox && i==lp->ItemIndex) continue;
		TStringDynArray itm_buf = get_csv_array(ExtToolListBox->Items->Strings[i], EXTTOOL_CSVITMCNT, true);
		if (SameText(alias, itm_buf[4])) {
			msgbox_WARN(mgs.cat_sprintf(_T("�O���c�[�� \"%s\" �ŏd�����Ă��܂��B"), itm_buf[0].c_str()));
			return true;
		}
	}

	return false;
}

//---------------------------------------------------------------------------
//�O���c�[��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtToolListBoxClick(TObject *Sender)
{
	TCheckListBox *lp = ExtToolListBox;
	if (lp->Count==0) return;
	if (!Sender) lp->ItemIndex = -1;

	if (lp->ItemIndex!=-1) {
		TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[lp->ItemIndex], EXTTOOL_CSVITMCNT, true);
		ToolTitEdit->Text	= itm_buf[0];
		ToolExeEdit->Text	= itm_buf[1];
		ToolPrmEdit->Text	= itm_buf[2];
		ToolDirEdit->Text	= itm_buf[3];
		ToolAliasEdit->Text = itm_buf[4];
		ToolAliasLabel->Caption = get_AliasInfo(itm_buf[4]);
	}
	else {
		ToolTitEdit->Text	= EmptyStr;
		ToolExeEdit->Text	= EmptyStr;
		ToolPrmEdit->Text	= EmptyStr;
		ToolDirEdit->Text	= EmptyStr;
		ToolAliasEdit->Text = EmptyStr;
		ToolAliasLabel->Caption = EmptyStr;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ToolTitEditChange(TObject *Sender)
{
	if (starts_tchs(_T("-<>"), ToolTitEdit->Text)) {
		ToolExeEdit->Text = EmptyStr;
		ToolPrmEdit->Text = EmptyStr;
		ToolDirEdit->Text = EmptyStr;
		if (starts_tchs(_T("-<"), ToolTitEdit->Text)) {
			ToolTitEdit->Text	= ToolTitEdit->Text.SubString(1, 1);
			ToolAliasEdit->Text = EmptyStr;
		}
	}
}

//---------------------------------------------------------------------------
//�O���c�[���̎��s�t�@�C����ݒ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SetExtToolExeName(UnicodeString fnam)
{
	if (test_LnkExt(get_extension(fnam))) {
		UnicodeString xnam, prm, fld;
		int shw;
		usr_SH->get_LnkInf(fnam, NULL, &xnam, &prm, &fld, &shw);
		if (test_FileExt(get_extension(xnam), ".exe")) {
			ToolTitEdit->Text = get_base_name(fnam);
			ToolExeEdit->Text = xnam;
			ToolPrmEdit->Text = prm;
			ToolDirEdit->Text = fld;
			ToolAliasEdit->Text = EmptyStr;
		}
		else
			msgbox_WARN("�����N�悪 .exe �t�@�C���ł͂���܂���");
	}
	else
		ToolExeEdit->Text = fnam;
}
//---------------------------------------------------------------------------
//�O���c�[���̒ǉ��E�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeExtToolList(bool add)
{
	TCheckListBox *lp = ExtToolListBox;
	if (check_DuplAliasMsg(ToolAliasEdit->Text, add? NULL : lp)) return;

	int  idx = lp->ItemIndex;
	bool chk = add? true : (idx!=-1)? lp->Checked[idx] : false;

	UnicodeString lbuf;
	lbuf.sprintf(_T("%s,%s,%s,%s,%s,%s"),
		make_csv_str(ToolTitEdit->Text).c_str(),	make_csv_str(ToolExeEdit->Text).c_str(),
		make_csv_str(ToolPrmEdit->Text).c_str(),	make_csv_str(ToolDirEdit->Text).c_str(),
		make_csv_str(ToolAliasEdit->Text).c_str(),	make_csv_str(chk).c_str());

	//�ǉ�
	if (add) {
		lp->Items->Add(lbuf);
		lp->Checked[lp->Count - 1] = true;
	}
	//�ύX
	else {
		if (idx==-1) return;
		UnicodeString last_alias = get_csv_item(lp->Items->Strings[idx], 4);
		lp->Items->Strings[idx] = lbuf;
		//�G�C���A�X�ύX�𔽉f
		UpdateAlias(last_alias, ToolAliasEdit->Text);
	}
}
//---------------------------------------------------------------------------
//�ǉ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddToolActionExecute(TObject *Sender)
{
	ChangeExtToolList(true);
}
//---------------------------------------------------------------------------
//�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgToolActionExecute(TObject *Sender)
{
	ChangeExtToolList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgToolActionUpdate(TObject *Sender)
{
	bool is_special = starts_tchs(_T("-<>"), ToolTitEdit->Text);
	((TAction*)Sender)->Enabled = is_special || (!ToolTitEdit->Text.IsEmpty() && !ToolExeEdit->Text.IsEmpty());
	ToolAliasEdit->Enabled = !starts_tchs(_T("-<"), ToolTitEdit->Text);
	ToolExeEdit->Enabled   = !is_special;
	ToolPrmEdit->Enabled   = !is_special;
	ToolDirEdit->Enabled   = !is_special;
}

//---------------------------------------------------------------------------
//�O���c�[���̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefExeBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("�O���c�[��"), F_FILTER_EXE2);
	UnicodeString fnam;
	if (UserModule->OpenDlgToStr(fnam)) SetExtToolExeName(fnam);
}
//---------------------------------------------------------------------------
//�O���c�[���p�����[�^����������̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefToolFmtBtnClick(TObject *Sender)
{
	TPoint p = ExtToolPanel->ClientToScreen(Point(RefToolFmtBtn->Left + 8, RefToolFmtBtn->Top + 8));
	ToolPopupMenu->Popup(p.x, p.y);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FrmtItemClick(TObject *Sender)
{
	ToolPrmEdit->SelText = get_tkn(((TMenuItem*)Sender)->Caption, ' ');
}
//---------------------------------------------------------------------------
//�O���c�[���̍�ƃf�B���N�g���̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefDirBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("��ƃf�B���N�g��"), dnam)) ToolDirEdit->Text = dnam;
}

//---------------------------------------------------------------------------
//�ǉ����j���[
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtMenuListBoxClick(TObject *Sender)
{
	TCheckListBox *lp = ExtMenuListBox;
	if (lp->Count==0) return;
	if (!Sender) lp->ItemIndex = -1;

	if (lp->ItemIndex!=-1) {
		TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[lp->ItemIndex], EXTMENU_CSVITMCNT, true);
		MenuTitEdit->Text = itm_buf[0];
		if (starts_tchs(_T("-<"), MenuTitEdit->Text)) {
			MenuAliasEdit->Text 	   = EmptyStr;
			MenuAliasLabel->Caption    = EmptyStr;
			MenuCmdComboBox->ItemIndex = -1;
			MenuPrmEdit->Text		   = EmptyStr;
			MenuIconEdit->Text		   = EmptyStr;
		}
		else {
			TComboBox *cp = MenuCmdComboBox;
			for (int i=0; i<cp->Items->Count; i++) {
				if (StartsText(UAPP_T(itm_buf[1], " "), cp->Items->Strings[i])) {
					cp->ItemIndex = i;  break;
				}
			}
			MenuPrmEdit->Text		= itm_buf[2];
			MenuAliasEdit->Text 	= itm_buf[3];
			MenuAliasLabel->Caption = get_AliasInfo(itm_buf[3]);
			MenuIconEdit->Text		= itm_buf[5];
		}
	}
	else {
		MenuTitEdit->Text		   = EmptyStr;
		MenuAliasEdit->Text 	   = EmptyStr;
		MenuAliasLabel->Caption    = EmptyStr;
		MenuCmdComboBox->ItemIndex = -1;
		MenuPrmEdit->Text		   = EmptyStr;
		MenuIconEdit->Text		   = EmptyStr;
	}

	RefMenuPrmBtn->Enabled = USAME_TI(get_tkn(MenuCmdComboBox->Text, ' '), "ExeCommands");
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::MenuCmdComboBoxChange(TObject *Sender)
{
	RefMenuPrmBtn->Enabled = USAME_TI(get_tkn(MenuCmdComboBox->Text, ' '), "ExeCommands");
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::MenuTitEditChange(TObject *Sender)
{
	if (starts_tchs(_T("-<>"), MenuTitEdit->Text)) {
		MenuCmdComboBox->ItemIndex = -1;
		MenuPrmEdit->Text	= EmptyStr;
		if (starts_tchs(_T("-<"), MenuTitEdit->Text)) {
			MenuTitEdit->Text	= MenuTitEdit->Text.SubString(1, 1);
			MenuAliasEdit->Text = EmptyStr;
		}
	}
}

//---------------------------------------------------------------------------
//���j���[���ڂ̒ǉ��E�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeExtMenuList(bool add)
{
	TCheckListBox *lp = ExtMenuListBox;
	if (check_DuplAliasMsg(MenuAliasEdit->Text, add? NULL : lp)) return;

	int  idx = lp->ItemIndex;
	bool chk = add? true : (idx!=-1)? lp->Checked[idx] : false;

	UnicodeString lbuf;
	lbuf.sprintf(_T("%s,%s,%s,%s,%s,%s"),
		make_csv_str(MenuTitEdit->Text).c_str(), make_csv_str(get_tkn(MenuCmdComboBox->Text, ' ')).c_str(),
		make_csv_str(MenuPrmEdit->Text).c_str(), make_csv_str(MenuAliasEdit->Text).c_str(),
		make_csv_str(chk).c_str(), make_csv_str(MenuIconEdit->Text).c_str());

	if (add) {
		lp->Items->Add(lbuf);
		lp->Checked[lp->Count - 1] = true;
	}
	else {
		if (idx==-1) return;
		UnicodeString last_alias = get_csv_item(lp->Items->Strings[idx], 3);
		lp->Items->Strings[idx] = lbuf;
		//�G�C���A�X�ύX�𔽉f
		UpdateAlias(last_alias, MenuAliasEdit->Text);
	}
}
//---------------------------------------------------------------------------
//�ǉ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddMenuActionExecute(TObject *Sender)
{
	ChangeExtMenuList(true);
}
//---------------------------------------------------------------------------
//�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgMenuActionExecute(TObject *Sender)
{
	ChangeExtMenuList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgMenuActionUpdate(TObject *Sender)
{
	bool is_special = starts_tchs(_T("-<>"), MenuTitEdit->Text);
	((TAction*)Sender)->Enabled = is_special ||
		(!MenuTitEdit->Text.IsEmpty() && !MenuCmdComboBox->Text.IsEmpty() && !MenuPrmEdit->Text.IsEmpty());
	MenuAliasEdit->Enabled   = !starts_tchs(_T("-<"), MenuTitEdit->Text);
	MenuCmdComboBox->Enabled = !is_special;
	MenuPrmEdit->Enabled	 = !is_special;
}

//---------------------------------------------------------------------------
//�p�����[�^�t�@�C���̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefCmdsBtnClick(TObject *Sender)
{
	UnicodeString subtit;
	if (PageControl1->ActivePage==ExtMenuSheet) {
		if (!MenuTitEdit->Text.IsEmpty()) subtit.sprintf(_T(" - [%s]"), MenuTitEdit->Text.c_str());
	}
	else if (PageControl1->ActivePage==EventSheet) {
		int idx = EventListBox->ItemIndex;
		if (idx!=-1) subtit.sprintf(_T(" - [%s]"), get_pre_tab(EventListBox->Items->Strings[idx]).c_str());
	}

	UnicodeString cmd;
	if		(PageControl1->ActivePage==ExtMenuSheet) cmd = get_CmdStr(MenuPrmEdit->Text);
	else if (PageControl1->ActivePage==EventSheet)	 cmd = get_CmdStr(EventCmdsEdit->Text);

	//�ʏ�t�@�C���̑I��
	if (contained_wd_i(_T("ContextMenu|ExeMenuFile|FileEdit|LoadWorkList|OpenByApp|OpenByWin|TextViewer"), cmd)) {
		UnicodeString tit = "�p�����[�^�̎w��" + subtit;
		UserModule->PrepareOpenDlg(tit.c_str(), F_FILTER_ALL, NULL, RefParamPath);
		UnicodeString fnam;
		if (UserModule->OpenDlgToStr(fnam)) {
			RefParamPath = ExtractFilePath(fnam);
			if (contained_wd_i(_T("ContextMenu|ExeMenuFile|LoadWorkList"), cmd))
				fnam = to_relative_name(fnam);
			else if (remove_top_text(fnam, ExePath))
				fnam.Insert("%ExePath%", 1);
			cmd.cat_sprintf(_T("_\"%s\""), fnam.c_str());

			if (PageControl1->ActivePage==ExtMenuSheet)
				MenuPrmEdit->Text = cmd;
			else if (PageControl1->ActivePage==EventSheet) {
				EventCmdsEdit->Text = cmd;
				int idx = EventListBox->ItemIndex;
				if (idx!=-1)
					EventListBox->Items->Strings[idx] = get_pre_tab(EventListBox->Items->Strings[idx]) + "\t" + cmd;
			}
		}
	}
	//�R�}���h�t�@�C���̑I��
	else if (CmdFileListDlg->ShowToSelect()==mrOk) {
		UnicodeString fnam = CmdFileListDlg->CmdFileName;
		fnam = "@" + to_relative_name(fnam);
		if (PageControl1->ActivePage==ExtMenuSheet)
			MenuPrmEdit->Text = fnam;
		else if (PageControl1->ActivePage==EventSheet) {
			switch (((TComponent*)Sender)->Tag) {
			case 100:
				EventCmdsEdit->Text = fnam;
				if (EventListBox->ItemIndex!=-1)
					EventListBox->Items->Strings[EventListBox->ItemIndex]
						 = get_pre_tab(EventListBox->Items->Strings[EventListBox->ItemIndex]) + "\t" + fnam;
				break;
			case 101: TimerCmd1Edit->Text = fnam;	break;
			case 102: TimerCmd2Edit->Text = fnam;	break;
			}
		}
	}
}
//---------------------------------------------------------------------------
//�A�C�R���̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefMenuIconBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("�A�C�R���̎w��"), F_FILTER_ICO, NULL, IconFilePath);
	if (UserModule->OpenDlgToEdit(MenuIconEdit, true)) IconFilePath = ExtractFilePath(rel_to_absdir(MenuIconEdit->Text));
}

//---------------------------------------------------------------------------
//�֘A�t��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AssociateListBoxClick(TObject *Sender)
{
	TListBox *lp = AssociateListBox;
	if (!Sender) lp->ItemIndex = -1;

	if (lp->ItemIndex!=-1) {
		AsoExtEdit->Text	 = AssociateListBox->Items->Names[lp->ItemIndex];
		AsoAppComboBox->Text = exclude_quot(AssociateListBox->Items->ValueFromIndex[lp->ItemIndex]);
	}
	else {
		AsoExtEdit->Text	 = EmptyStr;
		AsoAppComboBox->Text = EmptyStr;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AssociateListBoxMouseUp(TObject *Sender, TMouseButton Button,
		TShiftState Shift, int X, int Y)
{
	if (Button!=mbRight) return;
	TListBox *lp = AssociateListBox;
	int idx = lp->ItemIndex;  if (idx==-1) return;

	TStringDynArray app_lst;
	UnicodeString fext = lp->Items->Names[idx];
	for (int i=0; i<lp->Count; i++) {
		if (test_FileExt(fext, lp->Items->Names[i]))
			add_dyn_array(app_lst, exclude_quot(lp->Items->ValueFromIndex[i]));
	}

	if (app_lst.Length>1) {
		std::unique_ptr<TStringList> m_lst(new TStringList());
		make_AssoMenuList(app_lst, m_lst.get());
		//�e�X�g���j���[���쐬
		TestPopupMenu->Items->Clear();
		TestPopupMenu->AutoHotkeys = AutoHotkeyCheckBox->Checked? maAutomatic : maManual;
		IconImgListP->Clear();
		for (int i=0; i<m_lst->Count; i++) {
			TStringDynArray m_buf = split_strings_tab(Trim(m_lst->Strings[i]));
			if (m_buf.Length>0) {
				TMenuItem *mp  = new TMenuItem(TestPopupMenu);
				mp->Caption    = m_buf[0];
				mp->ImageIndex = (m_buf.Length==3)? add_IconImage(m_buf[2], IconImgListP) : -1;
				TestPopupMenu->Items->Add(mp);
			}
		}
		//�e�X�g���j���[��\��
		TestPopupMenu->Popup(Mouse->CursorPos.x, Mouse->CursorPos.y);
	}
}

//---------------------------------------------------------------------------
//�֘A�t���ꗗ�̃\�[�g (�����g���q���͂��̂܂�)
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SortAssoListBox(UnicodeString item)
{
	TListBox *lp = AssociateListBox;
	if (lp->Count<2) return;

	std::unique_ptr<TStringList> x_lst(new TStringList());
	for (int i=0; i<lp->Count; i++) {
		UnicodeString lbuf = lp->Items->Strings[i];
		UnicodeString fext = lp->Items->Names[i];
		TStringList *o_lst;
		int idx = x_lst->IndexOf(fext);
		if (idx==-1) {
			o_lst = new TStringList();
			x_lst->AddObject(fext, (TObject*)o_lst);
		}
		else
			o_lst = (TStringList*)x_lst->Objects[idx];
		o_lst->Add(lbuf);
	}
	x_lst->Sort();

	lp->Clear();
	for (int i=0; i<x_lst->Count; i++) {
		TStringList *o_lst = (TStringList*)x_lst->Objects[i];
		for (int j=0; j<o_lst->Count; j++) lp->Items->Add(o_lst->Strings[j]);
		delete o_lst;
	}

	if (!item.IsEmpty()) {
		int idx = lp->Items->IndexOf(item);
		if (idx!=-1) lp->ItemIndex = idx;
	}
}

//---------------------------------------------------------------------------
//�֘A�t�����ڂ̒ǉ��E�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeAssociateList(bool add)
{
	if (AsoExtEdit->Text.IsEmpty() || AsoAppComboBox->Text.IsEmpty()) return;

	UnicodeString s = AsoExtEdit->Text.LowerCase();
	remove_top_s(s, '.');
	s.cat_sprintf(_T("=\"%s\""), AsoAppComboBox->Text.c_str());

	//�ǉ�
	if (add) {
		AssociateListBox->Items->Add(s);
		if (AsoSortCheckBox->Checked) SortAssoListBox(s);
	}
	//�ύX
	else {
		if (AssociateListBox->ItemIndex==-1) return;
		AssociateListBox->Items->Strings[AssociateListBox->ItemIndex] = s;
		if (AsoSortCheckBox->Checked) SortAssoListBox(s);
	}

	MaxWd_AssExt = 0;
	for (int i=0; i<AssociateListBox->Count; i++)
		MaxWd_AssExt = std::max(AssociateListBox->Canvas->TextWidth(AssociateListBox->Items->Names[i]), MaxWd_AssExt);
	MaxWd_AssExt += ScaledInt(20);
}

//---------------------------------------------------------------------------
//�֘A�t���̎Q�Ɠ��e��ݒ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AsoAppComboBoxDropDown(TObject *Sender)
{
	std::unique_ptr<TStringList> cmd_lst(new TStringList());
	for (int i=0; i<AssociateListBox->Count; i++) {
		UnicodeString lbuf = exclude_quot(AssociateListBox->Items->ValueFromIndex[i]);
		if (!USTARTS_TS('$', lbuf) && !USAME_TS(lbuf, "-")) cmd_lst->Add(lbuf);
	}
	cmd_lst->Sort();

	//�d���폜
	if (cmd_lst->Count>1) {
		UnicodeString laststr = cmd_lst->Strings[0];
		int i = 1;
		while (i<cmd_lst->Count) {
			UnicodeString lbuf = cmd_lst->Strings[i];
			if (SameStr(laststr, lbuf))
				cmd_lst->Delete(i);
			else {
				laststr = lbuf;
				i++;
			}
		}
	}

	//�G�C���A�X
	for (int i=0; i<ExtMenuListBox->Count; i++) {
		UnicodeString itm = get_csv_item(ExtMenuListBox->Items->Strings[i], 3);
	 	if (!itm.IsEmpty()) cmd_lst->Add("$" + itm);
	}
	for (int i=0; i<ExtToolListBox->Count; i++) {
		UnicodeString itm = get_csv_item(ExtToolListBox->Items->Strings[i], 4);
	 	if (!itm.IsEmpty()) cmd_lst->Add("$" + itm);
	}

	AsoAppComboBox->Items->Assign(cmd_lst.get());
}

//---------------------------------------------------------------------------
//�ǉ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddAssoActionExecute(TObject *Sender)
{
	ChangeAssociateList(true);
}
//---------------------------------------------------------------------------
//�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgAssoActionExecute(TObject *Sender)
{
	ChangeAssociateList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgAssoActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !AsoExtEdit->Text.IsEmpty() && !AsoAppComboBox->Text.IsEmpty();
}

//---------------------------------------------------------------------------
//�֘A�t�����s�t�@�C���̑I��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AsoRefBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("�֘A�t��"), F_FILTER_EXE2 _T("|") F_FILTER_NBT);
	UnicodeString fnam;
	if (UserModule->OpenDlgToStr(fnam)) SetExtNameToCtrl(fnam, AsoAppComboBox, true);
}

//---------------------------------------------------------------------------
//�ꗗ�̕`��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OptListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);

	if (lp==EventListBox && USAME_TI(EventCmdList[Index].key, "OnNewDrive") && !State.Contains(odSelected)) {
		cv->Brush->Color = !OpenAddDrvCheckBox->Checked? col_Invalid : scl_Window;
		cv->Font->Color  = scl_WindowText;
	}
	else
		SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);

	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);
	UnicodeString lbuf = lp->Items->Strings[Index];
	bool brk = false;

	//SPI
	if (lp==SpiListBox) {
		int w_x = 0;
		for (int i=0; i<lp->Count; i++)
			w_x = std::max(cv->TextWidth(get_pre_tab(lp->Items->Strings[i])), w_x);
		cv->TextOut(xp, yp, split_pre_tab(lbuf));
		cv->TextOut(xp + w_x + 20, yp, lbuf);
		if (Index>0) {
			UnicodeString vt   = lbuf.SubString(1, 4);
			UnicodeString u_vt = get_post_tab(lp->Items->Strings[Index - 1]).SubString(1, 4);
			brk = !SameText(vt, u_vt);
		}
	}
	//�C�x���g
	else if (lp==EventListBox) {
		UnicodeString tmp = split_pre_tab(lbuf);
		brk = remove_top_s(tmp, '|');
		cv->TextOut(xp, yp, tmp);
		cv->TextOut(xp + MaxWd_Ev, yp, lbuf);
	}
	//�֘A�t��
	else {
		if (Index>0) brk = !SameText(lp->Items->Names[Index], lp->Items->Names[Index-1]);
		cv->TextOut(xp, yp, split_tkn(lbuf, '='));
		cv->TextOut(xp + MaxWd_AssExt, yp, exclude_quot(lbuf));
	}

	//���E��(���)
	if (brk) draw_separateLine(cv, Rect);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OptMenuListBoxDrawItem(TWinControl *Control, int Index,
		TRect &Rect, TOwnerDrawState State)
{
	TCheckListBox *lp = (TCheckListBox*)Control;
	bool is_tool = (lp==ExtToolListBox);
	THeaderSections *sp = ((THeaderControl *)(is_tool? ExtToolHeader : ExtMenuHeader))->Sections;

	TCanvas *cv  = lp->Canvas;
	cv->Font->Assign(lp->Font);
	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);

	//���ڂ̊K�w�𒲂ׂ�
	int sub_lvl = 0;
	for (int i=0; i<=Index; i++) {
		UnicodeString itm = get_csv_item(lp->Items->Strings[i], 0);
		if (i<Index && USTARTS_TS('>', itm)) sub_lvl++;
		else if (USAME_TS(itm, "<") && sub_lvl>0) sub_lvl--;
	}

	int xp = Rect.Left + 2 + ScaledInt(18) + (sub_lvl * cv->TextWidth("W"));
	int yp = Rect.Top + get_TopMargin(cv);

	UnicodeString lbuf = lp->Items->Strings[Index];
	TStringDynArray itm_buf = get_csv_array(lbuf, (is_tool? EXTTOOL_CSVITMCNT : EXTMENU_CSVITMCNT), true);
	//�Z�p���[�^
	if (is_separator(itm_buf[0])) {
		TRect rc = Rect;  rc.Left = xp - 4;
		draw_Separator(cv, rc);
	}
	//����
	else {
		//�A�C�R��
		usr_SH->draw_SmallIcon(rel_to_absdir(cv_env_str(itm_buf[is_tool? 1 : 5])), cv, Rect.Left + 2, yp);
		//�L���v�V����
		UnicodeString lbuf = minimize_str(itm_buf[0], cv, sp->Items[0]->Width - xp, true);
		cv->TextOut(xp, yp, lbuf);
		//�G�C���A�X
		if (!starts_tchs(_T("-<"), lbuf)) {
			cv->TextOut(sp->Items[0]->Width, yp, minimize_str(itm_buf[is_tool? 4 : 3], cv, sp->Items[1]->Width, true));
		}
		//�ݒ�
		if (!starts_tchs(_T("-<>"), lbuf)) {
			lbuf = EmptyStr;
			for (int i=1; i<(is_tool? 4 : 3); i++) {
				if (i>1) lbuf.UCAT_T(", ");
				lbuf += itm_buf[i];
			}
			cv->TextOut(sp->Items[0]->Width + sp->Items[1]->Width, yp, lbuf);
		}
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtMenuHeaderSectionResize(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
	ExtMenuListBox->Repaint();
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExtToolHeaderSectionResize(THeaderControl *HeaderControl,
          THeaderSection *Section)
{
	ExtToolListBox->Repaint();
}

//---------------------------------------------------------------------------
//�L�[�ݒ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyTabControlDrawTab(TCustomTabControl *Control, int TabIndex,
		const TRect &Rect, bool Active)
{
	//���e�[�}���p���ɉ����^�u���������`�悳��Ȃ��s��̑΍�
	draw_BottomTab(Control, TabIndex, Rect, Active);
}
//---------------------------------------------------------------------------
//�J�e�S���[�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyTabControlChanging(TObject *Sender, bool &AllowChange)
{
	KeyListBuf[KeyTabControl->TabIndex]->Assign(KeyListBox->Items);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyTabControlChange(TObject *Sender)
{
	KeyListBox->Items->Assign(KeyListBuf[KeyTabControl->TabIndex]);

	Key2ComboBox->ItemIndex = -1;
	Key2ComboBox->Enabled	= (KeyTabControl->TabIndex==0 || KeyTabControl->TabIndex==2 || KeyTabControl->TabIndex==3);
	SelCheckBox->Enabled	= (KeyTabControl->TabIndex==0);

	KeyFilterEdit->Text = EmptyStr;
	SortKeyListBox();
	SetCmdCombo();
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyHeaderControlDrawSection(THeaderControl *HeaderControl,
		  THeaderSection *Section, const TRect &Rect, bool Pressed)
{
	draw_SortHeader(HeaderControl, Section, Rect, (Section->Index==KeySortMode)? 1 : 0, true);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyHeaderControlSectionClick(THeaderControl *HeaderControl,
		  THeaderSection *Section)
{
	KeySortMode = Section->Index;
	SortKeyListBox();
}

//---------------------------------------------------------------------------
//�R�}���h(CmdComboBox)�̍��ڂ�ݒ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SetCmdCombo(
	UnicodeString kwd)		//�t�B���^������ (default = EmptyStr : �i�荞�ݖ���)
{
	PrmComboBox->Text	 = EmptyStr;
	PrmComboBox->Enabled = false;

	TStrings *sp = CmdComboBox->Items;
	sp->Clear();
	if (KeyTabControl->TabIndex==-1) return;

	std::unique_ptr<TStringList> cmd_lst(new TStringList());
	UnicodeString modstr = GetCmdModeStr();
	for (int i=0; i<CmdSetList->Count; i++) {
		UnicodeString lbuf = CmdSetList->Strings[i];
		if (StartsText(modstr, lbuf)) cmd_lst->Add(get_tkn_r(lbuf, ':'));
	}
	//�ǉ����j���[/�O���c�[���̃G�C���A�X
	if (KeyTabControl->TabIndex==0 || KeyTabControl->TabIndex==2 || KeyTabControl->TabIndex==3) {
		for (int i=0; i<ExtMenuListBox->Count; i++) {
			UnicodeString itm = get_csv_item(ExtMenuListBox->Items->Strings[i], 3);
		 	if (!itm.IsEmpty()) cmd_lst->Add("$" + itm);
		}
		for (int i=0; i<ExtToolListBox->Count; i++) {
			UnicodeString itm = get_csv_item(ExtToolListBox->Items->Strings[i], 4);
		 	if (!itm.IsEmpty()) cmd_lst->Add("$" + itm);
		}
	}

	if (UnRegCmdCheckBox->Checked) {
		TListBox *lp = KeyListBox;
		//�o�^�σR�}���h�𒊏o
		std::unique_ptr<TStringList> reg_lst(new TStringList());
		for (int i=0; i<lp->Count; i++) {
			if (StartsText(modstr, lp->Items->Strings[i])) reg_lst->Add(lp->Items->ValueFromIndex[i]);
		}
		//���o�^���ڂ݂̂��c��
		int i = 0;
		while (i<cmd_lst->Count) {
			bool found = false;
			for (int j=0; j<reg_lst->Count && !found; j++)
				found = SameText(cmd_lst->Strings[i], get_CmdStr(reg_lst->Strings[j]));
			if (found) cmd_lst->Delete(i); else i++;
		}
		//���݈ʒu�̍��ڂ�ǉ�
		int idx = lp->ItemIndex;
		if (idx!=-1) {
			cmd_lst->Add(get_CmdStr(lp->Items->ValueFromIndex[idx]));
			cmd_lst->Sort();
		}
	}

	//�t�B���^
	if (!kwd.IsEmpty()) {
		std::unique_ptr<TStringList> lst(new TStringList());
		lst->Assign(cmd_lst.get());
		UnicodeString ptn = usr_Migemo->GetRegExPtn(MigemoCheckBox->Checked, kwd);
		if (!ptn.IsEmpty()) {
			TRegExOptions opt; opt << roIgnoreCase;
			int i=0;
			while (i<lst->Count) {
				UnicodeString lbuf = get_CmdDesc(lst->Strings[i], false, ExtMenuListBox->Items, ExtToolListBox->Items);
				if (!TRegEx::IsMatch(lbuf, ptn, opt)) lst->Delete(i); else i++;
			}
		}
		cmd_lst->Assign(lst.get());
	}

	//�h���b�v�_�E�����ڂ����蓖��
	for (int i=0; i<cmd_lst->Count; i++)
		sp->Add(get_CmdDesc(cmd_lst->Strings[i], false, ExtMenuListBox->Items, ExtToolListBox->Items));
}

//---------------------------------------------------------------------------
//�R�}���h�R���{�{�b�N�X�̕`��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::CmdComboBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,
		TOwnerDrawState State)
{
	TComboBox *cp = (TComboBox*)Control;
	TCanvas   *cv = cp->Canvas;
	int yp = Rect.Top + get_TopMargin(cv);
	int xp = Rect.Left + 4;

	UnicodeString lbuf = cp->Items->Strings[Index];
	UnicodeString cmd  = split_tkn(lbuf, ' ');

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);
	cv->TextOut(xp, yp, cmd);
	xp += cv->TextHeight("Q")*10;
	cv->TextOut(xp, yp, Trim(lbuf));
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::CmdComboBoxChange(TObject *Sender)
{
	UnicodeString cmd = get_tkn(CmdComboBox->Text," ");

	int tab_idx = KeyTabControl->TabIndex;
	UnicodeString params;

	PrmComboBox->Clear();
	PrmComboBox->Style	  = csDropDownList;
	PrmComboBox->Text	  = EmptyStr;
	PrmComboBox->Enabled  = false;
	RefCmdPrmBtn->Enabled = false;

	if (contained_wd_i(
	 	_T("AddTag|CalcDirSize|CalcDirSizeAll|Calculator|ChangeDir|ChangeOppDir|Clone|CloneToCurr|ContextMenu|ConvertHtm2Txt|")
		_T("Copy|CopyTo|CountLines|CreateDir|DateSelect|DebugCmdFile|ExeCommands|ExeMenuFile|FileEdit|FileRun|Filter|")
		_T("FindFileDlg|FindFileDirDlg|FindTag|FTPChmod|Grep|IncSearch|JumpTo|JumpIndex|PropertyDlg|MaskFind|MaskSelect|")
		_T("MatchSelect|ListArchive|ListDuration|ListExpFunc|ListTail|ListText|LoadBgImage|LoadWorkList|MoveTo|NewTextFile|")
		_T("OpenByApp|OpenByExp|OpenByWin|Pack|PackToCurr|PlayList|RegExChecker|")
		_T("ScrollUpLog|ScrollUpText|ScrollDownLog|ScrollDownText|SetDirTime|")
		_T("SetPathMask|SetSttBarFmt|SetSubSize|SetTag|SubDirList|TagJumpDirect|TagViewDirect|ToTab|TextViewer|ImageViewer|")
		_T("ViewTail|WatchTail|WidenCurList|WinPos|CursorUp|CursorDown|FindDown|FindUp|JumpLine|OpenURL|")
		_T("ScrollCursorDown|ScrollCursorUp|ScrollDown|ScrollUp|")
		_T("SetColor|SetMargin|SetTab|SetWidth|SetUserDefStr|SetFontSize|HelpCurWord|Restart"), cmd))
	{
		PrmComboBox->Enabled = true;
		PrmComboBox->Style = USAME_TI(cmd, "ImageViewer")? csDropDownList : csDropDown;
		//�t�@�C��/�f�B���N�g���Q�ƃ{�^��
		RefCmdPrmBtn->Enabled = contained_wd_i(
			_T("CalcDirSize|CalcDirSizeAll|ChangeDir|ChangeOppDir|ContextMenu|CopyTo|DebugCmdFile|ExeCommands|ExeMenuFile|")
			_T("FileEdit|FileRun|JumpTo|ListArchive|ListDuration|ListExpFunc|ListTail|ListText|LoadBgImage|LoadWorkList|")
			_T("MoveTo|OpenByApp|OpenByExp|OpenByWin|PropertyDlg|")
			_T("PlayList|TextViewer|SetColor|SubDirList|HelpCurWord|Restart"), cmd);
	}

	//�p�����[�^���ڂ�ݒ�
	if (USAME_TI(cmd, "AlphaBlend")) {
		PrmComboBox->Style = csDropDown;
		params.USET_T("\r\nIN : �����x�����\r\n");
	}
	else if (USAME_TI(cmd, "AppList")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("FA : �ꗗ���Ƀt�H�[�J�X\n")
			_T("FL : �����`���[���Ƀt�H�[�J�X\n")
			_T("FI : �����`���[���Ƀt�H�[�J�X(INC.�T�[�`)\n")
			_T("AO : �ꗗ�̂ݕ\��\n")
			_T("LO : �����`���[�̂ݕ\��\n")
			_T("LI : �����`���[�̂ݕ\��(INC.�T�[�`)\n"));
	}
	else if (USAME_TI(cmd, "BgImgMode")) {
		params.sprintf(_T("%s"),
			_T("OFF : ��\��\n")
			_T("1 : 2��ʂɂ킽���ĕ\��\n")
			_T("2 : ���ꂼ��ɕ\��\n")
			_T("3 : �f�X�N�g�b�v�w�i��\��\n")
			_T("^1 : 2��ʂɂ킽���ĕ\��/��\��\n")
			_T("^2 : ���ꂼ��ɕ\��/��\��\n")
			_T("^3 : �f�X�N�g�b�v�w�i��\��/��\��\n"));
	}
	else if (contained_wd_i(_T("CursorUp|CursorDown|ScrollCursorDown|ScrollCursorUp|ScrollDown|ScrollUp"), cmd) && tab_idx!=4) {
		params.USET_T("\nHP : ���y�[�W��\nFP : 1�y�[�W��\n");
	}
	else if (contained_wd_i(_T("CalcDirSize|CalcDirSizeAll"), cmd)) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("LO : ���ʂ����O�ɏo��\n")
			_T("CC : ���ʂ��N���b�v�{�[�h�ɃR�s�[\n")
			_T("LS : ���ʂ��ꗗ�\��\n")
			_T("FC : �t�@�C�����A�T�u�f�B���N�g������\��\n"));
		if (USAME_TI(cmd, "CalcDirSizeAll")) {
			params.UCAT_T("SG : �O���t�\��(�΃J�����g)\nDG : �O���t�\��(�΃h���C�u)\n");
		}
	}
	else if (USAME_TI(cmd, "Calculator")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("CB : �N���b�v�{�[�h����Čv�Z\n"));
	}
	else if (USAME_TI(cmd, "ChangeCodePage")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("932 :   Shift_JIS\n")
			_T("50220 : JIS(ISO-2022-JP)\n")
			_T("20932 : EUC-JP\n")
			_T("1252 :  Latin-1\n")
			_T("65001 : UTF-8\n")
			_T("1200 :  UTF-16\n"));
	}
	else if (contained_wd_i(_T("ChangeDrive|DriveGraph|EjectDrive"), cmd)) {
		for (int i=0; i<26; i++) params.cat_sprintf(_T("%c\n"), 'A'+i);	//�p��
		if (USAME_TI(cmd, "EjectDrive")) params.UCAT_T(". : �J�����g�h���C�u\n");
		if (USAME_TI(cmd, "DriveGraph")) params = "\n" + params;
	}
	else if (contained_wd_i(_T("ChangeRegDir|ChangeOppRegDir"), cmd)) {
		for (int i=0; i<RegDirList->Count; i++) {
			TStringDynArray itm_buf = get_csv_array(RegDirList->Strings[i], 3);
			if (itm_buf.Length!=3 || itm_buf[0].IsEmpty()) continue;
			params.cat_sprintf(_T("%s : %s\n"), itm_buf[0].UpperCase().c_str(), itm_buf[1].c_str());
		}
	}
	else if (USAME_TI(cmd, "CheckUpdate")) {
		params.USET_T("\nNC : �ۑ��ꏊ�̑I���A�m�F�Ȃ�\n");
	}
	else if (USAME_TI(cmd, "ClearAll")) {
		params.USET_T("\nAL : ���E���ׂĂ̑I����Ԃ�����\n");
	}
	else if (USAME_TI(cmd, "ClearMark")) {
		params.USET_T("\nAC : ���ׂĂ̏ꏊ�̂��ׂẴ}�[�N������\n");
	}
	else if (USAME_TI(cmd, "ClipCopy")) {
		if (tab_idx==2) {
			params.USET_T("\nAD : ���݂̓��e�ɒǉ�\n");
		}
		else if (tab_idx==3) {
			params.USET_T("\nVI : �\������Ă����ԂŃR�s�[\n");
		}
	}
	else if (USAME_TI(cmd, "Close") && tab_idx==2) {
		params.USET_T("\nAL : ���ׂĂ̕ʃE�B���h�E�����\n");
	}
	else if (USAME_TI(cmd, "CompareDlg")) {
		params.USET_T("\nNC : �_�C�A���O���o�����A���O�̂ݔ�r\n");
	}
	else if (contained_wd_i(_T("CompareHash|GetHash"), cmd)) {
		params.sprintf(_T("%s"),
			_T("MD5\n")
			_T("SHA1\n")
			_T("SHA256\n")
			_T("SHA384\n")
			_T("SHA512\n")
			_T("CRC32\n"));
	}
	else if (USAME_TI(cmd, "ConvertDoc2Txt")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("SJ : Shift_JIS �ŏo��(�f�t�H���g)\n")
			_T("IJ : ISO-2022-JP �ŏo��\n")
			_T("EJ : EUC-JP �ŏo��\n")
			_T("U8 : UTF-8 �ŏo��\n")
			_T("UL : UTF-16 �ŏo��\n")
			_T("UB : UTF-16BE �ŏo��\n"));
	}
	else if (USAME_TI(cmd, "ConvertHtm2Txt")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("HD : �w�b�_����}��\n")
			_T("MD : Markdown�L�@�ɕϊ�\n")
			_T("TX : �ʏ�e�L�X�g�ɕϊ�\n"));
	}
	else if (USAME_TI(cmd, "CopyDir")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("TO : �R�s�[������\n")
			_T("KT : �^�C���X�^���v���ێ�\n")
			_T("CC : �f�B���N�g�������N���b�v�{�[�h�ɃR�s�[\n")
			_T("LS : �f�B���N�g�������ꗗ�\��\n"));
	}
	else if (contained_wd_i(_T("CopyFileName|ListFileName"), cmd)) {
		PrmComboBox->Style = csDropDown;
		params.USET_T("\nFN : �t�@�C���������̂�\n");
	}
	else if (USAME_TI(cmd, "CreateDir")) {
		params.USET_T("\r\nIN : �f�t�H���g�����w�肵�ē���\r\n");
	}
	else if (contained_wd_i(_T("CsrDirToOpp|LinkToOpp|SwapLR"), cmd)) {
		params.USET_T("\nTO : ���Α��ֈړ�\n");
	}
	else if (contained_wd_i(_T("CurrFromOpp|CurrToOpp"), cmd)) {
		params.USET_T("\nSL : �I����Ԃ𔽉f\n");
		if (USAME_TI(cmd, "CurrToOpp")) params.UCAT_T("TO : ���Α��ֈړ�\n");
	}
	else if (USAME_TI(cmd, "DelJpgExif")) {
		params.USET_T("\nKT : �^�C���X�^���v���ێ�\n");
	}
	else if (USAME_TI(cmd, "DirHistory")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("GA : �S�̗�����\��\n")
			_T("GS : �S�̗�����\��(�\�[�g/�d���폜)\n")
			_T("FM : �S�̗������t�B���^����\n")
			_T("AC : �J�����g���̗��������ׂč폜\n")
			_T("GC : �S�̗��������ׂč폜\n"));
	}
	else if (USAME_TI(cmd, "DistributionDlg")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("XC : �m�F�Ȃ��Œ����ɃR�s�[\n")
			_T("XM : �m�F�Ȃ��Œ����Ɉړ�\n")
			_T("SN : �t�@�C�����X�g����}�X�N�ƐU�����ݒ�\n"));
	}
	else if (USAME_TI(cmd, "Duplicate")) {
		params.USET_T("\nRA : �Ǘ��҂Ƃ��ē�d�N��\n");
	}
	else if (contained_wd_i(_T("EditHistory|ViewHistory"), cmd)) {
		params.USET_T("\nFF : �t�B���^���Ƀt�H�[�J�X\nAC : ���������ׂď���\n");
	}
	else if (USAME_TI(cmd, "Eject")) {
		params.USET_T("\n");
		for (int i=0; i<DriveInfoList->Count; i++) {
			drive_info *dp = (drive_info *)DriveInfoList->Objects[i];
			if (dp->drv_type==DRIVE_CDROM)
				params.cat_sprintf(_T("%s\n"), get_tkn(dp->drive_str, ':').c_str());
		}
	}
	else if (USAME_TI(cmd, "ExeCommandLine")) {
		params.USET_T("\nFN : �J�[�\���ʒu�̃t�@�C���������\n");
	}
	else if (contained_wd_i(_T("ExeExtMenu|ExeExtTool"), cmd)) {
		TStringList *lst= USAME_TI(cmd, "ExeExtMenu")? ExtMenuList : ExtToolList;
		for (int i=0; i<lst->Count; i++) {
			UnicodeString itm = get_csv_item(lst->Strings[i], 0);
			if (!contains_s(itm, _T('&'))) continue;
			UnicodeString ak = get_tkn_r(itm, '&').SubString(1, 1);
			if (ak.IsEmpty() || !_istalnum(ak[1])) continue;
			params.cat_sprintf(_T("%s : %s\n"), ak.UpperCase().c_str(), itm.c_str());
		}
	}
	else if (USAME_TI(cmd, "ExeToolBtn")) {
		TStringList *lst= ToolBtnList;
		for (int i=0; i<lst->Count; i++) {
			TStringDynArray itm_buf = get_csv_array(lst->Strings[i], 2, true);
			if (is_separator(itm_buf[0])) continue;
			params.cat_sprintf(_T("%u : %s\n"), i + 1, (!itm_buf[0].IsEmpty()? itm_buf[0] : itm_buf[1]).c_str());
		}
	}
	else if (USAME_TI(cmd, "ExtractIcon")) {
		PrmComboBox->Style = csDropDown;
		params.USET_T("\nSI : �X���[���A�C�R���𒊏o\n");
	}
	else if (USAME_TI(cmd, "Filter")) {
		params.USET_T("\nCS : �召���������\nCA : ���s�O�ɑI���}�X�N������\n");
	}
	else if (contained_wd_i(_T("FindFileDlg|FindFileDirDlg"), cmd)) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("NM : �}�X�N�����\��\n")
			_T("FK : �����ꗓ�Ƀt�H�[�J�X\n")
			_T("R0 : �u�T�u�f�B���N�g���������v�I�t\n")
			_T("R1 : �u�T�u�f�B���N�g���������v�I��\n"));
		if (USAME_TI(cmd, "FindFileDlg")) {
			params.cat_sprintf(_T("%s"),
				_T("X0 : �u�g�������v�I�t\n")
				_T("X1 : �u�g�������v�I��\n")
				_T("A0 : �u�A�[�J�C�u���������v�I�t\n")
				_T("A1 : �u�A�[�J�C�u���������v�I��\n"));
		}
	}
	else if (USAME_TI(cmd, "FindHardLink")) {
		params.USET_T("\nOP : ���ʃ��X�g���甽�Α��֔��f\n");
	}
	else if (USAME_TI(cmd, "FindMark")) {
		params.USET_T("\nAL : ���ׂẴ}�[�N���ڂ�����\n");
	}
	else if (USAME_TI(cmd, "FindTagName")) {
		params.USET_T("\nEJ : �e�L�X�g�G�f�B�^�Ń_�C���N�g�^�O�W�����v\n");
		if (tab_idx==2) params.UCAT_T("CO : ���݂̃t�@�C���݂̂�����\n");
	}
	else if (contained_wd_i(_T("FindSelDown|FindSelUp"), cmd)) {
		params.USET_T("\nEM : �}�b�`��������\��\n");
	}
	else if (USAME_TI(cmd, "HelpContents")) {
		params.USET_T("\nCI : �R�}���h�̍���\nFI : �R�}���h�̋@�\�ʍ���\nCH : �ύX����\n");
	}
	else if (USAME_TI(cmd, "IncSearch")) {
		params.USET_T("\nMM : Migomo���[�h\nNM : �ʏ탂�[�h\n");
		if (tab_idx==0) params.UCAT_T("FM : �t�B���^�}�X�N�E���[�h\nCA : ���s�O�ɑI���}�X�N������\n");
	}
	else if (USAME_TI(cmd, "InputDir")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("ND : �_�C�A���O��\�����Ȃ��œ���\n")
			_T("SD : �t�H���_�Q�ƃ_�C�A���O��\��\n"));
	}
	else if (USAME_TI(cmd, "DriveList")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("ND : �|�b�v�A�b�v���j���[�őI��\n")
			_T("NS : �|�b�v�A�b�v���j���[�őI��(�󂫗e�ʔ�\��)\n"));
	}
	else if (USAME_TI(cmd, "InputCommands")) {
		params.USET_T("\nEL : �G�R�[�A�R�}���h�t�@�C���̍s�ԍ��\��\n");
	}
	else if (contained_wd_i(_T("CmdFileList|ListClipboard|MarkList|KeyList|FunctionList|UserDefList"), cmd)) {
		params.USET_T("\nFF : �t�B���^���Ƀt�H�[�J�X\n");
	}
	else if (contained_wd_i(_T("Exit|Close"), cmd)) {
		params.USET_T("\nNS : INI�t�@�C����ۑ����Ȃ�\nNX : ����NyanFi���I�������Ȃ�\n");
	}
	else if (USAME_TI(cmd, "ExPopupMenu")) {
		params.USET_T("\nMN : �ǉ����j���[�̂ݕ\��\nTL : �O���c�[���̂ݕ\��\n");
	}
	else if (USAME_TI(cmd, "Library")) {
		params.USET_T("\nSD : �I���_�C�A���O��\��\n* : �I�����j���[��\��\n");
		std::unique_ptr<TStringList> lst(new TStringList());
		get_files(LibraryPath, _T("*.library-ms"), lst.get());
		for (int i=0; i<lst->Count; i++)
			params.cat_sprintf(_T("%s\n"), get_base_name(lst->Strings[i]).c_str());
	}
	else if (contained_wd_i(_T("CountLines|ListArchive|ListDuration|ListExpFunc|ListNyanFi"), cmd)) {
		params.USET_T("\nCC : ���ʂ��N���b�v�{�[�h�ɃR�s�[\nLS : ���ʂ��ꗗ�\��\n");
	}
	else if (USAME_TI(cmd, "ListLog")) {
		params.USET_T("\nEO : �G���[�ӏ��̍i�荞�ݕ\��\n");
	}
	else if (USAME_TI(cmd, "ListTail")) {
		params.USET_T("\nTE : �Ō���Ɉړ�r\nFF : �t�B���^���Ƀt�H�[�J�X\n");
	}
	else if (USAME_TI(cmd, "ListText")) {
		params.USET_T("\nFF : �t�B���^���Ƀt�H�[�J�X\nEO : �G���[�ӏ��̍i�荞�ݕ\��\n");
	}
	else if (USAME_TI(cmd, "Mark") && (tab_idx==0 || tab_idx==3)) {
		params.USET_T("\nND : �J�[�\�����ړ����Ȃ�\nIM : ���������\n");
	}
	else if (USAME_TI(cmd, "MoveTab")) {
		params.USET_T("\nTP : �擪�ɐ擪\nED : �Ō�Ɉړ�\nPR : �P�O�Ɉړ�\n");
	}
	else if (USAME_TI(cmd, "PageBind")) {
		params.USET_T("\nR : �E�Ԃ�\nL : ���Ԃ�\n");
	}
	else if (USAME_TI(cmd, "NextNyanFi")) {
		params.USET_T("\nDN : �Ȃ���Γ�d�N��\n");
	}
	else if (USAME_TI(cmd, "PlayList")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("RP : ���s�[�g�Đ�\n")
			_T("SF : �V���b�t���Đ�\n")
			_T("SR : �V���b�t���E���s�[�g�Đ�\n")
			_T("NX : ���̋Ȃ�\n")
			_T("PR : �O�̋Ȃ�\n")
			_T("PS : �ꎞ��~\n")
			_T("RS : �ĊJ\n")
			_T("FI : �t�@�C������\��\n")
			_T("LS : �v���C���X�g��\��\n"));
	}
	else if (contained_wd_i(_T("RegDirDlg|PathMaskDlg"), cmd)) {
		params.USET_T("\nND : �|�b�v�A�b�v���j���[�őI��\n");
	}
	else if (contained_wd_i(_T("RegDirPopup|PushDir|PopDir"), cmd)) {
		params.USET_T("\nOP : ���Α��Ŏ��s\n");
	}
	else if (USAME_TI(cmd, "ReloadList")) {
		params.USET_T("\nCO : �J�����g�̂ݍX�V\nHL : �n�[�h�����N�̃^�C���X�^���v�X�V\nOFF : �X�V�֎~\n");
	}
	else if (USAME_TI(cmd, "Restart")) {
		params.USET_T("\nNS : INI�t�@�C����ۑ����Ȃ�\nRA : �Ǘ��҂Ƃ��čċN��\nDM : �Ǘ��҂���~�i���čċN��\n");
	}
	else if (USAME_TI(cmd, "SaveAsWorkList")) {
		params.USET_T("\nFL : �J�����g�̓��e�����[�N���X�g�Ƃ��ĕۑ�\n");
	}
	else if (contained_wd_i(_T("ScrollUpLog|ScrollUpText|ScrollDownLog|ScrollDownText"), cmd)) {
		params.USET_T("\nHP : ���y�[�W��\nFP : 1�y�[�W��\n");
		params.cat_sprintf(_T("%s\n"), contains_i(cmd, _T("Down"))? _T("ED : �Ō��") : _T("TP : �擪��"));
	}
	else if (USAME_TI(cmd, "Select") && (tab_idx==0 || tab_idx==3)) {
		PrmComboBox->Style = csDropDown;
		params.USET_T("\n");
		if (tab_idx==0) params.UCAT_T("IN : �J��Ԃ��񐔂����\n");
		params.UCAT_T("ND : �J�[�\�����ړ����Ȃ�\n");
	}
	else if (USAME_TI(cmd, "SelectFile")) {
		params.USET_T("\nNX : ���̃t�@�C����\��\n");
	}
	else if (USAME_TI(cmd, "SelEmptyDir")) {
		params.USET_T("\nNF : �t�@�C�����܂܂�Ă��Ȃ���ΑI��\n");
	}
	else if (contained_wd_i(_T("MarkMask|SelMask"), cmd)) {
		params.USET_T("\nCA : �}�X�N������\n");
	}
	else if (USAME_TI(cmd, "SetColor")) {
		params.USET_T("\nRS : �z�F�����Z�b�g\n");
	}
	else if (USAME_TI(cmd, "SetPathMask")) {
		params.USET_T("\nEX : �J�[�\���ʒu�̊g���q�Ń}�X�N\n");
	}
	else if (USAME_TI(cmd, "SetTopAddr")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("TP : �擪�A�h���X��擪��\n")
			_T("NX : �擪�A�h���X���㑱����\n")
			_T("PR : �擪�A�h���X���s����\n")
			_T("ED : �I�[�A�h���X���Ō��\n"));
	}
	else if (USAME_TI(cmd, "ShowFileInfo")) {
		params.USET_T("\nSD : �_�C�A���O�ŕ\��\n");
	}
	else if (USAME_TI(cmd, "Sort")) {
		params.USET_T("\nAO : ����\nDO : �~��\n");
	}
	else if (USAME_TI(cmd, "SortDlg")) {
		params.sprintf(_T("%s"),
		 	_T("\n")
			_T("F : ���O��\n")
			_T("E : �g���q��\n")
			_T("D : �X�V������\n")
			_T("S : �T�C�Y��\n")
			_T("A : ������\n")
			_T("L : �ꏊ��(���ʃ��X�g)\n")
			_T("FE : ���O/�g���q���g�O���؂�ւ�\n")
			_T("FD : ���O/�X�V�������g�O���؂�ւ�\n")
			_T("FS : ���O/�T�C�Y���g�O���؂�ւ�\n")
			_T("ED : �g���q/�X�V�������g�O���؂�ւ�\n")
			_T("ES : �g���q/�T�C�Y���g�O���؂�ւ�\n")
			_T("DS : �X�V������/�T�C�Y���g�O���؂�ւ�\n")
			_T("IV : ���݂̃\�[�g���@���t����\n")
			_T("IA : ���ׂẴ\�[�g���@���t����\n")
			_T("XN : �f�B���N�g�� - �t�@�C���Ɠ���\n")
			_T("XF : �f�B���N�g�� - ���O\n")
			_T("XD : �f�B���N�g�� - �X�V����\n")
			_T("XS : �f�B���N�g�� - �T�C�Y\n")
			_T("XA : �f�B���N�g�� - ����\n")
			_T("XX : �f�B���N�g������ʂ��Ȃ�\n")
			_T("XNX : �t�@�C���Ɠ���/�f�B���N�g������ʂ��Ȃ��g�O���؂�ւ�\n"));
	}
	else if (USAME_TI(cmd, "SubDirList")) {
		params.USET_T("\nND : �|�b�v�A�b�v���j���[�őI��\n");
	}
	else if (USAME_TI(cmd, "SwapName")) {
		params.USET_T("\nLR : ���E�œ���ւ�\n");
	}
	else if (USAME_TI(cmd, "TabHome")) {
		params.USET_T("\nAL : ���ׂẴ^�u�ɓK�p\nCO : �J�����g�݂̂ɓK�p\n");
	}
	else if (contained_wd_i("TagJump|TagView", cmd)) {
		params.USET_T("\nDJ : ������Ȃ���΃_�C���N�g�^�O�W�����v\n");
	}
	else if (contained_wd_i(_T("TextViewer|ImageViewer"), cmd)) {
		params.USET_T("\nCB : �N���b�v�{�[�h�̓��e��\��\nNN : ����NyanFi�ŕ\��\n");
		if (USAME_TI(cmd, "TextViewer")) params.UCAT_T("XW : �ʃE�B���h�E�ŕ\��\n");
	}
	else if (USAME_TI(cmd, "ToOppSameItem")) {
		params.USET_T("\nNO : ���Α��ֈړ����Ȃ�\n");
	}
	else if (contained_wd_i(_T("ToParent|ToParentOnLeft|ToParentOnRight"), cmd)) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("DL : ���[�g�Ȃ�h���C�u/���L�t�H���_�ꗗ��\��\n")
			_T("DP : ���[�g�Ȃ�h���C�u�I�����j���[��\��\n"));
	}
	else if (USAME_TI(cmd, "ToTab")) {
		params.USET_T("\n");
		for (int i=0; i<TabList->Count; i++) {
			UnicodeString s = get_csv_item(TabList->Strings[i], 2);
			if (!s.IsEmpty()) params.cat_sprintf(_T("%s\n"), s.c_str());
		}
	}
	else if (contained_wd_i(_T("UnPack|UnPackToCurr"), cmd)) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("CD : �f�B���N�g�����쐬���Ă��̒���\n")
			_T("CD2 : ���[�g�ɕ����̑Ώۂ���������f�B���N�g���쐬\n"));
	}
	else if (USAME_TI(cmd, "UpdateFromArc")) {
		params.USET_T("\nUN : �V�����A�[�J�C�u��T���čX�V\n");
	}
	else if (contained_wd_i("ViewIniFile|ViewLog", cmd)) {
		params.UCAT_T("\nXW : �ʃE�B���h�E�ŕ\��\n");
	}
	else if (USAME_TI(cmd, "WatchTail")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("ST : �Ď����e��\��\n")
			_T("CC : �J�[�\���ʒu�t�@�C���̊Ď��𒆎~\n")
			_T("AC : ���ׂĂ̊Ď��𒆎~\n"));
	}
	if (USAME_TI(cmd, "WebMap")) {
		params.USET_T("\nIN : �ܓx,�o�x�����\n");
	}
	else if (USAME_TI(cmd, "WebSearch")) {
		params.USET_T("\nCB : �N���b�v�{�[�h�̓��e������\n");
		if (tab_idx==0) params.UCAT_T("FN : �J�[�\���ʒu�̃t�@�C����������\n");
	}
	else if (USAME_TI(cmd, "WorkList") && tab_idx==0) {
		params.USET_T("\nOP : ���Α��Ŏ��s\nRL : �ύX���e��j�����ēǂݍ��ݒ���\nDI : �����ȍ��ڂ��ꊇ�폜\n");
	}
	else if (contained_wd_i(_T("ZoomIn|ZoomOut"), cmd)) {
		params.USET_T("\n");
		for (int i=2; i<=12; i++) params.cat_sprintf(_T("%u\n"), i);
	}
	else if (contained_wd_i(_T("Copy|Move|Delete|CompleteDelete"), cmd)) {
		params.USET_T("\nSO : �I�����ڂ̂ݏ���\n");
		if (USAME_TI(cmd, "Copy")) {
			params.cat_sprintf(_T("%s"),
				_T("OP : ���Α��R�s�[��̃J�[�\���ʒu��ݒ�\nOP2 : �R�s�[��̃J�[�\���ʒu�𒀎��ݒ�\n")
			 	_T("TO : �R�s�[������\nSD : �R�s�[����Q��\nSS : �J�����g�̃T�u�f�B���N�g����I��\n"));
		}
		if (USAME_TI(cmd, "Move")) {
			params.cat_sprintf(_T("%s"),
				_T("OP : ���Α��ړ���̃J�[�\���ʒu��ݒ�\nOP2 : �ړ���̃J�[�\���ʒu�𒀎��ݒ�\n")
			 	_T("TO : �ړ�������\nSD : �ړ�����Q��\nSS : �J�����g�̃T�u�f�B���N�g����I��\n"));
		}
	}
	//�g�O������R�}���h
	else if (contained_wd_i(
		_T("FileListOnly|HideSizeTime|LockTextPreview|MenuBar|MuteVolume|")
		_T("ShowByteSize|ShowFKeyBar|ShowHideAtr|ShowIcon|ShowPreview|ShowProperty|ShowStatusBar|ShowSystemAtr|ShowTabBar|")
		_T("PauseAllTask|Suspend|SyncLR|UseTrash|")
		_T("BitmapView|CharInfo|CsvRecord|Highlight|HtmlToText|Inspector|ShowCR|ShowLineNo|ShowRuby|ShowRuler|ShowTAB|FixedLen|")
		_T("DoublePage|FullScreen|GifViewer|SubViewer|GrayScale|Histogram|Loupe|ShowGrid|ShowSeekBar|Sidebar|")
		_T("Thumbnail|ThumbnailEx|WarnHighlight"),
		cmd))
	{
		params.USET_T("\nON : �\��/�L��\nOFF : ��\��/����/����\n");
		if (USAME_TI(cmd, "SubViewer"))
			params.UCAT_T("LK : ���b�N/����\nRL : ����90�x��]\nRR : �E��90�x��]\nFH : ���E���]\nFV : �㉺���]\n");
	}
	//�I�v�V�����ݒ�
	else if (USAME_TI(cmd, "OptionDlg")) {
		params.sprintf(_T("%s"),
			_T("\n")
			_T("GN : ���\n")
			_T("MO : �}�E�X����\n")
			_T("DS : �f�U�C��\n")
			_T("TV : �e�L�X�g�r���A�[\n")
			_T("IV : �C���[�W�r���A�[\n")
			_T("ED : �G�f�B�^\n")
			_T("AC : �֘A�t��\n")
			_T("ST : �N����\n")
			_T("XM : �ǉ����j���[\n")
			_T("XT : �O���c�[��\n")
			_T("KY : �L�[�ݒ�\n")
			_T("NT : �ʒm�E�m�F�E�q���g\n")
			_T("CM : �R�}���h\n")
			_T("EV : �C�x���g\n"));
	}

	if (!params.IsEmpty()) PrmComboBox->Items->Text = params;

	if (PrmComboBox->Items->Count>0) {
		PrmComboBox->Enabled   = true;
		PrmComboBox->ItemIndex = 0;
	}
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::UnRegCmdCheckBoxClick(TObject *Sender)
{
	SetCmdCombo();
	KeyListBoxClick(KeyListBox);
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyListBoxClick(TObject *Sender)
{
	inh_KeySet++;

	if (!KeyFilterEdit->Text.IsEmpty() || UnRegCmdCheckBox->Checked) {
		KeyFilterEdit->Text = EmptyStr;
		SetCmdCombo();
	}

	TListBox *lp = (TListBox *)Sender;
	int idx = lp->ItemIndex;  if (idx==-1) return;
	UnicodeString kstr = get_tkn_r(lp->Items->Names[idx], ':');
	UnicodeString fstr = lp->Items->ValueFromIndex[idx];

	ShiftCheckBox->Checked = remove_text(kstr, KeyStr_Shift);
	CtrlCheckBox->Checked  = remove_text(kstr, KeyStr_Ctrl);
	AltCheckBox->Checked   = remove_text(kstr, KeyStr_Alt);
	SelCheckBox->Checked   = remove_text(kstr, KeyStr_SELECT);

	if (contains_s(kstr, _T('~'))) {
		//2�X�g���[�N
		KeyComboBox->ItemIndex	= KeyComboBox->Items->IndexOf(get_tkn(kstr, '~'));
		Key2ComboBox->ItemIndex = Key2ComboBox->Items->IndexOf(get_tkn_r(kstr, '~'));
	}
	else {
		KeyComboBox->ItemIndex	= KeyComboBox->Items->IndexOf(kstr);
		Key2ComboBox->ItemIndex = 0;
	}

	//�R�}���h
	UnicodeString cmd = fstr;
	UnicodeString prm;
	if (!starts_Dollar(cmd)) {
		prm = get_PrmStr(cmd);
		cmd = get_CmdStr(cmd);
	}

	TComboBox *cp = CmdComboBox;
	for (int i=0; i<cp->Items->Count; i++) {
		if (StartsText(UAPP_T(cmd, " "), cp->Items->Strings[i])) {
			cp->ItemIndex = i;
			CmdComboBoxChange(cp);
			break;
		}
	}

	//�p�����[�^
	cp = PrmComboBox;
	if (cp->Enabled) {
		if (cp->Style==csDropDown)
			cp->Text = prm;
		else {
			for (int i=0; i<cp->Items->Count; i++) {
				if (SameText(prm, get_tkn(cp->Items->Strings[i], ' '))) {
					cp->ItemIndex = i; break;
				}
			}
		}
	}

	inh_KeySet--;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyListBoxDblClick(TObject *Sender)
{
	TListBox *lp = (TListBox *)Sender;
	int idx = lp->ItemIndex;  if (idx==-1) return;

	UnicodeString kstr = lp->Items->Names[idx];
	TRegExOptions opt; opt << roIgnoreCase;
	if (TRegEx::IsMatch(kstr, ":(Shift\\+|Ctrl\\+|Alt\\+)*F\\d+", opt)) {
		UnicodeString lbl  = FKeyLabelBuf->Values[kstr];
		UnicodeString tit = "�t�@���N�V�����L�[�\�����̕ύX";
		tit.cat_sprintf(_T(" - %s"), KeyListBuf[KeyTabControl->TabIndex]->Values[kstr].c_str());
		UnicodeString prm;  prm.sprintf(_T("[%s]"), get_tkn_r(kstr, ":").c_str());
		if (input_query_ex(tit.c_str(), prm.c_str(), &lbl)) {
			FKeyLabelBuf->Values[kstr] = lbl;
			lp->Invalidate();
		}
	}
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeySetClick(TObject *Sender)
{
	if (inh_KeySet>0) return;

	UnicodeString kbuf = GetCmdKeyStr().UCAT_T("=");

	TListBox *lp = KeyListBox;
	int idx = -1;
	for (int i=0; i<lp->Count; i++) {
		if (StartsText(kbuf, lp->Items->Strings[i])) { idx = i;  break; }
	}

	if (idx!=-1) {
		lp->ItemIndex = idx;
		KeyListBoxClick(lp);
	}
}

//---------------------------------------------------------------------------
//�t�B���^
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyFilterEdit_Change(TObject *Sender)
{
	UnicodeString kwd = ((TEdit*)Sender)->Text;
	cursor_Default();
	set_RedrawOff(CmdComboBox);
	{
		CmdComboBox->DroppedDown = false;
		SetCmdCombo(kwd);
		if (!kwd.IsEmpty()) {
			CmdComboBox->DroppedDown = true;
			CmdComboBox->ItemIndex	 = 0;
			CmdComboBoxChange(CmdComboBox);
		}
	}
	set_RedrawOn(CmdComboBox);
	Screen->Cursor = crArrow;	//���h���b�v�_�E�����ɃJ�[�\���������錻�ۂւ̑΍�
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyFilterEdit_KeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);

	if (contained_wd_i(KeysStr_CsrDown, KeyStr)) {
		CmdComboBox->SetFocus();
		perform_Key(CmdComboBox, VK_DOWN);
	}
	else if (equal_ENTER(KeyStr)) {
		//�m��
		if (CmdComboBox->DroppedDown) {
			CmdComboBox->SetFocus();
			perform_Key_RETURN(CmdComboBox);
		}
	}
	else if	(equal_ESC(KeyStr) && !KeyFilterEdit->Text.IsEmpty())
		KeyFilterEdit->Text = EmptyStr;
	else if (SameText(KeyStr, KeyStr_Migemo))
		MigemoCheckBox->Checked = !MigemoCheckBox->Checked;
	else return;

	Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyFilterEdit_KeyPress(TObject *Sender, System::WideChar &Key)
{
	if (Key==VK_RETURN || Key==VK_ESCAPE) Key = 0;
}

//---------------------------------------------------------------------------
//�L�[�ݒ�̌���
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::MatchKeyItem(int idx)
{
	bool found = false;
	TListBox *lp = KeyListBox;
	UnicodeString kwd = FindKeyEdit->Text;
	if (!kwd.IsEmpty() && idx>=0 && idx<lp->Count) {
		UnicodeString dsc = get_CmdDesc(lp->Items->ValueFromIndex[idx],
								false, ExtMenuListBox->Items, ExtToolListBox->Items);
		found = ContainsText(dsc, kwd);
	}
	return found;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyEditChange(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	if (!FindKeyEdit->Text.IsEmpty()) {
		int idx = -1;
		for (int i=0; i<lp->Count && idx==-1; i++) if (MatchKeyItem(i)) idx = i;
		lp->ItemIndex = idx;
		if (idx!=-1) KeyListBoxClick(lp);
	}
	else lp->ItemIndex = -1;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyEditKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
	if (!Shift.Empty()) return;

	if		(Key==VK_UP)	{ FindKeyUpAction->Execute();	Key = 0; }
	else if (Key==VK_DOWN)	{ FindKeyDownAction->Execute();	Key = 0; }
}
//---------------------------------------------------------------------------
//�������Ɍ���
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyDownActionExecute(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	if (!FindKeyEdit->Text.IsEmpty() && lp->Count>0) {
		int i0 = (lp->ItemIndex!=-1)? lp->ItemIndex : 0;
		if (MatchKeyItem(i0)) i0++;
		int idx = -1;
		for (int i=i0; i<lp->Count && idx==-1; i++) if (MatchKeyItem(i)) idx = i;
		if (idx!=-1) {
			lp->ItemIndex = idx;
			KeyListBoxClick(lp);
		}
		else beep_Warn();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyDownActionUpdate(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	int idx = -1;
	for (int i=lp->Count-1; i>=0 && idx==-1; i--) if (MatchKeyItem(i)) idx = i;
	((TAction*)Sender)->Enabled = (idx!=-1 && idx>lp->ItemIndex);
}
//---------------------------------------------------------------------------
//������Ɍ���
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyUpActionExecute(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	if (!FindKeyEdit->Text.IsEmpty() && lp->ItemIndex>0) {
		int i0 = lp->ItemIndex;
		if (MatchKeyItem(i0)) i0--;
		int idx = -1;
		for (int i=i0; i>=0 && idx==-1; i--) if (MatchKeyItem(i)) idx = i;
		if (idx!=-1) {
			lp->ItemIndex = idx;
			KeyListBoxClick(lp);
		}
		else beep_Warn();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindKeyUpActionUpdate(TObject *Sender)
{
	TListBox *lp = KeyListBox;
	int idx = -1;
	for (int i=0; i<lp->Count && idx==-1; i++) if (MatchKeyItem(i)) idx = i;
	((TAction*)Sender)->Enabled = (idx!=-1 && idx<lp->ItemIndex);
}

//---------------------------------------------------------------------------
//�L�[�ꗗ�̕`��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::KeyListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,
		TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);
	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);

	UnicodeString key = lp->Items->Names[Index];
	UnicodeString cmd = lp->Items->ValueFromIndex[Index];

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);

	cv->TextOut(xp, yp, get_tkn_r(key, ":"));
	xp += cv->TextWidth("Shift+Ctrl+Alt+Space ");

	UnicodeString dsc = get_CmdDesc(cmd, true, ExtMenuListBox->Items, ExtToolListBox->Items);
	int w_cmd = get_CharWidth(cv, 40);
	cmd = minimize_str(del_CmdDesc(cmd), cv, w_cmd - 8, true);
	cv->TextOut(xp, yp, cmd);
	xp += w_cmd;
	cv->TextOut(xp, yp, dsc);

	//�t�@���N�V�����L�[��
	UnicodeString lbl = FKeyLabelBuf->Values[key];
	if (!lbl.IsEmpty()) {
		xp = Rect.Left + Rect.Width() - get_CharWidth(cv, 16);
		cv->TextOut(xp, yp, lbl);
	}
}

//---------------------------------------------------------------------------
//�L�[�����ꗗ�̃\�[�g
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::SortKeyListBox(UnicodeString item)
{
	TListBox *lp = KeyListBox;
	std::unique_ptr<TStringList> key_lst(new TStringList());
	key_lst->Assign(lp->Items);

	switch (KeySortMode) {
	case  1:	//�R�}���h
		key_lst->CustomSort(KeyComp_Cmd);	break;
	case  2:	//����
		cursor_HourGlass();
		key_lst->CustomSort(KeyComp_Dsc);
		cursor_Default();
		break;
	default:	//�L�[
		key_lst->CustomSort(KeyComp_Key);
	}

	lp->Items->Assign(key_lst.get());
	if (!item.IsEmpty()) {
		int idx = lp->Items->IndexOf(item);
		if (idx!=-1) lp->ItemIndex = idx;
	}

	KeyHeaderControl->Repaint();
}

//---------------------------------------------------------------------------
//�L�[�ݒ�̒ǉ��E�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeKeyList(bool add)
{
	if (KeyComboBox->ItemIndex!=-1 && CmdComboBox->ItemIndex!=-1) {
		try {
			UnicodeString ibuf = GetCmdKeyStr();
			UnicodeString cmd  = get_tkn(CmdComboBox->Text, ' ');
			if (PrmComboBox->Enabled && !PrmComboBox->Text.IsEmpty())
				cmd.cat_sprintf(_T("_%s"), get_tkn(PrmComboBox->Text, _T(" : ")).c_str());

			TListBox *lp = KeyListBox;
			//�ǉ�
			if (add) {
				ibuf.cat_sprintf(_T("=%s"), cmd.c_str());
				lp->Items->Add(ibuf);
			}
			//�ύX
			else {
				int kidx = -1;
				TStrings *sp = lp->Items;
				for (int i=0; i<sp->Count; i++) {
					if (SameText(sp->Names[i], ibuf)) { kidx = i;  break; }
				}
				int c_idx = lp->ItemIndex;
		 		if (kidx!=-1 && kidx!=c_idx) UserAbort(USTR_Registered);
				ibuf.cat_sprintf(_T("=%s"), cmd.c_str());
				lp->Items->Strings[c_idx] = ibuf;
			}
			SortKeyListBox(ibuf);

			SetCmdCombo();
			KeySetClick(NULL);
			KeyListBuf[KeyTabControl->TabIndex]->Assign(KeyListBox->Items);
			KeyListBoxClick(lp);
		}
		catch (EAbort &e) {
			msgbox_WARN(e.Message);
		}
	}
}

//---------------------------------------------------------------------------
//�p�����[�^�̎Q��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefCmdPrmBtnClick(TObject *Sender)
{
	UnicodeString cmd = get_tkn(CmdComboBox->Text, ' ');
	//�f�B���N�g��
	if (contained_wd_i(_T("ChangeDir|ChangeOppDir|CopyTo|MoveTo|OpenByExp|PlayList|SubDirList"), cmd)) {
		UnicodeString dnam = PrmComboBox->Text;
		if (UserModule->SelectDirEx(_T("�p�����[�^�w�� : �f�B���N�g��"), dnam)) PrmComboBox->Text = dnam;
	}
	//�t�@�C��
	else {
		if (contained_wd_i(_T("DebugCmdFile|ExeCommands"), cmd)) {
			if (CmdFileListDlg->ShowToSelect()==mrOk) {
				PrmComboBox->Text = "@" + to_relative_name(CmdFileListDlg->CmdFileName);
			}
		}
		else {
			bool is_nwl = USAME_TI(cmd, "LoadWorkList");
			bool is_mnu = contained_wd_i(_T("ExeMenuFile|ContextMenu"), cmd);
			bool is_chm = USAME_TI(cmd, "HelpCurWord");
			bool is_ini = contained_wd_i(_T("Restart|SetColor"), cmd);
			bool is_run = USAME_TI(cmd, "FileRun");

			UserModule->PrepareOpenDlg(_T("�p�����[�^�w�� : �t�@�C��"), NULL,
				(is_nwl? _T("*.nwl") : is_chm? _T("*.chm") : is_ini? _T("*.ini") : _T("*.*")),
				(is_ini? ExePath : RefParamPath));
			UnicodeString fnam;
			if (UserModule->OpenDlgToStr(fnam)) {
				RefParamPath = ExtractFilePath(fnam);
				if (is_run) {
					TStringDynArray plst = split_strings_semicolon(GetEnvironmentVariable("PATH"));
					for (int i=0; i<plst.Length; i++) {
						if (SameText(IncludeTrailingPathDelimiter(plst[i]), RefParamPath)) {
							fnam = ExtractFileName(fnam); break;
						}
					}
				}
				else if (is_nwl || is_mnu || is_ini)
					fnam = to_relative_name(fnam);
				PrmComboBox->Text = fnam;
			}
		}
	}
}
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::CheckKeyAction(bool ok, bool add)
{
	if (ok && PrmComboBox->Enabled) {
		if (PrmComboBox->Style==csDropDownList)
			ok = (PrmComboBox->ItemIndex!=-1);
		else if (contained_wd_i(_T("ChangeDir|ChangeOppDir|ExeCommands|MaskFind"), get_tkn(CmdComboBox->Text, ' ')))
			ok = !PrmComboBox->Text.IsEmpty();
	}

	//2�X�g���[�N�J�n�L�[�̃`�F�b�N
	if (ok) ok = !IsFirstCmdKey();

	//�����`�F�b�N
	if (add && ok) {
		UnicodeString kbuf = GetCmdKeyStr();
		TStrings *sp = KeyListBox->Items;
		for (int i=0; i<sp->Count; i++)
			if (SameText(sp->Names[i], kbuf)) { ok = false;  break; }
	}

	return ok;
}

//---------------------------------------------------------------------------
//�ǉ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddKeyActionExecute(TObject *Sender)
{
	ChangeKeyList(true);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddKeyActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled =
		CheckKeyAction(KeyComboBox->ItemIndex!=-1 && CmdComboBox->ItemIndex!=-1, true);
}

//---------------------------------------------------------------------------
//�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgKeyActionExecute(TObject *Sender)
{
	ChangeKeyList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgKeyActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled =
		CheckKeyAction(KeyListBox->ItemIndex!=-1 && KeyComboBox->ItemIndex!=-1 && CmdComboBox->ItemIndex!=-1);
}

//---------------------------------------------------------------------------
//�L�[�ݒ�̍폜
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DelKeyActionExecute(TObject *Sender)
{
	delete_ListItem(KeyListBox);
	SetCmdCombo();
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::DelKeyActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = (KeyListBox->ItemIndex!=-1);
}

//---------------------------------------------------------------------------
//����
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::FindEditChange(TObject *Sender)
{
	TPageControl *pp = PageControl1;
	for (int i=0; i<pp->PageCount; i++) pp->Pages[i]->Tag = 0;
	pp->Repaint();
	FindMarkList->ClearAll();
	FindMarkList->MarkColor = col_OptFind;

	UnicodeString wd = FindEdit->Text;
	if (!wd.IsEmpty()) {
		//�^�u�V�[�g���Ɍ���
		for (int i=0; i<pp->PageCount; i++) {
			TTabSheet *sp = pp->Pages[i];
			sp->Tag = (FindMarkList->SearchControl((TWinControl*)sp, wd)>0 || ContainsText(sp->Caption, wd))? 1 : 0;
			//���������� Tag=1 ��
		}
		FindMarkList->ShowMark();
		PageControl1->Repaint();
		//�����������ڂ��܂ރ^�u���A�N�e�B�u��
		bool found = false;
		for (int i=0; i<pp->PageCount && !found; i++) {
			if (pp->Pages[i]->Tag==1) {
				pp->ActivePage = pp->Pages[i];
				found = true;
			}
		}
		if (!found) beep_Warn();
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::PageControl1Change(TObject *Sender)
{
	HelpContext = PageControl1->ActivePage->HelpContext;

	if (!Sender || PageControl1->ActivePage==KeySetSheet) {
		KeyTabControl->TabIndex =
			LogWndListBox->Focused()? 4 :
			   (ScrMode==SCMD_FLIST)? (CurStt->is_IncSea? 1 : 0) :
			   (ScrMode==SCMD_TVIEW)? 2 :
			   (ScrMode==SCMD_IVIEW)? 3 : 0;

		KeyTabControlChange(KeyTabControl);
		KeyComboBox->ItemIndex = -1;
	}

	if (!Sender || PageControl1->ActivePage==AssoSheet) 	AssociateListBoxClick(NULL);
	if (!Sender || PageControl1->ActivePage==ExtMenuSheet)	ExtMenuListBoxClick(NULL);
	if (!Sender || PageControl1->ActivePage==ExtToolSheet)	ExtToolListBoxClick(NULL);

	UserModule->InitializeListBox(GetCurListBox());
}

//---------------------------------------------------------------------------
//�^�u�̕`�� (�������ʂ̋����\��)
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::PageControl1DrawTab(TCustomTabControl *Control, int TabIndex,
		const TRect &Rect, bool Active)
{
	TTabControl *tp = (TTabControl*)Control;
	//�w�i
	TCanvas *cv = tp->Canvas;
	cv->Brush->Color = (PageControl1->Pages[TabIndex]->Tag==0)? Color : col_OptFind;
	cv->FillRect(Rect);
	//�^�C�g��
	cv->Font->Color = scl_WindowText;
	UnicodeString tstr = tp->Tabs->Strings[TabIndex];
	cv->TextOut(Rect.Left + (Rect.Width() - cv->TextWidth(tstr))/2, Rect.Top + 4, tstr);
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefIniPatBtnClick(TObject *Sender)
{
	UnicodeString dnam = ExePath;
	if (UserModule->SelectDirEx(_T("�����p�X"), dnam)) {
		dnam = to_path_name(dnam);
		if (((TComponent*)Sender)->Tag==1) R_IniPatEdit->Text = dnam; else L_IniPatEdit->Text = dnam;
	}
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::L_IniPatMod1RadioBtnClick(TObject *Sender)
{
	if (R_IniPatMod1RadioBtn->Checked) R_IniPatMod0RadioBtn->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::R_IniPatMod1RadioBtnClick(TObject *Sender)
{
	if (L_IniPatMod1RadioBtn->Checked) L_IniPatMod0RadioBtn->Checked = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefHomeWorkBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("�z�[�����[�N���X�g�̎w��"), F_FILTER_NWL, _T("*.nwl"), WorkListPath);
	UserModule->OpenDlgToEdit(HomeWorkListEdit, true);
}

//---------------------------------------------------------------------------
//�z�F�̃C���|�[�g
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::InpColBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("�z�F�̃C���|�[�g"), F_FILTER_INI, _T("*.INI"));
	if (UserModule->OpenDlg->Execute()) {
		std::unique_ptr<TStringList> col_lst(new TStringList());
		std::unique_ptr<TStringList> ext_lst(new TStringList());
		std::unique_ptr<UsrIniFile> inp_file(new UsrIniFile(UserModule->OpenDlg->FileName));
		inp_file->ReadSection("Color", col_lst.get());
		inp_file->LoadListItems("ExtColList",	ext_lst.get());
		if ((col_lst->Count + ext_lst->Count)==0)
			msgbox_WARN("�z�F�ݒ肪����܂���B");
		else {
			if (col_lst->Count>0) ColBufList->Assign(col_lst.get());
			if (ext_lst->Count>0) ExtColListBox->Items->Assign(ext_lst.get());
			//�\�����X�V
			ExtColListBox->Repaint();
			OptColListBox->Repaint();
			TimColListBox->Repaint();
		}
	}
}
//---------------------------------------------------------------------------
//�z�F�̃G�N�X�|�[�g
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExpColBtnClick(TObject *Sender)
{
	UserModule->PrepareSaveDlg(_T("�z�F�̃G�N�X�|�[�g"), F_FILTER_INI, _T("Export_Col.INI"));
	if (UserModule->SaveDlg->Execute()) {
		std::unique_ptr<UsrIniFile> exp_file(new UsrIniFile(UserModule->SaveDlg->FileName));
		exp_file->AssignSection("Color", ColBufList);
		std::unique_ptr<TStringList> ext_lst(new TStringList());
		ext_lst->Assign(ExtColListBox->Items);
		exp_file->SaveListItems("ExtColList", ext_lst.get());
		if (!exp_file->UpdateFile(true)) msgbox_ERR(USTR_FaildSave);
	}
}

//---------------------------------------------------------------------------
//�L�[�ݒ�̃C���|�[�g
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::InpKeyBtnClick(TObject *Sender)
{
	UserModule->PrepareOpenDlg(_T("�L�[�ݒ�̃C���|�[�g"), F_FILTER_INI, _T("*.INI"));
	if (UserModule->OpenDlg->Execute()) {
		std::unique_ptr<UsrIniFile> inp_file(new UsrIniFile(UserModule->OpenDlg->FileName));
		std::unique_ptr<TStringList> key_lst(new TStringList());
		inp_file->ReadSection("KeyFuncList", key_lst.get());
		if (key_lst->Count>0) {
			for (int i=0; i<MAX_KEYTABS; i++) {
				KeyListBuf[i]->Clear();
				UnicodeString modstr = GetCmdModeStr(i + 1);
				for (int j=0; j<key_lst->Count; j++){
					UnicodeString lbuf = key_lst->Strings[j];
					if (StartsText(modstr, lbuf)) KeyListBuf[i]->Add(lbuf);
				}
			}
			inp_file->ReadSection("FKeyLabelList", FKeyLabelBuf);
			KeyTabControlChange(KeyTabControl);
			//���̑��̃L�[�ݒ�
			UnicodeString sstr = inp_file->ReadString(SCT_Option, "DlgMoveShift");
			DlgMoveShiftCheckBox->Checked = ContainsText(sstr, KeyStr_Shift);
			DlgMoveCtrlCheckBox->Checked  = ContainsText(sstr, KeyStr_Ctrl);
			DlgMoveAltCheckBox->Checked	  = ContainsText(sstr, KeyStr_Alt);
			sstr = inp_file->ReadString(SCT_Option, "DlgSizeShift");
			DlgSizeShiftCheckBox->Checked = ContainsText(sstr, KeyStr_Shift);
			DlgSizeCtrlCheckBox->Checked  = ContainsText(sstr, KeyStr_Ctrl);
			DlgSizeAltCheckBox->Checked	  = ContainsText(sstr, KeyStr_Alt);
			sstr = inp_file->ReadString(SCT_Option, "IniSeaShift", "F:Ctrl+");
			IniSeaShiftCheckBox->Checked  = ContainsText(sstr, KeyStr_Shift);
			IniSeaCtrlCheckBox->Checked   = ContainsText(sstr, KeyStr_Ctrl);
			IniSeaAltCheckBox->Checked	  = ContainsText(sstr, KeyStr_Alt);
			IniSeaByNumCheckBox->Checked  = inp_file->ReadBool(SCT_Option, "IniSeaByNum");
			IniSeaBySignCheckBox->Checked = inp_file->ReadBool(SCT_Option, "IniSeaBySign");
		}
		else msgbox_WARN("�L�[�ݒ肪����܂���B");
	}
}
//---------------------------------------------------------------------------
//�L�[�ݒ�̃G�N�X�|�[�g
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ExpKeyBtnClick(TObject *Sender)
{
	UserModule->PrepareSaveDlg(_T("�L�[�ݒ�̃G�N�X�|�[�g"), F_FILTER_INI, _T("Export_Key.INI"));
	if (UserModule->SaveDlg->Execute()) {
		std::unique_ptr<UsrIniFile> exp_file(new UsrIniFile(UserModule->SaveDlg->FileName));
		std::unique_ptr<TStringList> key_lst(new TStringList());
		KeyListBuf[KeyTabControl->TabIndex]->Assign(KeyListBox->Items);
		for (int i=0; i<MAX_KEYTABS; i++) key_lst->AddStrings(KeyListBuf[i]);
		key_lst->CustomSort(KeyComp_Key);
		exp_file->AssignSection("KeyFuncList",	 key_lst.get());
		exp_file->AssignSection("FKeyLabelList", FKeyLabelBuf);
		//���̑��̃L�[�ݒ�
		UnicodeString sstr;
		if (DlgMoveShiftCheckBox->Checked) sstr += KeyStr_Shift;
		if (DlgMoveCtrlCheckBox->Checked)  sstr += KeyStr_Ctrl;
		if (DlgMoveAltCheckBox->Checked)   sstr += KeyStr_Alt;
		exp_file->WriteString(SCT_Option, "DlgMoveShift", sstr);
		sstr = EmptyStr;
		if (DlgSizeShiftCheckBox->Checked) sstr += KeyStr_Shift;
		if (DlgSizeCtrlCheckBox->Checked)  sstr += KeyStr_Ctrl;
		if (DlgSizeAltCheckBox->Checked)   sstr += KeyStr_Alt;
		exp_file->WriteString(SCT_Option, "DlgSizeShift", sstr);
		sstr.USET_T("F:");
		if (IniSeaShiftCheckBox->Checked) sstr += KeyStr_Shift;
		if (IniSeaCtrlCheckBox->Checked)  sstr += KeyStr_Ctrl;
		if (IniSeaAltCheckBox->Checked)   sstr += KeyStr_Alt;
		exp_file->WriteString(SCT_Option, "IniSeaShift", sstr);
		exp_file->WriteBool(SCT_Option, "IniSeaByNum",	IniSeaByNumCheckBox);
		exp_file->WriteBool(SCT_Option, "IniSeaBySign",	IniSeaBySignCheckBox);
		//�����o��
		if (!exp_file->UpdateFile(true)) msgbox_ERR(USTR_FaildSave);
	}
}
//---------------------------------------------------------------------------
//�E��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AlRightBtnClick(TObject *Sender)
{
	IniWinWdEdit->Text = Screen->Width - EditToInt(IniWinLeftEdit);
	WinSizeChanged = IniWin1RadioBtn->Checked;
}
//---------------------------------------------------------------------------
//�ő啝
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::MaxWdBtnClick(TObject *Sender)
{
	IniWinLeftEdit->Text = "0";
	IniWinWdEdit->Text	 = Screen->Width;
	WinSizeChanged = IniWin1RadioBtn->Checked;
}
//---------------------------------------------------------------------------
//�ő卂
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::MaxHiBtnClick(TObject *Sender)
{
	IniWinTopEdit->Text = "0";
	IniWinHiEdit->Text	= Screen->Height;
	WinSizeChanged = IniWin1RadioBtn->Checked;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::IniWinSizeChange(TObject *Sender)
{
	WinSizeChanged = IniWin1RadioBtn->Checked;
}

//---------------------------------------------------------------------------
//OpenStandard �R�}���h�ւ̊֘A�t��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::StdCmdListBoxClick(TObject *Sender)
{
	int idx = StdCmdListBox->ItemIndex;  if (idx==-1) return;
	FExtStdEdit->Text = StdCmdListBox->Items->Names[idx];
	UnicodeString cmd = StdCmdListBox->Items->ValueFromIndex[idx];
	if (contains_s(cmd, _T('_'))) {
		StdCmdPrmEdit->Text = get_PrmStr(cmd);
		cmd = get_CmdStr(cmd).UCAT_T("_");
	}

	for (int i=0; i<StdCmdComboBox->Items->Count; i++) {
		if (ContainsText(StdCmdComboBox->Items->Strings[i], cmd)) {
			StdCmdComboBox->ItemIndex = i;  break;
		}
	}

	StdCmdComboBoxChange(NULL);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::StdCmdComboBoxChange(TObject *Sender)
{
	bool has_prm = contains_s(StdCmdComboBox->Text, _T("ExeCommands_"));
	StdCmdPrmEdit->Enabled	 = has_prm;
	RefStdCmdPrmBtn->Enabled = has_prm;
	if (!has_prm) StdCmdPrmEdit->Text = EmptyStr;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::StdCmdListBoxDrawItem(TWinControl *Control, int Index, TRect &Rect,
		TOwnerDrawState State)
{
	TListBox *lp = (TListBox*)Control;
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(lp->Font);
	int xp = Rect.Left + 2;
	int yp = Rect.Top + get_TopMargin(cv);

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);

	int w_x = 0;
	for (int i=0; i<lp->Count; i++) w_x = std::max(w_x, cv->TextWidth(lp->Items->Names[i]));
	cv->TextOut(xp, yp, lp->Items->Names[Index]);
	cv->TextOut(xp + w_x + 20, yp, lp->Items->ValueFromIndex[Index]);
}

//---------------------------------------------------------------------------
void __fastcall TOptionDlg::RefStdCmdPrmBtnClick(TObject *Sender)
{
	if (contains_i(StdCmdComboBox->Text, _T("ExeCommands_"))) {
		if (CmdFileListDlg->ShowToSelect()==mrOk)
			StdCmdPrmEdit->Text = "@" + to_relative_name(CmdFileListDlg->CmdFileName);
	}
}

//---------------------------------------------------------------------------
//�W���R�}���h�̒ǉ��E�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChangeStdCmdList(bool add)
{
	if (FExtStdEdit->Text.IsEmpty() || StdCmdComboBox->Text.IsEmpty()) return;
	UnicodeString fext = Trim(FExtStdEdit->Text.LowerCase());	if (fext.IsEmpty()) return;
	if (fext[1]!='.') fext.Insert(".", 1);
	UnicodeString cmd = get_in_paren(StdCmdComboBox->Text);
	if (contains_s(cmd, _T('_'))) cmd += StdCmdPrmEdit->Text;

	//�ǉ�
	if (add) {
		StdCmdListBox->Items->Add(fext.cat_sprintf(_T("=%s"), cmd.c_str()));
	}
	//�ύX
	else {
		int idx = StdCmdListBox->ItemIndex;  if (idx==-1) return;
		StdCmdListBox->Items->Strings[idx] = fext.cat_sprintf(_T("=%s"), cmd.c_str());
	}
}
//---------------------------------------------------------------------------
//�ǉ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddStdCmdActionExecute(TObject *Sender)
{
	ChangeStdCmdList(true);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddStdCmdActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled = !FExtStdEdit->Text.IsEmpty() && !StdCmdComboBox->Text.IsEmpty();
}
//---------------------------------------------------------------------------
//�ύX
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgStdCmdActionExecute(TObject *Sender)
{
	ChangeStdCmdList(false);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::ChgStdCmdActionUpdate(TObject *Sender)
{
	((TAction*)Sender)->Enabled
		 = StdCmdListBox->ItemIndex!=-1 && !FExtStdEdit->Text.IsEmpty() && !StdCmdComboBox->Text.IsEmpty();
}

//---------------------------------------------------------------------------
//�C�x���g
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EventListBoxClick(TObject *Sender)
{
	EventCmdsEdit->Enabled = false;
	EventCmdsEdit->Text    = get_post_tab(EventListBox->Items->Strings[EventListBox->ItemIndex]);
	EventCmdsEdit->Enabled = true;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EventCmdsEditChange(TObject *Sender)
{
	if (!EventCmdsEdit->Enabled) return;

	int idx = EventListBox->ItemIndex;
	if (idx!=-1)
		EventListBox->Items->Strings[idx]
			= get_pre_tab(EventListBox->Items->Strings[idx]) + "\t" + EventCmdsEdit->Text;
}

//---------------------------------------------------------------------------
//�R�}���h/���j���[�t�@�C���̕ҏW
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EditFileActionExecute(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;
	UnicodeString fnam = get_cmds_prm_file(
		(PageControl1->ActivePage==EventSheet)? ((tag==0)? EventCmdsEdit->Text : (tag==1)? TimerCmd1Edit->Text : TimerCmd2Edit->Text)
											  : MenuPrmEdit->Text);
	if (!fnam.IsEmpty()) open_by_TextEditor(fnam);
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::EditFileActionUpdate(TObject *Sender)
{
	int tag = ((TComponent*)Sender)->Tag;
	UnicodeString fnam = get_cmds_prm_file(
		(PageControl1->ActivePage==EventSheet)? ((tag==0)? EventCmdsEdit->Text : (tag==1)? TimerCmd1Edit->Text : TimerCmd2Edit->Text)
											  : MenuPrmEdit->Text);
	((TAction*)Sender)->Enabled = !fnam.IsEmpty();
}

//---------------------------------------------------------------------------
//�폜�����f�B���N�g���̒ǉ�
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::AddPrtDirActionExecute(TObject *Sender)
{
	UnicodeString dnam;
	if (UserModule->SelectDirEx(_T("��ƃf�B���N�g��"), dnam)) {
		if (PrtDirListBox->Items->IndexOf(dnam)==-1) {
			PrtDirListBox->Items->Add(dnam);
			PrtDirListBox->ItemIndex = PrtDirListBox->Items->IndexOf(dnam);
		}
	}
}

//---------------------------------------------------------------------------
//�m��
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OkActionExecute(TObject *Sender)
{
	ModalResult = mrNone;

	//NyanFi �z�b�g�L�[�ēo�^
	if (IsPrimary) {
		UnicodeString kstr;
		if (!HotKeyComboBox->Text.IsEmpty()) {
			if (HotShiftCheckBox->Checked) kstr += KeyStr_Shift;
			if (HotCtrlCheckBox->Checked)  kstr += KeyStr_Ctrl;
			if (HotAltCheckBox->Checked)   kstr += KeyStr_Alt;
			if (HotWinCheckBox->Checked)   kstr.UCAT_T("Win+");
			kstr += HotKeyComboBox->Text;
		}
		if (register_HotKey(ID_CALL_HOTKEY, kstr))
			CallHotKey = kstr;
		else {
			FindEdit->Text = EmptyStr;
			FindEdit->Text = "�Ăяo���z�b�g�L�[";
			msgbox_ERR(GlobalErrMsg);
			return;
		}
		//AppList �z�b�g�L�[�ēo�^
		kstr = EmptyStr;
		if (!AppKeyComboBox->Text.IsEmpty()) {
			if (AppShiftCheckBox->Checked) kstr += KeyStr_Shift;
			if (AppCtrlCheckBox->Checked)  kstr += KeyStr_Ctrl;
			if (AppAltCheckBox->Checked)   kstr += KeyStr_Alt;
			if (AppWinCheckBox->Checked)   kstr.UCAT_T("Win+");
			kstr += AppKeyComboBox->Text;
		}
		if (register_HotKey(ID_APP_HOTKEY, kstr))
			AppListHotKey = kstr;
		else {
			FindEdit->Text = EmptyStr;
			FindEdit->Text = "AppList";
			msgbox_ERR(GlobalErrMsg);
			return;
		}
	}

	//�R���g���[���̃^�O�ɂ��������ĕύX��K�p
	ApplyOptionByTag(this);

	//�^�O��p���Ȃ��ݒ�
	TempPath		 = to_path_name(TempDirEdit->Text);
	TempPathA		 = rel_to_absdir(TempPath);
	TempPathFTP.sprintf(_T("%s" TMP_FTP_D), TempPathA.c_str());
	MaxTasks		 = MaxTasksComboBox->ItemIndex + 1;
	AppListHotPrm	 = get_word_i_idx(_T("|FA|FL|FI|AO|LO|LI"), AppPrmComboBox->ItemIndex);
	ViewTxtLimitSize = EditToInt(LimitTxtEdit) * 1024;
	ViewBinLimitSize = std::max(StrToInt64Def(LimitBinEdit->Text, 0) * 1048576, 1048576LL);
	GetInfLimitSize  = std::max(EditToInt(LimitRarEdit) * 1048576, 1048576);

	UnicodeString tmp;
	//Migemo �ď�����
	if (!SameText(MigemoPath, MigemoDirEdit->Text)) {
		MigemoPath = to_path_name(MigemoDirEdit->Text);
		delete usr_Migemo;
		usr_Migemo = new MigemoUnit(rel_to_absdir(MigemoPath));
		AddLog(tmp.sprintf(_T("  LOAD migemo.dll   %s"), usr_Migemo->DictReady? _T("OK") : _T("NONE")));
	}
	usr_Migemo->MinLength = IncSeaMigemoMin;

	//Susie �v���O�C���̍ď�����
	if (!SameText(SpiDir, SusieDirEdit->Text)) {
		SpiDir = to_path_name(SusieDirEdit->Text);
		delete SPI;
		SPI = new SpiUnit(rel_to_absdir(SpiDir));
		if (SPI->PlgList->Count>0) {
			for (int i=0; i<SPI->PlgList->Count; i++) {
				spi_info *sp = SPI->PlgList->Items[i];
				AddLog(tmp.sprintf(_T("  LOAD %-12s %s"), ExtractFileName(sp->FileName).c_str(), sp->FileType.c_str()));
			}
		}
	}

	//7z.dll�Ή��g���q
	usr_ARC->FExt7zDll = FExt7zDll;

	EtcEditorList->Assign(EtcEditorListBox->Items);

	ColorList->Assign(ColBufList);
	set_col_from_ColorList();

	ExtColList->Assign(ExtColListBox->Items);
	AssociateList->Assign(AssociateListBox->Items);
	OpenStdCmdList->Assign(StdCmdListBox->Items);
	ProtectDirList->Assign(PrtDirListBox->Items);

	TCheckListBox *lp = ExtMenuListBox;
	for (int i=0; i<lp->Count; i++) {
		TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[i], EXTMENU_CSVITMCNT, true);
		itm_buf[4].sprintf(_T("%s"), lp->Checked[i]? _T("1") : _T("0"));
		lp->Items->Strings[i] = make_csv_rec_str(itm_buf);
	}
	ExtMenuList->Assign(lp->Items);

	lp = ExtToolListBox;
	for (int i=0; i<lp->Count; i++) {
		TStringDynArray itm_buf = get_csv_array(lp->Items->Strings[i], EXTTOOL_CSVITMCNT, true);
		itm_buf[5].sprintf(_T("%s"), lp->Checked[i]? _T("1") : _T("0"));
		lp->Items->Strings[i] = make_csv_rec_str(itm_buf);
	}
	ExtToolList->Assign(lp->Items);

	ViewFoldFitWin = FitFoldRadioBtn->Checked;

	//�L�[�ݒ�
	KeyListBuf[KeyTabControl->TabIndex]->Assign(KeyListBox->Items);
	std::unique_ptr<TStringList> key_lst(new TStringList());
	for (int i=0; i<MAX_KEYTABS; i++) key_lst->AddStrings(KeyListBuf[i]);
	key_lst->CustomSort(KeyComp_Key);
	KeyFuncList->Assign(key_lst.get());
	//�t�@���N�V�����L�[�̕\����
	FKeyLabelList->Assign(FKeyLabelBuf);
	int i = 0;
	while (i<FKeyLabelList->Count) {
		if (KeyFuncList->IndexOfName(FKeyLabelList->Names[i])==-1) FKeyLabelList->Delete(i); else i++;
	}

	IniPathMode[0] = L_IniPatMod1RadioBtn->Checked? 1 : L_IniPatMod2RadioBtn->Checked? 2 : 0;
	IniPathMode[1] = R_IniPatMod1RadioBtn->Checked? 1 : R_IniPatMod2RadioBtn->Checked? 2 : 0;
	IniWorkMode    = IniWork1RadioBtn->Checked? 1 : 0;
	IniWinMode	   = IniWin1RadioBtn->Checked? 1 : 0;

	DlgMoveShift = EmptyStr;
	if (DlgMoveShiftCheckBox->Checked) DlgMoveShift += KeyStr_Shift;
	if (DlgMoveCtrlCheckBox->Checked)  DlgMoveShift += KeyStr_Ctrl;
	if (DlgMoveAltCheckBox->Checked)   DlgMoveShift += KeyStr_Alt;
	DlgSizeShift = EmptyStr;
	if (DlgSizeShiftCheckBox->Checked) DlgSizeShift += KeyStr_Shift;
	if (DlgSizeCtrlCheckBox->Checked)  DlgSizeShift += KeyStr_Ctrl;
	if (DlgSizeAltCheckBox->Checked)   DlgSizeShift += KeyStr_Alt;

	//�������T�[�`
	IniSeaShift.USET_T("F:");
	if (IniSeaShiftCheckBox->Checked) IniSeaShift += KeyStr_Shift;
	if (IniSeaCtrlCheckBox->Checked)  IniSeaShift += KeyStr_Ctrl;
	if (IniSeaAltCheckBox->Checked)   IniSeaShift += KeyStr_Alt;

	//�C�x���g
	for (int i=0; i<MAX_EVENT_CMD; i++)
		 *(EventCmdList[i].sp) = get_post_tab(EventListBox->Items->Strings[i]);

	SetupTimer();

	ModalResult = mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::OkActionUpdate(TObject *Sender)
{
	HotKeyComboBox->Enabled =
		(HotShiftCheckBox->Checked || HotCtrlCheckBox->Checked || HotAltCheckBox->Checked || HotWinCheckBox->Checked);

	DirBraEdit->Color = ShowIconCheckBox->Checked? col_Invalid : scl_Window;
	DirKetEdit->Color = ShowIconCheckBox->Checked? col_Invalid : scl_Window;

	bool flag = (DlgMoveShiftCheckBox->Checked || DlgMoveCtrlCheckBox->Checked || DlgMoveAltCheckBox->Checked);
	DlgMoveLabel->Font->Color = flag? scl_WindowText : col_Invalid;
	DlgMoveEdit->Color		  = flag? scl_Window	 : col_Invalid;

	flag = (DlgSizeShiftCheckBox->Checked ||DlgSizeCtrlCheckBox->Checked || DlgSizeAltCheckBox->Checked);
	DlgSizeLabel->Font->Color = flag? scl_WindowText : col_Invalid;
	DlgSizeEdit->Color		  = flag? scl_Window	 : col_Invalid;

	InvColIfEmpty(NoWatchPathEdit);
	InvColIfEmpty(NoInfPathEdit);
	InvColIfEmpty(FExtGetInfEdit);
	InvColIfEmpty(FExtNoInfEdit);
	InvColIfEmpty(NoImgPrvPathEdit);
	InvColIfEmpty(FExtImgPrvEdit);
	InvColIfEmpty(FExtNoImgPrvEdit);
	InvColIfEmpty(ExtTxViewerEdit);
	ExtTxViewerFmtEdit->Color = ExtTxViewerEdit->Color;

	HdrStrEdit->Color = MarkdownCheckBox->Checked? col_Invalid : scl_Window;

	SelBaseOnlyCheckBox->Enabled  = SelByMouseCheckBox->Checked;
	SelIconCheckBox->Enabled	  = SelByMouseCheckBox->Checked;

	PrvTxtInfLnEdit->Color		  = PrevTxtToInfCheckBox->Checked? scl_Window : col_Invalid;
	PrevTailCheckBox->Enabled	  = PrevTxtToInfCheckBox->Checked;
	flag = (PrevTxtToInfCheckBox->Checked && PrevTailCheckBox->Checked);
	PrvTxtTailLnEdit->Color		  = flag? scl_Window : col_Invalid;
	PrvActTailLnEdit->Color		  = flag? scl_Window : col_Invalid;

	AppendLogCheckBox->Enabled	  = SaveLogCheckBox->Checked;
	MaxLogFilesEdit->Enabled	  = SaveLogCheckBox->Checked;
	BeepTopEndCheckBox->Enabled   = HintTopEndCheckBox->Checked;
	SeekSwapNPCheckBox->Enabled   = SeekBindDirCheckBox->Checked;

	flag = (EditToInt(FwCntWarnEdit)==0 || EditToInt(FwTmWarnEdit)==0);
	FwCntWarnEdit->Color = flag? col_Invalid : scl_Window;
	FwTmWarnEdit->Color  = flag? col_Invalid : scl_Window;

	flag = (EditToInt(FwCntTaskFinEdit)==0 || EditToInt(FwTmTaskFinEdit)==0);
	FwCntTaskFinEdit->Color = flag? col_Invalid : scl_Window;
	FwTmTaskFinEdit->Color	= flag? col_Invalid : scl_Window;

	EventCmdsEdit->Enabled = (EventListBox->ItemIndex!=-1);
	RefCmdsBtn->Enabled	   = (EventListBox->ItemIndex!=-1);

	InvColIfEmpty(NopDtctTimeEdit);
	InvColIfEmpty(WatchIntervalEdit);
	InvColIfEmpty(FixedLimitEdit);

	ListPercentEdit->Color	 = !KeepOnResizeCheckBox->Checked? col_Invalid : scl_Window;
	FindPathWdEdit->Color	 = !FindPathColCheckBox->Checked? col_Invalid : scl_Window;

	ModalAlphaEdit->Color	 = !ModalScrCheckBox->Checked? col_Invalid : scl_Window;
	ModalColorBox->Color 	 = !ModalScrCheckBox->Checked? col_Invalid : scl_Window;

	RefFontBtn->Enabled		 = (FontComboBox->ItemIndex!=-1);
	RefColBtn->Enabled		 = (OptColListBox->ItemIndex!=-1);
	RefTimColBtn->Enabled	 = (TimColListBox->ItemIndex!=-1);

	if (!UserModule->SpuitEnabled()) {
		SpuitImage->Visible  = (OptColListBox->ItemIndex!=-1);
		SpuitImage2->Visible = (TimColListBox->ItemIndex!=-1);
		SpuitImage3->Visible = !ExtColorEdit->Text.IsEmpty();
	}

	DivDirInfUDCheckBox->Enabled = DivFlistUDCheckBox->Checked;

	int bgmod = BgImgModeComboBox->ItemIndex;
	BgImgSubModeComboBox->Enabled = (bgmod==1 || bgmod==2);
	BgImg1Edit->Color	= (bgmod==0 || bgmod==3)?		col_Invalid : scl_Window;
	BgImg2Edit->Color	= (bgmod!=2)?					col_Invalid : scl_Window;
	HideTimeEdit->Color = !HideScrCheckBox->Checked?	col_Invalid : scl_Window;
	TileIfSzEdit->Color = !TileIfCheckBox->Checked?		col_Invalid : scl_Window;
	InvColIfEmpty(SpImgEdit);
	InvColIfEmpty(SpImg2Edit);

	CloseOtherCheckBox->Enabled = !TaskTrayCheckBox->Checked;
	if (TaskTrayCheckBox->Checked) CloseOtherCheckBox->Checked = true;

	MarkPathEdit->Color = !MarkImgCheckBox->Checked? col_Invalid : scl_Window;
	MarkFextEdit->Color = !MarkImgCheckBox->Checked? col_Invalid : scl_Window;
	MarkMemoEdit->Color = !MarkImgCheckBox->Checked? col_Invalid : scl_Window;
}

//---------------------------------------------------------------------------
//�L�����Z������
//---------------------------------------------------------------------------
void __fastcall TOptionDlg::CanButtonClick(TObject *Sender)
{
	//�X�|�C�g�̃L�����Z��
	if (UserModule->SpuitEnabled()) UserModule->EndSpuit();
	//����
	else ModalResult = mrCancel;
}

//---------------------------------------------------------------------------
//�w���v
//---------------------------------------------------------------------------
bool __fastcall TOptionDlg::FormHelp(WORD Command, NativeInt Data, bool &CallHelp)
{
	if (Command==HELP_CONTEXT || Command==HELP_CONTEXTPOPUP) {
		if (PageControl1->ActivePage==KeySetSheet && (CmdComboBox->Focused() || PrmComboBox->Focused())) {
			UnicodeString topic;

			switch (KeyTabControl->TabIndex) {
			case 0: topic.UCAT_T(HELPTOPIC_FL);		break;	//�t�@�C���[
			case 1: topic.UCAT_T(HELPTOPIC_IS);		break;	//INC.�T�[�`
			case 2: topic.UCAT_T(HELPTOPIC_TV);		break;	//�e�L�X�g�r���A�[
			case 3: topic.UCAT_T(HELPTOPIC_IV);		break;	//�C���[�W�r���A�[
			case 4: topic.UCAT_T(HELPTOPIC_CILW);	break;	//���O
			}

			UnicodeString kwd = get_tkn(CmdComboBox->Text, ' ');
			if (!contains_s(topic, _T('#')) && !kwd.IsEmpty() && !starts_Dollar(kwd))
				topic.cat_sprintf(_T("#%s"), kwd.c_str());

			HtmlHelpTopic(topic.c_str());
		}
		else {
			HtmlHelpContext(Data);
		}
		CallHelp = false;
	}
	return true;
}
//---------------------------------------------------------------------------
