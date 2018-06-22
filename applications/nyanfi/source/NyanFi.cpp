//----------------------------------------------------------------------//
// NyanFi																//
//----------------------------------------------------------------------//
#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include <memory>

//---------------------------------------------------------------------------
USEFORM("RenDlg.cpp", RenameDlg);
USEFORM("SameDlg.cpp", SameNameDlg);
USEFORM("ShareDlg.cpp", NetShareDlg);
USEFORM("PackDlg.cpp", PackArcDlg);
USEFORM("PrnImgDlg.cpp", PrintImgDlg);
USEFORM("RegExChk.cpp", RegExChecker);
USEFORM("SrtModDlg.cpp", SortModeDlg);
USEFORM("SyncDlg.cpp", RegSyncDlg);
USEFORM("TabDlg.cpp", TabSetDlg);
USEFORM("Splash.cpp", SplashForm);
USEFORM("KeyDlg.cpp", KeyListDlg);
USEFORM("LoupeFrm.cpp", LoupeForm);
USEFORM("MainFrm.cpp", NyanFiForm);
USEFORM("InpExDlg.cpp", InputExDlg);
USEFORM("InspectFrm.cpp", InspectForm);
USEFORM("JoinDlg.cpp", JoinTextDlg);
USEFORM("ModalScr.cpp", ModalScrForm);
USEFORM("OptDlg.cpp", OptionDlg);
USEFORM("MaskDlg.cpp", PathMaskDlg);
USEFORM("MaskSelDlg.cpp", MaskSelectDlg);
USEFORM("XmlView.cpp", XmlViewer);
USEFORM("TaskDlg.cpp", TaskManDlg);
USEFORM("UserMdl.cpp", UserModule); /* TDataModule: File Type */
USEFORM("InpDir.cpp", InpDirDlg);
USEFORM("CompDlg.cpp", FileCompDlg);
USEFORM("CreDirsDlg.cpp", CreateDirsDlg);
USEFORM("CsvRecFrm.cpp", CsvRecForm);
USEFORM("CmdListDlg.cpp", CmdFileListDlg);
USEFORM("ColDlg.cpp", ColorDlg);
USEFORM("ColPicker.cpp", ColorPicker);
USEFORM("DirDlg.cpp", RegDirDlg);
USEFORM("DistrDlg.cpp", DistributionDlg);
USEFORM("DotDlg.cpp", DotNyanDlg);
USEFORM("CvEncDlg.cpp", CvTxtEncDlg);
USEFORM("CvImgDlg.cpp", CvImageDlg);
USEFORM("DebugFrm.cpp", DebugForm);
USEFORM("AppDlg.cpp", AppListDlg);
USEFORM("AskRep.cpp", AskRepDlg);
USEFORM("BakDlg.cpp", BackupDlg);
USEFORM("About.cpp", AboutBox);
USEFORM("ChInfFrm.cpp", CharInfoForm);
USEFORM("ChmodDlg.cpp", FtpChmodDlg);
USEFORM("BitmapFrm.cpp", BitmapForm);
USEFORM("BtnDlg.cpp", ToolBtnDlg);
USEFORM("CalcDlg.cpp", Calculator);
USEFORM("GrepOptDlg.cpp", GrepExOptDlg);
USEFORM("HistDlg.cpp", DirHistoryDlg);
USEFORM("FuncDlg.cpp", FuncListDlg);
USEFORM("GenInfDlg.cpp", GeneralInfoDlg);
USEFORM("GifView.cpp", GifViewer);
USEFORM("InpCmds.cpp", InpCmdsDlg);
USEFORM("HistFrm.cpp", HistForm);
USEFORM("ExeDlg.cpp", ExeCmdDlg);
USEFORM("ExpCsv.cpp", ExpCsvDlg);
USEFORM("ExTViewer.cpp", ExTxtViewer);
USEFORM("DriveDlg.cpp", SelDriveDlg);
USEFORM("DrvGraph.cpp", DriveGraph);
USEFORM("EditHistDlg.cpp", EditHistoryDlg);
USEFORM("FindTag.cpp", FindTagForm);
USEFORM("FindTxtDlg.cpp", FindTextDlg);
USEFORM("FtpDlg.cpp", FtpConnectDlg);
USEFORM("FileExtDlg.cpp", FileExtensionDlg);
USEFORM("FileInfDlg.cpp", FileInfoDlg);
USEFORM("FindDlg.cpp", FindFileDlg);
USEFORM("TagDlg.cpp", TagManDlg);
//---------------------------------------------------------------------------
#include "Global.h"
#include "Splash.h"

