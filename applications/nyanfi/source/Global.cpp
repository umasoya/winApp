//----------------------------------------------------------------------//
// NyanFi																//
//  �O���[�o��															//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <algorithm>
#include <memory>
#include <utilcls.h>
#include <shlwapi.h>
#include <winioctl.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <VersionHelpers.h>
#include <System.DateUtils.hpp>
#include <System.Character.hpp>
#include <System.IOUtils.hpp>
#include <System.Zip.hpp>
#include <RegularExpressions.hpp>
#include <Vcl.Imaging.pngimage.hpp>
#include <Vcl.Imaging.GIFImg.hpp>
#include <Vcl.Clipbrd.hpp>
#include "usr_wic.h"
#include "usr_arc.h"
#include "usr_mmfile.h"
#include "usr_exif.h"
#include "usr_xd2tx.h"
#include "usr_highlight.h"
#include "check_thread.h"
#include "UserFunc.h"
#include "UserMdl.h"
#include "InpExDlg.h"
#include "InpDir.h"
#include "Global.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

//---------------------------------------------------------------------------
UnicodeString RstBatName;	//�ċN���p�o�b�`�t�@�C����

//---------------------------------------------------------------------------
//HTML�w���v
//�IVcl.HTMLHelpViewer �͗p�����AHtmlHelp API(���I�����N)�ŏ���
HINSTANCE	  hHHctrl	  = NULL;	//hhctrl.ocx
FUNC_HtmlHelp lpfHtmlHelp = NULL;
DWORD		  Cookie	  = NULL;
bool 		  CancelHelp  = false;

//---------------------------------------------------------------------------
//����JAPI
HINSTANCE hGdi32 = NULL;
FUNC_GetFontResourceInfo lpGetFontResourceInfo = NULL;

//---------------------------------------------------------------------------
UnicodeString KeyStr_SELECT   = "SELECT+";
UnicodeString KeyStr_Copy	  = "Ctrl+C";
UnicodeString KeyStr_Cut	  = "Ctrl+X";
UnicodeString KeyStr_Migemo   = "Ctrl+M";
UnicodeString KeyStr_Filter   = "Ctrl+F";
UnicodeString KeysStr_ToList  = "Ctrl+L";
UnicodeString KeysStr_CsrDown = "DOWN|Ctrl+X";
UnicodeString KeysStr_CsrUp   = "UP|Ctrl+E";
UnicodeString KeysStr_PgDown  = "PGDN|Ctrl+C";
UnicodeString KeysStr_PgUp	  = "PGUP|Ctrl+R";
UnicodeString KeysStr_Popup   = "APP|Shift+F10";

//---------------------------------------------------------------------------
UnicodeString SortIdStr    = "FEDSA";
UnicodeString ScrModeIdStr = "FSVIL";

//---------------------------------------------------------------------------
HWND  MainHandle = NULL;		//���C���E�B���h�E�̃n���h��
DWORD ProcessId  = 0;			//�v���Z�XID
bool  IsAdmin	 = false;		//�Ǘ��Ҍ���
int   StartedCount;				//���s�J�n�J�E���g(m�b)
int   NyanFiIdNo = 0;			//���d NyanFi ����ID
bool  IsPrimary  = true;		//�ŏ��ɋN�����ꂽ
win_dat Win2Data;				//��d�N���I�����̉�ʏ��

int    ScrMode  = SCMD_FLIST;	//��ʃ��[�h
double ScrScale = 1.0;			//��ʃX�P�[�����O
int    SIcoSize = 16;			//�X���[���A�C�R���̕\���T�C�Y

bool  IsMuted	= false;		//���ʃ~���[�g

TRichEdit *TempRichEdit = NULL;

TTaskThread *TaskThread[MAX_TASK_THREAD];	//�^�X�N�����X���b�h
int  MaxTasks;								//�ő�^�X�N��
bool RsvSuspended = false;					//�\��ۗ̕�
TaskConfigList *TaskReserveList;			//�^�X�N�\�񃊃X�g
int  LastOpCount;							//�Ō�ɑ��삵�����̃J�E���g�l

TImgViewThread   *ImgViewThread   = NULL;	//�摜�r���[�����X���b�h
TThumbnailThread *ThumbnailThread = NULL;	//�T���l�C���擾�X���b�h
TGetIconThread   *GetIconThread   = NULL;	//�A�C�R���擾�X���b�h

UserArcUnit *usr_ARC;			//�A�[�J�C�u�������j�b�g
UnicodeString FExt7zDll;		//7z.dll �őΉ�����g���q

MigemoUnit  *usr_Migemo;		//Migemo�������j�b�g
UnicodeString MigemoPath;
bool LastMigemoMode;
bool LastMigemoModeF;

SpiUnit *SPI = NULL;			//Susie�v���O�C��
UnicodeString SpiDir;			//Susie�v���O�C���̃f�B���N�g��
bool UseSpiFirst;				//Susie�v���O�C����D��I�Ɏg��

int  WicScaleOpt;				//WIC �̏k���E�g��A���S���Y��
UnicodeString WicFextStr;		//WIC ���Ή����Ă���g���q

bool gCopyAll;
bool gCopyCancel;

int  gCopyMode	= CPYMD_OW;		//���E���ʃf�B���N�g���̏ꍇ�̃R�s�[���[�h
int  gCopyMode2 = CPYMD_OW;		//���E������f�B���N�g���̏ꍇ�̃R�s�[���[�h
int  xCopyMode = -1;			//ExeCommand ���ŗp���鋭�����[�h

UnicodeString gCopyFmt = "\\N_\\SN(1)";

int  OptionPageIndex = 0;		//�I�v�V�����ݒ�_�C�A���O�̃y�[�W�C���f�b�N�X

bool SyncLR;					//���E�̃f�B���N�g���ύX�𓯊�

UnicodeString GlobalErrMsg;

bool Initialized	= false;	//����������������
bool UnInitializing = false;	//�I���������J�n���ꂽ
bool Closing		= false;	//�I��������

int  InhReload = 0;				//Reload �̗}�~
bool DisReload = false;			//Reload �̖�����

UnicodeString FindPath;
bool FindAborted;				//�������f�v��
int  FindTag;
int  FindCount;					//�}�b�`��

bool CalcAborted;				//�v�Z���f�v��
int  CalcTag;

bool MultiInstance;				//��d�N��������
bool CloseOthers;				//����NyanFi���I��
bool StoreTaskTray;				//�ŏ������Ƀ^�X�N�g���C�Ɋi�[
bool ShowIcon;					//�A�C�R����\��
bool UseIndIcon;				//�t�@�C���ŗL�̃A�C�R�����g�p
bool ShowHideAtr;				//�B���t�@�C����\��
bool ShowSystemAtr;				//�V�X�e���t�@�C����\��
bool DispRegName;				//�p�X�̕\���ɓo�^����p����
bool UncToNetDrive;				//UNC���l�b�g���[�N�h���C�u����
bool PathInTitleBar;			//�^�C�g���o�[�ɃJ�����g�p�X��\��
bool OmitEndOfName;				//�������O�͖������ȗ�
bool ShowDirJumboIco;			//�f�B���N�g���͓���A�C�R����\��
bool ShowInZipImg;				//ZIP���̉摜��\��
bool PreviewAniGif;				//�A�j���[�V����GIF�̃v���r���[
bool ForceDel;					//�Ǎ���p�t�@�C���������I�ɏ㏑���E�폜
bool RemoveCdReadOnly;			//CD-ROM����̃R�s�[���ɓǍ���p����������
bool CopyNoBuffering;			//�o�b�t�@�[�Ȃ�I/O���g�p���ăR�s�[
bool ShowArcCopyProg;
bool DelUseTrash;				//�t�@�C���̍폜�ɃS�~�����g��
bool EditNewText;				//�V�K�e�L�X�g�쐬��ɃG�f�B�^�ŊJ��
bool ViewArcInf;				//�A�[�J�C�u���̃t�@�C������\��
bool ReloadOnActive;			//�A�N�e�B�u���ɍŐV�̏���
bool OpenAddedDrive;			//�h���C�u���ǉ����ꂽ��J�����g�ɕ\��
bool CheckUnc;					//UNC�p�X���`�F�b�N
bool ShowMsgHint;				//�x���Ȃǂ��q���g�\��
bool ShowKeyHint;				//2�X�g���[�N����̃q���g�\��
bool ShowNoKeyHint;				//������2�X�g���[�N����̌x�����o��
bool ShowCopyHint;				//�N���b�v�{�[�h�ւ̃R�s�[�����q���g�\��
bool ShowDuplNotify;			//��d�N�����ꂽNyanFi����̒ʒm��\��
bool LogErrOnly;				//�^�X�N�ŃG���[�ӏ��݂̂����O�\��
bool LogErrMsg;					//���O�ɃG���[���b�Z�[�W���o��
bool LogDebugInf;				//���O�Ƀf�o�b�O�����o��
bool LogHideSkip;				//���O�ɃX�L�b�v���ڂ��o�͂��Ȃ�
bool LogFullPath;				//���O�̏������e���t���p�X�ŕ\��
bool SaveLog;					//�I�����Ƀ��O��ۑ�
bool AppendLog;					//�����̃��O�͒ǋL�ۑ�
bool NotSortWorkList;			//���[�N���X�g�̓\�[�g���Ȃ�
bool AutoDelWorkList;			//���݂��Ȃ����ڂ������폜
bool DirHistCsrPos;				//�f�B���N�g�������ŃJ�[�\���ʒu���L��
bool DelDplDirHist;				//�d������f�B���N�g���������폜
bool WorkToDirHist;				//���[�N���X�g�𗚗��Ɋ܂߂�
bool NoCheckDirHist;			//�f�B���N�g�������̑��݃`�F�b�N���s��Ȃ�
bool ExtSaveDirHist;			//�S�̗�����ʃt�@�C���ɕۑ�
bool LoopFilerCursor;			//�t�@�C���[�ŃJ�[�\�������[�v�ړ�
bool FlCursorVisible;			//�t�@�C���[�ŃJ�[�\������ɉ��̈��
bool AutoCompComboBox;			//�R���{�{�b�N�X���͂������⊮
bool DialogCenter;				//�_�C�A���O�����C����ʂ̒�����
bool MenuAutoHotkey;			//���j���[�̃A�N�Z�����[�^�L�[����������
bool EscapeHelp;				//ESC�L�[�Ńw���v�����
bool InhbitAltMenu;				//ALT�L�[�Ń��j���[�Ƀt�H�[�J�X���Ȃ�
bool SelectByMouse;				//�}�E�X�Ńt�@�C���[�̍��ڂ�I��
bool SelectBaseOnly;			//�t�@�C�����啔�ł̂ݑI��
bool SelectIconSngl;			//�A�C�R�������ŌʂɑI��
bool TimColEnabled;				//�^�C���X�^���v�̔z�F��L��
bool PriorFExtCol;				//�g���q�����͑����F���D��
bool ColorOnlyFExt;				//�g���q�ʔz�F�͊g���q�����݂̂ɓK�p
bool ShowMainMenu;				//���C�����j���[��\��
bool ShowImgPreview;			//�C���[�W�v���r���[��\��
bool ShowProperty;				//�t�@�C������\��
bool ShowFileListOnly;			//�t�@�C�����X�g�݂̂�\��
bool ShowFileListOnly2;
bool ShowSttBar;				//�X�e�[�^�X�o�[��\��
bool ShowToolBar;				//�c�[���o�[��\��
bool ShowToolBarV;
bool ShowToolBarI;
bool ToolBarISide;
bool ShowTabBar;				//�^�u�o�[��\��
bool ShowFKeyBar;				//�t�@���N�V�����L�[�o�[��\��
bool NoClsTabBtn;				//�^�u�̕���{�^����\��
bool HideScrBar;				//�c�X�N���[���o�[���B��
bool ShowByteSize;				//�o�C�g�P�ʕ\��
bool ShowHeader;				//�w�b�_��\��
bool FixListWidth;				//���X�g�����Œ�
bool KeepOnResize;				//�E�B���h�E�T�C�Y�ύX���ɔ䗦���ێ�
bool KeepCurListWidth;			//�J�����g���̃��X�g�����ێ�
bool NoSpaceFExt;				//�g���q�𕪗����Ȃ�
bool HideTitleMenu;				//�ő剻���Ƀ^�C�g���o�[�ƃ��j���[���B��
bool FlatInfPanel;				//���p�l�����t���b�g��
bool ShowLineNo;				//�s�ԍ���\��
bool ShowLineCursor;			//�s�J�[�\����\��
bool ShowTAB;					//�^�u��\��
bool ShowCR;					//���s��\��
bool ShowTextRuler;				//���[����\��
bool TxtSttIsBottom;			//���w�b�_�������ɕ\��(TV)
bool ChkAozora;					//�󕶌Ɍ`�����l������
bool UseXd2tx;					//xd2txlib.dll �Ńe�L�X�g�\��
bool ClickableUrl;				//�_�u���N���b�N��URL���J��
bool RestoreViewLine;			//�r���A�[�ŃJ�[�\���̍s�ʒu�𕜌�
bool TvCursorVisible;			//�r���A�[�ŃJ�[�\������ɉ��̈��
bool LimitBinCsr;				//�o�C�i���\���ŃJ�[�\���ړ��𐧌�
bool TxtColorHint;				//�J�[�\���ʒu���l�̃J���[���q���g�\��
bool BinMemMaped;				//�o�C�i���ł̓������}�b�v�h�t�@�C���Ƃ��ĊJ��
bool EmpComment;				//�R�����g�������\��
bool EmpStrings;				//������������\��
bool EmpReserved;				//�\���������\��
bool EmpSymbol;					//�V���{���������\��
bool EmpNumeric;				//���l�������\��
bool EmpHeadline;				//���o���������\��
bool EmpRuby;					//���r�������\��
bool RotViewImg;				//Exif���ɂ��������ĉ摜����]
bool KeepZoomRatio;				//�ʂ̉摜�Ɉڂ������ɃY�[���{�����ێ�����
bool ShowThumbName;				//�T���l�C���Ƀt�@�C������\��
bool ShowThumbExif;				//�T���l�C���ɊȈ�Exif����\��
bool ShowThumbFExt;				//�T���l�C���Ɋg���q�������\��
bool NotThumbIfArc;				//�A�[�J�C�u���͈ꊇ�擾���Ȃ�
bool LoopViewCursor;			//�C���[�W�r���A�[�ŃJ�[�\�������[�v�ړ�
bool HintTopEndImg;				//�擪�E�Ō�Ńq���g�\��
bool BeepTopEndImg;				//�x����
bool ImgSttIsBottom;			//���w�b�_�������ɕ\��(IV)
bool SeekBindDir;				//�V�[�N�o�[�̎n�_��Ԃ������ɍ��킹��
bool SeekSwapNxtPrv;			//�E�Ԃ���Next/PrevFile����
bool OneStepInDblPg;			//���J���\���ł�1�t�@�C�����ړ�
bool HideCsrInFull;				//�S��ʕ\���Ń}�E�X�J�[�\�����B��
bool HideThumbInFull;			//�S��ʕ\���ŃT���l�C�����B��
bool AnimateGif;				//�C���[�W�r���A�[�ŃA�j���[�V����GIF��\��
bool ShowThumbScroll;			//�T���l�C���̃X�N���[���o�[��\��
bool ShowHistogram;				//�q�X�g�O������\��
bool ShowLoupe;					//���[�y��\��
bool ShowGifViewer;				//GIF�r���A�[��\��
bool ShowSeekBar;				//�V�[�N�o�[��\��
bool WarnHighlight;				//����ьx��
bool DoublePage;				//���J���\��
bool RightBind;					//�E�Ԃ�

bool PermitDotCmds;				//.nyanfi �ŃR�}���h���s������
bool InheritDotNyan;			//��ʃf�B���N�g������ .nyanfi ���p��
bool DotNyanPerUser;			//���[�U���ʂ� .nyanfi ���쐬

int  ScrBarStyle;				//�X�N���[���o�[�E�X�^�C��

bool ModalScreen;				//���[�_���\������
int  ModalScrAlpha;

int  InitialModeI;				//�C���[�W�r���A�[���J�������̏������
int  LastZoomRatio;				//�O��̃Y�[��

bool MarkImgClose;				//�C���[�W�r���A�[����鎞�Ƀ}�[�N
UnicodeString MarkImgPath;		//�}�[�N����p�X
UnicodeString MarkImgFExt;		//�}�[�N����g���q
UnicodeString MarkImgMemo;

UnicodeString DlgMoveShift;		//�_�C�A���O�ړ��̃V�t�g�L�[
UnicodeString DlgSizeShift;		//�_�C�A���O�̃T�C�Y�ύX�̃V�t�g�L�[
int  DlgMovePrm;
int  DlgSizePrm;

UnicodeString WheelCmdF[4];		//�z�C�[���R�}���h
UnicodeString WheelCmdV[4];
UnicodeString WheelCmdI[4];

UnicodeString WheelBtnCmdF[4];	//�z�C�[���{�^�������������̃R�}���h
UnicodeString WheelBtnCmdV;
UnicodeString WheelBtnCmdI;

UnicodeString X1BtnCmdF;		//X1�{�^�������������̃R�}���h
UnicodeString X2BtnCmdF;		//X2�{�^�������������̃R�}���h
UnicodeString X1BtnCmdV;
UnicodeString X2BtnCmdV;
UnicodeString X1BtnCmdI;
UnicodeString X2BtnCmdI;

UnicodeString EmpBinPtn1;		//�o�C�i�������p�^�[��1
UnicodeString EmpBinPtn2;		//�o�C�i�������p�^�[��2
UnicodeString EmpBinPtn3;		//�o�C�i�������p�^�[��3

UnicodeString HtmInsHrCls;		//�u���b�N�̑O�Ɍr����}������N���X
UnicodeString HtmHdrStr;		//���o������
bool HtmInsHrSct;				//<section>�̑O�Ɍr���}��
bool HtmInsHrArt;				//<article>�̑O�Ɍr���}��
bool HtmInsHrNav;				//<nav>�̑O�Ɍr���}��
bool ToMarkdown;				//Markdown�L�@�ɕϊ�

UnicodeString NoWatchPath;		//�f�B���N�g���Ď����珜�O����p�X

UnicodeString FExtGetInf;		//�t�@�C�������擾����g���q
UnicodeString FExtNoInf;		//�t�@�C�������擾���Ȃ��g���q
UnicodeString NoInfPath;		//�t�@�C�������擾���Ȃ��p�X���X�g

UnicodeString FExtImgPrv;		//�C���[�W�v���r���[���s���g���q
UnicodeString FExtNoImgPrv;		//�C���[�W�v���r���[���s��Ȃ��g���q
UnicodeString NoImgPrvPath;		//�C���[�W�v���r���[���s��Ȃ��p�X���X�g

UnicodeString FExtNoIView;		//�C���[�W�r���A�[�ŉ{�����Ȃ��g���q

UnicodeString DrvInfFmtR;		//�h���C�u���̏��� : ���[�g
UnicodeString DrvInfFmtS;		//�h���C�u���̏��� : �I����
UnicodeString DrvInfFmtN;		//�h���C�u���̏��� : ���̑�
bool SelColDrvInf;				//�I�����̓t�@�C�����X�g�̔z�F��

UnicodeString SttBarFmt;		//�X�e�[�^�X�o�[�̏���
UnicodeString SttClockFmt;		//���v�̏���

int  MaxLogFiles;				//���O�t�@�C���̕ۑ����㐔

bool SureCopy;					//�m�F - �R�s�[
bool SureMove;					//�m�F - �ړ�
bool SureDelete;				//�m�F - �폜
bool SureUnPack;				//�m�F - ��
bool SureExec;					//�m�F - ���s
bool SureOther;					//�m�F - ���̑�
bool SureExit;					//�m�F - �I��
bool SureCmpDel;				//�m�F - ���S�폜��2�x�m�F
bool SureCancel;				//�m�F - �L�����Z���{�^����\��
bool SureDefNo;					//�m�F - �u�������v���f�t�H���g

int  ProtectDirMode;			//�f�B���N�g���̍폜�������[�h
bool ProtectSubDir;				//���ʃf�B���N�g��������
bool ProtectFile;				//�t�@�C��������

int  BorderMoveWidth;			//BorderLeft�ABorderRight: �P��̈ړ���
UnicodeString AutoRenFmt;		//Copy�AMove: �������̎�����������

int  RemoteWaitTime;			//�R�[���o�b�N���̃E�F�C�g�^�C��
int  NormalWaitTime;
int  NopDtctTime;				//�����삾�Ƃ݂Ȃ�����
int  TimeTolerance;				//�^�C���X�^���v�̋��e�덷

bool AppListChgMin;				//AppList:    ���A�v���ɐ؂�ւ����Ƃ��ɍŏ���
bool CreDirChg;					//CreateDir:  �f�B���N�g���쐬��ɃJ�����g�ύX
bool CreDirCnvChar;				//CreateDir:  �����u����K�p
UnicodeString FExtExeFile;		//ExeCommandLine: ���s�t�@�C���Ƃ݂Ȃ��g���q
int  OpenByMode;				//OpenByApp:  �֘A�t������Ă��Ȃ��ꍇ�̓��샂�[�h
bool OpenByCmdPrompt;			//OpenByApp:  [..] �ŃR�}���h�v�����v�g���J��
bool OpenDirByStd;				//OpenByApp:  �f�B���N�g���ł͕W�� Enter �L�[����
int  DblClickFlMode;			//�t�@�C�����X�g�ł̃_�u���N���b�N
bool OpenOnlyCurApp;			//OpenByApp:  �I����Ԃɂ�����炸�J�[�\���ʒu�݂̂��J��
bool OpenOnlyCurWin;			//OpenByWin:  �I����Ԃɂ�����炸�J�[�\���ʒu�݂̂��J��
bool DownAfterOpenStd;			//OpenStandard: ���s��ɃJ�[�\�������Ɉړ�
bool OpenStdOnResList;			//OpenStandard: ���ʃ��X�g�ł��ʏ퓮��
UnicodeString IniSeaShift;		//IniSearch:  �������T�[�`�̃V�t�g�L�[
bool IniSeaByNum;				//  �����L�[�ł��������T�[�`
bool IniSeaBySign;				//  Shift+�����L�[�̋L�����T�[�`
bool IncSeaCaseSens;			//IncSearch:  �召���������
bool IncSeaLoop;				//IncSearch:  �㉺�[�Ń��[�v
bool IncSeaMatch1Exit;			//IncSearch:  �}�b�`��1�Ŕ�����
int  IncSeaMigemoMin;			//IncSearch:  migemo �̌����J�n������
bool NotShowNoTask;				//TaskMan:    �^�X�N�����s���Ă��Ȃ��Ƃ��̓}�l�[�W����\�����Ȃ�
UnicodeString WebSeaUrl;		//WebSearch:  �����G���W��URL

bool FindPathColumn;			//���ʃ��X�g�Ɂu�ꏊ�v���\��
int  FindPathWidth;				//�u�ꏊ�v�̍ŏ���

bool GrepShowItemNo;			//GREP���ʂɍ��ڔԍ���\��
bool GrepFileItemNo;			//���ڔԍ����t�@�C���P�ʂŕ\��
bool GrepShowSubDir;			//GREP���ʂɃT�u�f�B���N�g������\��
bool GrepTrimTop;				//GREP���ʂōs���̃^�u��󔒂��폜
bool GrepOmitTop;				//GREP���ʂŃ}�b�`�ꂪ�����Ȃ��Ƃ��O�������ȗ�
bool GrepEmFilter;				//GREP���ʂŃt�B���^�̌�������\��
int  GrepOutMode;				//GREP �o�͐�	0:����/ 1:�t�@�C��/ 2:�N���b�v�{�[�h
UnicodeString GrepFileName;		//�o�̓t�@�C����
UnicodeString GrepAppName;		//�N���A�v����
UnicodeString GrepAppParam;		//�A�v���̋N���p�����[�^
UnicodeString GrepAppDir;		//�A�v���̍�ƃf�B���N�g��
bool GrepAppend;				//�����t�@�C���ɒǉ�

UnicodeString GrepFileFmt;		//�t�@�C�����̏���
UnicodeString GrepInsStrW;		//�}�b�`��O�̑}��������
UnicodeString GrepInsStrW2;		//�}�b�`���̑}��������
bool GrepTrimLeft;				//�s���̃^�u��󔒂��폜
bool GrepReplaceTab;			//�^�u����1�����ɒu��
bool GrepReplaceCr;				//���s�𕶎���ɒu��
UnicodeString GrepRepCrStr;		//���s�̒u��������

bool BackupReplace;				//�u���o�b�N�A�b�v�쐬
UnicodeString FExtRepBackup;	//�u���o�b�N�A�b�v�̊g���q
UnicodeString RepBackupDir;		//�u���o�b�N�A�b�v��
UnicodeString ReplaceLogName;	//�u�����O�t�@�C����

bool SaveReplaceLog;			//�u�����O��ۑ�
bool ReplaceAppend;				//�����t�@�C���ɒǉ�
bool OpenReplaceLog;			//���O���e�L�X�g�G�f�B�^�ŊJ��

UnicodeString UserName;			//���[�U��
UnicodeString TempPath;			//�ꎞ�f�B���N�g��
UnicodeString TempPathA;		//�ꎞ�f�B���N�g��(���)
UnicodeString TempPathFTP;		//FTP�p�ꎞ�f�B���N�g��(���)

UnicodeString DownloadPath;		//�_�E�����[�h�p
UnicodeString LibraryPath;		//���C�u����
UnicodeString WorkListPath;		//���[�N���X�g�Q�ƃp�X
UnicodeString RefParamPath;		//�p�����[�^�Q�ƃp�X
UnicodeString IconFilePath;		//�A�C�R���Q�ƃp�X
UnicodeString CmdFilePath;		//�R�}���h�t�@�C���Q�ƃp�X

int VersionNo;
UnicodeString VersionStr;		//�o�[�W�����\��
UnicodeString CopyrightStr;		//���쌠

UnicodeString DirBraStr;		//�f�B���N�g�����ʕ���
UnicodeString DirKetStr;
UnicodeString TimeStampFmt;		//�^�C���X�^���v�̕\������

UnicodeString CallHotKey;		//�Ăяo���z�b�g�L�[
UnicodeString AppListHotKey;	//�A�v���ꗗ�z�b�g�L�[
UnicodeString AppListHotPrm;	//�A�v���ꗗ�p�����[�^

UnicodeString SaveTxtPath;		//�e�L�X�g�ۑ��p�X
int SaveEncIndex;				//�G���R�[�f�B���O�̃C���f�b�N�X

UnicodeString NoDirHistPath;	//�f�B���N�g�������ɓ���Ȃ��p�X
UnicodeString NoEditHistPath;	//�ҏW�����ɓ���Ȃ��p�X
UnicodeString NoViewHistPath;	//�{�������ɓ���Ȃ��p�X

//---------------------------------------------------------------------------
UnicodeString FTPTextModeFExt;	//�e�L�X�g���[�h�œ]������g���q
bool FTPDlKeepTime;				//�_�E�����[�h�t�@�C���̃^�C���X�^���v���ێ�
bool FTPUpKeepTime;				//�A�b�v���[�h�t�@�C���̃^�C���X�^���v���ێ�
bool FTPUpToLower;				//�t�@�C�����������������ăA�b�v���[�h����
bool FTPLogResponse;			//�������b�Z�[�W�����O�ɕ\��
int  FTPRemoteSide;				//�����[�g�� 0:�J�����g/ 1:��/ 2:�E
int  FTPDisconTimeout;			//FTP��������̏ꍇ�ؒf���鎞��
UnicodeString FTPSndConnect;	//�ڑ����̒ʒm��
UnicodeString FTPSndDiscon;		//�ؒf���̒ʒm��
UnicodeString FTPSndTransfer;	//�]���������̒ʒm��

//---------------------------------------------------------------------------
int CurTabIndex = 0;			//�^�u�C���f�b�N�X(0�`

UnicodeString CurPathName;
int CurListTag = 0;				//�J�����g���^�O  0:��/ 1:�E
int OppListTag = 1;				//���Α��^�O
int LastCurTag = 0;				//�O��I�����̃J�����g

TListBox *FileListBox[MAX_FILELIST];	//�t�@�C�����X�g�{�b�N�X

UnicodeString ViewFileName;				//�\�����̃t�@�C����
UnicodeString ViewFileName2;

//---------------------------------------------------------------------------
TStringList *GeneralList;				//TStringList �̃��X�g(�ꊇ�j���p)

TStringList *FileList[MAX_FILELIST];	//�t�@�C�����X�g
TStringList *ArcFileList[MAX_FILELIST];	//���z�f�B���N�g���̃t�@�C�����X�g
TStringList *ResultList[MAX_FILELIST];	//�������ʃ��X�g
TStringList *SelMaskList[MAX_FILELIST];	//�I���}�X�N���X�g
TStringList *TmpBufList;				//�ꎞ�ޔ����X�g

TStringList *TabList;					//�^�u���X�g
int FlTabWidth;							//�^�u�̕�
int FlTabStyle;							//�^�u�̃X�^�C��

TStringList *DriveInfoList;				//�h���C�u���
TStringList *DriveLogList;				//�h���C�u�e�ʃ��O
UnicodeString DriveLogName;				//�h���C�u�e�ʃ��O�̃t�@�C����

TStringList *WatchTailList;				//�����Ď����X�g
UnicodeString LastWatchLog;				//���O�̊Ď����O

TStringList *InvalidUncList;			//������UNC���X�g

//�t�@�C���ŗL�A�C�R���̃L���b�V�� (�K�� IconRWLock �ŕی삷�邱��)
TStringList *CachedIcoList;
TMultiReadExclusiveWriteSynchronizer *IconRWLock;
int IconCache;							//�A�C�R���L���V����

TStringList *GeneralIconList;			//�t�@�C�����X�g�\���p�̈�ʃA�C�R��
TStringList *MenuBtnIcoList;			//

TStringList *UsrIcoList;				//���j���[�A�c�[���{�^���p�A�C�R��
TStringList *DrvIcoList;				//�h���C�u�̃A�C�R�����X�g

TStringList *BakSetupList;				//�o�b�N�A�b�v�ݒ胊�X�g
TStringList *SyncDirList;				//�����R�s�[�惊�X�g
TStringList *AssRenList;				//�֘A�t�@�C�����������ݒ胊�X�g
TStringList *DistrDefList;				//�U�蕪�����`���X�g

TStringList *GrepPathList;				//GREP �Ώۃp�X�̃��X�g
TStringList *GrepFileList; 				//GREP �Ώۃt�@�C�����X�g
TStringList *GrepResultList;			//GREP �̌��ʃ��X�g
TStringList *GrepResultBuff;			//GREP ���ʂ̑ޔ��o�b�t�@

TStringList *ViewFileList;				//�C���[�W�r���A�[�ł̃t�@�C�������X�g
bool isViewIcon   = false;				//�C���[�W�r���A�[�ŃA�C�R����\����
bool isViewAGif   = false;				//�C���[�W�r���A�[�ŃA�j���[�V����GIF��\����
bool isViewClip   = false;				//�C���[�W�r���A�[�ŃN���b�v�{�[�h��\����
bool isViewFTP	  = false;				//�C���[�W�r���A�[��FTP�̃t�@�C����\����
bool rqThumbnail  = false;				//�N���b�v�{�[�h�{����̃T���l�C�����A�v��
bool nrmThumbnail = false;				//�ʏ��ʎ��̃T���l�C���\��

TStringList *LaunchList;				//�����`���[���X�g(�A�v���P�[�V�����ꗗ)

TStringList *CmdFileList;				//�R�}���h�t�@�C���̃��X�g
TStringList *XCMD_VarList = NULL;		//�ϐ����X�g

UnicodeString PathMask[MAX_FILELIST];	//�p�X�}�X�N
UnicodeString DriveInf[MAX_FILELIST];	//�h���C�u���

flist_stt ListStt[MAX_FILELIST];
flist_stt *CurStt;
flist_stt *OppStt;

TStringList *WorkList;					//���[�N���X�g
UnicodeString WorkListName;				//���[�N���X�g�̃t�@�C����
TDateTime WorkListTime;					//���[�N���X�g�̃^�C���X�^���v
bool WorkListChanged;					//���[�N���X�g�̓��e���ύX���ꂽ
bool WorkListFiltered;					//���[�N���X�g�Ƀt�B���^���K�p����Ă���
bool WorkListHasSep;					//���[�N���X�g�ɃZ�p���[�^������
bool rqWorkListDirInf;					//���[�N���X�g�̃f�B���N�g�����X�V��v��

TStringList  *PlayList;					//�v���C���X�g
UnicodeString PlayListFile;				//�v���C���X�g�̃t�@�C����
UnicodeString PlayFile;					//�Đ����t�@�C����
int  PlayStbIdx;
bool PlayRepeat;						//���s�[�g�Đ�
bool PlayShuffle;						//�V���b�t���Đ�
bool ListShuffled;						//�V���b�t�����ꂽ

TStringList *PopMenuList;				//ExeMenuFile �R�}���h�p���j���[����
TStringList *ToolBtnList;				//�c�[���{�^����`���X�g
TStringList *ToolBtnListV;
TStringList *ToolBtnListI;

//---------------------------------------------------------------------------
//�N�����I�v�V����
int  IniPathMode[MAX_FILELIST];			//�p�X���[�h
UnicodeString InitialPath[MAX_FILELIST];
UnicodeString InitialMask[MAX_FILELIST];
int  IniSortMode[MAX_FILELIST];
int  IniWorkMode;						//���[�N���X�g 0:�O��/ 1:�w��
UnicodeString HomeWorkList;
int  IniWinMode;						//�T�C�Y���[�h 0:�O��/ 1:�w��
int  IniWinLeft, IniWinTop, IniWinWidth, IniWinHeight;
bool FixWinPos;							//�E�B���h�E�T�C�Y���Œ�
bool IniPathToTab1;						//�����p�X���^�u1�ɐݒ�
bool IniTabHomeAll;						//�����p�X�ȊO�̃^�u���z�[���ɖ߂�
bool ShowSplash;						//�X�v���b�V����\��

//�t�@�C�����X�g�̃\�[�g���[�h
int  SortMode[MAX_FILELIST];			//0:���O/ 1:�g���q/ 2:�X�V����/ 3:�T�C�Y/ 4:����
int  DirSortMode[MAX_FILELIST];			//0:�t�@�C���Ɠ���/ 1:���O/ 2:�X�V����/ 3:�T�C�Y/ 4:����/ 5:�f�B���N�g������ʂ��Ȃ�

//�t�@�C�����X�g�̃\�[�g��
bool FlOdrNatural[MAX_FILELIST];		//���R��
bool FlOdrDscName[MAX_FILELIST];		//�~��
bool FlOdrSmall[MAX_FILELIST];			//��������
bool FlOdrOld[MAX_FILELIST];			//�Â���
bool FlOdrDscAttr[MAX_FILELIST];		//�����~��
bool FlOdrDscPath[MAX_FILELIST];		//�ꏊ�~��

bool SortBoth;					//���E�Ƃ��ύX

//��r�֐��̃\�[�g��
bool NaturalOrder = true;		//���R��
bool DscNameOrder = false;		//���O�~��
bool SmallOrder   = false;		//��������
bool OldOrder	  = false;		//�Â���
bool DscAttrOrder = false;		//�����~��
bool DscPathOrder = false;		//�ꏊ�~��

TStringList *OptionList;		//�I�v�V����
TStringList *KeyFuncList;		//�L�[���蓖��
TStringList *FKeyLabelList;		//�t�@���N�V�����L�[��
TStringList *HotKeyList;		//�z�b�g�L�[
TStringList *DirStack;			//�f�B���N�g���X�^�b�N
TStringList *PathMaskList;		//�p�X�}�X�N���X�g
TStringList *RegDirList;		//�o�^�f�B���N�g��
TStringList *ProtectDirList;	//�폜�����f�B���N�g��
TStringList *ColorList;			//�z�F
TStringList *ExtColList;		//�g���q�ʔz�F
TStringList *AssociateList;		//�֘A�t��
TStringList *OpenStdCmdList;	//OpenStandard �̃R�}���h�֘A�t��
TStringList *EtcEditorList;		//���̑��̃G�f�B�^���X�g
TStringList *ExtToolList;		//�O���c�[��
TStringList *ExtMenuList;		//�ǉ����j���[
TStringList *CommandList;		//�R�}���h���X�g
TStringList *CmdSetList;		//�R�}���h�ꗗ���X�g
TStringList *TaskCmdList;		//�^�X�N�R�}���h�����X�g
TStringList *DriveList;			//�ω��Ď��p�h���C�u���X�g
TStringList *NetDriveList;		//���ݗL���ȃl�b�g�h���C�u���X�g
TStringList *AllDirHistory;		//�S�̂̃f�B���N�g������
TStringList *TextViewHistory;	//�e�L�X�g�{������
TStringList *TextEditHistory;	//�e�L�X�g�ҏW����
TStringList *WorkListHistory;	//���[�N���X�g����
TStringList *InputDirHistory;	//�f�B���N�g�����̗͂���
TStringList *InputCmdsHistory;	//�R�}���h���̗͂���
TStringList *InputCmdsHistoryV;
TStringList *InputCmdsHistoryI;
TStringList *IncSeaHistory;		//�C���N�������^���T�[�`�̃L�[���[�h����
TStringList *FilterHistory;		//�t�B���^�̃L�[���[�h����
TStringList *WebSeaHistory;		//Web������̗���
TStringList *LatLngHistory;		//�ܓx�o�x�����̗���
TStringList *HeadlineList;		//���o�����X�g (�g���q=���K�\���p�^�[��)
TStringList *ZoomRatioList;		//�Y�[���{�����X�g
TStringList *CnvCharList;		//�֎~�����^���[�U��`�����ϊ����X�g
TStringList *RenCmdFileList;	//���������R�}���h�t�@�C���̃��X�g
TStringList *RedrawList;		//�`��}�~�p���X�g
TStringList *CmdRequestList;	//�R�}���h�v�����X�g

//�^�X�N����̃��O�����o�b�t�@ (�K�� LogRWLock �ŕی삷�邱��)
UnicodeString LogBufStr;
TMultiReadExclusiveWriteSynchronizer *LogRWLock;

TListBox     *LogWndListBox;	//���O�E�B���h�E�p���X�g�{�b�N�X
TStringList  *LogBufList;		//���O���X�g�{�b�N�X(���z)�p�o�b�t�@
UsrScrollPanel *LogWndScrPanel;	//�V���v���X�N���[���o�[

int  LayoutMode;		//��ʃ��C�A�E�g���[�h
bool DivFileListUD;		//�t�@�C�����X�g���㉺�ɕ���
bool DivDirInfUD;		//�f�B���N�g�������㉺�ɕ���

//�t�H���g
TFont *ListFont;		//�t�@�C�����X�g��
TFont *FileInfFont;		//�t�@�C�����
TFont *TxtPrvFont;		//�e�L�X�g�v���r���[
TFont *LogFont;			//���O
TFont *ViewerFont;		//�e�L�X�g�r���A�[
TFont *GrepResFont;		//GREP���ʃ��X�g
TFont *DirInfFont;		//�f�B���N�g�����
TFont *DrvInfFont;		//�h���C�u���
TFont *LstHdrFont;		//�ꗗ�̃w�b�_
TFont *ViewHdrFont;		//�r���A�[���w�b�_
TFont *GenListFont;		//�ꗗ�_�C�A���O
TFont *HintFont;		//�q���g
TFont *SttBarFont;		//�X�e�[�^�X�o�[
TFont *CalcFont;		//�d��
TFont *DialogFont;		//�_�C�A���O
TFont *ToolBarFont;		//�c�[���o�[
TFont *TabBarFont;		//�^�u�o�[
TFont *CharInfFont;		//�������
TStringList *FontList;

//�J���[
TColor col_bgList;		//�ꗗ�̔w�i�F
TColor col_bgList2;		//�ꗗ�̔w�i�F(����)
TColor col_fgList;		//�ꗗ�̕����F
TColor col_Splitter;	//�������E��
TColor col_bgArc;		//���z�f�B���N�g���̔w�i�F
TColor col_bgFind;		//�������ʃ��X�g�̔w�i�F
TColor col_bgWork;		//���[�N���X�g�̔w�i�F
TColor col_bgFTP;		//FTP�����[�g���̔w�i�F
TColor col_selItem;		//�I�����ڂ̔w�i�F
TColor col_oppItem;		//���΃p�X�I�����ڂ̔w�i�F
TColor col_fgSelItem;	//�I�����ڂ̕����F
TColor col_bgMark;		//�}�[�N���ڂ̔w�i�F
TColor col_matchItem;	//�C���N�������^���T�[�`�Ń}�b�`
TColor col_Cursor;		//���C���J�[�\���̐F
TColor col_bgScrBar;	//�V���v���X�N���[���o�[�̔w�i�F
TColor col_bgScrKnob;	//�V���v���X�N���[���m�u�̐F
TColor col_frScrKnob;	//�V���v���X�N���[���m�u�̗֊s�F
TColor col_Folder;		//�f�B���N�g���̕����F
TColor col_SymLink;		//�V���{���b�N�����N�̕����F
TColor col_Protect;		//�폜�����f�B���N�g���̕����F
TColor col_ReadOnly;	//�ǂݍ��ݐ�p�����̕����F
TColor col_Hidden;		//�B�������̕����F
TColor col_System;		//�V�X�e�������̕����F
TColor col_fgTagNam;	//�^�O���̕����F
TColor col_InvItem;		//�����ȍ��ڂ̕����F
TColor col_bgTabBar;	//�^�u�o�[�̔w�i�F
TColor col_bgActTab;	//�A�N�e�B�u�ȃ^�u�̔w�i�F
TColor col_bgInAcTab;	//��A�N�e�B�u�ȃ^�u�̔w�i�F
TColor col_frmTab;		//�^�u�̗֊s�F
TColor col_fgTab;		//�^�u�̕����F
TColor col_bgListHdr;	//���X�g�w�b�_�̔w�i�F
TColor col_fgListHdr;	//���X�g�w�b�_�̕����F
TColor col_bgDirInf;	//�f�B���N�g�����̔w�i�F
TColor col_fgDirInf;	//�f�B���N�g�����̕����F
TColor col_bgDirRel;	//�f�B���N�g���֌W�̔w�i�F
TColor col_fgDirRel;	//�f�B���N�g���֌W�̕����F
TColor col_bgDrvInf;	//�h���C�u���̔w�i�F
TColor col_fgDrvInf;	//�h���C�u���̕����F
TColor col_bgInf;		//�t�@�C�����̔w�i�F
TColor col_fgInf;		//�t�@�C�����̕����F
TColor col_fgInfNam;	//�t�@�C�����̍��ږ������F
TColor col_bgTxtPrv;	//�e�L�X�g�v���r���[�̔w�i�F
TColor col_fgTxtPrv;	//�e�L�X�g�v���r���[�̕����F
TColor col_bgLog;		//���O�̔w�i�F
TColor col_fgLog;		//���O�̕����F
TColor col_bgTask;		//�^�X�N��ԕ\���̔w�i�F
TColor col_fgPrgBar;	//�^�X�N�i���o�[�F
TColor col_bgPrgBar;	//�^�X�N�i���w�i�F
TColor col_Error;		//�G���[/���ӂ̕����F

TColor col_bgView;		//�e�L�X�g�r���A�[�̔w�i�F
TColor col_fgView;		//�e�L�X�g�r���A�[�̕����F
TColor col_Margin;		//�e�L�X�g�r���A�[�̗]����
TColor col_bgRuler;		//���[���̔w�i�F
TColor col_fgRuler;		//���[���̖ڐ��F
TColor col_bgLineNo;	//�s�ԍ��w�i�F
TColor col_LineNo;		//�s�ԍ������F
TColor col_Mark;		//�s�}�[�N
TColor col_bdrLine;		//�s�ԍ��̋��E��
TColor col_bdrFold;		//�܂�Ԃ����E��
TColor col_bdrFixed;	//�Œ蒷�\���̏c�r��
TColor col_Comment;		//�R�����g�̕����F
TColor col_Strings;		//������̕����F
TColor col_Reserved;	//�\���̕����F
TColor col_Symbol;		//�V���{���̕����F
TColor col_Numeric;		//���l�̕����F
TColor col_fgEmpBin1;	//�o�C�i�����������F1
TColor col_fgEmpBin2;	//�o�C�i�����������F2
TColor col_fgEmpBin3;	//�o�C�i�����������F3
TColor col_Headline;	//���o���̕����F
TColor col_URL;			//URL�̕����F
TColor col_LocalLink;	//���[�J�t�@�C���ւ̃����N
TColor col_fgEmp;		//���������F
TColor col_bgEmp;		//�����w�i�F
TColor col_Ruby;		//���r
TColor col_TAB;			//�^�u�\���F
TColor col_CR;			//���s�\���F
TColor col_HR;			//�r���̐F
TColor col_Ctrl;		//�R���g���[���R�[�h

TColor col_bgImage;		//�摜�̔w�i�F
TColor col_bgDblPg;		//���J���\���̗]���F
TColor col_bgWMF;		//���^�t�@�C���̔w�i�F
TColor col_bdrThumb;	//�T���l�C���̋��E��
TColor col_ThumbExif;	//�T���l�C����Eixf���
TColor col_ImgGrid;		//�摜�����O���b�h
TColor col_OptFind;		//�I�v�V�����ݒ�̌�������
TColor col_bgHint;		//�q���g�\���̔w�i�F
TColor col_fgHint;		//�q���g�\���̕����F
TColor col_bgWarn;		//�x���\���̔w�i�F
TColor col_Invalid;		//�����ȍ��ڂ̔w�i�F
TColor col_Illegal;		//�s���ȓ��͍��ڂ̔w�i�F
TColor col_Tim1H;		//1���Ԉȓ��̃^�C���X�^���v�F
TColor col_Tim3H;		//3���Ԉȓ��́`
TColor col_Tim1D;		//1���ȓ��́`
TColor col_Tim3D;		//3���ȓ��́`
TColor col_Tim7D;		//7���ȓ��́`
TColor col_Tim1M;		//1�����ȓ��́`
TColor col_Tim3M;		//3�����ȓ��́`
TColor col_Tim6M;		//6�����ȓ��́`
TColor col_Tim1Y;		//1�N�ȓ��́`
TColor col_Tim3Y;		//3�N�ȓ��́`
TColor col_TimOld;		//������O�́`
TColor col_Size4G;		//4GB�ȏ�
TColor col_Size1G;		//1GB�ȏ�
TColor col_Size1M;		//1MB�ȏ�
TColor col_Size1K;		//1KB�ȏ�
TColor col_SizeLT;		//1KB����
TColor col_Size0;		//0

TColor col_bgTlBar1;	//�c�[���o�[�̃O���f�[�V�����J�n�F
TColor col_bgTlBar2;	//�c�[���o�[�̃O���f�[�V�����I���F
TColor col_fgTlBar;		//�c�[���o�[�̕����F
TColor col_htTlBar;		//�c�[���o�[�z�b�g�g���b�L���O�F

TColor col_bgSttBar;	//�X�e�[�^�X�o�[�̔w�i�F
TColor col_fgSttBar;	//�X�e�[�^�X�o�[�̕����F
TColor col_bgInfHdr;	//���w�b�_�̔w�i�F
TColor col_fgInfHdr;	//���w�b�_�̕����F

TColor col_ModalScr;	//���[�_���\�����ʐF

TColor col_Teal = clTeal;

//�w�i�摜
Graphics::TBitmap *BgImgBuff[MAX_BGIMAGE];
UnicodeString BgImgName[MAX_BGIMAGE];

int  BgImgMode;					//���[�h
int  BgImgSubMode;
int  BgColAlpha;				//�w�i�F�A���t�@
bool BgImgGray;					//�O���[�X�P�[����
bool BgImgHideScr;				//�X�N���[�����ɉB��
int  BgHideTime;				//���A����
bool BgImgTileIf;				//�w��T�C�Y�ȉ��Ȃ���ׂĕ\��
int  BgTileSize;

bool AlphaForm;
int  AlphaValue;

//�O���c�[��
UnicodeString TextEditor;		//�e�L�X�g�G�f�B�^
UnicodeString TextEditorFrmt;	//�p�����[�^(Grep�p)

UnicodeString ExtTxViewer;		//�O���e�L�X�g�r���A�[
UnicodeString ExtTxViewerFrmt;

UnicodeString ImageEditor;		//�C���[�W�G�f�B�^
UnicodeString FExtImgEidt;		//�C���[�W�G�f�B�^�̑Ώۊg���q
bool ImageEditSgl;				//�C���[�W�G�f�B�^���t�@�C�����ɌʋN��

UnicodeString FExtViewTab4;		//�^�u4�̊g���q

UnicodeString BinaryEditor;		//�o�C�i���G�f�B�^

//�T�E���h
UnicodeString SoundTaskFin;		//�^�X�N�I�����̒ʒm��
UnicodeString SoundFindFin;		//�����I�����̒ʒm��
UnicodeString SoundWarning;		//�x����
UnicodeString SoundWatch;		//�t�@�C���Ď�

int  FlashCntWarning;			//�x���̓_�ŉ�
int  FlashTimeWarning;			//�x���̓_�ŊԊu
int  FlashCntTaskfin;			//�^�X�N�I�����̓_�ŉ�
int  FlashTimeTaskfin;			//�^�X�N�I�����̓_�ŊԊu

bool WarnPowerFail;				//�d���ؒf�x��
bool WarnDisconnect;			//�C���^�[�l�b�g�ڑ��؂�
bool WarnLowBattery;			//�o�b�e���[�ቺ
int  BatLowerLimit;

int  MaxLogLines;				//���O�̍ő�s��
int  MsgHintTime;				//�q���g�̕\������
int  KeyHintTime;				//2�X�g���[�N�q���g�̑ҋ@����
int  NotifyTaskTime;			//�`�b�ȏォ�������^�X�N�̏I�����ɖ炷
int  CmpDelOwCnt;				//���S�폜�̏㏑����
int  ViewTxtInterLn;			//�r���A�[�̍s��
int  ViewLeftMargin;			//�r���A�[�̍��]��
int  ViewFoldWidth;				//�܂�Ԃ���(���p�P��)
bool ViewFoldFitWin;			//�܂�Ԃ����E�B���h�E���ɍ��킹��
int  ViewFixedLimit;			//�Œ蒷�\���̍ő啝
int  ViewTxtLimitSize;			//�e�L�X�g�̍ő�Ǎ��T�C�Y
int  ViewBinLimitSize;			//�o�C�i���̍ő�Ǎ�(or �}�b�v)�T�C�Y
int  ListWheelScrLn;			//�z�C�[���ɂ��X�N���[���s��(���X�g)
int  ViewWheelScrLn;			//�z�C�[���ɂ��X�N���[���s��(�r���A�[)
int  PlaySndLimitTime;			//�T�E���h�Đ��^�C������
bool ShowTextPreview;			//�e�L�X�g�v���r���[��\��
int  PrvTxtInfLn;				//�e�L�X�g�v���r���[�̍s��
bool ShowTailPreview;			//�����v���r���[��\��
int  PrvTxtTailLn;				//�����v���r���[�s��
int  PrvActTailLn;				//�����v���r���[��L���ɂ���s��
bool LimitGetRarInf;			//RAR�̃t�@�C�����擾���T�C�Y�Ő���
int  GetInfLimitSize;			//RAR�̃t�@�C�����擾�̐����T�C�Y
int  WatchInterval;				//�f�B���N�g���Ď��Ԋu
int  LogInterval;				//���O�̍X�V�Ԋu
int  ListInterLn;				//�t�@�C�����X�g�̍s��
int  CursorWidth;				//�J�[�\�����̕�
int  CursorAlpha;				//�J�[�\���s�w�i�A���t�@
int  SplitterWidth;				//���E���̕�
int  SplitterWidth2;
int  SizeFormatMode;
int  SizeDecDigits;				//�T�C�Y�\���ɂ����鏬���_�ȉ��̌���
int  ListPercent;				//�t�@�C�����X�g���̔䗦
int  ImgFrameMargin;			//�t�B�b�g�\�����̗]����
int  ImgFitMaxZoom;				//�t�B�b�g�\�����̋��e�{��
int  ImgGridHorzN;				//�O���b�h�̉�������
int  ImgGridVertN;				//�O���b�h�̏c������
int  MinShowTime;				//�ŏ��\���^�C��
int  ThumbnailSize;				//�T���l�C���̃T�C�Y
int  ThumbBdrWidth;				//�T���l�C���̋��E����
int  ThumbnailPos;				//�T���l�C���̕\���ʒu
int  ImgDblMargin;				//���J���Ԋu

bool ShowImgSidebar;			//�T�C�h�o�[��\��
bool ImgSidebarIsLeft;			//�T�C�h�o�[�͍�

bool HasCallHotkey;				//�Ăяo���z�b�g�L�[�����邩�H

bool HideSizeTime;				//�T�C�Y�ƍX�V�������B��

//---------------------------------------------------------------------------
//�C�x���g�������Ɏ��s����R�}���h
UnicodeString OnAppStart;
UnicodeString OnAppClose;
UnicodeString OnMaximized;
UnicodeString OnMinimized;
UnicodeString OnRestored;
UnicodeString OnCurChange;
UnicodeString OnTabChange;
UnicodeString OnNewDrive;
UnicodeString OnArcOpend;
UnicodeString OnArcClosed;
UnicodeString OnFindOpend;
UnicodeString OnGrepOpen;
UnicodeString OnGrepClosed;
UnicodeString OnWatchNotify;
UnicodeString OnFTPConnect;
UnicodeString OnFTPDiscon;
UnicodeString OnFlCursor;
UnicodeString OnFlExDClick;
UnicodeString OnFlRClick;
UnicodeString OnDragStart;
UnicodeString OnDragEnd;
UnicodeString OnFlTbRClick;
UnicodeString OnTabDClick;
UnicodeString OnTabRClick;
UnicodeString OnDirDClick;
UnicodeString OnDirRClick;
UnicodeString OnRelDClick;
UnicodeString OnRelRClick;
UnicodeString OnHdrRClick;
UnicodeString OnFScrRClick;
UnicodeString OnDrvDClick;
UnicodeString OnDrvRClick;
UnicodeString OnSplDClick;
UnicodeString OnSplRClick;
UnicodeString OnImgDClick;
UnicodeString OnTxtDClick;
UnicodeString OnFlIDClick;
UnicodeString OnTskDClick;
UnicodeString OnTskRClick;
UnicodeString OnLogDClick;
UnicodeString OnSttDClick;
UnicodeString OnSttRClick;
UnicodeString OnTimDClick;
UnicodeString OnTimRClick;
UnicodeString OnTvOpen;
UnicodeString OnTvOpened;
UnicodeString OnTvClose;
UnicodeString OnTvClosed;
UnicodeString OnTvTbRClick;
UnicodeString OnTvHRClick;
UnicodeString OnIvOpened;
UnicodeString OnIvClosed;
UnicodeString OnFullScr;
UnicodeString OnNormScr;
UnicodeString OnIvTbRClick;
UnicodeString OnIvImgDClick;
UnicodeString OnIvImgRClick;
UnicodeString OnIvMgnDClick;
UnicodeString OnThmDClick;
UnicodeString OnThmRClick;
UnicodeString OnIvIDClick;
UnicodeString OnIvSbRClick;
UnicodeString OnDragStartI;
UnicodeString OnDragEndI;

const event_rec EventCmdList[MAX_EVENT_CMD] = {
	{&OnAppStart,		_T("OnAppStart"),	_T("NyanFi �N����")},
	{&OnAppClose,		_T("OnAppClose"),	_T("NyanFi �I����")},
	{&OnMaximized,		_T("OnMaximized"),	_T("�E�B���h�E���ő剻���ꂽ��")},
	{&OnMinimized,		_T("OnMinimized"),	_T("�E�B���h�E���ŏ������ꂽ��")},
	{&OnRestored,		_T("OnRestored"),	_T("�E�B���h�E�����̃T�C�Y�ɖ߂�����")},
	{&OnCurChange,		_T("OnCurChange"),	_T("|�J�����g�̃f�B���N�g���ύX(FL)")},
	{&OnTabChange,		_T("OnTabChange"),	_T("�ʂ̃^�u�Ɉڂ�����(FL)")},
	{&OnNewDrive,		_T("OnNewDrive"),	_T("�ǉ��h���C�u���J���ꂽ��(FL)")},
	{&OnArcOpend,		_T("OnArcOpend"),	_T("���z�f�B���N�g�����J��������(FL)")},
	{&OnArcClosed,		_T("OnArcClosed"),	_T("���z�f�B���N�g�����������(FL)")},
	{&OnFindOpend,		_T("OnFindOpend"),	_T("�������ʃ��X�g���\�����ꂽ����(FL)")},
	{&OnFlCursor,		_T("OnFlCursor"),	_T("�t�@�C�����X�g�ŃJ�[�\���ړ�����(FL)")},
	{&OnFlExDClick,		_T("OnFlExDClick"),	_T("�t�@�C�����X�g�Ńt�@�C�����啔�ȊO���_�u���N���b�N(FL)")},
	{&OnFlRClick,		_T("OnFlRClick"),	_T("�t�@�C�����X�g�ŉE�N���b�N(FL)")},
	{&OnDragStart,		_T("OnDragStart"),	_T("�t�@�C�����X�g����̃h���b�O�J�n��(FL)")},
	{&OnDragEnd,		_T("OnDragEnd"),	_T("�t�@�C�����X�g����̃h���b�O������(FL)")},
	{&OnGrepOpen,		_T("OnGrepOpen"),	_T("�����񌟍�(GREP)���J�����O(FL,TV)")},
	{&OnGrepClosed,		_T("OnGrepClosed"),	_T("�����񌟍�(GREP)���������(FL)")},
	{&OnWatchNotify,	_T("OnWatchNotify"),_T("WatchTail�R�}���h�ŒǋL���o")},
	{&OnFTPConnect,		_T("OnFTPConnect"),	_T("|FTP�z�X�g�ɐڑ�����(FL)")},
	{&OnFTPDiscon,		_T("OnFTPDiscon"),	_T("FTP�z�X�g����ؒf����(FL)")},
	{&OnFlTbRClick,		_T("OnFlTbRClick"),	_T("|�t�@�C���[�̃c�[���o�[���E�N���b�N(FL)")},
	{&OnTabDClick,		_T("OnTabDClick"),	_T("�^�u�o�[�̋󂫃X�y�[�X���_�u���N���b�N(FL)")},
	{&OnTabRClick,		_T("OnTabRClick"),	_T("�^�u�o�[���E�N���b�N(FL)")},
	{&OnDirDClick,		_T("OnDirDClick"),	_T("�f�B���N�g�������_�u���N���b�N(FL)")},
	{&OnDirRClick,		_T("OnDirRClick"),	_T("�f�B���N�g�������E�N���b�N(FL)")},
	{&OnRelDClick,		_T("OnRelDClick"),	_T("�f�B���N�g���֌W���_�u���N���b�N(FL)")},
	{&OnRelRClick,		_T("OnRelRClick"),	_T("�f�B���N�g���֌W���E�N���b�N(FL)")},
	{&OnHdrRClick,		_T("OnHdrRClick"),	_T("�t�@�C�����X�g�̃w�b�_���E�N���b�N(FL)")},
	{&OnFScrRClick,		_T("OnFScrRClick"),	_T("�t�@�C�����X�g�̃V���v���X�N���[���o�[���E�N���b�N(FL)")},
	{&OnDrvDClick,		_T("OnDrvDClick"),	_T("�h���C�u�����_�u���N���b�N(FL)")},
	{&OnDrvRClick,		_T("OnDrvRClick"),	_T("�h���C�u�����E�N���b�N(FL)")},
	{&OnSplDClick,		_T("OnSplDClick"),	_T("���E�������E�����_�u���N���b�N(FL)")},
	{&OnSplRClick,		_T("OnSplRClick"),	_T("���E�������E�����E�N���b�N(FL)")},
	{&OnImgDClick,		_T("OnImgDClick"),	_T("�C���[�W�v���r���[���_�u���N���b�N(FL)")},
	{&OnTxtDClick,		_T("OnTxtDClick"),	_T("�e�L�X�g�v���r���[���_�u���N���b�N(FL)")},
	{&OnFlIDClick,		_T("OnFlIDClick"),	_T("�t�@�C�������_�u���N���b�N(FL)")},
	{&OnTskDClick,		_T("OnTskDClick"),	_T("�^�X�N�\�����_�u���N���b�N")},
	{&OnTskRClick,		_T("OnTskRClick"),	_T("�^�X�N�\�����E�N���b�N")},
	{&OnLogDClick,		_T("OnLogDClick"),	_T("���O�E�B���h�E���_�u���N���b�N")},
	{&OnSttDClick,		_T("OnSttDClick"),	_T("�X�e�[�^�X�o�[���_�u���N���b�N")},
	{&OnSttRClick,		_T("OnSttRClick"),	_T("�X�e�[�^�X�o�[���E�N���b�N")},
	{&OnTimDClick,		_T("OnTimDClick"),	_T("���v�p�l�����_�u���N���b�N")},
	{&OnTimRClick,		_T("OnTimRClick"),	_T("���v�p�l�����E�N���b�N")},
	{&OnTvOpen,			_T("OnTvOpen"),		_T("|�e�L�X�g�r���A�[���J�����O(FL)")},
	{&OnTvOpened,		_T("OnTvOpened"),	_T("�e�L�X�g�r���A�[���J��������(TV)")},
	{&OnTvClose,		_T("OnTvClose"),	_T("�e�L�X�g�r���A�[����钼�O(TV)")},
	{&OnTvClosed,		_T("OnTvClosed"),	_T("�e�L�X�g�r���A�[���������(FL)")},
	{&OnTvTbRClick,		_T("OnTvTbRClick"),	_T("�e�L�X�g�r���A�[�̃c�[���o�[���E�N���b�N(TV)")},
	{&OnTvHRClick,		_T("OnTvHRClick"),	_T("�e�L�X�g�r���A�[�ŏ��w�b�_���E�N���b�N(TV)")},
	{&OnIvOpened,		_T("OnIvOpened"),	_T("|�C���[�W�r���A�[���J��������(IV)")},
	{&OnIvClosed,		_T("OnIvClosed"),	_T("�C���[�W�r���A�[���������(FL)")},
	{&OnFullScr,		_T("OnFullScr"),	_T("�C���[�W�r���A�[�őS��ʕ\���ɂ�����(IV)")},
	{&OnNormScr,		_T("OnNormScr"),	_T("�C���[�W�r���A�[�Œʏ�\���ɖ߂�����(IV)")},
	{&OnIvTbRClick,		_T("OnIvTbRClick"),	_T("�C���[�W�r���A�[�̃c�[���o�[���E�N���b�N(IV)")},
	{&OnIvImgDClick,	_T("OnIvImgDClick"),_T("�C���[�W�r���A�[�ŉ摜�������_�u���N���b�N(IV)")},
	{&OnIvImgRClick,	_T("OnIvImgRClick"),_T("�C���[�W�r���A�[�ŉ摜�\�������E�N���b�N(IV)")},
	{&OnIvMgnDClick,	_T("OnIvMgnDClick"),_T("�C���[�W�r���A�[�ŗ]���������_�u���N���b�N(IV)")},
	{&OnThmDClick,		_T("OnThmDClick"),	_T("�C���[�W�r���A�[�ŃT���l�C�����_�u���N���b�N(IV)")},
	{&OnThmRClick,		_T("OnThmRClick"),	_T("�C���[�W�r���A�[�ŃT���l�C�����E�N���b�N(IV)")},
	{&OnIvIDClick,		_T("OnIvIDClick"),	_T("�C���[�W�r���A�[�Ńt�@�C�������_�u���N���b�N(IV)")},
	{&OnIvSbRClick,		_T("OnIvSbRClick"),	_T("�C���[�W�r���A�[�ŃV�[�N�o�[���E�N���b�N(IV)")},
	{&OnDragStartI,		_T("OnDragStartI"),	_T("�T���l�C������̃h���b�O�J�n��(IV)")},
	{&OnDragEndI,		_T("OnDragEndI"),	_T("�T���l�C������̃h���b�O������(IV)")}
};

bool Timer_Enabled[MAX_TIMER_EVENT], Timer_NopAct[MAX_TIMER_EVENT];
int  Timer_StartCnt[MAX_TIMER_EVENT], Timer_TimeCnt[MAX_TIMER_EVENT], Timer_NopCnt[MAX_TIMER_EVENT];
int  Timer_RepeatN[MAX_TIMER_EVENT], Timer_RepeatCnt[MAX_TIMER_EVENT];
TStringList  *Timer_AlarmList[MAX_TIMER_EVENT];
UnicodeString Timer_Condition[MAX_TIMER_EVENT];
UnicodeString OnTimerEvent[MAX_TIMER_EVENT];

//---------------------------------------------------------------------------
//TStringList ���쐬���AGeneralList �ɓo�^(�I�����Ɉꊇ�j��)
//---------------------------------------------------------------------------
TStringList *CreStringList(int obj_type)
{
	TStringList *lp = new TStringList();
	GeneralList->AddObject(IntToStr(obj_type), (TObject*)lp);
	return lp;
}

//---------------------------------------------------------------------------
//������
//---------------------------------------------------------------------------
void InitializeGlobal()
{
	Randomize();

	UserModule->ExePath = ExePath;

	LibraryPath = cv_env_str("%APPDATA%\\Microsoft\\Windows\\Libraries\\");

	//�o�[�W����
	unsigned mj, mi, bl;
	VersionNo = GetProductVersion(Application->ExeName, mj, mi, bl)? mj*100 + mi*10 + bl : 0;
	VersionStr.sprintf(_T("V%.2f"), VersionNo/100.0);
	CopyrightStr.USET_T(COPYRIGHT_INF);

	ScrScale = Screen->PixelsPerInch / 96.0;	//�X�P�[�����O��ݒ�
	SIcoSize = ScaledInt(16);

	//���[�U��
	_TCHAR szName[256];
	DWORD size = 256;
	if (::GetUserName(szName, &size)) UserName = szName;

	//�Ǘ��Ҍ����������Ă��邩?
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID admin_group;
	BOOL res = ::AllocateAndInitializeSid(
					&NtAuthority, 2,
					SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
					&admin_group);
	if (res) {
		if (!CheckTokenMembership(NULL, admin_group, &res)) res = FALSE;
		::FreeSid(admin_group);
	}
	IsAdmin = res;

	WIC_get_ex_list(NULL, &WicFextStr);

	mute_Volume("GET");	//�~���[�g��Ԃ��擾

	//�p�~�Z�N�V�����A�L�[�̍폜�A�C��
	IniFile->DeleteKey(SCT_General,	"FindPathCol");							//v11.60
	IniFile->DeleteKey(SCT_General,	"FindPathWidth");
	IniFile->DeleteKey(SCT_General,	"FindTagPathCol");
	IniFile->DeleteKey(SCT_General,	"FindTagPathWidth");
	IniFile->ReplaceKey("Event",	"OnImgRClick",	"OnIvImgRClick");		//V11.54
	IniFile->DeleteKey(SCT_Option,	"SaveWorkListUtf8");					//V11.41
	UnicodeString sct_grep = "Grep";
	IniFile->ReplaceKey(sct_grep,	"BakupReplace", 	"BackupReplace");	//V11.27
	IniFile->ReplaceKey(sct_grep,	"RepBakupDir", 		"RepBackupDir");
	IniFile->ReplaceKey(sct_grep,	"FExtRepBakup", 	"FExtRepBackup");
	IniFile->ReplaceKey(sct_grep,	"GrepFileFmt", 		"FileFmt");			//V11.05
	IniFile->ReplaceKey(sct_grep,	"GrepInsStrW", 		"InsStrW");
	IniFile->ReplaceKey(sct_grep,	"GrepInsStrW2",		"InsStrW2");
	IniFile->ReplaceKey(sct_grep,	"GrepShowItemNo",	"ShowItemNo");
	IniFile->ReplaceKey(sct_grep,	"GrepFileItemNo",	"FileItemNo");
	IniFile->ReplaceKey(sct_grep,	"GrepTrimTop",		"TrimTop");
	IniFile->ReplaceKey(sct_grep,	"GrepOmitTop",		"OmitTop");
	IniFile->ReplaceKey(sct_grep,	"GrepEmFilter",		"EmFilter");
	IniFile->DeleteKey(SCT_General,	"RegExResHeight");						//V10.99
	IniFile->DeleteKey(SCT_General,	"RegExRefWidth");						//V10.99
	IniFile->DeleteKey(SCT_General,	"RegExChkMltLn");						//V10.90
	IniFile->DeleteKey(SCT_General,	"FuncListHeadline");					//V10.81

	CurStt = &ListStt[CurListTag];
	OppStt = &ListStt[OppListTag];

	for (int i=0; i<MAX_BGIMAGE; i++) BgImgBuff[i] = new Graphics::TBitmap();

	GeneralList = new TStringList();

	for (int i=0; i<MAX_FILELIST; i++) {
		FileList[i] 	 = CreStringList(GENLST_FILELIST);
		ArcFileList[i]	 = CreStringList(GENLST_FILELIST);
		ResultList[i]	 = CreStringList(GENLST_FILELIST);
		SelMaskList[i]	 = CreStringList();

		IniPathMode[i]	 = 0;
		SortMode[i] 	 = 1;
		DirSortMode[i] 	 = 0;

		flist_stt *sp	 = &ListStt[i];
		sp->clu_size	 = 0;
		sp->drive_Used	 = 0;
		sp->drive_Free	 = 0;
		sp->drive_Total  = 0;
		sp->cur_total	 = -1;
		sp->occ_total	 = -1;
		sp->f_cnt		 = 0;
		sp->d_cnt		 = 0;
		sp->sel_f_cnt	 = 0;
		sp->sel_d_cnt	 = 0;
		sp->sel_size	 = 0;

		sp->show_f_d_cnt = false;
		sp->dir_graph	 = false;
		sp->dsk_graph	 = false;
		sp->sub_counted	 = false;

		sp->lwd_half	 = 8;
		sp->lwd_fext_max = 32;
		sp->lwd_fext	 = 32;
		sp->lwd_size	 = 100;
		sp->lwd_time	 = 100;
		sp->lwd_path	 = 0;

		sp->lxp_fext	 = 0;
		sp->lxp_size	 = 0;
		sp->lxp_time	 = 0;

		sp->color_bgDirInf = Graphics::clNone;
		sp->color_fgDirInf = Graphics::clNone;
		sp->color_bgDrvInf = Graphics::clNone;
		sp->color_fgDrvInf = Graphics::clNone;

		sp->is_Work 	 = false;
		sp->is_FTP		 = false;
		sp->is_Arc		 = false;
		sp->arc_RetList  = CreStringList();
		sp->arc_TmpList  = CreStringList();

		sp->is_Find 	 = false;
		sp->is_narrow	 = false;
		sp->find_dir	 = false;
		sp->find_both	 = false;
		sp->find_sub	 = false;
		sp->find_arc	 = false;
		sp->find_mark	 = false;
		sp->find_tag	 = false;
		sp->find_tag	 = false;
		sp->find_tag	 = false;
		sp->find_hlink	 = false;

		sp->find_SubList = CreStringList();
		sp->find_RegEx	 = false;
		sp->find_And	 = false;
		sp->find_Case	 = false;
		sp->find_ResLink = false;
		sp->find_DirLink = false;
		sp->find_PathSort= false;

		sp->is_IncSea	 = false;
		sp->is_Migemo	 = false;
		sp->is_Filter	 = false;
		sp->filter_sens  = false;
	}

	for (int i=0; i<MAX_TIMER_EVENT; i++) {
		Timer_Enabled[i]   = false;
		Timer_AlarmList[i] = CreStringList();
	}

	TmpBufList		  = CreStringList(GENLST_FILELIST);
	ViewFileList	  = CreStringList(GENLST_FILELIST);
	WorkList		  = CreStringList(GENLST_FILELIST);
	LaunchList		  = CreStringList(GENLST_FILELIST);
	GeneralIconList	  = CreStringList(GENLST_ICON);
	CachedIcoList	  = CreStringList(GENLST_ICON);
	MenuBtnIcoList	  = CreStringList(GENLST_ICON);
	UsrIcoList		  = CreStringList(GENLST_ICON);
	DrvIcoList		  = CreStringList(GENLST_ICON);
	FontList		  = CreStringList(GENLST_FONT);
	CmdFileList 	  = CreStringList(GENLST_CMDSLIST);
	TabList 		  = CreStringList(GENLST_TABLIST);
	DriveInfoList	  = CreStringList(GENLST_DRIVE);
	DriveLogList	  = CreStringList();
	WatchTailList	  = CreStringList();
	InvalidUncList	  = CreStringList();
	PlayList		  = CreStringList();
	XCMD_VarList	  = CreStringList();
	BakSetupList	  = CreStringList();
	SyncDirList 	  = CreStringList();
	AssRenList		  = CreStringList();
	DistrDefList	  = CreStringList();
	GrepPathList	  = CreStringList();
	GrepFileList	  = CreStringList();
	GrepResultList	  = CreStringList();
	GrepResultBuff	  = CreStringList();
	KeyFuncList 	  = CreStringList();
	FKeyLabelList	  = CreStringList();
	HotKeyList		  = CreStringList();
	DirStack		  = CreStringList();
	RegDirList		  = CreStringList();
	ProtectDirList	  = CreStringList();
	PathMaskList	  = CreStringList();
	ColorList		  = CreStringList();
	ExtColList		  = CreStringList();
	AssociateList	  = CreStringList();
	OpenStdCmdList	  = CreStringList();
	EtcEditorList	  = CreStringList();
	ExtToolList 	  = CreStringList();
	ExtMenuList 	  = CreStringList();
	DriveList		  = CreStringList();
	NetDriveList	  = CreStringList();
	AllDirHistory	  = CreStringList();
	TextViewHistory   = CreStringList();
	TextEditHistory   = CreStringList();
	WorkListHistory   = CreStringList();
	InputDirHistory   = CreStringList();
	InputCmdsHistory  = CreStringList();
	InputCmdsHistoryV = CreStringList();
	InputCmdsHistoryI = CreStringList();
	IncSeaHistory	  = CreStringList();
	FilterHistory	  = CreStringList();
	WebSeaHistory	  = CreStringList();
	LatLngHistory	  = CreStringList();
	HeadlineList	  = CreStringList();
	RenCmdFileList	  = CreStringList();
	RedrawList		  = CreStringList();
	CmdRequestList	  = CreStringList();
	PopMenuList 	  = CreStringList();
	ToolBtnList 	  = CreStringList();
	ToolBtnListV	  = CreStringList();
	ToolBtnListI	  = CreStringList();
	CnvCharList 	  = CreStringList();
	LogBufList		  = CreStringList();

	//�R�}���h���X�g���쐬
	CommandList = CreStringList();
	CommandList->Text =
		"F:AboutNyanFi=�o�[�W�������\n"
		"F:AddTab=�^�u��ǉ�\n"
		"F:BackDirHist=�f�B���N�g��������߂�\n"
		"F:Backup=���΃p�X�Ƀo�b�N�A�b�v\n"
		"F:BgImgMode=�w�i�摜�̕\�����[�h�ݒ�\n"
		"F:BorderLeft=�t�@�C�����X�g�̋��E�����Ɉړ�\n"
		"F:BorderRight=�t�@�C�����X�g�̋��E���E�Ɉړ�\n"
		"F:CalcDirSize=�f�B���N�g���e�ʂ��v�Z\n"
		"F:CalcDirSizeAll=�S�f�B���N�g���̗e�ʂ��v�Z\n"
		"F:ChangeDir=�f�B���N�g����ύX\n"
		"F:ChangeDrive=�h���C�u�̕ύX\n"
		"F:ChangeOppDir=���Α��̃f�B���N�g����ύX\n"
		"F:ChangeOppRegDir=���Α��œo�^�f�B���N�g�����J��\n"
		"F:ChangeRegDir=�o�^�f�B���N�g�����J��\n"
		"F:CheckUpdate=�X�V�̊m�F\n"
		"F:ClearMask=�I���}�X�N/�p�X�}�X�N������\n"
		"F:Clone=���΃p�X�ɃN���[���쐬\n"
		"F:CloneToCurr=�J�����g�ɃN���[���쐬\n"
		"F:CommandPrompt=�R�}���h�v�����v�g\n"
		"F:CompareDlg=�����t�@�C���̔�r�_�C�A���O\n"
		"F:CompareHash=�n�b�V���l�̔�r\n"
		"F:CompleteDelete=���S�폜\n"
		"F:ContextMenu=�R���e�L�X�g���j���[��\��\n"
		"F:ConvertDoc2Txt=�o�C�i���������e�L�X�g�ϊ�\n"
		"F:ConvertHtm2Txt=HTML���e�L�X�g�ϊ�\n"
		"F:ConvertImage=�摜�t�@�C���̕ϊ�\n"
		"F:ConvertTextEnc=�����R�[�h�̕ϊ�\n"
		"F:Copy=�R�s�[\n"
		"F:CopyCmdName=�R�}���h�����N���b�v�{�[�h�ɃR�s�[\n"
		"F:CopyDir=�f�B���N�g���\���̃R�s�[\n"
		"F:CopyFileName=�t�@�C�������N���b�v�{�[�h�ɃR�s�[\n"
		"F:CopyTo=�w��f�B���N�g���ɃR�s�[\n"
		"F:CopyToClip=�N���b�v�{�[�h�ɃR�s�[\n"
		"F:CountLines=�e�L�X�g�t�@�C���̍s�����J�E���g\n"
		"F:CreateDir=�f�B���N�g���̍쐬\n"
		"F:CreateDirsDlg=�f�B���N�g���ꊇ�쐬�_�C�A���O\n"
		"F:CreateHardLink=�n�[�h�����N�̍쐬\n" 
		"F:CreateShortcut=�V���[�g�J�b�g�̍쐬\n"
		"F:CreateTestFile=�e�X�g�t�@�C���̍쐬\n"
		"F:CsrDirToOpp=�J�[�\���ʒu�̃f�B���N�g���𔽑Α��ɊJ��\n"
		"F:CurrFromOpp=�J�����g�p�X�𔽑΃p�X��\n"
		"F:CurrToOpp=���΃p�X���J�����g�p�X��\n"
		"F:CursorEnd=�J�[�\�����ŉ��s�Ɉړ�\n"
		"F:CursorEndSel=�I�����Ȃ���J�[�\�����ŉ��s�Ɉړ�\n"
		"F:CursorTop=�J�[�\�����ŏ�s�Ɉړ�\n"
		"F:CursorTopSel=�I�����Ȃ���J�[�\�����ŏ�s�Ɉړ�\n"
		"F:CutToClip=�N���b�v�{�[�h�ɐ؂���\n"
		"F:DateSelect=�w�肵�����t�����ɍ����t�@�C����I��\n"
		"F:DelJpgExif=Exif���̍폜\n"
		"F:DelSelMask=�I���}�X�N���獀�ڂ��폜\n"
		"F:DelTab=�^�u���폜\n"
		"F:DirHistory=�f�B���N�g��������\��\n"
		"F:DirStack=�f�B���N�g���E�X�^�b�N��\��\n"
		"F:DistributionDlg=�U�蕪���_�C�A���O\n"
		"F:DotNyanDlg=.nyanfi �t�@�C���̐ݒ�\n"
		"F:DriveList=�h���C�u�ꗗ��\��\n"
		"F:DriveGraph=�h���C�u�g�p�����ڂ�\��\n"
		"F:EditIniFile=INI�t�@�C���̕ҏW\n"
		"F:Eject=CD/DVD�h���C�u�̃g���C���J��\n"
		"F:EjectDrive=�h���C�u�̎��O��\n"
		"F:EmptyTrash=���ݔ�����ɂ���\n"
		"F:EqualListWidth=���E�̃��X�g�����ϓ���\n"
		"F:ExeCommandLine=�R�}���h���C���̎��s\n"
		"F:ExeExtMenu=�ǉ����j���[�̎��s\n"
		"F:ExeExtTool=�O���c�[���̎��s\n"
		"F:Exit=NyanFi�̏I��\n"
		"F:ExitDupl=��d�N�����ꂽNyanFi���I��\n"
		"F:ExPopupMenu=�g���|�b�v�A�b�v���j���[\n"
		"F:ExtractChmSrc=CHM����\�[�X�𒊏o\n"
		"F:ExtractGifBmp=�A�j��GIF����r�b�g�}�b�v�𒊏o\n"
		"F:ExtractIcon=�A�C�R���𒊏o\n"
		"F:ExtractMp3Img=MP3�̖��ߍ��݉摜�𒊏o\n"
		"F:FileExtList=�g���q�ʈꗗ\n"
		"F:FileListOnly=�t�@�C�����X�g�݂̂�\��\n"
		"F:FileRun=�t�@�C�������w�肵�Ď��s\n"
		"F:Filter=�t�@�C�����X�g�̍i�荞��\n"
		"F:FindDirDlg=�f�B���N�g���������_�C�A���O\n"
		"F:FindFileDirDlg=�t�@�C��/�f�B���N�g���������_�C�A���O\n"
		"F:FindFileDlg=�t�@�C���������_�C�A���O\n"
		"F:FindHardLink=�n�[�h�����N���\n"
		"F:FindMark=�}�[�N���ڂ�����\n"
		"F:FindTag=�^�O����\n"
		"F:FTPChmod=�p�[�~�b�V�����̐ݒ�\n"
		"F:FTPConnect=FTP�z�X�g�ɐڑ�\n"
		"F:FTPDisconnect=FTP�z�X�g����ؒf\n"
		"F:ForwardDirHist=�f�B���N�g��������i��\n"
		"F:GetHash=�t�@�C���̃n�b�V���l���擾\n"
		"F:HideSizeTime=�T�C�Y�ƍX�V�������B��\n"
		"F:ImageViewer=�C���[�W�r���A�[�ŊJ��\n"
		"F:InputDir=���͂����f�B���N�g���ɕύX\n"
		"F:InputPathMask=�p�X�}�X�N�����\n"
		"F:InsSeparator=���[�N���X�g�ɃZ�p���[�^��}��\n"
		"F:ItemTmpDown=���ڂ��ꎞ�I��1���Ɉړ�\n"
		"F:ItemTmpUp=���ڂ��ꎞ�I��1��Ɉړ�\n"
		"F:JoinText=�e�L�X�g�t�@�C���̌���\n"
		"F:JumpTo=�w�肵���t�@�C���ʒu��\n"
		"F:Library=���C�u������\n"
		"F:LinkToOpp=�����N��𔽑Α��ɊJ��\n"
		"F:ListArchive=�A�[�J�C�u�̓��e�ꗗ\n"
		"F:ListClipboard=�N���b�v�{�[�h���ꗗ�ŕ\��\n"
		"F:ListDuration=�t�@�C���Đ����Ԃ̈ꗗ\n"
		"F:ListExpFunc=�G�N�X�|�[�g�֐��ꗗ\n"
		"F:ListFileName=�t�@�C�������ꗗ�ŕ\��\n"
		"F:ListNyanFi=NyanFi �ɂ��Ă̏������O�ɕ\��\n"
		"F:ListText=�e�L�X�g�t�@�C�����ꗗ�ŕ\��\n"
		"F:ListTail=�e�L�X�g�t�@�C���̖������ꗗ�ŕ\��\n"
		"F:LockTextPreview=�e�L�X�g�v���r���[�����b�N/����\n"
		"F:LogFileInfo=�t�@�C���������O�ɏo��\n"
		"F:MarkMask=�}�[�N���ڂ������c���đ����B��\n"
		"F:MaskFind=�w��}�X�N�Ƀ}�b�`���鍀�ڂ�����\n"
		"F:MatchSelect=�w�蕶������܂ރt�@�C����I��\n"
		"F:MonitorOff=�f�B�X�v���C�̓d����؂�\n"
		"F:Move=�ړ�\n"
		"F:MoveTab=�^�u�̈ʒu���ړ�\n"
		"F:MoveTo=�w��f�B���N�g���ֈړ�\n"
		"F:MuteVolume=���ʃ~���[�g\n"
		"F:NameFromClip=�t�@�C�������N���b�v�{�[�h�̓��e��\n"
		"F:NameToLower=�t�@�C�����̏�������\n"
		"F:NameToUpper=�t�@�C�����̑啶����\n"
		"F:NetConnect=�l�b�g���[�N�h���C�u�̊��蓖��\n"
		"F:NetDisconnect=�l�b�g���[�N�h���C�u�̐ؒf\n"
		"F:NewTextFile=�V�K�e�L�X�g�t�@�C���̍쐬\n"
		"F:NextDrive=���̃h���C�u��\n"
		"F:NextTab=���̃^�u��\n"
		"F:OpenByExp=�G�N�X�v���[���ŊJ��\n"
		"F:OpenCtrlPanel=�R���g���[���p�l�����J��\n"
		"F:OpenStandard=�W���� Enter �L�[����\n"
		"F:OpenTrash=���ݔ����J��\n"
		"F:Pack=���΃p�X�ɃA�[�J�C�u�쐬\n"
		"F:PackToCurr=�J�����g�p�X�ɃA�[�J�C�u�쐬\n"
		"F:Paste=�\��t��\n"
		"F:PathMaskDlg=�p�X�}�X�N�_�C�A���O\n"
		"F:PlayList=�v���C���X�g������čĐ�\n"
		"F:PopDir=�f�B���N�g�����|�b�v\n"
		"F:PopupTab=�^�u�I�����j���[��\��\n"
		"F:PowerOff=Windows���I��\n"
		"F:PrevDrive=�O�̃h���C�u��\n"
		"F:PrevTab=�O�̃^�u��\n"
		"F:PushDir=�f�B���N�g�����v�b�V��\n"
		"F:Reboot=Windows���ċN��\n"
		"F:RegDirDlg=�o�^�f�B���N�g���_�C�A���O\n"
		"F:RegDirPopup=�o�^�f�B���N�g���E�|�b�v�A�b�v���j���[\n"
		"F:RegSyncDlg=�����R�s�[�̐ݒ�\n"
		"F:ReloadList=�t�@�C�����X�g���ŐV�̏��ɍX�V\n"
		"F:RenameDlg=���O���̕ύX\n"
		"F:Restart=NyanFi�̍ċN��\n"
		"F:ReturnList=�t�@�C�����X�g�\���ɖ߂�\n"
		"F:ScrollDownLog=���O�����ɃX�N���[��\n"
		"F:ScrollDownText=�e�L�X�g�v���r���[�����ɃX�N���[��\n"
		"F:ScrollUpLog=���O����ɃX�N���[��\n"
		"F:ScrollUpText=�e�L�X�g�v���r���[����ɃX�N���[��\n"
		"F:SelAllItem=���ׂĂ̍��ڂ�I��/����\n"
		"F:SelectUp=�I��/������A�J�[�\������Ɉړ�\n"
		"F:SelEmptyDir=��̃f�B���N�g��������I��\n"
		"F:SelMask=�I�𒆂̍��ڂ������c���đ����B��\n"
		"F:SelOnlyCur=�J�����g�������ɂ���t�@�C����I��\n"
		"F:SelReverseAll=���ׂĂ̍��ڂ̑I����Ԃ𔽓]\n"
		"F:SetAlias=���[�N���X�g�̍��ڂɃG�C���A�X��ݒ�\n"
		"F:SetArcTime=�A�[�J�C�u�̃^�C���X�^���v���ŐV�ɍ��킹��\n"
		"F:SetDirTime=�f�B���N�g���̃^�C���X�^���v���ŐV�ɍ��킹��\n"
		"F:SetExifTime=�^�C���X�^���v��Exif�B�e�����ɐݒ�\n"
		"F:SetPathMask=�p�X�}�X�N��ݒ�\n"
		"F:SetSttBarFmt=�X�e�[�^�X�o�[������ݒ�\n"
		"F:SetSubSize=�T�u�E�B���h�E�̃T�C�Y��ݒ�\n"
		"F:ShareList=���L�t�H���_�ꗗ\n"
		"F:ShowByteSize=�t�@�C���T�C�Y���o�C�g�P�ʂŕ\��\n"
		"F:ShowHideAtr=�B���t�@�C����\��\n"
		"F:ShowIcon=�A�C�R���̕\��/��\��\n"
		"F:ShowPreview=�C���[�W�v���r���[�̕\��/��\��\n"
		"F:ShowProperty=�t�@�C�����̕\��/��\��\n"
		"F:ShowSystemAtr=�V�X�e���t�@�C����\��\n"
		"F:ShowTabBar=�^�u�o�[�̕\��/��\��\n"
		"F:SoloTab=���̃^�u�����ׂč폜\n"
		"F:SubDirList=�T�u�f�B���N�g���ꗗ\n"
		"F:SwapLR=���E�̃t�@�C�����X�g�����ւ���\n"
		"F:SwapName=���O�����ւ���\n"
		"F:SyncLR=���E�f�B���N�g���̓����ύX��L��/����\n"
		"F:TabDlg=�^�u�̐ݒ�_�C�A���O\n"
		"F:TabHome=�^�u�̃z�[����\n"
		"F:TestArchive=�A�[�J�C�u�̐�����������\n"
		"F:TextViewer=�e�L�X�g�r���A�[�ŊJ��\n"
		"F:ToExViewer=�ʃE�B���h�E�̃e�L�X�g�r���A�[��\n"
		"F:ToLog=���O�E�B���h�E��\n"
		"F:ToNextOnRight=�E�t�@�C�����X�g�Ŏ���NyanFi��\n"
		"F:ToOpposite=���Α��̃t�@�C�����X�g��\n"
		"F:ToOppSameItem=�J�[�\���ʒu�Ɠ����̔��΃p�X���ڂ�\n"
		"F:ToParent=�e�f�B���N�g����\n"
		"F:ToParentOnLeft=���t�@�C�����X�g�Őe�f�B���N�g����\n"
		"F:ToParentOnRight=�E�t�@�C�����X�g�Őe�f�B���N�g����\n"
		"F:ToPrevOnLeft=���t�@�C�����X�g�őO��NyanFi��\n"
		"F:ToRoot=���[�g�f�B���N�g����\n"
		"F:ToTab=�w��ԍ�/�L���v�V�����̃^�u��\n"
		"F:ToText=�e�L�X�g�v���r���[��\n"
		"F:TrimTagData=�^�O�f�[�^�̐���\n"
		"F:UndoRename=���O�̉��������ɖ߂�\n"
		"F:UnPack=���΃p�X�ɉ�\n"
		"F:UnPackToCurr=�J�����g�p�X�ɉ�\n"
		"F:UpdateFromArc=�A�[�J�C�u����X�V\n"
		"F:ViewIniFile=INI�t�@�C���̉{��\n"
		"F:ViewTail=�e�L�X�g�t�@�C���̖������{��\n"
		"F:WatchTail=�e�L�X�g�t�@�C���̒ǉ��X�V���Ď�\n"
		"F:WidenCurList=�J�����g�����X�g�̕����L����\n"
		"F:WinMaximize=�E�B���h�E�̍ő剻\n"
		"F:WinMinimize=�E�B���h�E�̍ŏ���\n"
		"F:WinNormal=�E�B���h�E�����̃T�C�Y�ɖ߂�\n"
		"F:WorkItemDown=���[�N���X�g�̍��ڂ�1���Ɉړ�\n"
		"F:WorkItemUP=���[�N���X�g�̍��ڂ�1��Ɉړ�\n"
		"F:XmlViewer=XML�r���A�[\n"
		"FI:AddTag=���ڂɃ^�O��ǉ�\n"
		"FI:DelTag=���ڂ̃^�O���폜\n"
		"FI:HomeWorkList=�z�[�����[�N���X�g���J��\n"
		"FI:ListFileInfo=�t�@�C�������_�C�A���O�\��\n"
		"FI:LoadBgImage=�w�i�摜��ǂݍ���\n"
		"FI:LoadWorkList=���[�N���X�g���t�@�C������ǂݍ���\n"
		"FI:MaskSelect=�w��}�X�N�Ƀ}�b�`����t�@�C����I��\n"
		"FI:NewWorkList=�V�K���[�N���X�g�̍쐬\n"
		"FI:NextMark=���̃}�[�N���ڂ�\n"
		"FI:PrevMark=�O�̃}�[�N���ڂ�\n"
		"FI:NextSameName=�t�@�C�����啔���������t�@�C����\n"
		"FI:OpenByApp=�Ǝ��̊֘A�t���ŊJ��\n"
		"FI:OpenByWin=Windows�̊֘A�t���ŊJ��\n"
		"FI:SaveAsWorkList=���[�N���X�g�ɖ��O��t���ĕۑ�\n"
		"FI:SaveWorkList=���[�N���X�g���㏑���ۑ�\n"
		"FI:SelAllFile=���ׂẴt�@�C����I��/����\n"
		"FI:SelMark=�}�[�N���ڂ�I��\n"
		"FI:SelReverse=�t�@�C���̑I����Ԃ𔽓]\n"
		"FI:SelSameExt=�g���q�������t�@�C����I��\n"
		"FI:SelSameName=�t�@�C�����啔�������t�@�C����I��\n"
		"FI:SelWorkItem=���[�N���X�g�̓o�^���ڂ�I��\n"
		"FI:SetTag=���ڂɃ^�O��ݒ�\n"
		"FI:ShowFileInfo=�t�@�C�����������I�ɕ\��\n"
		"FI:SortDlg=�\�[�g�_�C�A���O\n"
		"FI:SubViewer=�T�u�r���A�[�̕\��/��\��\n"
		"FI:UseTrash=�폜�ɂ��ݔ����g��/�g��Ȃ�\n"
		"FI:WorkList=���[�N���X�g\n"
		"FL:CancelAllTask=���ׂẴ^�X�N�𒆒f\n"
		"FL:ClearLog=���O���N���A\n"
		"FL:PauseAllTask=���ׂẴ^�X�N����U��~/�ĊJ\n"
		"FL:Suspend=�^�X�N�\�񍀖ڂ̎��s��ۗ�/����\n"
		"FL:TaskMan=�^�X�N�}�l�[�W��\n"
		"FL:ToLeft=���t�@�C�����X�g��\n"
		"FL:ToRight=�E�t�@�C�����X�g��\n"
		"FL:ViewLog=���O���e�L�X�g�r���A�[�ŕ\��\n"
		"FS:ClearAll=���ׂĂ̑I����Ԃ�����\n"
		"FSI:Select=�I��/����\n"
		"FSVIL:HelpContents=�w���v�̖ڎ�/������\��\n"
		"FSVIL:KeyList=�L�[���蓖�Ĉꗗ��\��\n"
		"FSVIL:OptionDlg=�I�v�V�����ݒ�\n"
		"FV:BinaryEdit=�t�@�C���̃o�C�i���ҏW\n"
		"FV:CursorDownSel=�I�����Ȃ���J�[�\�������Ɉړ�\n"
		"FV:CursorUpSel=�I�����Ȃ���J�[�\������Ɉړ�\n"
		"FV:EditHighlight=�\�������\����`�t�@�C���̕ҏW\n"
		"FV:EditHistory=�ŋߕҏW�����t�@�C���ꗗ��\��\n"
		"FV:Grep=�����񌟍�(GREP)\n"
		"FV:HtmlToText=�e�L�X�g�r���A�[��HTML���e�L�X�g�ϊ��\��\n"
		"FV:IncSearch=�C���N�������^���T�[�`\n"
		"FV:FindTagName=tags ����^�O������\n"
		"FV:FixedLen=�e�L�X�g�r���A�[��CSV/TSV���Œ蒷�\��\n"
		"FV:PageDownSel=�I�����Ȃ���1�y�[�W���Ɉړ�\n"
		"FV:PageUpSel=�I�����Ȃ���1�y�[�W��Ɉړ�\n"
		"FV:RegExChecker=���K�\���`�F�b�J�[\n"
		"FV:SetColor=�e�L�X�g�r���A�[�̔z�F\n"
		"FV:SetFontSize=�t�H���g�T�C�Y��ύX\n"
		"FV:SetMargin=�e�L�X�g�r���A�[�̍����]����ݒ�\n"
		"FV:SetTab=�e�L�X�g�r���A�[�̃^�u�X�g�b�v����ݒ�\n"
		"FV:SetWidth=�e�L�X�g�r���A�[�̐܂�Ԃ�����ݒ�\n"
		"FV:ShowCR=�e�L�X�g�r���A�[�ŉ��s��\��/��\��\n"
		"FV:ShowLineNo=�e�L�X�g�r���A�[�̍s�ԍ���\��/��\��\n"
		"FV:ShowRuby=�e�L�X�g�r���A�[�Ń��r��\��/��\��\n"
		"FV:ShowRuler=�e�L�X�g�r���A�[�̃��[����\��/��\��\n"
		"FV:ShowTAB=�e�L�X�g�r���A�[�Ń^�u��\��/��\��\n"
		"FV:ViewHistory=�ŋ߉{�������t�@�C���ꗗ��\��\n"
		"FV:WebSearch=Web�Ō���\n"
		"FV:ZoomReset=�Y�[��������\n"
		"FVL:CursorDown=�J�[�\�������Ɉړ�\n"
		"FVL:CursorUp=�J�[�\������Ɉړ�\n"
		"FVI:AlphaBlend=���C���E�B���h�E�𓧉ߕ\��\n"
		"FVI:AppList=�A�v���P�[�V�����ꗗ\n"
		"FVI:Calculator=�d��\n"
		"FVI:ClearMark=���ׂẴ}�[�N������\n"
		"FVI:Close=����\n"
		"FVI:CopyFileInfo=�t�@�C�������N���b�v�{�[�h�ɃR�s�[\n"
		"FVI:CmdFileList=�R�}���h�t�@�C���ꗗ\n"
		"FVI:DebugCmdFile=�R�}���h�t�@�C���̃f�o�b�O���s\n"
		"FVI:Delete=�폜\n"
		"FVI:Duplicate=NyanFi�̓�d�N��\n"
		"FVI:ExeCommands=�w�肵���R�}���h�����s\n"
		"FVI:ExeMenuFile=���j���[�t�@�C���̎��s\n"
		"FVI:ExeToolBtn=�c�[���{�^���̎��s\n"
		"FVI:FileEdit=�t�@�C���̕ҏW\n"
		"FVI:InputCommands=���͂����R�}���h�����s\n"
		"FVI:ListLog=���O���ꗗ�ŕ\��\n"
		"FVI:Mark=�}�[�N/����\n"
		"FVI:MarkList=�}�[�N�ꗗ��\��\n"
		"FVI:MenuBar=���j���[�o�[�̕\��/��\��\n"
		"FVI:NextNyanFi=����NyanFi���A�N�e�B�u��\n"
		"FVI:PopupMainMenu=���C�����j���[���|�b�v�A�b�v�\��\n"
		"FVI:PrevNyanFi=�O��NyanFi���A�N�e�B�u��\n"
		"FVI:PropertyDlg=�v���p�e�B�_�C�A���O��\��\n"
		"FVI:ScrollDown=���ɃX�N���[��\n"
		"FVI:ScrollUp=��ɃX�N���[��\n"
		"FVI:ShowFKeyBar=�t�@���N�V�����L�[�o�[�̕\��/��\��\n"
		"FVI:ShowStatusBar=�X�e�[�^�X�o�[�̕\��/��\��\n"
		"FVI:ShowToolBar=�c�[���o�[��\��\n"
		"FVI:ToolBarDlg=�c�[���o�[�̐ݒ�\n"
		"FVI:WebMap=�摜��GPS����w��n�_�̒n�}���J��\n"
		"FVI:WinPos=�E�B���h�E�̎l�ӂ�ݒ�\n"
		"FVIL:PageDown=1�y�[�W���Ɉړ�\n"
		"FVIL:PageUp=1�y�[�W��Ɉړ�\n"
		"FVI:ZoomIn=�Y�[���C��\n"
		"FVI:ZoomOut=�Y�[���A�E�g\n"
		"I:ColorPicker=�J���[�s�b�J�[\n"
		"I:DoublePage=���J���\��\n"
		"I:EndFile=�Ō�̃t�@�C���Ɉړ�\n"
		"I:EqualSize=���{�\��\n"
		"I:FittedSize=��ʃt�B�b�g�\��\n"
		"I:FlipHorz=���E���]\n"
		"I:FlipVert=�㉺���]\n"
		"I:FullScreen=�S��ʕ\��\n"
		"I:GifViewer=GIF�r���A�[�̕\��/��\��\n"
		"I:GrayScale=�O���[�X�P�[���\��\n"
		"I:Histogram=�q�X�g�O�����̕\��/��\��\n"
		"I:JumpIndex=�w�肵���C���f�b�N�X�Ɉړ�\n"
		"I:Loupe=���[�y�̕\��/��\��\n"
		"I:NextPage=���y�[�W�Ɉړ�\n"
		"I:PageBind=���J���\���̒Ԃ�������ݒ�\n"
		"I:PrevPage=�O�y�[�W�Ɉړ�\n"
		"I:Print=�摜�̈��\n"
		"I:RotateLeft=����90�x��]\n"
		"I:RotateRight=�E��90�x��]\n"
		"I:ScrollLeft=���ɃX�N���[��\n"
		"I:ScrollRight=�E�ɃX�N���[��\n"
		"I:SendToWorkList=���[�N���X�g�ɑ���\n"
		"I:ShowGrid=�摜�����O���b�h�̕\��/��\��\n"
		"I:ShowSeekBar=�V�[�N�o�[�̕\��/��\��\n"
		"I:Sidebar=�T�C�h�o�[�̕\��/��\��\n"
		"I:Thumbnail=�T���l�C���̕\��/��\��\n"
		"I:ThumbnailEx=�T���l�C���̑S�ʕ\��/�ʏ�\��\n"
		"I:TopFile=�擪�t�@�C���Ɉړ�\n"
		"I:WarnHighlight=����ьx��\n"
		"S:ClearIncKeyword=�L�[���[�h���N���A\n"
		"S:IncMatchSelect=�}�b�`���鍀�ڂ����ׂđI��\n"
		"S:IncSearchDown=�}�b�`���鍀�ڂ�����������\n"
		"S:IncSearchExit=�C���N�������^���T�[�`���甲����\n"
		"S:IncSearchUp=�}�b�`���鍀�ڂ����������\n"
		"S:KeywordHistory=�L�[���[�h�������Q��\n"
		"S:MigemoMode=Migomo���[�h�̐؂芷��\n"
		"S:NormalMode=�ʏ�̃T�[�`���[�h�ɖ߂�\n"
		"S:SelectDown=�I��/������A�������Ɍ���\n"
		"V:BackViewHist=�e�L�X�g�r���A�[�̗�����߂�\n"
		"V:BitmapView=�r�b�g�}�b�v�r���[�̕\��/��\��\n"
		"V:BoxSelMode=���`�I�����[�h�̊J�n/����\n"
		"V:ChangeCodePage=�����R�[�h�ύX\n"
		"V:ChangeViewMode=�e�L�X�g/�o�C�i���\���̐؂芷��\n"
		"V:CharInfo=�������̕\��/��\��\n"
		"V:CsvRecord=CSV/TSV���R�[�h�̕\��/��\��\n"
		"V:CursorLeft=�J�[�\�������Ɉړ�\n"
		"V:CursorLeftSel=�I�����Ȃ���J�[�\�������Ɉړ�\n"
		"V:CursorRight=�J�[�\�����E�Ɉړ�\n"
		"V:CursorRightSel=�I�����Ȃ���J�[�\�����E�Ɉړ�\n"
		"V:ExportCsv=CSV/TSV�G�N�X�|�[�g\n"
		"V:FindDown=�������ɍČ���\n"
		"V:FindLinkDown=�����N����������Ɍ���\n"
		"V:FindLinkUp=�����N���������Ɍ���\n"
		"V:FindMarkDown=�}�[�N���������Ɍ���\n"
		"V:FindMarkUp=�}�[�N��������Ɍ���\n"
		"V:FindSelDown=�I�𕶎�����������Ɍ���\n"
		"V:FindSelUp=�I�𕶎����������Ɍ���\n"
		"V:FindText=�����񌟍�\n"
		"V:FindUp=������ɍČ���\n"
		"V:FunctionList=�֐��ꗗ��\��\n"
		"V:HelpCurWord=�J�[�\���ʒu�̒P����w��w���v�Ō���\n"
		"V:Highlight=�}�b�`��������\��\n"
		"V:Inspector=�C���X�y�N�^�̕\��/��\��\n"
		"V:ImgPreview=�C���[�W�v���r���[\n"
		"V:JumpLine=�w��s�ԍ��Ɉړ�\n"
		"V:LineEnd=�s���Ɉړ�\n"
		"V:LineEndSel=�I�����Ȃ���s���Ɉړ�\n"
		"V:LineTop=�s���Ɉړ�\n"
		"V:LineTopSel=�I�����Ȃ���s���Ɉړ�\n"
		"V:OpenURL=URL���J��\n"
		"V:ReloadFile=�t�@�C���̍ēǂݍ���\n"
		"V:SaveDump=�_���v���X�g���t�@�C���ɕۑ�\n"
		"V:ScrollCursorDown=�X�N���[�����Ȃ��牺�Ɉړ�\n"
		"V:ScrollCursorUp=�X�N���[�����Ȃ����Ɉړ�\n"
		"V:SearchPair=�Ή����銇�ʂ�����\n"
		"V:SelCurWord=�J�[�\���ʒu�̒P���I��\n"
		"V:SelLine=�J�[�\���s��I��\n"
		"V:SelLineCR=�J�[�\���s�S�̂����s�P�ʂőI��\n"
		"V:SelectAll=���ׂđI��\n"
		"V:SelectFile=�\�����t�@�C����I��/����\n"
		"V:SelectMode=�I�����[�h�̊J�n/����\n"
		"V:SetTopAddr=�擪�A�h���X��ݒ�\n"
		"V:SetUserDefStr=���[�U��`�������ݒ�\n"
		"V:Sort=�e�L�X�g�S�̂����s�P�ʂŃ\�[�g\n"
		"V:SwitchSameName=�t�@�C�����啔���������̃t�@�C���ɐ؂芷��\n"
		"V:SwitchSrcHdr=�w�b�_/�\�[�X�t�@�C���̐؂芷��\n"
		"V:TagJump=�G�f�B�^�Ń^�O�W�����v\n"
		"V:TagJumpDirect=�G�f�B�^�Ń_�C���N�g�^�O�W�����v\n"
		"V:TagView=�r���A�[�Ń^�O�W�����v\n"
		"V:TagViewDirect=�r���A�[�Ń_�C���N�g�^�O�W�����v\n"
		"V:TextEnd=�Ō���Ɉړ�\n"
		"V:TextEndSel=�I�����Ȃ���Ō���Ɉړ�\n"
		"V:TextTop=�擪�Ɉړ�\n"
		"V:TextTopSel=�I�����Ȃ���擪�Ɉړ�\n"
		"V:UserDefList=���[�U��`������ꗗ��\��\n"
		"V:WordLeft=�O�̒P��Ɉړ�\n"
		"V:WordRight=���̒P��Ɉړ�\n"
		"VIL:ClipCopy=�N���b�v�{�[�h�ɃR�s�[\n"
		"VI:NextFile=���̃t�@�C����\��\n"
		"VI:PrevFile=�O�̃t�@�C����\��\n"
		"VL:NextErr=���̃G���[�ʒu��\n"
		"VL:PrevErr=�O�̃G���[�ʒu��\n"
		"InitialSearch=�������T�[�`\n";

	//�R�}���h�ꗗ���X�g��ݒ�
	CmdSetList = CreStringList();
	UnicodeString cmd, tmp;
	for (int i=0; i<CommandList->Count; i++) {
		UnicodeString lbuf = CommandList->Strings[i];
		if (!contains_s(lbuf, _T(':'))) continue;
		UnicodeString ct = split_tkn(lbuf, ':');
		CommandList->Strings[i] = lbuf;
		cmd = get_tkn(lbuf, '=');
		for (int j=1; j<=ct.Length(); j++)
			CmdSetList->Add(tmp.sprintf(_T("%s:%s"), ct.SubString(j, 1).c_str(), cmd.c_str()));
	}
	CmdSetList->Sort();

	//�Y�[���{�����X�g
	ZoomRatioList = CreStringList();
	ZoomRatioList->Text = "10\n25\n50\n75\n100\n150\n200\n300\n400\n";

	//�^�X�N�R�}���h�����X�g���쐬
	TaskCmdList = CreStringList();
	TaskCmdList->Text =
		"CPY=�R�s�[\n"
		"MOV=�ړ�\n"
		"DEL=�폜\n"
		"CPYDIR=�f�B���N�g���̃R�s�[\n"
		"CMPDEL=���S�폜\n"
		"CVIMG=�摜�ϊ�\n"
		"DLEXIF=Exif�폜\n"
		"EXTCHM=���o\n"
		"BACKUP=�o�b�N�A�b�v\n"
		"DTIME=�^�C���ݒ�\n";

	//----------------------------------------------
	//�I�v�V�����̃L�[�ƕϐ��Ƃ̑Ή���o�^���ēǍ�
	//----------------------------------------------
	OptionList = CreStringList();

	struct {
		const _TCHAR  *def;	//�L�[��=�f�t�H���g
		TObject *vp;		//�ϐ��ւ̃|�C���^
	} opt_def_list[] = {
		//[Option]
		//String (�f�t�H���g�l�����p���ň͂�Ŏw��)
		{_T("TextEditor=\"\""),						(TObject*)&TextEditor},
		{_T("TextEditorFrmt=\"$F\""),				(TObject*)&TextEditorFrmt},
		{_T("ExtTxViewer=\"\""),					(TObject*)&ExtTxViewer},
		{_T("ExtTxViewerFrmt=\"$F\""),				(TObject*)&ExtTxViewerFrmt},
		{_T("ImageEditor=\"\""),					(TObject*)&ImageEditor},
		{_T("FExtImgEidt=\".jpg.gif.png.bmp\""),	(TObject*)&FExtImgEidt},
		{_T("BinaryEditor=\"\""),					(TObject*)&BinaryEditor},
		{_T("FExtViewTab4=\".cpp.cxx.c.h\""),		(TObject*)&FExtViewTab4},
		{_T("SoundTaskFin=\"\""),					(TObject*)&SoundTaskFin},
		{_T("SoundFindFin=\"\""),					(TObject*)&SoundFindFin},
		{_T("SoundWarning=\"\""),	 				(TObject*)&SoundWarning},
		{_T("SoundWatch=\"\""),	 					(TObject*)&SoundWatch},
		{_T("EmpBinPtn1=\"\""),						(TObject*)&EmpBinPtn1},
		{_T("EmpBinPtn2=\"\""),						(TObject*)&EmpBinPtn2},
		{_T("EmpBinPtn3=\"\""),						(TObject*)&EmpBinPtn3},
		{_T("HtmInsHrCls=\"post\""),				(TObject*)&HtmInsHrCls},
		{_T("HtmHdrStr=\"��;��;��;��;��;��\""),		(TObject*)&HtmHdrStr},
		{_T("IniSeaShift=\"F:Ctrl+\""),				(TObject*)&IniSeaShift},
		{_T("AutoRenFmt=\"\\N_\\SN(1)\""),			(TObject*)&AutoRenFmt},
		{_T("FExtExeFile=\".com.exe.bat.cmd\""),	(TObject*)&FExtExeFile},
		{_T("CallHotKey=\"\""),						(TObject*)&CallHotKey},
		{_T("AppListHotKey=\"\""),					(TObject*)&AppListHotKey},
		{_T("AppListHotPrm=\"\""),					(TObject*)&AppListHotPrm},
		{_T("NoWatchPath=\"\""),					(TObject*)&NoWatchPath},
		{_T("FExtGetInf=\"\""),						(TObject*)&FExtGetInf},
		{_T("FExtNoInf=\"\""),						(TObject*)&FExtNoInf},
		{_T("NoInfPath=\"\""),						(TObject*)&NoInfPath},
		{_T("FExtImgPrv=\"\""),						(TObject*)&FExtImgPrv},
		{_T("FExtNoImgPrv=\"\""),					(TObject*)&FExtNoImgPrv},
		{_T("NoImgPrvPath=\"\""),					(TObject*)&NoImgPrvPath},
		{_T("FExtNoIView=\"\""),					(TObject*)&FExtNoIView},
		{_T("MarkImgPath=\"\""),					(TObject*)&MarkImgPath},
		{_T("MarkImgFExt=\".jpg\""),				(TObject*)&MarkImgFExt},
		{_T("MarkImgMemo=\"������\""),				(TObject*)&MarkImgMemo},
		{_T("FExt7zDll=\".arj.chm.msi.wim\""),		(TObject*)&FExt7zDll},
		{_T("SttBarFmt=\"$F\""),					(TObject*)&SttBarFmt},
		{_T("SttClockFmt=\"\""),					(TObject*)&SttClockFmt},
		{_T("DrvInfFmtR=\"$VN    $US Use    $FS Free($FR) \""),
													(TObject*)&DrvInfFmtR},
		{_T("DrvInfFmtS=\"$AD $SD Directories    $AF $SF Files Selected    $SZ \""),
													(TObject*)&DrvInfFmtS},
		{_T("DrvInfFmtN=\"$VN    $US Use    $FS Free($FR) \""),
													(TObject*)&DrvInfFmtN},
		{_T("WebSeaUrl=\"https://www.google.co.jp/search?q=\\S&ie=UTF-8\""),
													(TObject*)&WebSeaUrl},
		{_T("DlgMoveShift=\"Alt+\""),				(TObject*)&DlgMoveShift},
		{_T("DlgSizeShift=\"\""),					(TObject*)&DlgSizeShift},
		{_T("WheelCmdF0=\"ScrollUp/ScrollDown\""),	(TObject*)&WheelCmdF[0]},
		{_T("WheelCmdF1=\"ZoomIn/ZoomOut\""),		(TObject*)&WheelCmdF[1]},
		{_T("WheelCmdF2=\"\""),						(TObject*)&WheelCmdF[2]},
		{_T("WheelCmdF3=\"\""),						(TObject*)&WheelCmdF[3]},
		{_T("WheelCmdV0=\"ScrollUp/ScrollDown\""),	(TObject*)&WheelCmdV[0]},
		{_T("WheelCmdV1=\"ZoomIn/ZoomOut\""),		(TObject*)&WheelCmdV[1]},
		{_T("WheelCmdV2=\"PageUp/PageDown\""),		(TObject*)&WheelCmdV[2]},
		{_T("WheelCmdV3=\"\""),						(TObject*)&WheelCmdV[3]},
		{_T("WheelCmdI0=\"PrevFile/NextFile\""),	(TObject*)&WheelCmdI[0]},
		{_T("WheelCmdI1=\"ZoomIn/ZoomOut\""),		(TObject*)&WheelCmdI[1]},
		{_T("WheelCmdI2=\"ScrollUp/ScrollDown\""),	(TObject*)&WheelCmdI[2]},
		{_T("WheelCmdI3=\"ScrollLeft/ScrollRight\""),	(TObject*)&WheelCmdI[3]},
		{_T("WheelBtnCmdF=\"ZoomReset\""),			(TObject*)&WheelBtnCmdF[0]},
		{_T("WheelBtnCmdF1=\"\""),					(TObject*)&WheelBtnCmdF[1]},
		{_T("WheelBtnCmdF2=\"\""),					(TObject*)&WheelBtnCmdF[2]},
		{_T("WheelBtnCmdF3=\"\""),					(TObject*)&WheelBtnCmdF[3]},
		{_T("WheelBtnCmdV=\"ZoomReset\""),			(TObject*)&WheelBtnCmdV},
		{_T("WheelBtnCmdI=\"FittedSize\""),			(TObject*)&WheelBtnCmdI},
		{_T("X1BtnCmdF=\"BackDirHist\""),			(TObject*)&X1BtnCmdF},
		{_T("X2BtnCmdF=\"\""),						(TObject*)&X2BtnCmdF},
		{_T("X1BtnCmdV=\"\""),						(TObject*)&X1BtnCmdV},
		{_T("X2BtnCmdV=\"\""),						(TObject*)&X2BtnCmdV},
		{_T("X1BtnCmdI=\"\""),						(TObject*)&X1BtnCmdI},
		{_T("X2BtnCmdI=\"\""),						(TObject*)&X2BtnCmdI},
		{_T("MigemoPath=\"\""),						(TObject*)&MigemoPath},
		{_T("SpiDir=\"\""),							(TObject*)&SpiDir},
		{_T("BgImgName=\"\""),						(TObject*)&BgImgName[0]},
		{_T("BgImgName2=\"\""),						(TObject*)&BgImgName[1]},
		{_T("SpImgName=\"\""),						(TObject*)&BgImgName[2]},
		{_T("SpImgName2=\"\""),						(TObject*)&BgImgName[3]},
		{_T("KnobImgName=\"\""),					(TObject*)&BgImgName[4]},
		{_T("NoDirHistPath=\"\""),					(TObject*)&NoDirHistPath},
		{_T("NoEditHistPath=\"\""),					(TObject*)&NoEditHistPath},
		{_T("NoViewHistPath=\"\""),					(TObject*)&NoViewHistPath},
		{_T("FTPTextModeFExt=\".txt.htm.html.shtm.shtml.css.cgi.log.pl.sh.rb.js.c.cpp.h\""),
													(TObject*)&FTPTextModeFExt},
		{_T("FTPSndConnect=\"\""),					(TObject*)&FTPSndConnect},
		{_T("FTPSndDiscon=\"\""),					(TObject*)&FTPSndDiscon},
		{_T("FTPSndTransfer=\"\""),					(TObject*)&FTPSndTransfer},
		// default = ExePath
		{_T("DownloadPath=\"%ExePath%\""),			(TObject*)&DownloadPath},
		{_T("WorkListPath=\"%ExePath%\""),			(TObject*)&WorkListPath},
		{_T("RefParamPath=\"%ExePath%\""),			(TObject*)&RefParamPath},
		{_T("IconFilePath=\"%ExePath%\""),			(TObject*)&IconFilePath},
		{_T("SaveTxtPath=\"%ExePath%\""),			(TObject*)&SaveTxtPath},
		{_T("CmdFilePath=\"%ExePath%\""),			(TObject*)&CmdFilePath},

		//Integer  (�f�t�H���g�l�����p���ň͂܂��Ɏw��)
		{_T("MaxTasks=4"),					(TObject*)&MaxTasks},
		{_T("FlashCntWarning=0"),			(TObject*)&FlashCntWarning},
		{_T("FlashTimeWarning=0"),			(TObject*)&FlashTimeWarning},
		{_T("FlashCntTaskfin=0"),			(TObject*)&FlashCntTaskfin},
		{_T("FlashTimeTaskfin=0"),			(TObject*)&FlashTimeTaskfin},
		{_T("MaxLogLines=1000"),			(TObject*)&MaxLogLines},
		{_T("MsgHintTime=1000"),			(TObject*)&MsgHintTime},
		{_T("KeyHintTime=1000"),			(TObject*)&KeyHintTime},
		{_T("NotifyTaskTime=10"),			(TObject*)&NotifyTaskTime},
		{_T("ViewTxtInterLn=4"),			(TObject*)&ViewTxtInterLn},
		{_T("ViewLeftMargin=10"),			(TObject*)&ViewLeftMargin},
		{_T("ViewFoldWidth=80"),	 		(TObject*)&ViewFoldWidth},
		{_T("ViewFixedLimit=0"),			(TObject*)&ViewFixedLimit},
		{_T("ViewTxtLimitSize=262144"),		(TObject*)&ViewTxtLimitSize},
		{_T("ViewWheelScrLn=3"),			(TObject*)&ViewWheelScrLn},
		{_T("ListWheelScrLn=2"),			(TObject*)&ListWheelScrLn},
		{_T("PlaySndLimitTime=3"),			(TObject*)&PlaySndLimitTime},
		{_T("CmpDelOwCnt=3"),				(TObject*)&CmpDelOwCnt},
		{_T("PrvTxtInfLn=1000"),			(TObject*)&PrvTxtInfLn},
		{_T("PrvTxtTailLn=100"),			(TObject*)&PrvTxtTailLn},
		{_T("PrvActTailLn=0"),				(TObject*)&PrvActTailLn},
		{_T("GetInfLimitSize=0x40000000"),	(TObject*)&GetInfLimitSize},
		{_T("WatchInterval=1000"),			(TObject*)&WatchInterval},
		{_T("LogInterval=250"),				(TObject*)&LogInterval},
		{_T("InitialModeI=0"),				(TObject*)&InitialModeI},
		{_T("LastZoomRatio=100"),			(TObject*)&LastZoomRatio},
		{_T("RemoteWaitTime=200"),			(TObject*)&RemoteWaitTime},
		{_T("NormalWaitTime=2"),			(TObject*)&NormalWaitTime},
		{_T("NopDtctTime=10"),				(TObject*)&NopDtctTime},
		{_T("TimeTolerance=2000"),			(TObject*)&TimeTolerance},
		{_T("DlgMovePrm=16"),				(TObject*)&DlgMovePrm},
		{_T("DlgSizePrm=16"),				(TObject*)&DlgSizePrm},
		{_T("BorderMoveWidth=50"),			(TObject*)&BorderMoveWidth},
		{_T("OpenByMode=0"),				(TObject*)&OpenByMode},
		{_T("DblClickFlMode=0"),			(TObject*)&DblClickFlMode},
		{_T("IncSeaMigemoMin=1"),			(TObject*)&IncSeaMigemoMin},
		{_T("DefDropMode=0"),				(TObject*)&DefDropMode},
		{_T("ListInterLn=4"),				(TObject*)&ListInterLn},
		{_T("CursorWidth=2"),				(TObject*)&CursorWidth},
		{_T("CursorAlpha=0"),				(TObject*)&CursorAlpha},
		{_T("SplitterWidth=4"),				(TObject*)&SplitterWidth},
		{_T("SplitterWidth2=4"),			(TObject*)&SplitterWidth2},
		{_T("SizeFormatMode=0"),			(TObject*)&SizeFormatMode},
		{_T("SizeDecDigits=1"),				(TObject*)&SizeDecDigits},
		{_T("ListPercent=50"),				(TObject*)&ListPercent},
		{_T("ImgFrameMargin=0"),			(TObject*)&ImgFrameMargin},
		{_T("ImgFitMaxZoom=100"),			(TObject*)&ImgFitMaxZoom},
		{_T("ImgGridHorzN=3"),				(TObject*)&ImgGridHorzN},
		{_T("ImgGridVertN=3"),				(TObject*)&ImgGridVertN},
		{_T("MinShowTime=0"),				(TObject*)&MinShowTime},
		{_T("ThumbnailSize=120"),			(TObject*)&ThumbnailSize},
		{_T("ThumbBdrWidth=3"),				(TObject*)&ThumbBdrWidth},
		{_T("ImgDblMargin=0"),				(TObject*)&ImgDblMargin},
		{_T("WicScaleOpt=0"),				(TObject*)&WicScaleOpt},
		{_T("MaxLogFiles=5"),				(TObject*)&MaxLogFiles},
		{_T("BatLowerLimit=50"),			(TObject*)&BatLowerLimit},
		{_T("ScrBarStyle=0"),				(TObject*)&ScrBarStyle},
		{_T("ModalScrAlpha=64"),			(TObject*)&ModalScrAlpha},
		{_T("BgImgMode=0"),					(TObject*)&BgImgMode},
		{_T("BgImgSubMode=0"),				(TObject*)&BgImgSubMode},
		{_T("BgColAlpha=0"),				(TObject*)&BgColAlpha},
		{_T("BgHideTime=1000"),				(TObject*)&BgHideTime},
		{_T("BgTileSize=100"),				(TObject*)&BgTileSize},
		{_T("AlphaValue=255"),				(TObject*)&AlphaValue},
		{_T("FlTabWidth=100"),				(TObject*)&FlTabWidth},
		{_T("FlTabStyle=0"),				(TObject*)&FlTabStyle},
		{_T("IconCache=500"),				(TObject*)&IconCache},
		{_T("FTPRemoteSide=0"),				(TObject*)&FTPRemoteSide},
		{_T("FTPDisconTimeout=300"),		(TObject*)&FTPDisconTimeout},
		{_T("ProtectDirMode=0"),			(TObject*)&ProtectDirMode},
		{_T("FindPathWidth=150"),			(TObject*)&FindPathWidth},

		//Bool (�f�t�H���g�l�� true �܂��� false �Ŏw��)
		{_T("UseSpiFirst=false"),			(TObject*)&UseSpiFirst},
		{_T("LastMigemoMode=false"),		(TObject*)&LastMigemoMode},
		{_T("LastMigemoModeF=false"),		(TObject*)&LastMigemoModeF},
		{_T("DivFileListUD=false"),			(TObject*)&DivFileListUD},
		{_T("DivDirInfUD=true"),			(TObject*)&DivDirInfUD},
		{_T("ViewFoldFitWin=true"),			(TObject*)&ViewFoldFitWin},
		{_T("TvCursorVisible=false"),		(TObject*)&TvCursorVisible},
		{_T("LimitBinCsr=true"),			(TObject*)&LimitBinCsr},
		{_T("TxtColorHint=true"),			(TObject*)&TxtColorHint},
		{_T("LimitGetRarInf=false"),		(TObject*)&LimitGetRarInf},
		{_T("BinMemMaped=false"),			(TObject*)&BinMemMaped},
		{_T("MultiInstance=false"),			(TObject*)&MultiInstance},
		{_T("CloseOthers=false"),			(TObject*)&CloseOthers},
		{_T("StoreTaskTray=false"),			(TObject*)&StoreTaskTray},
		{_T("ShowIcon=false"),		 		(TObject*)&ShowIcon},
		{_T("UseIndIcon=true"),		 		(TObject*)&UseIndIcon},
		{_T("ShowHideAtr=false"),	 		(TObject*)&ShowHideAtr},
		{_T("ShowSystemAtr=false"),			(TObject*)&ShowSystemAtr},
		{_T("DispRegName=false"),			(TObject*)&DispRegName},
		{_T("UncToNetDrive=false"),			(TObject*)&UncToNetDrive},
		{_T("PathInTitleBar=false"),		(TObject*)&PathInTitleBar},
		{_T("OmitEndOfName=false"),			(TObject*)&OmitEndOfName},
		{_T("ShowDirJumboIco=true"),		(TObject*)&ShowDirJumboIco},
		{_T("ShowInZipImg=false"),			(TObject*)&ShowInZipImg},
		{_T("PreviewAniGif=false"),			(TObject*)&PreviewAniGif},
		{_T("ForceDel=false"),				(TObject*)&ForceDel},
		{_T("RemoveCdReadOnly=false"),		(TObject*)&RemoveCdReadOnly},
		{_T("CopyNoBuffering=false"),		(TObject*)&CopyNoBuffering},
		{_T("ShowArcCopyProg=false"),		(TObject*)&ShowArcCopyProg},
		{_T("DelUseTrash=false"),			(TObject*)&DelUseTrash},
		{_T("EditNewText=false"),			(TObject*)&EditNewText},
		{_T("ViewArcInf=true"),				(TObject*)&ViewArcInf},
		{_T("PreviewTxtToInf=true"),		(TObject*)&ShowTextPreview},
		{_T("ShowTailPreview=false"),		(TObject*)&ShowTailPreview},
		{_T("ReloadOnActive=true"),			(TObject*)&ReloadOnActive},
		{_T("OpenAddedDrive=false"),		(TObject*)&OpenAddedDrive},
		{_T("CheckUnc=true"),				(TObject*)&CheckUnc},
		{_T("NotShowNoTask=false"),			(TObject*)&NotShowNoTask},
		{_T("ShowMsgHint=false"),	 		(TObject*)&ShowMsgHint},
		{_T("ShowKeyHint=false"),			(TObject*)&ShowKeyHint},
		{_T("ShowNoKeyHint=true"),			(TObject*)&ShowNoKeyHint},
		{_T("ShowCopyHint=true"),			(TObject*)&ShowCopyHint},
		{_T("ShowDuplNotify=true"),			(TObject*)&ShowDuplNotify},
		{_T("LogErrOnly=false"),			(TObject*)&LogErrOnly},
		{_T("LogErrMsg=true"),				(TObject*)&LogErrMsg},
		{_T("LogDebugInf=false"),			(TObject*)&LogDebugInf},
		{_T("LogHideSkip=false"),			(TObject*)&LogHideSkip},
		{_T("LogFullPath=false"),			(TObject*)&LogFullPath},
		{_T("SaveLog=true"),				(TObject*)&SaveLog},
		{_T("AppendLog=false"),				(TObject*)&AppendLog},
		{_T("NotSortWorkList=false"),	 	(TObject*)&NotSortWorkList},
		{_T("AutoDelWorkList=true"),	 	(TObject*)&AutoDelWorkList},
		{_T("NoCheckDirHist=false"),		(TObject*)&NoCheckDirHist},
		{_T("ExtSaveDirHist=false"),		(TObject*)&ExtSaveDirHist},
		{_T("DirHistCsrPos=true"),	 		(TObject*)&DirHistCsrPos},
		{_T("DelDplDirHist=false"),			(TObject*)&DelDplDirHist},
		{_T("WorkToDirHist=false"),			(TObject*)&WorkToDirHist},
		{_T("LoopFilerCursor=false"),		(TObject*)&LoopFilerCursor},
		{_T("FlCursorVisible=false"),		(TObject*)&FlCursorVisible},
		{_T("AutoCompComboBox=true"),		(TObject*)&AutoCompComboBox},
		{_T("DialogCenter=false"),			(TObject*)&DialogCenter},
		{_T("SelectByMouse=false"),			(TObject*)&SelectByMouse},
		{_T("SelectBaseOnly=false"),		(TObject*)&SelectBaseOnly},
		{_T("SelectIconSngl=false"),		(TObject*)&SelectIconSngl},
		{_T("ShowLineNo=true"),				(TObject*)&ShowLineNo},
		{_T("ShowLineCursor=true"),			(TObject*)&ShowLineCursor},
		{_T("ShowTAB=true"),				(TObject*)&ShowTAB},
		{_T("ShowCR=true"),					(TObject*)&ShowCR},
		{_T("ShowTextRuler=true"),			(TObject*)&ShowTextRuler},
		{_T("TxtSttIsBottom=false"),		(TObject*)&TxtSttIsBottom},
		{_T("ChkAozora=false"),				(TObject*)&ChkAozora},
		{_T("UseXd2tx=true"),				(TObject*)&UseXd2tx},
		{_T("ClickableUrl=true"),			(TObject*)&ClickableUrl},
		{_T("RestoreViewLine=true"),		(TObject*)&RestoreViewLine},
		{_T("EmpComment=true"),				(TObject*)&EmpComment},
		{_T("EmpStrings=true"),				(TObject*)&EmpStrings},
		{_T("EmpReserved=true"),			(TObject*)&EmpReserved},
		{_T("EmpSymbol=true"),				(TObject*)&EmpSymbol},
		{_T("EmpNumeric=true"),				(TObject*)&EmpNumeric},
		{_T("EmpHeadline=true"),			(TObject*)&EmpHeadline},
		{_T("EmpRuby=true"),				(TObject*)&EmpRuby},
		{_T("ToMarkdown=false"),			(TObject*)&ToMarkdown},
		{_T("HtmInsHrSct=false"),			(TObject*)&HtmInsHrSct},
		{_T("HtmInsHrArt=false"),			(TObject*)&HtmInsHrArt},
		{_T("HtmInsHrNav=false"),			(TObject*)&HtmInsHrNav},
		{_T("RotViewImg=true"),				(TObject*)&RotViewImg},
		{_T("KeepZoomRatio=false"),			(TObject*)&KeepZoomRatio},
		{_T("ShowThumbName=false"),			(TObject*)&ShowThumbName},
		{_T("ShowThumbExif=false"),			(TObject*)&ShowThumbExif},
		{_T("ShowThumbFExt=false"),			(TObject*)&ShowThumbFExt},
		{_T("NotThumbIfArc=true"),			(TObject*)&NotThumbIfArc},
		{_T("LoopViewCursor=false"),		(TObject*)&LoopViewCursor},
		{_T("HintTopEndImg=false"),			(TObject*)&HintTopEndImg},
		{_T("BeepTopEndImg=false"),			(TObject*)&BeepTopEndImg},
		{_T("ImgSttIsBottom=false"),		(TObject*)&ImgSttIsBottom},
		{_T("SeekBindDir=false"),			(TObject*)&SeekBindDir},
		{_T("SeekSwapNxtPrv=false"),		(TObject*)&SeekSwapNxtPrv},
		{_T("OneStepInDblPg=false"),		(TObject*)&OneStepInDblPg},
		{_T("HideCsrInFull=true"),			(TObject*)&HideCsrInFull},
		{_T("HideThumbInFull=false"),		(TObject*)&HideThumbInFull},
		{_T("AnimateGif=false"),			(TObject*)&AnimateGif},
		{_T("ShowThumbScroll=true"),		(TObject*)&ShowThumbScroll},
		{_T("MarkImgClose=false"),			(TObject*)&MarkImgClose},
		{_T("TimColEnabled=false"),			(TObject*)&TimColEnabled},
		{_T("PriorFExtCol=false"),			(TObject*)&PriorFExtCol},
		{_T("ColorOnlyFExt=false"),			(TObject*)&ColorOnlyFExt},
		{_T("ShowMainMenu=true"),			(TObject*)&ShowMainMenu},
		{_T("ShowImgPreview=true"),			(TObject*)&ShowImgPreview},
		{_T("ShowProperty=true"),			(TObject*)&ShowProperty},
		{_T("ShowFileListOnly=false"),		(TObject*)&ShowFileListOnly},
		{_T("ShowFileListOnly2=false"),		(TObject*)&ShowFileListOnly2},
		{_T("ShowSttBar=true"),				(TObject*)&ShowSttBar},
		{_T("ShowToolBar=false"),			(TObject*)&ShowToolBar},
		{_T("ShowToolBarV=false"),			(TObject*)&ShowToolBarV},
		{_T("ShowToolBarI=false"),			(TObject*)&ShowToolBarI},
		{_T("ToolBarISide=false"),			(TObject*)&ToolBarISide},
		{_T("ShowTabBar=false"),			(TObject*)&ShowTabBar},
		{_T("ShowFKeyBar=false"),			(TObject*)&ShowFKeyBar},
		{_T("NoClsTabBtn=false"),			(TObject*)&NoClsTabBtn},
		{_T("HideScrBar=false"),			(TObject*)&HideScrBar},
		{_T("ShowByteSize=false"),			(TObject*)&ShowByteSize},
		{_T("ShowHeader=false"),			(TObject*)&ShowHeader},
		{_T("FixListWidth=false"),			(TObject*)&FixListWidth},
		{_T("KeepOnResize=false"),			(TObject*)&KeepOnResize},
		{_T("KeepCurListWidth=false"),		(TObject*)&KeepCurListWidth},
		{_T("NoSpaceFExt=false"),			(TObject*)&NoSpaceFExt},
		{_T("HideTitleMenu=false"),			(TObject*)&HideTitleMenu},
		{_T("FlatInfPanel=false"),			(TObject*)&FlatInfPanel},
		{_T("MenuAutoHotkey=false"),		(TObject*)&MenuAutoHotkey},
		{_T("EscapeHelp=false"),			(TObject*)&EscapeHelp},
		{_T("InhbitAltMenu=false"),			(TObject*)&InhbitAltMenu},
		{_T("SelColDrvInf=false"),			(TObject*)&SelColDrvInf},
		{_T("SureCopy=false"),				(TObject*)&SureCopy},
		{_T("SureMove=false"),				(TObject*)&SureMove},
		{_T("SureDelete=true"),				(TObject*)&SureDelete},
		{_T("SureUnPack=false"),			(TObject*)&SureUnPack},
		{_T("SureExec=false"),				(TObject*)&SureExec},
		{_T("SureOther=true"),				(TObject*)&SureOther},
		{_T("SureExit=false"),				(TObject*)&SureExit},
		{_T("SureCmpDel=true"),				(TObject*)&SureCmpDel},
		{_T("SureCancel=false"),			(TObject*)&SureCancel},
		{_T("SureDefNo=false"),				(TObject*)&SureDefNo},
		{_T("WarnPowerFail=false"),			(TObject*)&WarnPowerFail},
		{_T("WarnLowBattery=false"),		(TObject*)&WarnLowBattery},
		{_T("WarnDisconnect=false"),		(TObject*)&WarnDisconnect},
		{_T("AppListChgMin=false"),			(TObject*)&AppListChgMin},
		{_T("CreDirChg=true"),				(TObject*)&CreDirChg},
		{_T("CreDirCnvChar=false"),			(TObject*)&CreDirCnvChar},
		{_T("OpenByCmdPrompt=false"),		(TObject*)&OpenByCmdPrompt},
		{_T("OpenDirByStd=false"),			(TObject*)&OpenDirByStd},
		{_T("OpenOnlyCurApp=false"),		(TObject*)&OpenOnlyCurApp},
		{_T("OpenOnlyCurWin=false"),		(TObject*)&OpenOnlyCurWin},
		{_T("IniSeaByNum=false"),	 		(TObject*)&IniSeaByNum},
		{_T("IniSeaBySign=false"),			(TObject*)&IniSeaBySign},
		{_T("IncSeaCaseSens=false"),		(TObject*)&IncSeaCaseSens},
		{_T("IncSeaLoop=true"),				(TObject*)&IncSeaLoop},
		{_T("IncSeaMatch1Exit=false"),		(TObject*)&IncSeaMatch1Exit},
		{_T("DownAfterOpenStd=false"),		(TObject*)&DownAfterOpenStd},
		{_T("OpenStdOnResList=false"),		(TObject*)&OpenStdOnResList},
		{_T("SyncLR=false"),				(TObject*)&SyncLR},
		{_T("ModalScreen=false"),			(TObject*)&ModalScreen},
		{_T("BgImgGray=false"),				(TObject*)&BgImgGray},
		{_T("BgImgHideScr=false"),			(TObject*)&BgImgHideScr},
		{_T("BgImgTileIf=false"),			(TObject*)&BgImgTileIf},
		{_T("AlphaForm=false"),				(TObject*)&AlphaForm},
		{_T("ProtectSubDir=true"),			(TObject*)&ProtectSubDir},
		{_T("ProtectFile=false"),			(TObject*)&ProtectFile},
		{_T("ImageEditSgl=fale"),			(TObject*)&ImageEditSgl},
		{_T("FTPDlKeepTime=false"),			(TObject*)&FTPDlKeepTime},
		{_T("FTPUpKeepTime=false"),			(TObject*)&FTPUpKeepTime},
		{_T("FTPUpToLower=false"),			(TObject*)&FTPUpToLower},
		{_T("FTPLogResponse=false"),		(TObject*)&FTPLogResponse},
		{_T("FindPathColumn=false"),		(TObject*)&FindPathColumn},

		//[General] (prefix = U:)
		{_T("U:LastCurTag=0"),				(TObject*)&LastCurTag},
		{_T("U:SortMode1=1"),				(TObject*)&SortMode[0]},
		{_T("U:SortMode2=1"),				(TObject*)&SortMode[1]},
		{_T("U:DirSortMode1=0"),			(TObject*)&DirSortMode[0]},
		{_T("U:DirSortMode2=0"),			(TObject*)&DirSortMode[1]},
		{_T("U:IniPathMode1=0"),			(TObject*)&IniPathMode[0]},
		{_T("U:IniPathMode2=0"),			(TObject*)&IniPathMode[1]},
		{_T("U:IniSortMode1=0"),			(TObject*)&IniSortMode[0]},
		{_T("U:IniSortMode2=0"),			(TObject*)&IniSortMode[1]},
		{_T("U:InitialPath1=\"\""),			(TObject*)&InitialPath[0]},
		{_T("U:InitialPath2=\"\""),			(TObject*)&InitialPath[1]},
		{_T("U:InitialMask1=\"\""),			(TObject*)&InitialMask[0]},
		{_T("U:InitialMask2=\"\""),			(TObject*)&InitialMask[1]},
		{_T("U:IniWorkMode=0"),				(TObject*)&IniWorkMode},
		{_T("U:HomeWorkList=\"WORKLIST.nwl\""),	(TObject*)&HomeWorkList},
		{_T("U:IniWinMode=0"),				(TObject*)&IniWinMode},
		{_T("U:FixWinPos=false"),			(TObject*)&FixWinPos},
		{_T("U:IniPathToTab1=true"),		(TObject*)&IniPathToTab1},
		{_T("U:IniTabHomeAll=false"),		(TObject*)&IniTabHomeAll},
		{_T("U:ShowSplash=true"),			(TObject*)&ShowSplash},
		{_T("U:IniWinWidth=800"),			(TObject*)&IniWinWidth},
		{_T("U:IniWinHeight=600"),			(TObject*)&IniWinHeight},
		{_T("U:IniWinLeft=50"),				(TObject*)&IniWinLeft},
		{_T("U:IniWinTop=50"),				(TObject*)&IniWinTop},
		{_T("U:LayoutMode=0"),				(TObject*)&LayoutMode},
		{_T("U:SaveEncIndex=0"),			(TObject*)&SaveEncIndex},
		{_T("U:CopyAll=false"),				(TObject*)&gCopyAll},
		{_T("U:CopyMode=0"),				(TObject*)&gCopyMode},
		{_T("U:CopyMode2=0"),				(TObject*)&gCopyMode2},

		{_T("U:FlOdrNatural1=true"),		(TObject*)&FlOdrNatural[0]},
		{_T("U:FlOdrNatural2=true"),		(TObject*)&FlOdrNatural[1]},
		{_T("U:FlOdrDscName1=false"),		(TObject*)&FlOdrDscName[0]},
		{_T("U:FlOdrDscName2=false"),		(TObject*)&FlOdrDscName[1]},
		{_T("U:FlOdrSmall1=false"),			(TObject*)&FlOdrSmall[0]},
		{_T("U:FlOdrSmall2=false"),			(TObject*)&FlOdrSmall[1]},
		{_T("U:FlOdrOld1=false"),			(TObject*)&FlOdrOld[0]},
		{_T("U:FlOdrOld2=false"),			(TObject*)&FlOdrOld[1]},
		{_T("U:FlOdrDscAttr1=false"),		(TObject*)&FlOdrDscAttr[0]},
		{_T("U:FlOdrDscAttr2=false"),		(TObject*)&FlOdrDscAttr[1]},
		{_T("U:FlOdrDscPath1=false"),		(TObject*)&FlOdrDscPath[0]},
		{_T("U:FlOdrDscPath2=false"),		(TObject*)&FlOdrDscPath[1]},

		{_T("U:SortBoth=false"),			(TObject*)&SortBoth},
		{_T("U:ShowImgSidebar=false"),		(TObject*)&ShowImgSidebar},
		{_T("U:ImgSidebarIsLeft=true"),		(TObject*)&ImgSidebarIsLeft},
		{_T("U:ThumbnailPos=1"),			(TObject*)&ThumbnailPos},
		{_T("U:ShowHistogram=false"),		(TObject*)&ShowHistogram},
		{_T("U:ShowLoupe=false"),			(TObject*)&ShowLoupe},
		{_T("U:ShowGifViewer=false"),		(TObject*)&ShowGifViewer},
		{_T("U:ShowSeekBar=false"),			(TObject*)&ShowSeekBar},
		{_T("U:WarnHighlight=false"),		(TObject*)&WarnHighlight},
		{_T("U:DoublePage=false"),			(TObject*)&DoublePage},
		{_T("U:RightBind=true"),			(TObject*)&RightBind},
		{_T("U:PermitDotCmds=false"),		(TObject*)&PermitDotCmds},
		{_T("U:InheritDotNyan=false"),		(TObject*)&InheritDotNyan},
		{_T("U:DotNyanPerUser=false"),		(TObject*)&DotNyanPerUser},

		//[Grep] (prefix = G:)
		{_T("G:OutMode=0"),					(TObject*)&GrepOutMode},
		{_T("G:OutFileName=\"\""),			(TObject*)&GrepFileName},
		{_T("G:OutAppName=\"\""),			(TObject*)&GrepAppName},
		{_T("G:OutAppParam=\"\""),			(TObject*)&GrepAppParam},
		{_T("G:OutAppDir=\"\""),			(TObject*)&GrepAppDir},
		{_T("G:GrepAppend=false"),			(TObject*)&GrepAppend},
		{_T("G:FileFmt=\"$F $L:\""),		(TObject*)&GrepFileFmt},
		{_T("G:InsStrW=\"\""),				(TObject*)&GrepInsStrW},
		{_T("G:InsStrW2=\"\""),				(TObject*)&GrepInsStrW2},
		{_T("G:TremLeft=true"),				(TObject*)&GrepTrimLeft},
		{_T("G:ReplaceTab=true"),			(TObject*)&GrepReplaceTab},
		{_T("G:ReplaceCr=true"),			(TObject*)&GrepReplaceCr},
		{_T("G:RepCrStr=\" �^ \""),			(TObject*)&GrepRepCrStr},
		{_T("G:BackupReplace=true"),		(TObject*)&BackupReplace},
		{_T("G:FExtRepBackup=\".bak\""),	(TObject*)&FExtRepBackup},
		{_T("G:RepBackupDir=\"\""),			(TObject*)&RepBackupDir},
		{_T("G:ReplaceLogName=\"\""),		(TObject*)&ReplaceLogName},
		{_T("G:SaveReplaceLog=false"),		(TObject*)&SaveReplaceLog},
		{_T("G:ReplaceAppend=false"),		(TObject*)&ReplaceAppend},
		{_T("G:OpenReplaceLog=false"),		(TObject*)&OpenReplaceLog},
		{_T("G:ShowItemNo=false"),			(TObject*)&GrepShowItemNo},
		{_T("G:FileItemNo=false"),			(TObject*)&GrepFileItemNo},
		{_T("G:ShowSubDir=true"),			(TObject*)&GrepShowSubDir},
		{_T("G:TrimTop=false"),				(TObject*)&GrepTrimTop},
		{_T("G:OmitTop=true"),				(TObject*)&GrepOmitTop},
		{_T("G:EmFilter=false"),			(TObject*)&GrepEmFilter},

		//�Z�N�V���� (prefix = S:)
		{_T("S:KeyFuncList="),				(TObject*)KeyFuncList},
		{_T("S:FKeyLabelList="),			(TObject*)FKeyLabelList},
		{_T("S:Associate="),				(TObject*)AssociateList},
		{_T("S:OpenStdCmd="),	 			(TObject*)OpenStdCmdList},
		{_T("S:EtcEditor="),				(TObject*)EtcEditorList},
		{_T("S:HeadlineList="),				(TObject*)HeadlineList},
		{_T("S:CnvCharList="),				(TObject*)CnvCharList},
		{_T("S:Color="),					(TObject*)ColorList},
		{_T("S:BakSetupList="),				(TObject*)BakSetupList},
		{_T("S:CustomColors="),				(TObject*)UserModule->ColorDlg->CustomColors},

		//���X�g	(prefix = L:)	�ő區�ڐ�,���p�����O��
		{_T("L:DirStack=30,false"),			(TObject*)DirStack},
		{_T("L:TabList=30,false"),			(TObject*)TabList},
		{_T("L:PathMaskList=30,false"),		(TObject*)PathMaskList},
		{_T("L:RegDirList=50,false"),		(TObject*)RegDirList},
		{_T("L:ProtectDirList=0,true"),		(TObject*)ProtectDirList},
		{_T("L:SyncDirList=50,false"),		(TObject*)SyncDirList},
		{_T("L:AssRenList=30,false"),		(TObject*)AssRenList},
		{_T("L:DistrDefList=200,false"),	(TObject*)DistrDefList},
		{_T("L:ExtColList=30,true"),		(TObject*)ExtColList},
		{_T("L:ExtMenuList=100,false"),		(TObject*)ExtMenuList},
		{_T("L:ExtToolList=100,false"),		(TObject*)ExtToolList},
		{_T("L:ToolBtnList=0,false"),		(TObject*)ToolBtnList},
		{_T("L:ToolBtnListV=0,false"),		(TObject*)ToolBtnListV},
		{_T("L:ToolBtnListI=0,false"),		(TObject*)ToolBtnListI},
		{_T("L:TextViewHistory=50,true"),	(TObject*)TextViewHistory},
		{_T("L:TextEditHistory=50,true"),	(TObject*)TextEditHistory},
		{_T("L:WorkListHistory=50,true"),	(TObject*)WorkListHistory},
		{_T("L:InpDirHistory=50,true"),		(TObject*)InputDirHistory},
		{_T("L:InpCmdsHistory=30,true"),	(TObject*)InputCmdsHistory},
		{_T("L:InpCmdsHistoryV=20,true"),	(TObject*)InputCmdsHistoryV},
		{_T("L:InpCmdsHistoryI=20,true"),	(TObject*)InputCmdsHistoryI},
		{_T("L:IncSeaHistory=50,true"),		(TObject*)IncSeaHistory},
		{_T("L:FilterHistory=50,true"),		(TObject*)FilterHistory},
		{_T("L:WebSeaHistory=30,true"),		(TObject*)WebSeaHistory},
		{_T("L:LatLngHistory=100,true"),	(TObject*)LatLngHistory}
	};

	int cnt = sizeof(opt_def_list)/sizeof(opt_def_list[0]);
	for (int i=0; i<cnt; i++) OptionList->AddObject(opt_def_list[i].def, (TObject*)opt_def_list[i].vp);
	//�ǂݍ���
	LoadOptions();

	//�f�t�H���g�̃L�[�ݒ�
	if (KeyFuncList->Count==0) {
		KeyFuncList->Text =
			"F:A=SelAllFile\n"
			"F:B=BackDirHist\n"
			"F:C=Copy\n"
			"F:D=Delete\n"
			"F:E=FileEdit\n"
			"F:F=IncSearch\n"
			"F:G=ImageViewer\n"
			"F:H=DirHistory\n"
			"F:I=CalcDirSize\n"
			"F:J=RegDirDlg\n"
			"F:K=CreateDir\n"
			"F:L=DriveList\n"
			"F:M=Move\n"
			"F:O=CurrFromOpp\n"
			"F:P=Pack\n"
			"F:Q=Exit\n"
			"F:R=RenameDlg\n"
			"F:S=SortDlg\n"
			"F:U=UnPack\n"
			"F:V=TextViewer\n"
			"F:W=WorkList\n"
			"F:X=ContextMenu\n"
			"F:ESC=TaskMan\n"
			"F:F3=FindFileDlg\n"
			"F:F5=ReloadList\n"
			"F:LEFT=ToLeft\n"
			"F:RIGHT=ToRight\n"
			"F:SPACE=Select\n"
			"F:Ctrl+Enter=OpenByApp\n"
			"F:Shift+O=CurrToOpp\n"
			"F:Shift+DOWN=CursorDownSel\n"
			"F:Shift+UP=CursorUpSel\n"
			"F:Shift+ENTER=OpenByWin\n"
			"V:B=ChangeViewMode\n"
			"V:F=FindText\n"
			"V:E=FileEdit\n"
			"V:Q=Close\n"
			"I:C=ClipCopy\n"
			"I:D=Delete\n"
			"I:E=FileEdit\n"
			"I:S=SortDlg\n"
			"I:SPACE=Select\n"
			"I:Q=Close\n"
			"L:ESC=TaskMan\n"
			"S:ENTER=IncSearchExit\n";
	}

	//�f�t�H���g�̃p�X�}�X�N
	if (PathMaskList->Count==0) {
		PathMaskList->Text =
			"A,\"�}�X�N������\",\"*\"\n"
			"X,\"���s�t�@�C��\",\"*.exe;*.com;*.bat;*.cmd\"\n";
	}

	//�֎~�����̃f�t�H���g
	if (CnvCharList->Count<9) {
		CnvCharList->Text =
			"\\=��\n"
			"/=�^\n"
			":=�F\n"
			"*=��\n"
			"?=�H\n"
			"\"=�h\n"
			"<=��\n"
			">=��\n"
			"|=�b\n";
	}

	//----------------------------------
	//���̑��̏�����
	//----------------------------------
	usr_SH = new UserShell(MainHandle);
	usr_SH->PropNameWidth = FPRP_NAM_WD;
	usr_SH->ScrScale = ScrScale;

	usr_TAG = new TagManager(UAPP_T(ExePath, TAGDATA_FILE));

	usr_ARC = new UserArcUnit(MainHandle);
	usr_ARC->FExt7zDll = FExt7zDll;

	SPI = new SpiUnit(rel_to_absdir(SpiDir));

	usr_Migemo = new MigemoUnit(rel_to_absdir(MigemoPath));
	usr_Migemo->MinLength = IncSeaMigemoMin;

	TaskReserveList = new TaskConfigList();
	ClearNopStt();

	//�h���C�u�e�ʃ��O�̏�����
	DriveLogName = UAPP_T(ExePath, DRVLOG_FILE);
	if (file_exists(DriveLogName) && load_text_ex(DriveLogName, DriveLogList)!=0)
		update_DriveLog(false);
	else
		DriveLogName = EmptyStr;

	//----------------------------------
	//HtmlHelp �̏�����
	//----------------------------------
	Application->HelpFile = ChangeFileExt(Application->ExeName, ".chm");
	hHHctrl = ::LoadLibrary(_T("hhctrl.ocx"));
	if (hHHctrl) {
		lpfHtmlHelp = (FUNC_HtmlHelp)::GetProcAddress(hHHctrl, "HtmlHelpW");
		if (lpfHtmlHelp) lpfHtmlHelp(NULL, NULL, HH_INITIALIZE, (DWORD)&Cookie);
	}

	//����JAPI
	HINSTANCE hGdi32 = ::LoadLibrary(_T("gdi32.dll"));
	if (hGdi32) {
		lpGetFontResourceInfo = (FUNC_GetFontResourceInfo)::GetProcAddress(hGdi32, "GetFontResourceInfoW");
	}

	WorkListChanged = WorkListFiltered = rqWorkListDirInf = false;
}

//---------------------------------------------------------------------------
//��Еt��
//---------------------------------------------------------------------------
void EndGlobal()
{
	::mciSendString(_T("close all"), NULL, 0, NULL);

	for (int i=0; i<GeneralList->Count; i++) {
		TStringList *lst = (TStringList*)GeneralList->Objects[i];

		switch (GeneralList->Strings[i].ToIntDef(0)) {
		case GENLST_FILELIST:
			clear_FileList(lst);
			break;
		case GENLST_DRIVE:
			for (int j=0; j<lst->Count; j++) {
				drive_info *dp = (drive_info*)lst->Objects[j];
				delete dp->small_ico;
				delete dp->large_ico;
				delete dp;
			}
			break;
		case GENLST_CMDSLIST:
			for (int j=0; j<lst->Count; j++) {
				cmdf_rec *cp = (cmdf_rec*)lst->Objects[j];
				delete cp->file_buf;
				delete cp->cmd_list;
				delete cp;
			}
			break;
		case GENLST_TABLIST:
			for (int j=0; j<lst->Count; j++) del_tab_info((tab_info*)lst->Objects[j]);
			break;
		case GENLST_ICON:
			for (int j=0; j<lst->Count; j++) delete (TIcon*)lst->Objects[j];
			break;
		case GENLST_FONT:
			for (int j=0; j<lst->Count; j++) delete (TFont*)lst->Objects[j];
			break;
		}

		delete lst;
	}
	delete GeneralList;

	for (int i=0; i<MAX_BGIMAGE; i++) delete BgImgBuff[i];

	delete usr_Migemo;
	delete SPI;
	delete usr_ARC;
	delete usr_TAG;
	delete usr_SH;

	delete TaskReserveList;

	if (hHHctrl) {
		if (lpfHtmlHelp) {
			lpfHtmlHelp(NULL, NULL, HH_CLOSE_ALL, 0) ;
			lpfHtmlHelp(NULL, NULL, HH_UNINITIALIZE, (DWORD)Cookie);
		}
		::FreeLibrary(hHHctrl);
	}

	if (hGdi32) ::FreeLibrary(hGdi32);
}

//---------------------------------------------------------------------------
//�^�C�}�[�ݒ�̍X�V
//---------------------------------------------------------------------------
void SetupTimer()
{
	for (int i=0; i<MAX_TIMER_EVENT; i++) {
		Timer_Enabled[i] = Timer_NopAct[i] = false;
		if (Timer_Condition[i].IsEmpty()) continue;
		TStringDynArray prm_lst = get_csv_array(Timer_Condition[i], MAX_TIMER_ALARM);
		if (prm_lst.Length==0) continue;
		//����
		if (contains_s(prm_lst[0], _T(':'))) {
			Timer_TimeCnt[i] = Timer_NopCnt[i] = 0;
			Timer_AlarmList[i]->Clear();
			for (int j=0; j<prm_lst.Length; j++) {
				if (prm_lst[j].IsEmpty()) continue;
				try {
					if (USTARTS_TS("??:", prm_lst[j])) {
						UnicodeString mm = get_tkn_r(prm_lst[j], _T("??:"));
						for (int h=0; h<24; h++) {
							TDateTime dt = TTime(UnicodeString().sprintf(_T("%02u:%s"), h, mm.c_str())) + Date();
							if (CompareDateTime(Now(), dt)==GreaterThanValue) dt = IncDay(dt, 1);
							Timer_AlarmList[i]->Add(format_DateTime(dt));
						}
					}
					else {
						TDateTime dt = TTime(prm_lst[j]) + Date();
						if (CompareDateTime(Now(), dt)==GreaterThanValue) dt = IncDay(dt, 1);
						Timer_AlarmList[i]->Add(format_DateTime(dt));
					}
				}
				catch (EConvertError &e) {
					;
				}
			}

			if (Timer_AlarmList[i]->Count>0) {
				Timer_AlarmList[i]->Sort();
				Timer_Enabled[i] = true;
			}
		}
		//�Ԋu�A������
		else {
			UnicodeString prm = prm_lst[0];
			if (remove_top_s(prm, '!')) {
				Timer_TimeCnt[i] = 0;
				Timer_NopCnt[i]  = param_to_mSec(prm);
			}
			else {
				Timer_TimeCnt[i] = param_to_mSec(prm);
				Timer_NopCnt[i]  = 0;
			}

			if (Timer_TimeCnt[i]>0 || Timer_NopCnt[i]>0) {
				Timer_RepeatCnt[i] = Timer_RepeatN[i] = (prm_lst.Length>1)? prm_lst[1].ToIntDef(0) : 0;
				Timer_Enabled[i]   = true;
			}
		}
	}
}
//---------------------------------------------------------------------------
//�������Ԃ̉���
//---------------------------------------------------------------------------
void ClearNopStt()
{
	LastOpCount = GetTickCount();
	for (int i=0; i<MAX_TIMER_EVENT; i++) Timer_NopAct[i] = false;
}

//---------------------------------------------------------------------------
//�I�v�V�����ݒ�̓ǂݍ���
//---------------------------------------------------------------------------
void LoadOptions()
{
	UnicodeString sct, tmp;

	//�o�^�I�v�V�����̓ǂݍ���
	TStringList *lp = OptionList;
	for (int i=0; i<lp->Count; i++) {
		UnicodeString key  = lp->Names[i];
		UnicodeString vbuf = lp->ValueFromIndex[i];
		//Section
		if (remove_top_text(key, _T("S:")))
			IniFile->ReadSection(key, (TStringList*)lp->Objects[i]);
		//List
		else if (remove_top_text(key, _T("L:"))) {
			int  n = split_tkn(vbuf, ',').ToIntDef(20);
			bool q = USAME_TI(vbuf, "true");
			IniFile->LoadListItems(key, (TStringList*)lp->Objects[i], n, q);
		}
		else {
			sct = remove_top_text(key, _T("U:"))? SCT_General :
				  remove_top_text(key, _T("G:"))? UnicodeString("Grep") : SCT_Option;
			if (is_quot(vbuf)) {
				//UnicodeString
				vbuf = exclude_quot(vbuf);
				if (USAME_TI(vbuf, "%ExePath%"))
					*((UnicodeString*)lp->Objects[i]) = to_path_name(IniFile->ReadString(sct, key, ExePath));
				else
					*((UnicodeString*)lp->Objects[i]) = IniFile->ReadString(sct, key, vbuf);
			}
			else {
				//bool
				if (contained_wd_i(_T("true|false"), vbuf))
					*((bool*)lp->Objects[i]) = IniFile->ReadBool(sct, key, USAME_TI(vbuf, "true"));
				//int
				else
					*((int*)lp->Objects[i])  = IniFile->ReadInteger(sct, key, vbuf.ToIntDef(0));
			}
		}
	}

	//�^�u���X�g�̏�����
	for (int i=0; i<TabList->Count; i++) {
		tab_info *tp = new tab_info;
		for (int j=0; j<MAX_FILELIST; j++) {
			tp->sel_list[j]   = new TStringList();
			tp->dir_hist[j]   = new TStringList();
			tp->dir_hist_p[j] = 0;
			IniFile->LoadListItems(sct.sprintf(_T("DirHistory%02u_%u"), i + 1, j), tp->dir_hist[j], 30, false);
		}
		TabList->Objects[i] = (TObject*)tp;
	}

	//�S�̃f�B���N�g������
	if (ExtSaveDirHist) {
		std::unique_ptr<UsrIniFile> hst_file(new UsrIniFile(ExePath + DIR_HIST_FILE));
		hst_file->LoadListItems("AllDirHistory", AllDirHistory, 0, false);
	}
	else {
		IniFile->LoadListItems("AllDirHistory", AllDirHistory, 100, false);
	}

	//���̑��̃I�v�V�����̓ǂݍ���
	sct = SCT_Option;
	TempPath  = to_path_name(IniFile->ReadString(sct, "TempPath", "Temp\\"));
	remove_top_s(TempPath, '\\');
	TempPathA = rel_to_absdir(TempPath);
	TempPathFTP.sprintf(_T("%s" TMP_FTP_D), TempPathA.c_str());

	if (MaxTasks<1 || MaxTasks>4) MaxTasks = 4;
	if (CmpDelOwCnt<=0) CmpDelOwCnt = 3;

	DirBraStr = IniFile->ReadString(sct, "DirBraStr", !IniFile->KeyExists(sct, "DirBraStr")? "[" : "");
	DirKetStr = IniFile->ReadString(sct, "DirKetStr", !IniFile->KeyExists(sct, "DirKetStr")? "]" : "");

	TimeStampFmt = IniFile->ReadString(sct, "TimeStampFmt",
		!IniFile->KeyExists(sct, "TimeStampFmt")? "yy/mm/dd hh:nn" : "");

	ViewBinLimitSize = IniFile->ReadInteger(sct, "ViewBinLimitSize", FAILED_BUF_SIZE);
	if (ViewBinLimitSize>MAX_MEMMAP_SIZE) ViewBinLimitSize = MAX_MEMMAP_SIZE;

	set_col_from_ColorList();

	//�C�x���g
	for (int i=0; i<MAX_EVENT_CMD; i++)
		*(EventCmdList[i].sp) = IniFile->ReadString("Event", EventCmdList[i].key);

	//�^�C�}�[�C�x���g
	sct.USET_T("TimerEvent");
	for (int i=0; i<MAX_TIMER_EVENT; i++) {
		UnicodeString key;
		Timer_Condition[i] = IniFile->ReadString(sct, key.sprintf(_T("Condition%u"), i + 1));
		OnTimerEvent[i]    = IniFile->ReadString(sct, key.sprintf(_T("OnTimer%u"), i + 1));
		Timer_TimeCnt[i]   = Timer_NopCnt[i] = Timer_RepeatCnt[i] = Timer_RepeatN[i] = 0;
	}
	SetupTimer();
}

//---------------------------------------------------------------------------
//�I�v�V�����ݒ�̕ۑ�
//---------------------------------------------------------------------------
void SaveOptions()
{
	//�f�B���N�g�������̐i�ޕ������폜
	for (int i=0; i<TabList->Count; i++) {
		for (int j=0; j<MAX_FILELIST; j++) {
			TStringList *h_lst = get_DirHistory(i, j);
			if (h_lst) {
				int *h_ptr = get_DirHistPtr(CurTabIndex, j);
				if (h_ptr) {
					for (int k=0; k<*h_ptr && h_lst->Count>0; k++) h_lst->Delete(0);
				}
			}
		}
	}

	UnicodeString sct;
	//�o�^�I�v�V����
	TStringList *lp = OptionList;
	for (int i=0; i<lp->Count; i++) {
		UnicodeString key  = lp->Names[i];
		UnicodeString vbuf = lp->ValueFromIndex[i];
		//Section
		if (remove_top_text(key, _T("S:")))
			IniFile->AssignSection(key, (TStringList*)lp->Objects[i]);
		//List
		else if (remove_top_text(key, _T("L:")))
			IniFile->SaveListItems(key, (TStringList*)lp->Objects[i], get_tkn(vbuf, ',').ToIntDef(20));
		else {
			sct = remove_top_text(key, _T("U:"))? SCT_General :
				  remove_top_text(key, _T("G:"))? UnicodeString("Grep") : SCT_Option;
			if (is_quot(vbuf))
				//UnicodeString
				IniFile->WriteString(sct, key,	*((UnicodeString*)lp->Objects[i]));
			else {
				//bool
				if (contained_wd_i(_T("true|false"), vbuf))
					IniFile->WriteBool(sct, key,	*((bool*)lp->Objects[i]));
				//int
				else
					IniFile->WriteInteger(sct, key,	*((int*)lp->Objects[i]));
			}
		}
	}

	//�^�u���X�g
	for (int i=0; i<MAX_TABLIST; i++) {
		tab_info *tp = get_TabInfo(i);
		for (int j=0; j<MAX_FILELIST; j++) {
			sct.sprintf(_T("DirHistory%02u_%u"), i + 1, j);
			if (tp)
				IniFile->SaveListItems(sct, tp->dir_hist[j], 30);
			else
				IniFile->EraseSection(sct);
		}
	}

	//�S�̃f�B���N�g������
	if (ExtSaveDirHist) {
		std::unique_ptr<TStringList> fbuf(new TStringList());
		UnicodeString s;
		fbuf->Add("[AllDirHistory]");
		for (int i=0; i<AllDirHistory->Count; i++) {
			fbuf->Add(s.sprintf(_T("Item%u=%s"), i + 1, AllDirHistory->Strings[i].c_str()));
		}
		//�ۑ�
		UnicodeString fnam = UAPP_T(ExePath, DIR_HIST_FILE);
		UnicodeString msg  = make_LogHdr(_T("SAVE"), fnam);
		if (saveto_TextUTF8(fnam, fbuf.get())) {
			IniFile->EraseSection("AllDirHistory");
		}
		else {
			UnicodeString errmsg = LoadUsrMsg(USTR_FaildSave, _T("�����t�@�C��"));
			msgbox_ERR(errmsg);
			set_LogErrMsg(msg, errmsg);
		}
		AddLog(msg);
	}
	else {
		IniFile->SaveListItems("AllDirHistory", AllDirHistory, 100);
	}

	//���̑��̃I�v�V����
	sct = SCT_Option;
	IniFile->WriteString( sct, "TempPath",		to_relative_name(TempPathA));
	IniFile->WriteString( sct, "DirBraStr",		DirBraStr);
	IniFile->WriteString( sct, "DirKetStr",		DirKetStr);
	IniFile->WriteString( sct, "TimeStampFmt",	TimeStampFmt);
	IniFile->WriteInteger(sct, "ViewBinLimitSize",	ViewBinLimitSize);

	//�C�x���g
	for (int i=0; i<MAX_EVENT_CMD; i++)
		IniFile->WriteString("Event", EventCmdList[i].key,	*(EventCmdList[i].sp));

	sct.USET_T("TimerEvent");
	for (int i=0; i<MAX_TIMER_EVENT; i++) {
		UnicodeString key;
		IniFile->WriteString(sct, key.sprintf(_T("Condition%u"), i + 1),	Timer_Condition[i]);
		IniFile->WriteString(sct, key.sprintf(_T("OnTimer%u"), i + 1),		OnTimerEvent[i]);
	}
}

//---------------------------------------------------------------------------
//INI�t�@�C�����X�V
//---------------------------------------------------------------------------
void UpdateIniFile()
{
	if (IniFile->Modified) {
		UnicodeString msg = make_LogHdr(_T("SAVE"), IniFile->FileName);
		if (!IniFile->UpdateFile()) {
			UnicodeString errmsg = LoadUsrMsg(USTR_FaildSave, _T("INI�t�@�C��"));
			msgbox_ERR(errmsg);
			set_LogErrMsg(msg, errmsg);
		}
		AddLog(msg);
	}
}

//---------------------------------------------------------------------------
//�^�O�ɐݒ肳��Ă���ϐ��|�C���^��p���ăR���g���[���ɒl��ݒ�
//---------------------------------------------------------------------------
void BringOptionByTag(TForm *fp)
{
	for (int i=0; i<fp->ComponentCount; i++) {
		TComponent *cp = fp->Components[i];  if (cp->Tag==0) continue;
		if (class_is_CheckBox(cp))
			((TCheckBox*)cp)->Checked = *(bool*)cp->Tag;
		else if (class_is_Edit(cp)) {
			if (((TEdit*)cp)->NumbersOnly)
				((TEdit*)cp)->Text = *(int*)cp->Tag;
			else
				((TEdit*)cp)->Text = *(UnicodeString*)cp->Tag;
		}
		else if (class_is_LabeledEdit(cp)) {
			if (((TLabeledEdit*)cp)->NumbersOnly)
				((TLabeledEdit*)cp)->Text = *(int*)cp->Tag;
			else
				((TLabeledEdit*)cp)->Text = *(UnicodeString*)cp->Tag;
		}
		else if (class_is_RadioGroup(cp))
			((TRadioGroup*)cp)->ItemIndex = *(int*)cp->Tag;
		else if (class_is_ComboBox(cp)) {
			if (((TComboBox*)cp)->Style==csDropDown)
				((TComboBox*)cp)->Text = *(UnicodeString*)cp->Tag;
			else
				((TComboBox*)cp)->ItemIndex = *(int*)cp->Tag;
		}
	}
}

//---------------------------------------------------------------------------
//int �^�I�v�V�����̃f�t�H���g�l���擾
//---------------------------------------------------------------------------
int GetOptionIntDef(int tag)
{
	int def = 0;
	int idx = OptionList->IndexOfObject((TObject*)tag);
	if (idx!=-1) {
		UnicodeString vbuf = OptionList->ValueFromIndex[idx];
		if (!is_quot(vbuf)) def = vbuf.ToIntDef(0);
	}
	return def;
}
//---------------------------------------------------------------------------
//�^�O�ɐݒ肳��Ă���ϐ��|�C���^��p���ăR���g���[���̕ύX�𔽉f
//---------------------------------------------------------------------------
void ApplyOptionByTag(TForm *fp)
{
	for (int i=0; i<fp->ComponentCount; i++) {
		TComponent *cp = fp->Components[i];  if (cp->Tag==0) continue;
		if (class_is_CheckBox(cp))
			*(bool*)cp->Tag = ((TCheckBox*)cp)->Checked;
		else if (class_is_Edit(cp)) {
			if (((TEdit*)cp)->NumbersOnly)
				*(int*)cp->Tag = ((TEdit*)cp)->Text.ToIntDef(GetOptionIntDef(cp->Tag));
			else
				*(UnicodeString*)cp->Tag = ((TEdit*)cp)->Text;
		}
		else if (class_is_LabeledEdit(cp)) {
			if (((TLabeledEdit*)cp)->NumbersOnly)
				*(int*)cp->Tag = ((TLabeledEdit*)cp)->Text.ToIntDef(GetOptionIntDef(cp->Tag));
			else
				*(UnicodeString*)cp->Tag = ((TLabeledEdit*)cp)->Text;
		}
		else if (class_is_RadioGroup(cp))
			*(int*)cp->Tag = ((TRadioGroup*)cp)->ItemIndex;
		else if (class_is_ComboBox(cp)) {
			if (((TComboBox*)cp)->Style==csDropDown)
				*(UnicodeString*)cp->Tag = ((TComboBox*)cp)->Text;
			else
				*(int*)cp->Tag = ((TComboBox*)cp)->ItemIndex;
		}
	}
}

//---------------------------------------------------------------------------
//�ꗗ�p�O���b�h�̏�����
//---------------------------------------------------------------------------
void InitializeListGrid(TStringGrid *gp, TFont *fnt)
{
	gp->Color = col_bgList;
	gp->Font->Assign(fnt? fnt : ListFont);
	gp->DefaultRowHeight = get_FontHeight(gp->Font, ListInterLn);
}
//---------------------------------------------------------------------------
//�ꗗ�p�w�b�_�̏�����
//---------------------------------------------------------------------------
void InitializeListHeader(THeaderControl *hp, const _TCHAR *hdr, TFont *fnt)
{
	hp->DoubleBuffered = true;
	hp->Font->Assign(fnt? fnt : LstHdrFont);
	hp->Height = get_FontHeight(hp->Font, 6);

	UnicodeString s = hdr;
	for (int i=0; i<hp->Sections->Count && !s.IsEmpty(); i++)
		hp->Sections->Items[i]->Text = split_tkn(s, '|');
}

//---------------------------------------------------------------------------
//�t�H�[�����̃R���{�{�b�N�X�ɂ��Ď����⊮�̐ݒ��K�p
//---------------------------------------------------------------------------
void set_ComboBox_AutoComp(TForm *frm)
{
	for (int i=0; i<frm->ComponentCount; i++) {
		TComponent *cp = frm->Components[i];
		if (class_is_ComboBox(cp)) ((TComboBox*)cp)->AutoComplete = AutoCompComboBox;
	}
}

//---------------------------------------------------------------------------
//Megemo �I�v�V������ݒ�
//---------------------------------------------------------------------------
void set_MigemoCheckBox(TCheckBox *cp, const _TCHAR *key)
{
	cp->Enabled = usr_Migemo->DictReady;
	cp->Checked = usr_Migemo->DictReady && IniFile->ReadBoolGen(key);
}

//---------------------------------------------------------------------------
//�R���{�{�b�N�X�̌����������ւ�
//---------------------------------------------------------------------------
void change_ComboBoxHistory(TComboBox *cp,
	const _TCHAR *nrm_sct,	//�ʏ�̗����Z�N�V����
	const _TCHAR *reg_sct,	//���K�\���̗����Z�N�V����
	bool reg_sw)			//true = ���K�\��
{
	UnicodeString s = cp->Text;
	IniFile->SaveComboBoxItems(cp, !reg_sw? reg_sct : nrm_sct);
	IniFile->LoadComboBoxItems(cp,  reg_sw? reg_sct : nrm_sct);
	cp->Text = s;
}

//---------------------------------------------------------------------------
//���X�g�̍i�荞��
//Objects ���e���ݒ� (0 �Ȃ� ���X�g�̃C���f�b�N�X��ݒ�)
//---------------------------------------------------------------------------
void filter_List(
	TStringList *i_lst, //�Ώۃ��X�g
	TStringList *o_lst, //���ʃ��X�g
	UnicodeString kwd,	//������
	bool migemo_sw,		//Migemo���g�p
	bool and_or_sw)		//AND/OR����	(default = false)
{
	o_lst->Clear();

	std::unique_ptr<TStringList> ptn_lst(new TStringList());
	if (and_or_sw) {
		TStringDynArray wd_lst = SplitString(Trim(kwd), " ");
		for (int i=0; i<wd_lst.Length; i++) {
			UnicodeString ptn = migemo_sw? usr_Migemo->GetRegExPtn(true, wd_lst[i]) : wd_lst[i];
			if (!ptn.IsEmpty()) ptn_lst->Add(ptn);
		}
	}
	else {
		UnicodeString ptn = migemo_sw? usr_Migemo->GetRegExPtn(true, kwd) : kwd;
		if (!ptn.IsEmpty()) ptn_lst->Add(ptn);
	}

	if (ptn_lst->Count>0) {
		TRegExOptions opt; opt << roIgnoreCase;
		for (int i=0; i<i_lst->Count; i++) {
			UnicodeString lbuf = i_lst->Strings[i];
			bool ok = true;
			if (migemo_sw) {
				for (int j=0; j<ptn_lst->Count && ok; j++)
					ok = TRegEx::IsMatch(lbuf, ptn_lst->Strings[j], opt);
			}
			else {
				for (int j=0; j<ptn_lst->Count && ok; j++) {
					UnicodeString swd = ptn_lst->Strings[j];
					if (and_or_sw) {
						bool or_ok = false;
						do {
							UnicodeString s = split_tkn(swd, '|');
							if (!s.IsEmpty() && ContainsText(lbuf, s)) or_ok = true;
						} while (!or_ok && !swd.IsEmpty());
						ok = or_ok;
					}
					else
						ok = ContainsText(lbuf, swd);
				}
			}

			if (ok) {
				int idx = (int)i_lst->Objects[i];
				if (idx==0) idx = i;
				o_lst->AddObject(lbuf, (TObject*)idx);
			}
		}
	}
}

//---------------------------------------------------------------------------
//�t�@�C���ꗗ�\�[�g�p��r�֐�
//---------------------------------------------------------------------------
int __fastcall CompDirName(file_rec *fp0, file_rec *fp1)
{
	return SameText(fp0->b_name, fp1->b_name) ? CompTextN(fp0->p_name, fp1->p_name)
											  : CompTextN(fp0->b_name, fp1->b_name);
}
//---------------------------------------------------------------------------
int __fastcall CompDirTime(file_rec *fp0, file_rec *fp1)
{
	if (fp0->f_time==fp1->f_time) return CompDirName(fp0, fp1);
	return !OldOrder? ((fp0->f_time<fp1->f_time)? 1 : -1) : ((fp0->f_time>fp1->f_time)? 1 : -1);
}
//---------------------------------------------------------------------------
int __fastcall CompDirSize(file_rec *fp0, file_rec *fp1)
{
	if (fp0->f_size==fp1->f_size) return CompDirName(fp0, fp1);
	return !SmallOrder? ((fp1->f_size > fp0->f_size)? 1 : -1) : ((fp1->f_size < fp0->f_size)? 1 : -1);
}
//---------------------------------------------------------------------------
int __fastcall CompDirAttr(file_rec *fp0, file_rec *fp1)
{
	if (fp0->f_attr==fp1->f_attr) return CompDirName(fp0, fp1);
	return !DscAttrOrder? (fp1->f_attr - fp0->f_attr) : (fp0->f_attr - fp1->f_attr);
}

//---------------------------------------------------------------------------
//���O
int __fastcall SortComp_Name(TStringList *List, int Index1, int Index2)
{
	file_rec *fp0 = (file_rec*)List->Objects[Index1];
	file_rec *fp1 = (file_rec*)List->Objects[Index2];
	if (!fp0 || !fp1) return 0;

	if (fp0->is_up) return -1;
	if (fp1->is_up) return  1;

	int d_mode = (fp0->tag!=-1)? DirSortMode[fp0->tag] : 1;
	if (d_mode!=5) {
		if (fp0->is_dir && fp1->is_dir) {
			switch (d_mode) {
			case 2:  return CompDirTime(fp0, fp1);
			case 3:  return CompDirSize(fp0, fp1);
			case 4:  return CompDirAttr(fp0, fp1);
			default: return CompDirName(fp0, fp1);
			}
		}

		if (fp0->is_dir) return -1;
		if (fp1->is_dir) return  1;
	}

	//�t�@�C�����m
	UnicodeString nam0 = !fp0->alias.IsEmpty()? fp0->alias : fp0->b_name;
	UnicodeString nam1 = !fp1->alias.IsEmpty()? fp1->alias : fp1->b_name;
	int res = SameText(nam0, nam1)? CompTextN(fp0->f_ext, fp1->f_ext) : CompTextN(nam0, nam1);
	return (res==0)? CompTextN(fp0->p_name, fp1->p_name) : res;
}
//---------------------------------------------------------------------------
//�g���q
int __fastcall SortComp_Ext(TStringList *List, int Index1, int Index2)
{
	file_rec *fp0 = (file_rec*)List->Objects[Index1];
	file_rec *fp1 = (file_rec*)List->Objects[Index2];
	if (!fp0 || !fp1) return 0;

	if (fp0->is_up) return -1;
	if (fp1->is_up) return  1;

	int d_mode = (fp0->tag!=-1)? DirSortMode[fp0->tag] : 1;
	if (d_mode!=5) {
		if (fp0->is_dir && fp1->is_dir) {
			switch (d_mode) {
			case 2:  return CompDirTime(fp0, fp1);
			case 3:  return CompDirSize(fp0, fp1);
			case 4:  return CompDirAttr(fp0, fp1);
			default: return CompDirName(fp0, fp1);
			}
		}

		if (fp0->is_dir) return -1;
		if (fp1->is_dir) return  1;
	}

	if (SameText(fp0->f_ext, fp1->f_ext)) return SortComp_Name(List, Index1, Index2);

	return CompTextN(fp0->f_ext, fp1->f_ext);
}
//---------------------------------------------------------------------------
//�^�C���X�^���v
int __fastcall SortComp_Time(TStringList *List, int Index1, int Index2)
{
	file_rec *fp0 = (file_rec*)List->Objects[Index1];
	file_rec *fp1 = (file_rec*)List->Objects[Index2];
	if (!fp0 || !fp1) return 0;

	if (fp0->is_up) return -1;
	if (fp1->is_up) return  1;

	int d_mode = (fp0->tag!=-1)? DirSortMode[fp0->tag] : 2;
	if (d_mode!=5) {
		if (fp0->is_dir && fp1->is_dir) {
			switch (d_mode) {
			case 1:  return CompDirName(fp0, fp1);
			case 3:  return CompDirSize(fp0, fp1);
			case 4:  return CompDirAttr(fp0, fp1);
			default: return CompDirTime(fp0, fp1);
			}
		}

		if (fp0->is_dir) return -1;
		if (fp1->is_dir) return  1;
	}

	if (fp0->f_time==fp1->f_time) return SortComp_Ext(List, Index1, Index2);

	return !OldOrder? ((fp0->f_time<fp1->f_time)? 1 : -1) : ((fp0->f_time>fp1->f_time)? 1 : -1);
}
//---------------------------------------------------------------------------
//�T�C�Y
int __fastcall SortComp_Size(TStringList *List, int Index1, int Index2)
{
	file_rec *fp0 = (file_rec*)List->Objects[Index1];
	file_rec *fp1 = (file_rec*)List->Objects[Index2];
	if (!fp0 || !fp1) return 0;

	if (fp0->is_up) return -1;
	if (fp1->is_up) return  1;

	int d_mode = (fp0->tag!=-1)? DirSortMode[fp0->tag] : 1;
	if (d_mode!=5) {
		if (fp0->is_dir && fp1->is_dir) {
			switch (d_mode) {
			case 1:  return CompDirName(fp0, fp1);
			case 2:  return CompDirTime(fp0, fp1);
			case 4:  return CompDirAttr(fp0, fp1);
			default: return CompDirSize(fp0, fp1);
			}
		}

		if (fp0->is_dir) return -1;
		if (fp1->is_dir) return  1;
	}

	if (fp0->f_size==fp1->f_size) return SortComp_Ext(List, Index1, Index2);

	return !SmallOrder? ((fp1->f_size>fp0->f_size)? 1 : -1) : ((fp1->f_size<fp0->f_size)? 1 : -1);
}
//---------------------------------------------------------------------------
//����
int __fastcall SortComp_Attr(TStringList *List, int Index1, int Index2)
{
	file_rec *fp0 = (file_rec*)List->Objects[Index1];
	file_rec *fp1 = (file_rec*)List->Objects[Index2];
	if (!fp0 || !fp1) return 0;

	if (fp0->is_up) return -1;
	if (fp1->is_up) return  1;

	int d_mode = (fp0->tag!=-1)? DirSortMode[fp0->tag] : 1;
	if (d_mode!=5) {
		if (fp0->is_dir && fp1->is_dir) {
			switch (d_mode) {
			case 1:  return CompDirName(fp0, fp1);
			case 2:  return CompDirTime(fp0, fp1);
			case 3:  return CompDirSize(fp0, fp1);
			default: return CompDirAttr(fp0, fp1);
			}
		}

		if (fp0->is_dir) return -1;
		if (fp1->is_dir) return 1;
	}

	if (fp0->f_attr==fp1->f_attr)	return SortComp_Ext(List, Index1, Index2);

	return !DscAttrOrder? (fp1->f_attr - fp0->f_attr) : (fp0->f_attr - fp1->f_attr);
}

//---------------------------------------------------------------------------
//�f�B���N�g����(�o�^��)
int __fastcall SortComp_PathName(TStringList *List, int Index1, int Index2)
{
	file_rec *fp0 = (file_rec*)List->Objects[Index1];
	file_rec *fp1 = (file_rec*)List->Objects[Index2];
	if (!fp0 || !fp1) return 0;

	UnicodeString pnam0 = fp0->p_name;
	UnicodeString pnam1 = fp1->p_name;

	if (DispRegName) {
		pnam0 = get_RegDirName(pnam0);
		pnam1 = get_RegDirName(pnam1);
	}

	if (fp0->is_up) return -1;
	if (fp1->is_up) return  1;

	int d_mode = (fp0->tag!=-1)? DirSortMode[fp0->tag] : 1;
	if (d_mode!=5) {
		if (fp0->is_dir && fp1->is_dir) {
			switch (d_mode) {
			case 2:  return CompDirTime(fp0, fp1);
			case 3:  return CompDirSize(fp0, fp1);
			case 4:  return CompDirAttr(fp0, fp1);
			default:
				return (SameText(pnam0, pnam1) ? CompTextN2(fp0->b_name, fp1->b_name)
											   : CompTextN2(pnam0, pnam1));
			}
		}

		if (fp0->is_dir) return -1;
		if (fp1->is_dir) return 1;
	}

	//�t�@�C�����m
	if (SameText(pnam0, pnam1)) {
		int res = SameText(fp0->b_name, fp1->b_name) ? CompTextN2(fp0->f_ext, fp1->f_ext)
													 : CompTextN2(fp0->b_name, fp1->b_name);
		return (res==0)? CompTextN2(fp0->p_name, fp1->p_name) : res;
	}

	return CompTextN2(pnam0, pnam1);
}
//---------------------------------------------------------------------------
//�������e
int __fastcall SortComp_Memo(TStringList *List, int Index1, int Index2)
{
	file_rec *fp0 = (file_rec*)List->Objects[Index1];
	file_rec *fp1 = (file_rec*)List->Objects[Index2];
	if (!fp0 || !fp1) return 0;

	UnicodeString memo0 = get_pre_tab(fp0->memo);
	UnicodeString memo1 = get_pre_tab(fp1->memo);

	if (memo0.IsEmpty() && !memo1.IsEmpty()) return  1;
	if (!memo0.IsEmpty() && memo1.IsEmpty()) return -1;

	if ((memo0.IsEmpty() && memo1.IsEmpty()) || SameText(memo0, memo1)) {
		int res = SameText(fp0->b_name, fp1->b_name) ? CompTextN(fp0->f_ext, fp1->f_ext)
												     : CompTextN(fp0->b_name, fp1->b_name);
		return (res==0)? CompTextN(fp0->p_name, fp1->p_name) : res;
	}

	return CompTextN(memo0, memo1);
}
//---------------------------------------------------------------------------
//�}�[�N�ݒ����
int __fastcall SortComp_MarkTime(TStringList *List, int Index1, int Index2)
{
	file_rec *fp0 = (file_rec*)List->Objects[Index1];
	file_rec *fp1 = (file_rec*)List->Objects[Index2];
	if (!fp0 || !fp1) return 0;

	TDateTime dt0 = fp0->f_time;
	TDateTime dt1 = fp1->f_time;

	UnicodeString tstr0 = get_post_tab(fp0->memo);
	if (!tstr0.IsEmpty()) {
		try {
			dt0 = VarToDateTime(tstr0);
		}
		catch (...) {
			;
		}
	}

	UnicodeString tstr1 = get_post_tab(fp1->memo);
	if (!tstr1.IsEmpty()) {
		try {
			dt1 = VarToDateTime(tstr1);
		}
		catch (...) {
			;
		}
	}

	if (dt0==dt1) return SortComp_Ext(List, Index1, Index2);
	if ((int)dt0==0 || (int)dt1==0) return ((int)dt0==0)? 1 : -1;

	return !OldOrder? ((dt0<dt1)? 1 : -1) : ((dt0>dt1)? 1 : -1);
}


//---------------------------------------------------------------------------
//�L�[�ꗗ�\�[�g�p��r�֐�
//---------------------------------------------------------------------------
//�L�[
//---------------------------------------------------------------------------
int __fastcall KeyComp_Key(TStringList *List, int Index1, int Index2)
{
	UnicodeString k0 = List->Names[Index1];
	UnicodeString k1 = List->Names[Index2];

	//�J�e�S���[
	UnicodeString c0, c1;
	if (contains_s(k0, _T(':'))) c0 = split_tkn(k0, ':');
	if (contains_s(k1, _T(':'))) c1 = split_tkn(k1, ':');
	if (!SameText(c0, c1)) return ScrModeIdStr.Pos(c0) - ScrModeIdStr.Pos(c1);

	if (k0.IsEmpty() && k1.IsEmpty())
		return CompareText(List->ValueFromIndex[Index1], List->ValueFromIndex[Index2]);
	if (k0.IsEmpty() && !k1.IsEmpty())	return 1;
	if (!k0.IsEmpty() && k1.IsEmpty())	return -1;
	if ( contains_s(k0, _T('~')) && !contains_s(k1, _T('~')))	return 1;
	if (!contains_s(k0, _T('~')) &&  contains_s(k1, _T('~')))	return -1;

	//�V�t�g
	UnicodeString s0, s1;
	if (remove_text(k0, KeyStr_SELECT))	s0.UCAT_T("S0+");
	if (remove_text(k0, KeyStr_Shift))	s0.UCAT_T("S1+");
	if (remove_text(k0, KeyStr_Ctrl))	s0.UCAT_T("S2+");
	if (remove_text(k0, KeyStr_Alt))	s0.UCAT_T("S3+");
	if (remove_text(k1, KeyStr_SELECT))	s1.UCAT_T("S0+");
	if (remove_text(k1, KeyStr_Shift))	s1.UCAT_T("S1+");
	if (remove_text(k1, KeyStr_Ctrl))	s1.UCAT_T("S2+");
	if (remove_text(k1, KeyStr_Alt))	s1.UCAT_T("S3+");
	if (!SameText(s0, s1)) return CompareStr(s0, s1);

	//2�X�g���[�N
	if (contains_s(k0, _T('~')) && contains_s(k1, _T('~'))) {
		UnicodeString f0 = split_tkn(k0, '~');
		UnicodeString f1 = split_tkn(k1, '~');
		if (!SameText(f0, f1)) return CompareText(f0, f1);
	}

	//�L�[
	if (k0.Length()==1 && k1.Length()==1) return CompareText(k0, k1);

	if (k0.Length()>1 && k1.Length()>1) {
		if (k0[1]=='F' && k1[1]!='F') return -1;
		if (k0[1]!='F' && k1[1]=='F') return 1;
		return StrCmpLogicalW(k0.c_str(), k1.c_str());
	}

	return k0.Length() - k1.Length();
}
//---------------------------------------------------------------------------
//�R�}���h
//---------------------------------------------------------------------------
int __fastcall KeyComp_Cmd(TStringList *List, int Index1, int Index2)
{
	UnicodeString cmd0 = List->ValueFromIndex[Index1];
	UnicodeString cmd1 = List->ValueFromIndex[Index2];
	return !SameText(cmd0, cmd1)? CompareText(cmd0, cmd1) : KeyComp_Key(List, Index1, Index2);
}
//---------------------------------------------------------------------------
//����
//---------------------------------------------------------------------------
int __fastcall KeyComp_Dsc(TStringList *List, int Index1, int Index2)
{
	UnicodeString dsc0 = get_CmdDesc(List->ValueFromIndex[Index1], true);
	UnicodeString dsc1 = get_CmdDesc(List->ValueFromIndex[Index2], true);
	return !SameText(dsc0, dsc1)? CompareText(dsc0, dsc1) : KeyComp_Key(List, Index1, Index2);
}

//---------------------------------------------------------------------------
//.nyanfi �����擾
//---------------------------------------------------------------------------
UnicodeString get_dotNaynfi(UnicodeString dnam)
{
	UnicodeString fnam;
	if (!dnam.IsEmpty()) fnam = IncludeTrailingPathDelimiter(dnam);
	fnam.UCAT_T(".nyanfi");
	if (DotNyanPerUser && !UserName.IsEmpty()) fnam.cat_sprintf(_T("_%s"), UserName.c_str());
	return fnam;
}
//---------------------------------------------------------------------------
UnicodeString get_dotNaynfi(UnicodeString dnam,
	bool inherit,	//��ʂ���p��
	bool force)		//�J�����g�𖳎����ċ����I�Ɍp��
{
	if (force) dnam = IncludeTrailingPathDelimiter(get_parent_path(dnam));

	UnicodeString fnam = get_dotNaynfi(dnam);
	while (!file_exists(fnam) && inherit) {
		if (is_root_dir(dnam)) break;
		dnam = IncludeTrailingPathDelimiter(get_parent_path(dnam));
		fnam = get_dotNaynfi(dnam);
	}
	return fnam;
}

//---------------------------------------------------------------------------
//Web�����\����������擾
//---------------------------------------------------------------------------
UnicodeString get_WebSeaCaption()
{
	UnicodeString ret_str = get_tkn_m(WebSeaUrl, _T("//"), _T("/"));
	remove_top_text(ret_str, _T("www."));
	if (ret_str.IsEmpty()) ret_str = "Web";

	return ret_str.UCAT_T(" �Ō���");
}

//---------------------------------------------------------------------------
//�ŐV�̃^�C���X�^���v���H
//---------------------------------------------------------------------------
bool is_NewerTime(TDateTime scr_t, TDateTime dst_t)
{
	return (!WithinPastMilliSeconds(scr_t, dst_t, TimeTolerance) && scr_t>dst_t);
}

//---------------------------------------------------------------------------
//UNC�p�X�̑��݃`�F�b�N
//  ESC �Œ��f�\(�X���b�h�Ń`�F�b�N)
//�߂�l: true	= ���݂��� or ��UNC�p�X
//		  false = ���݂��Ȃ� or ������ UNC�p�X
//---------------------------------------------------------------------------
bool check_if_unc(UnicodeString pnam)
{
	UnicodeString tmp = pnam;
	if (!remove_top_s(tmp, "\\\\")) return true;

	TStringDynArray plst = SplitString(tmp, "\\");
	if (plst.Length<3) return false;

	TCheckPathThread *tp = new TCheckPathThread(true);
	tp->PathName = pnam;
	tp->Start();
	while (!tp->PathName.IsEmpty()) {
		if (is_KeyDown(VK_ESCAPE)) {
			ClearKeyBuff(true);
			break;
		}
		Sleep(100);
	}

	bool ok = tp->isOk;
	tp->Terminate();
	return ok;
}

//---------------------------------------------------------------------------
//�����񂩂�R�}���h�t�@�C�������擾
// �擪�� @ �͍폜���A��΃p�X��
// .nbt �łȂ���� EmptyStr ��Ԃ�
//---------------------------------------------------------------------------
UnicodeString get_cmdfile(UnicodeString s)
{
	if (!remove_top_AT(s) && !test_NbtExt(get_extension(s))) return EmptyStr; 
	return rel_to_absdir(s);
}

//---------------------------------------------------------------------------
//ExeCommands �̃p�����[�^����t�@�C�������擾
//---------------------------------------------------------------------------
UnicodeString get_cmds_prm_file(UnicodeString prm)
{
	if (remove_top_AT(prm))	return rel_to_absdir(prm);
	if (remove_top_text(prm, _T("ExeMenuFile_")))
							return rel_to_absdir(exclude_quot(prm));
	return EmptyStr;
}

//---------------------------------------------------------------------------
//�K�v�ȏꍇ���X�|���X�t�@�C�����쐬
//  �߂�l: ���X�|���X�t�@�C����
//			�s�v�̏ꍇ�� EmptyStr�A�G���[�̏ꍇ�� RESPONSE_ERR
//---------------------------------------------------------------------------
UnicodeString make_ResponseFile(TStringList *lst,
	int arc_t,				//�A�[�J�C�u�^�C�v
	UnicodeString *files,	//[o] �t�@�C�������X�g (default = NULL)
	bool excl)				//true: ���X�|���X�t�@�C��/���X�g�̈���݂̂��쐬
							// (default = false: ���X�|���X�t�@�C��/���X�g�ɕ��U)
{
	std::unique_ptr<TStringList> r_lst(new TStringList());
	std::unique_ptr<TStringList> f_lst(new TStringList());

	//�����A�[�J�C�o�ȊO (SPI)
	if (arc_t==0) {
		for (int i=0; i<lst->Count; i++) r_lst->Add(add_quot_if_spc(lst->Strings[i]));
	}
	//�����A�[�J�C�o
	else {
		if (arc_t==UARCTYP_CAB && lst->Count>250) {
			for (int i=0; i<lst->Count; i++) r_lst->Add(add_quot_if_spc(lst->Strings[i]));
		}
		else {
			for (int i=0; i<lst->Count; i++) {
				UnicodeString fnam = lst->Strings[i];
				if ((arc_t!=UARCTYP_RAR && starts_AT(fnam)) || (arc_t==UARCTYP_CAB && USTARTS_TS('-', fnam)))
					r_lst->Add(add_quot_if_spc(fnam));
				else {
					//��unrar32.dll �̃o�O? �΍�
					if (starts_AT(fnam)) fnam = "?" + exclude_top(fnam);
					f_lst->Add(add_quot_if_spc(fnam));
				}
			}
		}

		//���X�|���X�t�@�C���̂�
		if ((excl || !files) && r_lst->Count>0 && f_lst->Count>0) {
			r_lst->Clear();
			f_lst->Clear();
			for (int i=0; i<lst->Count; i++) r_lst->Add(add_quot_if_spc(lst->Strings[i]));
		}
		//���X�g
		if (files && f_lst->Count>0) {
			for (int i=0; i<f_lst->Count; i++) {
				if (i>0) *files += " ";
				*files += f_lst->Strings[i];
			}
		}
	}

	UnicodeString res_file;
	if (r_lst->Count>0) {
		res_file.sprintf(_T("%s" RESPONSE_FILE), TempPathA.c_str());
		//�t�@�C���ۑ�
		try {
			if (usr_ARC->IsUnicode(arc_t))
				r_lst->SaveToFile(res_file, TEncoding::UTF8);
			else
				r_lst->SaveToFile(res_file);
		}
		catch (...) {
			res_file = RESPONSE_ERR;
		}
	}

	return res_file;
}
//---------------------------------------------------------------------------
UnicodeString make_ResponseFile(UnicodeString fnam, int arc_t)
{
	std::unique_ptr<TStringList> lst(new TStringList());
	lst->Add(fnam);
	return make_ResponseFile(lst.get(), arc_t);
}

//---------------------------------------------------------------------------
//�N���[����/�����������̉��������������W�J
//---------------------------------------------------------------------------
UnicodeString format_CloneName(
	UnicodeString fmt,		//����
	UnicodeString fnam,		//�t�@�C����
	UnicodeString dst_dir,	//�쐬/�R�s�[��f�B���N�g��
	bool is_dir)			//�Ώۂ̓f�B���N�g���[��? (default = false)
{
	if (fmt.IsEmpty()) fmt = FMT_AUTO_REN;

	dst_dir = IncludeTrailingPathDelimiter(dst_dir);
	UnicodeString bnam = !is_dir? get_base_name(fnam) : ExtractFileName(fnam);
	UnicodeString fext = !is_dir? get_extension(fnam) : EmptyStr;

	UnicodeString ret_str, last_str;
	int sn = 0;
	for (int i=0; ; i++) {
		ret_str = dst_dir;
		UnicodeString fmt_str = (i==0)? get_tkn(fmt, _T("\\-")) : REPLACE_TS(fmt, "\\-", "");

		UnicodeString tmp = fmt_str;
		while (!tmp.IsEmpty()) {
			WideChar c = split_top_wch(tmp);
			if (c=='\\') {
				//�A��
				if (USTARTS_TS("SN(", tmp)) {
					UnicodeString nstr = split_in_paren(tmp);
					if (nstr.IsEmpty()) nstr = "1";
					ret_str.cat_sprintf(_T("%0*u"), nstr.Length(), nstr.ToIntDef(0) + sn);
				}
				//����
				else if (USTARTS_TS("DT(", tmp) || USTARTS_TS("TS(", tmp)) {
					TDateTime dt = USTARTS_TS("TS(", tmp)? get_file_age(fnam) : Now();
					ret_str += FormatDateTime(split_in_paren(tmp), dt);
				}
				else {
					c = split_top_wch(tmp);
					//�t�@�C�����啔�܂��̓f�B���N�g����
					if (c=='N') ret_str += bnam;
				}
			}
			else ret_str.cat_sprintf(_T("%c"), c);
		}

		ret_str += fext;
		if (!file_exists(ret_str)) break;

		//�d���ɂ��Ď��s
		if (contains_s(fmt_str, _T("\\SN(")))
			sn++;
		else if (SameText(last_str, ret_str))
			fmt += SFX_AUTOREN;

		last_str = ret_str;
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//�����������W�J
//  \L(n)    �t�@�C�����̍�����n����
//  \S(m,n)  �t�@�C������m�Ԗڂ���n����
//  \R(n)    �t�@�C�����̉E����n����
//  \A       �t�@�C�����S��
//  \E       �g���q
//  \TS(�c)  �^�C���X�^���v
//  \XT(�c)  Exit�B�e����
//  \\       \ ���̂���
//---------------------------------------------------------------------------
UnicodeString format_FileName(UnicodeString fmt, UnicodeString fnam)
{
	UnicodeString ret_str;
	UnicodeString bnam = get_base_name(fnam);
	UnicodeString fext = get_extension(fnam);	remove_top_s(fext, '.');

	int i = 1;
	while (i<=fmt.Length()) {
		WideChar c = fmt[i];
		if (c=='\\') {
			UnicodeString s = fmt.SubString(i, 512);
			if (s.Pos(')')) s = get_tkn(s, ')').UCAT_T(")");

			if (USTARTS_TS("\\L(", s)) {
				int n = get_in_paren(s).ToIntDef(0);
				if (n>0) ret_str += bnam.SubString(1, n);
			}
			else if (USTARTS_TS("\\S(", s)) {
				int m = get_tkn_m(s, '(', ',').ToIntDef(0);
				int n = get_tkn_m(s, ',', ')').ToIntDef(0);
				if (n>0 && m>0) ret_str += bnam.SubString(m, n);
			}
			else if (USTARTS_TS("\\R(", s)) {
				int n = get_in_paren(s).ToIntDef(0);
				if (n>0) ret_str += bnam.SubString(bnam.Length() - n + 1, n);
			}
			else if (USTARTS_TS("\\TS(", s)) {
				ret_str += FormatDateTime(get_in_paren(s), get_file_age(fnam));
			}
			else if (USTARTS_TS("\\XT(", s)) {
				ret_str += EXIF_GetExifTimeStr(fnam, get_in_paren(s));
			}
			else {
				s = fmt.SubString(i, 2);
				if		(USTARTS_TS("\\A", s)) ret_str += bnam;
				else if (USTARTS_TS("\\E", s)) ret_str += fext;
				else						   ret_str += s.SubString(2, 1);
			}
			i += s.Length();
		}
		else {
			ret_str += c;
			i++;
		}
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//�Ή�����\�[�X�^�w�b�_�t�@�C�������擾
//---------------------------------------------------------------------------
UnicodeString get_SrcHdrName(UnicodeString fnam)
{
	UnicodeString fext = get_extension(fnam);
	if      (test_FileExt(fext, FEXT_C_SRC)) fext = FEXT_C_HDR;
	else if (test_FileExt(fext, FEXT_C_HDR)) fext = FEXT_C_SRC;
	else return EmptyStr;

	TStringDynArray x_lst = SplitString(fext, ".");
	bool found = false;
	for (int i=0; i<x_lst.Length && !found; i++) {
		if (x_lst[i].IsEmpty()) continue;
		fnam  = ChangeFileExt(fnam, "." + x_lst[i]);
		found = FileExists(fnam);
	}

	return found? fnam : EmptyStr;
}

//---------------------------------------------------------------------------
//�t�@�C�����啔���������̃t�@�C�������擾
//---------------------------------------------------------------------------
UnicodeString get_NextSameName(
	UnicodeString fnam,		//�t�@�C����
	bool only_text)			//true = �e�L�X�g�̂� (default = false)
{
	std::unique_ptr<TStringList> l_lst(new TStringList());
	get_files(ExtractFilePath(fnam), get_base_name(fnam).UCAT_T(".*").c_str(), l_lst.get());
	if (l_lst->Count<2) return EmptyStr;

	int idx = l_lst->IndexOf(fnam);
	if (idx==-1) return EmptyStr;

	int idx0 = -1, idx1 = -1;
	for (int i=0; i<l_lst->Count && idx1==-1; i++) {
		if (i<=idx && idx0!=-1) continue;
		UnicodeString nnam = l_lst->Strings[i];
		if (dir_exists(nnam)) continue;
		if (only_text && !is_TextFile(nnam)) continue;
		if (i<=idx) idx0 = i; else idx1 = i;
	}
	idx = (idx1!=-1)? idx1 : idx0;

	return (idx!=-1)? l_lst->Strings[idx] : EmptyStr;
}
//---------------------------------------------------------------------------
UnicodeString get_NextSameName(
	TStringList *lst,
	int idx,
	bool only_text)			//true = �e�L�X�g�̂� (default = false)
{
	if (idx<0 || idx>=lst->Count) return EmptyStr;
	UnicodeString bnam = get_base_name(lst->Strings[idx]);

	int idx0 = -1, idx1 = -1;
	for (int i=0; i<lst->Count && idx1==-1; i++) {
		if (i<=idx && idx0!=-1) continue;
		file_rec *fp = (file_rec*)lst->Objects[i];
		if (fp->is_dir || fp->is_dummy) continue;
		if (!SameText(fp->b_name, bnam)) continue;
		if (only_text && !is_TextFile(fp->f_name)) continue;
		if (i<=idx) idx0 = i; else idx1 = i;
	}
	idx = (idx1!=-1)? idx1 : idx0;

	return (idx!=-1)? lst->Strings[idx] : EmptyStr;
}

//---------------------------------------------------------------------------
//���C�u�������X�g�̎擾
//---------------------------------------------------------------------------
void get_LibraryList(
	UnicodeString fnam,		//���C�u�����t�@�C����(������ \ �Ȃ烉�C�u�����p�X)
	TStringList *lst,
	bool get_info)			//�t�@�C�����Ƃ��Ď擾 (default = false)
{
	try {
		std::unique_ptr<TStringList> l_lst(new TStringList());
		if (ends_PathDlmtr(fnam))
			get_files(fnam, _T("*.library-ms"), l_lst.get());
		else {
			if (!file_exists(fnam)) Abort();
			l_lst->Text = fnam;
		}

		int l_cnt = 0;
		for (int l_i=0; l_i<l_lst->Count; l_i++) {
			std::unique_ptr<TStringList> f_buf(new TStringList());
			load_text_ex(l_lst->Strings[l_i], f_buf.get());
			int tag = 0;
			for (int i=0; i<f_buf->Count; i++) {
				UnicodeString lbuf = Trim(f_buf->Strings[i]);
				switch (tag) {
				case 0:
					if (contains_i(lbuf, _T("<searchConnectorDescriptionList>"))) tag = 1;
					break;
				case 1:
					if (contains_i(lbuf, _T("<searchConnectorDescription"))) tag = 2;
					break;
				case 2:
					if (contains_i(lbuf, _T("<url>")) && contains_i(lbuf, _T("</url>"))) {
						UnicodeString url = get_tkn_m(lbuf, _T("<url>"), _T("</url>"));
						if (USTARTS_TI("knownfolder:", url))
							url = usr_SH->KnownGuidStrToPath(get_tkn_r(url, ':'));
						if (!url.IsEmpty()) {
							if (l_cnt==0 && get_info) lst->Add(EmptyStr);	//�Z�p���[�^
							l_cnt++;
							if (get_info)
								add_PropLine(UnicodeString("�ꏊ").cat_sprintf(_T("%u"), l_cnt), url, lst);
							else
								lst->Add(url);
						}
						tag = 1;
					}
					break;
				}
			}
		}
		if (l_cnt>0 && get_info) lst->Add(EmptyStr);	//�Z�p���[�^
	}
	catch (...) {
		;
	}
}

//---------------------------------------------------------------------------
//�J�����g�����C�u�����o�^�f�B���N�g���̃��[�g�Ȃ烉�C�u�����t�@�C����Ԃ�
//---------------------------------------------------------------------------
UnicodeString get_LibFile_if_root()
{
	if (CurStt->LibraryInfo.IsEmpty()) return EmptyStr;

	UnicodeString dnam = CurStt->LibraryInfo;
	UnicodeString lnam = split_pre_tab(dnam);
	if (SameText(IncludeTrailingPathDelimiter(dnam), CurPathName)) {
		CurStt->LibraryInfo = CurStt->LibSubPath = EmptyStr;
		return lnam;
	}
	else
		return EmptyStr;
}

//---------------------------------------------------------------------------
//�����Ώۃ��X�g���擾
//  �߂�l: �I�v�V����������(O,D)
//---------------------------------------------------------------------------
UnicodeString get_SyncDstList(
	UnicodeString dnam,		//�f�B���N�g����
	TStringList *lst,
	bool del_sw, 			//�����폜			(default = false)
	UnicodeString ex_dnam)	//���O�f�B���N�g��	(default = EmptyStr)
{
	dnam = IncludeTrailingPathDelimiter(dnam);

	lst->Clear();
	lst->Add(dnam);

	UnicodeString opt;
	for (int i=0; i<SyncDirList->Count; i++) {
		//"�^�C�g��","�L��:1/����:0","�I�v�V����","dir1","dir2",...
		TStringDynArray syn_lst = get_csv_array(SyncDirList->Strings[i], 50);	//***
		if (syn_lst.Length<5)		continue;	//�s��
		if (equal_0(syn_lst[1]))	continue;	//����
		if (del_sw && !ContainsText(syn_lst[2], "D")) continue;
		//�����Ώۂ����邩?
		for (int j=3; j<syn_lst.Length; j++) syn_lst[j] = IncludeTrailingPathDelimiter(syn_lst[j]);
		bool flag = false;
		UnicodeString snam;	//�T�u�f�B���N�g������
		for (int j=3; j<syn_lst.Length; j++) {
			if (StartsText(syn_lst[j], dnam)) {
				snam = dnam;
				snam.Delete(1, syn_lst[j].Length());
				flag = true;
				break;
			}
		}
		if (!flag) continue;

		//�����Ώۂ���
		opt = syn_lst[2];
		for (int j=3; j<syn_lst.Length; j++) {
			UnicodeString pnam = syn_lst[j] + snam;
			if (!SameText(dnam, pnam) && !SameText(ex_dnam, pnam) && dir_exists(pnam)) lst->Add(pnam);
		}
		break;
	}

	return opt;
}

//---------------------------------------------------------------------------
//�������ׂ��f�B���N�g�������邩?
//---------------------------------------------------------------------------
bool has_SyncDir(
	UnicodeString dnam,
	bool del_sw)			//�����폜	(default = false)
{
	std::unique_ptr<TStringList> d_lst(new TStringList());
	get_SyncDstList(dnam, d_lst.get(), del_sw);
	return (d_lst->Count>1);
}

//---------------------------------------------------------------------------
//�f�B���N�g�����݂��ɓ����֌W�ɂ��邩?
//  �߂�l: S | [1][2]  S=����, 1=dnam1���o�^, 2=dnam2���o�^
//---------------------------------------------------------------------------
UnicodeString is_SyncDir(UnicodeString dnam1, UnicodeString dnam2)
{
	UnicodeString ret_str;
	dnam1 = IncludeTrailingPathDelimiter(dnam1);
	dnam2 = IncludeTrailingPathDelimiter(dnam2);

	for (int i=0; i<SyncDirList->Count; i++) {
		//"�^�C�g��","�L��:1/����:0","�I�v�V����","dir1","dir2",...
		TStringDynArray syn_lst = get_csv_array(SyncDirList->Strings[i], 50);
		if (syn_lst.Length<5 || equal_0(syn_lst[1])) continue;

		for (int j=3; j<syn_lst.Length; j++) syn_lst[j] = IncludeTrailingPathDelimiter(syn_lst[j]);
		bool flag1 = false, flag2 = false;
		UnicodeString snam1, snam2;	//�T�u�f�B���N�g������
		for (int j=3; j<syn_lst.Length; j++) {
			UnicodeString inam = syn_lst[j];
			if (StartsText(inam, dnam1)) {
				snam1 = dnam1;  snam1.Delete(1, inam.Length());  flag1 = true;
			}
			if (StartsText(inam, dnam2)) {
				snam2 = dnam2;  snam2.Delete(1, inam.Length());  flag2 = true;
			}
		}

		if (flag1 || flag2) {
			if (flag1 && flag2 && !SameText(dnam1, dnam2) && SameText(snam1, snam2)) {
				ret_str.USET_T("S");  break;
			}
			if (flag1) ret_str.UCAT_T("1");
			if (flag2) ret_str.UCAT_T("2");
		}
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//�ꎞ�f�B���N�g�����N���A
//---------------------------------------------------------------------------
void ClearTempDir(UnicodeString dnam)
{
	if (!is_EmptyDir(TempPathA + dnam)) {
		std::unique_ptr<TStringList> fbuf(new TStringList());
		get_files(TempPathA + dnam, _T("*.*"), fbuf.get(), true);
		for (int i=0; i<fbuf->Count; i++) {
			UnicodeString fnam = fbuf->Strings[i];
			if (set_FileWritable(fnam)) {
				DeleteFile(fnam);
				del_CachedIcon(fnam);
			}
		}
	}
}
//---------------------------------------------------------------------------
//�A�[�J�C�u�p�̈ꎞ�f�B���N�g�����폜
//---------------------------------------------------------------------------
void ClearTempArc(
	UnicodeString dnam)		//�f�B���N�g���� (default = EmptyStr : ���ׂč폜)
{
	UnicodeString sea_str = dnam.IsEmpty()? (TempPathA + TMP_ARC_P) : ExcludeTrailingPathDelimiter(dnam);
	TSearchRec sr;
	if (FindFirst(sea_str, faAnyFile, sr)==0) {
		do {
			if ((sr.Attr & faDirectory)==0) continue;
			UnicodeString snam = TempPathA + UnicodeString(sr.Name);
			std::unique_ptr<TStringList> fbuf(new TStringList());
			get_files(snam, _T("*.*"), fbuf.get(), true);
			for (int i=0; i<fbuf->Count; i++) {
				UnicodeString fnam = fbuf->Strings[i];
				if (set_FileWritable(fnam)) {
					DeleteFile(fnam);
					del_CachedIcon(fnam);
				}
			}
			delete_Dirs(snam);
		} while(FindNext(sr)==0);
		FindClose(sr);
	}
}
//---------------------------------------------------------------------------
void ClearTempArcList(int tag)
{
	flist_stt *lst_stt = &ListStt[tag];
	int i = 0;
	while (i < lst_stt->arc_TmpList->Count) {
		UnicodeString dnam = lst_stt->arc_TmpList->Strings[i];
		ClearTempArc(dnam);
		if (!dir_exists(dnam)) lst_stt->arc_TmpList->Delete(i); else i++;
	}
}
//---------------------------------------------------------------------------
//�A�[�J�C�u�p�̈ꎞ�f�B���N�g�����X�V
//  tag �w��̏ꍇ�Aarc_TmpList �ɓo�^
//---------------------------------------------------------------------------
UnicodeString UpdateTempArcList(int tag)
{
	//�����̈ꎞ�f�B���N�g�����폜
	if (tag!=-1) ClearTempArcList(tag);

	//�V�K�ꎞ�f�B���N�g����ǉ�
	int n = 0;
	while (file_exists(TempPathA + UnicodeString().sprintf(_T(TMP_ARC_F), n))) n++;
	UnicodeString tmp_path = TempPathA + UnicodeString().sprintf(_T(TMP_ARC_F), n) + "\\";
	if (create_ForceDirs(tmp_path)) {
		if (tag==0 || tag==1) {
			flist_stt *lst_stt = &ListStt[tag];
			lst_stt->arc_TmpList->Insert(0, tmp_path);
		}
		return tmp_path;
	}
	else
		return EmptyStr;
}

//---------------------------------------------------------------------------
//�A�[�J�C�u���t�@�C���̈ꎞ�𓀏���
//  fp->tmp_name �Ɉꎞ�t�@�C������ݒ�
//---------------------------------------------------------------------------
bool SetTmpFile(
	file_rec *fp,
	bool not_unpk,		//�𓀂����Ɉꎞ�t�@�C������ݒ肷�邾�� (default = false)
	bool show_prg)		//�i���󋵂�\�� (default = false)
{
	if (!fp || fp->is_dir || !fp->is_virtual || fp->arc_name.IsEmpty()) return false;

	int tag = fp->tag;
	if (tag==-1) tag = CurListTag;
	flist_stt *lst_stt = &ListStt[tag];

	try {
		UnicodeString tmp_path;
		bool is_v_item = contains_Slash(fp->f_name);
		if (!file_exists(fp->tmp_name) || is_v_item) {
			//�A�[�J�C�u������
			if (is_v_item) {
				if (!not_unpk) UpdateTempArcList(tag);
				tmp_path = lst_stt->arc_TmpList->Strings[0];
			}
			//���d�A�[�J�C�u
			else if (test_ArcAxExt(fp->f_ext)) {
				tmp_path = UpdateTempArcList();
				if (tmp_path.IsEmpty()) Abort();
			}
			//���̑�
			else
				tmp_path = lst_stt->arc_TmpList->Strings[0];

			UnicodeString tmp_name;
			tmp_name.sprintf(_T("%s%s%s"), tmp_path.c_str(), fp->b_name.c_str(), fp->f_ext.c_str());

			if (!not_unpk && !file_exists(tmp_name)) {
				fp->tmp_name = EmptyStr;
				if (SPI->TestFExt(get_extension(fp->arc_name), true)) {
					if (!SPI->UnPack(fp->arc_name, fp->f_name, tmp_path)) Abort();
				}
				else {
					UnicodeString fnam = fp->f_name;
					if (contains_Slash(fnam)) fnam = get_tkn_r(fnam, '/');
					UnicodeString res_file = make_ResponseFile(fnam, usr_ARC->GetArcType(fp->arc_name));
					if (res_file==RESPONSE_ERR) Abort();
					if (!res_file.IsEmpty()) fnam = "@" + res_file;
					bool ok = usr_ARC->UnPack(fp->arc_name, tmp_path, add_quot_if_spc(fnam), false, !show_prg);
					delete_FileIf(res_file);
					if (!ok) {
						UnicodeString msg = make_LogHdr(_T("UNPACK"), fnam);
						msg[1] = 'E';
						set_LogErrMsg(msg, LoadUsrMsg(USTR_FaildTmpUnpack));
						msg += usr_ARC->ErrMsg;
						AddLog(msg);
						Abort();
					}
				}
			}
			fp->tmp_name = tmp_name;
		}
		return true;
	}
	catch (...) {
		return false;
	}
}

//---------------------------------------------------------------------------
//ZIP �A�[�J�C�u���̓K���ȉ摜�������ĉ�
//---------------------------------------------------------------------------
UnicodeString ExtractInZipImg(UnicodeString arc_file)
{
	if (!test_FileExt(get_extension(arc_file), FEXT_ZIPIMG)) return EmptyStr;

	UnicodeString i_fnam;
	std::unique_ptr<TZipFile> zp(new TZipFile());

	try {
		zp->Open(arc_file, zmRead);

		UnicodeString i_cover, i_first;
		for (int i=0; i<zp->FileCount; i++) {
			UnicodeString fnam = zp->FileName[i];
			UnicodeString fext = get_extension(fnam);
			if (test_FileExt(fext, FEXT_WICSTD + WicFextStr) || SPI->TestFExt(fext)) {
				if (i_first.IsEmpty()) i_first = fnam;
				if (i_cover.IsEmpty() && (contains_i(fnam, _T("cover")) || contains_i(fnam, _T("title")) || contains_i(fnam, _T("page"))))
					i_cover = fnam;
			}
		}

		UnicodeString znam = !i_cover.IsEmpty()? i_cover : i_first;
		if (!znam.IsEmpty()) {
			i_fnam = TempPathA + ExtractFileName(REPLACE_TS(znam, "/", "\\"));
			zp->Extract(znam, TempPathA, false);
		}
	}
	catch (...) {
		if (file_exists(i_fnam)) DeleteFile(i_fnam);
		i_fnam = EmptyStr;
	}

	zp->Close();

	return i_fnam;
}

//---------------------------------------------------------------------------
//���k�Ώۃ��X�g�ɍ��ڂ�ǉ�
//---------------------------------------------------------------------------
void add_PackItem(file_rec *fp, int arc_t, UnicodeString src_dir, TStringList *lst)
{
	UnicodeString fnam = fp->n_name;
	if (fp->is_dir) {
		if (arc_t==UARCTYP_CAB) {
			//cab32.dll �� -r �̑�֍�
			std::unique_ptr<TStringList> fbuf(new TStringList());
			get_files(fp->f_name, _T("*.*"), fbuf.get(), true);
			for (int i=0; i<fbuf->Count; i++) {
				UnicodeString lbuf = fbuf->Strings[i];
				lst->Add(lbuf.Delete(1, src_dir.Length()));
			}
		}
		else {
			//�J�����g�̋�f�B���N�g���̓}�X�N��t���Ȃ�
			if (!is_EmptyDir(fp->f_name)) fnam.UCAT_T("\\*");
			lst->Add(fnam);
		}
	}
	else lst->Add(fnam);
}

//---------------------------------------------------------------------------
//������UNC�p�X��?
//---------------------------------------------------------------------------
bool is_InvalidUnc(UnicodeString dnam, 
	bool del_sw)	//�����������ڂ��폜 (default = false);
{
	int idx = -1;
	if (USTARTS_TS("\\\\", dnam)) {
		idx = InvalidUncList->IndexOf(dnam);
		if (idx!=-1 && del_sw) InvalidUncList->Delete(idx);
	}

	return (idx != -1);
}

//---------------------------------------------------------------------------
//�p�X�̑��݂��`�F�b�N���A������Η��p�\�ȃp�X���擾
//---------------------------------------------------------------------------
UnicodeString CheckAvailablePath(UnicodeString dnam, int tag)
{
	//�f�B���N�g���̑��݃`�F�b�N(�Ȃ���ΐe������)
	UnicodeString drvnam = ExtractFileDrive(dnam);
	if (!dnam.IsEmpty()) {
		drvnam = ExtractFileDrive(dnam);

		if (is_InvalidUnc(dnam)) dnam = EmptyStr;

		if (USTARTS_TS("\\\\", dnam) || is_drive_accessible(drvnam)) {
			bool found = true;
			while (!dir_exists(dnam)) {
				dnam = get_parent_path(dnam);
				if (!contains_PathDlmtr(dnam) && !dir_exists(dnam)) {
					found = false;  break;
				}
			}
			if (found) return dnam;
		}

		//���݂��Ȃ��h���C�u�̗����E�X�^�b�N���ڂ��폜
		for (int i=0; i<MAX_FILELIST+1; i++) {
			TStringList *h_lst = (i<MAX_FILELIST)? get_DirHistory(CurTabIndex, i) : DirStack;
			if (h_lst) {
				int idx = 0;
				while (idx<h_lst->Count)
					if (StartsText(drvnam, get_csv_item(h_lst->Strings[idx], 0))) h_lst->Delete(idx); else idx++;
			}
		}
	}

	//��������T��
	TStringList *h_lst = get_DirHistory(CurTabIndex, tag);
	if (h_lst && h_lst->Count>0)
		dnam = get_csv_item(h_lst->Strings[0], 0);
	//���݂���h���C�u����T��
	else {
		dnam = EmptyStr;
		DWORD d_bit  = ::GetLogicalDrives();	//���p�\�ȃh���C�u���r�b�g�}�X�N�`���Ŏ擾
		DWORD d_flag = 0x00000001;
		for (int d_n = _T('A'); d_n <= _T('Z'); d_n++,d_flag<<=1 ){
			if ((d_bit & d_flag)==0) continue;
			drvnam.sprintf(_T("%c:\\"), d_n);
			if (::GetDriveType(drvnam.c_str())==DRIVE_FIXED) {
				dnam = drvnam;  break;
			}
		}
	}
	return dnam;
}

//---------------------------------------------------------------------------
//���݂̃c�[���{�^����`���X�g���擾
//---------------------------------------------------------------------------
TStringList *GetCurBtnList(int scr_mode)
{
	return (scr_mode==SCMD_FLIST)? ToolBtnList :
		   (scr_mode==SCMD_TVIEW)? ToolBtnListV :
		   (scr_mode==SCMD_IVIEW)? ToolBtnListI : NULL;
}

//---------------------------------------------------------------------------
//�t�@�C�����X�g���擾
//---------------------------------------------------------------------------
TStringList* GetFileList(int tag)
{
	flist_stt *cur_stt = &ListStt[tag];
	return cur_stt->is_Arc?  ArcFileList[tag] :
		   cur_stt->is_Find? ResultList[tag] :
		   cur_stt->is_Work? WorkList: FileList[tag];
}
//---------------------------------------------------------------------------
//�J�����g�̃��X�g���擾
//---------------------------------------------------------------------------
TStringList* GetCurList(
	bool only_filer)	//�t�@�C���[�݂̂�Ώ� (default = false)
{
	return ((!only_filer && ScrMode==SCMD_IVIEW)? ViewFileList : GetFileList(CurListTag));
}
//---------------------------------------------------------------------------
//���΃p�X�̃��X�g���擾
//---------------------------------------------------------------------------
TStringList* GetOppList()
{
	return GetFileList(OppListTag);
}

//---------------------------------------------------------------------------
//�t�@�C�����X�g�̔w�i�F���擾
//---------------------------------------------------------------------------
TColor get_FlBgColor(flist_stt *lst_stt, int idx)
{
	return lst_stt->is_Find? col_bgFind : lst_stt->is_Arc? col_bgArc :
		   lst_stt->is_Work? col_bgWork : lst_stt->is_FTP? col_bgFTP :
		   is_AltLnBgCol(idx)? col_bgList2 : col_bgList;
}

//---------------------------------------------------------------------------
//�t�@�C�����X�g�̃\�[�g
//---------------------------------------------------------------------------
void SortList(TStringList *lst, int tag)
{
	flist_stt *lst_stt = &ListStt[tag];
	if (lst_stt->is_Work && (NotSortWorkList || WorkListHasSep)) return;

	NaturalOrder = FlOdrNatural[tag];
	DscNameOrder = FlOdrDscName[tag];
	SmallOrder   = FlOdrSmall[tag];
	OldOrder	 = FlOdrOld[tag];
	DscAttrOrder = FlOdrDscAttr[tag];
	DscPathOrder = FlOdrDscPath[tag];

	if (lst_stt->is_Find && lst_stt->find_PathSort) {
		lst->CustomSort(SortComp_PathName);		//�ꏊ
	}
	else {
		switch (SortMode[tag]) {
		case 1:  lst->CustomSort(SortComp_Ext);		break;
		case 2:  lst->CustomSort(SortComp_Time);	break;
		case 3:  lst->CustomSort(SortComp_Size);	break;
		case 4:  lst->CustomSort(SortComp_Attr);	break;
		default: lst->CustomSort(SortComp_Name);
		}
	}
}

//---------------------------------------------------------------------------
//�t�@�C���X�g�ō��ڃT�[�`
//---------------------------------------------------------------------------
int IndexOfFileList(
	UnicodeString fnam,	//�t�@�C����
	int tag,			//�^�O				(default = CurListTag)
	int top_idx)		//�擪�C���f�b�N�X	(default = -1)
{
	if (fnam.IsEmpty()) return -1;

	TStringList *lst = GetFileList(tag);
	int idx = lst->IndexOf(fnam);

	if (idx!=-1) {
		TListBox *lp = FileListBox[tag];
		if (top_idx!=-1) lp->TopIndex = top_idx;
		lp->ItemIndex = idx;
	}

	return idx;
}

//---------------------------------------------------------------------------
file_rec* ExistsInList(UnicodeString fnam, TStringList *lst)
{
	file_rec *ret_fp = NULL;
	for (int i=0; i<lst->Count && !ret_fp; i++) {
		file_rec *fp = (file_rec*)lst->Objects[i];
		if (SameText(fp->n_name, fnam)) ret_fp = fp;
	}
	return ret_fp;
}

//---------------------------------------------------------------------------
//�I�𒆃t�@�C���̃��X�g��������擾
//�I���s������ EAbort ��O
//---------------------------------------------------------------------------
UnicodeString GetSelFileStr(
	TStringList *lst,
	bool use_tmp,			//�A�[�J�C�u���̃t�@�C�����ꎞ��	(default = false)
	bool inc_dir,			//�f�B���N�g�����܂߂�				(default = false)
	TStringList *s_lst)		//[o] �I�����X�g					(default = NULL)
{
	UnicodeString f_str;
	if (s_lst) s_lst->Clear();

	for (int i=0; i<lst->Count; i++) {
		file_rec *fp = (file_rec*)lst->Objects[i];
		if (!fp->selected) continue;
		if (!inc_dir && fp->is_dir) continue;

		UnicodeString fnam;
		if (use_tmp && fp->is_virtual) {
			if (!SetTmpFile(fp)) UserAbort(USTR_FaildTmpUnpack);
			fnam = fp->tmp_name;
		}
		else
			fnam = fp->f_name;

		if (!f_str.IsEmpty()) f_str.UCAT_T(" ");
		f_str += add_quot_if_spc(fnam);

		if (s_lst) s_lst->Add(add_quot_if_spc(fnam));
	}
	return f_str;
}

//---------------------------------------------------------------------------
//�t�@�C�����X�g�̃N���A
//---------------------------------------------------------------------------
void clear_FileList(TStringList *lst)
{
	for (int i=0; i<lst->Count; i++) {
		file_rec *fp = (file_rec*)lst->Objects[i];
		if (fp) {
			delete fp->inf_list;
			delete fp;
		}
	}
	lst->Clear();
}
//---------------------------------------------------------------------------
//�t�@�C�����X�g�̍��ڍ폜
//---------------------------------------------------------------------------
void del_FileListItem(TStringList *lst, int idx)
{
	if (idx>=0 && idx<lst->Count) {
		file_rec *fp = (file_rec*)lst->Objects[idx];
		if (fp) {
			delete fp->inf_list;
			delete fp;
		}
		lst->Delete(idx);
	}
}

//---------------------------------------------------------------------------
//�I��L��
//---------------------------------------------------------------------------
bool ListSelected(TStringList *lst)
{
	bool res = false;
	if (lst) for (int i=0; i<lst->Count && !res; i++) res = ((file_rec*)lst->Objects[i])->selected;
	return res;
}
//---------------------------------------------------------------------------
//�I�𒆂̍��ڐ�
//---------------------------------------------------------------------------
int GetSelCount(TStringList *lst)
{
	int s_cnt = 0;
	if (lst)
		for (int i=0; i<lst->Count; i++) if (((file_rec*)lst->Objects[i])->selected) s_cnt++;
	return s_cnt;
}
//---------------------------------------------------------------------------
//�I�𒆍��ږ����X�g���擾
//---------------------------------------------------------------------------
void GetSelList(
	TStringList *lst,
	TStringList *sel_lst,
	bool with_path,		//�p�X�t�Ŏ擾		(default = true)
	bool with_obj)		//Objects ���擾	(default = false)
{
	if (lst && sel_lst) {
		sel_lst->Clear();
		for (int i=0; i<lst->Count; i++) {
			file_rec *fp = (file_rec*)lst->Objects[i];
			if (fp->selected) {
				if (with_obj)
					sel_lst->AddObject(with_path? fp->f_name : fp->n_name, (TObject*)fp);
				else
					sel_lst->Add(with_path? fp->f_name : fp->n_name);
			}
		}
	}
}

//---------------------------------------------------------------------------
//�J�����g�̑I����Ԃ�����
//---------------------------------------------------------------------------
void ClrSelect(TStringList *lst)
{
	if (!lst) lst = GetCurList();
	for (int i=0; i<lst->Count; i++) ((file_rec*)lst->Objects[i])->selected = false;
}

//---------------------------------------------------------------------------
//file_rec �𐶐����ď�����
//  copy_fp ���w�肷��ƁA���̓��e���R�s�[
//---------------------------------------------------------------------------
file_rec* cre_new_file_rec(file_rec *copy_fp)
{
	file_rec *fp = new file_rec;
	fp->inf_list = new TStringList();
	fp->matched  = false;
	fp->failed	 = false;

	if (copy_fp) {
		fp->f_name	   = copy_fp->f_name;
		fp->p_name	   = copy_fp->p_name;
		fp->n_name	   = copy_fp->n_name;
		fp->b_name	   = copy_fp->b_name;
		fp->f_ext	   = copy_fp->f_ext;
		fp->alias	   = copy_fp->alias;
		fp->arc_name   = copy_fp->arc_name;
		fp->tmp_name   = copy_fp->tmp_name;
		fp->r_name	   = copy_fp->r_name;
		fp->memo	   = copy_fp->memo;
		fp->tag 	   = copy_fp->tag;
		fp->is_up	   = copy_fp->is_up;
		fp->is_dir	   = copy_fp->is_dir;
		fp->is_sym	   = copy_fp->is_sym;
		fp->is_virtual = copy_fp->is_virtual;
		fp->is_ftp	   = copy_fp->is_ftp;
		fp->is_dummy   = copy_fp->is_dummy;
		fp->selected   = copy_fp->selected;
		fp->f_size	   = copy_fp->f_size;
		fp->f_attr	   = copy_fp->f_attr;
		fp->attr_str   = copy_fp->attr_str;
		fp->f_time	   = copy_fp->f_time;
		fp->o_size	   = copy_fp->o_size;
		fp->f_count    = copy_fp->f_count;
		fp->d_count    = copy_fp->d_count;
		fp->img_ori    = copy_fp->img_ori;
		fp->is_video   = copy_fp->is_video;
		fp->inf_list->Assign(copy_fp->inf_list);
		fp->prv_text   = copy_fp->prv_text;
		fp->tail_text  = copy_fp->tail_text;
	}
	else {
		fp->tag 	   = -1;
		fp->is_up	   = false;
		fp->is_dir	   = false;
		fp->is_sym	   = false;
		fp->is_virtual = false;
		fp->is_ftp	   = false;
		fp->is_dummy   = false;
		fp->selected   = false;
		fp->f_size	   = -1;
		fp->f_attr	   = 0;
		fp->f_time	   = 0;
		fp->o_size	   = -1;
		fp->f_count    = -1;
		fp->d_count    = -1;
		fp->img_ori    = 0;
		fp->is_video   = false;
	}

	return fp;
}
//---------------------------------------------------------------------------
//�t�@�C�������� file_rec �𐶐�
//---------------------------------------------------------------------------
file_rec* cre_new_file_rec(
	UnicodeString fnam,		//�t�@�C����(������ \ �̏ꍇ�̓f�B���N�g����)
	int tag)
{
	file_rec *fp = cre_new_file_rec();
	fp->tag = tag;
	bool ok = true;

	if (fnam.IsEmpty())
		fp->is_dummy = true;
	else if (is_separator(fnam)) {
		fp->alias.USET_T("-");
		fp->is_dummy = true;
	}
	else if (USAME_TS(fnam, "..")) {
		fp->is_up	 = true;
		fp->is_dir	 = true;
		fp->f_name.USET_T("..");
		fp->n_name.USET_T("..");
		fp->b_name.USET_T("..");
	}
	else {
		bool is_dir = ends_PathDlmtr(fnam);
		ok = is_dir? dir_exists(fnam) : file_exists(fnam);
		if (ok) {
			if (is_dir) fnam = ExcludeTrailingPathDelimiter(fnam);
			fp->f_name = fp->r_name = fnam;
			fp->p_name = ExtractFilePath(fnam);
			fp->n_name = ExtractFileName(fnam);
			fp->f_time = get_file_age(fnam);
			fp->f_attr = file_GetAttr(fnam);
			fp->attr_str = get_file_attr_str(fp->f_attr);
			if (is_dir) {
				fp->is_dir = true;
				fp->b_name = ExtractFileName(fnam);
			}
			else {
				fp->b_name = get_base_name(fnam);
				fp->f_ext  = get_extension(fnam);
				fp->f_size = get_file_size(fnam);
			}
		}
	}

	if (ok) {
		return fp;
	}
	else {
		del_file_rec(fp);
		return NULL;
	}
}
//---------------------------------------------------------------------------
file_rec* cre_new_up_rec(int tag)
{
	file_rec *fp = cre_new_file_rec();
	fp->tag = tag;
	fp->is_up	 = true;
	fp->is_dir	 = true;
	fp->f_name.USET_T("..");
	fp->n_name.USET_T("..");
	fp->b_name.USET_T("..");
	fp->attr_str = get_file_attr_str(fp->f_attr);
	return fp;
}

//---------------------------------------------------------------------------
void del_file_rec(file_rec *fp)
{
	if (fp) {
		delete fp->inf_list;
		delete fp;
	}
}

//---------------------------------------------------------------------------
void inv_file_rec(file_rec *fp)
{
	if (fp) {
		fp->f_size	 = -1;
		fp->f_attr	 = faInvalid;
		fp->f_time	 = 0;
		fp->attr_str = get_file_attr_str(fp->f_attr);
	}
}

//---------------------------------------------------------------------------
// FileList�̓��e�����ւ���
//---------------------------------------------------------------------------
void swap_FileList(TStringList *lst1, TStringList *lst2, bool swap_tag)
{
	std::unique_ptr<TStringList> sbuf(new TStringList());
	sbuf->Assign(lst1); lst1->Assign(lst2); lst2->Assign(sbuf.get());

	//�^�O�̓���ւ�
	if (swap_tag) {
		for (int i=0; i<lst1->Count; i++) {
			file_rec *fp = (file_rec*)lst1->Objects[i];
			fp->tag = (fp->tag==0)? 1 : 0;
		}
		for (int i=0; i<lst2->Count; i++) {
			file_rec *fp = (file_rec*)lst2->Objects[i];
			fp->tag = (fp->tag==0)? 1 : 0;
		}
	}
}

//---------------------------------------------------------------------------
//�t�@�C�����̍��ڂ��X�V
//---------------------------------------------------------------------------
void update_FileInf(file_rec *fp, UnicodeString tit, UnicodeString vstr)
{
	tit = REPLACE_TS(tit, "SHA", "SHA-");

	int idx = -1;
	for (int i=3; i<fp->inf_list->Count; i++) {
		if (StartsText(UAPP_T(tit, ":"), Trim(fp->inf_list->Strings[i]))) {
			fp->inf_list->Delete(i);
			idx = i;
			break;
		}
	}
	UnicodeString lbuf = make_PropLine(tit, vstr);
	if (idx!=-1)
		fp->inf_list->Insert(idx, lbuf);
	else
		fp->inf_list->Add(lbuf);
}

//---------------------------------------------------------------------------
//�^�u���X�g�̏���
//---------------------------------------------------------------------------
tab_info* cre_tab_info()
{
	tab_info *tp = new tab_info;
	for (int i=0; i<MAX_FILELIST; i++) {
		tp->sel_list[i]   = new TStringList();
		tp->dir_hist[i]   = new TStringList();
		tp->dir_hist_p[i] = 0;
	}
	return tp;
}
//---------------------------------------------------------------------------
int add_TabList(UnicodeString item)
{
	return TabList->AddObject(item, (TObject*)cre_tab_info());
}
//---------------------------------------------------------------------------
void insert_TabList(int idx, UnicodeString item)
{
	TabList->InsertObject(idx, item, (TObject*)cre_tab_info());
}

//---------------------------------------------------------------------------
void del_tab_info(tab_info *tp)
{
	if (tp) {
		for (int i=0; i<MAX_FILELIST; i++) {
			delete tp->sel_list[i];
			delete tp->dir_hist[i];
		}
		delete tp;
	}
}

//---------------------------------------------------------------------------
tab_info *get_TabInfo(int tab_idx)
{
	if (tab_idx==-1) tab_idx = CurTabIndex;
	if (tab_idx>=0 && tab_idx<TabList->Count) return (tab_info*)TabList->Objects[tab_idx];
	return NULL;
}

//---------------------------------------------------------------------------
TStringList *get_DirHistory(int tab_idx, int tag)
{
	tab_info *tp = get_TabInfo(tab_idx);
	return (tp? tp->dir_hist[tag] : NULL);
}
//---------------------------------------------------------------------------
int *get_DirHistPtr(int tab_idx, int tag)
{
	tab_info *tp = get_TabInfo(tab_idx);
	return (tp? &tp->dir_hist_p[tag] : NULL);
}
//---------------------------------------------------------------------------
void clear_DirHistory(int tab_idx, int tag)
{
	TStringList *h_lst = get_DirHistory(tab_idx, tag);
	if (h_lst) h_lst->Clear();
}

//---------------------------------------------------------------------------
UnicodeString get_TabWorkList(int tab_idx)
{
	if (tab_idx>=0 && tab_idx<TabList->Count) {
		TStringDynArray itm_buf = get_csv_array(TabList->Strings[tab_idx], TABLIST_CSVITMCNT, true);
		int w_mod = itm_buf[6].ToIntDef(0);
		return rel_to_absdir((w_mod==1)? HomeWorkList : (w_mod==2)? itm_buf[7] : EmptyStr);
	}
	else
		return EmptyStr;
}

//---------------------------------------------------------------------------
//�����ݒ�̃N���A
//---------------------------------------------------------------------------
void clear_FindStt(flist_stt *lst_stt)
{
	lst_stt->is_Find	  = false;

	lst_stt->find_dir	  = false;
	lst_stt->find_both	  = false;
	lst_stt->find_sub	  = false;
	lst_stt->find_arc	  = false;
	lst_stt->find_mark	  = false;
	lst_stt->find_tag	  = false;
	lst_stt->find_tag	  = false;
	lst_stt->find_tag	  = false;
	lst_stt->find_hlink   = false;

	lst_stt->find_RegEx   = false;
	lst_stt->find_And	  = false;
	lst_stt->find_Case	  = false;

/*	���ʃ��X�g�̏�Ԉێ��̂��߈ȉ��͏��������Ȃ�
	lst_stt->find_ResLink = false;
	lst_stt->find_DirLink = false;
	lst_stt->find_PathSort= false;
*/

	lst_stt->find_Path	  = EmptyStr;
	lst_stt->find_Name	  = EmptyStr;
	lst_stt->find_Mask	  = EmptyStr;
	lst_stt->find_Keywd   = EmptyStr;

	lst_stt->find_dt_mode = 0;
	lst_stt->find_sz_mode = 0;
	lst_stt->find_at_mode = 0;
	lst_stt->find_tm_mode = 0;

	lst_stt->find_codepage = -1;
	lst_stt->find_bom_mode = 0;
	lst_stt->find_linebrk  = EmptyStr;

	lst_stt->find_rt_mode  = 0;
	lst_stt->find_fs_mode  = 0;
	lst_stt->find_fw_mode  = 0;
	lst_stt->find_fh_mode  = 0;

	lst_stt->find_iw_mode  = 0;
	lst_stt->find_ih_mode  = 0;

	lst_stt->find_sp_mode  = 0;
	lst_stt->find_bt_mode  = 0;
	lst_stt->find_ch_mode  = 0;

	lst_stt->find_ic_mode  = 0;

	lst_stt->find_hl_mode  = 0;

	lst_stt->find_PrpKwd   = EmptyStr;
	lst_stt->find_TxtKwd   = EmptyStr;
	lst_stt->find_ExifKwd  = EmptyStr;
	lst_stt->find_LatLng   = EmptyStr;
}

//---------------------------------------------------------------------------
//���������̃`�F�b�N
//---------------------------------------------------------------------------
bool check_int(int v0, int v1, int mode)
{
	return ((mode==1)? (v0<=v1) : (mode==2)? (v0==v1) : (mode==3)? (v0>=v1) : true);
}
//---------------------------------------------------------------------------
bool check_prop(UnicodeString fnam, UnicodeString prop, int v, int mode)
{
	if (mode==0) return true;

	UnicodeString vstr = usr_SH->get_PropStr(fnam, prop);
	return !vstr.IsEmpty()? check_int(extract_int(vstr), v, mode) : false;
}

//---------------------------------------------------------------------------
//�W���t�@�C������
//�I�s���Ȑ��K�\���p�^�[����p���Ȃ�����(�v���O�`�F�b�N)
//---------------------------------------------------------------------------
bool check_file_std(
	UnicodeString fnam,		//�p�X���t�@�C����
	TDateTime f_tm, __int64 f_sz, int f_atr, flist_stt *lst_stt)
{
	//�L�[���[�h
	if (!lst_stt->find_Keywd.IsEmpty()) {
		UnicodeString kwd = lst_stt->find_Keywd;
		bool is_regex = lst_stt->find_RegEx;
		//�_�u���N�H�[�e�[�V�����ň͂܂�Ă�����󔒂��܂ތ�Ƃ��Đ��K�\����
		if (is_quot(kwd)) {
			kwd = TRegEx::Escape(exclude_quot(kwd));
			if (contains_s(kwd, _T(' '))) kwd = REPLACE_TS(kwd, " ", "\\s");
			is_regex = true;
		}
		//����
		if (is_regex) {
			TRegExOptions opt;  if (!lst_stt->find_Case) opt << roIgnoreCase;
			if (!TRegEx::IsMatch(fnam, kwd, opt)) return false;
		}
		else
			if (!find_mlt(kwd, fnam, lst_stt->find_And, false, lst_stt->find_Case)) return false;
	}
	//�^�C���X�^���v
	if (lst_stt->find_dt_mode>0) {
		if (!lst_stt->find_dt_str.IsEmpty() && lst_stt->find_dt_mode==1) {
		    if (!str_match(lst_stt->find_dt_str, format_Date(f_tm))) return false;
		}
		else {
			TValueRelationship res = CompareDate(f_tm, lst_stt->find_date);
			switch (lst_stt->find_dt_mode) {
			case 1: if (res!=EqualsValue)							return false; break;
			case 2: if (res!=EqualsValue && res!=LessThanValue)		return false; break;
			case 3: if (res!=EqualsValue && res!=GreaterThanValue)	return false; break;
			}
		}
	}
	//�T�C�Y
	if (!(f_atr & faDirectory) && lst_stt->find_sz_mode>0) {
		switch (lst_stt->find_sz_mode) {
		case 1: if (!(f_sz <= lst_stt->find_size)) return false; break;
		case 2: if (!(f_sz >= lst_stt->find_size)) return false; break;
		}
	}
	//����
	if (lst_stt->find_at_mode>0) {
		switch (lst_stt->find_at_mode) {
		case 1: if (!(f_atr & lst_stt->find_attr)) return false; break;
		case 2: if ((f_atr & lst_stt->find_attr))  return false; break;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//�g���t�@�C������
//�I�s���Ȑ��K�\���p�^�[����p���Ȃ�����(�v���O�`�F�b�N)
//---------------------------------------------------------------------------
bool check_file_ex(UnicodeString fnam, flist_stt *lst_stt)
{
	if (lst_stt->find_tm_mode>0) {
		try {
			TTime tm = get_duration(fnam)/86400000.0;	//ms/(24*60*60*1000)
			TValueRelationship res = CompareTime(tm, lst_stt->find_time);
			switch (lst_stt->find_tm_mode) {
			case 1: if (res!=EqualsValue && res!=LessThanValue)		Abort(); break;
			case 2: if (res!=EqualsValue)							Abort(); break;
			case 3: if (res!=EqualsValue && res!=GreaterThanValue)	Abort(); break;
			}
		}
		catch (...) {
			return false;
		}
	}

	//�R�[�h�y�[�W/���s�R�[�h
	if (lst_stt->find_codepage!=-1 || !lst_stt->find_linebrk.IsEmpty()) {
		//�R�[�h�y�[�W
		UnicodeString linebrk;
		bool has_bom;
		int code_page = get_FileCodePage(fnam, &linebrk, &has_bom);	if (code_page==0) return false;
		if (lst_stt->find_codepage!=-1 && code_page!=lst_stt->find_codepage) return false;
		//���s�R�[�h
		if (!lst_stt->find_linebrk.IsEmpty()) {
			if (USAME_TS(lst_stt->find_linebrk, "����")) {
				std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
				std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
				ms->CopyFrom(ms.get(), 0);
				linebrk = get_StreamLineBreak(fs.get(), code_page, true);
			}
			if (!SameStr(linebrk, lst_stt->find_linebrk)) return false;
		}
		//BOM
		if (lst_stt->find_bom_mode>0) {
			if (lst_stt->find_bom_mode==1 && !has_bom) return false;
			if (lst_stt->find_bom_mode==2 &&  has_bom) return false;
		}
	}

	if (!check_prop(fnam, "�r�b�g ���[�g",	lst_stt->find_rate,  lst_stt->find_rt_mode)) return false;
	if (!check_prop(fnam, "�t���[����",		lst_stt->find_fps,   lst_stt->find_fs_mode)) return false;
	if (!check_prop(fnam, "�t���[����",		lst_stt->find_fr_wd, lst_stt->find_fw_mode)) return false;
	if (!check_prop(fnam, "�t���[����",		lst_stt->find_fr_hi, lst_stt->find_fh_mode)) return false;

	//�T���v�����O���g��
	if (lst_stt->find_sp_mode>0 || lst_stt->find_bt_mode>0 || lst_stt->find_ch_mode>0) {
		try {
			std::unique_ptr<TStringList> lst(new TStringList());
			get_WavInf(fnam, lst.get());
			if (!contains_s(lst->Text, _T(" �`��: "))) Abort();
			UnicodeString frmt = get_tkn_m(lst->Text, _T("�`��: "), _T("\r\n"));
			//�T���v�����O
			int sp = split_tkn(frmt, _T("Hz ")).ToInt();
			if (!check_int(sp, lst_stt->find_smpl, lst_stt->find_sp_mode)) Abort();
			//�r�b�g
			int bt = split_tkn(frmt, _T("bit ")).ToInt();
			switch (lst_stt->find_bt_mode) {
			case 1: if (bt!=8)	Abort(); break;
			case 2: if (bt!=16)	Abort(); break;
			case 3: if (bt!=24)	Abort(); break;
			case 4: if (bt!=32)	Abort(); break;
			}
			//�`�����l��
			switch (lst_stt->find_ch_mode) {
			case 1: if (!USAME_TI(frmt, "���m"))		Abort(); break;
			case 2: if (!USAME_TI(frmt, "�X�e���I"))	Abort(); break;
			case 3: if (!USAME_TI(frmt, "4ch"))			Abort(); break;
			case 4: if (!USAME_TI(frmt, "6ch"))			Abort(); break;
			case 5: if (!USAME_TI(frmt, "8ch"))			Abort(); break;
			}
		}
		catch (...) {
			return false;
		}
	}

	//�摜�T�C�Y
	if (lst_stt->find_iw_mode>0 || lst_stt->find_ih_mode>0) {
		unsigned int i_wd, i_hi;
		if (!get_img_size(fnam, &i_wd, &i_hi)) return false;
		if (lst_stt->find_img_max && i_hi>i_wd) std::swap(i_wd, i_hi);
		if (!check_int(i_wd, lst_stt->find_img_wd, lst_stt->find_iw_mode)) return false;
		if (!check_int(i_hi, lst_stt->find_img_hi, lst_stt->find_ih_mode)) return false;
	}

	//�A�C�R����
	if (lst_stt->find_ic_mode>0) {
		int ixn = (int)::ExtractIcon(HInstance, fnam.c_str(), -1);
		if (!check_int(ixn, lst_stt->find_icon, lst_stt->find_ic_mode)) return false;
	}

	//�����N�J�E���g��
	if (lst_stt->find_hl_mode>0) {
		int hln = get_HardLinkCount(fnam);
		if (hln<2) return false;
		if (lst_stt->find_hl_mode==1 && hln>lst_stt->find_hl_cnt)	return false;
	 	if (lst_stt->find_hl_mode==2 && hln!=lst_stt->find_hl_cnt)	return false;
	 	if (lst_stt->find_hl_mode==3 && hln<lst_stt->find_hl_cnt)	return false;
	}

	//�t�@�C����񕶎��񌟍�
	if (!lst_stt->find_PrpKwd.IsEmpty()) {
		std::unique_ptr<TStringList> lst(new TStringList());	//�t�@�C�����p
		TStringList *i_lst = lst.get();
		add_PropLine(_T("���"), usr_SH->get_FileTypeStr(fnam, true), i_lst);

		UnicodeString f_ext = get_extension(fnam);
		if		(test_AppInfExt(f_ext))		get_AppInf(fnam,  i_lst);
		else if (test_HtmlExt(f_ext))		get_HtmlInf(fnam, i_lst);
		else if (test_ExifExt(f_ext))		get_ExifInf(fnam, i_lst);
		else if (test_FontExt(f_ext))		get_FontInf(fnam, i_lst);
		else if (test_Mp3Ext(f_ext))		ID3_GetInf(fnam,  i_lst);
		else if (test_PngExt(f_ext))		get_PngInf(fnam,  i_lst);
		else if (test_GifExt(f_ext))		get_GifInf(fnam,  i_lst);
		else if (USAME_TI(f_ext, ".webp"))	get_WebpInf(fnam, i_lst);
		else if (test_IcoExt(f_ext))		get_IconInf(fnam, i_lst);
		else if (test_LnkExt(f_ext))		usr_SH->get_LnkInf(fnam,  i_lst);
		else if (xd2tx_PropExt(f_ext))		xd2tx_GetInfo(fnam, i_lst);
		else 								usr_SH->get_PropInf(fnam, i_lst);

		if (lst_stt->find_PrpRegEx) {
			TRegExOptions opt;  if (!lst_stt->find_PrpCase) opt << roIgnoreCase;
			if (!TRegEx::IsMatch(i_lst->Text, lst_stt->find_PrpKwd, opt)) return false;
		}
		else {
			if (!find_mlt(lst_stt->find_PrpKwd, i_lst->Text, lst_stt->find_PrpAnd, false, lst_stt->find_PrpCase)) return false;
		}
	}

	//�e�L�X�g
	if (!lst_stt->find_TxtKwd.IsEmpty()) {
		std::unique_ptr<TStringList> f_buf(new TStringList());
		if (xd2tx_TestExt(get_extension(fnam))) {
			if (!xd2tx_Extract(fnam, f_buf.get())) return false;
		}
		else {
			if (load_text_ex(fnam, f_buf.get())==0) return false;
		}
		if (lst_stt->find_TxtRegEx) {
			TRegExOptions opt;
			if (!lst_stt->find_TxtCase) opt << roIgnoreCase;
			opt<<roMultiLine;
			if (!TRegEx::IsMatch(f_buf->Text, lst_stt->find_TxtKwd, opt)) return false;
		}
		else
			if (!find_mlt(lst_stt->find_TxtKwd, f_buf->Text, lst_stt->find_TxtAnd, false, lst_stt->find_TxtCase)) return false;
	}

	//Exif��񕶎��񌟍�
	if (!lst_stt->find_ExifKwd.IsEmpty()) {
		std::unique_ptr<TStringList> i_lst(new TStringList());
		if (!get_ExifInf(fnam, i_lst.get())) return false;
		if (lst_stt->find_ExifRegEx) {
			TRegExOptions opt;  if (!lst_stt->find_ExifCase) opt << roIgnoreCase;
			if (!TRegEx::IsMatch(i_lst->Text, lst_stt->find_ExifKwd, opt)) return false;
		}
		else {
			if (!find_mlt(lst_stt->find_ExifKwd, i_lst->Text,
				lst_stt->find_ExifAnd, false, lst_stt->find_ExifCase))
					return false;
		}
	}

	//GPS�����͈�
	if (!lst_stt->find_LatLng.IsEmpty()) {
		double lat, lng;
		if (!get_GpsInf(fnam, &lat, &lng)) return false;
		return (GetDistance(lst_stt->find_GpsLat, lst_stt->find_GpsLng, lat, lng) <= lst_stt->find_GpsRange);
	}

	return true;
}

//---------------------------------------------------------------------------
//�t�H���g�����擾
//---------------------------------------------------------------------------
void get_FontInf(UnicodeString fnam, TStringList *lst)
{
	if (!test_FontExt(get_extension(fnam))) return;

	//�C���X�g�[���ς�
	bool ok = false;
	if (SameText(ExtractFileDir(fnam), cv_env_str("%WINDIR%\\Fonts")) && lpGetFontResourceInfo) {
		DWORD dwSize = 0;
		if (lpGetFontResourceInfo(fnam.c_str(), &dwSize, NULL, 2)) {
			int n = dwSize/sizeof(LOGFONT);
			if (n>0) {
				std::unique_ptr<LOGFONT[]> lfs(new LOGFONT[n]);
				if (lpGetFontResourceInfo(fnam.c_str(), &dwSize, lfs.get(), 2)) {
					//�t�H���g��
					UnicodeString font_nam;
					for (int i=0; i<n; i++) {
						if (i>0) font_nam.UCAT_T("; ");
						font_nam.cat_sprintf(_T("%s"), lfs[i].lfFaceName);
					}
					add_PropLine(_T("�^�C�g��"), font_nam, lst);
					ok = true;
				}
			}
		}
	}

	if (!ok) usr_SH->get_PropInf(fnam, lst);
}
//---------------------------------------------------------------------------
//�t�H���g�����擾
//---------------------------------------------------------------------------
UnicodeString get_FontName(UnicodeString fnam)
{
	UnicodeString ret_str;

	std::unique_ptr<TStringList> lst(new TStringList());
	get_FontInf(fnam, lst.get());
	for (int i=0; i<lst->Count; i++) {
		UnicodeString lbuf = lst->Strings[i];
		if (StartsText("�^�C�g��:", Trim(lbuf))) {
			ret_str = Trim(get_tkn_m(lbuf, ':', ';'));
			break;
		}
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//�h���C�u���X�g���擾
//  �߂�l : �V���ɒǉ����ꂽ�h���C�u���
//---------------------------------------------------------------------------
drive_info *get_DriveInfoList()
{
	drive_info *new_drive = NULL;

	DWORD d_bit  = ::GetLogicalDrives();	//���p�\�ȃh���C�u���r�b�g�}�X�N�`���Ŏ擾
	DWORD d_flag = 0x00000001;
	for (int d_n = _T('A'); d_n <= _T('Z'); d_n++,d_flag<<=1) {
		UnicodeString dstr;
		dstr.sprintf(_T("%c:\\"), d_n);
		int idx = DriveInfoList->IndexOf(dstr);
		drive_info *dp;
		if ((d_bit & d_flag)==0) {
			//���݂��Ȃ��Ȃ����h���C�u�����폜
			if (idx!=-1) {
				dp = (drive_info *)DriveInfoList->Objects[idx];
				delete dp->small_ico;
				delete dp->large_ico;
				delete dp;
				DriveInfoList->Delete(idx);
			}
			continue;
		}

		if (idx==-1) {
			dp = new drive_info;
			dp->small_ico = new TIcon();
			dp->large_ico = new TIcon();
			DriveInfoList->AddObject(dstr, (TObject*)dp);
			new_drive = dp;
		}
		else {
			dp = (drive_info *)DriveInfoList->Objects[idx];
			dp->small_ico->Handle = NULL;
			dp->large_ico->Handle = NULL;
		}

		dp->drive_str  = dstr;
		dp->accessible = is_drive_accessible(dstr);		//�A�N�Z�X�\
		dp->ejectable  = EjectDrive(dstr, false);		//���O���\

		//���
		dp->drv_type = ::GetDriveType(dstr.c_str());
		switch (dp->drv_type) {
		case DRIVE_REMOVABLE:	dp->type_str.USET_T("�����[�o�u���E���f�B�A");	break;
		case DRIVE_FIXED:		dp->type_str.USET_T("�n�[�h�f�B�X�N");			break;
		case DRIVE_REMOTE:		dp->type_str.USET_T("�l�b�g���[�N�E�h���C�u");	break;
		case DRIVE_CDROM:		dp->type_str.USET_T("CD-ROM�h���C�u");			break;
		case DRIVE_RAMDISK:		dp->type_str.USET_T("RAM�f�B�X�N");	dp->is_RAM = true;	break;
		default: 				dp->type_str = EmptyStr;
		}

		//�{�����[�����A�t�@�C���V�X�e��
		if (dp->accessible)
			dp->volume = get_volume_info(dstr, &dp->f_system);
		else
			dp->volume = dp->f_system = EmptyStr;

		dp->bus_type = EmptyStr;
		dp->is_SSD	 = dp->is_RAM = false;
		if (dp->drv_type!=DRIVE_REMOTE) {
			//�ڑ�I/F���擾
			UnicodeString drv;
			drv.sprintf(_T("\\\\.\\%s"), ExtractFileDrive(dstr).c_str());
			HANDLE hDrive = ::CreateFile(drv.c_str(), 0,
				FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
			if (hDrive!=INVALID_HANDLE_VALUE) {
				bool rq_chk_ram = false;
				DWORD dwLen = 4096;
				std::unique_ptr<BYTE[]> pcbData(new BYTE[dwLen]);
				::ZeroMemory(pcbData.get(), dwLen);

				STORAGE_PROPERTY_QUERY sQuery;
				sQuery.PropertyId = StorageDeviceProperty;
				sQuery.QueryType  = PropertyStandardQuery;
				sQuery.AdditionalParameters[0] = NULL;
				DWORD dwRet;
				if (::DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY,
						&sQuery, sizeof(sQuery), pcbData.get(), dwLen, &dwRet, NULL))
				{
					STORAGE_DEVICE_DESCRIPTOR *pDescriptor = (STORAGE_DEVICE_DESCRIPTOR*)pcbData.get();
					int ix = -1;
					switch(pDescriptor->BusType) {
					case BusTypeScsi:   ix = 0;		break;
					case BusTypeAtapi:  ix = 1;		break;
					case BusTypeAta:    ix = 2;		break;
					case BusType1394:   ix = 3;		break;
					case BusTypeSsa:    ix = 4;		break;
					case BusTypeFibre:  ix = 5;		break;
					case BusTypeUsb:    ix = 6;		break;
					case BusTypeRAID:   ix = 7;		break;
					case BusTypeiScsi:  ix = 8;		break;
					case BusTypeSas:    ix = 9;		break;
					case BusTypeSata:   ix = 10;	break;
					case BusTypeSd:	    ix = 11;	break;
					case BusTypeMmc:    ix = 12;	break;
					case BusTypeSpaces: ix = 13;	break;
					default:
						;
					}
					dp->bus_type = get_word_i_idx(_T("SCSI|ATAPI|ATA|IEEE1394|SSA|Fibre|USB|RAID|iSCSI|SAS|SATA|SD|MMC|SPACE"), ix);

					//SSD�̔��� (Windows 7 �ȍ~)
					if (dp->accessible && IsWindows7OrGreater()) {
						::ZeroMemory(pcbData.get(), dwLen);
						sQuery.PropertyId = StorageDeviceSeekPenaltyProperty;
						sQuery.QueryType  = PropertyStandardQuery;
						if (::DeviceIoControl(hDrive, IOCTL_STORAGE_QUERY_PROPERTY,
								&sQuery, sizeof(sQuery),
								pcbData.get(), sizeof(DEVICE_SEEK_PENALTY_DESCRIPTOR), &dwRet, NULL))
						{
							DEVICE_SEEK_PENALTY_DESCRIPTOR *pDescriptor = (DEVICE_SEEK_PENALTY_DESCRIPTOR*)pcbData.get();
							dp->is_SSD = !pDescriptor->IncursSeekPenalty && !USAME_TI(dp->bus_type, "SD");
							if (dp->is_SSD) dp->type_str.USET_T("�\���b�h�X�e�[�g");
						}
						else rq_chk_ram = true;
					}
				}
				else rq_chk_ram = true;

				//RAM�f�B�X�N�̐���
				if (rq_chk_ram && dp->accessible && dp->drv_type==DRIVE_FIXED && dp->bus_type.IsEmpty()) {
					MEMORYSTATUSEX stt_ex;
					stt_ex.dwLength = sizeof(stt_ex);
					if (::GlobalMemoryStatusEx(&stt_ex)) {
						__int64 sTotal = DiskSize((char)dstr[1] - 'A' + 1);
						//�T�C�Y�������������g�p�e�ʂ�菬����
						if (sTotal<=(__int64)(stt_ex.ullTotalPhys - stt_ex.ullAvailPhys)) {
							dp->is_RAM	 = true;
							dp->type_str.USET_T("RAM�f�B�X�N");
						}
					}
				}

				::CloseHandle(hDrive);
			}
		}

		//UNC(�����[�g)
		dp->unc = EmptyStr;
		if (dp->drv_type==DRIVE_REMOTE) {
			DWORD InfoSize = 1000;
			std::unique_ptr<BYTE[]> inf(new BYTE[InfoSize]);
			LPUNIVERSAL_NAME_INFO pInf = (LPUNIVERSAL_NAME_INFO)inf.get();
			if (WNetGetUniversalName(dstr.c_str(), UNIVERSAL_NAME_INFO_LEVEL, pInf, &InfoSize)==NO_ERROR)
				dp->unc = pInf->lpUniversalName;
		}

		//�A�C�R��
		UnicodeString inam = dp->accessible?
								def_if_empty(get_autorun_ico(UAPP_T(dstr, "autorun.inf")), dstr) : dstr;
		SHFILEINFO sif;
		if (::SHGetFileInfo(inam.c_str(), 0, &sif, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_SMALLICON))
			dp->small_ico->Handle = sif.hIcon;
		if (::SHGetFileInfo(inam.c_str(), 0, &sif, sizeof(SHFILEINFO), SHGFI_ICON | SHGFI_LARGEICON))
			dp->large_ico->Handle = sif.hIcon;
	}

	DriveInfoList->Sort();

	return new_drive;
}
//---------------------------------------------------------------------------
//�w��h���C�u�̏����擾
//---------------------------------------------------------------------------
drive_info *get_DriveInfo(
	UnicodeString dstr)		//�擪���h���C�u�����̕�����
{
	drive_info *dp = NULL;
	if (!dstr.IsEmpty() && isalpha(dstr[1])) {
		UnicodeString drv_str;
		drv_str.sprintf(_T("%c:\\"), dstr[1]);
		int idx = DriveInfoList->IndexOf(drv_str.UpperCase());
		if (idx!=-1) dp = (drive_info *)DriveInfoList->Objects[idx];
	}
	return dp;
}
//---------------------------------------------------------------------------
//�{�����[�������X�V
//---------------------------------------------------------------------------
void update_DriveVolume()
{
	for (int i=0; i<DriveInfoList->Count; i++) {
		drive_info *dp = (drive_info *)DriveInfoList->Objects[i];
		if (dp->accessible) dp->volume = get_volume_info(dp->drive_str);
	}
}

//---------------------------------------------------------------------------
//�h���C�u�e�ʃ��O�̓��e���擾
//---------------------------------------------------------------------------
void get_DriveLogList(
	UnicodeString dstr,	//�Ώۃh���C�u
	TStringList *lst,
	double &r_min,		//�ŏ��g�p��
	double &r_max)		//�ő�g�p��
{
	update_DriveLog(DriveLogName.IsEmpty());

	//�w��h���C�u�̂ݒ��o
	lst->Clear();
	r_min = r_max = -1;
	if (DriveLogList->Count>0) {
		dstr = dstr.SubString(1, 1);
		for (int i=1; i<DriveLogList->Count; i++) {
			UnicodeString lbuf = DriveLogList->Strings[i];
			if (SameText(get_csv_item(lbuf, 1), dstr)) {
				lst->Add(lbuf);
				__int64 used_sz = StrToInt64Def(get_csv_item(lbuf, 2), -1);
				__int64 free_sz = StrToInt64Def(get_csv_item(lbuf, 3), -1);
				if (used_sz>=0 && free_sz>=0) {
					double r = 1.0 * used_sz / (used_sz + free_sz);;
					r_min = (r_min<0)? r : std::min(r_min, r);
					r_max = (r_max<0)? r : std::max(r_max, r);
				}
			}
		}
		lst->Sort();
	}
}

//---------------------------------------------------------------------------
//�h���C�u�e�ʃ��O�̍X�V
//---------------------------------------------------------------------------
void update_DriveLog(bool save)
{
	if (DriveLogList->Count==0) DriveLogList->Add("Date,Drive,Used,Free");

	for (int i=0; i<DriveInfoList->Count; i++) {
		drive_info *dp = (drive_info *)DriveInfoList->Objects[i];
		if (!dp->accessible || dp->drv_type==DRIVE_CDROM) continue;

		__int64 used_sz = 0, free_sz = 0;
		ULARGE_INTEGER FreeAvailable, TotalBytes, FreeBytes;
		if (::GetDiskFreeSpaceEx(dp->drive_str.c_str(), &FreeAvailable, &TotalBytes, &FreeBytes)) {
			__int64 total_sz = TotalBytes.QuadPart;
			free_sz = FreeBytes.QuadPart;
			used_sz = total_sz - free_sz;
		}

		//�{���̃f�[�^��T��
		int idx = -1;
		UnicodeString dstr = dp->drive_str.SubString(1, 1);
		for (int j=DriveLogList->Count-1; j>0; j--) {
			UnicodeString ibuf = DriveLogList->Strings[j];
			TDateTime dt;
			if (str_to_DateTime(get_csv_item(ibuf, 0), &dt)) {
				TValueRelationship res = CompareDate(dt, Now());
				if (res==LessThanValue) continue;
				if (res==EqualsValue && SameText(get_csv_item(ibuf, 1), dstr)) {
					idx = j; break;
				}
			}
		}

		UnicodeString lbuf = FormatDateTime("yyyy/mm/dd", Now());
		lbuf.cat_sprintf(_T(",%s,%llu,%llu"), dp->drive_str.SubString(1, 1).c_str(), used_sz, free_sz);
		if (idx!=-1) 
			DriveLogList->Strings[idx] = lbuf;
		else
			DriveLogList->Add(lbuf);
	}

	if (save) {
		DriveLogName = UAPP_T(ExePath, DRVLOG_FILE);
		if (!saveto_TextFile(DriveLogName, DriveLogList)) DriveLogName = EmptyStr;
	}
}

//---------------------------------------------------------------------------
//�A�[�J�C�u����w��f�B���N�g���ȉ��̑S���ڂ̃��X�g���擾
//---------------------------------------------------------------------------
void get_ArcList(
	UnicodeString anam,		//�A�[�J�C�u�̃t�@�C����
	UnicodeString dnam,		//�Ώۃf�B���N�g����
	TStrings *f_lst,		//[o] �t�@�C�������X�g
	TStrings *d_lst)		//[o] (��)�f�B���N�g�������X�g
{
	if (usr_ARC->OpenArc(anam)) {
		dnam = IncludeTrailingPathDelimiter(dnam);
		arc_find_inf inf;
		if (usr_ARC->FindFirstEx(EmptyStr, &inf, true)) {
			do {
				UnicodeString fnam = inf.f_name;
				if (!StartsText(dnam, fnam)) continue;
				fnam.Delete(1, dnam.Length());
				//�f�B���N�g��
				if (inf.is_dir || fnam.IsEmpty()) {
					if (d_lst->IndexOf(inf.f_name)==-1) d_lst->Add(inf.f_name);
				}
				//�t�@�C��
				else {
					if (f_lst->IndexOf(inf.f_name)==-1) f_lst->Add(inf.f_name);
				}
			} while (usr_ARC->FindNextEx(&inf, true));
		}
		usr_ARC->CloseArc();
	}
}

//---------------------------------------------------------------------------
//�ėp���X�g�̍��ڍ���ݒ� (�s�� = 1/3)
//---------------------------------------------------------------------------
void set_ListBoxItemHi(TListBox *lp, TFont *font)
{
	if (font) lp->Font->Assign(font);
	lp->ItemHeight = std::max(get_FontHeight(lp->Font, abs(lp->Font->Height) / 3.0 + 1), min_ItemHeight(lp->Tag));
}
//---------------------------------------------------------------------------
void set_ListBoxItemHi(TCheckListBox *lp, TFont *font)
{
	if (font) lp->Font->Assign(font);
	lp->ItemHeight = std::max(get_FontHeight(lp->Font, abs(lp->Font->Height) / 3.0 + 1), min_ItemHeight(lp->Tag));
}

//---------------------------------------------------------------------------
//�W�����X�g�̐ݒ� (�s�� = ListInterLn)
//---------------------------------------------------------------------------
void set_StdListBox(TListBox *lp,
	int tag,			//	(default = 0)
	TFont *font)		//	(default = NULL)
{
	if (tag!=0) lp->Tag = tag;
	lp->Color = col_bgList;
	if (!font) font = ListFont;
	lp->Font->Assign(font);
	lp->Canvas->Font->Assign(font);
	lp->ItemHeight = std::max(get_FontHeight(lp->Font, ListInterLn), min_ItemHeight(tag));
}
//---------------------------------------------------------------------------
void set_StdListBox(TCheckListBox *lp, int tag, TFont *font)
{
	if (tag!=0) lp->Tag = tag;
	lp->Color = col_bgList;
	if (!font) font = ListFont;
	lp->Font->Assign(font);
	lp->Canvas->Font->Assign(font);
	lp->ItemHeight = std::max(get_FontHeight(lp->Font, ListInterLn), min_ItemHeight(tag));
}

//---------------------------------------------------------------------------
//�V���v���X�N���[���o�[�̐ݒ�
//---------------------------------------------------------------------------
void set_UsrScrPanel(UsrScrollPanel *sp)
{
	int std_wd = ::GetSystemMetrics(SM_CXVSCROLL);
	int knob_wd = std_wd;

	sp->KnobImgBuffV = NULL;
	sp->KnobImgBuffH = NULL;

	switch (ScrBarStyle) {
	case 2:
		knob_wd *= 0.75;	break;
	case 3:
		knob_wd /= 2;		break;
	case 4:
		if (!BgImgBuff[BGIMGID_KNOB_V]->Empty) {
			knob_wd = std::min(BgImgBuff[BGIMGID_KNOB_V]->Width + 2, knob_wd);
			sp->KnobImgBuffV = BgImgBuff[BGIMGID_KNOB_V];
			if ((sp->Flag & USCRPNL_FLAG_HS) && !BgImgBuff[BGIMGID_KNOB_H]->Empty)
				sp->KnobImgBuffH = BgImgBuff[BGIMGID_KNOB_H];
		}
		break;
	}

	sp->KnobWidth = knob_wd;
	sp->UpdateKnob();

	sp->Color		 = col_bgScrBar;
	sp->KnobColor	 = col_bgScrKnob;
	sp->KnobBdrColor = col_frScrKnob;

	bool is_fl	= (sp->Flag & USCRPNL_FLAG_FL);
	sp->Visible = is_fl? (!HideScrBar && ScrBarStyle>0) : (ScrBarStyle>0);
	if (is_fl) sp->ListCsrVisible = FlCursorVisible;

	if (sp->ParentPanel) {
		//���X�g�{�b�N�X�̃T�C�Y����
		if (sp->AssoListBox) {
			//��
			int wd = sp->ParentPanel->ClientWidth;
			if ((is_fl && HideScrBar) || ScrBarStyle>0) wd += (std_wd + 2);
			sp->AssoListBox->Width = wd;
			//����
			if (sp->Flag & USCRPNL_FLAG_HS) {
				int hi = sp->ParentPanel->ClientHeight;
				if (ScrBarStyle>0) hi += (std_wd + 2);
				sp->AssoListBox->Height = hi;
			}
		}
		//�O���b�h�̃T�C�Y����
		else if (sp->AssoStrGrid) {
			//��
			int wd = sp->ParentPanel->ClientWidth;
			if (ScrBarStyle>0) wd += (std_wd + 2);
			sp->AssoStrGrid->Width = wd;
			//����
			if (sp->Flag & USCRPNL_FLAG_HS) {
				int hi = sp->ParentPanel->ClientHeight;
				if (ScrBarStyle>0) hi += (std_wd + 2);
				sp->AssoStrGrid->Height = hi;
			}
		}
	}
}

//---------------------------------------------------------------------------
//�g���q�̕��E�ʒu�𒲐� (�������Ɏg�p)
//---------------------------------------------------------------------------
void set_FextWidth(file_rec *fp, int tag)
{
	TCanvas *cv = FileListBox[tag]->Canvas;
	flist_stt *lst_stt = &ListStt[tag];

	int x_base = ScaledInt(ShowIcon? 20 : 8);
	int w_mx_f = std::min(lst_stt->lxp_size - x_base - lst_stt->lwd_half * 9, lst_stt->lwd_fext_max);

	lst_stt->lwd_fext = std::max(lst_stt->lwd_fext, get_TextWidth(cv, fp->f_ext, IsIrregularFont(cv->Font)));
	lst_stt->lwd_fext = std::min(lst_stt->lwd_fext, w_mx_f);
	lst_stt->lxp_fext = lst_stt->lxp_size - lst_stt->lwd_fext - lst_stt->lwd_half;
}

//---------------------------------------------------------------------------
//�t�@�C�����X�g�����X�g�{�b�N�X�ɐݒ�
// (���z���X�g�{�b�N�X�Ή�)
//---------------------------------------------------------------------------
void assign_FileListBox(
	TListBox *lp,
	TStringList *lst,
	int idx,				//�C���f�b�N�X (default = -1)
	UsrScrollPanel *sp)		//�V���v���X�N���[���o�[ (default = NULL)
{
	if (lp->Style==lbVirtualOwnerDraw)
		lp->Count = lst->Count;
	else
		lp->Items->Assign(lst);

	if (idx!=-1) ListBoxSetIndex(lp, idx);

	if (sp) sp->UpdateKnob();
}

//---------------------------------------------------------------------------
//���z�t�@�C�����X�g�{�b�N�X�̍X�V(�ŏ���250ms�Ԋu)
//---------------------------------------------------------------------------
void update_VirtualFileListBox(
	TStrings *lst, TListBox *lp,
	int idx,				//�C���f�b�N�X�A�w�肵���ꍇ�����X�V (default = -1)
	UsrScrollPanel *sp)		//�V���v���X�N���[���o�[ (default = NULL)
{
	static unsigned int start_cnt = 0;

	if ((GetTickCount() - start_cnt)>250 || idx!=-1) {	//***
		start_cnt = GetTickCount();
		set_RedrawOff(lp);
		{
			if (idx==-1) idx = lp->ItemIndex;
			lp->Count = lst->Count;
			lp->ItemIndex = std::min(idx, lp->Count - 1);
		}
		set_RedrawOn(lp);
		if (sp) sp->UpdateKnob();
	}
}

//---------------------------------------------------------------------------
//�t�@�C���������X�g�̎擾
//---------------------------------------------------------------------------
void get_FindListF(UnicodeString pnam, flist_stt *lst_stt, TStrings *lst, int tag)
{
	if (FindAborted) return;
	if (pnam.IsEmpty()) return;
	if (!dir_exists(pnam)) return;

	pnam = IncludeTrailingPathDelimiter(pnam);
	FindPath  = pnam;
	FindCount = (lst->Count>1)? lst->Count - 1 : 0;

	TListBox *lp = FileListBox[tag];

	if (lst_stt->find_Mask.IsEmpty()) lst_stt->find_Mask = "*.*";

	//�T�u�f�B���N�g��������
	UnicodeString sea_str;
	TSearchRec sr;
	if (lst_stt->find_sub) {
		sea_str = UAPP_T(pnam, "*");
		if (sea_str.Length()>=MAX_PATH) sea_str.Insert("\\\\?\\", 1);
		if (FindFirst(sea_str, faAnyFile, sr)==0) {
			do {
				if ((sr.Attr & faDirectory)==0) continue;
				if (!ShowHideAtr   && (sr.Attr & faHidden))  continue;
				if (!ShowSystemAtr && (sr.Attr & faSysFile)) continue;
				if (ContainsStr("..", sr.Name)) continue;
				get_FindListF(pnam + sr.Name, lst_stt, lst, tag);
				Application->ProcessMessages();
			} while(FindNext(sr)==0 && !FindAborted);
			FindClose(sr);
		}
	}
	if (FindAborted) return;

	//�t�@�C��������
	sea_str = lst_stt->find_arc? UAPP_T(pnam, "*") : (pnam + lst_stt->find_Mask);

	if (sea_str.Length()>=MAX_PATH) sea_str.Insert("\\\\?\\", 1);

	if (FindFirst(sea_str, faAnyFile, sr)==0) {
		int add_cnt = 0;
		do {
			Application->ProcessMessages();

			bool is_dir = (sr.Attr & faDirectory);
			if (!lst_stt->find_both && is_dir) continue;
			if (!ShowHideAtr   && (sr.Attr & faHidden))  continue;
			if (!ShowSystemAtr && (sr.Attr & faSysFile)) continue;
			UnicodeString fnam = sr.Name;  if (ContainsStr("..", fnam)) continue;
			UnicodeString fext = !is_dir? get_extension(fnam) : EmptyStr;

			//�A�[�J�C�u������
			if (lst_stt->find_arc && test_ArcExt(fext)) {
				UnicodeString anam = pnam + fnam;
				FindPath = anam;
				Application->ProcessMessages();

				if (usr_ARC->OpenArc(anam)) {
					arc_find_inf inf;
					UnicodeString arc_mask = USAME_TS(lst_stt->find_Mask, "*.*")? UnicodeString("*") : lst_stt->find_Mask;
					if (usr_ARC->FindFirstEx(EmptyStr, &inf, true)) {
						do {
							if (!lst_stt->find_both && inf.is_dir) continue;
							if (!ShowHideAtr   && (inf.f_attr & faHidden))  continue;
							if (!ShowSystemAtr && (inf.f_attr & faSysFile)) continue;

							if (inf.is_dir) inf.f_name = ExcludeTrailingPathDelimiter(inf.f_name);
							if (!str_match(arc_mask, ExtractFileName(inf.f_name))) continue;
							if (!check_file_std(ExtractFileName(inf.f_name), inf.f_time, inf.f_size, inf.f_attr, lst_stt)) continue;

							//��������
							file_rec *fp   = cre_new_file_rec();
							fp->is_virtual = true;
							fp->p_name.sprintf(_T("%s/%s"), anam.c_str(), ExtractFilePath(inf.f_name).c_str());
							fp->f_name.sprintf(_T("%s/%s"), anam.c_str(), inf.f_name.c_str());
							fp->n_name	   = ExtractFileName(inf.f_name);
							fp->arc_name   = anam;
							fp->r_name	   = fp->f_name;
							fp->tag 	   = tag;
							fp->is_dir	   = inf.is_dir;
							fp->f_time	   = inf.f_time;
							fp->f_attr	   = inf.f_attr;
							fp->attr_str   = get_file_attr_str(fp->f_attr);
							if (fp->is_dir)
								fp->b_name = ExtractFileName(inf.f_name);
							else {
								fp->b_name = get_base_name(inf.f_name);
								fp->f_ext  = get_extension(inf.f_name);
								fp->f_size = inf.f_size;
								//�g���q�̕��E�ʒu����
								set_FextWidth(fp, tag);
							}
							lst->AddObject(fp->f_name, (TObject*)fp);
							FindCount = (lst->Count>1)? lst->Count - 1 : 0;
							add_cnt++;
						} while (usr_ARC->FindNextEx(&inf, true));
					}
					usr_ARC->CloseArc();

					//���X�g�{�b�N�X�X�V(���z)
					if (add_cnt>0) update_VirtualFileListBox(lst, lp);
				}
			}
			else FindPath  = pnam;

			//��FindFirst �ł̌�}�b�`�����O
			//  ��: �V�X�e������ *.asp �� *.aspx �Ƀ}�b�` (��)
			if ((!is_dir || !USAME_TS(lst_stt->find_Mask, "*.*")) && !str_match(lst_stt->find_Mask, fnam)) continue;

			//�W������
			if (!check_file_std(fnam, sr.TimeStamp, sr.Size, sr.Attr, lst_stt)) continue;
			//�g���t�@�C������
			if (!is_dir && !check_file_ex(pnam + fnam, lst_stt)) continue;

			//��������
			file_rec *fp = cre_new_file_rec();
			fp->tag		 = tag;
			fp->is_dir	 = is_dir;
			fp->p_name	 = pnam;
			fp->f_name	 = pnam + fnam;
			fp->n_name	 = fnam;
			fp->f_time	 = sr.TimeStamp;
			fp->f_attr	 = sr.Attr;
			fp->attr_str = get_file_attr_str(fp->f_attr);
			if (fp->is_dir)
				fp->b_name = fp->n_name;
			else {
				fp->b_name = get_base_name(fp->f_name);
				fp->f_ext  = fext;
				fp->f_size = sr.Size;
				//�g���q�̕��E�ʒu����
				set_FextWidth(fp, tag);
			}
			lst->AddObject(fp->f_name, (TObject*)fp);
			FindCount = (lst->Count>1)? lst->Count - 1 : 0;
			add_cnt++;
		} while(FindNext(sr)==0 && !FindAborted);
		FindClose(sr);

		//���X�g�{�b�N�X�X�V(���z)
		if (add_cnt>0) update_VirtualFileListBox(lst, lp);
	}
}

//---------------------------------------------------------------------------
//�f�B���N�g���������X�g�̎擾
//---------------------------------------------------------------------------
void get_FindListD(UnicodeString pnam, flist_stt *lst_stt, TStrings *lst, int tag)
{
	if (FindAborted) return;
	if (pnam.IsEmpty() || !dir_exists(pnam)) return;

	pnam = IncludeTrailingPathDelimiter(pnam);
	FindPath  = pnam;
	FindCount = (lst->Count>1)? lst->Count - 1 : 0;
	if (lst_stt->find_Mask.IsEmpty()) lst_stt->find_Mask = "*";

	UnicodeString sea_str = UAPP_T(pnam, "*");
	if (sea_str.Length()>=MAX_PATH) sea_str.Insert("\\\\?\\", 1);

	TSearchRec sr;
	if (FindFirst(sea_str, faAnyFile, sr)==0) {
		do {
			if ((sr.Attr & faDirectory)==0) continue;
			if (!ShowHideAtr   && (sr.Attr & faHidden))  continue;
			if (!ShowSystemAtr && (sr.Attr & faSysFile)) continue;
			UnicodeString dnam = sr.Name;
			if (ContainsStr("..", dnam)) continue;

			if (str_match(lst_stt->find_Mask, dnam)) {
				bool match = true;
				//�^�C���X�^���v
				TDateTime f_tm = sr.TimeStamp;
				if (lst_stt->find_dt_mode>0) {
					if (!lst_stt->find_dt_str.IsEmpty() && lst_stt->find_dt_mode==1) {
					    match = str_match(lst_stt->find_dt_str, format_Date(f_tm));
					}
					else {
						TValueRelationship res = CompareDate(f_tm, lst_stt->find_date);
						switch (lst_stt->find_dt_mode) {
						case 1: match = (res==EqualsValue); break;
						case 2: match = (res==EqualsValue || res==LessThanValue);    break;
						case 3: match = (res==EqualsValue || res==GreaterThanValue); break;
						}
					}
				}
				//���e
				if (match && lst_stt->find_ct_mode>0) {
					bool is_empty = is_EmptyDir(pnam + dnam);
					switch (lst_stt->find_ct_mode) {
					case 1: match = is_empty;  break;
					case 2: match = !is_empty; break;
					}
				}
				//����
				if (match && lst_stt->find_at_mode>0) {
					switch (lst_stt->find_at_mode) {
					case 1: match = (sr.Attr & lst_stt->find_attr); break;
					case 2: match = (sr.Attr & lst_stt->find_attr); break;
					}
				}

				//��������
				if (match) {
					file_rec *fp = cre_new_file_rec();
					fp->is_dir	 = true;
					fp->p_name	 = pnam;
					fp->f_name	 = pnam + dnam;
					fp->n_name	 = dnam;
					fp->b_name	 = dnam;
					fp->tag		 = tag;
					fp->f_time	 = f_tm;
					fp->f_attr	 = sr.Attr;
					fp->attr_str = get_file_attr_str(fp->f_attr);
					lst->AddObject(fp->f_name, (TObject*)fp);
					FindCount = (lst->Count>1)? lst->Count - 1 : 0;
					//���X�g�{�b�N�X�X�V(���z)
					update_VirtualFileListBox(lst, FileListBox[tag]);
				}
			}

			get_FindListD(pnam + dnam, lst_stt, lst, tag);
			Application->ProcessMessages();
		} while(FindNext(sr)==0 && !FindAborted);
		FindClose(sr);
	}
}

//---------------------------------------------------------------------------
//���ׂẴT�u�f�B���N�g�����擾
//---------------------------------------------------------------------------
void get_SubDirs(UnicodeString pnam, TStrings *lst, TStatusBar *stt_bar, int stt_idx)
{
	if (!dir_exists(pnam)) return;
	pnam = IncludeTrailingPathDelimiter(pnam);
	if (stt_bar && stt_idx>=0 && stt_idx<stt_bar->Panels->Count) {
		stt_bar->Panels->Items[stt_idx]->Text = pnam;
		stt_bar->Repaint();
	}

	//�T�u�f�B���N�g��������
	UnicodeString sea_str;
	TSearchRec sr;
	sea_str = cv_ex_filename(UAPP_T(pnam, "*"));
	if (FindFirst(sea_str, faAnyFile, sr)==0) {
		do {
			if ((sr.Attr & faDirectory)==0) continue;
			if (!ShowHideAtr   && (sr.Attr & faHidden))  continue;
			if (!ShowSystemAtr && (sr.Attr & faSysFile)) continue;
			if (ContainsStr("..", sr.Name)) continue;
			get_SubDirs(pnam + sr.Name, lst, stt_bar, stt_idx);
			lst->Add(pnam + sr.Name);
		} while(FindNext(sr)==0);
		FindClose(sr);
	}
}

//---------------------------------------------------------------------------
//�f�B���N�g���e�ʂ̌v�Z
//  �߂�l: �f�B���N�g���e��
//---------------------------------------------------------------------------
__int64 get_DirSize(
	UnicodeString dnam,
	int *f_cnt,			//�t�@�C�����ւ̃|�C���^
	int *d_cnt,			//�f�B���N�g�����ւ̃|�C���^
	__int64 *o_size)	//��L�T�C�Y�ւ̃|�C���^
{
	if (CalcAborted) return -1;

	__int64 d_size = 0;
	int     clu_sz = get_ClusterSize(dnam);	//�N���X�^�T�C�Y
	__int64 clu_n  = 0;						//�N���X�^��

	dnam = IncludeTrailingPathDelimiter(dnam);
	UnicodeString sea_str = UAPP_T(dnam, "*");
	if (sea_str.Length()>=MAX_PATH) sea_str.Insert("\\\\?\\", 1);

	TSearchRec sr;
	if (FindFirst(sea_str, faAnyFile, sr)==0) {
		do {
			//�T�u�f�B���N�g��������
			if (sr.Attr & faDirectory) {
				if (!ContainsStr("..", sr.Name)) {
					d_size += get_DirSize(dnam + sr.Name, f_cnt, d_cnt, o_size);
					++(*d_cnt);
				}
			}
			//�t�@�C��
			else {
				d_size += sr.Size;
				if (clu_sz>0) {
					clu_n += (sr.Size/clu_sz);
					if (sr.Size%clu_sz>0) clu_n++;
				}
				++(*f_cnt);
			}
			Application->ProcessMessages();
		} while (FindNext(sr)==0 && !CalcAborted);
		FindClose(sr);
	}

	if (CalcAborted) return -1;

	if (clu_sz>0) *o_size += (clu_sz * clu_n);
	return d_size;
}
//---------------------------------------------------------------------------
//���z�f�B���N�g���e�ʂ̌v�Z
//  �߂�l: �f�B���N�g���e��
//---------------------------------------------------------------------------
__int64 get_ArcDirSize(
	UnicodeString anam,		//�A�[�J�C�u�̃t�@�C����
	UnicodeString dnam,		//�Ώۃf�B���N�g����
	int *f_cnt,				//�t�@�C�����ւ̃|�C���^
	int *d_cnt)				//�f�B���N�g�����ւ̃|�C���^
{
	if (CalcAborted) return -1;

	__int64 d_size = 0;

	if (usr_ARC->OpenArc(anam)) {
		dnam = IncludeTrailingPathDelimiter(dnam);
		arc_find_inf inf;
		if (usr_ARC->FindFirstEx(EmptyStr, &inf, true)) {
			std::unique_ptr<TStringList> d_lst(new TStringList());
			do {
				UnicodeString fnam = inf.f_name;
				if (!StartsText(dnam, fnam)) continue;
				fnam.Delete(1, dnam.Length());
				if (!fnam.IsEmpty()) {
					if (!inf.is_dir) {
						d_size += inf.f_size;
						++(*f_cnt);
						UnicodeString snam = ExtractFileDir(fnam);
						if (!snam.IsEmpty() && d_lst->IndexOf(snam)==-1) d_lst->Add(snam);
					}
					//�f�B���N�g��
					else {
						if (d_lst->IndexOf(fnam)==-1) d_lst->Add(fnam);
					}
				}
				Application->ProcessMessages();
			} while (usr_ARC->FindNextEx(&inf, true) && !CalcAborted);
			(*d_cnt) += d_lst->Count;
		}
		usr_ARC->CloseArc();
	}

	if (CalcAborted) return -1;
	return d_size;
}

//---------------------------------------------------------------------------
//�L���b�V������w��t�@�C���̃A�C�R�����폜
//---------------------------------------------------------------------------
void del_CachedIcon(UnicodeString fnam)
{
	if (!test_FileExt(get_extension(fnam), FEXT_INDIVICO)) return;

	IconRWLock->BeginWrite();
		int idx = CachedIcoList->IndexOf(fnam);
		if (idx!=-1) {
			delete (TIcon*)CachedIcoList->Objects[idx];
			CachedIcoList->Delete(idx);
		}
	IconRWLock->EndWrite();
}

//---------------------------------------------------------------------------
//�g���q�ˑ��A�C�R�����擾 (�L���b�V���𗘗p)
//---------------------------------------------------------------------------
HICON get_fext_icon(
	UnicodeString fext)		//�g���q .xxx	(default = EmptyStr: �t�H���_�A�C�R��)
{
	HICON hIcon = NULL;

	if (fext.IsEmpty()) fext = "$DIR$";
	int idx = GeneralIconList->IndexOf(fext);

	if (idx!=-1)
		hIcon = ((TIcon*)GeneralIconList->Objects[idx])->Handle;
	else {
		SHFILEINFO sif;
		if (::SHGetFileInfo(fext.c_str(), (USTARTS_TS('.', fext)? faArchive : faDirectory),
			&sif, sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON|SHGFI_USEFILEATTRIBUTES))
		{
			hIcon = sif.hIcon;
			TIcon *icon  = new TIcon();
			icon->Handle = hIcon;
			GeneralIconList->AddObject(fext, (TObject*)icon);
		}
	}

	return hIcon;
}

//---------------------------------------------------------------------------
//�X���[���A�C�R����`�� (�X���b�h�Ŏ擾)
//  CachedIcoList ���g�p
//  �W�����N�V����/�V���{���b�N�����N�ɂ��Ή�
//---------------------------------------------------------------------------
bool draw_SmallIcon(
	file_rec *fp,
	TCanvas *cv, int x, int y,
	bool force_cache)	//�����I�ɃL���V��
{
	if (!is_selectable(fp)) return false;

	HICON hIcon  = NULL;
	bool handled = false;

	//�ʏ�f�B���N�g��
	if (fp->is_dir && !fp->is_sym)
		hIcon = get_fext_icon();
	else {
		UnicodeString fext = LowerCase(fp->f_ext);
		//���t�@�C���ˑ�
		if ((UseIndIcon || force_cache) && (test_FileExt(fext, FEXT_INDIVICO) || fp->is_sym)) {
			UnicodeString fnam = (fp->is_virtual || fp->is_ftp)? fp->tmp_name : fp->f_name;
			if (!fnam.IsEmpty()) {
				if (fp->is_dir) fnam = IncludeTrailingPathDelimiter(fnam);

				IconRWLock->BeginWrite();
				{
					int idx = CachedIcoList->IndexOf(fnam);
					if (idx!=-1) {
						if (CachedIcoList->Objects[idx]) {
							hIcon = ((TIcon*)CachedIcoList->Objects[idx])->Handle;
							::DrawIconEx(cv->Handle, x, y, hIcon, SIcoSize, SIcoSize, 0, NULL, DI_NORMAL);
							handled = true;
						}
					}
					else
						CachedIcoList->Add(fnam);	//�X���b�h�Ɏ擾��v��
				}
				IconRWLock->EndWrite();
			}
			else hIcon = get_fext_icon(fext);
		}
		//�g���q�ˑ�
		else {
			if (fext.IsEmpty()) fext = ".nyanfi";
			hIcon = get_fext_icon(fext);
		}
	}

	if (handled) return true;
	if (!hIcon)  return false;

	//�`��
	::DrawIconEx(cv->Handle, x, y, hIcon, SIcoSize, SIcoSize, 0, NULL, DI_NORMAL);
	return true;
}
//---------------------------------------------------------------------------
//�w��t�@�C���̃X���[���A�C�R����`��
//  MenuBtnIcoList ���g�p
//---------------------------------------------------------------------------
bool draw_SmallIcon2(
	UnicodeString fnam,
	TCanvas *cv, int x, int y)
{
	HICON hIcon;

	UnicodeString fext = LowerCase(get_extension(fnam));
	if (fnam.Pos('*')==1) fnam = EmptyStr;

	//�f�B���N�g��
	if (ends_PathDlmtr(fnam))
		hIcon = get_fext_icon();
	//���t�@�C���ˑ�
	else if (test_FileExt(fext, FEXT_INDIVICO)) {
		if (!fnam.IsEmpty()) {
			int idx = MenuBtnIcoList->IndexOf(fnam);
			if (idx!=-1) {
				hIcon = (MenuBtnIcoList->Objects[idx])?
					((TIcon*)MenuBtnIcoList->Objects[idx])->Handle : usr_SH->get_SmallIcon(fnam);
			}
			else {
				hIcon = usr_SH->get_SmallIcon(fnam);
				if (hIcon) {
					TIcon *icon  = new TIcon();
					icon->Handle = hIcon;
					MenuBtnIcoList->AddObject(fnam, (TObject*)icon);
				}
			}
		}
		else hIcon = get_fext_icon(fext);
	}
	//�g���q�ˑ�
	else hIcon = get_fext_icon(fext);

	if (!hIcon)  return false;

	//�`��
	::DrawIconEx(cv->Handle, x, y, hIcon, SIcoSize, SIcoSize, 0, NULL, DI_NORMAL);
	return true;
}

//---------------------------------------------------------------------------
//�\���p�X���[���A�C�R�����擾���ăC���[�W���X�g�ɒǉ�
//  UsrIcoList �ɓo�^���čė��p
//---------------------------------------------------------------------------
int add_IconImage(
	UnicodeString fnam,		//�t�@�C���� (���p�X�ɑΉ�)
	TImageList *lst)
{
	if (fnam.IsEmpty()) return -1;

	int idx = -1;
	TIcon *icon = NULL;

	fnam = rel_to_absdir(get_actual_name(fnam));
	if (file_exists(fnam)) {
		int idx = UsrIcoList->IndexOf(fnam);
		if (idx==-1) {
			SHFILEINFO sif;
			if (::SHGetFileInfo(fnam.c_str(), 0, &sif, sizeof(SHFILEINFO), SHGFI_ICON|SHGFI_SMALLICON)) {
				icon = new TIcon();
				icon->Handle = sif.hIcon;
				UsrIcoList->AddObject(fnam, (TObject*)icon);
			}
		}
		else
			icon = (TIcon*)UsrIcoList->Objects[idx];
	}

	if (icon) idx = lst->AddIcon(icon);
	return idx;
}

//---------------------------------------------------------------------------
//�R�}���h����t�@�C�������擾
//  �t�@�C�����Ƀp�X���Ȃ��ꍇ�A���p�X���猟��
//---------------------------------------------------------------------------
UnicodeString get_file_from_cmd(UnicodeString s)
{
	//�擪�R�}���h���擾
	s = REPLACE_TS(s, ":\\", "\f");

	UnicodeString cmd;
	int p = 1;
	bool in_qut = false;
	while (p<=s.Length()) {
		WideChar c = s[p];
		//���p���O
		if (!in_qut) {
			if (c==':') break;
			if (c=='\"') in_qut = true;
			cmd.cat_sprintf(_T("%c"), c);
		}
		//���p����
		else {
			if (c=='\"') in_qut = false;
			cmd.cat_sprintf(_T("%c"), c);
		}
		p++;
	}

	cmd = Trim(REPLACE_TS(cmd, "\f", ":\\"));

	//�t�@�C�������擾
	UnicodeString fnam;
	if (remove_top_text(cmd, _T("OpenByWin_")) || remove_top_text(cmd, _T("SetExeFile_"))
		 || remove_top_text(cmd, _T("FileRun_")) || remove_top_text(cmd, _T("FileEdit_")))
	{
		fnam = get_actual_name(exclude_quot(cmd));
	}
	else if (USAME_TI(cmd, "CommandPrompt")) fnam.USET_T("%ComSpec%");
	else if (USAME_TI(cmd, "OpenByExp"))	 fnam.USET_T("%windir%\\explorer.exe");
	return fnam;
}

//---------------------------------------------------------------------------
//���s�t�@�C�������擾���ăR���g���[���ɐݒ� (.lnk ����̒��o�ɑΉ�)
//---------------------------------------------------------------------------
void SetExtNameToCtrl(UnicodeString fnam, TWinControl *cp, 
	bool nbt_sw)	//.nbt �ɑΉ� (default = false)
{
	UnicodeString fext = get_extension(fnam);
	if (test_LnkExt(fext)) {
		UnicodeString xnam, prm, fld;
		int shw;
		usr_SH->get_LnkInf(fnam, NULL, &xnam, &prm, &fld, &shw);
		if (test_ExeExt(get_extension(xnam)))
			fnam = xnam;
		else {
			msgbox_WARN("�����N�悪 .exe �t�@�C���ł͂���܂���");
			return;
		}
	}

	if (nbt_sw && test_FileExt(fext, _T(".nbt")))
		fnam = "@" + to_relative_name(fnam);

	//�R���g���[���ɐݒ�
	if		(class_is_Edit(cp))			((TEdit*)cp)->Text		  = fnam;
	else if (class_is_LabeledEdit(cp))	((TLabeledEdit*)cp)->Text = fnam;
	else if (class_is_ComboBox(cp))		((TComboBox*)cp)->Text	  = fnam;
}

//---------------------------------------------------------------------------
//���j���[�t�@�C����ǂݍ���
//---------------------------------------------------------------------------
bool load_MenuFile(UnicodeString fnam, TStringList *lst)
{
	fnam = rel_to_absdir(cv_env_str(fnam));

	if (load_text_ex(fnam, lst)!=0) {
		for (int i=0; i<lst->Count; i++) {
			UnicodeString lbuf = Trim(lst->Strings[i]);
			if (lbuf.IsEmpty() || USTARTS_TS(';', lbuf)) continue;
			TStringDynArray m_buf = split_strings_tab(lbuf);
			//EditMenuFile �R�}���h
			if (m_buf.Length>1 && USAME_TI(m_buf[1], "EditMenuFile")) m_buf[1].sprintf(_T("FileEdit_\"%s\""), fnam.c_str());
			//�A�C�R���t�@�C�� (���p�X�ɑΉ�)
			UnicodeString inum = (m_buf.Length==3)? m_buf[2] : (m_buf.Length==2)? get_file_from_cmd(m_buf[1]) : EmptyStr;
			if (!inum.IsEmpty()) {
				inum = rel_to_absdir(get_actual_name(inum));
				inum = file_exists(inum)? inum : EmptyStr;
				lbuf.sprintf(_T("%s\t%s"), m_buf[0].c_str(), m_buf[1].c_str());
				if (!inum.IsEmpty()) lbuf.cat_sprintf(_T("\t%s"), inum.c_str());
			}
			else {
				lbuf = m_buf[0];
				if (m_buf.Length==2) lbuf.cat_sprintf(_T("\t%s"), m_buf[1].c_str());
			}
			lst->Strings[i] = lbuf;
		}
	}

	return (lst->Count>0);
}

//---------------------------------------------------------------------------
//���[�N���X�g���t�@�C������ǂݍ���
//�@�t�@�C�����͎��s�p�X����̑��Ύw��ł���
//---------------------------------------------------------------------------
bool load_WorkList(UnicodeString wnam)
{
	wnam = rel_to_absdir(wnam);
	if (!file_exists(wnam)) return false;

	clear_FileList(WorkList);
	try {
		std::unique_ptr<TStringList> f_lst(new TStringList());
		f_lst->LoadFromFile(wnam);
		WorkListName	 = wnam;
		WorkListTime	 = get_file_age(wnam);
		WorkListChanged  = WorkListFiltered = false;
		rqWorkListDirInf = true;
		for (int i=0; i<f_lst->Count; i++) {
			UnicodeString lbuf = f_lst->Strings[i];
			UnicodeString fnam = split_pre_tab(lbuf);
			UnicodeString anam = lbuf;
			file_rec *fp = cre_new_file_rec();
			if (fnam.IsEmpty() && is_separator(anam)) {
				//�Z�p���[�^
				fp->alias	 = anam;
				fp->is_dummy = true;
				WorkList->AddObject(fnam, (TObject*)fp);
			}
			else {
				bool is_dir = ends_PathDlmtr(fnam);
				if (is_dir) fnam = ExcludeTrailingPathDelimiter(fnam);

				fp->f_name = fp->r_name = fnam;
				fp->p_name = ExtractFilePath(fnam);
				fp->n_name = ExtractFileName(fnam);
				fp->alias  = anam;

				if (is_dir) {
					fp->is_dir = true;
					fp->b_name = ExtractFileName(fnam);
				}
				else {
					fp->b_name = get_base_name(fnam);
					fp->f_ext  = get_extension(fnam);
				}

				bool ok = !is_InvalidUnc(fp->p_name) && (is_dir? dir_exists(fnam) : file_exists(fnam));
				if (ok) {
					fp->f_time	 = get_file_age(fnam);
					fp->f_attr	 = file_GetAttr(fnam);
					fp->attr_str = get_file_attr_str(fp->f_attr);
					if (!is_dir) fp->f_size = get_file_size(fnam);
				}
				else {
					if (AutoDelWorkList) {
						del_file_rec(fp);
						fp = NULL;
					}
					else {
						inv_file_rec(fp);
					}
				}

				if (fp) WorkList->AddObject(fnam, (TObject*)fp);
			}
		}
		return true;
	}
	catch (...) {
		clear_FileList(WorkList);
		return false;
	}
}

//---------------------------------------------------------------------------
//���[�N���X�g��ۑ�
//---------------------------------------------------------------------------
bool save_WorkList(UnicodeString wnam, TStringList *lst)
{
	if (wnam.IsEmpty()) return false;

	if (!lst) lst = WorkList;
	std::unique_ptr<TStringList> fbuf(new TStringList());
	for (int i=0; i<lst->Count; i++) {
		file_rec *fp = (file_rec*)lst->Objects[i];
		if (fp->is_up) continue;
		UnicodeString lbuf = (fp->is_dir)? IncludeTrailingPathDelimiter(fp->f_name) : fp->f_name;
		lbuf.cat_sprintf(_T("\t%s"), fp->alias.c_str());
		fbuf->Add(lbuf);
	}

	bool res = saveto_TextUTF8(wnam, fbuf.get());
	if (res) {
		WorkListChanged  = false;
		rqWorkListDirInf = true;
	}

	return res;
}

//---------------------------------------------------------------------------
//�w��g���q��L�[���[�h�Ƀ}�b�`���鎟�̃t�@�C��������
//---------------------------------------------------------------------------
int find_NextFile(
	TStringList *lst,
	int idx,
	UnicodeString fext,		//�g���q���X�g						(default = EmptyStr)
	UnicodeString keywd,	//�L�[���[�h(���K�\��)				(default = EmptyStr)
	bool skip_dir,			//�f�B���N�g�����X�L�b�v			(default = true)
	bool circular,			//������Ȃ�������擪����Č���	(default = true)
	bool case_sns)			//�召���������					(default = false)
{
	try {
		TRegExOptions opt; if (!case_sns) opt << roIgnoreCase;
		int idx0 = -1, idx1 = -1;
		for (int i=0; i<lst->Count && idx1==-1; i++) {
			if (i<=idx && idx0!=-1) continue;
			file_rec *fp = (file_rec*)lst->Objects[i];
			if ((skip_dir && fp->is_dir) || fp->is_dummy) continue;
			bool match;
			if (!keywd.IsEmpty()) {
				UnicodeString fnam = !fp->alias.IsEmpty()? fp->alias : fp->f_name;
				match = TRegEx::IsMatch(ExtractFileName(fnam), keywd, opt);
			}
			else
				match = (fext.IsEmpty() || test_FileExt(fp->f_ext, fext));
			if (match) {
				if (i<=idx) idx0 = i; else idx1 = i;
			}
		}
		return (idx1!=-1)? idx1 : (circular? idx0 : -1);
	}
	catch (...) {
		return -1;
	}
}
//---------------------------------------------------------------------------
//�w��g���q��L�[���[�h�Ƀ}�b�`����O�̃t�@�C��������
//---------------------------------------------------------------------------
int find_PrevFile(
	TStringList *lst,
	int idx,
	UnicodeString fext,		//�g���q���X�g
	UnicodeString keywd,	//�L�[���[�h(���K�\��)
	bool skip_dir,			//�f�B���N�g�����X�L�b�v
	bool circular,			//������Ȃ�������Ōォ��Č���
	bool case_sns)			//�召���������
{
	try {
		TRegExOptions opt; if (!case_sns) opt << roIgnoreCase;
		int idx0 = -1, idx1 = -1;
		for (int i=lst->Count-1; i>=0 && idx1==-1; i--) {
			if (i>=idx && idx0!=-1) continue;
			file_rec *fp = (file_rec*)lst->Objects[i];
			if ((skip_dir && fp->is_dir) || fp->is_dummy) continue;
			bool match;
			if (!keywd.IsEmpty()) {
				UnicodeString fnam = !fp->alias.IsEmpty()? fp->alias : fp->f_name;
				match = TRegEx::IsMatch(ExtractFileName(fnam), keywd, opt);
			}
			else
				match = (fext.IsEmpty() || test_FileExt(fp->f_ext, fext));
			if (match) {
				if (i>=idx) idx0 = i; else idx1 = i;
			}
		}
		return (idx1!=-1)? idx1 : (circular? idx0 : -1);
	}
	catch (...) {
		return -1;
	}
}

//---------------------------------------------------------------------------
int to_NextFile(TStringList *lst, int idx)
{
	try {
		int idx0 = -1, idx1 = -1;
		for (int i=0; i<lst->Count && idx1==-1; i++) {
			if (i<=idx && idx0!=-1) continue;
			file_rec *fp = (file_rec*)lst->Objects[i];
			if (fp->is_dir || fp->is_dummy || fp->f_attr==faInvalid) continue;
			if (i<=idx) idx0 = i; else idx1 = i;
		}
		return (idx1!=-1)? idx1 : (LoopFilerCursor? idx0 : -1);
	}
	catch (...) {
		return -1;
	}
}
//---------------------------------------------------------------------------
int to_PrevFile(TStringList *lst, int idx)
{
	try {
		int idx0 = -1, idx1 = -1;
		for (int i=lst->Count-1; i>=0 && idx1==-1; i--) {
			if (i>=idx && idx0!=-1) continue;
			file_rec *fp = (file_rec*)lst->Objects[i];
			if (fp->is_dir || fp->is_dummy || fp->f_attr==faInvalid) continue;
			if (i>=idx) idx0 = i; else idx1 = i;
		}
		return (idx1!=-1)? idx1 : (LoopFilerCursor? idx0 : -1);
	}
	catch (...) {
		return -1;
	}
}

//---------------------------------------------------------------------------
//�֘A�t������Ă���A�v���̃��X�g���擾
//---------------------------------------------------------------------------
TStringDynArray get_AssociatedApps(UnicodeString fext)
{
	TStringDynArray ret_array;
	for (int i=0; i<AssociateList->Count; i++) {
		if (test_FileExt(fext, AssociateList->Names[i]))
			add_dyn_array(ret_array, exclude_quot(AssociateList->ValueFromIndex[i]));
	}
	return ret_array;
}

//---------------------------------------------------------------------------
//�֘A�t�����X�g���烁�j���[�p���X�g���쐬
//---------------------------------------------------------------------------
void make_AssoMenuList(TStringDynArray app_lst, TStringList *lst)
{
	for (int i=0; i<app_lst.Length; i++) {
		UnicodeString anam = app_lst[i];
		UnicodeString lbuf;
		//�R�}���h
		if (USTARTS_TI("ExeCommands_", anam))
			lbuf = get_CmdDesc(anam, true);
		//�R�}���h�t�@�C��
		else if (remove_top_AT(anam))
			lbuf = get_base_name(anam);
		//�G�C���A�X
		else if (remove_top_Dollar(anam)) {
			UnicodeString inam;	//�A�C�R��
			//�O���c�[��
			TStringDynArray itm_buf = record_of_csv_list(ExtToolList, anam, 4, EXTTOOL_CSVITMCNT);
			if (itm_buf.Length==EXTTOOL_CSVITMCNT) {
				lbuf = itm_buf[0];
				inam = itm_buf[1];
			}
			//�ǉ����j���[
			else {
				itm_buf = record_of_csv_list(ExtMenuList, anam, 3, EXTMENU_CSVITMCNT);
				if (itm_buf.Length==EXTMENU_CSVITMCNT) {
					lbuf = itm_buf[0];
					inam = itm_buf[5];
				}
			}
			if (lbuf.IsEmpty() || USTARTS_TS('>', lbuf)) continue;
			if (!inam.IsEmpty()) lbuf.cat_sprintf(_T("\t\t%s"), get_actual_path(inam).c_str());
		}
		//�A�v���P�[�V����
		else {
			UnicodeString dsc_str = split_dsc(anam);
			lbuf = def_if_empty(dsc_str, get_base_name(anam));
			lbuf.cat_sprintf(_T("\t\t%s"), anam.c_str());
		}
		lst->Add(lbuf);
	}
}

//---------------------------------------------------------------------------
//�󗓂Ȃ疳���F��
//---------------------------------------------------------------------------
void InvColIfEmpty(TLabeledEdit *ep)
{
	bool flag = ep->NumbersOnly? (ep->Text.ToIntDef(0)==0) : ep->Text.IsEmpty();
	ep->Color = flag? col_Invalid : scl_Window;
}
//---------------------------------------------------------------------------
void InvColIfEmpty(TEdit *ep)
{
	bool flag = ep->NumbersOnly? (ep->Text.ToIntDef(0)==0) : ep->Text.IsEmpty();
	ep->Color = flag? col_Invalid : scl_Window;
}

//---------------------------------------------------------------------------
//�t�@�C�����̐F���擾
//---------------------------------------------------------------------------
TColor get_FileColor(file_rec *fp, TColor col_x)
{
	if (fp->f_attr==faInvalid)		return col_Error;
	if (fp->f_attr & faSysFile) 	return col_System;
	if (fp->f_attr & faHidden)		return col_Hidden;
	if (fp->f_attr & faReadOnly)	return col_ReadOnly;

	if (fp->is_dir) {
		if (fp->is_sym) 			return col_SymLink;
		if (col_Protect!=Graphics::clNone && is_ProtectDir(fp->is_up? fp->p_name : fp->f_name))
									return col_Protect;
									return col_Folder;
	}

	return (ColorOnlyFExt? col_fgList : col_x);
}

//---------------------------------------------------------------------------
//�g���q�ʃJ���[���擾
//---------------------------------------------------------------------------
TColor get_ExtColor(UnicodeString fext)
{
	TColor col_f = col_fgList;
	if (!fext.IsEmpty()) {
		for (int i=0; i<ExtColList->Count; i++) {
			UnicodeString ibuf = ExtColList->Strings[i];
			if (test_FileExt(fext, get_tkn_r(ibuf, ','))) {
				col_f = (TColor)get_tkn(ibuf, ',').ToIntDef(col_fgList);  break;
			}
		}
	}
	//�g���q�Ȃ�(�h�b�g�t�@�C�����܂�)
	else {
		for (int i=0; i<ExtColList->Count; i++) {
			UnicodeString ibuf = ExtColList->Strings[i];
			if (USAME_TS(get_tkn_r(ibuf, ','), ".")) {
				col_f = (TColor)get_tkn(ibuf, ',').ToIntDef(col_fgList);  break;
			}
		}
	}
	return col_f;
}

//---------------------------------------------------------------------------
//�^�C���X�^���v�F���擾
//---------------------------------------------------------------------------
TColor get_TimeColor(TDateTime dt, TColor col_def)
{
	TDateTime dt0 = Now();
	double dd  = dt0 - dt;

	TColor col_t =
			(dd <= 1.0/24) ? col_Tim1H :	//1���Ԉȓ�
			(dd <= 3.0/24) ? col_Tim3H : 	//3���Ԉȓ�
			(dd <= 1.0)	   ? col_Tim1D :	//1���ȓ�
			(dd <= 3.0)	   ? col_Tim3D :	//3���ȓ�
			(dd <= 7.0)	   ? col_Tim7D :	//7���ȓ�
			(dd <= (double)(dt0 - IncMonth(dt0, -1))) ? col_Tim1M :	//1�����ȓ�
			(dd <= (double)(dt0 - IncMonth(dt0, -3))) ? col_Tim3M :	//3�����ȓ�
			(dd <= (double)(dt0 - IncMonth(dt0, -6))) ? col_Tim6M :	//6�����ȓ�
			(dd <= (double)(dt0 - IncYear( dt0, -1))) ? col_Tim1Y :	//1�N�ȓ�
			(dd <= (double)(dt0 - IncYear( dt0, -3))) ? col_Tim3Y :	//3�N�ȓ�
						     						    col_TimOld;

	if (col_t==Graphics::clNone) col_t = col_def;

	return col_t;
}
//---------------------------------------------------------------------------
//�T�C�Y�F���擾
//---------------------------------------------------------------------------
TColor get_SizeColor(__int64 size, TColor col_def)
{
	TColor col_s =
		           (size==-1) ? col_def    :
		            (size==0) ? col_Size0  :
		(size>=4294967296ull) ? col_Size4G :
		(size>=1073741824ull) ? col_Size1G :
		(size>=1073741824ull) ? col_Size1G :
			(size>=1048576ul) ? col_Size1M :
			     (size>=1024) ? col_Size1K :
								col_SizeLT;

	if (col_s==Graphics::clNone) col_s = col_def;

	return col_s;
}

//---------------------------------------------------------------------------
//�f�B���N�g�����܂��̓��[�N���X�g�����擾
//---------------------------------------------------------------------------
UnicodeString get_DirNwlName(UnicodeString pnam)
{
	if (!ends_PathDlmtr(pnam) && test_NwlExt(get_extension(pnam)))
		return ChangeFileExt(ExtractFileName(pnam), EmptyStr);
	else
		return (is_root_dir(pnam)? pnam : get_dir_name(pnam));
}

//---------------------------------------------------------------------------
//�t�@�C�����\����������擾
//---------------------------------------------------------------------------
UnicodeString get_DispName(file_rec *fp)
{
	UnicodeString ret_str;

	if (fp) {
		if (fp->is_dir)
			ret_str.sprintf(_T("[%s]"), get_dir_name(fp->f_name).c_str());
		else if (fp->is_dummy && is_separator(fp->alias))
			ret_str.USET_T("< �Z�p���[�^ >");
		else {
			if (!fp->alias.IsEmpty())
				ret_str.sprintf(_T("%s%s (%s)"), fp->alias.c_str(), fp->f_ext.c_str(), fp->n_name.c_str());
			else
				ret_str = fp->n_name;
		}
	}
	return ret_str;
}
//---------------------------------------------------------------------------
//�o�^���ɒu��
//---------------------------------------------------------------------------
UnicodeString get_RegDirName(UnicodeString pnam)
{
	pnam = IncludeTrailingPathDelimiter(pnam);
	if (SameText(pnam, LibraryPath)) return "���C�u����\\";

	int dlen = 0;
	UnicodeString rnam;
	for (int i=0; i<RegDirList->Count; i++) {
		TStringDynArray itm_buf = get_csv_array(RegDirList->Strings[i], 3, true);
		if (is_separator(itm_buf[1])) continue;
		UnicodeString dnam = get_actual_path(itm_buf[2]);
		if (StartsText(dnam, pnam) && dnam.Length()>dlen) {
			dlen = dnam.Length();
			rnam = itm_buf[1];
		}
	}

	UnicodeString ret_nam;
	if (dlen>0) {
		ret_nam.sprintf(_T("<%s>\\"), rnam.c_str());
		ret_nam += pnam.Delete(1, dlen);
	}
	else
		ret_nam = pnam;
	return ret_nam;
}
//---------------------------------------------------------------------------
//UNC�p�X���l�b�g���[�N�h���C�u���p�X�ɕϊ�
//---------------------------------------------------------------------------
UnicodeString UNC_to_NetDriveName(UnicodeString pnam)
{
	if (USTARTS_TS("\\\\", pnam)) {
		for (int i=0; i<DriveInfoList->Count; i++) {
			drive_info *dp = (drive_info *)DriveInfoList->Objects[i];
			if (dp->unc.IsEmpty()) continue;
			if (StartsText(dp->unc, pnam)) {
				pnam = ReplaceText(pnam, dp->unc, dp->drive_str);
				break;
			}
		}
	}
	return pnam;
}
//---------------------------------------------------------------------------
//�l�b�g���[�N�h���C�u���p�X��UNC�p�X�ɕϊ�
//---------------------------------------------------------------------------
UnicodeString NetDriveName_to_UNC(UnicodeString pnam)
{
	if (!USTARTS_TS("\\\\", pnam)) {
		for (int i=0; i<DriveInfoList->Count; i++) {
			drive_info *dp = (drive_info *)DriveInfoList->Objects[i];
			if (dp->unc.IsEmpty()) continue;
			if (StartsText(dp->drive_str, pnam)) {
				pnam = ReplaceText(pnam, dp->drive_str, dp->unc);
				break;
			}
		}
	}
	return pnam;
}

//---------------------------------------------------------------------------
// *._sf �t�@�C������p�X���擾
//---------------------------------------------------------------------------
UnicodeString get_PathFrom_SF(file_rec *fp)
{
	UnicodeString dnam;
	if (fp->is_virtual && !fp->is_ftp) {
		if (SPI->TestFExt(fp->f_ext, true)) {
			std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
			if (SPI->UnPack(fp->arc_name, fp->f_name, ms.get())) {
				std::unique_ptr<TStringList> o_lst(new TStringList());
				int code_page = get_MemoryCodePage(ms.get());
				if (code_page>0) {
					std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
					ms->Seek(0, soFromBeginning);
					o_lst->LoadFromStream(ms.get(), enc.get());
					if (o_lst->Count>0) dnam = o_lst->Strings[0];
				}
			}
		}
	}
	else
		dnam = get_top_line(fp->f_name);
	dnam = get_tkn_r(dnam, _T("_SF:"));

	return dnam;
}

//---------------------------------------------------------------------------
//�o�^�f�B���N�g���̃p�X���ڂ��擾
//---------------------------------------------------------------------------
UnicodeString get_RegDirItem(int idx)
{
	UnicodeString dnam;
	if (idx>=0 && idx<RegDirList->Count) {
		TStringDynArray itm_buf = get_csv_array(RegDirList->Strings[idx], 3, true);
		if (!itm_buf[2].IsEmpty())
			dnam = exclede_delimiter_if_root(get_actual_path(itm_buf[2]));
	}
	return dnam;
}
//---------------------------------------------------------------------------
UnicodeString get_RegDirItem(WideChar key)
{
	UnicodeString dnam;
	TStringDynArray itm_buf = record_of_csv_list(RegDirList, key, 0, 3);
	if (itm_buf.Length==3 && !itm_buf[2].IsEmpty())
		dnam = exclede_delimiter_if_root(get_actual_path(itm_buf[2]));
	return dnam;
}

//---------------------------------------------------------------------------
//�o�^�f�B���N�g���̎w�荀�ڂ��Z�p���[�^���l�����Đ擪�ֈړ�
//---------------------------------------------------------------------------
void move_top_RegDirItem(int idx)
{
	if (idx>0 && idx<RegDirList->Count) {
		int top = 0;
		for (int i=idx; i>0; i--) {
			TStringDynArray itm_buf = get_csv_array(RegDirList->Strings[i], 3, true);
			if (is_separator(itm_buf[1])) {
				top = i + 1;
				break;
			}
		}
		RegDirList->Move(idx, top);
	}
}

//---------------------------------------------------------------------------
//�t�@�C�����X�g�̏㉺���E���擾
//---------------------------------------------------------------------------
UnicodeString get_LRUD_str(int tag, bool both)
{
	return both ? (DivFileListUD? "�㉺" : "���E")
				: (DivFileListUD? ((tag==0)? "��" : "��") : ((tag==0)? "��" : "�E"));
}

//---------------------------------------------------------------------------
//�폜�������ꂽ�f�B���N�g����?
//---------------------------------------------------------------------------
bool is_ProtectDir(UnicodeString dnam)
{
	if (dnam.IsEmpty()) return false;

	dnam = IncludeTrailingPathDelimiter(dnam);
	bool ret = false;
	for (int i=0; i<ProtectDirList->Count && !ret; i++) {
		UnicodeString prt_nam = IncludeTrailingPathDelimiter(ProtectDirList->Strings[i]);
		ret = StartsText(dnam, prt_nam) || (ProtectSubDir && StartsText(prt_nam, dnam));
	}

	return ret;
}

//---------------------------------------------------------------------------
//�t�@�C����񃊃X�g���擾
//---------------------------------------------------------------------------
void GetFileInfList(
	file_rec *fp,
	bool force)			//�����I�Ɏ擾 (default = false)
{
	if (!fp) return;

	flist_stt *lst_stt = (fp->tag!=-1)? &ListStt[fp->tag] : NULL;

	TStringList *i_list = fp->inf_list;
	i_list->Clear();
	fp->prv_text  = EmptyStr;
	fp->tail_text = EmptyStr;

	//�t�@�C����
	UnicodeString lbuf;
	if (fp->is_up) {
		if (lst_stt) lbuf = lst_stt->is_Find? "<���ʃ��X�g>" : "<�J�����g>";
	}
	else {
		lbuf = fp->b_name + fp->f_ext;
		if (!fp->alias.IsEmpty()) lbuf.cat_sprintf(_T("  <%s>"), fp->alias.c_str());
		//�Ď���?
		if (!fp->is_dir && !fp->is_virtual && !fp->is_ftp) {
			for (int i=0; i<WatchTailList->Count; i++) {
				if (SameText(get_pre_tab(WatchTailList->Strings[i]), fp->f_name)) {
					lbuf.UCAT_T("  (�Ď���)");
					break;
				}
			}
		}
	}
	i_list->Add(lbuf);

	//�p�X��
	lbuf = fp->p_name;
	if (fp->is_up && lst_stt && lst_stt->is_Find) {
		if (lst_stt->find_mark) {
			if (lst_stt->find_SubList->Count==0) lbuf.USET_T("*");
		}
		else {
			lbuf += lst_stt->find_Mask;
			if (!lst_stt->find_Keywd.IsEmpty()) lbuf.cat_sprintf(_T(" : %s"), lst_stt->find_Keywd.c_str());
		}
	}
	else if (fp->is_virtual && !contains_Slash(fp->f_name))
		lbuf.sprintf(_T("%s\\%s"), fp->arc_name.c_str(), lbuf.c_str());
	i_list->Add(lbuf);

	//�����A�T�C�Y�A�^�C���X�^���v
	i_list->Add(get_FileInfStr(fp));

	//�x��
	UnicodeString warn_str = make_PropLine(_T("�x��"), "���O");
	UnicodeString tmp;
	if (fp->f_attr==faInvalid)
		i_list->Add(make_PropLine(_T("�x��"), "���݂��܂���"));
	if (fp->f_name.Length()>=MAX_PATH)
		i_list->Add(tmp.sprintf(_T("%s���������܂�(%u)"), warn_str.c_str(), fp->f_name.Length()));
	if (ContainsStr(fp->f_name, L"\u202e"))
		i_list->Add(tmp.sprintf(_T("%s�ɐ��䕶���uRLO�v���܂܂�Ă��܂�"), warn_str.c_str()));
	if (USTARTS_TS(' ', fp->b_name))
		i_list->Add(tmp.sprintf(_T("%s�̐擪�ɋ󔒂�����܂�"), warn_str.c_str()));
	if (UENDS_TS(' ', fp->b_name))
		i_list->Add(tmp.sprintf(_T("%s�̖����ɋ󔒂�����܂�"), warn_str.c_str()));
	if (contains_s(fp->b_name, _T("  ")))
		i_list->Add(tmp.sprintf(_T("%s�ɘA�������󔒂��܂܂�Ă��܂�"), warn_str.c_str()));

	//�f�B���N�g�������
	if (fp->is_dir && lst_stt) {
		//FTP�����[�g
		if (fp->is_ftp) {
			add_PropLine(_T("���"), (fp->is_sym? "�V���{���b�N�����N" : "�f�B���N�g��"), i_list);
			//�t�@�C�����A�f�B���N�g����
			if (fp->is_up) {
				if (lst_stt->f_cnt!=-1)
					add_PropLine(_T("�t�@�C����"), get_size_str_B(lst_stt->f_cnt, 0), i_list);
				if (lst_stt->d_cnt!=-1)
					add_PropLine(_T("�f�B���N�g����"), get_size_str_B(lst_stt->d_cnt, 0), i_list);
			}
		}
		//���̑�
		else if (fp->tag!=-1) {
			if (fp->is_sym) {
				unsigned int tag;
				UnicodeString lnam = get_ReparsePointTarget(fp->f_name, tag);
				if (tag!=0) {
					add_PropLine(_T("���"),
						(tag==IO_REPARSE_TAG_MOUNT_POINT)? "�W�����N�V����" : "�V���{���b�N�����N", i_list);
					add_PropLine(_T("�����N��"), lnam, i_list);
				}
			}
			else {
				tmp = "�f�B���N�g��";
				if (!fp->is_virtual && is_ProtectDir(fp->is_up? fp->p_name : fp->f_name)) tmp.UCAT_T(" (�폜����)");
				add_PropLine(_T("���"), tmp, i_list);
			}

			//�^�O
			if (!fp->is_virtual) {
				UnicodeString tags = usr_TAG->GetTags(fp->f_name);
				if (!tags.IsEmpty()) add_PropLine(_T("�^�O"), tags, i_list);
			}

			//�t�@�C���A�f�B���N�g����
			int f_cnt  = fp->is_up? lst_stt->f_cnt : fp->f_count;
			int d_cnt  = fp->is_up? lst_stt->d_cnt : fp->d_count;
			__int64 f_size	 = fp->is_up? lst_stt->cur_total : fp->f_size;
			__int64 o_size	 = fp->is_up? lst_stt->occ_total : fp->o_size;
			__int64 drv_size = lst_stt->drive_Total;
			if (f_cnt!=-1) {
				lbuf = make_PropLine(_T("�t�@�C����"), get_size_str_B(f_cnt, 0));
				if (fp->is_up && lst_stt->sub_counted)
					lbuf.cat_sprintf(_T(" / %s"), get_size_str_B(lst_stt->f_cnt_total, 0).c_str());
				i_list->Add(lbuf);
			}
			if (d_cnt!=-1) {
				lbuf = make_PropLine(_T("�f�B���N�g����"), get_size_str_B(d_cnt, 0));
				if (fp->is_up && lst_stt->sub_counted)
					lbuf.cat_sprintf(_T(" / %s"), get_size_str_B(lst_stt->d_cnt_total, 0).c_str());
				i_list->Add(lbuf);
			}

			//�T�C�Y
			if (f_size>0) {
				i_list->Add(get_PropTitle(_T("���v�T�C�Y")).cat_sprintf(_T("%s (%s)"),
					get_size_str_G(f_size, 10, SizeDecDigits, 1).c_str(), get_size_str_B(f_size, 5).c_str()));
				if (!fp->is_virtual && o_size>0) {
					i_list->Add(get_PropTitle(_T("��L�T�C�Y")).cat_sprintf(_T("%s (%s)"),
						get_size_str_G(o_size, 10, SizeDecDigits, 1).c_str(), get_size_str_B(o_size, 5).c_str()));
					__int64 g_size = o_size - f_size;
					i_list->Add(get_PropTitle(_T("�N���X�^gap")).cat_sprintf(_T("%s (%s) %4.1f%%"),
						get_size_str_G(g_size, 10, SizeDecDigits, 1).c_str(), get_size_str_B(g_size, 5).c_str(),
						100.0 * g_size/o_size));
					if (drv_size>0)
						i_list->Add(get_PropTitle(_T("�h���C�u��L��")).cat_sprintf(_T("%6.2f%%"), 100.0 * o_size/drv_size));
				}
			}

			//����
			if (!fp->is_virtual) {
				UnicodeString cfg_nam = get_dotNaynfi(fp->is_up? fp->p_name : fp->f_name);
				if (file_exists(cfg_nam)) {
					std::unique_ptr<TStringList> cfg_lst(new TStringList());
					load_text_ex(cfg_nam, cfg_lst.get());
					add_PropLine_if(_T("����"), cfg_lst->Values["Description"], i_list);
				}
			}
		}

		//�g���q�ʃt�@�C����
		if (fp->is_up) {
			TStringList *lst = GetFileList(fp->tag);
			add_FExtInfList(lst, i_list);
		}

		//����
		UnicodeString memo = IniFile->GetMarkMemo(fp->r_name);
		if (!memo.IsEmpty()) {
			i_list->Add(EmptyStr);	//�Z�p���[�^
			add_PropLine(_T("����"), memo, i_list);
		}
	}

	if (fp->is_dir) return;

	fp->is_video = test_FileExt(fp->f_ext, FEXT_VIDEO);

	if (!force) {
		//�t�@�C�������擾���Ȃ��p�X�̃`�F�b�N
		if (is_NoInfPath(fp, NoInfPath)) return;
		//�t�@�C�������擾����g���q�̃`�F�b�N
		if (!FExtGetInf.IsEmpty() && !test_FileExt(fp->f_ext, FExtGetInf)) return;
		//�t�@�C�������擾���Ȃ��g���q�̃`�F�b�N
		if (test_FileExt(fp->f_ext, FExtNoInf)) return;
		//RAR�ɑ΂��鐧��
		if (LimitGetRarInf && test_FileExt(fp->f_ext, _T(".rar")) && fp->f_size>GetInfLimitSize) return;
		//��������?
		if (is_Processing(fp)) return;
	}

	//--------------------------
	//�t�@�C�������擾
	//--------------------------
	std::unique_ptr<TStringList> i_lst(new TStringList());
	//._SF
	if (test_FileExt(fp->f_ext, _T("._sf"))) {
		add_PropLine_if(_T("�p�X"), get_PathFrom_SF(fp), i_list);
	}
	//���̑�
	else if (get_FileInfList(fp, i_lst.get()))
		i_list->AddStrings(i_lst.get());
}

//---------------------------------------------------------------------------
//�w�肵���t�@�C�����̒l��������擾
//---------------------------------------------------------------------------
UnicodeString get_FileInfValue(
	file_rec *fp,
	UnicodeString tit,		//�^�C�g��
	UnicodeString dlmt)		// dlmt: �l�̋�؂蕶�� (default = EmptyStr)
{
	UnicodeString ret_str;

	for (int i=3; i<fp->inf_list->Count; i++) {
		UnicodeString lbuf = fp->inf_list->Strings[i];
		if (StartsText(UAPP_T(tit, ":"), Trim(lbuf))) {
			ret_str = get_tkn_r(lbuf, ':');
			if (!dlmt.IsEmpty()) ret_str = get_tkn(ret_str, ';');
			ret_str = Trim(ret_str);
			break;
		}
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//�t�@�C�����̕`��
//---------------------------------------------------------------------------
void draw_InfListBox(TListBox *lp, TRect &Rect, int Index, TOwnerDrawState State)
{
	TCanvas  *cv = lp->Canvas;
	cv->Font->Assign(FileInfFont);
	bool is_irreg = IsIrregularFont(cv->Font);

	cv->Brush->Color = (State.Contains(odSelected) && lp->Focused())? col_selItem : col_bgInf;
	cv->FillRect(Rect);

	int xp = Rect.Left + 2;
	int yp = Rect.Top + 1;
	UnicodeString lbuf = lp->Items->Strings[Index];
	bool use_fgsel = lp->Focused() && is_SelFgCol(State);

	//��{���
	if ((lp->Tag & LBTAG_OPT_FIF1) && Index<3) {
		cv->Font->Color = use_fgsel? col_fgSelItem : col_fgInf;
		if (Index==1)
			PathNameOut(lbuf, cv, xp, yp);
		else
			cv->TextOut(xp, yp, lbuf);
	}
	//�t�@�C�����
	else {
		int w_max = 0;
		int idx0 = (lp->Tag & LBTAG_OPT_FIF1)? 3: 0;
		for (int i=idx0; i<lp->Count; i++)
			w_max = std::max(w_max, get_TextWidth(cv, get_tkn(lp->Items->Strings[i], _T(": ")), is_irreg));

		//�Z�p���[�^
		if (lbuf.IsEmpty())
			draw_separateLine(cv, Rect, 1);
		//����
		else {
			//���O
			UnicodeString namstr = split_tkn(lbuf, _T(": "));
			xp = Rect.Left + 2 + w_max - get_TextWidth(cv, namstr, is_irreg);
			cv->Font->Color = use_fgsel? col_fgSelItem :
				USTARTS_TS('.', Trim(namstr))? get_ExtColor(get_tkn_r(namstr, '.')) :
				contains_wd_i(namstr, _T("�G���[|�x��"))? col_Error : col_fgInfNam;
			namstr.UCAT_T(": ");
			cv->TextOut(xp, yp, namstr);	xp += get_TextWidth(cv, namstr, is_irreg);
			//���e
			if ((lp->Tag & LBTAG_OPT_FIF2) && contains_s(namstr, _T("����:"))) {
				try {
					cv->Font->Color = use_fgsel? col_fgSelItem : get_TimeColor(TDateTime(lbuf), col_fgInf);
				}
				catch (EConvertError &e) {
					cv->Font->Color = use_fgsel? col_fgSelItem : col_fgInf;
				}
			}
			else if (USAME_TS(Trim(namstr), "�^�O:"))
				cv->Font->Color = use_fgsel? col_fgSelItem : col_fgTagNam;
			else if (USAME_TS(Trim(namstr), "���s�R�[�h:"))
				cv->Font->Color = use_fgsel? col_fgSelItem : USAME_TS(lbuf, "����")? col_Error : col_fgInf;
			else if (USAME_TS(Trim(namstr), "�f�o�b�O���:"))
				cv->Font->Color = use_fgsel? col_fgSelItem : contains_s(lbuf, _T("�I��"))? col_Error : col_fgInf;
			else {
				if (use_fgsel)
					cv->Font->Color = col_fgSelItem;
				else {
					if (lbuf.Pos("://")) {
						TMatch mt = TRegEx::Match(lbuf, URL_MATCH_PTN);
						cv->Font->Color = mt.Success? col_URL : col_fgInf;
					}
					else
						cv->Font->Color = col_fgInf;
				}
			}

			if ((lp->Tag & LBTAG_FIF_LIST) && USAME_TS(Trim(namstr), "�ꏊ:"))
				PathNameOut(lbuf, cv, xp, yp);
			else
				EmphasisTextOut(lbuf, EmptyStr, cv, xp, yp);
		}
	}
}

//---------------------------------------------------------------------------
//�J���[�ݒ胊�X�g�̕`��
//---------------------------------------------------------------------------
void draw_ColorListBox(TListBox *lp, TRect &Rect, int Index, TOwnerDrawState State, TStringList *col_lst)
{
	TCanvas  *cv = lp->Canvas;
	int yp = Rect.Top + get_TopMargin(cv);

	UnicodeString col_nam = lp->Items->Names[Index];
	UnicodeString vbuf	  = lp->Items->ValueFromIndex[Index];
	bool brk = remove_top_s(vbuf, '|');

	SetHighlight(cv, State.Contains(odSelected));
	cv->FillRect(Rect);
	cv->TextOut(Rect.Left + 34, yp, vbuf);

	//�J���[
	TRect rc = Rect;  rc.Right = rc.Left + 30;
	cv->Brush->Color = (TColor)col_lst->Values[col_nam].ToIntDef(clBlack);
	if (cv->Brush->Color!=Graphics::clNone)
		cv->FillRect(rc);
	else {
		cv->Brush->Color = scl_BtnFace;
		cv->FillRect(rc);
		out_Text(cv, rc.Left + 2, yp, _T("����"), scl_WindowText);
	}

	//���E��
	cv->Brush->Color = scl_Window;
	if (brk) draw_separateLine(cv, Rect);
}

//---------------------------------------------------------------------------
//���͗��̕`��(�^���L�����b�g�t��)
//---------------------------------------------------------------------------
void draw_InputPaintBox(TPaintBox *pp, UnicodeString s)
{
	TCanvas *cv = pp->Canvas;
	cv->Font->Assign(pp->Font);
	cv->Brush->Color = col_bgList;
	cv->FillRect(pp->ClientRect);

	int xp = ScaledInt(2);
	int yp = get_TopMargin(cv);
	cv->Font->Color = col_fgList;
	cv->TextOut(xp, yp, s);

	xp += get_TextWidth(cv, s, IsIrregularFont(cv->Font));

	//�L�����b�g
	draw_Line(cv, xp, 2, xp, pp->ClientHeight-2, ScaledInt(1), col_Cursor);
}

//---------------------------------------------------------------------------
//�t�@�C���T�C�Y�𕶎����
//---------------------------------------------------------------------------
UnicodeString get_FileSizeStr(__int64 sz, bool trim)
{
	UnicodeString ret_str = ShowByteSize ? get_size_str_B(sz, 14)
										 : get_size_str_G(sz, ((SizeFormatMode>0)? 7 : 10), SizeDecDigits, SizeFormatMode);
	if (trim) ret_str = Trim(ret_str);
	return ret_str;
}

//---------------------------------------------------------------------------
//�t�@�C����񕶎�����擾
//---------------------------------------------------------------------------
UnicodeString get_FileInfStr(file_rec *fp,
	bool with_atr)	//������t��	(default = true)
{
	if (!fp) return EmptyStr;

	UnicodeString ret_str = with_atr? fp->attr_str : EmptyStr;

	if (fp->f_attr!=faInvalid) {
		if (!ret_str.IsEmpty()) ret_str.UCAT_T(" ");
		ret_str += get_FileSizeStr(fp->f_size);
		if (!ShowByteSize && fp->f_size!=-1 && UENDS_TS('B', ret_str))
			ret_str.cat_sprintf(_T(" (%s)"), get_size_str_B(fp->f_size, 0).c_str());

		ret_str += ("  " + format_DateTime(fp->f_time));
	}

	return ret_str;
}
//---------------------------------------------------------------------------
UnicodeString get_FileInfStr(UnicodeString fnam, bool with_atr)
{
	file_rec *fp = cre_new_file_rec(fnam);
	UnicodeString ret_str = get_FileInfStr(fp, with_atr);
	del_file_rec(fp);
	return ret_str;
}

//---------------------------------------------------------------------------
void get_FileNamePathInf(UnicodeString fnam, TStringList *lst, bool get_app)
{
	lst->Add(ExtractFileName(fnam));
	lst->Add(ExtractFilePath(fnam));

	if (file_exists(fnam)) {
		lst->Add(get_FileInfStr(fnam));
		if (get_app) get_AppInf(fnam, lst, false);
	}
	else
		lst->Add(LoadUsrMsg(USTR_NotFound));
}

//---------------------------------------------------------------------------
//�g���q�ʃt�@�C��������ǉ�
//---------------------------------------------------------------------------
void add_FExtInfList(
	TStringList *f_lst,	//[i] �t�@�C�������X�g (file_rec �t�ɂ��Ή�)
	TStringList *lst)	//[o] ��񃊃X�g
{
	std::unique_ptr<TStringList> x_lst(new TStringList());
	if (f_lst->Count>0) {
		for (int i=0; i<f_lst->Count; i++) {
			file_rec *fp = (file_rec*)f_lst->Objects[i];
			if (fp && (fp->is_dir || fp->is_dummy)) continue;

			UnicodeString xnam = def_if_empty(get_extension(f_lst->Strings[i]).LowerCase(), _T("."));
			int idx = x_lst->IndexOf(xnam);
			if (idx!=-1)
				x_lst->Objects[idx] = (TObject*)((int)x_lst->Objects[idx] + 1);
			else
				x_lst->AddObject(xnam, (TObject*)1);
		}

		//�ǉ�
		if (x_lst->Count>0) {
			x_lst->Sort();
			lst->Add(EmptyStr);
			for (int i=0; i<x_lst->Count; i++)
				lst->Add(get_PropTitle(x_lst->Strings[i]).cat_sprintf(_T("%u"), (int)x_lst->Objects[i]));
		}
	}
}

//---------------------------------------------------------------------------
//�t�@�C����񃊃X�g���擾
//---------------------------------------------------------------------------
bool get_FileInfList(
	file_rec *fp,
	TStringList *lst)	//[o] ��񃊃X�g
{
	if (!fp) return false;

	try {
		lst->Clear();

		UnicodeString fnam = (fp->is_virtual || fp->is_ftp)? fp->tmp_name : fp->f_name;
		if (!file_exists(fnam)) Abort();

		//----------------------
		//���ڂ̎��
		//----------------------
		UnicodeString tnam = usr_SH->get_FileTypeStr(fnam, true);
		//NyanFi �ŗL�̃t�@�C��
		if (SameText(fnam, IniFile->FileName))					tnam.UCAT_T(" [�g�p���̐ݒ�]");
		else if (SameText(fnam, rel_to_absdir(ReplaceLogName)))	tnam.UCAT_T(" [������u�����O]");
		else if (SameText(fnam, rel_to_absdir(GrepFileName)))	tnam.UCAT_T(" [GREP���O]");
		else if (SameText(ExtractFilePath(fnam), ExePath)) {
			UnicodeString nnam = ExtractFileName(fnam);
			if		(SameText(nnam, TAGDATA_FILE))	tnam.UCAT_T(" [�^�O�f�[�^]");
			else if (SameText(nnam, HILT_INI_FILE))	tnam.UCAT_T(" [�\�������\����`]");
			else if (SameText(nnam, DIR_HIST_FILE))	tnam.UCAT_T(" [�S�̃f�B���N�g������]");
			else if (SameText(nnam, DISTR_FILE))	tnam.UCAT_T(" [�U�蕪���o�^]");
			else if (SameText(nnam, CALC_INI_FILE))	tnam.UCAT_T(" [�d��̒�`]");
			else if (SameText(nnam, WEBMAP_TPLT))	tnam.UCAT_T(" [�}�b�v�\���e���v���[�g]");
			else if (SameText(nnam, DRVLOG_FILE))	tnam.UCAT_T(" [�h���C�u�e�ʃ��O]");
			else if (SameText(nnam, RENLOG_FILE))	tnam.UCAT_T(" [�������O]");
			else if (SameText(nnam, "NyanFi.exe"))	tnam.UCAT_T(" [���s��]");
			else if (is_match_regex_i(nnam, _T("^tasklog\\d?(~\\d)?\\.txt"))) tnam.UCAT_T(" [�^�X�N���O]");
		}

		add_PropLine(_T("���"), tnam, lst);

		//----------------------
		//�^�O
		//----------------------
		if (!fp->is_virtual && !fp->is_ftp) {
			UnicodeString tags = usr_TAG->GetTags(fnam);
			if (!tags.IsEmpty()) add_PropLine(_T("�^�O"), tags, lst);
		}

		UnicodeString fext = fp->f_ext;
		//------------------------------
		//�֘A�Â�����Ă���v���O����
		//------------------------------
		if (!test_FileExt(fext, FEXT_EXECUTE FEXT_APPINFO _T(".nbt"))) {
			_TCHAR pszFile[MAX_PATH*2];
			DWORD dwOut = MAX_PATH*2;
			if (::AssocQueryString(ASSOCF_NOTRUNCATE, ASSOCSTR_EXECUTABLE,
				fext.c_str(), _T("open"), pszFile, &dwOut)==S_OK)
			{
				lst->Add(UnicodeString().sprintf(_T("%s%s  /%s"),
					get_PropTitle(_T("�v���O����")).c_str(),
					ExtractFileName(pszFile).c_str(), ExtractFileDir(pszFile).c_str()));
			}
		}

		//----------------------
		//�t�@�C�����
		//----------------------
		std::unique_ptr<TStringList> ref_lst(new TStringList());

		if (test_AppInfExt(fext))			get_AppInf(fnam, lst);
		//�A�C�R��
		else if (test_IcoExt(fext))			get_IconInf(fnam, lst);
		//WebP
		else if (USAME_TI(fext, ".webp"))	get_WebpInf(fnam, lst);
		//�\���\�ȉ摜
		else if (is_Viewable(fp)) {
			if (test_MetaExt(fext))
				get_MetafileInf(fnam, lst);
			else {
				bool x_flag = false;
				if (test_ExifExt(fext))			 	x_flag = get_ExifInf(fnam, lst, &fp->img_ori);
				if (!x_flag && test_PngExt(fext))	x_flag = get_PngInf(fnam, lst);
				if (!x_flag && test_GifExt(fext))	x_flag = get_GifInf(fnam, lst);
				if (!x_flag) {
					unsigned int i_wd, i_hi;
					if (!contains_s(lst->Text, _T("�摜�T�C�Y: ")) && get_img_size(fnam, &i_wd, &i_hi))
						lst->Add(get_img_size_str(i_wd, i_hi));
					usr_SH->get_PropInf(fnam, lst);
				}
				if (test_JpgExt(fext)) get_JpgExInf(fnam, lst);
			}
		}
		//Exif
		else if (test_ExifExt(fext)) get_ExifInf(fnam, lst, &fp->img_ori);
		//WAV
		else if (USAME_TI(fext, ".wav")) {
			get_WavInf(fnam, lst);
			UnicodeString tmp = "�r�b�g ���[�g";
			if (!contains_s(lst->Text, _T("����: "))) tmp.Insert("����,", 1);
			usr_SH->get_PropInf(fnam, lst, tmp);
		}
		//MP3
		else if (test_Mp3Ext(fext)) {
			usr_SH->get_PropInf(fnam, lst, "����,�r�b�g ���[�g");
			ID3_GetInf(fnam, lst);
		}
		//Opus
		else if (USAME_TI(fext, ".opus")) {
			usr_SH->get_PropInf(fnam, lst, "����");
			get_OpusInf(fnam, lst);
		}
		//CDA
		else if (USAME_TI(fext, ".cda")) {
			get_CdaInf(fnam, lst);
		}
		//�V���[�g�J�b�g
		else if (test_LnkExt(fext)) {
			UnicodeString lnam;
			usr_SH->get_LnkInf(fnam, lst, &lnam);
			//�����N��̃t�@�C�����
			if (!lnam.IsEmpty() && file_exists(lnam) && !dir_exists(lnam) && test_AppInfExt(ExtractFileExt(lnam))) {
				lst->Add(EmptyStr);	//�Z�p���[�^
				get_AppInf(lnam, lst);
			}
		}
		else if (USAME_TI(fext, ".url")) {
			std::unique_ptr<UsrIniFile> url_file(new UsrIniFile(fnam));
			add_PropLine(_T("URL"), url_file->ReadString("InternetShortcut", "URL"), lst);
			add_PropLine_if(_T("IconFile"), url_file->ReadString("InternetShortcut", "IconFile"), lst);
		}
		//�A�[�J�C�u
		else if (test_ArcExt(fext)) {
			if (xd2tx_PropExt(fext)) {
				if (xd2tx_GetInfo(fnam, lst)) lst->Add(EmptyStr);
			}

			int fcnt;
			__int64 org_size;
			float ratio;
			bool is_enc;
			std::unique_ptr<TStringList> r_lst(new TStringList());
			if (usr_ARC->GetArcInfo(fnam, &fcnt, &org_size, &ratio, &is_enc, r_lst.get())) {
				add_PropLine(_T("�t�@�C����"), get_size_str_B(fcnt, 0), lst);
				if (org_size>0) add_PropLine(_T("���T�C�Y"), get_size_str_G(org_size, 8, SizeDecDigits), lst);
				if (ratio>0)	lst->Add(get_PropTitle(_T("���k��")).cat_sprintf(_T("%5.1f %"), ratio));
				if (is_enc)		lst->Add("�p�X���[�h�ی�");
				//�g���q�ʃt�@�C����
				add_FExtInfList(r_lst.get(), lst);
			}
			else {
				add_list_errmsg(lst, "�s���ȃA�[�J�C�u�ł�");
				InhReload++;	//�G���[�̌J��Ԃ���h������
			}
		}
		//XML
		else if (test_FileExt(fext, FEXT_XML)) {
			get_xml_inf(fnam, lst);
			if (test_LibExt(fext)) get_LibraryList(fnam, lst, true);
		}
		//HTML����
		else if (test_HtmlExt(fext)) get_HtmlInf(fnam, lst);
		//�X�^�C���V�[�g
		else if (USAME_TI(fext, ".css")) {
			UnicodeString lbuf = Trim(get_top_line(fnam));
			if (USTARTS_TI("@charset", lbuf))
				add_PropLine(_T("charset"), exclude_quot(Trim(get_tkn(lbuf.Delete(1, 8), ';'))), lst);
		}
		//C++Builder �v���W�F�N�g�A�\�[�X
		else if (test_FileExt(fext, _T(".cbproj.dproj.cpp.pas.dfm.fmx.h")))
			get_BorlandInf(fnam, lst);
		//NyanFi �R�}���h�t�@�C��
		else if (test_NbtExt(fext)) {
			UnicodeString lbuf = Trim(get_top_line(fnam));
			if (remove_top_s(lbuf, ';')) add_PropLine(_T("����"), lbuf, lst);
			get_ref_CmdFile(fnam, ref_lst.get());
		}
		//�t�H���g
		else if (test_FontExt(fext)) get_FontInf(fnam, lst);
		//xdoc2txt
		else if (xd2tx_PropExt(fext)) xd2tx_GetInfo(fnam, lst);
		//tags
		else if (USAME_TI(fp->n_name, "tags")) get_TagsInf(fnam, lst);
		//���̑�
		else if (!test_FileExt(fext, _T(".pdf.nwl"))) {
			//����
			if (USAME_TS(usr_SH->get_PropInf(fnam, lst), "�r�f�I")) fp->is_video = true;
			//���s�\�t�@�C��(�`�F�b�N)
			else get_AppInf(fnam, lst);
		}

		//����
		UnicodeString memo = IniFile->GetMarkMemo(fp->r_name);
		if (!memo.IsEmpty()) {
			if (lst->Count>0 && !lst->Strings[lst->Count - 1].IsEmpty()) lst->Add(EmptyStr);	//�Z�p���[�^
			add_PropLine(_T("����"), memo, lst);
		}

		//----------------------
		//�e�L�X�g�v���r���[
		//----------------------
		fp->prv_text  = EmptyStr;
		fp->tail_text = EmptyStr;
		int code_page = 0;
		UnicodeString line_brk;
		bool has_bom;
		bool is_text = is_TextFile(fnam, &code_page, &line_brk, &has_bom);

		if (ShowTextPreview) {
			//���b�`�e�L�X�g
			if (test_FileExt(fext, _T(".rtf.wri"))) {
				if (TempRichEdit) {
					TempRichEdit->Lines->LoadFromFile(fnam);
					fp->prv_text = TempRichEdit->Lines->Text;
					//�������e��ݒ�
					if (ShowTailPreview && PrvTxtTailLn>0) {
						int tp = std::max(TempRichEdit->Lines->Count - PrvTxtTailLn, 0);
						for (int i=tp; i<TempRichEdit->Lines->Count; i++)
							fp->tail_text += TempRichEdit->Lines->Strings[i] + "\r\n";
					}
				}
			}
			//�ʏ�e�L�X�g
			else if (is_text) {
				std::unique_ptr<TStringList> f_buf(new TStringList());
				int lmit_size = (PrvTxtInfLn>0)? std::max(PrvTxtInfLn*1024, TXT_DETECT_SIZE) : 0;		//***
				double ave_lsz = 0;
				code_page = load_text_ex(fnam, f_buf.get(), 0, lmit_size, false, NULL, &ave_lsz);
				int ln_cnt = f_buf->Count;

				//�G���[
				if (!GlobalErrMsg.IsEmpty())
					add_list_errmsg(lst, GlobalErrMsg);
				else {
					//XML
					if (ln_cnt>0) {
						UnicodeString lbuf = f_buf->Strings[0];
						if (contains_i(lbuf, _T("<?xml "))) {
							if (!contains_i(lst->Text, _T("XML version:"))) get_xml_inf(lbuf, lst);
						}
					}

					//�R�[�h�y�[�W
					if (code_page>0) {
						UnicodeString cp_str = make_PropLine(_T("�R�[�h�y�[�W"), code_page);
						UnicodeString cnam	 = get_NameOfCodePage(code_page);
						if (!cnam.IsEmpty()) {
							cp_str.cat_sprintf(_T(" / %s"), cnam.c_str());
							if (has_bom) cp_str.UCAT_T(" BOM�t");
						}
						lst->Add(cp_str);
					}

					//���s�R�[�h
					add_PropLine_if(_T("���s�R�[�h"), line_brk, lst);

					//�s��
					bool over = (PrvTxtInfLn>0 && ln_cnt>PrvTxtInfLn);
					if (over) {
						if (ave_lsz>0 && PrvTxtInfLn>=1000) {
							int n = fp->f_size / ave_lsz;
							n -= n % (int)pow(10, floor(log10(n/10.0)));
							add_PropLine(_T("����s��"), "��" + get_size_str_B(n, 0), lst);
						}
					}
					else {
						add_PropLine(_T("�s��"),   get_size_str_B(ln_cnt, 0), lst);
						add_PropLine(_T("������"), get_size_str_B(f_buf->Text.Length(), 0), lst);
					}

					//�v���r���[���e��ݒ�
					if (over) {
						for (int i=0; i<PrvTxtInfLn; i++) fp->prv_text += f_buf->Strings[i] + "\r\n";
						fp->prv_text += "\r\n";	//�ő�s��+1�ɂ��Ē��ߔ���
					}
					else
						fp->prv_text = f_buf->Text;

					//�������e��ݒ�
					if (ShowTailPreview && PrvTxtTailLn>0) {
						if (over) {
							if (load_text_tail(fnam, f_buf.get(), code_page, PrvTxtTailLn)!=0) fp->tail_text = f_buf->Text;
						}
						else {
							int tp = std::max(ln_cnt - PrvTxtTailLn, 0);
							for (int i=tp; i<ln_cnt; i++) fp->tail_text += f_buf->Strings[i] + "\r\n";
						}
					}
				}
			}
		}
		else if (is_text) {
			//�R�[�h�y�[�W
			if (code_page>0) {
				UnicodeString cp_str = make_PropLine(_T("�R�[�h�y�[�W"), code_page);
				UnicodeString cnam = get_NameOfCodePage(code_page);
				if (!cnam.IsEmpty()) {
					cp_str.cat_sprintf(_T(" / %s"), cnam.c_str());
					if (has_bom) cp_str.UCAT_T(" BOM�t");
				}
				lst->Add(cp_str);
			}
			//���s�R�[�h
			add_PropLine_if(_T("���s�R�[�h"), line_brk, lst);
		}

		//�R�}���h�t�@�C���̎Q�Ə��
		if (ref_lst->Count>0) {
			lst->Add(EmptyStr);
			lst->AddStrings(ref_lst.get());
		}

		return true;
	}
	catch (...) {
		return false;
	}
}

//---------------------------------------------------------------------------
//���ڂ͑I���\���H
//---------------------------------------------------------------------------
bool is_selectable(file_rec *fp)
{
	if (!fp)						return false;
	if (fp->is_dummy && is_separator(fp->alias))	return true;	//�Z�p���[�^
	if (fp->is_dummy || fp->is_up)	return false;
	if (fp->b_name.IsEmpty())		return false;
	return true;
}
//---------------------------------------------------------------------------
//�I���\�Ȃ�I��
//---------------------------------------------------------------------------
bool set_select(file_rec *fp, bool sw)
{
	if (is_selectable(fp)) fp->selected = sw;
	return fp? fp->selected : false;
}
//---------------------------------------------------------------------------
bool set_select_r(file_rec *fp)
{
	if (is_selectable(fp)) fp->selected = !fp->selected;
	return fp? fp->selected : false;
}

//---------------------------------------------------------------------------
//�t�@�C������������?
//---------------------------------------------------------------------------
bool is_Processing(file_rec *fp)
{
	bool flag = false;
	if (!fp->is_dir && (fp->f_attr & faReadOnly)==0) {
		UnicodeString fnam = (fp->is_virtual || fp->is_ftp)? fp->tmp_name : fp->f_name;
		if (!fnam.IsEmpty()) {
			DWORD acc_mod = GENERIC_READ;  if (fp->is_video) acc_mod |= GENERIC_WRITE;
			HANDLE hFile = ::CreateFile(cv_ex_filename(fnam).c_str(),
				acc_mod, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile!=INVALID_HANDLE_VALUE)
				::CloseHandle(hFile);
			else
				flag = true;
		}
	}
	return flag;
}

//---------------------------------------------------------------------------
//�擾���}�~����Ă���p�X���H
//---------------------------------------------------------------------------
bool is_NoInfPath(file_rec *fp, UnicodeString no_path)
{
	TStringDynArray path_lst = split_strings_semicolon(no_path);
	for (int i=0; i<path_lst.Length; i++) {
		UnicodeString pnam = path_lst[i];
		if (remove_top_Dollar(pnam)) {
			if (pnam.IsEmpty()) continue;
			pnam = pnam.UpperCase();
			UnicodeString dstr = get_drive_str(fp->p_name);
			if (USTARTS_TS("\\\\", pnam)) continue;
			int idx = DriveInfoList->IndexOf(dstr);
			if (idx==-1) continue;
			drive_info *dp = (drive_info *)DriveInfoList->Objects[idx];
			WideChar c0 = pnam[1];
			if (c0=='H' && dp->drv_type==DRIVE_FIXED && !dp->is_SSD && !dp->is_RAM) return true;
			if (c0=='C' && dp->drv_type==DRIVE_CDROM)		return true;
			if (c0=='N' && dp->drv_type==DRIVE_REMOTE)		return true;
			if (c0=='R' && dp->drv_type==DRIVE_REMOVABLE)	return true;
			if (c0=='U' && USAME_TI(dp->bus_type, "USB")) {
				if (pnam.Length()>1) {
					WideChar c1 = pnam[2];
					if (c0=='H' && dp->drv_type==DRIVE_FIXED && !dp->is_SSD) return true;
					if (c1=='C' && dp->drv_type==DRIVE_CDROM)		return true;
					if (c1=='R' && dp->drv_type==DRIVE_REMOVABLE)	return true;
				}
				else return true;
			}
		}
		else {
			pnam = IncludeTrailingPathDelimiter(pnam);
			if (pnam.IsEmpty()) continue;
			if (StartsText(pnam, fp->p_name)) return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------------
//UTF8�`�F�b�N
//---------------------------------------------------------------------------
int check_UTF8(BYTE *bp, int size)
{
	int cnt = 0;
	try {
		int i = 0;
		while (i<size) {
			if (bp[i]<0x80)
				i++;
			else {
				int bn;
				if		(0xc2<=bp[i] && bp[i]<=0xdf) bn = 1;
				else if (0xe0<=bp[i] && bp[i]<=0xef) bn = 2;
				else if (0xf0<=bp[i] && bp[i]<=0xf4) bn = 3;
				else Abort();
				i++;	if (i==size) break;
				for (int j=0; j<bn && i<size; j++) {
					if (0x80<=bp[i] && bp[i]<=0xbf) {
						i++;	if (i==size) break;
					}
					else Abort();
				}
				cnt++;
			}
		}
	}
	catch (...) {
		cnt = 0;
	}
	return cnt;
}

//---------------------------------------------------------------------------
//�������X�g���[���̃R�[�h�y�[�W���擾
//  �߂�l: �R�[�h�y�[�W (0:�s��/ -1:�o�C�i��)
//---------------------------------------------------------------------------
int get_MemoryCodePage(
	TMemoryStream *ms,
	bool *has_bom)		//[o] BOM�L��	(default = NULL)
{
	int code_page = 0;
	if (has_bom) *has_bom = false;

	int src_size = ms->Size;
	if (src_size==0) return 932;

	//�\������
	BYTE *bp = (BYTE*)ms->Memory;
	int x00_cnt	= 0;	//0x00 �̌�
	int z_rpt	= 0;	//0x00 �̘A����
	int z_max	= 0;	//0x00 �̍ő�A����
	int esc_cnt = 0;	//ESC �̌�
	int asc_cnt = 0;	//ASCII �̌�
	int ltn_cnt = 0;	//ISO8859(Latin-1��z��) �̌�
	int b7_cnt	= 0;	//b7==1 �̌�

	for (int i=0; i<src_size; i++) {
		BYTE b0 = bp[i];
		if (b0==0x00) {
			x00_cnt++; z_rpt++;
		}
		else {
			if (z_rpt>z_max) z_max = z_rpt;
			z_rpt = 0;
			if		(b0==0x1b) 			 esc_cnt++;
			else if (0x06<b0 && b0<0x7f) asc_cnt++;
			else if (0xa0<=b0)			 ltn_cnt++;
			if (b0 & 0x80)				 b7_cnt++;
		}
	}
	if (z_max>3 || (src_size<3 && x00_cnt>0)) return -1;	//�o�C�i���[

	//BOM�̗L�����`�F�b�N
	if		(bp[0]==0xfe && bp[1]==0xff)				code_page = 1201;		//UTF-16BE BOM�t
	else if (bp[0]==0xff && bp[1]==0xfe)				code_page = 1200;		//UTF-16 (LE) BOM�t
	else if (bp[0]==0xef && bp[1]==0xbb && bp[2]==0xbf)	code_page = CP_UTF8;	//UTF-8 BOM�t

	if (code_page>0) {
		if (has_bom) *has_bom = true;
		return code_page;
	}

	//BOM����
	if (check_UTF8(bp, src_size)>0) code_page = CP_UTF8;	//UTF-8 BOM����
	if (code_page>0) return code_page;

	if (x00_cnt==0) {
		if (esc_cnt>0) {
			//ISO-2022-JP?
			try {
				int jis_cnt = 0;
				int flag = 0;
				for (int i=0; i<src_size-1; i++) {
					BYTE b = bp[i];
					switch (flag) {
					case 0:
						if (b==0x1b) flag = 1;
						else if ((b>=0x80 && b<=0x8d) || (b>=0x90 && b<=0xa0) || b==0xfe) Abort();
						break;
					case 1:	//ESC
						flag = (b=='$')? 2 : (b=='(')? 5 : 0;
						break;
					case 2:	//ESC$
						flag = (b=='B' || b=='@')? 3 : 0;
						break;
					case 3:	//ESC$B/@
						if (b==0x1b) flag = 1; else jis_cnt++;
						break;
					case 5:	//ESC(
						flag = (b=='B' || b=='J')? 6 : 0;
						break;
					case 6:	//ESC(B/J
						if (b==0x1b) flag = 1; else jis_cnt++;
						break;
					}
				}
				if (jis_cnt > 0) code_page = 50220;
			}
			catch (...) {
				;
			}
		}
		else {
			//ShiftJIS?
			int sj_cnt	= 0;
			for (int i=0; i<src_size-1; i++) {
				BYTE b0 = bp[i];
				BYTE b1 = bp[i + 1];
				if (((0x81<=b0 && b0<=0x9f) || (0xe0<=b0 && b0<=0xfc))
					&& ((0x40<=b1 && b1<=0x7e) || (0x80<=b1 && b1<=0xfc)))
				{
					sj_cnt += 2;  i++;
				}
			}
			//EUC-JP?
			int euc_cnt = 0;
			for (int i=0; i<src_size-1; i++) {
				BYTE b0 = bp[i];
				BYTE b1 = bp[i + 1];
				if (((0xa1<=b0 && b0<=0xfe) && (0xa1<=b1 && b1<=0xfe))
					|| (b0==0x8e && (0xa1<=b1 && b1<=0xdf)))
						{ euc_cnt += 2;  i++; }
				else if (i < src_size-2) {
					BYTE b2 = bp[i + 2];
					if (b0==0x8f && (0xa1<=b1 && b1<=0xfe) && (0xa1<=b2 && b2<=0xfe))
						{ euc_cnt += 3;  i += 2; }
				}
			}
			//UTF-7?
			int utf7_cnt = 0;
			if (b7_cnt==0) {
				int  b64cnt = 0;
				bool is_b64 = false;
				for (int i=0; i<src_size; i++) {
					BYTE b0 = bp[i];
					if (!is_b64) {
						if (b0=='+') {
							b64cnt = 0; is_b64 = true;
						}
					}
					else {
						if (b0=='-') {
							utf7_cnt += b64cnt;
							is_b64 = false;
						}
						else if (!isalnum(b0) && b0!='+' && b0!='/') {
							utf7_cnt = 0; break;
						}
						else b64cnt++;
					}
				}
			}
			if ((euc_cnt + sj_cnt + asc_cnt) > src_size/2) {
				if		(euc_cnt>sj_cnt)	code_page = 20932;
				else if (utf7_cnt>sj_cnt)	code_page = 65000;
				else if (sj_cnt>0)			code_page = 932;
				else if (ltn_cnt>0)			code_page = 1252;
				else if (asc_cnt==src_size)	code_page = 20127;
			}
		}
		if (code_page>0) return code_page;
	}

	//UTF-16��?
	int be_asc = 1, le_asc = 1;
	int be_hk  = 1, le_hk  = 1;		//�Ђ炪�ȁA�J�^�J�i
	src_size -= (src_size%2);
	for (int i=0; i<src_size-1; i+=2) {
		BYTE b0 = bp[i];
		BYTE b1 = bp[i + 1];
		if		(b0==0x00 && 0x1f<b1 && b1<0x7f) be_asc++;
		else if (b1==0x00 && 0x1f<b0 && b0<0x7f) le_asc++;
		else if (b0==0x30 && 0x00<b1 && b1<0xf7) be_hk++;
		else if (b1==0x30 && 0x00<b0 && b0<0xf7) le_hk++;
	}

	int be_point = 0, le_point = 0;
	if		(be_asc/le_asc>10) be_point++;
	else if (le_asc/be_asc>10) le_point++;

	if		(be_hk/le_hk>10)   be_point++;
	else if (le_hk/be_hk>10)   le_point++;

	if		(be_point>le_point) code_page = 1201;	//BE
	else if	(le_point>be_point) code_page = 1200;	//LE

	return code_page;
}

//---------------------------------------------------------------------------
//�������X�g���[���̃R�[�h�y�[�W�𔻒肵�A������Ƃ��Ď擾
//---------------------------------------------------------------------------
UnicodeString get_MemoryStrins(TMemoryStream *ms)
{
	//�R�[�h�y�[�W�擾
	ms->Seek(0, soFromBeginning);
	int code_page = get_MemoryCodePage(ms);
	if (code_page==0) code_page = 932;

	//�o�b�t�@�ɓǂݍ���
	TBytes Bytes;
	Bytes.Length = ms->Size;
	ms->Seek(0, soFromBeginning);
	ms->Read(Bytes, ms->Size);

	//�G���R�[�h
	std::unique_ptr<TStringList> lst(new TStringList());
	std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
	return enc->GetString(Bytes, 0, Bytes.Length);
}

//---------------------------------------------------------------------------
//�X�g���[���̉��s�R�[�h���擾
//---------------------------------------------------------------------------
UnicodeString get_StreamLineBreak(
	TStream *ms,
	int  code_page,
	bool strict)	//���S�`�F�b�N	(default = false);
{
	if (code_page==0) return EmptyStr;

	bool is_BE = (code_page==1201);
	int  ch_sz = (code_page==1200 || code_page==1201)? 2 : 1;

	int r_cnt  = 0;	//CR
	int n_cnt  = 0;	//LF
	int rn_cnt = 0;	//CR/LF
	bool first = true;
	BYTE cbuf[2]; cbuf[1] = 0;

	ms->Seek(0, soFromBeginning);
	while (ms->Position < ms->Size) {
		if (ms->Read(cbuf, ch_sz)!=ch_sz) break;
		unsigned short c = is_BE? ((cbuf[0] << 8) | cbuf[1]) : ((cbuf[1] << 8) | cbuf[0]);
		bool is_CR = (c==0x000d);
		bool is_LF = (c==0x000a);
		if (is_CR || is_LF) {
			if (first) {
				if		(is_CR) {r_cnt++; first = false;}
				else if (is_LF) n_cnt++;
			}
			else {
				if		(is_CR) r_cnt++;
				else if (is_LF) {rn_cnt++; r_cnt--;}
				first =  true;
			}
			if (strict) {
				//���S�`�F�b�N(�g��������)�̏ꍇ�͒��f���󂯕t����
				Application->ProcessMessages();
	 			if (FindAborted) return EmptyStr;
			}
			else {
				if (first && (r_cnt + n_cnt + rn_cnt)>CRLF_DETECT_LINE) break;
			}
		}
	}

	if (!first) r_cnt--;
	if (rn_cnt>0 && r_cnt==0 && n_cnt==0)	return "CR/LF";
	if (rn_cnt==0 && r_cnt>0 && n_cnt==0)	return "CR";
	if (rn_cnt==0 && r_cnt==0 && n_cnt>0)	return "LF";
	if ((rn_cnt + r_cnt + n_cnt)>0)			return "����";
	if (strict)								return "����";
											return EmptyStr;
}

//---------------------------------------------------------------------------
//�t�@�C���̃R�[�h�y�[�W���擾
//  �߂�l: �R�[�h�y�[�W (0: �s��)
//---------------------------------------------------------------------------
int get_FileCodePage(
	UnicodeString fnam, 
	UnicodeString *line_brk,	//[o] ���s		(default = NULL)
	bool *has_bom)				//[o] BOM�L��	(default = NULL)
{
	int code_page = 0;

	//XML
	UnicodeString enc;
	UnicodeString fext = get_extension(fnam);
	if (test_FileExt(fext, FEXT_XML _T(".xhtml"))) {
		UnicodeString lbuf = get_top_line(fnam);
		if (contains_i(lbuf, _T("<?xml "))) {
			int p = pos_i(_T("encoding="), lbuf);
			if (p>0) {
				lbuf = lbuf.SubString(p, lbuf.Length() - p + 1);
				lbuf = get_tkn(get_tkn_m(lbuf, '=', ' '), '?');
				enc  = Trim(exclude_quot(lbuf));
			}
		}
	}
	//HTML
	else if (test_HtmlExt(fext)) {
		UnicodeString lbuf = get_html_header(fnam);
		int p = pos_i(_T("charset="), lbuf);
		if (p>0) {
			lbuf = lbuf.SubString(p, lbuf.Length() - p + 1);
			lbuf = get_tkn(get_tkn_m(lbuf, '=', '>'), '/');
			enc  = Trim(REPLACE_TS(lbuf, "\"", ""));
		}
	}

	if (!enc.IsEmpty()) code_page = get_CodePageOfName(enc);

	//���̑�
	if (code_page==0 || line_brk || has_bom) {
		try {
			std::unique_ptr<TFileStream>   fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
			std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
			ms->CopyFrom(fs.get(), std::min<__int64>(fs->Size, TXT_DETECT_SIZE));
			int cpag = get_MemoryCodePage(ms.get(), has_bom);
			if (code_page==0) code_page = cpag;
			//���s�R�[�h
			if (line_brk) *line_brk = get_StreamLineBreak(ms.get(), cpag);
		}
		catch (...) {
			;
		}
	}
	return code_page;
}

//---------------------------------------------------------------------------
//�t�@�C���̓e�L�X�g��?
//---------------------------------------------------------------------------
bool is_TextFile(
	UnicodeString fnam,
	int *code_page,				//[o] �R�[�h�y�[�W	(default = NULL)
	UnicodeString *line_brk,	//[o] ���s			(default = NULL)
	bool *has_bom)				//[o] BOM�L��		(default = NULL)
{
	if (!file_exists(fnam)) return false;

	//���炩�Ƀe�L�X�g�ł͂Ȃ��g���q�͂͂˂�
	if (test_NonTxtExt(get_extension(fnam))) return false;

	//�R�[�h�y�[�W�𒲂ׂ�(�擾�ł��Ȃ��������e�L�X�g�Ƃ݂Ȃ�)
	int cp = get_FileCodePage(fnam, line_brk, has_bom);
	if (code_page) *code_page = cp;
	return (cp>0);
}

//---------------------------------------------------------------------------
//�����񃊃X�g�̍s���A�R�����g�A�󔒍s���J�E���g
//  �߂�l: ���s��
//---------------------------------------------------------------------------
int CountListLines(
	TStringList *lst,		//�����񃊃X�g
	UnicodeString fnam,		//�t�@�C����
	int *rem_cnt,			//[o] �R�����g
	int *blk_cnt)			//[o] �󔒍s
{
	*blk_cnt = 0;

	std::unique_ptr<TStringList> ln_lst(new TStringList());	//�P�s�R�����g
	std::unique_ptr<TStringList> bg_lst(new TStringList());	//�u���b�N�J�n
	std::unique_ptr<TStringList> ed_lst(new TStringList());	//�u���b�N�I��
	//�R�����g����
	if (UserHighlight->GetCommentList(fnam, ln_lst.get(), bg_lst.get(), ed_lst.get())) {
		*rem_cnt = 0;
		bool in_mlt = false;
		for (int i=0; i<lst->Count; i++) {
			UnicodeString lbuf = lst->Strings[i];
			UnicodeString tbuf = Trim(lbuf);
			bool is_blnk = tbuf.IsEmpty();

			if (!in_mlt) {
				bool is_rem = false;
				for (int j=0; j<ln_lst->Count && !is_rem; j++)
					is_rem = StartsText(ln_lst->Strings[j], tbuf);

				if (!is_rem) {
					for (int j=0; j<bg_lst->Count; j++) {
						in_mlt = StartsStr(bg_lst->Strings[j], tbuf);
						if (in_mlt) {
							tbuf.Delete(1, bg_lst->Strings[j].Length());
							break;
						}
					}
				}

				if		(is_rem || in_mlt)	(*rem_cnt)++;
				else if (is_blnk)			(*blk_cnt)++;
			}
			else if (in_mlt) (*rem_cnt)++;

			if (in_mlt) {
				for (int j=0; j<ed_lst->Count && in_mlt; j++)
					if (ContainsStr(tbuf, ed_lst->Strings[j])) in_mlt = false;
			}
		}
	}
	//�R�����g����
	else {
		*rem_cnt = -1;
		for (int i=0; i<lst->Count; i++) {
			if (Trim(lst->Strings[i]).IsEmpty()) (*blk_cnt)++;
		}
	}

	return lst->Count;
}

//---------------------------------------------------------------------------
//���s�\�t�@�C����? (�v���O�C���Ȃǂ̃`�F�b�N�p)
//---------------------------------------------------------------------------
bool is_Executable(UnicodeString fnam)
{
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		std::unique_ptr<BYTE[]> mbuf(new BYTE[32]);
		if (fs->Read(mbuf.get(), 32)<32) Abort();
		if (mbuf[0]!='M' || mbuf[1]!='Z') Abort();
		int z_cnt = 0;
		for (int i=2; i<32; i++) if (mbuf[i]==0) z_cnt++;
		if (z_cnt<4) Abort();
		return true;
	}
	catch (...) {
		return false;
	}
}

//---------------------------------------------------------------------------
//�r�f�I/�I�[�f�B�I�E�t�@�C�����H
//---------------------------------------------------------------------------
bool is_AudioVideo(UnicodeString fnam)
{
	if (test_FileExt(ExtractFileExt(fnam), FEXT_DURATION)) return true;
	if (contained_wd_i(_T("�r�f�I|�I�[�f�B�I"), usr_SH->get_PropInf(fnam))) return true;
	return false;
}

//---------------------------------------------------------------------------
//�r���A�[�ŕ\���\���H (�A�C�R���͏���)
//---------------------------------------------------------------------------
bool is_Viewable(file_rec *fp)
{
	if (fp->is_dir || fp->is_dummy)	return false;
	if (fp->f_name.IsEmpty())		return false;
	if (is_ExtractIcon(fp))			return false;
	if (test_FileExt(fp->f_ext, FEXT_META FEXT_WICSTD + WicFextStr)) return true;
	if (SPI->TestFExt(fp->f_ext))	return true;
	return false;
}

//---------------------------------------------------------------------------
//�A�C�R�����o�\��?
//---------------------------------------------------------------------------
bool is_ExtractIcon(file_rec *fp)
{
	return test_FileExt(fp->f_ext, FEXT_ICONVIEW);
}

//---------------------------------------------------------------------------
//�A�[�J�C�u�̊g���q��?
//---------------------------------------------------------------------------
bool test_ArcExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_ARCHIVE) || (usr_ARC->Use7zDll && test_FileExt(fext, FExt7zDll));
}
//---------------------------------------------------------------------------
//�Ή��A�[�J�C�u(SPI���܂�)�̊g���q��?
//---------------------------------------------------------------------------
bool test_ArcAxExt(UnicodeString fext)
{
	return (test_ArcExt(fext) || SPI->TestFExt(fext, true));
}
//---------------------------------------------------------------------------
//Exif���擾�\�̊g���q��?
//---------------------------------------------------------------------------
bool test_ExifExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_EXIF);
}
//---------------------------------------------------------------------------
//HTML�����̊g���q��?
//---------------------------------------------------------------------------
bool test_HtmlExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_HTML);
}
//---------------------------------------------------------------------------
//�t�H���g�̊g���q��?
//---------------------------------------------------------------------------
bool test_FontExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_FONT);
}
//---------------------------------------------------------------------------
//���C�u�����̊g���q��?
//---------------------------------------------------------------------------
bool test_LibExt(UnicodeString fext)
{
	return USAME_TI(fext, ".library-ms");
}
//---------------------------------------------------------------------------
//.exe ��?
//---------------------------------------------------------------------------
bool test_ExeExt(UnicodeString fext)
{
	return USAME_TI(fext, ".exe");
}
//---------------------------------------------------------------------------
//�V���[�g�J�b�g�̊g���q��?
//---------------------------------------------------------------------------
bool test_LnkExt(UnicodeString fext)
{
	return USAME_TI(fext, ".lnk");
}
//---------------------------------------------------------------------------
//�A�C�R���̊g���q��?
//---------------------------------------------------------------------------
bool test_IcoExt(UnicodeString fext)
{
	return USAME_TI(fext, ".ico");
}
//---------------------------------------------------------------------------
//Jpeg�̊g���q��?
//---------------------------------------------------------------------------
bool test_JpgExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_JPEG);
}
//---------------------------------------------------------------------------
//GIF�̊g���q��?
//---------------------------------------------------------------------------
bool test_GifExt(UnicodeString fext)
{
	return USAME_TI(fext, ".gif");
}
//---------------------------------------------------------------------------
//PNG�̊g���q��?
//---------------------------------------------------------------------------
bool test_PngExt(UnicodeString fext)
{
	return USAME_TI(fext, ".png");
}
//---------------------------------------------------------------------------
//���^�t�@�C���̊g���q��?
//---------------------------------------------------------------------------
bool test_MetaExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_META);
}
//---------------------------------------------------------------------------
//MP3�̊g���q��?
//---------------------------------------------------------------------------
bool test_Mp3Ext(UnicodeString fext)
{
	return USAME_TI(fext, ".mp3");
}
//---------------------------------------------------------------------------
//MSI�Ή��T�E���h�̊g���q��?
//---------------------------------------------------------------------------
bool test_MciSndExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_MCISND);
}

//---------------------------------------------------------------------------
//�R�}���h�t�@�C���̊g���q��?
//---------------------------------------------------------------------------
bool test_NbtExt(UnicodeString fext)
{
	return USAME_TI(fext, ".nbt");
}
//---------------------------------------------------------------------------
//���[�N���X�g�̊g���q��?
//---------------------------------------------------------------------------
bool test_NwlExt(UnicodeString fext)
{
	return USAME_TI(fext, ".nwl");
}

//---------------------------------------------------------------------------
//�e�L�X�g�̊g���q��?
//---------------------------------------------------------------------------
bool test_TxtExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_TEXT);
}
//---------------------------------------------------------------------------
//�e�L�X�g�t�@�C����? (�g���q or �t�@�C�����Ŕ���)
//---------------------------------------------------------------------------
bool test_TxtFile(UnicodeString fnam)
{
	UnicodeString nnam = ExtractFileName(fnam);
	UnicodeString fext = get_extension(fnam);

	if (test_TxtExt(fext)) return true;

	if (fext.IsEmpty()) {
		if (USAME_TI(nnam, "tags") || USTARTS_TI(".nyanfi", nnam)) return true;
	}

	return false;
}
//---------------------------------------------------------------------------
//��e�L�X�g�̊g���q��?
//---------------------------------------------------------------------------
bool test_NonTxtExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_NONTEXT + WicFextStr);
}

//---------------------------------------------------------------------------
//�A�v�����擾�\�Ȋg���q��?
//---------------------------------------------------------------------------
bool test_AppInfExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_APPINFO);
}

//---------------------------------------------------------------------------
//�t�@�C������摜�T�C�Y���擾
//---------------------------------------------------------------------------
bool get_img_size(UnicodeString fnam, unsigned int *wd, unsigned int *hi)
{
	bool size_ok = WIC_get_img_size(fnam, wd, hi) || SPI->GetImgSize(fnam, wd, hi);

	if (!size_ok && test_ExifExt(get_extension(fnam))) {
		std::unique_ptr<TStringList> i_lst(new TStringList());	//�t�@�C�����p
		size_ok = get_ExifInf(fnam, i_lst.get(), NULL, wd, hi);
	}

	if (!size_ok && test_MetaExt(get_extension(fnam))) {
		try {
			std::unique_ptr<TMetafile> mf(new TMetafile());
			mf->LoadFromFile(fnam);
			*wd = mf->Width;
			*hi = mf->Height;
			size_ok = true;
		}
		catch (...) {
			;
		}
	}
	return size_ok;
}

//---------------------------------------------------------------------------
//�摜�t�@�C����ǂݍ���
//�߂�l ���s = 0/  ���� = LOADED_BY_STD�ALOADED_BY_WIC�ALOADED_BY_SPI
//---------------------------------------------------------------------------
int load_ImageFile(
	UnicodeString fnam, Graphics::TBitmap *o_bmp,
	int wic_type,		//				(default = WICIMG_PREVIEW)
	TColor trans_bg)	//����G�̔w�i�F	(default = clNone)
{
	int res = 0;
	try {
		o_bmp->Canvas->Lock();
		try {
			if (fnam.IsEmpty()) Abort();
			UnicodeString fext = get_extension(fnam);
			//PNG�͓Ǝ��ɏ���(�A���t�@�`���l���Ή�)
			if (test_PngExt(fext)) {
				std::unique_ptr<TPngImage>pngimg(new TPngImage());
				pngimg->LoadFromFile(fnam);
				pngimg->Transparent = true;
				o_bmp->SetSize(pngimg->Width, pngimg->Height);
				if (trans_bg!=Graphics::clNone) {
					o_bmp->Canvas->Brush->Color = trans_bg;
					o_bmp->Canvas->FillRect(Rect(0, 0, pngimg->Width, pngimg->Height));
				}
				o_bmp->Canvas->Draw(0, 0, pngimg.get());
				res = LOADED_BY_STD;
			}
			else if (test_GifExt(fext)) {
				std::unique_ptr<TGIFImage> gifimg(new TGIFImage());
				gifimg->LoadFromFile(fnam);
				gifimg->Transparent = true;
				o_bmp->SetSize(gifimg->Width, gifimg->Height);
				if (trans_bg!=Graphics::clNone) {
					o_bmp->Canvas->Brush->Color = trans_bg;
					o_bmp->Canvas->FillRect(Rect(0, 0, gifimg->Width, gifimg->Height));
				}
				o_bmp->Canvas->Draw(0, 0, gifimg.get());
				res = LOADED_BY_STD;
			}
			//SPI�D��Ȃ�SPI��
			else if (UseSpiFirst && SPI->LoadImage(fnam, o_bmp))
				res = LOADED_BY_SPI;
			//WIC��
			else if (WIC_load_image(fnam, o_bmp, wic_type))
				res = test_FileExt(fext, FEXT_WICSTD)? LOADED_BY_STD : LOADED_BY_WIC;
			//SPI�D��łȂ��Ȃ�SPI��
			else if (!UseSpiFirst && SPI->LoadImage(fnam, o_bmp))
				res = LOADED_BY_SPI;
		}
		catch (...) {
			res = 0;
		}
	}
	__finally {
		o_bmp->Canvas->Unlock();
	}
	return res;
}

//---------------------------------------------------------------------------
//ViewFileList �ŗL���ȍ��ڐ����擾
//---------------------------------------------------------------------------
int get_ViewCount()
{
	int cnt = 0;
	for (int i=0; i<ViewFileList->Count; i++)
		if (!((file_rec*)ViewFileList->Objects[i])->failed) cnt++;
	return cnt;
}

//---------------------------------------------------------------------------
//�t�@�C���̍폜
//---------------------------------------------------------------------------
bool delete_File(
	UnicodeString fnam,
	bool use_trash)		//���ݔ����g�p	(default = false)
{
	bool res;
	//���ݔ��ɓ����
	if (use_trash) {
		SHFILEOPSTRUCT lpFileOp;
		std::unique_ptr<_TCHAR[]> lpszFil(new _TCHAR[fnam.Length() + 4]);
		_TCHAR *p = lpszFil.get();
		_tcscpy(p, fnam.c_str());
		p += (_tcslen(p) + 1);  *p ='\0';

		lpFileOp.hwnd				   = MainHandle;
		lpFileOp.wFunc				   = FO_DELETE;
		lpFileOp.pFrom				   = lpszFil.get();
		lpFileOp.pTo				   = NULL;
		lpFileOp.fFlags 			   = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;
		lpFileOp.fAnyOperationsAborted = true;
		lpFileOp.hNameMappings		   = NULL;
		lpFileOp.lpszProgressTitle	   = NULL;
		res = (::SHFileOperation(&lpFileOp) == 0);
	}
	//�폜
	else
		res = ::DeleteFile(cv_ex_filename(fnam).c_str());
	return res;
}
//---------------------------------------------------------------------------
void delete_FileIf(UnicodeString fnam)
{
	if (file_exists(fnam)) delete_File(fnam);
}

//---------------------------------------------------------------------------
//�R�}���h�������擾
//---------------------------------------------------------------------------
UnicodeString get_CmdStr(UnicodeString s)
{
	return starts_AT(s)? s : get_tkn(s, '_');
}
//---------------------------------------------------------------------------
//�p�����[�^�������擾
//---------------------------------------------------------------------------
UnicodeString get_PrmStr(UnicodeString s)
{
	return get_tkn_r(s, '_');
}

//---------------------------------------------------------------------------
//�L�[�Ɋ��蓖�Ă��Ă���R�}���h���擾
//---------------------------------------------------------------------------
UnicodeString Key_to_CmdF(UnicodeString key)
{
	return KeyFuncList->Values["F:" + key];
}
//---------------------------------------------------------------------------
UnicodeString Key_to_CmdV(UnicodeString key)
{
	return KeyFuncList->Values["V:" + key];
}

//---------------------------------------------------------------------------
//�R�}���h�ɑ΂��������������擾
//---------------------------------------------------------------------------
UnicodeString get_CmdDesc(
	UnicodeString cmd,
	bool only_inf,			//�����̂ݎ擾 (default = false)
	TStrings *menu_list,
	TStrings *tool_list)
{
	if (!menu_list) menu_list = ExtMenuList;
	if (!tool_list) tool_list = ExtToolList;

	UnicodeString prm;
	if (!starts_Dollar(cmd)) {
		prm = get_PrmStr(cmd);
		cmd = get_CmdStr(cmd);
	}

	//����
	UnicodeString dsc = split_dsc(prm);
	if (dsc.IsEmpty()) {
		if (contained_wd_i(_T("ExeExtMenu|ExeExtTool"), cmd)) {
			//�Y�����郁�j���[���ڂ��擾
			TStrings *lst= USAME_TI(cmd, "ExeExtMenu")? menu_list : tool_list;
			for (int i=0; i<lst->Count && dsc.IsEmpty(); i++) {
				UnicodeString ibuf = get_csv_item(lst->Strings[i], 0);
				if (!contains_s(ibuf, _T('&'))) continue;
				UnicodeString ak = get_tkn_r(ibuf, '&').SubString(1, 1);
				if (!SameText(ak, prm)) continue;
				dsc = REPLACE_TS(ibuf, "&", "");
				break;
			}
		}
		//�G�C���A�X
		else if (starts_Dollar(cmd)) {
			UnicodeString alias = exclude_top(cmd);
			//�ǉ����j���[
			for (int i=0; i<menu_list->Count && dsc.IsEmpty(); i++) {
				TStringDynArray itm_buf = get_csv_array(menu_list->Strings[i], EXTMENU_CSVITMCNT, true);
				if (!SameText(alias, itm_buf[3])) continue;
				dsc = REPLACE_TS(itm_buf[0], "&", "");
				break;
			}
			//�O���c�[��
			for (int i=0; i<tool_list->Count && dsc.IsEmpty(); i++) {
				TStringDynArray itm_buf = get_csv_array(tool_list->Strings[i], EXTTOOL_CSVITMCNT, true);
				if (!SameText(alias, itm_buf[4])) continue;
				dsc = REPLACE_TS(itm_buf[0], "&", "");
				break;
			}
		}
	}

	if (dsc.IsEmpty()) dsc = CommandList->Values[cmd];

	if (only_inf)
		return dsc;
	else {
		if (!prm.IsEmpty()) cmd.cat_sprintf(_T("_%s"), prm.c_str());
		return UnicodeString().sprintf((cmd.Length()<20)? _T("%-20s%s") : _T("%s  %s"), cmd.c_str(), dsc.c_str());
	}
}

//---------------------------------------------------------------------------
//�R�}���h�p�����[�^���������������폜
//---------------------------------------------------------------------------
UnicodeString del_CmdDesc(UnicodeString cmd)
{
	UnicodeString prm = get_PrmStr(cmd);
	cmd = get_CmdStr(cmd);
	split_dsc(prm);
	if (!prm.IsEmpty()) cmd.cat_sprintf(_T("_%s"), prm.c_str());
	return cmd;
}

//---------------------------------------------------------------------------
TColor read_ColorList(UnicodeString key, TColor def, TStringList *lst)
{
	TColor col = def;
	if (!lst) lst = ColorList;
	UnicodeString vstr = lst->Values[key];
	if (!vstr.IsEmpty())
		col = (TColor)vstr.ToIntDef(def);
	else
		lst->Add(key.cat_sprintf(_T("=%d"), (int)def));
	return col;
}
//---------------------------------------------------------------------------
TColor read_ColorList(const _TCHAR *key, TColor def, TStringList *lst)
{
	return read_ColorList(UnicodeString(key), def, lst);
}

//---------------------------------------------------------------------------
//ColorList ����X�̔z�F���擾
//---------------------------------------------------------------------------
void set_col_from_ColorList()
{
	struct {
		TColor *vp;			//�ϐ��ւ̃|�C���^
		const _TCHAR *key;	//�L�[��=�f�t�H���g
		TColor def;
	} col_def_list[] = {
		{&col_bgList,	_T("bgList"),		clBlack},
		{&col_bgList2,	_T("bgList2"),		Graphics::clNone},
		{&col_fgList,	_T("fgList"),		clWhite},
		{&col_Splitter,	_T("Splitter"),		clBtnFace},
		{&col_bgArc,	_T("bgArc"),		clNavy},
		{&col_bgFind,	_T("bgFind"),		clNavy},
		{&col_bgWork,	_T("bgWork"),		clNavy},
		{&col_bgFTP,	_T("bgFTP"),		clNavy},
		{&col_selItem,	_T("selItem"),		clBlue},
		{&col_oppItem,	_T("oppItem"),		clNavy},
		{&col_fgSelItem,_T("fgSelItem"),	Graphics::clNone},
		{&col_bgMark,	_T("bgMark"),		clWebIndigo},
		{&col_matchItem,_T("matchItem"),	clNavy},
		{&col_Cursor,	_T("Cursor"),		clFuchsia},
		{&col_bgScrBar,	_T("bgScrBar"),		clAppWorkSpace},
		{&col_bgScrKnob,_T("bgScrKnob"),	clBtnFace},
		{&col_frScrKnob,_T("frScrKnob"),	clBtnShadow},
		{&col_Folder,	_T("Folder"),		clYellow},
		{&col_SymLink,	_T("SymLink"),		clYellow},
		{&col_Protect,	_T("Protect"),		Graphics::clNone},
		{&col_ReadOnly,	_T("ReadOnly"),		clDkGray},
		{&col_Hidden,	_T("Hidden"),		clTeal},
		{&col_System,	_T("System"),		clPurple},
		{&col_fgTagNam,	_T("fgTagNam"),		clLime},
		{&col_InvItem,	_T("InvItem"),		clGray},
		{&col_bgTabBar,	_T("bgTabBar"),		clBtnFace},
		{&col_bgActTab,	_T("bgActTab"),		clWindow},
		{&col_bgInAcTab,_T("bgInAcTab"),	clBtnFace},
		{&col_frmTab,	_T("frmTab"),		clBlack},
		{&col_fgTab,	_T("fgTab"),		clBtnText},
		{&col_bgListHdr,_T("bgListHdr"),	clBtnFace},
		{&col_fgListHdr,_T("fgListHdr"),	clBtnText},
		{&col_bgDirInf,	_T("bgDirInf"),		clBtnFace},
		{&col_fgDirInf,	_T("fgDirInf"),		clBtnText},
		{&col_bgDirRel,	_T("bgDirRel"),		clBtnFace},
		{&col_fgDirRel,	_T("fgDirRel"),		clBtnText},
		{&col_bgDrvInf,	_T("bgDrvInf"),		clBtnFace},
		{&col_fgDrvInf,	_T("fgDrvInf"),		clBtnText},
		{&col_bgInf,	_T("bgInf"),		clBlack},
		{&col_fgInf,	_T("fgInf"),		clWhite},
		{&col_fgInfNam,	_T("fgInfNam"),		clWhite},
		{&col_bgTxtPrv,	_T("bgTxtPrv"),		clBlack},
		{&col_fgTxtPrv,	_T("fgTxtPrv"),		clWhite},
		{&col_bgLog,	_T("bgLog"),		clBlack},
		{&col_fgLog,	_T("fgLog"),		clWhite},
		{&col_bgTask,	_T("bgTask"),		clBlack},
		{&col_fgPrgBar,	_T("fgPrgBar"),		clLime},
		{&col_bgPrgBar,	_T("bgPrgBar"),		clGray},
		{&col_Error,	_T("Error"),		clRed},
		{&col_bgView,	_T("bgView"),		clNavy},
		{&col_fgView,	_T("fgView"),		clWhite},
		{&col_Margin,	_T("Margin"),		clNavy},
		{&col_bgLineNo,	_T("bgLineNo"),		clNavy},
		{&col_LineNo,	_T("LineNo"),		clTeal},
		{&col_Mark,		_T("Mark"),			clFuchsia},
		{&col_bgRuler,	_T("bgRuler"),		clWindow},
		{&col_fgRuler,	_T("fgRuler"),		clWindowText},
		{&col_bdrLine,	_T("bdrLine"),		clDkGray},
		{&col_bdrFold,	_T("bdrFold"),		clBlue},
		{&col_bdrFixed,	_T("bdrFixed"),		Graphics::clNone},
		{&col_Comment,	_T("Comment"),		clLime},
		{&col_Strings,	_T("Strings"),		clAqua},
		{&col_Reserved,	_T("Reserved"),		clSkyBlue},
		{&col_Symbol,	_T("Symbol"),		clLtGray},
		{&col_Numeric,	_T("Numeric"),		clFuchsia},
		{&col_fgEmpBin1,_T("fgEmpBin1"),	clYellow},
		{&col_fgEmpBin2,_T("fgEmpBin2"),	clBlue},
		{&col_fgEmpBin3,_T("fgEmpBin3"),	clRed},
		{&col_Headline,	_T("Headline"),		clYellow},
		{&col_URL,		_T("URL"),			clWebLimeGreen},
		{&col_LocalLink,_T("LocalLink"),	clWebDeepPink},
		{&col_bgImage,	_T("bgImage"),		clBlack},
		{&col_bgDblPg,	_T("bgDblPg"),		clWindow},
		{&col_bgWMF,	_T("bgWMF"),		clWindow},
		{&col_fgEmp,	_T("fgEmp"),		clYellow},
		{&col_bgEmp,	_T("bgEmp"),		clNavy},
		{&col_Ruby,		_T("Ruby"),			clGray},
		{&col_TAB,		_T("TAB"),			clBlue},
		{&col_CR,		_T("CR"),			clPurple},
		{&col_HR,		_T("HR"),			clDkGray},
		{&col_Ctrl,		_T("Ctrl"),			clFuchsia},
		{&col_bdrThumb,	_T("bdrThumb"),		clBtnFace},
		{&col_ThumbExif,_T("ThumbExif"),	clWhite},
		{&col_bgHint,	_T("bgHint"),		Application->HintColor},
		{&col_fgHint,	_T("fgHint"),		clBlack},
		{&col_bgWarn,	_T("bgWarn"),		clRed},
		{&col_OptFind,	_T("OptFind"),		clRed},
		{&col_Invalid,	_T("Invalid"),		clLtGray},
		{&col_Illegal,	_T("Illegal"),		clWebPink},
		{&col_Tim1H,	_T("Tim1H"),		Graphics::clNone},
		{&col_Tim3H,	_T("Tim3H"),		Graphics::clNone},
		{&col_Tim1D,	_T("Tim1D"),		Graphics::clNone},
		{&col_Tim3D,	_T("Tim3D"),		Graphics::clNone},
		{&col_Tim7D,	_T("Tim7D"),		Graphics::clNone},
		{&col_Tim1M,	_T("Tim1M"),		Graphics::clNone},
		{&col_Tim3M,	_T("Tim3M"),		Graphics::clNone},
		{&col_Tim6M,	_T("Tim6M"),		Graphics::clNone},
		{&col_Tim1Y,	_T("Tim1Y"),		Graphics::clNone},
		{&col_Tim3Y,	_T("Tim3Y"),		Graphics::clNone},
		{&col_TimOld,	_T("TimOld"),		Graphics::clNone},
		{&col_Size4G,	_T("Size4G"),		Graphics::clNone},
		{&col_Size1G,	_T("Size1G"),		Graphics::clNone},
		{&col_Size1M,	_T("Size1M"),		Graphics::clNone},
		{&col_Size1K,	_T("Size1K"),		Graphics::clNone},
		{&col_SizeLT,	_T("SizeLT"),		Graphics::clNone},
		{&col_Size0,	_T("Size0"),		Graphics::clNone},
		{&col_ImgGrid,	_T("ImgGrid"),		clGray},
		{&col_bgTlBar1,	_T("bgTlBar1"),		clWhite},
		{&col_bgTlBar2,	_T("bgTlBar2"),		clWebGainsboro},
		{&col_fgTlBar,	_T("fgTlBar"),		clBlack},
		{&col_htTlBar,	_T("htTlBar"),		TColor(0x00efd3c6)},
		{&col_bgSttBar,	_T("bgSttBar"),		scl_BtnFace},
		{&col_fgSttBar,	_T("fgSttBar"),		scl_BtnText},
		{&col_bgInfHdr,	_T("bgInfHdr"),		scl_BtnFace},
		{&col_fgInfHdr,	_T("fgInfHdr"),		scl_BtnText},
		{&col_ModalScr,	_T("ModalScr"),		clBlack}
	};

	int cnt = sizeof(col_def_list)/sizeof(col_def_list[0]);
	for (int i=0; i<cnt; i++)
		*col_def_list[i].vp = read_ColorList(col_def_list[i].key, col_def_list[i].def);
}

//---------------------------------------------------------------------------
//�ҏW������ǉ�
//---------------------------------------------------------------------------
void add_TextEditHistory(UnicodeString fnam)
{
	fnam = exclude_quot(fnam);
	if (StartsText(TempPathA, fnam)) return;

	if (!NoEditHistPath.IsEmpty()) {
		TStringDynArray path_lst = split_strings_semicolon(NoEditHistPath);
		for (int i=0; i<path_lst.Length; i++) {
			if (path_lst[i].IsEmpty()) continue;
			if (ContainsText(fnam, path_lst[i])) return;
		}
	}

	int i=0;
	while (i < TextEditHistory->Count)
		if (SameText(TextEditHistory->Strings[i], fnam)) TextEditHistory->Delete(i); else i++;
	TextEditHistory->Insert(0, fnam);
}

//---------------------------------------------------------------------------
//�Ăяo���z�b�g�L�[�̓o�^
//---------------------------------------------------------------------------
bool register_HotKey(int id, UnicodeString kstr)
{
	GlobalErrMsg = EmptyStr;

	try {
		int idx = HotKeyList->IndexOfName(IntToStr(id));
		if (idx!=-1) {
			if (!::UnregisterHotKey(MainHandle, id)) Abort();
			HotKeyList->Delete(idx);
		}

		if (!kstr.IsEmpty()) {
			bool windows_key   = remove_text(kstr, _T("Win+"));
			TShortCut shortcut = TextToShortCut(kstr);
			Word vkcode;
			TShiftState ss;
			ShortCutToKey(shortcut, vkcode, ss);

			UINT modflag = 0;
			if (ss.Contains(ssShift)) modflag |= MOD_SHIFT;
			if (ss.Contains(ssAlt))   modflag |= MOD_ALT;
			if (ss.Contains(ssCtrl))  modflag |= MOD_CONTROL;
			if (windows_key)		  modflag |= MOD_WIN;
			if (modflag!=0 && vkcode!=0) {
				if (!::RegisterHotKey(MainHandle, id, modflag, vkcode)) Abort();
				HotKeyList->Add(UnicodeString().sprintf(_T("%u=%s"), id, kstr.c_str()));
			}
		}

		return true;
	}
	catch (EAbort &e) {
		GlobalErrMsg = SysErrorMessage(GetLastError());
		switch (id) {
		case ID_CALL_HOTKEY: GlobalErrMsg.UCAT_T("\r\n�uNyanFi �Ăяo���v�̐ݒ���m�F���Ă��������B");	break;
		case ID_APP_HOTKEY:  GlobalErrMsg.UCAT_T("\r\n�uAppList �R�}���h�v�̐ݒ���m�F���Ă��������B");	break;
		}
		return false;
	}
}

//---------------------------------------------------------------------------
//�t�H�[���̃^�C�g����ݒ�
//---------------------------------------------------------------------------
void set_FormTitle(TForm *frm, const _TCHAR *s)
{
	frm->Caption = s;
}

//---------------------------------------------------------------------------
//�m�F���b�Z�[�W
//---------------------------------------------------------------------------
bool msgbox_Sure(UnicodeString msg, bool ask)
{
	if (!ask) return true;
	unsigned int flag = (SureCancel? MB_YESNOCANCEL : MB_YESNO) | MB_ICONQUESTION;
	if (SureDefNo) flag |= MB_DEFBUTTON2;
	return (Application->MessageBox(msg.c_str(), _T("�m�F"), flag)==IDYES);
}
//---------------------------------------------------------------------------
bool msgbox_Sure(const _TCHAR *msg, bool ask)
{
	return msgbox_Sure(UnicodeString(msg), ask);
}
//---------------------------------------------------------------------------
bool msgbox_Sure(int id, bool ask)
{
	if (!ask) return true;
	return msgbox_Sure(LoadUsrMsg(id), ask);
}

//---------------------------------------------------------------------------
//�x����
//---------------------------------------------------------------------------
void beep_Warn(UnicodeString fnam)
{
	if (Initialized && !UnInitializing) {
		if (fnam.IsEmpty()) fnam = SoundWarning;
		bool fw = (FlashCntWarning>0 && FlashTimeWarning>0);
		if (!play_sound(fnam) && !fw) ::MessageBeep(MB_ICONEXCLAMATION);
		if (fw) flash_win(FlashCntWarning, FlashTimeWarning);
	}
}

//---------------------------------------------------------------------------
//�^�C�g���o�[�̓_��
//---------------------------------------------------------------------------
void flash_win(
	int cnt,	//�_�ŉ� default = 3
	int tm)		//�_�ŊԊu default = 250
{
	if (cnt<=0) cnt = 3;
	if (tm<=0)  tm  = 250;

	FLASHWINFO fwi;
	fwi.cbSize    = sizeof(FLASHWINFO);
	fwi.hwnd      = Screen->ActiveForm->Handle;
	fwi.dwFlags   = FLASHW_ALL;
	fwi.uCount    = cnt;
	fwi.dwTimeout = tm;
	::FlashWindowEx(&fwi);
}

//---------------------------------------------------------------------------
//�^�C�g���o�[�̕���{�^����L��/������
//---------------------------------------------------------------------------
void set_CloseButton(bool enabled)
{
	HMENU hMenu = ::GetSystemMenu(MainHandle, false);
	if (hMenu) {
		int mc = ::GetMenuItemCount(hMenu);
		if (mc>0) {
			int mi = ::GetMenuItemID(hMenu, mc - 1);
			::EnableMenuItem(hMenu, mi, enabled? MF_ENABLED : MF_GRAYED);
		}
	}
}

//---------------------------------------------------------------------------
//���ʃ~���[�g
//---------------------------------------------------------------------------
bool mute_Volume(
	UnicodeString prm)	// ON/OFF/GET(�擾�̂�) ��̏ꍇ�̓g�O��
{
	try {
		TComInterface<IMMDeviceEnumerator> devEnumerator;
		TComInterface<IMMDevice> device;
		TComInterface<IAudioEndpointVolume> endp_vol;

		if (FAILED(CoCreateInstance(
			__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&devEnumerator)))) Abort();

		if (FAILED(devEnumerator->GetDefaultAudioEndpoint(
			EDataFlow::eRender, ERole::eMultimedia, &device))) Abort();

		if (FAILED(device->Activate(
			__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (void**)&endp_vol))) Abort();

		BOOL mute = FALSE;
		if (FAILED(endp_vol->GetMute(&mute))) Abort() ;
		IsMuted = mute;

		if (!USAME_TI(prm, "GET")) {
			IsMuted = USAME_TI(prm, "ON")? true : USAME_TI(prm, "OFF")? false : !IsMuted;
			mute = IsMuted;
			if (FAILED(endp_vol->SetMute(mute, NULL))) Abort();
		}
		return true;
	}
	catch (EAbort &e) {
		return false;
	}
}

//---------------------------------------------------------------------------
//�T�E���h�Đ�
// �t�@�C�����͎��s�p�X����̑��Ύw��ł���
// �t�@�C�������݂��Ȃ��Ƃ��͉����炳�Ȃ�
//---------------------------------------------------------------------------
bool play_sound(UnicodeString fnam)
{
	fnam = rel_to_absdir(fnam);
	if (!file_exists(fnam)) return false;

	::sndPlaySound(fnam.c_str(), SND_ASYNC);
	return true;
}

//---------------------------------------------------------------------------
//MCI�ɂ��T�E���h�Đ�(WAV�AWMA�AMP3�AMID)
//�I���C������ MM_MCINOTIFY ������
//---------------------------------------------------------------------------
bool play_sound_ex(
	UnicodeString fnam,		//�t�@�C����(���s�p�X����̑��Ύw��ł���)
	bool limit)				//�b���������� (default = true)
{
	::mciSendString(_T("close TPLYSND"), NULL, 0, NULL);

	fnam = rel_to_absdir(fnam);
	if (!file_exists(fnam)) return false;
	if (limit && PlaySndLimitTime==0) return true;

	bool res = false;

	cursor_HourGlass();
	UnicodeString cmd;
	cmd.sprintf(_T("open \"%s\""), fnam.c_str());
	if (test_Mp3Ext(get_extension(fnam))) cmd.UCAT_T(" type MPEGVideo");
	cmd.UCAT_T(" alias TPLYSND");
	if (::mciSendString(cmd.c_str(), NULL, 0, NULL)==0) {
		if (limit) {
			::mciSendString(_T("set TPLYSND time format milliseconds"), NULL, 0, NULL);
			_TCHAR rbuf[256];
			if (::mciSendString(_T("status TPLYSND length"), rbuf, 255, NULL)==0) {
				int t = std::min(UnicodeString(rbuf).ToIntDef(0), PlaySndLimitTime*1000);
				if (t>0) {
					cmd.sprintf(_T("play TPLYSND from 0 to %u notify"), t);
					res = (::mciSendString(cmd.c_str(), NULL, 0, MainHandle)==0);
				}
			}
		}
		else
			res = (::mciSendString(_T("play TPLYSND notify"), NULL, 0, MainHandle)==0);

		if (!res) ::mciSendString(_T("close TPLYSND"), NULL, 0, NULL);
	}
	cursor_Default();
	return res;
}

//---------------------------------------------------------------------------
//�v���C���X�g�ɒǉ�
//---------------------------------------------------------------------------
bool add_PlayFile(UnicodeString fnam)
{
	if (!test_MciSndExt(get_extension(fnam))) return false;
	PlayList->Add(fnam);
	return true;
}
//---------------------------------------------------------------------------
bool add_PlayList(UnicodeString lnam)
{
	PlayListFile = EmptyStr;

	lnam = rel_to_absdir(lnam);
	int last_cnt = PlayList->Count;

	//�f�B���N�g��
	if (dir_exists(lnam)) {
		std::unique_ptr<TStringList> fbuf(new TStringList());
		get_files(lnam, _T("*.*"), fbuf.get(), true);
		for (int i=0; i<fbuf->Count; i++) add_PlayFile(fbuf->Strings[i]);
	}
	//�t�@�C��
	else if (file_exists(lnam)) {
		//�T�E���h�t�@�C��
		if (add_PlayFile(lnam))
			;
		//���X�g�t�@�C��
		else if (is_TextFile(lnam)) {
			std::unique_ptr<TStringList> fbuf(new TStringList());
			if (load_text_ex(lnam, fbuf.get())!=0) {
				PlayListFile = lnam;
				for (int i=0; i<fbuf->Count; i++) {
					UnicodeString inam = get_pre_tab(fbuf->Strings[i]);
					//�f�B���N�g��
					if (dir_exists(inam)) {
						std::unique_ptr<TStringList> fbuf(new TStringList());
						get_files(inam, _T("*.*"), fbuf.get(), true);
						for (int i=0; i<fbuf->Count; i++) add_PlayFile(fbuf->Strings[i]);
					}
					//�t�@�C��
					else if (file_exists(inam)) add_PlayFile(inam);
				}
			}
		}
	}

	return (PlayList->Count>last_cnt);
}

//---------------------------------------------------------------------------
//�v���C���X�g���Đ�
//---------------------------------------------------------------------------
bool play_PlayList(bool prev, bool inh_shfl)
{
	PlayFile  = EmptyStr;
	ListShuffled = false;
	::mciSendString(_T("close PLYLIST"), NULL, 0, NULL);
	if (PlayList->Count==0) return false;

	if (prev) {
		if (PlayStbIdx>1) PlayStbIdx-=2; else PlayStbIdx = 0;
	}
	else {
		//�V���b�t��
		if (!inh_shfl && PlayShuffle && PlayList->Count>1 && PlayStbIdx==0) {
			for (int i=0; i<PlayList->Count; i++) {
				int j = Random(PlayList->Count);
				UnicodeString s = PlayList->Strings[i];
				PlayList->Strings[i] = PlayList->Strings[j];
				PlayList->Strings[j] = s;
			}
			ListShuffled = true;
		}
	}

	bool res = false;
	int ng_cnt = 0;
	while (PlayStbIdx<PlayList->Count && ng_cnt<PlayList->Count && !res) {
		UnicodeString fnam = PlayList->Strings[PlayStbIdx];
		if (file_exists(fnam)) {
			UnicodeString cmd;
			cmd.sprintf(_T("open \"%s\""), fnam.c_str());
			if (test_Mp3Ext(get_extension(fnam))) cmd.UCAT_T(" type MPEGVideo");
			cmd.UCAT_T(" alias PLYLIST");
			if (::mciSendString(cmd.c_str(), NULL, 0, NULL)==0) {
				res = (::mciSendString(_T("play PLYLIST notify"), NULL, 0, MainHandle)==0);
				if (res) {
					PlayFile = fnam;
					AddLog(make_LogHdr(_T("PLAY"), fnam));
				}
				else 
					::mciSendString(_T("close PLYLIST"), NULL, 0, NULL);
			}
		}
		if (!res) ng_cnt++;
		PlayStbIdx++;
		if (PlayRepeat && PlayStbIdx==PlayList->Count) PlayStbIdx = 0;
	}
	return res;
}

//---------------------------------------------------------------------------
//�I�[�f�B�I/�r�f�I�t�@�C���̒���(ms)���擾
//---------------------------------------------------------------------------
int get_duration(UnicodeString fnam)
{
	int t = -1;
	UnicodeString fext = get_extension(fnam);

	//MCI �Ŏ擾
	if (test_MciSndExt(fext)) {
		UnicodeString cmd;
		cmd.sprintf(_T("open \"%s\" alias GETLENSND"), fnam.c_str());
		if (::mciSendString(cmd.c_str(), NULL, 0, NULL)==0) {
			::mciSendString(_T("set GETLENSND time format milliseconds"), NULL, 0, NULL);
			_TCHAR rbuf[256];
			if (::mciSendString(_T("status GETLENSND length"), rbuf, 255, NULL)==0)
				t = UnicodeString(rbuf).ToIntDef(0);
			::mciSendString(_T("close GETLENSND"), NULL, 0, NULL);
		}
	}
	else if (test_FileExt(fext, _T(".cda"))) {
		int len;
		if (get_CdaInf(fnam, NULL, &len)) t = len;
	}

	//�v���p�e�B����擾
	if (t==-1) t = usr_SH->get_Duration(fnam) * 1000;

	return t;
}

//---------------------------------------------------------------------------
//�^�u���l�����ĕ����񕝂��擾
//---------------------------------------------------------------------------
int get_TabTextWidth(
	UnicodeString s, TCanvas *cv,
	int tab_wd)		//�^�u�X�g�b�v��
{
	int wd = 0;
	TEXTMETRIC tm;
	::GetTextMetrics(cv->Handle , &tm);
	int w_tab = tab_wd * tm.tmAveCharWidth;

	bool is_irreg = IsIrregularFont(cv->Font);

	if (tab_wd>0) {
		while (!s.IsEmpty()) {
			int p = s.Pos('\t');
			if (p>0) {
				if (p>1) wd += get_TextWidth(cv, s.SubString(1, p - 1), is_irreg);
				wd += (w_tab - (wd % w_tab));
				s.Delete(1, p);
			}
			else {
				wd += get_TextWidth(cv, s, is_irreg);
				break;
			}
		}
	}
	else
		wd = get_TextWidth(cv, s, is_irreg);

	return wd;
}

//---------------------------------------------------------------------------
//������̕`��
//---------------------------------------------------------------------------
void out_Text(TCanvas *cv, int x, int y, const _TCHAR *s)
{
	cv->TextOut(x, y, s);
}
//---------------------------------------------------------------------------
void out_Text(TCanvas *cv, int x, int y, const _TCHAR *s, TColor fg)
{
	cv->Font->Color = fg;
	cv->TextOut(x, y, s);
}

//---------------------------------------------------------------------------
//�}�b�`�ꃊ�X�g���擾
//---------------------------------------------------------------------------
int get_MatchWordList(
	UnicodeString lbuf,	//�Ώە�����
	UnicodeString kwd,	//������ (�󔒋�؂�ɂ��AND�����Ή�)
	bool migemo_sw,		//Migemo���g�p
	bool regex_sw,		//���K�\��
	TStringList *lst)
{
	TStringDynArray wd_lst = SplitString(Trim(kwd), " ");
	std::unique_ptr<TStringList> ptn_lst(new TStringList());
	for (int i=0; i<wd_lst.Length; i++) {
		UnicodeString swd = wd_lst[i];
		if (!migemo_sw && !regex_sw) {
			do {
				UnicodeString s = split_tkn(swd, '|');
				if (!s.IsEmpty()) {
					if (s.Pos('*') || s.Pos('?')) {
						s = TRegEx::Escape(s);
						s = REPLACE_TS(s, "\\?", "[^\\\\:]");
						s = REPLACE_TS(s, "\\*", "[^\\\\:]*");
					}
					else
						s = TRegEx::Escape(s);

					ptn_lst->Add(s);
				}
			} while (!swd.IsEmpty());
		}
		else {
			UnicodeString ptn = regex_sw? swd : usr_Migemo->GetRegExPtn(migemo_sw, swd);
			if (!ptn.IsEmpty()) ptn_lst->Add(ptn);
		}
	}

	lst->Clear();
	TRegExOptions opt; opt << roIgnoreCase;
	for (int i=0; i<ptn_lst->Count; i++) {
		TMatchCollection mts = TRegEx::Matches(lbuf, ptn_lst->Strings[i], opt);
		for (int j=0; j<mts.Count; j++) lst->Add(mts.Item[j].Value);
	}

	return lst->Count;
}

//---------------------------------------------------------------------------
//��̋����\�� (URL�����L��)
//---------------------------------------------------------------------------
void EmphasisTextOut(
	UnicodeString s,		//�\��������
	TStringList *kw_lst,	//������̃��X�g
	TCanvas *cv,
	int &x,				//[i/o] �\��X�ʒu
	int y,				//[i]	�\��Y�ʒu
	bool case_sns,		//�召���������   (default = false)
	bool only_top,		//�擪�̌ꂾ������ (default = false)
	TColor fg,			//���������F (default = col_fgEmp)
	TColor bg)			//�����w�i�F (default = col_bgEmp)
{
	if (s.IsEmpty()) return;

	int s_len = s.Length();

	std::unique_ptr<TColor[]> FgCol(new TColor[s_len + 1]);
	std::unique_ptr<TColor[]> BgCol(new TColor[s_len + 1]);

	for (int i=0; i<=s_len; i++) {	//0 �� ���ݐF
		FgCol[i] = cv->Font->Color;
		BgCol[i] = cv->Brush->Color;
	}

	//URL����
	if (s.Pos("://")) {	//URL �������ԒZ�k�̂��߂̑O�`�F�b�N
		TMatch mt = TRegEx::Match(s, URL_MATCH_PTN);
		if (mt.Success) for (int j=0,p=mt.Index; j<mt.Length; j++,p++) FgCol[p] = col_URL;
	}

	//�L�[���[�h����
	if (kw_lst) {
		int ofs = 1;
		for (;;) {
			int p1 = 0;	//����������̖����ʒu + 1 �̍Ō��
			for (int i=0; i<kw_lst->Count; i++) {
				UnicodeString kwd = kw_lst->Strings[i];
				int p = case_sns ? PosEx(kwd, s, ofs) : PosEx(kwd.UpperCase(), s.UpperCase(), ofs);
				if (p>0) {
					int len = kwd.Length();
					for (int j=0,k=p; j<len; j++,k++) {
						FgCol[k] = fg;  BgCol[k] = bg;
					}
					p1 = std::max(p + len, p1);
				}
			}
			if (p1==0 || only_top) break;
			ofs = p1;
		}
	}

	//�R���g���[���R�[�h
	for (int i=1; i<=s_len; i++) {
		if (iscntrl(s[i]) && (s[i]!='\t')) {
			s[i] = (WideChar)((int)s[i] + 0x40);
			FgCol[i] = col_Ctrl;
		}
	}

	//������`��
	cv->Font->Color  = FgCol[1];
	cv->Brush->Color = BgCol[1];
	UnicodeString sbuf;
	for (int j=1; j<=s_len; j++) {
		if (FgCol[j]==cv->Font->Color && BgCol[j]==cv->Brush->Color)
			sbuf.cat_sprintf(_T("%c"), s[j]);
		else {
			TabCrTextOut(sbuf, cv, x, y, cv->Font->Color);
			sbuf = s[j];
			cv->Font->Color  = FgCol[j];
			cv->Brush->Color = BgCol[j];
		}
	}
	if (!sbuf.IsEmpty()) TabCrTextOut(sbuf, cv, x, y, cv->Font->Color);

	cv->Font->Color  = FgCol[0];
	cv->Brush->Color = BgCol[0];
}

//---------------------------------------------------------------------------
void EmphasisTextOut(
	UnicodeString s,
	UnicodeString kwd,	//������
	TCanvas *cv,
	int &x,				//[i/o] �\��X�ʒu
	int y,				//[i]	�\��Y�ʒu
	bool case_sns,		//�召���������   (default = false)
	bool only_top,		//�擪�̌ꂾ������ (default = false)
	TColor fg,			//���������F (default = col_fgEmp)
	TColor bg)			//�����w�i�F (default = col_bgEmp)
{
	std::unique_ptr<TStringList> kwd_lst(new TStringList());
	kwd_lst->Text = kwd;
	EmphasisTextOut(s, kwd_lst.get(), cv, x, y, case_sns, only_top, fg, bg);
}

//---------------------------------------------------------------------------
//�^�u����s���܂ޕ�����̕`��
//---------------------------------------------------------------------------
void TabCrTextOut(
	UnicodeString s,	//�\��������
	TCanvas *cv,
	int &x,				//[i/o] �\��X�ʒu
	int y,				//[i]	�\��Y�ʒu
	TColor fg,			//�����F
	int max_x)			//�����ʒu (default = 0 : ����)
{
	if (max_x>0 && x>=max_x) return;

	s = REPLACE_TS(s, "\r", "");
	int slen  = s.Length();
	int yh = y + cv->TextHeight(s);
	bool is_irreg = IsIrregularFont(cv->Font);

	UnicodeString sbuf;

	TMatchCollection mts = TRegEx::Matches(s, "[\t\n]");
	int p0 = 1;
	for (int i=0; i<mts.Count && p0<=slen; i++) {
		//������
		int p1 = mts.Item[i].Index;
		if (p1>p0) {
			sbuf = s.SubString(p0, p1 - p0);
			cv->Font->Color = fg;
			int w = get_TextWidth(cv, sbuf, is_irreg);
			cv->TextRect(Rect(x, y, x + w, yh), x, y, sbuf);
			x += w;
		}
		//�^�u or ���s
		bool is_tab = USAME_TS(s.SubString(p1, 1), "\t");
		sbuf = is_tab? L"\u21c0" : L"\u21b5";
		cv->Font->Color = is_tab? col_TAB : col_CR;
		cv->TextOut(x, y, sbuf);
		cv->Font->Color = fg;
		x += get_TextWidth(cv, sbuf, is_irreg);
		if (max_x>0 && x>=max_x) break;
		p0 = p1 + 1;
	}

	//������
	if (p0<=slen && (max_x==0 || x<max_x)) {
		sbuf = s.SubString(p0, slen - p0 + 1);
		cv->Font->Color = fg;
		int w = get_TextWidth(cv, sbuf, is_irreg);
		cv->TextRect(Rect(x, y, x + w, yh), x, y, sbuf);
		x += w;
	}
}

//---------------------------------------------------------------------------
//�r���A�^�u���܂ލs�̕`��
//---------------------------------------------------------------------------
void RuledLnTextOut(
	UnicodeString s,		//�\��������
	TCanvas *cv,
	TRect &rc,				//�\���ʒu (rc.Left �X�V)
	TColor fg,				//�����F
	int tab_wd,				//�^�u��			(default = 8)
	TStringList *kw_lst)	//������̃��X�g	(default = NULL)
{
	bool is_irreg = IsIrregularFont(cv->Font);

	int slen = s.Length();
	int xp	 = rc.Left + 2;
	int yp	 = rc.Top + 1;
	UnicodeString sbuf;

	//�^�u�����v�Z
	TEXTMETRIC tm;
	::GetTextMetrics(cv->Handle , &tm);
	int w_tab = tab_wd * tm.tmAveCharWidth;

	TMatchCollection mts = TRegEx::Matches(s, "[\t�\����������������������]");
	int p0 = 1;
	for (int i=0; i<mts.Count && p0<=slen; i++) {
		//������
		int p1 = mts.Item[i].Index;
		if (p1>p0) {
			sbuf = s.SubString(p0, p1 - p0);
			cv->Font->Color = fg;
			EmphasisTextOut(sbuf, kw_lst, cv, xp, yp);
		}
		//�r��
		sbuf = s.SubString(p1, 1);
		cv->Pen->Style = psSolid;
		cv->Pen->Width = 1;
		cv->Pen->Color = col_HR;
		int w  = get_TextWidth(cv, sbuf, is_irreg);
		int xc = xp + w/2;
		int yc = rc.Top + (rc.Bottom - rc.Top) / 2;
		if (starts_tchs(_T("������"), sbuf)) {
			cv->MoveTo(xc, rc.Top);  cv->LineTo(xc, rc.Bottom);	//�c��
			if (starts_tchs(_T("����"), sbuf)) {
				cv->MoveTo(xc, yc); cv->LineTo(USAME_TS(sbuf, "��")? xc + w : xp - 1, yc);
			}
		}
		else if (starts_tchs(_T("�\��������"), sbuf)) {
			cv->MoveTo(xp, yc);  cv->LineTo(xp + w, yc);	//����
			if (USAME_TS(sbuf, "��")) {
				cv->MoveTo(xc, rc.Top);  cv->LineTo(xc, rc.Bottom);
			}
			else if (starts_tchs(_T("����"), sbuf)) {
				cv->MoveTo(xc, yc); cv->LineTo(xc, USAME_TS(sbuf, "��")? rc.Bottom : rc.Top - 1);
			}
		}
		else if (starts_tchs(_T("����"), sbuf)) {
			cv->MoveTo(xc, rc.Bottom); cv->LineTo(xc, yc);
			cv->LineTo(USAME_TS(sbuf, "��")? xp + w : xp - 1, yc);
		}
		else if (starts_tchs(_T("����"), sbuf)) {
			cv->MoveTo(xc, rc.Top);  cv->LineTo(xc, yc);
			cv->LineTo(USAME_TS(sbuf, "��")? xp + w : xp - 1, yc);
		}
		//�^�u
		else if (USAME_TS(sbuf, "\t")) {
			UnicodeString tabstr = ShowTAB? L"\u21c0" : L" ";
			int dt = (xp - (rc.Left + 2)) % w_tab;
			xp += (w_tab - dt - get_TextWidth(cv, tabstr, is_irreg));
			cv->Font->Color = col_TAB;
			cv->TextOut(xp, yp, tabstr);
			w = get_TextWidth(cv, tabstr, is_irreg);
		}

		xp += w;
		if (xp>=rc.Right) break;
		p0 = p1 + 1;
	}

	//������
	if (p0<=slen && xp<rc.Right) {
		sbuf = s.SubString(p0, slen - p0 + 1);
		cv->Font->Color = fg;
		EmphasisTextOut(sbuf, kw_lst, cv, xp, yp);
	}

	rc.Left = std::min(xp, (int)rc.Right);
}

//---------------------------------------------------------------------------
//�e�L�X�g�v���r���[��`��
//---------------------------------------------------------------------------
void PrvTextOut(
	TListBox *lp,			//���X�g�{�b�N�X
	int idx,				//�C���f�b�N�X
	TCanvas *cv, TRect &rc,
	TColor fg,				//�����F
	int tab_wd,				//�^�u��
	TStringList *kw_lst,	//������̃��X�g			(default = NULL)
	UnicodeString fnam,		//�R�����g����p�t�@�C����	(default = EmptyStr)
	bool en_mlt)			//�����s�R�����g�ɑΉ�		(default = true)
{
	//�R�����g�̔���
	int p = (EmpComment && fg!=col_fgSelItem)?
				UserHighlight->GetCommentPos(fnam, lp, idx, en_mlt) : 0;

	UnicodeString s = lp->Items->Strings[idx];
	if (s.Length()>1024) s = s.SubString(1, 1024).UCAT_T("�c");	//*** ����������

	//�R�����g�L��
	if (p>0 && p<=s.Length()) {
		if (p>1) RuledLnTextOut(s.SubString(1, p - 1), cv, rc, fg, tab_wd, kw_lst);
		RuledLnTextOut(s.SubString(p, s.Length() - p + 1), cv, rc, col_Comment, tab_wd, kw_lst);
	}
	//�R�����g����
	else {
		RuledLnTextOut(s, cv, rc, fg, tab_wd, kw_lst);
	}
}

//---------------------------------------------------------------------------
//�s�ԍ���`��
//---------------------------------------------------------------------------
void LineNoOut(TCanvas *cv, TRect &rc, UnicodeString l_str)
{
	//�w�i
	int w_ln = get_CharWidth(cv, 6, ScaledInt(4));
	TRect l_rc = rc;
	cv->Brush->Color = col_bgLineNo;
	l_rc.Right = l_rc.Left + w_ln;
	cv->FillRect(l_rc);

	//�ԍ�
	if (!l_str.IsEmpty()) {
		cv->Font->Color = col_LineNo;
		cv->TextOut(
			l_rc.Left + w_ln - get_TextWidth(cv, l_str, IsIrregularFont(cv->Font)) - get_CharWidth(cv, 1)/2,
			l_rc.Top + 1,
			l_str);
	}

	//���E��
	cv->Pen->Style = psSolid;
	cv->Pen->Width = 1;
	cv->Pen->Color = col_bdrLine;
	cv->MoveTo(l_rc.Right - 1, l_rc.Top);
	cv->LineTo(l_rc.Right - 1, l_rc.Bottom);
	rc.Left = l_rc.Right - 1;
}
//---------------------------------------------------------------------------
void LineNoOut(TCanvas *cv, TRect &rc, int l_no)
{
	UnicodeString l_str;
	if (l_no>0) l_str.sprintf(_T("%u"), l_no);
	LineNoOut(cv, rc, l_str);
}

//---------------------------------------------------------------------------
//�p�X����`��
//  \ �� /�A���x����
//---------------------------------------------------------------------------
void PathNameOut(
	UnicodeString s,		//�\��������
	TStringList *kw_lst,	//������̃��X�g (�s�v�Ȃ�NULL)
	TCanvas *cv,
	int &x, int y,			//�\���ʒu (x �͍X�V)
	bool mgn_sw)			//true = / �̑O��Ƀ}�[�W�������� (default = true)
{
	if (s.IsEmpty()) return;

	int s_len = s.Length();
	std::unique_ptr<TColor[]> FgCol(new TColor[s_len + 1]);
	std::unique_ptr<TColor[]> BgCol(new TColor[s_len + 1]);

	for (int i=0; i<=s_len; i++) {	//0 �� ���ݐF
		FgCol[i] = cv->Font->Color;
		BgCol[i] = cv->Brush->Color;
	}

	//�L�[���[�h����
	if (kw_lst) {
		int ofs = 1;
		for (;;) {
			int p1 = 0;	//����������̖����ʒu + 1 �̍Ō��
			for (int i=0; i<kw_lst->Count; i++) {
				UnicodeString kwd = kw_lst->Strings[i];
				int p = PosEx(kwd.UpperCase(), s.UpperCase(), ofs);
				if (p>0) {
					int len = kwd.Length();
					for (int j=0,k=p; j<len; j++,k++) {
						FgCol[k] = col_fgEmp;  BgCol[k] = col_bgEmp;
					}
					p1 = std::max(p + len, p1);
				}
			}
			if (p1==0) break;
			ofs = p1;
		}
	}

	//������`��
	bool is_irreg = IsIrregularFont(cv->Font);
	TColor fg_sep = AdjustColor(cv->Font->Color, 96);
	s = yen_to_slash(s);
	int yh = y + cv->TextHeight("Q");
	int s_wd = get_TextWidth(cv, "/", is_irreg);
	int s_mg = s_wd/3;	//***
	cv->Font->Color  = FgCol[1];
	cv->Brush->Color = BgCol[1];
	UnicodeString sbuf;
	for (int i=1; i<=s_len; i++) {
		WideChar c = s[i];
		if (FgCol[i]!=cv->Font->Color || BgCol[i]!=cv->Brush->Color || EndsStr('/', sbuf)) {
			if (!sbuf.IsEmpty()) {
				bool flag = remove_end_s(sbuf, '/');
				int w = get_TextWidth(cv, sbuf, is_irreg);
				if (w>0) {
					cv->TextRect(Rect(x, y, x + w, yh), x, y, sbuf);
					x += w;
				}
				if (flag) {
					if (cv->Font->Color!=col_fgEmp) cv->Font->Color = fg_sep;
					if (mgn_sw && i>3) x += s_mg;
					cv->TextRect(Rect(x, y, x + s_wd, yh), x, y, "/");
					x += s_wd;
					if (mgn_sw && i>3) x += s_mg;
				}
			}
			sbuf = c;
			cv->Font->Color  = FgCol[i];
			cv->Brush->Color = BgCol[i];
		}
		else {
			sbuf.cat_sprintf(_T("%c"), c);
		}
	}

	if (!sbuf.IsEmpty()) {
		bool flag = remove_end_s(sbuf, '/');
		int w = get_TextWidth(cv, sbuf, is_irreg);
		if (w>0) {
			cv->TextRect(Rect(x, y, x + w, yh), x, y, sbuf);
			x += w;
		}
		if (flag) {
			if (cv->Font->Color!=col_fgEmp) cv->Font->Color = fg_sep;
			if (mgn_sw) x += s_mg;
			cv->TextRect(Rect(x, y, x + s_wd, yh), x, y, "/");
			x += s_wd;
			if (mgn_sw) x += s_mg;
		}
	}

	cv->Font->Color  = FgCol[0];
	cv->Brush->Color = BgCol[0];
}
//---------------------------------------------------------------------------
void PathNameOut(
	UnicodeString s,
	TCanvas *cv,
	int &x, int y,		//�\���ʒu (x �͍X�V)
	int w)				//������ (default = 0 : ������)
{
	if (s.IsEmpty()) return;

	if (w>0) s = get_MiniPathName(s, w, cv->Font);
	int s_len = s.Length();

	bool is_irreg = IsIrregularFont(cv->Font);
	TColor fg_org = cv->Font->Color;
	TColor fg_sep = AdjustColor(fg_org, 96);

	s = yen_to_slash(s);
	int yh = y + cv->TextHeight("Q");
	int s_wd = get_TextWidth(cv, "/", is_irreg);
	UnicodeString sbuf;
	for (int i=1; i<=s_len; i++) {
		WideChar c = s[i];
		if (EndsStr('/', sbuf)) {
			if (!sbuf.IsEmpty()) {
				bool flag = remove_end_s(sbuf, '/');
				int w = get_TextWidth(cv, sbuf, is_irreg);
				if (w>0) {
					cv->Font->Color = fg_org;
					cv->TextRect(Rect(x, y, x + w, yh), x, y, sbuf);
					x += w;
				}
				if (flag) {
					cv->Font->Color = fg_sep;
					cv->TextRect(Rect(x, y, x + s_wd, yh), x, y, "/");
					x += s_wd;
				}
			}
			sbuf = c;
		}
		else {
			sbuf.cat_sprintf(_T("%c"), c);
		}
	}

	if (!sbuf.IsEmpty()) {
		bool flag = remove_end_s(sbuf, '/');
		int w = get_TextWidth(cv, sbuf, is_irreg);
		if (w>0) {
			cv->Font->Color = fg_org;
			cv->TextRect(Rect(x, y, x + w, yh), x, y, sbuf);
			x += w;
		}
		if (flag) {
			cv->Font->Color = fg_sep;
			cv->TextRect(Rect(x, y, x + s_wd, yh), x, y, "/");
			x += s_wd;
		}
	}
}

//---------------------------------------------------------------------------
//�t�@�C������`��
//---------------------------------------------------------------------------
void FileNameOut(
	TCanvas *cv, TRect &rc,
	UnicodeString fnam,		//�t�@�C����
	bool use_fgsel,			//�I��F���g�p
	bool to_slash,			//\ �� / �ɒu��	(default = false)
	TStringList *kw_lst)	//������̃��X�g	(default = NULL)
{
	UnicodeString dnam = ExtractFilePath(fnam);
	//�f�B���N�g����
	if (!dnam.IsEmpty()) {
		TColor fg = use_fgsel? col_fgSelItem : col_Folder;
		fnam = ExtractFileName(fnam);
		if (to_slash) {
			int xp = rc.Left + 2;
			int yp = rc.Top + 1;
			cv->Font->Color = fg;
			PathNameOut(dnam, kw_lst, cv, xp, yp);
			rc.Left = xp;
		}
		else
			RuledLnTextOut(dnam, cv, rc, fg, 8, kw_lst);
	}

	//�t�@�C����
	RuledLnTextOut(fnam, cv, rc, use_fgsel? col_fgSelItem : get_ExtColor(get_extension(fnam)), 8, kw_lst);
}

//---------------------------------------------------------------------------
//�X�e�[�^�X�o�[�̔w�i��`��
//---------------------------------------------------------------------------
bool draw_SttBarBg(TStatusBar *sp, TMessage &msg)
{
	if (col_bgSttBar==scl_BtnFace) return false;

	std::unique_ptr<TCanvas> cv(new TCanvas());
	cv->Handle = (HDC)msg.WParam;
	cv->Brush->Color = col_bgSttBar;
	cv->FillRect(sp->ClientRect);
	//��[���E
	cv->Pen->Color = SelectWorB(col_bgSttBar, 0.25);
	cv->Pen->Width = 1;
	cv->Pen->Style = psSolid;
	cv->MoveTo(0, 0);
	cv->LineTo(sp->ClientWidth, 0);

	msg.Result = 1;
	return true;
}
//---------------------------------------------------------------------------
//���w�b�_�̔w�i��`��
//---------------------------------------------------------------------------
bool draw_InfHdrBg(TStatusBar *sp, TMessage &msg)
{
	if (col_bgInfHdr==scl_BtnFace) return false;

	std::unique_ptr<TCanvas> cv(new TCanvas());
	cv->Handle = (HDC)msg.WParam;
	cv->Brush->Color = col_bgInfHdr;
	cv->FillRect(sp->ClientRect);

	msg.Result = 1;
	return true;
}

//---------------------------------------------------------------------------
//�w�b�_�[�Z�N�V�����̕`��(�\�[�g�����}�[�N�t)
//---------------------------------------------------------------------------
void draw_SortHeader(
	THeaderControl *hp, THeaderSection *sp,
	TRect rc,
	int mk_mode, 		//0:����/ 1:��/ -1:��
	bool use_syscol)	//�V�X�e���F���g�p		(default = false : �ݒ�F)
{
	TCanvas *cv = hp->Canvas;
	cv->Font->Assign(hp->Font);
	int xp = rc.Left + 4;
	int yp = rc.Top + get_TopMargin(cv) + 1;

	cv->Brush->Color = use_syscol? scl_BtnFace : col_bgListHdr;
	cv->Font->Color  = use_syscol? scl_BtnText : col_fgListHdr;

	//�w�i
	cv->FillRect(rc);
	//�e�L�X�g
	if (rc.Width()>8) cv->TextOut(xp, yp, sp->Text);	//$$$

	//��؂��
	if (sp->Index < hp->Sections->Count-1) {
		cv->Pen->Style = psSolid;
		cv->Pen->Width = 1;
		cv->Pen->Color = SelectWorB(cv->Brush->Color, 0.25);
		cv->MoveTo(rc.Right - 2, rc.Top);  cv->LineTo(rc.Right - 2, rc.Bottom);
	}

	//�}�[�N
	if (mk_mode!=0) {
		xp += cv->TextWidth(sp->Text) + 4;
		draw_SortMark(cv, xp, yp + 1, (mk_mode==1), cv->Font->Color);
	}
}

//---------------------------------------------------------------------------
//���X�g�{�b�N�X�̃��C���J�[�\����`��
//---------------------------------------------------------------------------
void draw_ListCursor(TListBox *lp, TRect &Rect, int Index, TOwnerDrawState State)
{
	bool is_focused = State.Contains(odFocused);
	if (Index==lp->ItemIndex && is_focused) {
		int lw = std::max(CursorWidth, 1);
		int yp = Rect.Bottom - lw;
		draw_Line(lp->Canvas, Rect.Left, yp, Rect.Right, yp, lw, col_Cursor, psSolid);
	}
	if (is_focused) lp->Canvas->DrawFocusRect(Rect);
}
//---------------------------------------------------------------------------
void draw_ListCursor2(TListBox *lp, TRect &Rect, int Index, TOwnerDrawState State)
{
	//�J�[�\��
	bool is_focused = State.Contains(odFocused);
	if (Index==lp->ItemIndex && (is_focused || !lp->Focused())) {
		int lw = is_focused? std::max(CursorWidth, 1) : 1;
		int yp = Rect.Bottom - lw;
		draw_Line(lp->Canvas, Rect.Left, yp, Rect.Right, yp, lw, col_Cursor, is_focused? psSolid : psDot);
	}
	if (is_focused) lp->Canvas->DrawFocusRect(Rect);
}
//---------------------------------------------------------------------------
//�O���b�h�̃��C���J�[�\����`��
//---------------------------------------------------------------------------
void draw_GridCursor(TStringGrid *gp, TRect &Rect, int ARow, TGridDrawState State)
{
	if (gp->Row == ARow) {
		int lw = gp->Focused()? std::max(CursorWidth, 1) : 1;
		int yp = Rect.Bottom - lw;
		draw_Line(gp->Canvas, Rect.Left, yp, Rect.Right + 1, yp, lw, col_Cursor, gp->Focused()? psSolid : psDot);
	}
}

//---------------------------------------------------------------------------
//�摜�����O���b�h��`��
//---------------------------------------------------------------------------
void draw_ImgGrid(TCanvas *cv, Graphics::TBitmap *bmp, bool ext_ln)
{
	//�����O���b�h
	cv->Pen->Style = psSolid;
	cv->Pen->Width = 1;
	cv->Pen->Color = col_ImgGrid;
	//��
	for (int i=1; i<ImgGridHorzN; i++) {
		int x = bmp->Width/ImgGridHorzN * i;
		cv->MoveTo(x, 0);  cv->LineTo(x, bmp->Height);
	}
	//�c
	for (int i=1; i<ImgGridVertN; i++) {
		int y = bmp->Height/ImgGridVertN * i;
		cv->MoveTo(0, y);  cv->LineTo(bmp->Width, y);
	}
	//�O�`��
	if (ext_ln) {
		cv->Brush->Color = col_ImgGrid;
		cv->FrameRect(Rect(0, 0, bmp->Width, bmp->Height));
	}
}

//---------------------------------------------------------------------------
//�o�[�O���t�̕`��
//  0�`50% �΁��� 100%
//---------------------------------------------------------------------------
void draw_BarGraph(TCanvas *cv, TRect rc, double r)
{
	TColor org_col		= cv->Brush->Color;
	TBrushStyle org_stl = cv->Brush->Style;

	//�w�i
	cv->Brush->Color = col_bgPrgBar;
	cv->FillRect(rc);

	//�o�[
	rc.Right = rc.Left + rc.Width() * r;
	cv->Brush->Color =
		(r<=0.5)? clLime : TColor(RGB(std::min((int)((r - 0.5)*1020), 255), std::min((int)((1.0 - r)*1020), 255), 0));
	cv->FillRect(rc);

	cv->Brush->Color = org_col;
	cv->Brush->Style = org_stl;
}

//---------------------------------------------------------------------------
//�i���o�[�̕`��
//---------------------------------------------------------------------------
void draw_ProgressBar(TCanvas *cv, TRect rc, double r)
{
	TRect rc_f = rc;
	TRect rc_b = rc;
	int p_pos  = (int)(rc_b.Width() * r);
	rc_f.Right = rc_f.Left + p_pos;
	rc_b.Left  = rc_f.Right;

	cv->Brush->Color = col_fgPrgBar;
	cv->FillRect(rc_f);
	cv->Brush->Color = col_bgPrgBar;
	cv->FillRect(rc_b);
}

//---------------------------------------------------------------------------
//�����^�u�̕`��
//���N���V�b�N�ȊO�̃e�[�}�ŉ����^�u���������`�悳��Ȃ��s��̑΍�
//---------------------------------------------------------------------------
void draw_BottomTab(TCustomTabControl *Control, int idx, const TRect rc, bool active)
{
	TTabControl *tp = (TTabControl*)Control;
	TCanvas *cv = tp->Canvas;
	//�w�i
	cv->Brush->Color = scl_BtnFace;
	cv->FillRect(rc);
	//����
	UnicodeString titstr = tp->Tabs->Strings[idx];
	cv->Font->Color = scl_BtnText;
	cv->TextOut(
		rc.Left + (rc.Width() - cv->TextWidth(titstr))/2,
		rc.Bottom - cv->TextHeight(titstr) - 4,
		titstr);
}

//---------------------------------------------------------------------------
//�e�L�X�g���t�@�C���ɕۑ�
//---------------------------------------------------------------------------
bool saveto_TextFile(UnicodeString fnam, TStrings *lst, TEncoding *enc)
{
	try {
		if (enc)
			lst->SaveToFile(fnam, enc);
		else
			lst->SaveToFile(fnam);
		return true;
	}
	catch (...) {
		return false;
	}
}
//---------------------------------------------------------------------------
bool saveto_TextFile(UnicodeString fnam, TStrings *lst, int enc_idx)
{
	int code_page = (enc_idx>=0 && enc_idx<MAX_SAVE_CODEPAGES)? SaveCodePages[enc_idx].page : 932;
	std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
	return saveto_TextFile(fnam, lst, enc.get());
}

//---------------------------------------------------------------------------
bool saveto_TextUTF8(UnicodeString fnam, TStrings *lst)
{
	return saveto_TextFile(fnam, lst, TEncoding::UTF8);
}

//---------------------------------------------------------------------------
//�����R�[�h�𔻒�(�w��)���ăe�L�X�g�t�@�C����ǂݍ���
//  �߂�l: �R�[�h�y�[�W
//  �����L��̏ꍇ�A�ŏI�s�� TXLIMIT_MARK
//---------------------------------------------------------------------------
int load_text_ex(
	UnicodeString fnam,			//�t�@�C����
	TStringList *f_buf,			//�擾�o�b�t�@
	int code_page,				//�R�[�h�y�[�W		(default = 0 : �擾)
	int limit_size,				//�ő�Ǎ��T�C�Y	(default = 0 : ������)
	bool force_txt,				//NULL�������󔒂ɒu�����ċ����Ǎ� �T�C�Y�������̂ݗL�� (default = false)
	UnicodeString *line_brk,	//[o] ���s�R�[�h	(default = NULL);
	double *ave_lsz)			//[o] ���ύs�T�C�Y	(�������̂�)
{
	GlobalErrMsg = EmptyStr;
	f_buf->Clear();

	try {
		//�R�[�h�y�[�W���擾
		if (code_page==0) code_page = get_FileCodePage(fnam);

		//�������X�g���[���ɃT�C�Y�������ēǂݍ���
		if (limit_size>0) {
			std::unique_ptr<TFileStream>   fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
			std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
			ms->CopyFrom(fs.get(), std::min<__int64>(fs->Size, limit_size));

			//UTF8�̏ꍇ�A�����̓r���Ő؂�Ă���Ɠǂݍ��݂Ɏ��s���邽�߁A�[���o�C�g��؂�l�߂�
			if (code_page==65001 && ms->Size<fs->Size) {
				BYTE mbuf[8];
				ms->Seek(-8, soFromEnd);
				if (ms->Read(mbuf, 8)==8) {
					int n = 0;
					for (int i=7; i>=0; i--,n++) {
						if ((mbuf[i] & 0x80)==0x00) break;	//1�o�C�g����
						if ((mbuf[i] & 0xc0)==0xc0) {		//�擪�o�C�g
							n++; break;
						}
					}
					if (n>0) ms->Size = ms->Size - n;
				}
			}

			//�����\���̂��߂� NULL�������󔒂ɒu��
			if (force_txt) {
				ms->Seek(0, soFromBeginning);
				//UTF16
				if (code_page==1200 || code_page==1201) {
					WORD d;
					while (ms->Position < ms->Size) {
						if (ms->Read(&d, 2) < 2) break;
						if (d==0) {
							d = (code_page==1200)? 0x0020 : 0x2000;
							ms->Seek((__int64)-2, soCurrent);
							ms->Write(&d, 2);
						}
					}
				}
				//���̑�
				else {
					BYTE d;
					while (ms->Position < ms->Size) {
						if (ms->Read(&d, 1) < 1) break;
						if (d==0) {
							d = 0x20;
							ms->Seek((__int64)-1, soCurrent);
							ms->Write(&d, 1);
						}
					}
				}
			}

			//���s�R�[�h
			if (line_brk) *line_brk = force_txt? EmptyStr : get_StreamLineBreak(ms.get(), code_page);

			//�e�L�X�g�Ƃ��ēǂݍ���
			ms->Seek(0, soFromBeginning);
			if (code_page>0) {
				std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
				f_buf->LoadFromStream(ms.get(), enc.get());
			}
			else
				f_buf->LoadFromStream(ms.get());

			if (ms->Size<fs->Size) {
				if (f_buf->Count>0) {
					f_buf->Delete(f_buf->Count - 1);
					f_buf->Add(TXLIMIT_MARK);	//�s�������}�[�N
				}
			}

			if (ave_lsz && f_buf->Count>0) *ave_lsz = 1.0 * ms->Size / f_buf->Count;
		}
		//���ׂēǂݍ���
		else {
			//���s�R�[�h
			if (!force_txt && line_brk) {
				std::unique_ptr<TFileStream>   fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
				std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
				ms->CopyFrom(fs.get(), std::min<__int64>(fs->Size, TXT_DETECT_SIZE));
				*line_brk = get_StreamLineBreak(ms.get(), code_page);
			}

			//�ǂݍ���
			if (code_page>0) {
				std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
				f_buf->LoadFromFile(fnam, enc.get());
			}
			else
				f_buf->LoadFromFile(fnam);
		}
	}
	catch (...) {
		GlobalErrMsg = SysErrorMessage(GetLastError());
		f_buf->Clear();
		code_page = 0;
	}

	return code_page;
}

//---------------------------------------------------------------------------
//�e�L�X�g�t�@�C���̖������w��s�����ǂݍ���
//  �߂�l: �R�[�h�y�[�W	0 = �G���[
//---------------------------------------------------------------------------
int load_text_tail(
	UnicodeString fnam,
	TStringList *f_buf,
	int code_page,				//�R�[�h�y�[�W		(default = 0 : �擾)
	int limit_ln,				//�ő�Ǎ��s��		(default = 100)
	bool reverse,				//�t���\��			(default = false);
	UnicodeString *line_brk)	//[o] ���s�R�[�h	(default = NULL);
{
	GlobalErrMsg = EmptyStr;
	f_buf->Clear();
	if (limit_ln<=0) return 0;

	if (code_page==0) code_page = get_FileCodePage(fnam, line_brk);
	if (code_page==0) return 0;

	try {
		bool is_BE	= (code_page==1201);
		int  ch_sz	= (code_page==1200 || code_page==1201)? 2 : 1;
		int  ch_sz2 = ch_sz*2;
		BYTE cbuf[2]; cbuf[1] = 0;

		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		fs->Seek(-ch_sz, soFromEnd);
		int lcnt = 0;
		for (;;) {
			if (fs->Read(cbuf, ch_sz)!=ch_sz) break;
			unsigned short c = is_BE? ((cbuf[0] << 8) | cbuf[1]) : ((cbuf[1] << 8) | cbuf[0]);
			if (c==0x000d) {		//CR
				if (++lcnt>limit_ln) break;
				if (fs->Seek(-ch_sz2, soFromCurrent)==0) break;
			}
			else if (c==0x000a) {	//LF
				if (++lcnt>limit_ln) break;
				if (fs->Seek(-ch_sz2, soFromCurrent)==0) break;
				if (fs->Read(cbuf, ch_sz)!=ch_sz) break;
				unsigned short c = is_BE? ((cbuf[0] << 8) | cbuf[1]) : ((cbuf[1] << 8) | cbuf[0]);
				if (fs->Seek(-((c==0x000d)? ch_sz2 : ch_sz), soFromCurrent)==0) break;
			}
			else {
				if (lcnt==0 && c!=0x001a) lcnt++;
				if (fs->Seek(-ch_sz*2, soFromCurrent)==0) break;
			}
		}

		//�e�L�X�g�Ƃ��ēǂݍ���
		std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
		f_buf->LoadFromStream(fs.get(), enc.get());

		//�t��
		if (reverse) {
			std::unique_ptr<TStringList> tbuf(new TStringList());
			for (int i=f_buf->Count-1; i>=0; i--) tbuf->Add(f_buf->Strings[i]);
			f_buf->Assign(tbuf.get());
		}
	}
	catch (...) {
		GlobalErrMsg = SysErrorMessage(GetLastError());
		f_buf->Clear();
		code_page = 0;
	}

	return code_page;
}
//---------------------------------------------------------------------------
//�e�L�X�g�t�@�C���̖����̒ǉ����e���`�F�b�N
//---------------------------------------------------------------------------
bool check_text_tail(UnicodeString fnam, int &code_page, int size, UnicodeString kwd, TStringList *lst)
{
	if (code_page==0) code_page = get_FileCodePage(fnam);
	if (code_page==0) return false;

	try {
		bool is_BE = (code_page==1201);
		int  ch_sz = (code_page==1200 || code_page==1201)? 2 : 1;
		BYTE cbuf[2]; cbuf[1] = 0;

		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
		if (size<fs->Size) {
			fs->Seek(-(size + ch_sz), soFromEnd);
			for (;;) {
				if (fs->Read(cbuf, ch_sz)!=ch_sz) break;
				unsigned short c = is_BE? ((cbuf[0] << 8) | cbuf[1]) : ((cbuf[1] << 8) | cbuf[0]);
				if (c==0x000d || c==0x000a) break;
				if (fs->Seek(-ch_sz*2, soFromCurrent)==0) break;
			}
		}

		//�e�L�X�g�Ƃ��ēǂݍ���
		std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
		lst->LoadFromStream(fs.get(), enc.get());
		//�L�[���[�h�ōi�荞��
		if (!kwd.IsEmpty()) {
			int i=0;
			//���K�\��
			if (is_regex_slash(kwd)) {
				kwd = exclude_top_end(kwd);
				TRegExOptions opt; opt << roIgnoreCase;
				while (i < lst->Count) if (TRegEx::IsMatch(lst->Strings[i], kwd, opt)) i++; else lst->Delete(i);
			}
			//�ʏ�
			else {
				while (i < lst->Count) if (ContainsText(lst->Strings[i], kwd)) i++; else lst->Delete(i);
			}
		}
		return (lst->Count>0);
	}
	catch (...) {
		return false;
	}
}

//---------------------------------------------------------------------------
//�e�L�X�g�t�@�C���̐擪�s���擾
//---------------------------------------------------------------------------
UnicodeString get_top_line(UnicodeString fnam)
{
	UnicodeString ret_str;
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));

		//�R�[�h�y�[�W�擾
		std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
		ms->CopyFrom(fs.get(), std::min<__int64>(fs->Size, TXT_DETECT_SIZE));
		int code_page = get_MemoryCodePage(ms.get());
		if (code_page==0) Abort();

		//�擪�s�̃T�C�Y�擾
		bool is_BE = (code_page==1201);
		int  ch_sz = (code_page==1200 || code_page==1201)? 2 : 1;
		BYTE cbuf[2]; cbuf[1] = 0;
		ms->Seek(0, soFromBeginning);
		while (ms->Position < ms->Size) {
			if (ms->Read(cbuf, ch_sz)!=ch_sz) break;
			unsigned short c = is_BE? ((cbuf[0] << 8) | cbuf[1]) : ((cbuf[1] << 8) | cbuf[0]);
			if (c==0x000d || c==0x000a) {
				ms->Seek(-ch_sz, soFromCurrent);
				break;
			}
		}
		__int64 size = ms->Position;
		if (size==0) Abort();

		//�o�b�t�@�ɓǂݍ���
		TBytes Bytes;
		Bytes.Length = size;
		ms->Seek(0, soFromBeginning);
		ms->Read(Bytes, size);

		//�G���R�[�h
		std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
		ret_str = enc->GetString(Bytes, 0, Bytes.Length);
	}
	catch (...) {
		ret_str = EmptyStr;
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//XML�̐錾�����擾����
//---------------------------------------------------------------------------
void get_xml_inf(
	UnicodeString fnam,		//�t�@�C���� or "<?xml " ���܂ލs���e
	TStringList *lst)		//�擾�o�b�t�@(���O=�l �̌`���Ŋi�[);
{
	UnicodeString lbuf = fnam;
	if (!contains_i(lbuf, _T("<?xml "))) lbuf = get_top_line(fnam);
	if (contains_i(lbuf, _T("<?xml ")) && contains_i(lbuf, _T("?>"))) {
		lbuf = Trim(get_tkn_m(lbuf, _T("<?xml "), _T("?>")));
		while (!lbuf.IsEmpty()) {
			UnicodeString vstr = exclude_quot(get_tkn_m(lbuf, '=', ' '));
			if (vstr.IsEmpty()) break;
			UnicodeString nstr = REPLACE_TS(get_tkn(lbuf, '='), "version", "XML version");
			lst->Add(UnicodeString().sprintf(_T("%*s: %s"), FPRP_NAM_WD, nstr.c_str(), vstr.c_str()));
			lbuf = get_tkn_r(lbuf, ' ');
		}
	}
}

//---------------------------------------------------------------------------
//autorun.inf ����A�C�R���t�@�C�������擾
//---------------------------------------------------------------------------
UnicodeString get_autorun_ico(UnicodeString fnam)
{
	UnicodeString ico_nam;
	std::unique_ptr<TStringList> fbuf(new TStringList());
	if (load_text_ex(fnam, fbuf.get())!=0) {
		bool s_flag = false;
		for (int i=0; i<fbuf->Count; i++) {
			UnicodeString lbuf = Trim(fbuf->Strings[i]);
			if (USAME_TI(lbuf, "[autorun]"))
				s_flag = true;
			else if (s_flag && USTARTS_TI("icon=", lbuf)) {
				ico_nam = ExtractFilePath(fnam) + get_tkn_r(lbuf, '=');  break;
			}
		}
		if (!file_exists(ico_nam)) ico_nam = EmptyStr;
	}
	return ico_nam;
}

//---------------------------------------------------------------------------
//�e�L�X�g�G�f�B�^�ŊJ��
//---------------------------------------------------------------------------
bool open_by_TextEditor(UnicodeString fnam, int lno)
{
	try {
		GlobalErrMsg = EmptyStr;
		if (!file_exists(fnam))   SysErrAbort(ERROR_FILE_NOT_FOUND);
		UnicodeString editor = get_actual_path(TextEditor);
		if (!file_exists(editor)) UserAbort(USTR_AppNotFound);

		//�N���p�����[�^�̍\��
		UnicodeString p_fnam = add_quot_if_spc(fnam);
		UnicodeString prmstr;
		if (lno>0) {
			if (!TextEditorFrmt.IsEmpty()) {
				prmstr = TextEditorFrmt;
				prmstr = ReplaceStr(prmstr, "$F", p_fnam);
				prmstr = ReplaceStr(prmstr, "$L", IntToStr(lno));
			}
			else prmstr = p_fnam;
		}
		else prmstr = p_fnam;

		if (!Execute_ex(editor, prmstr)) UserAbort(USTR_FaildExec);

		//�ҏW�������X�V
		if (!ContainsText(fnam, TempPathA)) add_TextEditHistory(fnam);
		return true;
	}
	catch (EAbort &e) {
		GlobalErrMsg = e.Message;
		return false;
	}
}

//---------------------------------------------------------------------------
//�O���e�L�X�g�r���A�[�ŊJ��
//---------------------------------------------------------------------------
bool open_by_ExtTextViewer(UnicodeString fnam, int lno)
{
	try {
		GlobalErrMsg = EmptyStr;
		if (!file_exists(fnam))   SysErrAbort(ERROR_FILE_NOT_FOUND);
		UnicodeString viewer = get_actual_path(ExtTxViewer);
		if (!file_exists(viewer)) UserAbort(USTR_AppNotFound);

		//�N���p�����[�^�̍\��
		UnicodeString p_fnam = add_quot_if_spc(fnam);
		UnicodeString prmstr;
		if (lno>0) {
			if (!ExtTxViewerFrmt.IsEmpty()) {
				prmstr = ExtTxViewerFrmt;
				prmstr = ReplaceStr(prmstr, "$F", p_fnam);
				prmstr = ReplaceStr(prmstr, "$L", IntToStr(lno));
			}
			else prmstr = p_fnam;
		}
		else prmstr = p_fnam;

		if (!Execute_ex(viewer, prmstr)) UserAbort(USTR_FaildExec);

		return true;
	}
	catch (EAbort &e) {
		GlobalErrMsg = e.Message;
		return false;
	}
}

//---------------------------------------------------------------------------
//��������t�@�C�����ƍs�ԍ��ɕ���
//---------------------------------------------------------------------------
bool divide_FileName_LineNo(
	UnicodeString &fnam,	//[i]   �t�@�C�������܂ޕ�����(ctags ����)
							//[o]   �p�X�t���t�@�C����
	int &lno,				//[i/o] �s�ԍ�
	UnicodeString rnam,		//[i]   ��f�t�H���g�� (default = EmptyStr/ ExePath)
	int pos)				//[i]   �擾�J�n�ʒu	 (default = 0)
{
	GlobalErrMsg = EmptyStr;

	rnam = ExtractFilePath(rnam);
	if (rnam.IsEmpty()) rnam = ExePath;

	do {
		fnam = Trim(fnam);
		if (fnam.IsEmpty()) break;

		//html �����N
		TRegExOptions opt; opt << roIgnoreCase;
		TMatchCollection mts = TRegEx::Matches(fnam, "\\bhref=\".*?\"", opt);
		bool found = false;
		for (int i=0; i<mts.Count && !found; i++) {
			int p1 = mts.Item[i].Index + mts.Item[i].Length;
			if (mts.Count==1 || pos<p1) {
				fnam = exclude_quot(Trim(get_tkn_r(mts.Item[i].Value, '=')));
				fnam = get_tkn(fnam, '#');
				remove_top_text(fnam, _T("file:///"));
				fnam = slash_to_yen(fnam);
				lno = 1; found = true;
			}
		}
		if (found) break;

		//�}�[�N�_�E��
		mts = TRegEx::Matches(fnam, "\\[.*\\]\\([^*?<>]+\\.\\w+(#\\w+)?\\)", opt);
		found = false;
		for (int i=0; i<mts.Count && !found; i++) {
			int p1 = mts.Item[i].Index + mts.Item[i].Length;
			if (mts.Count==1 || pos<p1) {
				fnam = get_in_paren(mts.Item[i].Value);
				fnam = get_tkn(fnam, '#');
				fnam = slash_to_yen(fnam);
				fnam = rel_to_absdir(fnam, rnam);
				lno = 1; found = true;
			}
		}
		if (found) break;

		//���[�J���t�@�C��
		mts = TRegEx::Matches(fnam, LOCAL_FILE_PTN);
		for (int i=0; i<mts.Count && !found; i++) {
			int p1 = mts.Item[i].Index + mts.Item[i].Length;
			if (mts.Count==1 || pos<p1) {
				fnam = exclude_quot(Trim(get_tkn_r(mts.Item[i].Value, _T("file:///"))));
				fnam = get_tkn(fnam, '#');
				fnam = slash_to_yen(fnam);
				remove_end_s(fnam, ')');
				lno = 1; found = true;
			}
		}
		if (found) break;

		//c �C���N���[�h
		if (contains_s(fnam, _T("#include"))) {
			fnam = exclude_quot(Trim(get_tkn_r(fnam, _T("#include"))));
			if (contains_s(fnam, _T('<'))) fnam = get_tkn_m(fnam, '<', '>');
			lno = 1;	break;
		}

		//ctags �t�H�[�}�b�g
		TMatch mt = TRegEx::Match(fnam, "^.+\\t.+\\.\\w+\\t(\\d+|/.+/)", opt);
		if (mt.Success) {
			UnicodeString lbuf = get_post_tab(get_tkn(fnam, ";\""));
			UnicodeString nptn = get_post_tab(lbuf);
			fnam = get_pre_tab(lbuf);
			//�p�^�[��
			if (nptn.Length()>=2
				&& ((starts_t_s('/', nptn) && ends_t_s('/', nptn)) || (starts_t_s('?', nptn) && ends_t_s('?', nptn))))
			{
				lno = 1;
				nptn = exclude_top_end(nptn);
				nptn = REPLACE_TS(nptn, "\\/", "/");
				bool p_top = remove_top_s(nptn, '^');
				bool p_end = remove_end_s(nptn, '$');
				UnicodeString nkwd = nptn;
				nptn = TRegEx::Escape(nptn);
				if (p_top) nptn.Insert("^", 1);
				if (p_end) nptn.UCAT_T("$");
				if (chk_RegExPtn(nptn)) {
					fnam = rel_to_absdir(fnam, rnam);
					if (file_exists(fnam)) {
						std::unique_ptr<TStringList> fbuf(new TStringList());
						if (load_text_ex(fnam, fbuf.get())!=0) {
							TRegExOptions opt;
							for (int i=0; i<fbuf->Count; i++) {
								UnicodeString lbuf = fbuf->Strings[i];
								if (lbuf.Pos(nkwd)==0) continue;
								if (TRegEx::IsMatch(fbuf->Strings[i], nptn, opt)) {
									lno = i+1;	break;
								}
							}
						}
					}
				}
			}
			//�s�ԍ�
			else
				lno = nptn.ToIntDef(1);
			break;
		}

		//���
		UnicodeString lstr;
		if (remove_top_s(fnam, '\"')) {
			lstr = Trim(get_tkn_r(fnam, '\"'));
			fnam = get_tkn(fnam, '\"');
		}
		else {
			int p;
			int p0 = fnam.Pos(' ');
			int p1 = fnam.Pos('(');
			if (p0==0) p = p1; else if (p1==0) p = p0; else p = std::min(p0, p1);
			if (p>0) {
				lstr = fnam.SubString(p + 1, fnam.Length() - p);
				fnam = fnam.SubString(1, p - 1);
			}
		}

		//�s�ԍ����擾
		int p;
		int p0 = lstr.Pos(':');
		int p1 = lstr.Pos(')');
		if (p0==0) p = p1; else if (p1==0) p = p0; else p = std::min(p0, p1);
		if (p>0) {
			lstr = lstr.SubString(1, p - 1);
			if (contains_s(lstr, _T('('))) lstr = get_tkn_r(lstr, '(');
			lstr = Trim(lstr);
		}
		lno = lstr.ToIntDef(1);
	} while (0);

	fnam = rel_to_absdir(fnam, rnam);

	if (fnam.IsEmpty())
		GlobalErrMsg.USET_T("�W�����v�悪�擾�ł��܂���");
	else if (!file_exists(fnam))
		GlobalErrMsg = SysErrorMessage(ERROR_FILE_NOT_FOUND);

	return GlobalErrMsg.IsEmpty();
}

//---------------------------------------------------------------------------
//tags �t�@�C�����擾
//---------------------------------------------------------------------------
UnicodeString get_tags_file(UnicodeString rnam)
{
	if (rnam.IsEmpty()) return EmptyStr;

	rnam = ExtractFilePath(rnam);
	UnicodeString tags = IncludeTrailingPathDelimiter(rnam) + "tags";
	while (!file_exists(tags)) {
		if (is_root_dir(rnam)) break;
		rnam = IncludeTrailingPathDelimiter(get_parent_path(rnam));
		tags = rnam + "tags";
	}

	if (!file_exists(tags)) tags = EmptyStr;

	return tags;
}

//---------------------------------------------------------------------------
//�^�O������^�O�����擾
//	�^�O�� \t �t�@�C���� \t �����p�^�[ ��/�s�ԍ�;"�` �`���̃��X�g���擾
//---------------------------------------------------------------------------
int get_FileNameByTag(
	UnicodeString tnam,		//[i]   �^�O��
	UnicodeString &rnam,	//[i/o] ��f�t�H���g��
	TStringList *lst,		//[o]   ���ʃ��X�g
	UnicodeString onam)		//�Ώۃt�@�C��	(default = EmptyStr)
{
	GlobalErrMsg = EmptyStr;

	try {
		if (tnam.IsEmpty()) UserAbort(USTR_NoParameter);
		tnam.UCAT_T("\t");

		UnicodeString tags = get_tags_file(rnam);
		if (tags.IsEmpty()) throw EAbort(LoadUsrMsg(USTR_NotFound, _T("tags �t�@�C��")));
		rnam = ExtractFilePath(tags);

		std::unique_ptr<TStringList> f_buf(new TStringList());
		if (load_text_ex(tags, f_buf.get())==0) UserAbort(USTR_FaildLoad);

		lst->Clear();
		for (int i=0; i<f_buf->Count; i++) {
			UnicodeString lbuf = f_buf->Strings[i];
			if (!StartsText(tnam, lbuf)) continue;
			if (!onam.IsEmpty() && !SameText(onam, rel_to_absdir(get_tkn(get_tkn_r(lbuf, '\t'), '\t'), rnam))) continue;
			lst->Add(lbuf);
		}
		if (lst->Count==0) UserAbort(USTR_NotFound);

		return lst->Count;
	}
	catch (EAbort &e) {
		GlobalErrMsg = e.Message;
		return 0;
	}
}


//---------------------------------------------------------------------------
//HtmConv ���f�t�H���g�ŏ�����
//---------------------------------------------------------------------------
void ini_HtmConv_def(HtmConv *htmcnv, UnicodeString fnam)
{
	htmcnv->FileName	 = fnam;
	htmcnv->ToMarkdown	 = ToMarkdown;
	htmcnv->HeaderStr	 = HtmHdrStr;
	htmcnv->InsHrClass	 = HtmInsHrCls;
	htmcnv->InsHrSection = HtmInsHrSct;
	htmcnv->InsHrArticle = HtmInsHrArt;
	htmcnv->InsHrNav	 = HtmInsHrNav;
}

//---------------------------------------------------------------------------
// �w��t�@�C�������s/�J��
//---------------------------------------------------------------------------
bool Execute_ex(
	UnicodeString cmd,		//���s�t�@�C��/ �t�H���_�� or CLSId or shell �R�}���h(������\)
	UnicodeString prm,		//�p�����[�^				(default = EmptyStr)
	UnicodeString wdir,		//��ƃf�B���N�g��			(default = EmptyStr)
	UnicodeString opt,		//�I�v�V���� H=��\��/ W=�I���҂�/ O=�R���\�[���o�͂��擾/ L=���O�ɏo��
							//			 A=�Ǘ��҂Ƃ��Ď��s (W,O ����)
							//							(default = EmptyStr)
	DWORD *exit_code,		//  exit_code: �I���R�[�h	(default = NULL)
	TStringList *o_lst)		//  o_lst: �R���\�[���o��	(default = NULL)
{
	GlobalErrMsg  = EmptyStr;
	if (cmd.IsEmpty()) return false;

	try {
		if (!wdir.IsEmpty() && is_root_dir(wdir)) wdir = IncludeTrailingPathDelimiter(wdir);

		cmd = exclude_quot(cmd);
		if (ends_PathDlmtr(cmd)) {
			if (USTARTS_TS("::{", cmd) || USTARTS_TI("shell:", cmd)) //CLSID or shell �R�}���h
				cmd = ExcludeTrailingPathDelimiter(cmd);
			if (::ShellExecute(NULL, _T("explore"), cmd.c_str(), NULL, NULL, SW_SHOWNORMAL) <= (HINSTANCE)32)
				LastErrAbort();
		}
		else if (contains_s(opt, _T('A'))) {
			if (::ShellExecute(NULL, _T("runas"), cmd.c_str(), prm.c_str(), wdir.c_str(),
				contains_s(opt, _T('H'))? SW_HIDE : SW_SHOWNORMAL) <= (HINSTANCE)32)
					LastErrAbort();
		}
		else if (!contains_s(opt, _T('W')) && !contains_s(opt, _T('O')) && !contains_s(opt, _T('L'))) {
			if (::ShellExecute(NULL, _T("open"), cmd.c_str(), prm.c_str(), wdir.c_str(),
				contains_s(opt, _T('H'))? SW_HIDE : SW_SHOWNORMAL) <= (HINSTANCE)32)
					LastErrAbort();
		}
		else {
			UnicodeString cmdln = add_quot_if_spc(cmd);
			if (!prm.IsEmpty()) cmdln.cat_sprintf(_T(" %s"), prm.c_str());
			if (!Execute_cmdln(cmdln, wdir, opt, exit_code, o_lst)) throw EAbort(GlobalErrMsg);
		}
		return true;
	}
	catch (EAbort &e) {
		GlobalErrMsg = e.Message;
		return false;
	}
}
//---------------------------------------------------------------------------
bool Execute_cmdln(UnicodeString cmdln, UnicodeString wdir,
	UnicodeString opt, DWORD *exit_code, TStringList *o_lst)
{
	GlobalErrMsg  = EmptyStr;
	if (cmdln.IsEmpty()) return false;

	try {
		HANDLE hRead  = NULL;
		HANDLE hWrite = NULL;
		//���O�Ȃ��p�C�v���쐬
		if (contains_wd_i(opt, _T("O|L"))) {
			SECURITY_ATTRIBUTES	sa;
			sa.nLength				= sizeof(sa);
			sa.lpSecurityDescriptor = 0;
			sa.bInheritHandle		= TRUE;
			if (!::CreatePipe(&hRead, &hWrite, &sa, 0)) UserAbort(USTR_FaildProc);
		}

		STARTUPINFO si;
		::ZeroMemory(&si, sizeof(si));
		si.cb		   = sizeof(si);
		si.dwFlags	   = STARTF_USESHOWWINDOW;
		si.wShowWindow = contains_s(opt, _T('H'))? SW_HIDE : SW_SHOWNORMAL;
		if (hRead && hWrite) {
			si.dwFlags |= STARTF_USESTDHANDLES;
			si.hStdOutput = hWrite;
			si.hStdError  = hWrite;
		}

		PROCESS_INFORMATION pi;
		if (::CreateProcess(NULL, cmdln.c_str(), NULL, NULL, TRUE, 0, NULL, wdir.c_str(), &si, &pi)) {
			//�I���҂�
			if (contains_wd_i(opt, _T("W|O|L"))) {
				bool exited = false;

				//�R���\�[���o�͂̎�荞��
				if (::WaitForInputIdle(pi.hProcess, 0)==0xffffffff && hRead && hWrite) {
					bool l_sw = contains_s(opt, _T('L'));
					if (l_sw) AddLogCr();

					std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
					UnicodeString log_buf;
					DWORD len;
					while (::WaitForSingleObject(pi.hProcess, WAIT_INTERVAL)==WAIT_TIMEOUT) {
						if (::PeekNamedPipe(hRead, NULL, 0, NULL, &len, NULL) && len>0) {
							std::unique_ptr<char[]> buf(new char[len + 4]);
							if (::ReadFile(hRead, buf.get(), len, &len, NULL)) {
								ms->Write(buf.get(), len);
								//���O�ɏo��
								if (l_sw) {
									std::unique_ptr<TMemoryStream> mbuf(new TMemoryStream());
									mbuf->Write(buf.get(), len);
									log_buf += get_MemoryStrins(mbuf.get());
									int p = pos_r(_T("\r\n"), log_buf);
									if (p>0) {
										AddLog(log_buf.SubString(1, p + 1), false, true);
										log_buf.Delete(1, p + 1);
										Application->ProcessMessages();
									}
								}
							}
						}
					}
					if (::PeekNamedPipe(hRead, NULL, 0, NULL, &len, NULL) && len>0) {
						std::unique_ptr<char[]> buf(new char[len + 4]);
						if (::ReadFile(hRead, buf.get(), len, &len, NULL)) {
							ms->Write(buf.get(), len);
							//���O�ɏo��
							if (l_sw) {
								std::unique_ptr<TMemoryStream> mbuf(new TMemoryStream());
								mbuf->Write(buf.get(), len);
								log_buf += get_MemoryStrins(mbuf.get());
								AddLog(log_buf, false, true);
								Application->ProcessMessages();
							}
						}
					}

					//�o�͓��e��ݒ�
					if (ms->Size>0 && o_lst) o_lst->Text = get_MemoryStrins(ms.get());
					exited = true;
				}
				//�I���҂�
				else if (contains_s(opt, _T('W'))) {
					while (::WaitForSingleObject(pi.hProcess, 50)==WAIT_TIMEOUT) Sleep(MIN_INTERVAL);
					exited = true;
				}

				//�I����Ԃ��擾
				if (exited && exit_code) {
					if (!::GetExitCodeProcess(pi.hProcess, exit_code)) SysErrAbort(GetLastError());
				}
			}
			::CloseHandle(pi.hThread);
			::CloseHandle(pi.hProcess);
			if (hRead)  ::CloseHandle(hRead);
			if (hWrite) ::CloseHandle(hWrite);
		}
		//�N�����s
		else {
			if (hRead)  ::CloseHandle(hRead);
			if (hWrite) ::CloseHandle(hWrite);
			SysErrAbort(GetLastError());
		}
		return true;
	}
	catch (EAbort &e) {
		GlobalErrMsg = e.Message;
		return false;
	}
}

//---------------------------------------------------------------------------
//���s/�ҋ@���̃^�X�N�����擾
//---------------------------------------------------------------------------
int get_BusyTaskCount()
{
	int busy_cnt = 0;
	//���s���^�X�N
	for (int i=0; i<MAX_TASK_THREAD; i++) {
		TTaskThread *tp = TaskThread[i];
		if (tp && !tp->TaskCancel) busy_cnt++;
	}
	//�\��
	busy_cnt += TaskReserveList->Count;
	return busy_cnt;
}

//---------------------------------------------------------------------------
//�������s���܂ލő�^�X�N�����擾
//---------------------------------------------------------------------------
int get_MaxTaskCount()
{
	int max_cnt = 0;
	for (int i=0; i<MAX_TASK_THREAD; i++) {
		TTaskThread *tp = TaskThread[i];
		if (tp && !tp->TaskCancel) max_cnt++;
	}

	return std::max(max_cnt, MaxTasks);
}

//---------------------------------------------------------------------------
//�󂫃^�X�N�����邩?
//---------------------------------------------------------------------------
bool has_EmptyTask(
	bool ex_sw)	//�ő�^�X�N���𒴂��Ď擾
{
	bool res = false;
	int maxn = ex_sw? std::min((MaxTasks + 2), MAX_TASK_THREAD): MaxTasks;
	for (int i=0; i<maxn && !res ; i++) if (!TaskThread[i]) res = true;
	return res;
}

//---------------------------------------------------------------------------
//�^�X�N�X���b�h���擾
//�󂫂��ȂȂ�������ۗ����̏ꍇ�͗\��
//---------------------------------------------------------------------------
TTaskThread * CreTaskThread(
	TaskConfig **cp,
	bool reserve,		//�\�񂷂�					(default = true)
	bool force,			//�ۗ����ł������擾		(default = false)
	bool ext_sw)		//�ő�^�X�N���𒴂��Ď擾	(default = false)
{
	TTaskThread *tp = NULL;
	if (force || !RsvSuspended) {
		int maxn = ext_sw? std::min(MaxTasks + 2, MAX_TASK_THREAD) : MaxTasks;
		for (int i=0; i<maxn; i++) {
			if (!TaskThread[i]) {
				tp = new TTaskThread(true);
				tp->Tag = i;
				TaskThread[i]  = tp;
				tp->Start();
				break;
			}
		}
	}

	*cp = tp? tp->Config : reserve? new TaskConfig() : NULL;
	return tp;
}

//---------------------------------------------------------------------------
//�^�X�N�̊J�n�܂��͗\��
//---------------------------------------------------------------------------
void ActivateTask(TTaskThread *tp, TaskConfig *cp)
{
	UnicodeString msg;
	if (tp) {
		tp->TaskStart();
		if (cp) StartLog(msg.sprintf(_T("%s�J�n  %s"), cp->CmdStr.c_str(), cp->InfStr.c_str()), tp->Tag);
	}
	else if (cp) {
		TaskReserveList->Add(cp);
		StartLog(msg.sprintf(_T("%s�\��  %s"), cp->CmdStr.c_str(), cp->InfStr.c_str()));
	}
}

//---------------------------------------------------------------------------
//�\��ۗ����ڂ̎��s
//---------------------------------------------------------------------------
void StartReserve()
{
	while (TaskReserveList->Count>0 && has_EmptyTask()) {
		TaskConfig  *cp = NULL;
		TTaskThread *tp = CreTaskThread(&cp, false);
		if (!tp || !cp) break;
		TaskConfig *rp = TaskReserveList->Items[0];
		cp->Assign(rp);
		delete rp;
		TaskReserveList->Delete(0);
		ActivateTask(tp, cp);
	}
}
//---------------------------------------------------------------------------
bool StartReserve(
	int idx,
	bool ext_sw)	//�ő�^�X�N���𒴂��ĊJ�n	(default = false)
{
	bool ok = false;
	if (idx>=0 && idx<TaskReserveList->Count && has_EmptyTask(ext_sw)) {
		TaskConfig  *cp = NULL;
		TTaskThread *tp = CreTaskThread(&cp, false, true, ext_sw);
		if (tp && cp) {
			TaskConfig *rp = TaskReserveList->Items[idx];
			cp->Assign(rp);
			delete rp;
			TaskReserveList->Delete(idx);
			ActivateTask(tp, cp);
			ok = true;
		}
	}
	return ok;
}

//---------------------------------------------------------------------------
//�^�X�N�ɒᑬ���s��v��
//---------------------------------------------------------------------------
void RequestSlowTask()
{
	for (int i=0; i<MAX_TASK_THREAD; i++) {
		TTaskThread *tp = TaskThread[i];
		if (tp) tp->ReqTaskSlow = true;
	}
}

//---------------------------------------------------------------------------
//���O�p�ɍŐV�̃G���[���b�Z�[�W���擾
//---------------------------------------------------------------------------
UnicodeString get_LogErrMsg(
	UnicodeString msg,	//���b�Z�[�W (default = EmptyStr)
	bool add_cr)		//���s��}�� (default = true)
{
	UnicodeString ret_str;

	if (LogErrMsg) {
		if (!msg.IsEmpty())
			ret_str = msg;
		else {
			int msg_id = GetLastError();
			if (msg_id!=NO_ERROR) ret_str = SysErrorMessage(msg_id);
			remove_top_text(ret_str, _T("%1 ��"));
		}

		if (add_cr && !ret_str.IsEmpty() && !USTARTS_TS("\r\n", ret_str))
			ret_str.Insert("\r\n    ", 1);
	}

	return ret_str;
}
//---------------------------------------------------------------------------
void set_LogErrMsg(
	UnicodeString &msg,	//���b�Z�[�W
	UnicodeString s)	//�ǉ����b�Z�[�W (default = EmptyStr)
{
	if (USTARTS_TI("Abort", s)) s = EmptyStr;
	if (!msg.IsEmpty()) msg[1] = contains_s(s, _T("���f���܂���"))? 'C' : 'E';
	msg += get_LogErrMsg(s);
}

//---------------------------------------------------------------------------
//�O���b�h�⃊�X�g�ɕ������`�悷��ۂ̏㕔�}�[�W�����擾
//---------------------------------------------------------------------------
int get_TopMargin(TCanvas *cv)
{
	return has_Leading(cv)? 0 : ScaledInt(2);
}
//---------------------------------------------------------------------------
int get_TopMargin2(TCanvas *cv)
{
	return has_Leading(cv)? 1 : std::max(ListInterLn/2, ScaledInt(2));
}

//---------------------------------------------------------------------------
//���v�p��������擾
//---------------------------------------------------------------------------
UnicodeString GetClockStr()
{
	UnicodeString ret_str;
	if (!SttClockFmt.IsEmpty()) {
		ret_str = SttClockFmt;
		//�o�b�e���[
		UnicodeString lbuf, s, tmp;
		ret_str = ReplaceStr(ret_str, "$BP", tmp.sprintf(_T("\"%s%\""), get_BatteryPercentStr().c_str()));
		ret_str = ReplaceStr(ret_str, "$BT", tmp.sprintf(_T("\"%s\""),  get_BatteryTimeStr().c_str()));
		//�d�����
		if (contains_s(ret_str, _T("$PS("))) {
			lbuf = split_tkn(ret_str, _T("$PS(")).UCAT_T("\"");
			s    = split_tkn(ret_str, ')');
			SYSTEM_POWER_STATUS ps;
			::GetSystemPowerStatus(&ps);
			lbuf += ((ps.ACLineStatus==1)? get_tkn(s, ':') : get_tkn_r(s, ':')).UCAT_T("\"");
			ret_str.Insert(lbuf, 1);
		}
		//�ڑ����
		if (contains_s(ret_str, _T("$NS("))) {
			lbuf = split_tkn(ret_str, _T("$NS(")).UCAT_T("\"");
			s    = split_tkn(ret_str, ')');
			lbuf += (InternetConnected()? get_tkn(s, ':') : get_tkn_r(s, ':')).UCAT_T("\"");
			ret_str.Insert(lbuf, 1);
		}

		//���t�J�E���g�_�E��
		while (contains_s(ret_str, _T("$CD("))) {
			lbuf = split_tkn(ret_str, _T("$CD(")).UCAT_T("\"");
			s    = split_tkn(ret_str, ')');
			if (s.Length()<=5) s = FormatDateTime("yyyy/", Now()) + s;
			TDateTime dt;
			if (str_to_DateTime(split_tkn(s, ';'), &dt)) {
				if (s.IsEmpty()) s = format_Date(dt);
				int n = dt - Today();
				if (n==0)
					s.UCAT_T("����");
				else if (n>0)
					s.cat_sprintf(_T("�܂�%s��"), get_size_str_B(n, 0).c_str());
				else
					s.cat_sprintf(_T("����%s��"), get_size_str_B(-n, 0).c_str());
			}
			lbuf += s.UCAT_T("\"");
			ret_str.Insert(lbuf, 1);
		}

		//���� (���p�����̕�����͕ϊ����ꂸ�A���p�����O���Ă��̂܂ܕ\��)
		ret_str = format_DateTimeEx(ret_str, Now());	//$EN �ŉp���
	}
	return ret_str;
}

//---------------------------------------------------------------------------
// �R�}���h�t�@�C���̉������I�v�V�����ݒ�ɔ��f
//---------------------------------------------------------------------------
void RenameOptCmdFile()
{
	while (RenCmdFileList->Count > 0) {
		UnicodeString lbuf	  = RenCmdFileList->Strings[0];
		UnicodeString org_nam = split_pre_tab(lbuf);
		UnicodeString new_nam = lbuf;
		UnicodeString new_rel = to_relative_name(lbuf);
		if (!org_nam.IsEmpty() && !new_rel.IsEmpty()) {
			//�C�x���g
			for (int i=0; i<MAX_EVENT_CMD; i++) {
				if (SameText(org_nam, rel_to_absdir(get_tkn_r(*(EventCmdList[i].sp), '@'))))
					*(EventCmdList[i].sp) = "@" + new_rel;
			}
			//���X�g
			int idx = CmdFileList->IndexOf(org_nam);
			if (idx!=-1) CmdFileList->Strings[idx] = new_nam;

			//�ǉ����j���[
			for (int i=0; i<ExtMenuList->Count; i++) {
				TStringDynArray itm_buf = get_csv_array(ExtMenuList->Strings[i], EXTMENU_CSVITMCNT, true);
				if (is_separator(itm_buf[0]) || !USAME_TI(itm_buf[1], "ExeCommands")) continue;
				UnicodeString prm = itm_buf[2];
				if (!remove_top_AT(prm)) continue;
				if (is_same_file(org_nam, prm)) {
					itm_buf[2] = "@" + new_rel;
					ExtMenuList->Strings[i] = make_csv_rec_str(itm_buf);
				}
			}
			//�L�[�ݒ�
			for (int i=0; i<KeyFuncList->Count; i++) {
				UnicodeString vbuf = KeyFuncList->ValueFromIndex[i];
				if (!USTARTS_TI("ExeCommands_", vbuf)) continue;
				UnicodeString vstr = split_tkn(vbuf, '_').UCAT_T("_");
				UnicodeString dsc  = split_dsc(vbuf);
				if (!dsc.IsEmpty()) vstr.cat_sprintf(_T(":%s:"), dsc.c_str());
				if (!remove_top_AT(vbuf)) continue;
				if (is_same_file(org_nam, vbuf)) KeyFuncList->ValueFromIndex[i] = vstr.cat_sprintf(_T("@%s"), new_rel.c_str());
			}
			//�֘A�t��(OpenByApp)
			for (int i=0; i<AssociateList->Count; i++) {
				UnicodeString vbuf = exclude_quot(AssociateList->ValueFromIndex[i]);
				if (!remove_top_AT(vbuf)) continue;
				if (is_same_file(org_nam, vbuf)) AssociateList->ValueFromIndex[i] = "@" + new_rel;
			}
			//�֘A�t��(OpenStandard)
			for (int i=0; i<OpenStdCmdList->Count; i++) {
				UnicodeString vbuf = OpenStdCmdList->ValueFromIndex[i];
				if (!remove_top_text(vbuf, _T("ExeCommands_@"))) continue;
				if (is_same_file(org_nam, vbuf)) OpenStdCmdList->ValueFromIndex[i] = "ExeCommands_@" + new_rel;
			}
		}
		RenCmdFileList->Delete(0);
	}
}

//---------------------------------------------------------------------------
//�R�}���h�t�@�C���̎Q�Ə����擾
//---------------------------------------------------------------------------
UnicodeString get_ref_CmdFile(UnicodeString fnam, TStringList *lst)
{
	UnicodeString ref_str = "���Q�ƌ���";
	//�L�[�ݒ�
	int K_cnt = 0;
	for (int i=0; i<KeyFuncList->Count; i++) {
		UnicodeString vbuf = KeyFuncList->ValueFromIndex[i];
		if (!remove_top_text(vbuf, _T("ExeCommands_")))	continue;
		UnicodeString dsc = split_dsc(vbuf);
		if (!remove_top_AT(vbuf)) 						continue;
		if (!is_same_file(fnam, exclude_quot(vbuf)))	continue;
		K_cnt++;
		if (lst) {
			add_PropLine(ref_str, "�L�[", lst);
			UnicodeString tmp = make_PropLine(_T("�ݒ�"), KeyFuncList->Names[i]);
			if (!dsc.IsEmpty()) tmp.cat_sprintf(_T(" : %s"), dsc.c_str());
			lst->Add(tmp);
		}
	}
	//�ǉ����j���[
	int M_cnt = 0;
	for (int i=0; i<ExtMenuList->Count; i++) {
		TStringDynArray itm_buf = get_csv_array(ExtMenuList->Strings[i], EXTMENU_CSVITMCNT, true);
		if (is_separator(itm_buf[0]) || !USAME_TI(itm_buf[1], "ExeCommands")) continue;
		UnicodeString prm = itm_buf[2];
		if (!remove_top_AT(prm)) 					continue;
		if (!is_same_file(fnam, exclude_quot(prm))) continue;
		M_cnt++;
		if (lst) {
			lst->Add(make_PropLine(ref_str, "�ǉ����j���[") + (equal_0(itm_buf[4])? " (��\��)" : ""));
			add_PropLine(_T("�^�C�g��"), itm_buf[0], lst);
			UnicodeString alias = itm_buf[3];
			if (!alias.IsEmpty()) {
				add_PropLine(_T("�G�C���A�X"), alias, lst);
				add_PropLine_if(_T("�L�["), get_Alias_KeyStr(alias), lst);
			}
		}
	}
	//�c�[���{�^��
	int B_cnt = 0;
	for (int ti=0; ti<3; ti++) {
		TStringList *tblst = (ti==1)? ToolBtnListV : (ti==2)? ToolBtnListI : ToolBtnList;
		for (int i=0; i<tblst->Count; i++) {
			UnicodeString vbuf = get_csv_item(tblst->Strings[i], 1);
			if (!remove_top_AT(vbuf))		continue;
			if (!is_same_file(fnam, vbuf))	continue;
			B_cnt++;
			if (lst) lst->Add(make_PropLine(ref_str, "�c�[���{�^��(") + ((ti==1)? "TV)" : (ti==2)? "IV)" : "FL)"));
		}
	}

	//�֘A�t��(OpenByApp)
	int A_cnt = 0;
	for (int i=0; i<AssociateList->Count; i++) {
		UnicodeString vbuf = AssociateList->ValueFromIndex[i];
		vbuf = exclude_quot(vbuf);
		if (!remove_top_AT(vbuf))		continue;
		if (!is_same_file(fnam, vbuf))	continue;
		A_cnt++;
		if (lst) add_PropLine(ref_str, "�֘A�t��(OpenByApp): " + AssociateList->Names[i], lst);
	}
	//�֘A�t��(OpenStandard)
	int S_cnt = 0;
	for (int i=0; i<OpenStdCmdList->Count; i++) {
		UnicodeString vbuf = OpenStdCmdList->ValueFromIndex[i];
		if (!remove_top_text(vbuf, _T("ExeCommands_@"))) continue;
		if (!is_same_file(fnam, vbuf))	continue;
		S_cnt++;
		if (lst) add_PropLine(ref_str, "�֘A�t��(OpenStandard): " + OpenStdCmdList->Names[i], lst);
	}
	//�C�x���g
	int E_cnt = 0;
	for (int i=0; i<MAX_EVENT_CMD; i++) {
		if (!is_same_file(fnam, get_tkn_r(*(EventCmdList[i].sp), '@'))) continue;
		E_cnt++;
		if (lst) {
			UnicodeString tmp = EventCmdList[i].dsc;
			remove_top_s(tmp, '|');
			add_PropLine(ref_str, "�C�x���g: " + tmp, lst);
		}
	}
	for (int i=0; i<MAX_TIMER_EVENT; i++) {
		UnicodeString vbuf = OnTimerEvent[i];
		if (!remove_top_AT(vbuf))		continue;
		if (!is_same_file(fnam, vbuf))	continue;
		E_cnt++;
		if (lst) {
			add_PropLine(ref_str, "�^�C�}�[�C�x���g", lst);
			add_PropLine(_T("���s����"), Timer_Condition[i], lst);
		}
	}

	UnicodeString ret_str;
	if (K_cnt>0) ret_str.UCAT_T("K");
	if (M_cnt>0) ret_str.UCAT_T("M");
	if (B_cnt>0) ret_str.UCAT_T("B");
	if (A_cnt>0) ret_str.UCAT_T("A");
	if (S_cnt>0) ret_str.UCAT_T("S");
	if (E_cnt>0) ret_str.UCAT_T("E");

	//���s��
	if (lst) {
		int idx = CmdFileList->IndexOf(fnam);
		if (idx!=-1) {
			cmdf_rec *cp = (cmdf_rec*)CmdFileList->Objects[idx];
			add_PropLine(_T("���s��"), cp->exe_count, lst);
		}
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//�G�C���A�X���Q�Ƃ��Ă���L�[���擾(�J���}��؂蕶����)
//---------------------------------------------------------------------------
UnicodeString get_Alias_KeyStr(UnicodeString alias, TStringList *k_lst)
{
	UnicodeString ret_str;
	if (!k_lst) k_lst = KeyFuncList;
	for (int j=0; j<k_lst->Count; j++) {
		UnicodeString vbuf = k_lst->ValueFromIndex[j];
		if (!remove_top_Dollar(vbuf)) continue;
		if (SameText(alias, vbuf)) {
			if (!ret_str.IsEmpty()) ret_str.UCAT_T(", ");
			ret_str += k_lst->Names[j];
		}
	}
	return ret_str;
}

//---------------------------------------------------------------------------
//��������p�����O�����ăp�����[�^�𒊏o
//---------------------------------------------------------------------------
UnicodeString extract_ExeParam(UnicodeString prm, UnicodeString *dsc)
{
	//�������擾/�폜
	UnicodeString dsc_str = split_dsc(prm);
	if (dsc) *dsc = dsc_str;

	//���p�����O��
	return exclude_quot(prm);
}

//---------------------------------------------------------------------------
//�}�X�N�ݒ�p��������쐬
//---------------------------------------------------------------------------
UnicodeString make_PathMask(UnicodeString mask, UnicodeString def_desc)
{
	UnicodeString ret_str;
	UnicodeString desc = split_dsc(mask);
	if (desc.IsEmpty()) desc = def_desc;

	if (!mask.IsEmpty()) {
		if (desc.IsEmpty()) desc = mask;
		ret_str.sprintf(_T(",%s,%s"), make_csv_str(desc).c_str(), make_csv_str(mask).c_str());
	}
	return ret_str;
}

//---------------------------------------------------------------------------
//���O�̃w�b�_���쐬
//---------------------------------------------------------------------------
UnicodeString make_LogHdr(
	UnicodeString cmd,		//�R�}���h���ʕ�����
	UnicodeString fnam,		//���O
	bool is_dir,			//fnam �� �f�B���N�g���� (default = false)
	int wd)					//�\���� (default = 0)
{
	bool full  = LogFullPath && !USAME_TI(cmd, "LOAD");

	bool slash = starts_t_s('/', fnam);
	if (slash) fnam = slash_to_yen(fnam);

	UnicodeString lnam = is_dir ? ("[" +(full? ExcludeTrailingPathDelimiter(fnam) : get_dir_name(fnam)) + "]")
								: (full? fnam : ExtractFileName(fnam));

	if (slash) lnam = yen_to_slash(lnam);

	UnicodeString msg; msg.sprintf(_T("  %6s "), cmd.c_str());
	if (wd>0) msg.cat_sprintf(_T("%-*s"), wd, lnam.c_str()); else msg += lnam;
	return msg;
}
//---------------------------------------------------------------------------
UnicodeString make_LogHdr(const _TCHAR *cmd, UnicodeString fnam, bool is_dir, int wd)
{
	return make_LogHdr(UnicodeString(cmd), fnam, is_dir, wd);
}
//---------------------------------------------------------------------------
UnicodeString make_LogHdr(const _TCHAR *cmd, file_rec *fp)
{
	UnicodeString fnam = fp->f_name;
	if (fp->is_ftp) fnam = yen_to_slash(fnam);
	return make_LogHdr(UnicodeString(cmd), fnam);
}

//---------------------------------------------------------------------------
UnicodeString make_CreateLog(UnicodeString dnam)
{
	return make_LogHdr(_T("CREATE")).cat_sprintf(_T("[%s]"), dnam.c_str());
}
//---------------------------------------------------------------------------
UnicodeString make_RenameLog(UnicodeString o_nam, UnicodeString n_nam)
{
	return make_LogHdr(_T("RENAME"), o_nam).cat_sprintf(_T(" ---> %s"), ExtractFileName(n_nam).c_str());
}

//---------------------------------------------------------------------------
//���O�ɉ�������t��
//---------------------------------------------------------------------------
void set_RenameLog(UnicodeString &msg, UnicodeString fnam)
{
	if (msg.IsEmpty()) return;

	msg[1] = 'R';
	msg.cat_sprintf(_T(" ---> %s"), ExtractFileName(fnam).c_str());
}

//---------------------------------------------------------------------------
//���O���X�g�{�b�N�X�̍X�V
//---------------------------------------------------------------------------
void UpdateLogListBox()
{
	if (MaxLogLines>0) while (LogBufList->Count>MaxLogLines) LogBufList->Delete(0);

	TListBox *lp  = LogWndListBox;
	lp->Count	  = LogBufList->Count;
	lp->ItemIndex = lp->Count - 1;
	LogWndScrPanel->UpdateKnob();
}

//---------------------------------------------------------------------------
//���O�ɊJ�n����\��
//---------------------------------------------------------------------------
void StartLog(
	UnicodeString msg,	//���[�Z�[�W (\t �� " ---> " �ɒu��)
	int task_no)		//�^�X�N�ԍ� (default = -1)
{
	if (msg.IsEmpty())
		LogBufList->Add(EmptyStr);
	else {
		//�O����s�łȂ���΁A��s��}��
		if (LogBufList->Count>0 && !LogBufList->Strings[LogBufList->Count - 1].IsEmpty()) LogBufList->Add(EmptyStr);
		//�J�n�\��
		UnicodeString s;
		if (task_no>=0) s.sprintf(_T("%u>"), task_no + 1); else s.USET_T(">>");
		s += FormatDateTime("hh:nn:ss ", Now()) + REPLACE_TS(msg, "\t", " ---> ");
		LogBufList->Add(s);
	}

	UpdateLogListBox();
}
//---------------------------------------------------------------------------
void StartLog(const _TCHAR *msg, int task_no)
{
	StartLog(UnicodeString(msg), task_no);
}

//---------------------------------------------------------------------------
//���O�ɏI������\��
//---------------------------------------------------------------------------
void EndLog(UnicodeString msg, UnicodeString inf)
{
	if (contained_wd_i(_T("���k|��"), msg)) NotifyPrimNyan(UAPP_T(msg, "���I�����܂���"));

	AddLog(msg.cat_sprintf(_T("�I��%s"), inf.c_str()), true);
}
//---------------------------------------------------------------------------
void EndLog(const _TCHAR *msg, UnicodeString inf)
{
	EndLog(UnicodeString(msg), inf);
}
//---------------------------------------------------------------------------
void EndLog(int id, UnicodeString inf)
{
	EndLog(LoadUsrMsg(id), inf);
}

//---------------------------------------------------------------------------
//���s�G���[��\��
//---------------------------------------------------------------------------
void ExeErrLog(UnicodeString fnam, UnicodeString msg)
{
	UnicodeString lbuf = make_LogHdr(_T("EXEC"), fnam); lbuf[1] = 'E';
	AddLog(lbuf + get_LogErrMsg(msg));
}

//---------------------------------------------------------------------------
//���O���o��
//---------------------------------------------------------------------------
void AddLog(
	UnicodeString msg,	//���b�Z�[�W
	bool with_t,		//�^�C���\��   (default = false)
	bool raw)			//���̂܂܏o�� (default = false)
{
	if (!Initialized) return;

	UnicodeString s;
	if (!msg.IsEmpty()) {
		if (!raw)   s.USET_T(" >");
		if (with_t) s += FormatDateTime("hh:nn:ss ", Now());
		s += msg;
	}
	else s.USET_T("\r\n");

	std::unique_ptr<TStringList> log_buf(new TStringList());
	log_buf->Text = s;
	LogBufList->AddStrings(log_buf.get());
	UpdateLogListBox();
}
//---------------------------------------------------------------------------
void AddLog(const _TCHAR *msg, bool with_t)
{
	AddLog(UnicodeString(msg), with_t);
}
//---------------------------------------------------------------------------
void AddLogStrings(TStringList *lst)
{
	LogBufList->AddStrings(lst);
	UpdateLogListBox();
}
//---------------------------------------------------------------------------
void AddLogCr()
{
	TListBox *lp = LogWndListBox;
	if (lp->Count>0 && !lp->Items->Strings[lp->Count - 1].IsEmpty()) {
		LogBufList->Add(EmptyStr);
		UpdateLogListBox();
	}
}
//---------------------------------------------------------------------------
void AddErr_Highlight()
{
	UnicodeString msg = make_LogHdr(_T("LOAD"), ExtractFileName(UserHighlight->FileName));
	if (UserHighlight->ErrorList->Count>0) {
		AddLogCr();
		msg[1] = 'E';
		AddLog(msg);
		for (int i=0; i<UserHighlight->ErrorList->Count; i++)
			AddLog(UserHighlight->ErrorList->Strings[i]);
		UserHighlight->ErrorList->Clear();
	}
	else
		AddLog(msg);
}
//---------------------------------------------------------------------------
void AddDebugLog(UnicodeString msg, UnicodeString info, UnicodeString info2)
{
	if (!LogDebugInf) return;

	msg = "       ! " + msg;
	if (!info.IsEmpty()) msg.cat_sprintf(_T(" %s"), info.c_str());
	AddLog(msg);

	if (!info2.IsEmpty()) {
		std::unique_ptr<TStringList> lst(new TStringList());
		lst->Text = info2;
		for (int i=0; i<lst->Count; i++) AddLog("       ! " + lst->Strings[i]);
	}
}

//---------------------------------------------------------------------------
//LogBufStr �̓��e����ʂɏo��
//---------------------------------------------------------------------------
void FlushLog()
{
	LogRWLock->BeginRead();
		UnicodeString msg = LogBufStr;
	LogRWLock->EndRead();

	if (!msg.IsEmpty()) {
		LogRWLock->BeginWrite();
			LogBufStr.Delete(1, msg.Length());
		LogRWLock->EndWrite();

		std::unique_ptr<TStringList> log_buf(new TStringList());
		log_buf->Text = msg;
		AddLogStrings(log_buf.get());
	}
}

//---------------------------------------------------------------------------
//�`��̈ꎞ��~
// ���d�ݒ�ɑΉ�
//---------------------------------------------------------------------------
void set_RedrawOff(TWinControl *cp)
{
	int idx = RedrawList->IndexOfObject((TObject*)cp);
	if (idx==-1) {
		RedrawList->AddObject("1", (TObject*)cp);
		cp->Perform(WM_SETREDRAW, 0, 0);
	}
	else {
		int n = RedrawList->Strings[idx].ToIntDef(0);
		RedrawList->Strings[idx] = IntToStr(n + 1);
	}
}

//---------------------------------------------------------------------------
//�`���~�̉���
// ���ׂĉ������ꂽ�Ƃ��ɕ`��ĊJ
//---------------------------------------------------------------------------
void set_RedrawOn(TWinControl *cp)
{
	bool flag = false;
	int idx = RedrawList->IndexOfObject((TObject*)cp);
	if (idx!=-1) {
		//1�i�K����
		int n = RedrawList->Strings[idx].ToIntDef(0);
		if (n>0) n--;
		if (n==0) {
			//���ׂĉ������ꂽ
			RedrawList->Delete(idx);
			flag = true;
		}
		else RedrawList->Strings[idx] = IntToStr(n);
	}
	else flag = true;

	//�`��ĊJ
	if (flag) {
		cp->Perform(WM_SETREDRAW, 1, 0);
		::InvalidateRect(cp->Handle, NULL, TRUE);
	}
}

//---------------------------------------------------------------------------
//�t�H�[���ɑ΂������ȃL�[����
//---------------------------------------------------------------------------
bool __fastcall SpecialKeyProc(
	TForm *frm,
	WORD &Key,
	TShiftState Shift,
	const _TCHAR *topic)	//�w���v�g�s�b�N (default = NULL)
{
	UnicodeString KeyStr   = get_KeyStr(Key);		if (KeyStr.IsEmpty()) return false;
	UnicodeString ShiftStr = get_ShiftStr(Shift);
	KeyStr = ShiftStr + KeyStr;

	//Shift+F10 �ɂ��|�b�v�A�b�v���j���[�\��
	//���R���{�{�b�N�X�Ńf�t�H���g�̃��j���[���o�Ă��܂����ۂɑΉ�
	//  OnMessage �ŏE���Ȃ��悤�Ȃ̂ł����ŏ���
	if (USAME_TI(KeyStr, "Shift+F10")) {
		if (UserModule->ShowPopupMenu()) Key = 0;
		return true;
	}

	//�w���v
	if (equal_F1(KeyStr) && topic) {
		HtmlHelpTopic(topic);
		Key = 0;
		return true;
	}

	//�L�[�ɂ��_�C�A���O�̈ړ�/�T�C�Y�ύX
	if (ShiftStr.IsEmpty()) return false;

	bool handled = true;
	//�ړ�
	if (SameText(ShiftStr, DlgMoveShift)) {
		TRect dsk_rc = Screen->DesktopRect;
		switch (Key) {
		case VK_UP:	   frm->Top  = std::max<int>(frm->Top  - DlgMovePrm, dsk_rc.Top - frm->Height + 16);	break;
		case VK_DOWN:  frm->Top  = std::min<int>(frm->Top  + DlgMovePrm, dsk_rc.Bottom - 16);				break;
		case VK_LEFT:  frm->Left = std::max<int>(frm->Left - DlgMovePrm, dsk_rc.Left - frm->Width + 16);	break;
		case VK_RIGHT: frm->Left = std::min<int>(frm->Left + DlgMovePrm, dsk_rc.Right - 16);				break;
		default:	   handled = false;
		}
	}
	//�T�C�Y�ύX
	else if (SameText(ShiftStr, DlgSizeShift) && (frm->BorderStyle==bsSizeable || frm->BorderStyle==bsSizeToolWin)) {
		switch (Key) {
		case VK_UP:    frm->Height = frm->Height - DlgSizePrm;	break;
		case VK_DOWN:  frm->Height = frm->Height + DlgSizePrm;	break;
		case VK_LEFT:  frm->Width  = frm->Width  - DlgSizePrm;	break;
		case VK_RIGHT: frm->Width  = frm->Width  + DlgSizePrm;	break;
		default:	   handled = false;
		}
	}
	//����
	else if (USAME_TI(KeyStr, "Alt+F4"))
		frm->Close();
	else
		handled = false;

	if (handled) {
		//�R���{�{�b�N�X���h���b�v�_�E�����Ă��������
		for (int i=0; i<frm->ComponentCount; i++) {
			TComponent *cp = frm->Components[i];
			if (class_is_ComboBox(cp)) {
				if (((TComboBox*)cp)->DroppedDown)
					((TComboBox*)cp)->DroppedDown = false;
			}
		}

		Key = 0;
	}

	return handled;
}
//---------------------------------------------------------------------------
bool __fastcall SpecialKeyProc(
	TForm *frm,
	WORD &Key, TShiftState Shift,
	int hlp_cnt)	//�w���v�R���e�L�X�g
{
	UnicodeString topic;
	if (hlp_cnt>0) topic.sprintf(_T("hid00%03u.htm"), hlp_cnt);

	return SpecialKeyProc(frm, Key, Shift, topic.c_str());
}

//---------------------------------------------------------------------------
//����ȕҏW����
//  �J�[�\���ʒu�̑啶����/��������
//---------------------------------------------------------------------------
bool __fastcall SpecialEditProc(TObject *Sender, WORD &Key, TShiftState Shift)
{
	UnicodeString KeyStr = get_KeyStr(Key, Shift);

	bool handled = false;

	if (class_is_Edit(Sender) || class_is_LabeledEdit(Sender)) {
		TCustomEdit *ep = dynamic_cast<TCustomEdit *>(Sender);
		if (ep) {
			UnicodeString s = ep->Text;
			if (ep->SelLength==0 && ep->SelStart<s.Length()) {
				int p = ep->SelStart + 1;
				handled = true;
				if		(USAME_TI(KeyStr, "Ctrl+U")) s[p] = toupper(s[p]);	//�啶����
				else if (USAME_TI(KeyStr, "Ctrl+L")) s[p] = tolower(s[p]);	//��������
				else handled = false;

				if (handled) {
					ep->Text	 = s;
					ep->SelStart = p;
				}
			}
		}
	}

	if (handled) Key = 0;
	return handled;
}


//---------------------------------------------------------------------------
//�g�����̓{�b�N�X
//---------------------------------------------------------------------------
UnicodeString inputbox_ex(
	const _TCHAR *tit,		//�^�C�g��
	const _TCHAR *prm,		//�v�����v�g
	UnicodeString def,		//�f�t�H���g	(default = EmptyStr)
	bool num_only)			//�����̂ݓ���	(default = false)
{
	InputExDlg->Caption = tit;
	InputExDlg->InputEdit->EditLabel->Caption = prm? prm : null_TCHAR;
	InputExDlg->InputEdit->NumbersOnly = num_only;
	InputExDlg->InputEdit->Text = def;
	return ((InputExDlg->ShowModal()==mrOk)? InputExDlg->InputEdit->Text : EmptyStr);
}
//---------------------------------------------------------------------------
int inputbox_ex_n(
	const _TCHAR *tit,
	const _TCHAR *prm,
	UnicodeString def_s,	//�f�t�H���g������	(default = EmptyStr)
	int def_n)				//�f�t�H���g���l	(default = 0)
{
	return inputbox_ex(tit, prm, def_s, true).ToIntDef(def_n);
}
//---------------------------------------------------------------------------
bool input_query_ex(
	const _TCHAR *tit,		//�^�C�g��
	const _TCHAR *prm,		//�v�����v�g
	UnicodeString *value,
	int width,				//�\�����w��	(default = 0 : �Ȃ�)
	bool num_only,			//�����̂ݓ���	(default = false)
	UnicodeString hint)		//�q���g		(default = EmptyStr)
{
	InputExDlg->Caption = tit;
	InputExDlg->InputEdit->EditLabel->Caption = prm;
	InputExDlg->CustomWidth = width;
	InputExDlg->InputEdit->NumbersOnly = num_only;
	InputExDlg->InputEdit->Hint = hint;
	InputExDlg->InputEdit->Text = *value;
	bool res = (InputExDlg->ShowModal()==mrOk);
	if (res) *value = InputExDlg->InputEdit->Text;
	return res;
}
//---------------------------------------------------------------------------
bool input_query_ex(
	int tid,				//���b�Z�[�WID
	const _TCHAR *prm,
	UnicodeString *value,
	int width,
	bool num_only,
	UnicodeString hint)
{
	return input_query_ex(LoadUsrMsg(tid).c_str(), prm, value, width, num_only, hint);
}

//---------------------------------------------------------------------------
//�f�B���N�g�����̓{�b�N�X
//---------------------------------------------------------------------------
UnicodeString inputbox_dir(const _TCHAR *tit, const _TCHAR *cmd)
{
	InpDirDlg->Caption	  = tit;
	InpDirDlg->CommandStr = cmd;
	return ((InpDirDlg->ShowModal()==mrOk)? InpDirDlg->InpDirComboBox->Text : EmptyStr);
}

//---------------------------------------------------------------------------
//0�`9�L�[��?
//---------------------------------------------------------------------------
bool is_Num0to9(UnicodeString keystr)
{
	return (keystr.Length()==1 && CompareStr(keystr, "0")>=0 && CompareStr(keystr, "9")<=0);
}

//---------------------------------------------------------------------------
//�������T�[�`�L�[��?
//  keystr �ɐ��K�\���p�^�[����Ԃ�
//---------------------------------------------------------------------------
bool is_IniSeaKey(UnicodeString &keystr)
{
	if (keystr.IsEmpty()) return false;
	UnicodeString k; k.sprintf(_T("F:%s"), keystr.c_str());
	if (!IniSeaShift.IsEmpty() && !remove_top_text(k, IniSeaShift)) return false;
	if (k.Length()!=1) return false;

	if (_istalpha(k[1]) || USAME_TS(k, "�_") || ((IniSeaByNum || IniSeaBySign) && _istdigit(k[1]))) {
		k = REPLACE_TS(k, "�_", "_");
		if (IniSeaBySign) {
			//Shift+�����L�[�̋L�����T�[�`
			std::unique_ptr<TStringList> r_lst(new TStringList());
			r_lst->Text = (::GetKeyboardType(0)==7)?
				"1\t[1!]\n3\t[3#]\n4\t[4\\$]\n5\t[5%]\n6\t[6&]\n7\t[7']\n8\t[8\\(]\n9\t[9\\)]\n" :				//JP
				"0\t[0\\)]\n1\t[1!]\n2\t[2@]\n3\t[3#]\n4\t[4\\$]\n5\t[5%]\n6\t[6\\^]\n7\t[7&]\n9\t[9\\(]\n";	//US
			k = replace_str_by_list(k, r_lst.get());
		}
		keystr.sprintf(_T("^%s"), k.c_str());
		return chk_RegExPtn(keystr);
	}
	else
		return false;
}

//---------------------------------------------------------------------------
//�C���N�������^���T�[�`�̃L�[���[�h���X�V
//  ��ʃ��[�h�ɂ���Đ���������؂�ւ��AJP/US�L�[�{�[�h�Ή�
//---------------------------------------------------------------------------
bool update_IncSeaWord(
	UnicodeString &kwd,		//[i/o] �L�[���[�h
	UnicodeString keystr,	//[i]   ���̓L�[
	bool fl_sw)				//[i]   true = �t�@�C�����Ɏg���Ȃ��������l��
{
	bool is_char = false;
	bool is_cap  = false;

	remove_text(keystr, _T("10Key_"));
	keystr = REPLACE_TS(keystr, "��", "=");

	if (remove_top_text(keystr, KeyStr_Shift)) {
		if (keystr.Length()==1) {
			//Shift+�p��
			if (_istalpha(keystr[1])) {
				is_char = true; is_cap = true;
			}
			//Shift+����
			else if (_istdigit(keystr[1])) {
				std::unique_ptr<TStringList> r_lst(new TStringList());
				//�t�@�C����
				if (fl_sw) {
					r_lst->Text = is_JpKeybd()?
						"0\t\n1\t!\n2\t�h\n3\t#\n4\t$\n5\t%\n6\t&\n7\t'\n8\t(\n9\t)\n" : //JP
						"0\t)\n1\t!\n2\t@\n3\t#\n4\t$\n5\t%\n6\t^\n7\t&\n8\t\n9\t(\n";	 //US
				}
				//�e�L�X�g
				else {
					r_lst->Text = is_JpKeybd()?
						"0\t\n1\t!\n2\t\"\n3\t#\n4\t$\n5\t%\n6\t&\n7\t'\n8\t(\n9\t)\n" : //JP
						"0\t)\n1\t!\n2\t@\n3\t#\n4\t$\n5\t%\n6\t^\n7\t&\n8\t*\n9\t(\n";	 //US
				}
				keystr	= replace_str_by_list(keystr, r_lst.get());
				is_char = !keystr.IsEmpty();
			}
			//Shift+�L��
			else {
				std::unique_ptr<TStringList> r_lst(new TStringList());
				//�t�@�C����
				if (fl_sw) {
					r_lst->Text = is_JpKeybd()?
						"-\t=\n^\t~\n\\\t\n@\t`\n[\t{\n]\t}\n;\t+\n�_\t_\n,\t\n.\t\n/\t\n" :	//JP
						"`\t~\n-\t_\n=\t+\n[\t{\n]\t}\n;\t\n'\t\n\\\t\n,\t\n.\t\n/\t\n";		//US
				}
				//�e�L�X�g
				else {
					r_lst->Text = is_JpKeybd()?
						"-\t=\n^\t~\n\\\t|\n@\t`\n[\t{\n]\t}\n;\t+\n:\t*\n�_\t_\n,\t<\n.\t>\n/\t?\n" :	//JP
						"`\t~\n-\t_\n=\t+\n[\t{\n]\t}\n;\t:\n'\t\"\n\\\t|\n,\t<\n.\t>\n/\t?\n";	//US
				}
				keystr	= replace_str_by_list(keystr, r_lst.get());
				is_char = !keystr.IsEmpty();
			}
		}
	}
	else if (keystr.Length()==1) {
		//�p����
		if (_istalnum(keystr[1]))
			is_char = true;
		//�L��
		else {
			//�t�@�C����
			if (fl_sw)
				is_char = ContainsStr((is_JpKeybd()? "-^@[];,." : "-=[];,.`'"), keystr);
			//�e�L�X�g
			else
				is_char = ContainsStr((is_JpKeybd()? "-^\\@[];:,./" : "-=[];,.`'\\/"), keystr);
		}
	}
	else if (USAME_TI(keystr, "SPACE")) {
		keystr.USET_T(" ");
		is_char = true;
	}

	//�ꕶ�����
	if (contained_wd_i(_T("BKSP|Ctrl+H"), keystr)) {
		if (!kwd.IsEmpty()) delete_end(kwd);
		return true;
	}
	//�ꕶ���ǉ�
	if (is_char) {
		keystr = is_cap? keystr.UpperCase() : keystr.LowerCase();
		kwd += keystr;
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
//�J�[�\���L�[���R�}���h�ɕϊ�
//---------------------------------------------------------------------------
UnicodeString get_CsrKeyCmd(UnicodeString KeyStr)
{
	switch (idx_of_word_i(_T("DOWN|UP|LEFT|RIGHT"), KeyStr)) {
	case  0: return "CursorDown";
	case  1: return "CursorUp";
	case  2: return "ToLeft";
	case  3: return "ToRight";
	default: return EmptyStr;
	}
}

//---------------------------------------------------------------------------
//�P�Ƃ̒ʏ�R�}���h���H
//---------------------------------------------------------------------------
bool is_OneNrmCmd(
	UnicodeString cmds,
	bool no_prm)			//�p�����[�^���� (default = false)
{
	if (starts_AT(cmds))	 							return false;
	if (pos_r_q_colon(cmds)!=0)							return false;
	if (CommandList->IndexOfName(get_CmdStr(cmds))==-1)	return false;
	if (no_prm && !get_PrmStr(cmds).IsEmpty())	 		return false;
														return true;
}

//---------------------------------------------------------------------------
//�t�B���^������̈ꗗ�̃J�[�\���ړ�
//---------------------------------------------------------------------------
bool MovListBoxFromFilter(TListBox *lp, UnicodeString key_str)
{
	if		(contained_wd_i(KeysStr_CsrDown, key_str)) ListBoxCursorDown(lp);
	else if (contained_wd_i(KeysStr_CsrUp,   key_str)) ListBoxCursorUp(lp);
	else if (contained_wd_i(KeysStr_PgDown,  key_str)) ListBoxPageDown(lp);
	else if (contained_wd_i(KeysStr_PgUp,    key_str)) ListBoxPageUp(lp);
	else 											   return false;

	return true;
}
//---------------------------------------------------------------------------
bool MovGridFromFilter(TStringGrid *gp, UnicodeString key_str)
{
	if		(contained_wd_i(KeysStr_CsrDown, key_str)) GridCursorDown(gp);
	else if (contained_wd_i(KeysStr_CsrUp,   key_str)) GridCursorUp(gp);
	else if (contained_wd_i(KeysStr_PgDown,  key_str)) GridPageDown(gp);
	else if (contained_wd_i(KeysStr_PgUp,    key_str)) GridPageUp(gp);
	else											   return false;

	return true;
}

//---------------------------------------------------------------------------
//���X�g�{�b�N�X�ł̃R�}���h����
//---------------------------------------------------------------------------
bool ExeCmdListBox(TListBox *lp, UnicodeString cmd, UnicodeString prm)
{
	if (!lp || cmd.IsEmpty()) return false;

	if (prm.IsEmpty()) {
		prm = exclude_quot(get_PrmStr(cmd));
		cmd = get_CmdStr(cmd);
	}
	prm = extract_ExeParam(prm);

	int idx = lp->ItemIndex;
	int pn	= lp->ClientHeight/lp->ItemHeight;
	int mov_n;

	if (prm.IsEmpty() && USAME_TI(cmd, "Scroll")) mov_n = ViewWheelScrLn;
	else if (USAME_TI(prm, "HP")) mov_n = pn/2;
	else if (USAME_TI(prm, "FP")) mov_n = pn;
	else mov_n = prm.ToIntDef(1);

	TFont *l_font = (lp->Tag & LBTAG_GEN_LIST)? GenListFont :
					(lp->Tag & (LBTAG_OPT_FIF1 | LBTAG_OPT_FIF2))? FileInfFont : ListFont;

	//�ړ�
	int cmd_id = idx_of_word_i(
		_T("CursorDown|CursorUp|CursorTop|TextTop|CursorEnd|TextEnd|")				//0�`5
		_T("PageDown|PageUp|ScrollDown|ScrollUp|ScrollCursorDown|ScrollCursorUp"),	//6�`11
		cmd);

	if (cmd_id!=-1) {
		switch (cmd_id) {
		case 0:
			if ((lp->Tag & LBTAG_OPT_LOOP) && idx==lp->Count-1) ListBoxTop(lp); else ListBoxCursorDown(lp, mov_n);
			break;
		case 1:
			if ((lp->Tag & LBTAG_OPT_LOOP) && idx==0) ListBoxEnd(lp); else ListBoxCursorUp(lp, mov_n);
			break;
		case 2: case 3:	ListBoxTop(lp);						break;
		case 4: case 5:	ListBoxEnd(lp);						break;
		case 6:			ListBoxPageDown(lp);				break;
		case 7:			ListBoxPageUp(lp);					break;
		case 8:			ListBoxScrollDown(lp, mov_n);		break;
		case 9:			ListBoxScrollUp(lp, mov_n);			break;
		case 10:		ListBoxScrollDown(lp, mov_n, true);	break;
		case 11:		ListBoxScrollUp(lp, mov_n, true);	break;
		}
	}
	//�I��
	else if (lp->MultiSelect && USAME_TI(cmd, "Select")) {
		if (idx!=-1) {
			lp->Selected[idx] = !lp->Selected[idx];
			ListBoxCursorDown(lp);
		}
	}
	//���ׂđI��
	else if (lp->MultiSelect && (USTARTS_TI("SelAll",cmd) || USAME_TI(cmd, "SelectAll"))) {
		ListBoxSelectAll(lp, lp->SelCount==0);
	}
	//�R�s�[
	else if (USAME_TI(cmd, "ClipCopy")) {
		//�I���s�R�s�[
		if (lp->MultiSelect && lp->SelCount>0) {
			cursor_HourGlass();
			std::unique_ptr<TStringList> cb_buf(new TStringList());
			for (int i=0; i<lp->Count; i++) if (lp->Selected[i]) cb_buf->Add(lp->Items->Strings[i]);
			lp->Perform(WM_SETREDRAW, 0, 0);
			lp->ClearSelection();
			lp->Perform(WM_SETREDRAW, 1, 0);
			::InvalidateRect(lp->Handle, NULL, TRUE);
			if (USAME_TI(prm, "AD")) cb_buf->Text.Insert(Clipboard()->AsText, 1);	//�ǉ�
			cursor_Default();
			copy_to_Clipboard(cb_buf->Text);
		}
		//1�s�R�s�[
		else if (lp->Tag & LBTAG_OPT_LCPY) {
			if (idx!=-1) copy_to_Clipboard(lp->Items->Strings[idx]);
		}
	}
	//URL���J��
	else if (USAME_TI(cmd, "OpenURL")) {
		if (idx!=-1) Execute_ex(extract_URL(lp->Items->Strings[idx]));
	}
	//�Y�[��
	else if ((lp->Tag & LBTAG_OPT_ZOOM) && contained_wd_i(_T("ZoomIn|ZoomOut"), cmd)) {
		int d_sz = std::min(prm.ToIntDef(1), 12);
		lp->Font->Size = USAME_TI(cmd, "ZoomIn") ? std::min(lp->Font->Size + d_sz, MAX_FNTZOOM_SZ)
											 	 : std::max(lp->Font->Size - d_sz, MIN_FNTZOOM_SZ);
		set_ListBoxItemHi(lp);
	}
	else if ((lp->Tag & LBTAG_OPT_ZOOM) && USAME_TI(cmd, "ZoomReset")) {
		lp->Font->Assign(l_font);
		set_ListBoxItemHi(lp);
	}
	//�t�H���g�T�C�Y�ύX
	else if ((lp->Tag & LBTAG_OPT_ZOOM) && USAME_TI(cmd, "SetFontSize")) {
		bool x_sw = remove_top_s(prm, '^');
		if (!prm.IsEmpty()) {
			int f_sz = std::max(std::min(prm.ToIntDef(l_font->Size), MAX_FNTZOOM_SZ), MIN_FNTZOOM_SZ);
			lp->Font->Size = (x_sw && lp->Font->Size==f_sz)? l_font->Size : f_sz;
			set_ListBoxItemHi(lp);
		}
	}
	else return false;

	if (!lp->Focused()) lp->Invalidate();

	return true;
}
//---------------------------------------------------------------------------
bool ExeCmdListBox(TListBox *lp, const _TCHAR *cmd, UnicodeString prm)
{
	return ExeCmdListBox(lp, UnicodeString(cmd), prm);
}

//---------------------------------------------------------------------------
//�N���b�v�{�[�h�ɃR�s�[
//�R�s�[���̃q���g�\�����s��
//---------------------------------------------------------------------------
void copy_to_Clipboard(UnicodeString s)
{
	if (s.IsEmpty()) return;

	Clipboard()->AsText = s;
	::SendMessage(MainHandle, WM_NYANFI_CLPCOPIED, get_line_count(s) | 0x80000000, str_len_unicode(s));
	//WParam | 0x80000000 �Ńe�L�X�g�ł��邱�Ƃ�ʒm
}
//---------------------------------------------------------------------------
void copy_to_Clipboard(TPicture *pic)
{
	Clipboard()->Assign(pic);
	::SendMessage(MainHandle, WM_NYANFI_CLPCOPIED, pic->Width, pic->Height);
}
//---------------------------------------------------------------------------
void copy_to_Clipboard(TMetafile *mf)
{
	if (mf->Empty) return;

	Clipboard()->Assign(mf);
	::SendMessage(MainHandle, WM_NYANFI_CLPCOPIED, mf->Width, mf->Height);
}
//---------------------------------------------------------------------------
void copy_to_Clipboard(Graphics::TBitmap *bmp)
{
	if (bmp->Empty) return;

	Clipboard()->Assign(bmp);
	::SendMessage(MainHandle, WM_NYANFI_CLPCOPIED, bmp->Width, bmp->Height);
}

//---------------------------------------------------------------------------
//�ŏ��ɋN�����ꂽ NyanFi �̃n���h�����擾
//---------------------------------------------------------------------------
HWND get_PrimNyanWnd()
{
	std::unique_ptr<TStringList> lst(new TStringList());
	HWND hWnd = NULL;
	get_NyanFiList(lst.get());
	for (int i=0; i<lst->Count; i++) {
		if (!equal_1(get_csv_item(lst->Strings[i], 0))) continue;
		hWnd = (HWND)get_csv_item(lst->Strings[i], 1).ToIntDef(0);
	}
	return hWnd;
}

//---------------------------------------------------------------------------
// �E�B�C�h�E�� NyanFi �̃��C����ʂ�?
//---------------------------------------------------------------------------
bool IsNyanfiWnd(HWND hWnd)
{
	return USAME_TS(get_WndClassName(hWnd), "TNyanFiForm");
}
//---------------------------------------------------------------------------
// �E�B�C�h�E�͓�d�N�����ꂽ NyanFi �̃��C����ʂ�?
//---------------------------------------------------------------------------
bool IsNyanfi2Wnd(HWND hWnd)
{
	return (USAME_TS(get_WndClassName(hWnd), "TNyanFiForm")
			&& is_match_regex(get_WndText(hWnd), _T("NyanFi-\\d+$")));
}

//---------------------------------------------------------------------------
//�ŏ��ɋN�����ꂽ NyanFi �֕�����𑗏o
//---------------------------------------------------------------------------
bool NotifyPrimNyan(UnicodeString msg)
{
	if (IsPrimary || !ShowDuplNotify) return false;

	HWND hPrimeWnd = get_PrimNyanWnd();
	if (!hPrimeWnd) return false;

	UnicodeString lbuf;
	lbuf.sprintf(_T("NyanFi-%u\r\n�@%s�@"), NyanFiIdNo, msg.c_str());

	COPYDATASTRUCT cd;
	cd.dwData = CPYDTID_DPL_MSG;
	cd.cbData = sizeof(_TCHAR) * (lbuf.Length() + 1);
	cd.lpData = lbuf.c_str();
	::SendMessage(hPrimeWnd, WM_COPYDATA, 0, (LPARAM)&cd);
	return true;
}

//---------------------------------------------------------------------------
//���[�_���\���̃E�B���h�E���擾
//---------------------------------------------------------------------------
BOOL CALLBACK EnumModalWndProc(HWND hWnd, LPARAM hTop)
{
	DWORD pid;
	::GetWindowThreadProcessId(hWnd, &pid);
	if (ProcessId!=pid) return TRUE;

	if (::IsWindowVisible(hWnd) && USAME_TS(get_WndClassName(hWnd), "#32770")) {
		*((HWND*)hTop) = hWnd;
		return FALSE;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
HWND get_ModalWnd()
{
	HWND top_wnd = NULL;

	//�\�����̃��[�_���t�H�[����T��
	for (int i=0; i<Screen->FormCount; i++) {
		TForm *fp = Screen->Forms[i];
		if (!fp->Visible || fp==Application->MainForm || !fp->Floating || !fp->Enabled || fp->FormStyle==fsStayOnTop) continue;
		if (fp->ClassNameIs("TExTxtViewer")) continue;
		top_wnd = fp->Handle;
		break;
	}

	//�Ȃ���Ε\�����̃��b�Z�[�W�{�b�N�X��T��
	if (!top_wnd && !::IsWindowEnabled(MainHandle)) ::EnumWindows(EnumModalWndProc , (LPARAM)&top_wnd);

	return top_wnd;
}

//---------------------------------------------------------------------------
//���� NyanFi ���I��
//---------------------------------------------------------------------------
BOOL CALLBACK CloseNyanWndProc(HWND hWnd, LPARAM cnt)
{
	DWORD pid;
	::GetWindowThreadProcessId(hWnd, &pid);
	if (ProcessId!=pid && IsNyanfi2Wnd(hWnd)) {
		::PostMessage(hWnd, WM_CLOSE, 0, 0);
		*((int*)cnt) += 1;
	}
	return TRUE;
}
//---------------------------------------------------------------------------
BOOL CALLBACK CountNyanWndProc(HWND hWnd, LPARAM cnt)
{
	DWORD pid;
	::GetWindowThreadProcessId(hWnd, &pid);
	if (ProcessId!=pid && IsNyanfi2Wnd(hWnd)) *((int*)cnt) += 1;
	return TRUE;
}
//---------------------------------------------------------------------------
bool CloseOtherNyanFi()
{
	int cnt = 0;
	::EnumWindows(CloseNyanWndProc, (LPARAM)&cnt);
	if (cnt==0) return true;

	for (int i=0; i<20; i++) {	//�^�C���A�E�g10�b
		Application->ProcessMessages();
		cnt = 0;
		::EnumWindows(CountNyanWndProc, (LPARAM)&cnt);
		if (cnt==0) break;
		Sleep(500);
	}

	return (cnt==0);
}

//---------------------------------------------------------------------------
//NyanFi�̈ꗗ���擾
// �A��(1�`),�n���h��,�^�C�g��
//---------------------------------------------------------------------------
BOOL CALLBACK EnumNyanWndProc(HWND hWnd, LPARAM lst)
{
	if (IsNyanfiWnd(hWnd)) {
		UnicodeString tbuf = get_WndText(hWnd);
		int p = pos_r(_T(" - "), tbuf);
		UnicodeString lbuf = (p>0)? tbuf.SubString(p + 3, 16) : tbuf;
		if (lbuf.Pos('-')) lbuf = get_tkn_r(lbuf, '-'); else lbuf.USET_T("1");
		lbuf.cat_sprintf(_T(",%u,%s"), hWnd, tbuf.c_str());
		((TStringList*)lst)->Add(lbuf);
	}
	return TRUE;
}
//---------------------------------------------------------------------------
int get_NyanFiList(TStringList *lst)
{
	lst->Clear();
	::EnumWindows(EnumNyanWndProc, (LPARAM)lst);
	lst->Sort();
	return lst->Count;
}

//---------------------------------------------------------------------------
//�V�K�N�����̃^�C�g�����擾
//---------------------------------------------------------------------------
UnicodeString get_NewTitle(bool primary)
{
	UnicodeString tnam;

	if (!primary) {
		std::unique_ptr<TStringList> lst(new TStringList());
		if (get_NyanFiList(lst.get())>0) {
			int n = get_tkn(lst->Strings[lst->Count - 1], ',').ToIntDef(0);
			if (n>0) {
				NyanFiIdNo = n + 1;
				tnam.sprintf(_T("NyanFi-%u"), NyanFiIdNo);
			}
		}
	}

	if (tnam.IsEmpty()) {
		NyanFiIdNo = 1;
		tnam.USET_T("NyanFi");
	}

	return tnam;
}

//---------------------------------------------------------------------------
//�O���NyanFi�̃n���h�����擾
//---------------------------------------------------------------------------
HWND get_NyanFiWnd(
	int rel_idx)	//1=��/ -1=�O
{
	HWND hWnd = NULL;

	std::unique_ptr<TStringList> lst(new TStringList());
	if (get_NyanFiList(lst.get())<2) return hWnd;

	int idx = -1;
	for (int i=0; i<lst->Count && idx==-1; i++)
		if ((HWND)get_csv_item(lst->Strings[i], 1).ToIntDef(0)==MainHandle) idx = i;

	if (idx!=-1) {
		idx += rel_idx;
		while (idx<0) idx += lst->Count;
		idx %= lst->Count;
		hWnd = (HWND)get_csv_item(lst->Strings[idx], 1).ToIntDef(0);
	}

	return hWnd;
}
//---------------------------------------------------------------------------
//NyanFi���A�N�e�B�u��
// �߂�l: false=�����̃C���X�^���X���Ȃ�
//---------------------------------------------------------------------------
bool activate_NyanFi(
	int idx,	//rel? ����(1=��/ -1=�O) : ���
	bool rel)	//���Ύw��	(default = true);
{
	HWND hWnd = NULL;
	if (rel) {
		hWnd = get_NyanFiWnd(idx);	if (!hWnd) return false;
	}
	else {
		std::unique_ptr<TStringList> lst(new TStringList());
		get_NyanFiList(lst.get());
		UnicodeString n_str = idx;
		for (int i=0; i<lst->Count && !hWnd; i++) {
			if (SameStr(get_csv_item(lst->Strings[i], 0), n_str))
				hWnd = (HWND)get_csv_item(lst->Strings[i], 1).ToIntDef(0);
		}
	}
	if (!hWnd) return false;

	if (::IsIconic(hWnd)) ::SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	::SetForegroundWindow(hWnd);
	return true;
}

//---------------------------------------------------------------------------
//������NyanFi������
//---------------------------------------------------------------------------
bool IsDuplicated()
{
	std::unique_ptr<TStringList> lst(new TStringList());
	return (get_NyanFiList(lst.get())>1);
}

//---------------------------------------------------------------------------
//�ʃE�B���h�E�̃e�L�X�g�r���A�[�̃��X�g���擾
//---------------------------------------------------------------------------
int get_ExViewerList(TStringList *lst)
{
	for (int i=0; i<Screen->FormCount; i++) {
		TForm *fp = Screen->Forms[i];
		if (!fp->Visible) continue;
		if (fp->ClassNameIs("TExTxtViewer")) lst->AddObject(fp->Name, (TObject*)fp);
	}

	lst->Sort();
	return lst->Count;
}

//---------------------------------------------------------------------------
//�ʃE�B���h�E�̃e�L�X�g�r���A�[������΃t�H�[�J�X���ڂ�
//---------------------------------------------------------------------------
bool focus_ExViewer()
{
	std::unique_ptr<TStringList> lst(new TStringList());
	bool ok = (get_ExViewerList(lst.get()) > 0);
	if (ok) ((TForm *)lst->Objects[0])->SetFocus();
	return ok;
}
//---------------------------------------------------------------------------
//�ʃE�B���h�E�̂��ׂẴe�L�X�g�r���A�[�����
//---------------------------------------------------------------------------
void close_all_ExViewer(
	TForm *frm)		//���O����t�H�[��
{
	for (int i=0; i<Screen->FormCount; i++) {
		TForm *fp = Screen->Forms[i];
		if (fp->ClassNameIs("TExTxtViewer") && fp!=frm) fp->Close();
	}
}

//---------------------------------------------------------------------------
//�w���v�E�B���h�E���擾
//---------------------------------------------------------------------------
BOOL CALLBACK EnumHelpWndProc(HWND hWnd, LPARAM hHelp)
{
	DWORD pid;
	::GetWindowThreadProcessId(hWnd, &pid);
	if (ProcessId!=pid) return TRUE;
	if (!USAME_TI(get_WndClassName(hWnd), "HH Parent")) return TRUE;
	*((HWND*)hHelp) = hWnd;
	return FALSE;
}
//---------------------------------------------------------------------------
HWND get_HelpWnd()
{
	HWND hlp_wnd = NULL;
	::EnumWindows(EnumHelpWndProc, (LPARAM)&hlp_wnd);
	return hlp_wnd;
}

//---------------------------------------------------------------------------
//���j���[���擾
//---------------------------------------------------------------------------
BOOL CALLBACK EnumMenuWndProc(HWND hWnd, LPARAM hMenu)
{
	DWORD pid;
	::GetWindowThreadProcessId(hWnd, &pid);
	if (ProcessId!=pid) return TRUE;

	if (!USAME_TS(get_WndClassName(hWnd), "#32768")) return TRUE;
	*((HWND*)hMenu) = hWnd;
	return FALSE;
}
//---------------------------------------------------------------------------
HWND get_MenuWnd()
{
	HWND menu_wnd = NULL;
	::EnumWindows(EnumMenuWndProc, (LPARAM)&menu_wnd);
	return menu_wnd;
}

//---------------------------------------------------------------------------
//�w���v����
//---------------------------------------------------------------------------
void HtmlHelpTopic(
	const _TCHAR *topic)	// topic: �t�@�C����[#ID]
{
	if (!lpfHtmlHelp) return;

	UnicodeString pnam;
	pnam.sprintf(_T("%s::/%s"),
		Application->HelpFile.c_str(), (topic && _tcslen(topic))? topic : _T(HELPTOPIC_TOP));

	lpfHtmlHelp(Screen->ActiveForm->Handle, pnam.c_str(), HH_DISPLAY_TOPIC, NULL);
}
//---------------------------------------------------------------------------
void HtmlHelpContext(int idx)
{
	if (!lpfHtmlHelp) return;
	lpfHtmlHelp(Screen->ActiveForm->Handle, Application->HelpFile.c_str(), HH_HELP_CONTEXT, idx);
}
//---------------------------------------------------------------------------
void HtmlHelpKeyword(
	UnicodeString fnam,		//�w���v�t�@�C����
	UnicodeString kwd)		//�L�[���[�h
{
	if (lpfHtmlHelp) {
		HH_AKLINK ak;
		::ZeroMemory(&ak, sizeof(HH_AKLINK));
		ak.cbStruct 	= sizeof(HH_AKLINK);
		ak.pszKeywords	= kwd.c_str();
		ak.fIndexOnFail = TRUE;
		if (lpfHtmlHelp) lpfHtmlHelp(Screen->ActiveForm->Handle, fnam.c_str(), HH_KEYWORD_LOOKUP, (DWORD)&ak);
	}
}
//---------------------------------------------------------------------------
void HtmlHelpClose(
	bool only_fg)	//�t�H�A�O�����h�̏ꍇ�ɂ̂ݕ��� (default = false)
{
	if (lpfHtmlHelp) {
		HWND hWnd = get_HelpWnd();
		if (only_fg && ::GetForegroundWindow()!=hWnd) hWnd = NULL;
		if (hWnd) lpfHtmlHelp(NULL, NULL, HH_CLOSE_ALL, 0);
	}
}

//---------------------------------------------------------------------------
//2�n�_�Ԃ̋������擾 (Hubeny�̌����𗘗p)
//---------------------------------------------------------------------------
double GetDistance(double lat1, double lng1, double lat2, double lng2)
{
	double lat1_r	= lat1 * M_PI / 180;
	double lng1_r	= lng1 * M_PI / 180;
	double lat2_r	= lat2 * M_PI / 180;
	double lng2_r	= lng2 * M_PI / 180;
	double ave_r	= (lat1_r + lat2_r) / 2;
	double meridian = 6334834.0 / sqrt(pow(1.0 - 0.006674*sin(ave_r)*sin(ave_r), 3));
	double prime_v	= 6377397.0 / sqrt(1.0 - 0.006674*sin(ave_r)*sin(ave_r));
	double x = meridian * (lat1_r - lat2_r);
	double y = prime_v * cos(ave_r) * (lng1_r - lng2_r);
	return sqrt(x*x + y*y);
}

//---------------------------------------------------------------------------
//�ܓx�o�x���w�肵�� Google Maps ���J��
//---------------------------------------------------------------------------
bool OpenGoogleMaps(
	double lat,		//lat: �ܓx
	double lng,		//lng: �o�x
	UnicodeString fnam)
{
	try {
		GlobalErrMsg  = EmptyStr;

		UnicodeString lbuf;
		//�e���v���[�g
		UnicodeString tnam = ExePath + WEBMAP_TPLT;
		if (file_exists(tnam)) {
			std::unique_ptr<TStringList> fbuf(new TStringList());
			if (load_text_ex(tnam, fbuf.get())==0) UserAbort(USTR_FaildProc);
			lbuf = fbuf->Text;
		}
		//�f�t�H���g
		else {
			lbuf =
				"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\r\n"
				"<html xmlns=\"http://www.w3.org/1999/xhtml\">\r\n"
				"<head>\r\n"
				"<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>\r\n"
				"<title>$Title$</title>"
				"<script type=\"text/javascript\" src=\"http://maps.google.com/maps/api/js?sensor=false\"></script>\r\n"
				"<script type=\"text/javascript\">\r\n"
				"google.maps.event.addDomListener(window, 'load', function() {\r\n"
				"var latlng = new google.maps.LatLng($Latitude$, $Longitude$);\r\n"
				"var options = {zoom:16, center:latlng, mapTypeId:google.maps.MapTypeId.ROADMAP};\r\n"
				"var map = new google.maps.Map(document.getElementById(\"gmap\"), options);\r\n"
				"var marker = new google.maps.Marker({position:latlng, map:map});\r\n"
				"});\r\n"
				"</script>\r\n"
				"<style TYPE=\"text/css\">\r\n<!--\r\nhtml,body {height:100%;}\r\n-->\r\n</style>\r\n"
				"</head>\r\n"
				"<body>\r\n"
				"<div id=\"gmap\" style=\"width:100%; height:90%;\"></div>\r\n"
				"$FileRef$"
				"</body></html>\r\n";

			lbuf = ReplaceStr(lbuf, "$Title$",	   !fnam.IsEmpty()? "$FileName$�̒n�}" : "�w��n�_�̒n�}");
			lbuf = ReplaceStr(lbuf, "$FileRef$",   !fnam.IsEmpty()? "<p><a href=\"file:///$PathName$\">$PathName$</a></p>\r\n" : "");
		}

		lbuf = ReplaceStr(lbuf, "$Latitude$",  UnicodeString().sprintf(_T("%.8f") ,lat));
		lbuf = ReplaceStr(lbuf, "$Longitude$", UnicodeString().sprintf(_T("%.8f") ,lng));
		lbuf = ReplaceStr(lbuf, "$FileName$",  ExtractFileName(fnam));
		lbuf = ReplaceStr(lbuf, "$PathName$",  fnam);

		std::unique_ptr<TStringList> lst(new TStringList());
		lst->Text = lbuf;

		UnicodeString hnam;  hnam.sprintf(_T("%s" WEBMAP_FILE), TempPathA.c_str());
		if (!saveto_TextUTF8(hnam, lst.get())) throw EAbort(LoadUsrMsg(USTR_FaildSave, _T("�n�}�\���p�ꎞ�t�@�C��")));

		Execute_ex("file:///" + hnam);
		return true;
	}
	catch (EAbort &e) {
		GlobalErrMsg = e.Message;
		return false;
	}
}

//---------------------------------------------------------------------------
//�Í�����
//---------------------------------------------------------------------------
#define MAX_XORBIT_SIZE 128

unsigned char CipherXorPtn[MAX_XORBIT_SIZE];

//---------------------------------------------------------------------------
//�Í����̂��߂̃r�b�g�p�^�[�����쐬
//  Windows���[�U�� + �V�X�e���h���C�u�̃V���A���i���o�[���擾
//  �������ƂɃr�b�g���]�����A128�ɂȂ�܂ŌJ��Ԃ��Ȃ�
//  �擪���珇�ԂɁA����ȍ~�̒l�̍��v�̉��ʃo�C�g�������Ă���
//---------------------------------------------------------------------------
void get_cipher_ptn()
{
	UnicodeString keystr = UserName;
	if (keystr.IsEmpty()) keystr = "UnknownUser";

	UnicodeString sysdrv = GetEnvironmentVariable("SYSTEMDRIVE");
	if (!sysdrv.IsEmpty()) {
		UnicodeString dstr = IncludeTrailingPathDelimiter(sysdrv).UpperCase();
		DWORD VolSerialNo, MaxCompLen, Flags;
		if (::GetVolumeInformation(dstr.c_str(), NULL, 0, &VolSerialNo, &MaxCompLen, &Flags, NULL, 0))
			keystr += IntToHex((int)VolSerialNo, 8);
	}

	int n = keystr.Length();
	for (int i=0; i<MAX_XORBIT_SIZE; i++)
		CipherXorPtn[i] = (unsigned char) ~keystr[(i%n) + 1];

	for (int i=0; i<MAX_XORBIT_SIZE-1; i++) {
		int sum = 0;
		for (int j=i+1; j<MAX_XORBIT_SIZE; j++) sum += CipherXorPtn[j];
		CipherXorPtn[i] = (unsigned char)(sum & 0x00ff);
	}
}

//---------------------------------------------------------------------------
//�Í���
//  1. ��������t���ɂЂ�����Ԃ�
//  2. �������ƂɃr�b�g�p�^�[����XOR
//  3. 2��16�i���̕�����ɂ��Č���
//---------------------------------------------------------------------------
UnicodeString cipher(UnicodeString orgstr)
{
	if (orgstr.IsEmpty()) return EmptyStr;
	get_cipher_ptn();

	UnicodeString retstr;
	int n = orgstr.Length();
	for (int i=0; i<n; i++)
		retstr.cat_sprintf(_T("%02x"), (unsigned char)(orgstr[n - i] ^ CipherXorPtn[i%MAX_XORBIT_SIZE]));

	return retstr;
}

//---------------------------------------------------------------------------
//������
//  1.������̓�����2�����������16�i���Ƃ��Đ��l��
//  2.�r�b�g�p�^�[����XOR
//  3.�����ɒ����ċt���Ɍ���
//---------------------------------------------------------------------------
UnicodeString uncipher(UnicodeString cipstr)
{
	if (cipstr.IsEmpty()) return EmptyStr;
	get_cipher_ptn();

	UnicodeString retstr = EmptyStr;
	int n = cipstr.Length();
	for (int i=0; i<n/2; i++) {
		UnicodeString dgtstr = cipstr.SubString(i*2 + 1, 2);
		UnicodeString tmpstr;
		tmpstr.sprintf(_T("%c"), (unsigned char)(StrToIntDef("0x" + dgtstr, 0x3f) ^ CipherXorPtn[i%MAX_XORBIT_SIZE]));
		retstr = tmpstr + retstr;
	}

	return retstr;
}
//---------------------------------------------------------------------------