//---------------------------------------------------------------------------
int WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR lpCmdLine, int)
{
	bool restart   = false;
	bool duplicate = false;
	bool not_act   = false;

	StartedCount = GetTickCount();

	ExePath = REPLACE_TS(ExtractFilePath(Application->ExeName), "\\.\\", "\\");	//IDE���s���̑΍�

	//�N�����I�v�V�������擾
	std::unique_ptr<TStringList> opt_lst(new TStringList());
	split_cmd_line(lpCmdLine, opt_lst.get());

	//INI�t�@�C�����̐ݒ�
	UnicodeString inam;
	for (int i=0; i<opt_lst->Count; i++) {
		UnicodeString lbuf = opt_lst->Strings[i];
		if (remove_top_s(lbuf, "-I"))
			inam = rel_to_absdir(exclude_quot(lbuf));
		if (USTARTS_TI("-XCalculator_CB", lbuf))
			not_act = true;
	}
	if (inam.IsEmpty()) inam = ChangeFileExt(Application->ExeName, ".INI");

	//��d�N����������Ă��邩���`�F�b�N
	try {
		std::unique_ptr<TStringList> ibuf(new TStringList());
		ibuf->LoadFromFile(inam, TEncoding::UTF8);
		duplicate = equal_1(ibuf->Values["MultiInstance"]);
	}
	catch (...) {
		;
	}

	//�~���[�e�b�N�X�ɂ���d�N�����`�F�b�N
	HANDLE hMutex = ::CreateMutex(NULL, FALSE, _T("NyanFi_is_running"));
	if (GetLastError()==ERROR_ALREADY_EXISTS) {
		//�����L��
		HWND hPrimeWnd = get_PrimNyanWnd();
		if (hPrimeWnd) {
			IsPrimary = false;
			int cmdlen = lstrlen(lpCmdLine);
			if ((cmdlen>0 || !duplicate) && !not_act) {
				HWND hApp = ::GetWindow(hPrimeWnd, GW_OWNER);
				if (::IsIconic(hApp)) ::SendMessage(hApp, WM_SYSCOMMAND, SC_RESTORE, 0);
				if (::IsIconic(hPrimeWnd)) ::SendMessage(hPrimeWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
				::SetForegroundWindow(hPrimeWnd);
			}
			//�R�}���h���C���̓��e�𑗂�
			if (cmdlen>0) {
				COPYDATASTRUCT cd;
				cd.dwData = 1;
				cd.cbData = sizeof(_TCHAR) * (cmdlen + 1);
				cd.lpData = lpCmdLine;
				::SendMessage(hPrimeWnd, WM_COPYDATA, 0, (LPARAM)&cd);
				duplicate = false;
				for (int i=0; i<opt_lst->Count && !restart; i++)
					restart = SameText(opt_lst->Strings[i], "-Q");	//-Q or -q
			}
		}

		if (!restart && !duplicate) {	//�ċN���Ŗ����ꍇ��d�N�������ɏI��
			::CloseHandle(hMutex);
			return -1;
		}
	}

	//�ċN���̂��߂Ɋ����̏I����҂�(�ő�20�b)
	if (restart) {
		bool ok = false;
		for (int i=0; i<200 && !ok; i++) if (::WaitForSingleObject(hMutex, 100)==WAIT_OBJECT_0) ok = true;
		if (!ok) {	//�������I������Ȃ��̂ŏI��
			::CloseHandle(hMutex);
			return -1;
		}
		IsPrimary = true;
	}

	::CoInitialize(NULL);

	try {
		Screen->Cursor = crHourGlass;
		Application->Initialize();
		Application->MainFormOnTaskBar = true;

		//INI�t�@�C���̏�����
		IniFile = new UsrIniFile(inam);

		//�X�v���b�V���\��
		if (IniFile->ReadBoolGen(_T("ShowSplash"))) {
			SplashForm = new TSplashForm(Application);
			//�o�[�W����
			unsigned mj, mi, bl;
			if (GetProductVersion(Application->ExeName, mj, mi, bl)) {
				SplashForm->VerLabel->Caption = UnicodeString().sprintf(_T("V%.2f"), (mj*100 + mi*10 + bl)/100.0);
				SplashForm->Show();
				SplashForm->SetMsgLabel(_T("��������..."));
			}
		}

		Application->Title = get_NewTitle(IsPrimary);
		Application->CreateForm(__classid(TUserModule), &UserModule);
		Application->CreateForm(__classid(TNyanFiForm), &NyanFiForm);
		Application->CreateForm(__classid(TBitmapForm), &BitmapForm);
		Application->CreateForm(__classid(TCharInfoForm), &CharInfoForm);
		Application->CreateForm(__classid(TCmdFileListDlg), &CmdFileListDlg);
		Application->CreateForm(__classid(TColorPicker), &ColorPicker);
		Application->CreateForm(__classid(TCsvRecForm), &CsvRecForm);
		Application->CreateForm(__classid(TEditHistoryDlg), &EditHistoryDlg);
		Application->CreateForm(__classid(TFileInfoDlg), &FileInfoDlg);
		Application->CreateForm(__classid(TFindTextDlg), &FindTextDlg);
		Application->CreateForm(__classid(TFuncListDlg), &FuncListDlg);
		Application->CreateForm(__classid(TGeneralInfoDlg), &GeneralInfoDlg);
		Application->CreateForm(__classid(TGifViewer), &GifViewer);
		Application->CreateForm(__classid(THistForm), &HistForm);
		Application->CreateForm(__classid(TInpCmdsDlg), &InpCmdsDlg);
		Application->CreateForm(__classid(TInputExDlg), &InputExDlg);
		Application->CreateForm(__classid(TInspectForm), &InspectForm);
		Application->CreateForm(__classid(TKeyListDlg), &KeyListDlg);
		Application->CreateForm(__classid(TLoupeForm), &LoupeForm);
		Application->CreateForm(__classid(TModalScrForm), &ModalScrForm);
		Application->CreateForm(__classid(TSameNameDlg), &SameNameDlg);
		Application->CreateForm(__classid(TInpDirDlg), &InpDirDlg);
		cursor_Default();
		Application->Run();
	}
	catch (Exception &exception) {
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try {
			throw Exception(EmptyStr);
		}
		catch (Exception &exception) {
			Application->ShowException(&exception);
		}
	}

	::CoUninitialize();
	::CloseHandle(hMutex);

	//�ċN���p�o�b�`�t�@�C�����N��
	if (!RstBatName.IsEmpty()) {
		if (!Execute_ex(RstBatName, EmptyStr, ExtractFilePath(Application->ExeName), "H"))
			msgbox_ERR("�o�b�`�t�@�C���̋N���Ɏ��s���܂����B");
	}

	return 0;
}
//---------------------------------------------------------------------------