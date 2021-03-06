//----------------------------------------------------------------------//
// NyanFi																//
//  グローバル															//
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
UnicodeString RstBatName;	//再起動用バッチファイル名

//---------------------------------------------------------------------------
//HTMLヘルプ
//！Vcl.HTMLHelpViewer は用いず、HtmlHelp API(動的リンク)で処理
HINSTANCE	  hHHctrl	  = NULL;	//hhctrl.ocx
FUNC_HtmlHelp lpfHtmlHelp = NULL;
DWORD		  Cookie	  = NULL;
bool 		  CancelHelp  = false;

//---------------------------------------------------------------------------
//非公開API
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
HWND  MainHandle = NULL;		//メインウィンドウのハンドル
DWORD ProcessId  = 0;			//プロセスID
bool  IsAdmin	 = false;		//管理者権限
int   StartedCount;				//実行開始カウント(m秒)
int   NyanFiIdNo = 0;			//多重 NyanFi 識別ID
bool  IsPrimary  = true;		//最初に起動された
win_dat Win2Data;				//二重起動終了時の画面情報

int    ScrMode  = SCMD_FLIST;	//画面モード
double ScrScale = 1.0;			//画面スケーリング
int    SIcoSize = 16;			//スモールアイコンの表示サイズ

bool  IsMuted	= false;		//音量ミュート

TRichEdit *TempRichEdit = NULL;

TTaskThread *TaskThread[MAX_TASK_THREAD];	//タスク処理スレッド
int  MaxTasks;								//最大タスク数
bool RsvSuspended = false;					//予約の保留
TaskConfigList *TaskReserveList;			//タスク予約リスト
int  LastOpCount;							//最後に操作した時のカウント値

TImgViewThread   *ImgViewThread   = NULL;	//画像ビュー処理スレッド
TThumbnailThread *ThumbnailThread = NULL;	//サムネイル取得スレッド
TGetIconThread   *GetIconThread   = NULL;	//アイコン取得スレッド

UserArcUnit *usr_ARC;			//アーカイブ処理ユニット
UnicodeString FExt7zDll;		//7z.dll で対応する拡張子

MigemoUnit  *usr_Migemo;		//Migemo処理ユニット
UnicodeString MigemoPath;
bool LastMigemoMode;
bool LastMigemoModeF;

SpiUnit *SPI = NULL;			//Susieプラグイン
UnicodeString SpiDir;			//Susieプラグインのディレクトリ
bool UseSpiFirst;				//Susieプラグインを優先的に使う

int  WicScaleOpt;				//WIC の縮小・拡大アルゴリズム
UnicodeString WicFextStr;		//WIC が対応している拡張子

bool gCopyAll;
bool gCopyCancel;

int  gCopyMode	= CPYMD_OW;		//左右が別ディレクトリの場合のコピーモード
int  gCopyMode2 = CPYMD_OW;		//左右が同一ディレクトリの場合のコピーモード
int  xCopyMode = -1;			//ExeCommand 内で用いる強制モード

UnicodeString gCopyFmt = "\\N_\\SN(1)";

int  OptionPageIndex = 0;		//オプション設定ダイアログのページインデックス

bool SyncLR;					//左右のディレクトリ変更を同期

UnicodeString GlobalErrMsg;

bool Initialized	= false;	//初期化が完了した
bool UnInitializing = false;	//終了処理が開始された
bool Closing		= false;	//終了処理中

int  InhReload = 0;				//Reload の抑止
bool DisReload = false;			//Reload の無効化

UnicodeString FindPath;
bool FindAborted;				//検索中断要求
int  FindTag;
int  FindCount;					//マッチ数

bool CalcAborted;				//計算中断要求
int  CalcTag;

bool MultiInstance;				//二重起動を許す
bool CloseOthers;				//他のNyanFiを終了
bool StoreTaskTray;				//最小化時にタスクトレイに格納
bool ShowIcon;					//アイコンを表示
bool UseIndIcon;				//ファイル固有のアイコンを使用
bool ShowHideAtr;				//隠しファイルを表示
bool ShowSystemAtr;				//システムファイルを表示
bool DispRegName;				//パスの表示に登録名を用いる
bool UncToNetDrive;				//UNCをネットワークドライブ名に
bool PathInTitleBar;			//タイトルバーにカレントパスを表示
bool OmitEndOfName;				//長い名前は末尾を省略
bool ShowDirJumboIco;			//ディレクトリは特大アイコンを表示
bool ShowInZipImg;				//ZIP内の画像を表示
bool PreviewAniGif;				//アニメーションGIFのプレビュー
bool ForceDel;					//読込専用ファイルも強制的に上書き・削除
bool RemoveCdReadOnly;			//CD-ROMからのコピー時に読込専用属性を解除
bool CopyNoBuffering;			//バッファーなしI/Oを使用してコピー
bool ShowArcCopyProg;
bool DelUseTrash;				//ファイルの削除にゴミ箱を使う
bool EditNewText;				//新規テキスト作成後にエディタで開く
bool ViewArcInf;				//アーカイブ内のファイル情報を表示
bool ReloadOnActive;			//アクティブ時に最新の情報に
bool OpenAddedDrive;			//ドライブが追加されたらカレントに表示
bool CheckUnc;					//UNCパスをチェック
bool ShowMsgHint;				//警告などをヒント表示
bool ShowKeyHint;				//2ストローク操作のヒント表示
bool ShowNoKeyHint;				//無効な2ストローク操作の警告を出す
bool ShowCopyHint;				//クリップボードへのコピー情報をヒント表示
bool ShowDuplNotify;			//二重起動されたNyanFiからの通知を表示
bool LogErrOnly;				//タスクでエラー箇所のみをログ表示
bool LogErrMsg;					//ログにエラーメッセージを出力
bool LogDebugInf;				//ログにデバッグ情報を出力
bool LogHideSkip;				//ログにスキップ項目を出力しない
bool LogFullPath;				//ログの処理内容をフルパスで表示
bool SaveLog;					//終了時にログを保存
bool AppendLog;					//同日のログは追記保存
bool NotSortWorkList;			//ワークリストはソートしない
bool AutoDelWorkList;			//存在しない項目を自動削除
bool DirHistCsrPos;				//ディレクトリ履歴でカーソル位置を記憶
bool DelDplDirHist;				//重複するディレクトリ履歴を削除
bool WorkToDirHist;				//ワークリストを履歴に含める
bool NoCheckDirHist;			//ディレクトリ履歴の存在チェックを行わない
bool ExtSaveDirHist;			//全体履歴を別ファイルに保存
bool LoopFilerCursor;			//ファイラーでカーソルをループ移動
bool FlCursorVisible;			//ファイラーでカーソルを常に可視領域に
bool AutoCompComboBox;			//コンボボックス入力を自動補完
bool DialogCenter;				//ダイアログをメイン画面の中央に
bool MenuAutoHotkey;			//メニューのアクセラレータキーを自動調整
bool EscapeHelp;				//ESCキーでヘルプを閉じる
bool InhbitAltMenu;				//ALTキーでメニューにフォーカスしない
bool SelectByMouse;				//マウスでファイラーの項目を選択
bool SelectBaseOnly;			//ファイル名主部でのみ選択
bool SelectIconSngl;			//アイコン部分で個別に選択
bool TimColEnabled;				//タイムスタンプの配色を有効
bool PriorFExtCol;				//拡張子部分は属性色より優先
bool ColorOnlyFExt;				//拡張子別配色は拡張子部分のみに適用
bool ShowMainMenu;				//メインメニューを表示
bool ShowImgPreview;			//イメージプレビューを表示
bool ShowProperty;				//ファイル情報を表示
bool ShowFileListOnly;			//ファイルリストのみを表示
bool ShowFileListOnly2;
bool ShowSttBar;				//ステータスバーを表示
bool ShowToolBar;				//ツールバーを表示
bool ShowToolBarV;
bool ShowToolBarI;
bool ToolBarISide;
bool ShowTabBar;				//タブバーを表示
bool ShowFKeyBar;				//ファンクションキーバーを表示
bool NoClsTabBtn;				//タブの閉じるボタン非表示
bool HideScrBar;				//縦スクロールバーを隠す
bool ShowByteSize;				//バイト単位表示
bool ShowHeader;				//ヘッダを表示
bool FixListWidth;				//リスト幅を固定
bool KeepOnResize;				//ウィンドウサイズ変更時に比率を維持
bool KeepCurListWidth;			//カレント側のリスト幅を維持
bool NoSpaceFExt;				//拡張子を分離しない
bool HideTitleMenu;				//最大化時にタイトルバーとメニューを隠す
bool FlatInfPanel;				//情報パネルをフラットに
bool ShowLineNo;				//行番号を表示
bool ShowLineCursor;			//行カーソルを表示
bool ShowTAB;					//タブを表示
bool ShowCR;					//改行を表示
bool ShowTextRuler;				//ルーラを表示
bool TxtSttIsBottom;			//情報ヘッダを下部に表示(TV)
bool ChkAozora;					//青空文庫形式を考慮する
bool UseXd2tx;					//xd2txlib.dll でテキスト表示
bool ClickableUrl;				//ダブルクリックでURLを開く
bool RestoreViewLine;			//ビュアーでカーソルの行位置を復元
bool TvCursorVisible;			//ビュアーでカーソルを常に可視領域に
bool LimitBinCsr;				//バイナリ表示でカーソル移動を制限
bool TxtColorHint;				//カーソル位置数値のカラーをヒント表示
bool BinMemMaped;				//バイナリではメモリマップドファイルとして開く
bool EmpComment;				//コメントを強調表示
bool EmpStrings;				//文字列を強調表示
bool EmpReserved;				//予約語を強調表示
bool EmpSymbol;					//シンボルを強調表示
bool EmpNumeric;				//数値を強調表示
bool EmpHeadline;				//見出しを強調表示
bool EmpRuby;					//ルビを強調表示
bool RotViewImg;				//Exif情報にしたがって画像を回転
bool KeepZoomRatio;				//別の画像に移った時にズーム倍率を維持する
bool ShowThumbName;				//サムネイルにファイル名を表示
bool ShowThumbExif;				//サムネイルに簡易Exif情報を表示
bool ShowThumbFExt;				//サムネイルに拡張子を強調表示
bool NotThumbIfArc;				//アーカイブ内は一括取得しない
bool LoopViewCursor;			//イメージビュアーでカーソルをループ移動
bool HintTopEndImg;				//先頭・最後でヒント表示
bool BeepTopEndImg;				//警告音
bool ImgSttIsBottom;			//情報ヘッダを下部に表示(IV)
bool SeekBindDir;				//シークバーの始点を綴じ方向に合わせる
bool SeekSwapNxtPrv;			//右綴じでNext/PrevFile入替
bool OneStepInDblPg;			//見開き表示でも1ファイルずつ移動
bool HideCsrInFull;				//全画面表示でマウスカーソルを隠す
bool HideThumbInFull;			//全画面表示でサムネイルを隠す
bool AnimateGif;				//イメージビュアーでアニメーションGIFを表示
bool ShowThumbScroll;			//サムネイルのスクロールバーを表示
bool ShowHistogram;				//ヒストグラムを表示
bool ShowLoupe;					//ルーペを表示
bool ShowGifViewer;				//GIFビュアーを表示
bool ShowSeekBar;				//シークバーを表示
bool WarnHighlight;				//白飛び警告
bool DoublePage;				//見開き表示
bool RightBind;					//右綴じ

bool PermitDotCmds;				//.nyanfi でコマンド実行を許可
bool InheritDotNyan;			//上位ディレクトリから .nyanfi を継承
bool DotNyanPerUser;			//ユーザ名別に .nyanfi を作成

int  ScrBarStyle;				//スクロールバー・スタイル

bool ModalScreen;				//モーダル表示効果
int  ModalScrAlpha;

int  InitialModeI;				//イメージビュアーを開いた時の初期状態
int  LastZoomRatio;				//前回のズーム

bool MarkImgClose;				//イメージビュアーを閉じる時にマーク
UnicodeString MarkImgPath;		//マークするパス
UnicodeString MarkImgFExt;		//マークする拡張子
UnicodeString MarkImgMemo;

UnicodeString DlgMoveShift;		//ダイアログ移動のシフトキー
UnicodeString DlgSizeShift;		//ダイアログのサイズ変更のシフトキー
int  DlgMovePrm;
int  DlgSizePrm;

UnicodeString WheelCmdF[4];		//ホイールコマンド
UnicodeString WheelCmdV[4];
UnicodeString WheelCmdI[4];

UnicodeString WheelBtnCmdF[4];	//ホイールボタンを押した時のコマンド
UnicodeString WheelBtnCmdV;
UnicodeString WheelBtnCmdI;

UnicodeString X1BtnCmdF;		//X1ボタンを押した時のコマンド
UnicodeString X2BtnCmdF;		//X2ボタンを押した時のコマンド
UnicodeString X1BtnCmdV;
UnicodeString X2BtnCmdV;
UnicodeString X1BtnCmdI;
UnicodeString X2BtnCmdI;

UnicodeString EmpBinPtn1;		//バイナリ強調パターン1
UnicodeString EmpBinPtn2;		//バイナリ強調パターン2
UnicodeString EmpBinPtn3;		//バイナリ強調パターン3

UnicodeString HtmInsHrCls;		//ブロックの前に罫線を挿入するクラス
UnicodeString HtmHdrStr;		//見出し文字
bool HtmInsHrSct;				//<section>の前に罫線挿入
bool HtmInsHrArt;				//<article>の前に罫線挿入
bool HtmInsHrNav;				//<nav>の前に罫線挿入
bool ToMarkdown;				//Markdown記法に変換

UnicodeString NoWatchPath;		//ディレクトリ監視から除外するパス

UnicodeString FExtGetInf;		//ファイル情報を取得する拡張子
UnicodeString FExtNoInf;		//ファイル情報を取得しない拡張子
UnicodeString NoInfPath;		//ファイル情報を取得しないパスリスト

UnicodeString FExtImgPrv;		//イメージプレビューを行う拡張子
UnicodeString FExtNoImgPrv;		//イメージプレビューを行わない拡張子
UnicodeString NoImgPrvPath;		//イメージプレビューを行わないパスリスト

UnicodeString FExtNoIView;		//イメージビュアーで閲覧しない拡張子

UnicodeString DrvInfFmtR;		//ドライブ情報の書式 : ルート
UnicodeString DrvInfFmtS;		//ドライブ情報の書式 : 選択時
UnicodeString DrvInfFmtN;		//ドライブ情報の書式 : その他
bool SelColDrvInf;				//選択時はファイルリストの配色に

UnicodeString SttBarFmt;		//ステータスバーの書式
UnicodeString SttClockFmt;		//時計の書式

int  MaxLogFiles;				//ログファイルの保存世代数

bool SureCopy;					//確認 - コピー
bool SureMove;					//確認 - 移動
bool SureDelete;				//確認 - 削除
bool SureUnPack;				//確認 - 解凍
bool SureExec;					//確認 - 実行
bool SureOther;					//確認 - その他
bool SureExit;					//確認 - 終了
bool SureCmpDel;				//確認 - 完全削除は2度確認
bool SureCancel;				//確認 - キャンセルボタンを表示
bool SureDefNo;					//確認 - 「いいえ」がデフォルト

int  ProtectDirMode;			//ディレクトリの削除制限モード
bool ProtectSubDir;				//下位ディレクトリも制限
bool ProtectFile;				//ファイルも制限

int  BorderMoveWidth;			//BorderLeft、BorderRight: １回の移動幅
UnicodeString AutoRenFmt;		//Copy、Move: 同名時の自動改名書式

int  RemoteWaitTime;			//コールバック時のウェイトタイム
int  NormalWaitTime;
int  NopDtctTime;				//無操作だとみなす時間
int  TimeTolerance;				//タイムスタンプの許容誤差

bool AppListChgMin;				//AppList:    他アプリに切り替えたときに最小化
bool CreDirChg;					//CreateDir:  ディレクトリ作成後にカレント変更
bool CreDirCnvChar;				//CreateDir:  文字置換を適用
UnicodeString FExtExeFile;		//ExeCommandLine: 実行ファイルとみなす拡張子
int  OpenByMode;				//OpenByApp:  関連付けされていない場合の動作モード
bool OpenByCmdPrompt;			//OpenByApp:  [..] でコマンドプロンプトを開く
bool OpenDirByStd;				//OpenByApp:  ディレクトリでは標準 Enter キー動作
int  DblClickFlMode;			//ファイルリストでのダブルクリック
bool OpenOnlyCurApp;			//OpenByApp:  選択状態にかかわらずカーソル位置のみを開く
bool OpenOnlyCurWin;			//OpenByWin:  選択状態にかかわらずカーソル位置のみを開く
bool DownAfterOpenStd;			//OpenStandard: 実行後にカーソルを下に移動
bool OpenStdOnResList;			//OpenStandard: 結果リストでも通常動作
UnicodeString IniSeaShift;		//IniSearch:  頭文字サーチのシフトキー
bool IniSeaByNum;				//  数字キーでも頭文字サーチ
bool IniSeaBySign;				//  Shift+数字キーの記号もサーチ
bool IncSeaCaseSens;			//IncSearch:  大小文字を区別
bool IncSeaLoop;				//IncSearch:  上下端でループ
bool IncSeaMatch1Exit;			//IncSearch:  マッチ数1で抜ける
int  IncSeaMigemoMin;			//IncSearch:  migemo の検索開始文字数
bool NotShowNoTask;				//TaskMan:    タスクを実行していないときはマネージャを表示しない
UnicodeString WebSeaUrl;		//WebSearch:  検索エンジンURL

bool FindPathColumn;			//結果リストに「場所」列を表示
int  FindPathWidth;				//「場所」の最小列幅

bool GrepShowItemNo;			//GREP結果に項目番号を表示
bool GrepFileItemNo;			//項目番号をファイル単位で表示
bool GrepShowSubDir;			//GREP結果にサブディレクトリ名を表示
bool GrepTrimTop;				//GREP結果で行頭のタブや空白を削除
bool GrepOmitTop;				//GREP結果でマッチ語が見えないとき前部分を省略
bool GrepEmFilter;				//GREP結果でフィルタの語を強調表示
int  GrepOutMode;				//GREP 出力先	0:無し/ 1:ファイル/ 2:クリップボード
UnicodeString GrepFileName;		//出力ファイル名
UnicodeString GrepAppName;		//起動アプリ名
UnicodeString GrepAppParam;		//アプリの起動パラメータ
UnicodeString GrepAppDir;		//アプリの作業ディレクトリ
bool GrepAppend;				//既存ファイルに追加

UnicodeString GrepFileFmt;		//ファイル情報の書式
UnicodeString GrepInsStrW;		//マッチ語前の挿入文字列
UnicodeString GrepInsStrW2;		//マッチ語後の挿入文字列
bool GrepTrimLeft;				//行頭のタブや空白を削除
bool GrepReplaceTab;			//タブを空白1文字に置換
bool GrepReplaceCr;				//改行を文字列に置換
UnicodeString GrepRepCrStr;		//改行の置換文字列

bool BackupReplace;				//置換バックアップ作成
UnicodeString FExtRepBackup;	//置換バックアップの拡張子
UnicodeString RepBackupDir;		//置換バックアップ先
UnicodeString ReplaceLogName;	//置換ログファイル名

bool SaveReplaceLog;			//置換ログを保存
bool ReplaceAppend;				//既存ファイルに追加
bool OpenReplaceLog;			//ログをテキストエディタで開く

UnicodeString UserName;			//ユーザ名
UnicodeString TempPath;			//一時ディレクトリ
UnicodeString TempPathA;		//一時ディレクトリ(絶対)
UnicodeString TempPathFTP;		//FTP用一時ディレクトリ(絶対)

UnicodeString DownloadPath;		//ダウンロード用
UnicodeString LibraryPath;		//ライブラリ
UnicodeString WorkListPath;		//ワークリスト参照パス
UnicodeString RefParamPath;		//パラメータ参照パス
UnicodeString IconFilePath;		//アイコン参照パス
UnicodeString CmdFilePath;		//コマンドファイル参照パス

int VersionNo;
UnicodeString VersionStr;		//バージョン表示
UnicodeString CopyrightStr;		//著作権

UnicodeString DirBraStr;		//ディレクトリ括弧文字
UnicodeString DirKetStr;
UnicodeString TimeStampFmt;		//タイムスタンプの表示書式

UnicodeString CallHotKey;		//呼び出しホットキー
UnicodeString AppListHotKey;	//アプリ一覧ホットキー
UnicodeString AppListHotPrm;	//アプリ一覧パラメータ

UnicodeString SaveTxtPath;		//テキスト保存パス
int SaveEncIndex;				//エンコーディングのインデックス

UnicodeString NoDirHistPath;	//ディレクトリ履歴に入れないパス
UnicodeString NoEditHistPath;	//編集履歴に入れないパス
UnicodeString NoViewHistPath;	//閲覧履歴に入れないパス

//---------------------------------------------------------------------------
UnicodeString FTPTextModeFExt;	//テキストモードで転送する拡張子
bool FTPDlKeepTime;				//ダウンロードファイルのタイムスタンプを維持
bool FTPUpKeepTime;				//アップロードファイルのタイムスタンプを維持
bool FTPUpToLower;				//ファイル名を小文字化してアップロードする
bool FTPLogResponse;			//応答メッセージをログに表示
int  FTPRemoteSide;				//リモート側 0:カレント/ 1:左/ 2:右
int  FTPDisconTimeout;			//FTPが無操作の場合切断する時間
UnicodeString FTPSndConnect;	//接続時の通知音
UnicodeString FTPSndDiscon;		//切断時の通知音
UnicodeString FTPSndTransfer;	//転送完了時の通知音

//---------------------------------------------------------------------------
int CurTabIndex = 0;			//タブインデックス(0〜

UnicodeString CurPathName;
int CurListTag = 0;				//カレント側タグ  0:左/ 1:右
int OppListTag = 1;				//反対側タグ
int LastCurTag = 0;				//前回終了時のカレント

TListBox *FileListBox[MAX_FILELIST];	//ファイルリストボックス

UnicodeString ViewFileName;				//表示中のファイル名
UnicodeString ViewFileName2;

//---------------------------------------------------------------------------
TStringList *GeneralList;				//TStringList のリスト(一括破棄用)

TStringList *FileList[MAX_FILELIST];	//ファイルリスト
TStringList *ArcFileList[MAX_FILELIST];	//仮想ディレクトリのファイルリスト
TStringList *ResultList[MAX_FILELIST];	//検索結果リスト
TStringList *SelMaskList[MAX_FILELIST];	//選択マスクリスト
TStringList *TmpBufList;				//一時退避リスト

TStringList *TabList;					//タブリスト
int FlTabWidth;							//タブの幅
int FlTabStyle;							//タブのスタイル

TStringList *DriveInfoList;				//ドライブ情報
TStringList *DriveLogList;				//ドライブ容量ログ
UnicodeString DriveLogName;				//ドライブ容量ログのファイル名

TStringList *WatchTailList;				//末尾監視リスト
UnicodeString LastWatchLog;				//直前の監視ログ

TStringList *InvalidUncList;			//無効なUNCリスト

//ファイル固有アイコンのキャッシュ (必ず IconRWLock で保護すること)
TStringList *CachedIcoList;
TMultiReadExclusiveWriteSynchronizer *IconRWLock;
int IconCache;							//アイコンキャシュ数

TStringList *GeneralIconList;			//ファイルリスト表示用の一般アイコン
TStringList *MenuBtnIcoList;			//

TStringList *UsrIcoList;				//メニュー、ツールボタン用アイコン
TStringList *DrvIcoList;				//ドライブのアイコンリスト

TStringList *BakSetupList;				//バックアップ設定リスト
TStringList *SyncDirList;				//同期コピー先リスト
TStringList *AssRenList;				//関連ファイル同時改名設定リスト
TStringList *DistrDefList;				//振り分け正義リスト

TStringList *GrepPathList;				//GREP 対象パスのリスト
TStringList *GrepFileList; 				//GREP 対象ファイルリスト
TStringList *GrepResultList;			//GREP の結果リスト
TStringList *GrepResultBuff;			//GREP 結果の退避バッファ

TStringList *ViewFileList;				//イメージビュアーでのファイル名リスト
bool isViewIcon   = false;				//イメージビュアーでアイコンを表示中
bool isViewAGif   = false;				//イメージビュアーでアニメーションGIFを表示中
bool isViewClip   = false;				//イメージビュアーでクリップボードを表示中
bool isViewFTP	  = false;				//イメージビュアーでFTPのファイルを表示中
bool rqThumbnail  = false;				//クリップボード閲覧後のサムネイル復帰要求
bool nrmThumbnail = false;				//通常画面時のサムネイル表示

TStringList *LaunchList;				//ランチャーリスト(アプリケーション一覧)

TStringList *CmdFileList;				//コマンドファイルのリスト
TStringList *XCMD_VarList = NULL;		//変数リスト

UnicodeString PathMask[MAX_FILELIST];	//パスマスク
UnicodeString DriveInf[MAX_FILELIST];	//ドライブ情報

flist_stt ListStt[MAX_FILELIST];
flist_stt *CurStt;
flist_stt *OppStt;

TStringList *WorkList;					//ワークリスト
UnicodeString WorkListName;				//ワークリストのファイル名
TDateTime WorkListTime;					//ワークリストのタイムスタンプ
bool WorkListChanged;					//ワークリストの内容が変更された
bool WorkListFiltered;					//ワークリストにフィルタが適用されている
bool WorkListHasSep;					//ワークリストにセパレータがある
bool rqWorkListDirInf;					//ワークリストのディレクトリ情報更新を要求

TStringList  *PlayList;					//プレイリスト
UnicodeString PlayListFile;				//プレイリストのファイル名
UnicodeString PlayFile;					//再生中ファイル名
int  PlayStbIdx;
bool PlayRepeat;						//リピート再生
bool PlayShuffle;						//シャッフル再生
bool ListShuffled;						//シャッフルされた

TStringList *PopMenuList;				//ExeMenuFile コマンド用メニュー項目
TStringList *ToolBtnList;				//ツールボタン定義リスト
TStringList *ToolBtnListV;
TStringList *ToolBtnListI;

//---------------------------------------------------------------------------
//起動時オプション
int  IniPathMode[MAX_FILELIST];			//パスモード
UnicodeString InitialPath[MAX_FILELIST];
UnicodeString InitialMask[MAX_FILELIST];
int  IniSortMode[MAX_FILELIST];
int  IniWorkMode;						//ワークリスト 0:前回/ 1:指定
UnicodeString HomeWorkList;
int  IniWinMode;						//サイズモード 0:前回/ 1:指定
int  IniWinLeft, IniWinTop, IniWinWidth, IniWinHeight;
bool FixWinPos;							//ウィンドウサイズを固定
bool IniPathToTab1;						//初期パスをタブ1に設定
bool IniTabHomeAll;						//初期パス以外のタブをホームに戻す
bool ShowSplash;						//スプラッシュを表示

//ファイルリストのソートモード
int  SortMode[MAX_FILELIST];			//0:名前/ 1:拡張子/ 2:更新日時/ 3:サイズ/ 4:属性
int  DirSortMode[MAX_FILELIST];			//0:ファイルと同じ/ 1:名前/ 2:更新日時/ 3:サイズ/ 4:属性/ 5:ディレクトリを区別しない

//ファイルリストのソート順
bool FlOdrNatural[MAX_FILELIST];		//自然順
bool FlOdrDscName[MAX_FILELIST];		//降順
bool FlOdrSmall[MAX_FILELIST];			//小さい順
bool FlOdrOld[MAX_FILELIST];			//古い順
bool FlOdrDscAttr[MAX_FILELIST];		//属性降順
bool FlOdrDscPath[MAX_FILELIST];		//場所降順

bool SortBoth;					//左右とも変更

//比較関数のソート順
bool NaturalOrder = true;		//自然順
bool DscNameOrder = false;		//名前降順
bool SmallOrder   = false;		//小さい順
bool OldOrder	  = false;		//古い順
bool DscAttrOrder = false;		//属性降順
bool DscPathOrder = false;		//場所降順

TStringList *OptionList;		//オプション
TStringList *KeyFuncList;		//キー割り当て
TStringList *FKeyLabelList;		//ファンクションキー名
TStringList *HotKeyList;		//ホットキー
TStringList *DirStack;			//ディレクトリスタック
TStringList *PathMaskList;		//パスマスクリスト
TStringList *RegDirList;		//登録ディレクトリ
TStringList *ProtectDirList;	//削除制限ディレクトリ
TStringList *ColorList;			//配色
TStringList *ExtColList;		//拡張子別配色
TStringList *AssociateList;		//関連付け
TStringList *OpenStdCmdList;	//OpenStandard のコマンド関連付け
TStringList *EtcEditorList;		//その他のエディタリスト
TStringList *ExtToolList;		//外部ツール
TStringList *ExtMenuList;		//追加メニュー
TStringList *CommandList;		//コマンドリスト
TStringList *CmdSetList;		//コマンド一覧リスト
TStringList *TaskCmdList;		//タスクコマンド名リスト
TStringList *DriveList;			//変化監視用ドライブリスト
TStringList *NetDriveList;		//現在有効なネットドライブリスト
TStringList *AllDirHistory;		//全体のディレクトリ履歴
TStringList *TextViewHistory;	//テキスト閲覧履歴
TStringList *TextEditHistory;	//テキスト編集履歴
TStringList *WorkListHistory;	//ワークリスト履歴
TStringList *InputDirHistory;	//ディレクトリ入力の履歴
TStringList *InputCmdsHistory;	//コマンド入力の履歴
TStringList *InputCmdsHistoryV;
TStringList *InputCmdsHistoryI;
TStringList *IncSeaHistory;		//インクリメンタルサーチのキーワード履歴
TStringList *FilterHistory;		//フィルタのキーワード履歴
TStringList *WebSeaHistory;		//Web検索語の履歴
TStringList *LatLngHistory;		//緯度経度検索の履歴
TStringList *HeadlineList;		//見出しリスト (拡張子=正規表現パターン)
TStringList *ZoomRatioList;		//ズーム倍率リスト
TStringList *CnvCharList;		//禁止文字／ユーザ定義文字変換リスト
TStringList *RenCmdFileList;	//改名したコマンドファイルのリスト
TStringList *RedrawList;		//描画抑止用リスト
TStringList *CmdRequestList;	//コマンド要求リスト

//タスクからのログ書込バッファ (必ず LogRWLock で保護すること)
UnicodeString LogBufStr;
TMultiReadExclusiveWriteSynchronizer *LogRWLock;

TListBox     *LogWndListBox;	//ログウィンドウ用リストボックス
TStringList  *LogBufList;		//ログリストボックス(仮想)用バッファ
UsrScrollPanel *LogWndScrPanel;	//シンプルスクロールバー

int  LayoutMode;		//画面レイアウトモード
bool DivFileListUD;		//ファイルリストを上下に分割
bool DivDirInfUD;		//ディレクトリ情報を上下に分割

//フォント
TFont *ListFont;		//ファイルリスト等
TFont *FileInfFont;		//ファイル情報
TFont *TxtPrvFont;		//テキストプレビュー
TFont *LogFont;			//ログ
TFont *ViewerFont;		//テキストビュアー
TFont *GrepResFont;		//GREP結果リスト
TFont *DirInfFont;		//ディレクトリ情報
TFont *DrvInfFont;		//ドライブ情報
TFont *LstHdrFont;		//一覧のヘッダ
TFont *ViewHdrFont;		//ビュアー情報ヘッダ
TFont *GenListFont;		//一覧ダイアログ
TFont *HintFont;		//ヒント
TFont *SttBarFont;		//ステータスバー
TFont *CalcFont;		//電卓
TFont *DialogFont;		//ダイアログ
TFont *ToolBarFont;		//ツールバー
TFont *TabBarFont;		//タブバー
TFont *CharInfFont;		//文字情報
TStringList *FontList;

//カラー
TColor col_bgList;		//一覧の背景色
TColor col_bgList2;		//一覧の背景色(交互)
TColor col_fgList;		//一覧の文字色
TColor col_Splitter;	//分割境界線
TColor col_bgArc;		//仮想ディレクトリの背景色
TColor col_bgFind;		//検索結果リストの背景色
TColor col_bgWork;		//ワークリストの背景色
TColor col_bgFTP;		//FTPリモート側の背景色
TColor col_selItem;		//選択項目の背景色
TColor col_oppItem;		//反対パス選択項目の背景色
TColor col_fgSelItem;	//選択項目の文字色
TColor col_bgMark;		//マーク項目の背景色
TColor col_matchItem;	//インクリメンタルサーチでマッチ
TColor col_Cursor;		//ラインカーソルの色
TColor col_bgScrBar;	//シンプルスクロールバーの背景色
TColor col_bgScrKnob;	//シンプルスクロールノブの色
TColor col_frScrKnob;	//シンプルスクロールノブの輪郭色
TColor col_Folder;		//ディレクトリの文字色
TColor col_SymLink;		//シンボリックリンクの文字色
TColor col_Protect;		//削除制限ディレクトリの文字色
TColor col_ReadOnly;	//読み込み専用属性の文字色
TColor col_Hidden;		//隠し属性の文字色
TColor col_System;		//システム属性の文字色
TColor col_fgTagNam;	//タグ名の文字色
TColor col_InvItem;		//無効な項目の文字色
TColor col_bgTabBar;	//タブバーの背景色
TColor col_bgActTab;	//アクティブなタブの背景色
TColor col_bgInAcTab;	//非アクティブなタブの背景色
TColor col_frmTab;		//タブの輪郭色
TColor col_fgTab;		//タブの文字色
TColor col_bgListHdr;	//リストヘッダの背景色
TColor col_fgListHdr;	//リストヘッダの文字色
TColor col_bgDirInf;	//ディレクトリ情報の背景色
TColor col_fgDirInf;	//ディレクトリ情報の文字色
TColor col_bgDirRel;	//ディレクトリ関係の背景色
TColor col_fgDirRel;	//ディレクトリ関係の文字色
TColor col_bgDrvInf;	//ドライブ情報の背景色
TColor col_fgDrvInf;	//ドライブ情報の文字色
TColor col_bgInf;		//ファイル情報の背景色
TColor col_fgInf;		//ファイル情報の文字色
TColor col_fgInfNam;	//ファイル情報の項目名文字色
TColor col_bgTxtPrv;	//テキストプレビューの背景色
TColor col_fgTxtPrv;	//テキストプレビューの文字色
TColor col_bgLog;		//ログの背景色
TColor col_fgLog;		//ログの文字色
TColor col_bgTask;		//タスク状態表示の背景色
TColor col_fgPrgBar;	//タスク進捗バー色
TColor col_bgPrgBar;	//タスク進捗背景色
TColor col_Error;		//エラー/注意の文字色

TColor col_bgView;		//テキストビュアーの背景色
TColor col_fgView;		//テキストビュアーの文字色
TColor col_Margin;		//テキストビュアーの余白白
TColor col_bgRuler;		//ルーラの背景色
TColor col_fgRuler;		//ルーラの目盛色
TColor col_bgLineNo;	//行番号背景色
TColor col_LineNo;		//行番号文字色
TColor col_Mark;		//行マーク
TColor col_bdrLine;		//行番号の境界線
TColor col_bdrFold;		//折り返し境界線
TColor col_bdrFixed;	//固定長表示の縦罫線
TColor col_Comment;		//コメントの文字色
TColor col_Strings;		//文字列の文字色
TColor col_Reserved;	//予約語の文字色
TColor col_Symbol;		//シンボルの文字色
TColor col_Numeric;		//数値の文字色
TColor col_fgEmpBin1;	//バイナリ強調文字色1
TColor col_fgEmpBin2;	//バイナリ強調文字色2
TColor col_fgEmpBin3;	//バイナリ強調文字色3
TColor col_Headline;	//見出しの文字色
TColor col_URL;			//URLの文字色
TColor col_LocalLink;	//ローカファイルへのリンク
TColor col_fgEmp;		//強調文字色
TColor col_bgEmp;		//強調背景色
TColor col_Ruby;		//ルビ
TColor col_TAB;			//タブ表示色
TColor col_CR;			//改行表示色
TColor col_HR;			//罫線の色
TColor col_Ctrl;		//コントロールコード

TColor col_bgImage;		//画像の背景色
TColor col_bgDblPg;		//見開き表示の余白色
TColor col_bgWMF;		//メタファイルの背景色
TColor col_bdrThumb;	//サムネイルの境界線
TColor col_ThumbExif;	//サムネイルのEixf情報
TColor col_ImgGrid;		//画像分割グリッド
TColor col_OptFind;		//オプション設定の検索結果
TColor col_bgHint;		//ヒント表示の背景色
TColor col_fgHint;		//ヒント表示の文字色
TColor col_bgWarn;		//警告表示の背景色
TColor col_Invalid;		//無効な項目の背景色
TColor col_Illegal;		//不正な入力項目の背景色
TColor col_Tim1H;		//1時間以内のタイムスタンプ色
TColor col_Tim3H;		//3時間以内の〜
TColor col_Tim1D;		//1日以内の〜
TColor col_Tim3D;		//3日以内の〜
TColor col_Tim7D;		//7日以内の〜
TColor col_Tim1M;		//1ヶ月以内の〜
TColor col_Tim3M;		//3ヶ月以内の〜
TColor col_Tim6M;		//6ヶ月以内の〜
TColor col_Tim1Y;		//1年以内の〜
TColor col_Tim3Y;		//3年以内の〜
TColor col_TimOld;		//それより前の〜
TColor col_Size4G;		//4GB以上
TColor col_Size1G;		//1GB以上
TColor col_Size1M;		//1MB以上
TColor col_Size1K;		//1KB以上
TColor col_SizeLT;		//1KB未満
TColor col_Size0;		//0

TColor col_bgTlBar1;	//ツールバーのグラデーション開始色
TColor col_bgTlBar2;	//ツールバーのグラデーション終了色
TColor col_fgTlBar;		//ツールバーの文字色
TColor col_htTlBar;		//ツールバーホットトラッキング色

TColor col_bgSttBar;	//ステータスバーの背景色
TColor col_fgSttBar;	//ステータスバーの文字色
TColor col_bgInfHdr;	//情報ヘッダの背景色
TColor col_fgInfHdr;	//情報ヘッダの文字色

TColor col_ModalScr;	//モーダル表示効果色

TColor col_Teal = clTeal;

//背景画像
Graphics::TBitmap *BgImgBuff[MAX_BGIMAGE];
UnicodeString BgImgName[MAX_BGIMAGE];

int  BgImgMode;					//モード
int  BgImgSubMode;
int  BgColAlpha;				//背景色アルファ
bool BgImgGray;					//グレースケール化
bool BgImgHideScr;				//スクロール時に隠す
int  BgHideTime;				//復帰時間
bool BgImgTileIf;				//指定サイズ以下なら並べて表示
int  BgTileSize;

bool AlphaForm;
int  AlphaValue;

//外部ツール
UnicodeString TextEditor;		//テキストエディタ
UnicodeString TextEditorFrmt;	//パラメータ(Grep用)

UnicodeString ExtTxViewer;		//外部テキストビュアー
UnicodeString ExtTxViewerFrmt;

UnicodeString ImageEditor;		//イメージエディタ
UnicodeString FExtImgEidt;		//イメージエディタの対象拡張子
bool ImageEditSgl;				//イメージエディタをファイル毎に個別起動

UnicodeString FExtViewTab4;		//タブ4の拡張子

UnicodeString BinaryEditor;		//バイナリエディタ

//サウンド
UnicodeString SoundTaskFin;		//タスク終了時の通知音
UnicodeString SoundFindFin;		//検索終了時の通知音
UnicodeString SoundWarning;		//警告音
UnicodeString SoundWatch;		//ファイル監視

int  FlashCntWarning;			//警告の点滅回数
int  FlashTimeWarning;			//警告の点滅間隔
int  FlashCntTaskfin;			//タスク終了時の点滅回数
int  FlashTimeTaskfin;			//タスク終了時の点滅間隔

bool WarnPowerFail;				//電源切断警告
bool WarnDisconnect;			//インターネット接続切れ
bool WarnLowBattery;			//バッテリー低下
int  BatLowerLimit;

int  MaxLogLines;				//ログの最大行数
int  MsgHintTime;				//ヒントの表示時間
int  KeyHintTime;				//2ストロークヒントの待機時間
int  NotifyTaskTime;			//〜秒以上かかったタスクの終了時に鳴らす
int  CmpDelOwCnt;				//完全削除の上書き回数
int  ViewTxtInterLn;			//ビュアーの行間
int  ViewLeftMargin;			//ビュアーの左余白
int  ViewFoldWidth;				//折り返し幅(半角単位)
bool ViewFoldFitWin;			//折り返しをウィンドウ幅に合わせる
int  ViewFixedLimit;			//固定長表示の最大幅
int  ViewTxtLimitSize;			//テキストの最大読込サイズ
int  ViewBinLimitSize;			//バイナリの最大読込(or マップ)サイズ
int  ListWheelScrLn;			//ホイールによるスクロール行数(リスト)
int  ViewWheelScrLn;			//ホイールによるスクロール行数(ビュアー)
int  PlaySndLimitTime;			//サウンド再生タイム制限
bool ShowTextPreview;			//テキストプレビューを表示
int  PrvTxtInfLn;				//テキストプレビューの行数
bool ShowTailPreview;			//末尾プレビューを表示
int  PrvTxtTailLn;				//末尾プレビュー行数
int  PrvActTailLn;				//末尾プレビューを有効にする行数
bool LimitGetRarInf;			//RARのファイル情報取得をサイズで制限
int  GetInfLimitSize;			//RARのファイル情報取得の制限サイズ
int  WatchInterval;				//ディレクトリ監視間隔
int  LogInterval;				//ログの更新間隔
int  ListInterLn;				//ファイルリストの行間
int  CursorWidth;				//カーソル線の幅
int  CursorAlpha;				//カーソル行背景アルファ
int  SplitterWidth;				//境界線の幅
int  SplitterWidth2;
int  SizeFormatMode;
int  SizeDecDigits;				//サイズ表示における小数点以下の桁数
int  ListPercent;				//ファイルリスト幅の比率
int  ImgFrameMargin;			//フィット表示時の余白幅
int  ImgFitMaxZoom;				//フィット表示時の許容倍率
int  ImgGridHorzN;				//グリッドの横分割数
int  ImgGridVertN;				//グリッドの縦分割数
int  MinShowTime;				//最小表示タイム
int  ThumbnailSize;				//サムネイルのサイズ
int  ThumbBdrWidth;				//サムネイルの境界線幅
int  ThumbnailPos;				//サムネイルの表示位置
int  ImgDblMargin;				//見開き間隔

bool ShowImgSidebar;			//サイドバーを表示
bool ImgSidebarIsLeft;			//サイドバーは左

bool HasCallHotkey;				//呼び出しホットキーがあるか？

bool HideSizeTime;				//サイズと更新日時を隠す

//---------------------------------------------------------------------------
//イベント発生時に実行するコマンド
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
	{&OnAppStart,		_T("OnAppStart"),	_T("NyanFi 起動時")},
	{&OnAppClose,		_T("OnAppClose"),	_T("NyanFi 終了時")},
	{&OnMaximized,		_T("OnMaximized"),	_T("ウィンドウが最大化された時")},
	{&OnMinimized,		_T("OnMinimized"),	_T("ウィンドウが最小化された時")},
	{&OnRestored,		_T("OnRestored"),	_T("ウィンドウが元のサイズに戻った時")},
	{&OnCurChange,		_T("OnCurChange"),	_T("|カレントのディレクトリ変更(FL)")},
	{&OnTabChange,		_T("OnTabChange"),	_T("別のタブに移った時(FL)")},
	{&OnNewDrive,		_T("OnNewDrive"),	_T("追加ドライブが開かれた時(FL)")},
	{&OnArcOpend,		_T("OnArcOpend"),	_T("仮想ディレクトリを開いた直後(FL)")},
	{&OnArcClosed,		_T("OnArcClosed"),	_T("仮想ディレクトリを閉じた直後(FL)")},
	{&OnFindOpend,		_T("OnFindOpend"),	_T("検索結果リストが表示された直後(FL)")},
	{&OnFlCursor,		_T("OnFlCursor"),	_T("ファイルリストでカーソル移動直後(FL)")},
	{&OnFlExDClick,		_T("OnFlExDClick"),	_T("ファイルリストでファイル名主部以外をダブルクリック(FL)")},
	{&OnFlRClick,		_T("OnFlRClick"),	_T("ファイルリストで右クリック(FL)")},
	{&OnDragStart,		_T("OnDragStart"),	_T("ファイルリストからのドラッグ開始時(FL)")},
	{&OnDragEnd,		_T("OnDragEnd"),	_T("ファイルリストからのドラッグ完了時(FL)")},
	{&OnGrepOpen,		_T("OnGrepOpen"),	_T("文字列検索(GREP)を開く直前(FL,TV)")},
	{&OnGrepClosed,		_T("OnGrepClosed"),	_T("文字列検索(GREP)を閉じた直後(FL)")},
	{&OnWatchNotify,	_T("OnWatchNotify"),_T("WatchTailコマンドで追記検出")},
	{&OnFTPConnect,		_T("OnFTPConnect"),	_T("|FTPホストに接続した(FL)")},
	{&OnFTPDiscon,		_T("OnFTPDiscon"),	_T("FTPホストから切断した(FL)")},
	{&OnFlTbRClick,		_T("OnFlTbRClick"),	_T("|ファイラーのツールバーを右クリック(FL)")},
	{&OnTabDClick,		_T("OnTabDClick"),	_T("タブバーの空きスペースをダブルクリック(FL)")},
	{&OnTabRClick,		_T("OnTabRClick"),	_T("タブバーを右クリック(FL)")},
	{&OnDirDClick,		_T("OnDirDClick"),	_T("ディレクトリ情報をダブルクリック(FL)")},
	{&OnDirRClick,		_T("OnDirRClick"),	_T("ディレクトリ情報を右クリック(FL)")},
	{&OnRelDClick,		_T("OnRelDClick"),	_T("ディレクトリ関係をダブルクリック(FL)")},
	{&OnRelRClick,		_T("OnRelRClick"),	_T("ディレクトリ関係を右クリック(FL)")},
	{&OnHdrRClick,		_T("OnHdrRClick"),	_T("ファイルリストのヘッダを右クリック(FL)")},
	{&OnFScrRClick,		_T("OnFScrRClick"),	_T("ファイルリストのシンプルスクロールバーを右クリック(FL)")},
	{&OnDrvDClick,		_T("OnDrvDClick"),	_T("ドライブ情報をダブルクリック(FL)")},
	{&OnDrvRClick,		_T("OnDrvRClick"),	_T("ドライブ情報を右クリック(FL)")},
	{&OnSplDClick,		_T("OnSplDClick"),	_T("左右分割境界線をダブルクリック(FL)")},
	{&OnSplRClick,		_T("OnSplRClick"),	_T("左右分割境界線を右クリック(FL)")},
	{&OnImgDClick,		_T("OnImgDClick"),	_T("イメージプレビューをダブルクリック(FL)")},
	{&OnTxtDClick,		_T("OnTxtDClick"),	_T("テキストプレビューをダブルクリック(FL)")},
	{&OnFlIDClick,		_T("OnFlIDClick"),	_T("ファイル情報をダブルクリック(FL)")},
	{&OnTskDClick,		_T("OnTskDClick"),	_T("タスク表示をダブルクリック")},
	{&OnTskRClick,		_T("OnTskRClick"),	_T("タスク表示を右クリック")},
	{&OnLogDClick,		_T("OnLogDClick"),	_T("ログウィンドウをダブルクリック")},
	{&OnSttDClick,		_T("OnSttDClick"),	_T("ステータスバーをダブルクリック")},
	{&OnSttRClick,		_T("OnSttRClick"),	_T("ステータスバーを右クリック")},
	{&OnTimDClick,		_T("OnTimDClick"),	_T("時計パネルをダブルクリック")},
	{&OnTimRClick,		_T("OnTimRClick"),	_T("時計パネルを右クリック")},
	{&OnTvOpen,			_T("OnTvOpen"),		_T("|テキストビュアーを開く直前(FL)")},
	{&OnTvOpened,		_T("OnTvOpened"),	_T("テキストビュアーを開いた直後(TV)")},
	{&OnTvClose,		_T("OnTvClose"),	_T("テキストビュアーを閉じる直前(TV)")},
	{&OnTvClosed,		_T("OnTvClosed"),	_T("テキストビュアーを閉じた直後(FL)")},
	{&OnTvTbRClick,		_T("OnTvTbRClick"),	_T("テキストビュアーのツールバーを右クリック(TV)")},
	{&OnTvHRClick,		_T("OnTvHRClick"),	_T("テキストビュアーで情報ヘッダを右クリック(TV)")},
	{&OnIvOpened,		_T("OnIvOpened"),	_T("|イメージビュアーを開いた直後(IV)")},
	{&OnIvClosed,		_T("OnIvClosed"),	_T("イメージビュアーを閉じた直後(FL)")},
	{&OnFullScr,		_T("OnFullScr"),	_T("イメージビュアーで全画面表示にした時(IV)")},
	{&OnNormScr,		_T("OnNormScr"),	_T("イメージビュアーで通常表示に戻った時(IV)")},
	{&OnIvTbRClick,		_T("OnIvTbRClick"),	_T("イメージビュアーのツールバーを右クリック(IV)")},
	{&OnIvImgDClick,	_T("OnIvImgDClick"),_T("イメージビュアーで画像部分をダブルクリック(IV)")},
	{&OnIvImgRClick,	_T("OnIvImgRClick"),_T("イメージビュアーで画像表示部を右クリック(IV)")},
	{&OnIvMgnDClick,	_T("OnIvMgnDClick"),_T("イメージビュアーで余白部分をダブルクリック(IV)")},
	{&OnThmDClick,		_T("OnThmDClick"),	_T("イメージビュアーでサムネイルをダブルクリック(IV)")},
	{&OnThmRClick,		_T("OnThmRClick"),	_T("イメージビュアーでサムネイルを右クリック(IV)")},
	{&OnIvIDClick,		_T("OnIvIDClick"),	_T("イメージビュアーでファイル情報をダブルクリック(IV)")},
	{&OnIvSbRClick,		_T("OnIvSbRClick"),	_T("イメージビュアーでシークバーを右クリック(IV)")},
	{&OnDragStartI,		_T("OnDragStartI"),	_T("サムネイルからのドラッグ開始時(IV)")},
	{&OnDragEndI,		_T("OnDragEndI"),	_T("サムネイルからのドラッグ完了時(IV)")}
};

bool Timer_Enabled[MAX_TIMER_EVENT], Timer_NopAct[MAX_TIMER_EVENT];
int  Timer_StartCnt[MAX_TIMER_EVENT], Timer_TimeCnt[MAX_TIMER_EVENT], Timer_NopCnt[MAX_TIMER_EVENT];
int  Timer_RepeatN[MAX_TIMER_EVENT], Timer_RepeatCnt[MAX_TIMER_EVENT];
TStringList  *Timer_AlarmList[MAX_TIMER_EVENT];
UnicodeString Timer_Condition[MAX_TIMER_EVENT];
UnicodeString OnTimerEvent[MAX_TIMER_EVENT];

//---------------------------------------------------------------------------
//TStringList を作成し、GeneralList に登録(終了時に一括破棄)
//---------------------------------------------------------------------------
TStringList *CreStringList(int obj_type)
{
	TStringList *lp = new TStringList();
	GeneralList->AddObject(IntToStr(obj_type), (TObject*)lp);
	return lp;
}

//---------------------------------------------------------------------------
//初期化
//---------------------------------------------------------------------------
void InitializeGlobal()
{
	Randomize();

	UserModule->ExePath = ExePath;

	LibraryPath = cv_env_str("%APPDATA%\\Microsoft\\Windows\\Libraries\\");

	//バージョン
	unsigned mj, mi, bl;
	VersionNo = GetProductVersion(Application->ExeName, mj, mi, bl)? mj*100 + mi*10 + bl : 0;
	VersionStr.sprintf(_T("V%.2f"), VersionNo/100.0);
	CopyrightStr.USET_T(COPYRIGHT_INF);

	ScrScale = Screen->PixelsPerInch / 96.0;	//スケーリングを設定
	SIcoSize = ScaledInt(16);

	//ユーザ名
	_TCHAR szName[256];
	DWORD size = 256;
	if (::GetUserName(szName, &size)) UserName = szName;

	//管理者権限を持っているか?
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

	mute_Volume("GET");	//ミュート状態を取得

	//廃止セクション、キーの削除、修正
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

	//コマンドリストを作成
	CommandList = CreStringList();
	CommandList->Text =
		"F:AboutNyanFi=バージョン情報\n"
		"F:AddTab=タブを追加\n"
		"F:BackDirHist=ディレクトリ履歴を戻る\n"
		"F:Backup=反対パスにバックアップ\n"
		"F:BgImgMode=背景画像の表示モード設定\n"
		"F:BorderLeft=ファイルリストの境界を左に移動\n"
		"F:BorderRight=ファイルリストの境界を右に移動\n"
		"F:CalcDirSize=ディレクトリ容量を計算\n"
		"F:CalcDirSizeAll=全ディレクトリの容量を計算\n"
		"F:ChangeDir=ディレクトリを変更\n"
		"F:ChangeDrive=ドライブの変更\n"
		"F:ChangeOppDir=反対側のディレクトリを変更\n"
		"F:ChangeOppRegDir=反対側で登録ディレクトリを開く\n"
		"F:ChangeRegDir=登録ディレクトリを開く\n"
		"F:CheckUpdate=更新の確認\n"
		"F:ClearMask=選択マスク/パスマスクを解除\n"
		"F:Clone=反対パスにクローン作成\n"
		"F:CloneToCurr=カレントにクローン作成\n"
		"F:CommandPrompt=コマンドプロンプト\n"
		"F:CompareDlg=同名ファイルの比較ダイアログ\n"
		"F:CompareHash=ハッシュ値の比較\n"
		"F:CompleteDelete=完全削除\n"
		"F:ContextMenu=コンテキストメニューを表示\n"
		"F:ConvertDoc2Txt=バイナリ文書→テキスト変換\n"
		"F:ConvertHtm2Txt=HTML→テキスト変換\n"
		"F:ConvertImage=画像ファイルの変換\n"
		"F:ConvertTextEnc=文字コードの変換\n"
		"F:Copy=コピー\n"
		"F:CopyCmdName=コマンド名をクリップボードにコピー\n"
		"F:CopyDir=ディレクトリ構造のコピー\n"
		"F:CopyFileName=ファイル名をクリップボードにコピー\n"
		"F:CopyTo=指定ディレクトリにコピー\n"
		"F:CopyToClip=クリップボードにコピー\n"
		"F:CountLines=テキストファイルの行数をカウント\n"
		"F:CreateDir=ディレクトリの作成\n"
		"F:CreateDirsDlg=ディレクトリ一括作成ダイアログ\n"
		"F:CreateHardLink=ハードリンクの作成\n" 
		"F:CreateShortcut=ショートカットの作成\n"
		"F:CreateTestFile=テストファイルの作成\n"
		"F:CsrDirToOpp=カーソル位置のディレクトリを反対側に開く\n"
		"F:CurrFromOpp=カレントパスを反対パスに\n"
		"F:CurrToOpp=反対パスをカレントパスに\n"
		"F:CursorEnd=カーソルを最下行に移動\n"
		"F:CursorEndSel=選択しながらカーソルを最下行に移動\n"
		"F:CursorTop=カーソルを最上行に移動\n"
		"F:CursorTopSel=選択しながらカーソルを最上行に移動\n"
		"F:CutToClip=クリップボードに切り取り\n"
		"F:DateSelect=指定した日付条件に合うファイルを選択\n"
		"F:DelJpgExif=Exif情報の削除\n"
		"F:DelSelMask=選択マスクから項目を削除\n"
		"F:DelTab=タブを削除\n"
		"F:DirHistory=ディレクトリ履歴を表示\n"
		"F:DirStack=ディレクトリ・スタックを表示\n"
		"F:DistributionDlg=振り分けダイアログ\n"
		"F:DotNyanDlg=.nyanfi ファイルの設定\n"
		"F:DriveList=ドライブ一覧を表示\n"
		"F:DriveGraph=ドライブ使用率推移を表示\n"
		"F:EditIniFile=INIファイルの編集\n"
		"F:Eject=CD/DVDドライブのトレイを開く\n"
		"F:EjectDrive=ドライブの取り外し\n"
		"F:EmptyTrash=ごみ箱を空にする\n"
		"F:EqualListWidth=左右のリスト幅を均等に\n"
		"F:ExeCommandLine=コマンドラインの実行\n"
		"F:ExeExtMenu=追加メニューの実行\n"
		"F:ExeExtTool=外部ツールの実行\n"
		"F:Exit=NyanFiの終了\n"
		"F:ExitDupl=二重起動されたNyanFiを終了\n"
		"F:ExPopupMenu=拡張ポップアップメニュー\n"
		"F:ExtractChmSrc=CHMからソースを抽出\n"
		"F:ExtractGifBmp=アニメGIFからビットマップを抽出\n"
		"F:ExtractIcon=アイコンを抽出\n"
		"F:ExtractMp3Img=MP3の埋め込み画像を抽出\n"
		"F:FileExtList=拡張子別一覧\n"
		"F:FileListOnly=ファイルリストのみを表示\n"
		"F:FileRun=ファイル名を指定して実行\n"
		"F:Filter=ファイルリストの絞り込み\n"
		"F:FindDirDlg=ディレクトリ名検索ダイアログ\n"
		"F:FindFileDirDlg=ファイル/ディレクトリ名検索ダイアログ\n"
		"F:FindFileDlg=ファイル名検索ダイアログ\n"
		"F:FindHardLink=ハードリンクを列挙\n"
		"F:FindMark=マーク項目を検索\n"
		"F:FindTag=タグ検索\n"
		"F:FTPChmod=パーミッションの設定\n"
		"F:FTPConnect=FTPホストに接続\n"
		"F:FTPDisconnect=FTPホストから切断\n"
		"F:ForwardDirHist=ディレクトリ履歴を進む\n"
		"F:GetHash=ファイルのハッシュ値を取得\n"
		"F:HideSizeTime=サイズと更新日時を隠す\n"
		"F:ImageViewer=イメージビュアーで開く\n"
		"F:InputDir=入力したディレクトリに変更\n"
		"F:InputPathMask=パスマスクを入力\n"
		"F:InsSeparator=ワークリストにセパレータを挿入\n"
		"F:ItemTmpDown=項目を一時的に1つ下に移動\n"
		"F:ItemTmpUp=項目を一時的に1つ上に移動\n"
		"F:JoinText=テキストファイルの結合\n"
		"F:JumpTo=指定したファイル位置へ\n"
		"F:Library=ライブラリへ\n"
		"F:LinkToOpp=リンク先を反対側に開く\n"
		"F:ListArchive=アーカイブの内容一覧\n"
		"F:ListClipboard=クリップボードを一覧で表示\n"
		"F:ListDuration=ファイル再生時間の一覧\n"
		"F:ListExpFunc=エクスポート関数一覧\n"
		"F:ListFileName=ファイル名を一覧で表示\n"
		"F:ListNyanFi=NyanFi についての情報をログに表示\n"
		"F:ListText=テキストファイルを一覧で表示\n"
		"F:ListTail=テキストファイルの末尾を一覧で表示\n"
		"F:LockTextPreview=テキストプレビューをロック/解除\n"
		"F:LogFileInfo=ファイル情報をログに出力\n"
		"F:MarkMask=マーク項目だけを残して他を隠す\n"
		"F:MaskFind=指定マスクにマッチする項目を検索\n"
		"F:MatchSelect=指定文字列を含むファイルを選択\n"
		"F:MonitorOff=ディスプレイの電源を切る\n"
		"F:Move=移動\n"
		"F:MoveTab=タブの位置を移動\n"
		"F:MoveTo=指定ディレクトリへ移動\n"
		"F:MuteVolume=音量ミュート\n"
		"F:NameFromClip=ファイル名をクリップボードの内容に\n"
		"F:NameToLower=ファイル名の小文字化\n"
		"F:NameToUpper=ファイル名の大文字化\n"
		"F:NetConnect=ネットワークドライブの割り当て\n"
		"F:NetDisconnect=ネットワークドライブの切断\n"
		"F:NewTextFile=新規テキストファイルの作成\n"
		"F:NextDrive=次のドライブへ\n"
		"F:NextTab=次のタブへ\n"
		"F:OpenByExp=エクスプローラで開く\n"
		"F:OpenCtrlPanel=コントロールパネルを開く\n"
		"F:OpenStandard=標準の Enter キー動作\n"
		"F:OpenTrash=ごみ箱を開く\n"
		"F:Pack=反対パスにアーカイブ作成\n"
		"F:PackToCurr=カレントパスにアーカイブ作成\n"
		"F:Paste=貼り付け\n"
		"F:PathMaskDlg=パスマスクダイアログ\n"
		"F:PlayList=プレイリストを作って再生\n"
		"F:PopDir=ディレクトリをポップ\n"
		"F:PopupTab=タブ選択メニューを表示\n"
		"F:PowerOff=Windowsを終了\n"
		"F:PrevDrive=前のドライブへ\n"
		"F:PrevTab=前のタブへ\n"
		"F:PushDir=ディレクトリをプッシュ\n"
		"F:Reboot=Windowsを再起動\n"
		"F:RegDirDlg=登録ディレクトリダイアログ\n"
		"F:RegDirPopup=登録ディレクトリ・ポップアップメニュー\n"
		"F:RegSyncDlg=同期コピーの設定\n"
		"F:ReloadList=ファイルリストを最新の情報に更新\n"
		"F:RenameDlg=名前等の変更\n"
		"F:Restart=NyanFiの再起動\n"
		"F:ReturnList=ファイルリスト表示に戻る\n"
		"F:ScrollDownLog=ログを下にスクロール\n"
		"F:ScrollDownText=テキストプレビューを下にスクロール\n"
		"F:ScrollUpLog=ログを上にスクロール\n"
		"F:ScrollUpText=テキストプレビューを上にスクロール\n"
		"F:SelAllItem=すべての項目を選択/解除\n"
		"F:SelectUp=選択/解除後、カーソルを上に移動\n"
		"F:SelEmptyDir=空のディレクトリだけを選択\n"
		"F:SelMask=選択中の項目だけを残して他を隠す\n"
		"F:SelOnlyCur=カレント側だけにあるファイルを選択\n"
		"F:SelReverseAll=すべての項目の選択状態を反転\n"
		"F:SetAlias=ワークリストの項目にエイリアスを設定\n"
		"F:SetArcTime=アーカイブのタイムスタンプを最新に合わせる\n"
		"F:SetDirTime=ディレクトリのタイムスタンプを最新に合わせる\n"
		"F:SetExifTime=タイムスタンプをExif撮影日時に設定\n"
		"F:SetPathMask=パスマスクを設定\n"
		"F:SetSttBarFmt=ステータスバー書式を設定\n"
		"F:SetSubSize=サブウィンドウのサイズを設定\n"
		"F:ShareList=共有フォルダ一覧\n"
		"F:ShowByteSize=ファイルサイズをバイト単位で表示\n"
		"F:ShowHideAtr=隠しファイルを表示\n"
		"F:ShowIcon=アイコンの表示/非表示\n"
		"F:ShowPreview=イメージプレビューの表示/非表示\n"
		"F:ShowProperty=ファイル情報の表示/非表示\n"
		"F:ShowSystemAtr=システムファイルを表示\n"
		"F:ShowTabBar=タブバーの表示/非表示\n"
		"F:SoloTab=他のタブをすべて削除\n"
		"F:SubDirList=サブディレクトリ一覧\n"
		"F:SwapLR=左右のファイルリストを入れ替える\n"
		"F:SwapName=名前を入れ替える\n"
		"F:SyncLR=左右ディレクトリの同期変更を有効/解除\n"
		"F:TabDlg=タブの設定ダイアログ\n"
		"F:TabHome=タブのホームへ\n"
		"F:TestArchive=アーカイブの正当性を検査\n"
		"F:TextViewer=テキストビュアーで開く\n"
		"F:ToExViewer=別ウィンドウのテキストビュアーへ\n"
		"F:ToLog=ログウィンドウへ\n"
		"F:ToNextOnRight=右ファイルリストで次のNyanFiへ\n"
		"F:ToOpposite=反対側のファイルリストへ\n"
		"F:ToOppSameItem=カーソル位置と同名の反対パス項目へ\n"
		"F:ToParent=親ディレクトリへ\n"
		"F:ToParentOnLeft=左ファイルリストで親ディレクトリへ\n"
		"F:ToParentOnRight=右ファイルリストで親ディレクトリへ\n"
		"F:ToPrevOnLeft=左ファイルリストで前のNyanFiへ\n"
		"F:ToRoot=ルートディレクトリへ\n"
		"F:ToTab=指定番号/キャプションのタブへ\n"
		"F:ToText=テキストプレビューへ\n"
		"F:TrimTagData=タグデータの整理\n"
		"F:UndoRename=直前の改名を元に戻す\n"
		"F:UnPack=反対パスに解凍\n"
		"F:UnPackToCurr=カレントパスに解凍\n"
		"F:UpdateFromArc=アーカイブから更新\n"
		"F:ViewIniFile=INIファイルの閲覧\n"
		"F:ViewTail=テキストファイルの末尾を閲覧\n"
		"F:WatchTail=テキストファイルの追加更新を監視\n"
		"F:WidenCurList=カレント側リストの幅を広げる\n"
		"F:WinMaximize=ウィンドウの最大化\n"
		"F:WinMinimize=ウィンドウの最小化\n"
		"F:WinNormal=ウィンドウを元のサイズに戻す\n"
		"F:WorkItemDown=ワークリストの項目を1つ下に移動\n"
		"F:WorkItemUP=ワークリストの項目を1つ上に移動\n"
		"F:XmlViewer=XMLビュアー\n"
		"FI:AddTag=項目にタグを追加\n"
		"FI:DelTag=項目のタグを削除\n"
		"FI:HomeWorkList=ホームワークリストを開く\n"
		"FI:ListFileInfo=ファイル情報をダイアログ表示\n"
		"FI:LoadBgImage=背景画像を読み込む\n"
		"FI:LoadWorkList=ワークリストをファイルから読み込む\n"
		"FI:MaskSelect=指定マスクにマッチするファイルを選択\n"
		"FI:NewWorkList=新規ワークリストの作成\n"
		"FI:NextMark=次のマーク項目へ\n"
		"FI:PrevMark=前のマーク項目へ\n"
		"FI:NextSameName=ファイル名主部が同じ次ファイルへ\n"
		"FI:OpenByApp=独自の関連付けで開く\n"
		"FI:OpenByWin=Windowsの関連付けで開く\n"
		"FI:SaveAsWorkList=ワークリストに名前を付けて保存\n"
		"FI:SaveWorkList=ワークリストを上書き保存\n"
		"FI:SelAllFile=すべてのファイルを選択/解除\n"
		"FI:SelMark=マーク項目を選択\n"
		"FI:SelReverse=ファイルの選択状態を反転\n"
		"FI:SelSameExt=拡張子が同じファイルを選択\n"
		"FI:SelSameName=ファイル名主部が同じファイルを選択\n"
		"FI:SelWorkItem=ワークリストの登録項目を選択\n"
		"FI:SetTag=項目にタグを設定\n"
		"FI:ShowFileInfo=ファイル情報を強制的に表示\n"
		"FI:SortDlg=ソートダイアログ\n"
		"FI:SubViewer=サブビュアーの表示/非表示\n"
		"FI:UseTrash=削除にごみ箱を使う/使わない\n"
		"FI:WorkList=ワークリスト\n"
		"FL:CancelAllTask=すべてのタスクを中断\n"
		"FL:ClearLog=ログをクリア\n"
		"FL:PauseAllTask=すべてのタスクを一旦停止/再開\n"
		"FL:Suspend=タスク予約項目の実行を保留/解除\n"
		"FL:TaskMan=タスクマネージャ\n"
		"FL:ToLeft=左ファイルリストへ\n"
		"FL:ToRight=右ファイルリストへ\n"
		"FL:ViewLog=ログをテキストビュアーで表示\n"
		"FS:ClearAll=すべての選択状態を解除\n"
		"FSI:Select=選択/解除\n"
		"FSVIL:HelpContents=ヘルプの目次/索引を表示\n"
		"FSVIL:KeyList=キー割り当て一覧を表示\n"
		"FSVIL:OptionDlg=オプション設定\n"
		"FV:BinaryEdit=ファイルのバイナリ編集\n"
		"FV:CursorDownSel=選択しながらカーソルを下に移動\n"
		"FV:CursorUpSel=選択しながらカーソルを上に移動\n"
		"FV:EditHighlight=構文強調表示定義ファイルの編集\n"
		"FV:EditHistory=最近編集したファイル一覧を表示\n"
		"FV:Grep=文字列検索(GREP)\n"
		"FV:HtmlToText=テキストビュアーでHTML→テキスト変換表示\n"
		"FV:IncSearch=インクリメンタルサーチ\n"
		"FV:FindTagName=tags からタグ名検索\n"
		"FV:FixedLen=テキストビュアーでCSV/TSVを固定長表示\n"
		"FV:PageDownSel=選択しながら1ページ下に移動\n"
		"FV:PageUpSel=選択しながら1ページ上に移動\n"
		"FV:RegExChecker=正規表現チェッカー\n"
		"FV:SetColor=テキストビュアーの配色\n"
		"FV:SetFontSize=フォントサイズを変更\n"
		"FV:SetMargin=テキストビュアーの左側余白を設定\n"
		"FV:SetTab=テキストビュアーのタブストップ幅を設定\n"
		"FV:SetWidth=テキストビュアーの折り返し幅を設定\n"
		"FV:ShowCR=テキストビュアーで改行を表示/非表示\n"
		"FV:ShowLineNo=テキストビュアーの行番号を表示/非表示\n"
		"FV:ShowRuby=テキストビュアーでルビを表示/非表示\n"
		"FV:ShowRuler=テキストビュアーのルーラを表示/非表示\n"
		"FV:ShowTAB=テキストビュアーでタブを表示/非表示\n"
		"FV:ViewHistory=最近閲覧したファイル一覧を表示\n"
		"FV:WebSearch=Webで検索\n"
		"FV:ZoomReset=ズームを解除\n"
		"FVL:CursorDown=カーソルを下に移動\n"
		"FVL:CursorUp=カーソルを上に移動\n"
		"FVI:AlphaBlend=メインウィンドウを透過表示\n"
		"FVI:AppList=アプリケーション一覧\n"
		"FVI:Calculator=電卓\n"
		"FVI:ClearMark=すべてのマークを解除\n"
		"FVI:Close=閉じる\n"
		"FVI:CopyFileInfo=ファイル情報をクリップボードにコピー\n"
		"FVI:CmdFileList=コマンドファイル一覧\n"
		"FVI:DebugCmdFile=コマンドファイルのデバッグ実行\n"
		"FVI:Delete=削除\n"
		"FVI:Duplicate=NyanFiの二重起動\n"
		"FVI:ExeCommands=指定したコマンドを実行\n"
		"FVI:ExeMenuFile=メニューファイルの実行\n"
		"FVI:ExeToolBtn=ツールボタンの実行\n"
		"FVI:FileEdit=ファイルの編集\n"
		"FVI:InputCommands=入力したコマンドを実行\n"
		"FVI:ListLog=ログを一覧で表示\n"
		"FVI:Mark=マーク/解除\n"
		"FVI:MarkList=マーク一覧を表示\n"
		"FVI:MenuBar=メニューバーの表示/非表示\n"
		"FVI:NextNyanFi=次のNyanFiをアクティブに\n"
		"FVI:PopupMainMenu=メインメニューをポップアップ表示\n"
		"FVI:PrevNyanFi=前のNyanFiをアクティブに\n"
		"FVI:PropertyDlg=プロパティダイアログを表示\n"
		"FVI:ScrollDown=下にスクロール\n"
		"FVI:ScrollUp=上にスクロール\n"
		"FVI:ShowFKeyBar=ファンクションキーバーの表示/非表示\n"
		"FVI:ShowStatusBar=ステータスバーの表示/非表示\n"
		"FVI:ShowToolBar=ツールバーを表示\n"
		"FVI:ToolBarDlg=ツールバーの設定\n"
		"FVI:WebMap=画像のGPS情報や指定地点の地図を開く\n"
		"FVI:WinPos=ウィンドウの四辺を設定\n"
		"FVIL:PageDown=1ページ下に移動\n"
		"FVIL:PageUp=1ページ上に移動\n"
		"FVI:ZoomIn=ズームイン\n"
		"FVI:ZoomOut=ズームアウト\n"
		"I:ColorPicker=カラーピッカー\n"
		"I:DoublePage=見開き表示\n"
		"I:EndFile=最後のファイルに移動\n"
		"I:EqualSize=等倍表示\n"
		"I:FittedSize=画面フィット表示\n"
		"I:FlipHorz=左右反転\n"
		"I:FlipVert=上下反転\n"
		"I:FullScreen=全画面表示\n"
		"I:GifViewer=GIFビュアーの表示/非表示\n"
		"I:GrayScale=グレースケール表示\n"
		"I:Histogram=ヒストグラムの表示/非表示\n"
		"I:JumpIndex=指定したインデックスに移動\n"
		"I:Loupe=ルーペの表示/非表示\n"
		"I:NextPage=次ページに移動\n"
		"I:PageBind=見開き表示の綴じ方向を設定\n"
		"I:PrevPage=前ページに移動\n"
		"I:Print=画像の印刷\n"
		"I:RotateLeft=左に90度回転\n"
		"I:RotateRight=右に90度回転\n"
		"I:ScrollLeft=左にスクロール\n"
		"I:ScrollRight=右にスクロール\n"
		"I:SendToWorkList=ワークリストに送る\n"
		"I:ShowGrid=画像分割グリッドの表示/非表示\n"
		"I:ShowSeekBar=シークバーの表示/非表示\n"
		"I:Sidebar=サイドバーの表示/非表示\n"
		"I:Thumbnail=サムネイルの表示/非表示\n"
		"I:ThumbnailEx=サムネイルの全面表示/通常表示\n"
		"I:TopFile=先頭ファイルに移動\n"
		"I:WarnHighlight=白飛び警告\n"
		"S:ClearIncKeyword=キーワードをクリア\n"
		"S:IncMatchSelect=マッチする項目をすべて選択\n"
		"S:IncSearchDown=マッチする項目を下方向検索\n"
		"S:IncSearchExit=インクリメンタルサーチから抜ける\n"
		"S:IncSearchUp=マッチする項目を上方向検索\n"
		"S:KeywordHistory=キーワード履歴を参照\n"
		"S:MigemoMode=Migomoモードの切り換え\n"
		"S:NormalMode=通常のサーチモードに戻る\n"
		"S:SelectDown=選択/解除後、下方向に検索\n"
		"V:BackViewHist=テキストビュアーの履歴を戻る\n"
		"V:BitmapView=ビットマップビューの表示/非表示\n"
		"V:BoxSelMode=箱形選択モードの開始/解除\n"
		"V:ChangeCodePage=文字コード変更\n"
		"V:ChangeViewMode=テキスト/バイナリ表示の切り換え\n"
		"V:CharInfo=文字情報の表示/非表示\n"
		"V:CsvRecord=CSV/TSVレコードの表示/非表示\n"
		"V:CursorLeft=カーソルを左に移動\n"
		"V:CursorLeftSel=選択しながらカーソルを左に移動\n"
		"V:CursorRight=カーソルを右に移動\n"
		"V:CursorRightSel=選択しながらカーソルを右に移動\n"
		"V:ExportCsv=CSV/TSVエクスポート\n"
		"V:FindDown=下方向に再検索\n"
		"V:FindLinkDown=リンク先を下方向に検索\n"
		"V:FindLinkUp=リンク先を上方向に検索\n"
		"V:FindMarkDown=マークを下方向に検索\n"
		"V:FindMarkUp=マークを上方向に検索\n"
		"V:FindSelDown=選択文字列を下方向に検索\n"
		"V:FindSelUp=選択文字列を上方向に検索\n"
		"V:FindText=文字列検索\n"
		"V:FindUp=上方向に再検索\n"
		"V:FunctionList=関数一覧を表示\n"
		"V:HelpCurWord=カーソル位置の単語を指定ヘルプで検索\n"
		"V:Highlight=マッチ語を強調表示\n"
		"V:Inspector=インスペクタの表示/非表示\n"
		"V:ImgPreview=イメージプレビュー\n"
		"V:JumpLine=指定行番号に移動\n"
		"V:LineEnd=行末に移動\n"
		"V:LineEndSel=選択しながら行末に移動\n"
		"V:LineTop=行頭に移動\n"
		"V:LineTopSel=選択しながら行頭に移動\n"
		"V:OpenURL=URLを開く\n"
		"V:ReloadFile=ファイルの再読み込み\n"
		"V:SaveDump=ダンプリストをファイルに保存\n"
		"V:ScrollCursorDown=スクロールしながら下に移動\n"
		"V:ScrollCursorUp=スクロールしながら上に移動\n"
		"V:SearchPair=対応する括弧を検索\n"
		"V:SelCurWord=カーソル位置の単語を選択\n"
		"V:SelLine=カーソル行を選択\n"
		"V:SelLineCR=カーソル行全体を改行単位で選択\n"
		"V:SelectAll=すべて選択\n"
		"V:SelectFile=表示中ファイルを選択/解除\n"
		"V:SelectMode=選択モードの開始/解除\n"
		"V:SetTopAddr=先頭アドレスを設定\n"
		"V:SetUserDefStr=ユーザ定義文字列を設定\n"
		"V:Sort=テキスト全体を改行単位でソート\n"
		"V:SwitchSameName=ファイル名主部が同じ次のファイルに切り換え\n"
		"V:SwitchSrcHdr=ヘッダ/ソースファイルの切り換え\n"
		"V:TagJump=エディタでタグジャンプ\n"
		"V:TagJumpDirect=エディタでダイレクトタグジャンプ\n"
		"V:TagView=ビュアーでタグジャンプ\n"
		"V:TagViewDirect=ビュアーでダイレクトタグジャンプ\n"
		"V:TextEnd=最後尾に移動\n"
		"V:TextEndSel=選択しながら最後尾に移動\n"
		"V:TextTop=先頭に移動\n"
		"V:TextTopSel=選択しながら先頭に移動\n"
		"V:UserDefList=ユーザ定義文字列一覧を表示\n"
		"V:WordLeft=前の単語に移動\n"
		"V:WordRight=次の単語に移動\n"
		"VIL:ClipCopy=クリップボードにコピー\n"
		"VI:NextFile=次のファイルを表示\n"
		"VI:PrevFile=前のファイルを表示\n"
		"VL:NextErr=次のエラー位置へ\n"
		"VL:PrevErr=前のエラー位置へ\n"
		"InitialSearch=頭文字サーチ\n";

	//コマンド一覧リストを設定
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

	//ズーム倍率リスト
	ZoomRatioList = CreStringList();
	ZoomRatioList->Text = "10\n25\n50\n75\n100\n150\n200\n300\n400\n";

	//タスクコマンド名リストを作成
	TaskCmdList = CreStringList();
	TaskCmdList->Text =
		"CPY=コピー\n"
		"MOV=移動\n"
		"DEL=削除\n"
		"CPYDIR=ディレクトリのコピー\n"
		"CMPDEL=完全削除\n"
		"CVIMG=画像変換\n"
		"DLEXIF=Exif削除\n"
		"EXTCHM=抽出\n"
		"BACKUP=バックアップ\n"
		"DTIME=タイム設定\n";

	//----------------------------------------------
	//オプションのキーと変数との対応を登録して読込
	//----------------------------------------------
	OptionList = CreStringList();

	struct {
		const _TCHAR  *def;	//キー名=デフォルト
		TObject *vp;		//変数へのポインタ
	} opt_def_list[] = {
		//[Option]
		//String (デフォルト値を引用符で囲んで指定)
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
		{_T("HtmHdrStr=\"■;●;◆;□;○;◇\""),		(TObject*)&HtmHdrStr},
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
		{_T("MarkImgMemo=\"しおり\""),				(TObject*)&MarkImgMemo},
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

		//Integer  (デフォルト値を引用符で囲まずに指定)
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

		//Bool (デフォルト値を true または false で指定)
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
		{_T("G:RepCrStr=\" ／ \""),			(TObject*)&GrepRepCrStr},
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

		//セクション (prefix = S:)
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

		//リスト	(prefix = L:)	最大項目数,引用符を外す
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
	//読み込み
	LoadOptions();

	//デフォルトのキー設定
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

	//デフォルトのパスマスク
	if (PathMaskList->Count==0) {
		PathMaskList->Text =
			"A,\"マスクを解除\",\"*\"\n"
			"X,\"実行ファイル\",\"*.exe;*.com;*.bat;*.cmd\"\n";
	}

	//禁止文字のデフォルト
	if (CnvCharList->Count<9) {
		CnvCharList->Text =
			"\\=￥\n"
			"/=／\n"
			":=：\n"
			"*=＊\n"
			"?=？\n"
			"\"=”\n"
			"<=＜\n"
			">=＞\n"
			"|=｜\n";
	}

	//----------------------------------
	//その他の初期化
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

	//ドライブ容量ログの初期化
	DriveLogName = UAPP_T(ExePath, DRVLOG_FILE);
	if (file_exists(DriveLogName) && load_text_ex(DriveLogName, DriveLogList)!=0)
		update_DriveLog(false);
	else
		DriveLogName = EmptyStr;

	//----------------------------------
	//HtmlHelp の初期化
	//----------------------------------
	Application->HelpFile = ChangeFileExt(Application->ExeName, ".chm");
	hHHctrl = ::LoadLibrary(_T("hhctrl.ocx"));
	if (hHHctrl) {
		lpfHtmlHelp = (FUNC_HtmlHelp)::GetProcAddress(hHHctrl, "HtmlHelpW");
		if (lpfHtmlHelp) lpfHtmlHelp(NULL, NULL, HH_INITIALIZE, (DWORD)&Cookie);
	}

	//非公開API
	HINSTANCE hGdi32 = ::LoadLibrary(_T("gdi32.dll"));
	if (hGdi32) {
		lpGetFontResourceInfo = (FUNC_GetFontResourceInfo)::GetProcAddress(hGdi32, "GetFontResourceInfoW");
	}

	WorkListChanged = WorkListFiltered = rqWorkListDirInf = false;
}

//---------------------------------------------------------------------------
//後片付け
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
//タイマー設定の更新
//---------------------------------------------------------------------------
void SetupTimer()
{
	for (int i=0; i<MAX_TIMER_EVENT; i++) {
		Timer_Enabled[i] = Timer_NopAct[i] = false;
		if (Timer_Condition[i].IsEmpty()) continue;
		TStringDynArray prm_lst = get_csv_array(Timer_Condition[i], MAX_TIMER_ALARM);
		if (prm_lst.Length==0) continue;
		//時刻
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
		//間隔、無操作
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
//無操作状態の解除
//---------------------------------------------------------------------------
void ClearNopStt()
{
	LastOpCount = GetTickCount();
	for (int i=0; i<MAX_TIMER_EVENT; i++) Timer_NopAct[i] = false;
}

//---------------------------------------------------------------------------
//オプション設定の読み込み
//---------------------------------------------------------------------------
void LoadOptions()
{
	UnicodeString sct, tmp;

	//登録オプションの読み込み
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

	//タブリストの初期化
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

	//全体ディレクトリ履歴
	if (ExtSaveDirHist) {
		std::unique_ptr<UsrIniFile> hst_file(new UsrIniFile(ExePath + DIR_HIST_FILE));
		hst_file->LoadListItems("AllDirHistory", AllDirHistory, 0, false);
	}
	else {
		IniFile->LoadListItems("AllDirHistory", AllDirHistory, 100, false);
	}

	//その他のオプションの読み込み
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

	//イベント
	for (int i=0; i<MAX_EVENT_CMD; i++)
		*(EventCmdList[i].sp) = IniFile->ReadString("Event", EventCmdList[i].key);

	//タイマーイベント
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
//オプション設定の保存
//---------------------------------------------------------------------------
void SaveOptions()
{
	//ディレクトリ履歴の進む部分を削除
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
	//登録オプション
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

	//タブリスト
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

	//全体ディレクトリ履歴
	if (ExtSaveDirHist) {
		std::unique_ptr<TStringList> fbuf(new TStringList());
		UnicodeString s;
		fbuf->Add("[AllDirHistory]");
		for (int i=0; i<AllDirHistory->Count; i++) {
			fbuf->Add(s.sprintf(_T("Item%u=%s"), i + 1, AllDirHistory->Strings[i].c_str()));
		}
		//保存
		UnicodeString fnam = UAPP_T(ExePath, DIR_HIST_FILE);
		UnicodeString msg  = make_LogHdr(_T("SAVE"), fnam);
		if (saveto_TextUTF8(fnam, fbuf.get())) {
			IniFile->EraseSection("AllDirHistory");
		}
		else {
			UnicodeString errmsg = LoadUsrMsg(USTR_FaildSave, _T("履歴ファイル"));
			msgbox_ERR(errmsg);
			set_LogErrMsg(msg, errmsg);
		}
		AddLog(msg);
	}
	else {
		IniFile->SaveListItems("AllDirHistory", AllDirHistory, 100);
	}

	//その他のオプション
	sct = SCT_Option;
	IniFile->WriteString( sct, "TempPath",		to_relative_name(TempPathA));
	IniFile->WriteString( sct, "DirBraStr",		DirBraStr);
	IniFile->WriteString( sct, "DirKetStr",		DirKetStr);
	IniFile->WriteString( sct, "TimeStampFmt",	TimeStampFmt);
	IniFile->WriteInteger(sct, "ViewBinLimitSize",	ViewBinLimitSize);

	//イベント
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
//INIファイルを更新
//---------------------------------------------------------------------------
void UpdateIniFile()
{
	if (IniFile->Modified) {
		UnicodeString msg = make_LogHdr(_T("SAVE"), IniFile->FileName);
		if (!IniFile->UpdateFile()) {
			UnicodeString errmsg = LoadUsrMsg(USTR_FaildSave, _T("INIファイル"));
			msgbox_ERR(errmsg);
			set_LogErrMsg(msg, errmsg);
		}
		AddLog(msg);
	}
}

//---------------------------------------------------------------------------
//タグに設定されている変数ポインタを用いてコントロールに値を設定
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
//int 型オプションのデフォルト値を取得
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
//タグに設定されている変数ポインタを用いてコントロールの変更を反映
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
//一覧用グリッドの初期化
//---------------------------------------------------------------------------
void InitializeListGrid(TStringGrid *gp, TFont *fnt)
{
	gp->Color = col_bgList;
	gp->Font->Assign(fnt? fnt : ListFont);
	gp->DefaultRowHeight = get_FontHeight(gp->Font, ListInterLn);
}
//---------------------------------------------------------------------------
//一覧用ヘッダの初期化
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
//フォーム内のコンボボックスについて自動補完の設定を適用
//---------------------------------------------------------------------------
void set_ComboBox_AutoComp(TForm *frm)
{
	for (int i=0; i<frm->ComponentCount; i++) {
		TComponent *cp = frm->Components[i];
		if (class_is_ComboBox(cp)) ((TComboBox*)cp)->AutoComplete = AutoCompComboBox;
	}
}

//---------------------------------------------------------------------------
//Megemo オプションを設定
//---------------------------------------------------------------------------
void set_MigemoCheckBox(TCheckBox *cp, const _TCHAR *key)
{
	cp->Enabled = usr_Migemo->DictReady;
	cp->Checked = usr_Migemo->DictReady && IniFile->ReadBoolGen(key);
}

//---------------------------------------------------------------------------
//コンボボックスの検索履歴入れ替え
//---------------------------------------------------------------------------
void change_ComboBoxHistory(TComboBox *cp,
	const _TCHAR *nrm_sct,	//通常の履歴セクション
	const _TCHAR *reg_sct,	//正規表現の履歴セクション
	bool reg_sw)			//true = 正規表現
{
	UnicodeString s = cp->Text;
	IniFile->SaveComboBoxItems(cp, !reg_sw? reg_sct : nrm_sct);
	IniFile->LoadComboBoxItems(cp,  reg_sw? reg_sct : nrm_sct);
	cp->Text = s;
}

//---------------------------------------------------------------------------
//リストの絞り込み
//Objects 内容も設定 (0 なら リストのインデックスを設定)
//---------------------------------------------------------------------------
void filter_List(
	TStringList *i_lst, //対象リスト
	TStringList *o_lst, //結果リスト
	UnicodeString kwd,	//検索語
	bool migemo_sw,		//Migemoを使用
	bool and_or_sw)		//AND/OR検索	(default = false)
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
//ファイル一覧ソート用比較関数
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
//名前
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

	//ファイル同士
	UnicodeString nam0 = !fp0->alias.IsEmpty()? fp0->alias : fp0->b_name;
	UnicodeString nam1 = !fp1->alias.IsEmpty()? fp1->alias : fp1->b_name;
	int res = SameText(nam0, nam1)? CompTextN(fp0->f_ext, fp1->f_ext) : CompTextN(nam0, nam1);
	return (res==0)? CompTextN(fp0->p_name, fp1->p_name) : res;
}
//---------------------------------------------------------------------------
//拡張子
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
//タイムスタンプ
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
//サイズ
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
//属性
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
//ディレクトリ名(登録名)
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

	//ファイル同士
	if (SameText(pnam0, pnam1)) {
		int res = SameText(fp0->b_name, fp1->b_name) ? CompTextN2(fp0->f_ext, fp1->f_ext)
													 : CompTextN2(fp0->b_name, fp1->b_name);
		return (res==0)? CompTextN2(fp0->p_name, fp1->p_name) : res;
	}

	return CompTextN2(pnam0, pnam1);
}
//---------------------------------------------------------------------------
//メモ内容
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
//マーク設定日時
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
//キー一覧ソート用比較関数
//---------------------------------------------------------------------------
//キー
//---------------------------------------------------------------------------
int __fastcall KeyComp_Key(TStringList *List, int Index1, int Index2)
{
	UnicodeString k0 = List->Names[Index1];
	UnicodeString k1 = List->Names[Index2];

	//カテゴリー
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

	//シフト
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

	//2ストローク
	if (contains_s(k0, _T('~')) && contains_s(k1, _T('~'))) {
		UnicodeString f0 = split_tkn(k0, '~');
		UnicodeString f1 = split_tkn(k1, '~');
		if (!SameText(f0, f1)) return CompareText(f0, f1);
	}

	//キー
	if (k0.Length()==1 && k1.Length()==1) return CompareText(k0, k1);

	if (k0.Length()>1 && k1.Length()>1) {
		if (k0[1]=='F' && k1[1]!='F') return -1;
		if (k0[1]!='F' && k1[1]=='F') return 1;
		return StrCmpLogicalW(k0.c_str(), k1.c_str());
	}

	return k0.Length() - k1.Length();
}
//---------------------------------------------------------------------------
//コマンド
//---------------------------------------------------------------------------
int __fastcall KeyComp_Cmd(TStringList *List, int Index1, int Index2)
{
	UnicodeString cmd0 = List->ValueFromIndex[Index1];
	UnicodeString cmd1 = List->ValueFromIndex[Index2];
	return !SameText(cmd0, cmd1)? CompareText(cmd0, cmd1) : KeyComp_Key(List, Index1, Index2);
}
//---------------------------------------------------------------------------
//説明
//---------------------------------------------------------------------------
int __fastcall KeyComp_Dsc(TStringList *List, int Index1, int Index2)
{
	UnicodeString dsc0 = get_CmdDesc(List->ValueFromIndex[Index1], true);
	UnicodeString dsc1 = get_CmdDesc(List->ValueFromIndex[Index2], true);
	return !SameText(dsc0, dsc1)? CompareText(dsc0, dsc1) : KeyComp_Key(List, Index1, Index2);
}

//---------------------------------------------------------------------------
//.nyanfi 名を取得
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
	bool inherit,	//上位から継承
	bool force)		//カレントを無視して強制的に継承
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
//Web検索表示文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_WebSeaCaption()
{
	UnicodeString ret_str = get_tkn_m(WebSeaUrl, _T("//"), _T("/"));
	remove_top_text(ret_str, _T("www."));
	if (ret_str.IsEmpty()) ret_str = "Web";

	return ret_str.UCAT_T(" で検索");
}

//---------------------------------------------------------------------------
//最新のタイムスタンプか？
//---------------------------------------------------------------------------
bool is_NewerTime(TDateTime scr_t, TDateTime dst_t)
{
	return (!WithinPastMilliSeconds(scr_t, dst_t, TimeTolerance) && scr_t>dst_t);
}

//---------------------------------------------------------------------------
//UNCパスの存在チェック
//  ESC で中断可能(スレッドでチェック)
//戻り値: true	= 存在する or 非UNCパス
//		  false = 存在しない or 無効な UNCパス
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
//文字列からコマンドファイル名を取得
// 先頭の @ は削除し、絶対パス化
// .nbt でなければ EmptyStr を返す
//---------------------------------------------------------------------------
UnicodeString get_cmdfile(UnicodeString s)
{
	if (!remove_top_AT(s) && !test_NbtExt(get_extension(s))) return EmptyStr; 
	return rel_to_absdir(s);
}

//---------------------------------------------------------------------------
//ExeCommands のパラメータからファイル名を取得
//---------------------------------------------------------------------------
UnicodeString get_cmds_prm_file(UnicodeString prm)
{
	if (remove_top_AT(prm))	return rel_to_absdir(prm);
	if (remove_top_text(prm, _T("ExeMenuFile_")))
							return rel_to_absdir(exclude_quot(prm));
	return EmptyStr;
}

//---------------------------------------------------------------------------
//必要な場合レスポンスファイルを作成
//  戻り値: レスポンスファイル名
//			不要の場合は EmptyStr、エラーの場合は RESPONSE_ERR
//---------------------------------------------------------------------------
UnicodeString make_ResponseFile(TStringList *lst,
	int arc_t,				//アーカイブタイプ
	UnicodeString *files,	//[o] ファイル名リスト (default = NULL)
	bool excl)				//true: レスポンスファイル/リストの一方のみを作成
							// (default = false: レスポンスファイル/リストに分散)
{
	std::unique_ptr<TStringList> r_lst(new TStringList());
	std::unique_ptr<TStringList> f_lst(new TStringList());

	//統合アーカイバ以外 (SPI)
	if (arc_t==0) {
		for (int i=0; i<lst->Count; i++) r_lst->Add(add_quot_if_spc(lst->Strings[i]));
	}
	//統合アーカイバ
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
					//※unrar32.dll のバグ? 対策
					if (starts_AT(fnam)) fnam = "?" + exclude_top(fnam);
					f_lst->Add(add_quot_if_spc(fnam));
				}
			}
		}

		//レスポンスファイルのみ
		if ((excl || !files) && r_lst->Count>0 && f_lst->Count>0) {
			r_lst->Clear();
			f_lst->Clear();
			for (int i=0; i<lst->Count; i++) r_lst->Add(add_quot_if_spc(lst->Strings[i]));
		}
		//リスト
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
		//ファイル保存
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
//クローン化/同名処理時の改名書式文字列を展開
//---------------------------------------------------------------------------
UnicodeString format_CloneName(
	UnicodeString fmt,		//書式
	UnicodeString fnam,		//ファイル名
	UnicodeString dst_dir,	//作成/コピー先ディレクトリ
	bool is_dir)			//対象はディレクトリーか? (default = false)
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
				//連番
				if (USTARTS_TS("SN(", tmp)) {
					UnicodeString nstr = split_in_paren(tmp);
					if (nstr.IsEmpty()) nstr = "1";
					ret_str.cat_sprintf(_T("%0*u"), nstr.Length(), nstr.ToIntDef(0) + sn);
				}
				//日時
				else if (USTARTS_TS("DT(", tmp) || USTARTS_TS("TS(", tmp)) {
					TDateTime dt = USTARTS_TS("TS(", tmp)? get_file_age(fnam) : Now();
					ret_str += FormatDateTime(split_in_paren(tmp), dt);
				}
				else {
					c = split_top_wch(tmp);
					//ファイル名主部またはディレクトリ名
					if (c=='N') ret_str += bnam;
				}
			}
			else ret_str.cat_sprintf(_T("%c"), c);
		}

		ret_str += fext;
		if (!file_exists(ret_str)) break;

		//重複により再試行
		if (contains_s(fmt_str, _T("\\SN(")))
			sn++;
		else if (SameText(last_str, ret_str))
			fmt += SFX_AUTOREN;

		last_str = ret_str;
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//書式文字列を展開
//  \L(n)    ファイル名の左からn文字
//  \S(m,n)  ファイル名のm番目からn文字
//  \R(n)    ファイル名の右からn文字
//  \A       ファイル名全体
//  \E       拡張子
//  \TS(…)  タイムスタンプ
//  \XT(…)  Exit撮影日時
//  \\       \ そのもの
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
//対応するソース／ヘッダファイル名を取得
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
//ファイル名主部が同じ次のファイル名を取得
//---------------------------------------------------------------------------
UnicodeString get_NextSameName(
	UnicodeString fnam,		//ファイル名
	bool only_text)			//true = テキストのみ (default = false)
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
	bool only_text)			//true = テキストのみ (default = false)
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
//ライブラリリストの取得
//---------------------------------------------------------------------------
void get_LibraryList(
	UnicodeString fnam,		//ライブラリファイル名(末尾が \ ならライブラリパス)
	TStringList *lst,
	bool get_info)			//ファイル情報として取得 (default = false)
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
							if (l_cnt==0 && get_info) lst->Add(EmptyStr);	//セパレータ
							l_cnt++;
							if (get_info)
								add_PropLine(UnicodeString("場所").cat_sprintf(_T("%u"), l_cnt), url, lst);
							else
								lst->Add(url);
						}
						tag = 1;
					}
					break;
				}
			}
		}
		if (l_cnt>0 && get_info) lst->Add(EmptyStr);	//セパレータ
	}
	catch (...) {
		;
	}
}

//---------------------------------------------------------------------------
//カレントがライブラリ登録ディレクトリのルートならライブラリファイルを返す
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
//同期対象リストを取得
//  戻り値: オプション文字列(O,D)
//---------------------------------------------------------------------------
UnicodeString get_SyncDstList(
	UnicodeString dnam,		//ディレクトリ名
	TStringList *lst,
	bool del_sw, 			//同期削除			(default = false)
	UnicodeString ex_dnam)	//除外ディレクトリ	(default = EmptyStr)
{
	dnam = IncludeTrailingPathDelimiter(dnam);

	lst->Clear();
	lst->Add(dnam);

	UnicodeString opt;
	for (int i=0; i<SyncDirList->Count; i++) {
		//"タイトル","有効:1/無効:0","オプション","dir1","dir2",...
		TStringDynArray syn_lst = get_csv_array(SyncDirList->Strings[i], 50);	//***
		if (syn_lst.Length<5)		continue;	//不正
		if (equal_0(syn_lst[1]))	continue;	//無効
		if (del_sw && !ContainsText(syn_lst[2], "D")) continue;
		//同期対象があるか?
		for (int j=3; j<syn_lst.Length; j++) syn_lst[j] = IncludeTrailingPathDelimiter(syn_lst[j]);
		bool flag = false;
		UnicodeString snam;	//サブディレクトリ部分
		for (int j=3; j<syn_lst.Length; j++) {
			if (StartsText(syn_lst[j], dnam)) {
				snam = dnam;
				snam.Delete(1, syn_lst[j].Length());
				flag = true;
				break;
			}
		}
		if (!flag) continue;

		//同期対象あり
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
//同期すべきディレクトリがあるか?
//---------------------------------------------------------------------------
bool has_SyncDir(
	UnicodeString dnam,
	bool del_sw)			//同期削除	(default = false)
{
	std::unique_ptr<TStringList> d_lst(new TStringList());
	get_SyncDstList(dnam, d_lst.get(), del_sw);
	return (d_lst->Count>1);
}

//---------------------------------------------------------------------------
//ディレクトリが互いに同期関係にあるか?
//  戻り値: S | [1][2]  S=同期, 1=dnam1が登録, 2=dnam2が登録
//---------------------------------------------------------------------------
UnicodeString is_SyncDir(UnicodeString dnam1, UnicodeString dnam2)
{
	UnicodeString ret_str;
	dnam1 = IncludeTrailingPathDelimiter(dnam1);
	dnam2 = IncludeTrailingPathDelimiter(dnam2);

	for (int i=0; i<SyncDirList->Count; i++) {
		//"タイトル","有効:1/無効:0","オプション","dir1","dir2",...
		TStringDynArray syn_lst = get_csv_array(SyncDirList->Strings[i], 50);
		if (syn_lst.Length<5 || equal_0(syn_lst[1])) continue;

		for (int j=3; j<syn_lst.Length; j++) syn_lst[j] = IncludeTrailingPathDelimiter(syn_lst[j]);
		bool flag1 = false, flag2 = false;
		UnicodeString snam1, snam2;	//サブディレクトリ部分
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
//一時ディレクトリをクリア
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
//アーカイブ用の一時ディレクトリを削除
//---------------------------------------------------------------------------
void ClearTempArc(
	UnicodeString dnam)		//ディレクトリ名 (default = EmptyStr : すべて削除)
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
//アーカイブ用の一時ディレクトリを更新
//  tag 指定の場合、arc_TmpList に登録
//---------------------------------------------------------------------------
UnicodeString UpdateTempArcList(int tag)
{
	//既存の一時ディレクトリを削除
	if (tag!=-1) ClearTempArcList(tag);

	//新規一時ディレクトリを追加
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
//アーカイブ内ファイルの一時解凍処理
//  fp->tmp_name に一時ファイル名を設定
//---------------------------------------------------------------------------
bool SetTmpFile(
	file_rec *fp,
	bool not_unpk,		//解凍せずに一時ファイル名を設定するだけ (default = false)
	bool show_prg)		//進捗状況を表示 (default = false)
{
	if (!fp || fp->is_dir || !fp->is_virtual || fp->arc_name.IsEmpty()) return false;

	int tag = fp->tag;
	if (tag==-1) tag = CurListTag;
	flist_stt *lst_stt = &ListStt[tag];

	try {
		UnicodeString tmp_path;
		bool is_v_item = contains_Slash(fp->f_name);
		if (!file_exists(fp->tmp_name) || is_v_item) {
			//アーカイブ内項目
			if (is_v_item) {
				if (!not_unpk) UpdateTempArcList(tag);
				tmp_path = lst_stt->arc_TmpList->Strings[0];
			}
			//多重アーカイブ
			else if (test_ArcAxExt(fp->f_ext)) {
				tmp_path = UpdateTempArcList();
				if (tmp_path.IsEmpty()) Abort();
			}
			//その他
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
//ZIP アーカイブ内の適当な画像を見つけて解凍
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
//圧縮対象リストに項目を追加
//---------------------------------------------------------------------------
void add_PackItem(file_rec *fp, int arc_t, UnicodeString src_dir, TStringList *lst)
{
	UnicodeString fnam = fp->n_name;
	if (fp->is_dir) {
		if (arc_t==UARCTYP_CAB) {
			//cab32.dll の -r の代替策
			std::unique_ptr<TStringList> fbuf(new TStringList());
			get_files(fp->f_name, _T("*.*"), fbuf.get(), true);
			for (int i=0; i<fbuf->Count; i++) {
				UnicodeString lbuf = fbuf->Strings[i];
				lst->Add(lbuf.Delete(1, src_dir.Length()));
			}
		}
		else {
			//カレントの空ディレクトリはマスクを付けない
			if (!is_EmptyDir(fp->f_name)) fnam.UCAT_T("\\*");
			lst->Add(fnam);
		}
	}
	else lst->Add(fnam);
}

//---------------------------------------------------------------------------
//無効なUNCパスか?
//---------------------------------------------------------------------------
bool is_InvalidUnc(UnicodeString dnam, 
	bool del_sw)	//見つかった項目を削除 (default = false);
{
	int idx = -1;
	if (USTARTS_TS("\\\\", dnam)) {
		idx = InvalidUncList->IndexOf(dnam);
		if (idx!=-1 && del_sw) InvalidUncList->Delete(idx);
	}

	return (idx != -1);
}

//---------------------------------------------------------------------------
//パスの存在をチェックし、無ければ利用可能なパスを取得
//---------------------------------------------------------------------------
UnicodeString CheckAvailablePath(UnicodeString dnam, int tag)
{
	//ディレクトリの存在チェック(なければ親を検索)
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

		//存在しないドライブの履歴・スタック項目を削除
		for (int i=0; i<MAX_FILELIST+1; i++) {
			TStringList *h_lst = (i<MAX_FILELIST)? get_DirHistory(CurTabIndex, i) : DirStack;
			if (h_lst) {
				int idx = 0;
				while (idx<h_lst->Count)
					if (StartsText(drvnam, get_csv_item(h_lst->Strings[idx], 0))) h_lst->Delete(idx); else idx++;
			}
		}
	}

	//履歴から探す
	TStringList *h_lst = get_DirHistory(CurTabIndex, tag);
	if (h_lst && h_lst->Count>0)
		dnam = get_csv_item(h_lst->Strings[0], 0);
	//存在するドライブから探す
	else {
		dnam = EmptyStr;
		DWORD d_bit  = ::GetLogicalDrives();	//利用可能なドライブをビットマスク形式で取得
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
//現在のツールボタン定義リストを取得
//---------------------------------------------------------------------------
TStringList *GetCurBtnList(int scr_mode)
{
	return (scr_mode==SCMD_FLIST)? ToolBtnList :
		   (scr_mode==SCMD_TVIEW)? ToolBtnListV :
		   (scr_mode==SCMD_IVIEW)? ToolBtnListI : NULL;
}

//---------------------------------------------------------------------------
//ファイルリストを取得
//---------------------------------------------------------------------------
TStringList* GetFileList(int tag)
{
	flist_stt *cur_stt = &ListStt[tag];
	return cur_stt->is_Arc?  ArcFileList[tag] :
		   cur_stt->is_Find? ResultList[tag] :
		   cur_stt->is_Work? WorkList: FileList[tag];
}
//---------------------------------------------------------------------------
//カレントのリストを取得
//---------------------------------------------------------------------------
TStringList* GetCurList(
	bool only_filer)	//ファイラーのみを対象 (default = false)
{
	return ((!only_filer && ScrMode==SCMD_IVIEW)? ViewFileList : GetFileList(CurListTag));
}
//---------------------------------------------------------------------------
//反対パスのリストを取得
//---------------------------------------------------------------------------
TStringList* GetOppList()
{
	return GetFileList(OppListTag);
}

//---------------------------------------------------------------------------
//ファイルリストの背景色を取得
//---------------------------------------------------------------------------
TColor get_FlBgColor(flist_stt *lst_stt, int idx)
{
	return lst_stt->is_Find? col_bgFind : lst_stt->is_Arc? col_bgArc :
		   lst_stt->is_Work? col_bgWork : lst_stt->is_FTP? col_bgFTP :
		   is_AltLnBgCol(idx)? col_bgList2 : col_bgList;
}

//---------------------------------------------------------------------------
//ファイルリストのソート
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
		lst->CustomSort(SortComp_PathName);		//場所
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
//ファイルストで項目サーチ
//---------------------------------------------------------------------------
int IndexOfFileList(
	UnicodeString fnam,	//ファイル名
	int tag,			//タグ				(default = CurListTag)
	int top_idx)		//先頭インデックス	(default = -1)
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
//選択中ファイルのリスト文字列を取得
//！失敗したら EAbort 例外
//---------------------------------------------------------------------------
UnicodeString GetSelFileStr(
	TStringList *lst,
	bool use_tmp,			//アーカイブ内のファイルを一時解凍	(default = false)
	bool inc_dir,			//ディレクトリも含める				(default = false)
	TStringList *s_lst)		//[o] 選択リスト					(default = NULL)
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
//ファイルリストのクリア
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
//ファイルリストの項目削除
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
//選択有り
//---------------------------------------------------------------------------
bool ListSelected(TStringList *lst)
{
	bool res = false;
	if (lst) for (int i=0; i<lst->Count && !res; i++) res = ((file_rec*)lst->Objects[i])->selected;
	return res;
}
//---------------------------------------------------------------------------
//選択中の項目数
//---------------------------------------------------------------------------
int GetSelCount(TStringList *lst)
{
	int s_cnt = 0;
	if (lst)
		for (int i=0; i<lst->Count; i++) if (((file_rec*)lst->Objects[i])->selected) s_cnt++;
	return s_cnt;
}
//---------------------------------------------------------------------------
//選択中項目名リストを取得
//---------------------------------------------------------------------------
void GetSelList(
	TStringList *lst,
	TStringList *sel_lst,
	bool with_path,		//パス付で取得		(default = true)
	bool with_obj)		//Objects も取得	(default = false)
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
//カレントの選択状態を解除
//---------------------------------------------------------------------------
void ClrSelect(TStringList *lst)
{
	if (!lst) lst = GetCurList();
	for (int i=0; i<lst->Count; i++) ((file_rec*)lst->Objects[i])->selected = false;
}

//---------------------------------------------------------------------------
//file_rec を生成して初期化
//  copy_fp を指定すると、その内容をコピー
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
//ファイル名から file_rec を生成
//---------------------------------------------------------------------------
file_rec* cre_new_file_rec(
	UnicodeString fnam,		//ファイル名(末尾が \ の場合はディレクトリ名)
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
// FileListの内容を入れ替える
//---------------------------------------------------------------------------
void swap_FileList(TStringList *lst1, TStringList *lst2, bool swap_tag)
{
	std::unique_ptr<TStringList> sbuf(new TStringList());
	sbuf->Assign(lst1); lst1->Assign(lst2); lst2->Assign(sbuf.get());

	//タグの入れ替え
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
//ファイル情報の項目を更新
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
//タブリストの処理
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
//検索設定のクリア
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

/*	結果リストの状態維持のため以下は初期化しない
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
//検索条件のチェック
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
//標準ファイル検索
//！不正な正規表現パターンを用いないこと(要事前チェック)
//---------------------------------------------------------------------------
bool check_file_std(
	UnicodeString fnam,		//パス無ファイル名
	TDateTime f_tm, __int64 f_sz, int f_atr, flist_stt *lst_stt)
{
	//キーワード
	if (!lst_stt->find_Keywd.IsEmpty()) {
		UnicodeString kwd = lst_stt->find_Keywd;
		bool is_regex = lst_stt->find_RegEx;
		//ダブルクォーテーションで囲まれていたら空白を含む語として正規表現で
		if (is_quot(kwd)) {
			kwd = TRegEx::Escape(exclude_quot(kwd));
			if (contains_s(kwd, _T(' '))) kwd = REPLACE_TS(kwd, " ", "\\s");
			is_regex = true;
		}
		//検索
		if (is_regex) {
			TRegExOptions opt;  if (!lst_stt->find_Case) opt << roIgnoreCase;
			if (!TRegEx::IsMatch(fnam, kwd, opt)) return false;
		}
		else
			if (!find_mlt(kwd, fnam, lst_stt->find_And, false, lst_stt->find_Case)) return false;
	}
	//タイムスタンプ
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
	//サイズ
	if (!(f_atr & faDirectory) && lst_stt->find_sz_mode>0) {
		switch (lst_stt->find_sz_mode) {
		case 1: if (!(f_sz <= lst_stt->find_size)) return false; break;
		case 2: if (!(f_sz >= lst_stt->find_size)) return false; break;
		}
	}
	//属性
	if (lst_stt->find_at_mode>0) {
		switch (lst_stt->find_at_mode) {
		case 1: if (!(f_atr & lst_stt->find_attr)) return false; break;
		case 2: if ((f_atr & lst_stt->find_attr))  return false; break;
		}
	}

	return true;
}

//---------------------------------------------------------------------------
//拡張ファイル検索
//！不正な正規表現パターンを用いないこと(要事前チェック)
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

	//コードページ/改行コード
	if (lst_stt->find_codepage!=-1 || !lst_stt->find_linebrk.IsEmpty()) {
		//コードページ
		UnicodeString linebrk;
		bool has_bom;
		int code_page = get_FileCodePage(fnam, &linebrk, &has_bom);	if (code_page==0) return false;
		if (lst_stt->find_codepage!=-1 && code_page!=lst_stt->find_codepage) return false;
		//改行コード
		if (!lst_stt->find_linebrk.IsEmpty()) {
			if (USAME_TS(lst_stt->find_linebrk, "混在")) {
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

	if (!check_prop(fnam, "ビット レート",	lst_stt->find_rate,  lst_stt->find_rt_mode)) return false;
	if (!check_prop(fnam, "フレーム率",		lst_stt->find_fps,   lst_stt->find_fs_mode)) return false;
	if (!check_prop(fnam, "フレーム幅",		lst_stt->find_fr_wd, lst_stt->find_fw_mode)) return false;
	if (!check_prop(fnam, "フレーム高",		lst_stt->find_fr_hi, lst_stt->find_fh_mode)) return false;

	//サンプリング周波数
	if (lst_stt->find_sp_mode>0 || lst_stt->find_bt_mode>0 || lst_stt->find_ch_mode>0) {
		try {
			std::unique_ptr<TStringList> lst(new TStringList());
			get_WavInf(fnam, lst.get());
			if (!contains_s(lst->Text, _T(" 形式: "))) Abort();
			UnicodeString frmt = get_tkn_m(lst->Text, _T("形式: "), _T("\r\n"));
			//サンプリング
			int sp = split_tkn(frmt, _T("Hz ")).ToInt();
			if (!check_int(sp, lst_stt->find_smpl, lst_stt->find_sp_mode)) Abort();
			//ビット
			int bt = split_tkn(frmt, _T("bit ")).ToInt();
			switch (lst_stt->find_bt_mode) {
			case 1: if (bt!=8)	Abort(); break;
			case 2: if (bt!=16)	Abort(); break;
			case 3: if (bt!=24)	Abort(); break;
			case 4: if (bt!=32)	Abort(); break;
			}
			//チャンネル
			switch (lst_stt->find_ch_mode) {
			case 1: if (!USAME_TI(frmt, "モノ"))		Abort(); break;
			case 2: if (!USAME_TI(frmt, "ステレオ"))	Abort(); break;
			case 3: if (!USAME_TI(frmt, "4ch"))			Abort(); break;
			case 4: if (!USAME_TI(frmt, "6ch"))			Abort(); break;
			case 5: if (!USAME_TI(frmt, "8ch"))			Abort(); break;
			}
		}
		catch (...) {
			return false;
		}
	}

	//画像サイズ
	if (lst_stt->find_iw_mode>0 || lst_stt->find_ih_mode>0) {
		unsigned int i_wd, i_hi;
		if (!get_img_size(fnam, &i_wd, &i_hi)) return false;
		if (lst_stt->find_img_max && i_hi>i_wd) std::swap(i_wd, i_hi);
		if (!check_int(i_wd, lst_stt->find_img_wd, lst_stt->find_iw_mode)) return false;
		if (!check_int(i_hi, lst_stt->find_img_hi, lst_stt->find_ih_mode)) return false;
	}

	//アイコン数
	if (lst_stt->find_ic_mode>0) {
		int ixn = (int)::ExtractIcon(HInstance, fnam.c_str(), -1);
		if (!check_int(ixn, lst_stt->find_icon, lst_stt->find_ic_mode)) return false;
	}

	//リンクカウント数
	if (lst_stt->find_hl_mode>0) {
		int hln = get_HardLinkCount(fnam);
		if (hln<2) return false;
		if (lst_stt->find_hl_mode==1 && hln>lst_stt->find_hl_cnt)	return false;
	 	if (lst_stt->find_hl_mode==2 && hln!=lst_stt->find_hl_cnt)	return false;
	 	if (lst_stt->find_hl_mode==3 && hln<lst_stt->find_hl_cnt)	return false;
	}

	//ファイル情報文字列検索
	if (!lst_stt->find_PrpKwd.IsEmpty()) {
		std::unique_ptr<TStringList> lst(new TStringList());	//ファイル情報用
		TStringList *i_lst = lst.get();
		add_PropLine(_T("種類"), usr_SH->get_FileTypeStr(fnam, true), i_lst);

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

	//テキスト
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

	//Exif情報文字列検索
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

	//GPS距離範囲
	if (!lst_stt->find_LatLng.IsEmpty()) {
		double lat, lng;
		if (!get_GpsInf(fnam, &lat, &lng)) return false;
		return (GetDistance(lst_stt->find_GpsLat, lst_stt->find_GpsLng, lat, lng) <= lst_stt->find_GpsRange);
	}

	return true;
}

//---------------------------------------------------------------------------
//フォント情報を取得
//---------------------------------------------------------------------------
void get_FontInf(UnicodeString fnam, TStringList *lst)
{
	if (!test_FontExt(get_extension(fnam))) return;

	//インストール済み
	bool ok = false;
	if (SameText(ExtractFileDir(fnam), cv_env_str("%WINDIR%\\Fonts")) && lpGetFontResourceInfo) {
		DWORD dwSize = 0;
		if (lpGetFontResourceInfo(fnam.c_str(), &dwSize, NULL, 2)) {
			int n = dwSize/sizeof(LOGFONT);
			if (n>0) {
				std::unique_ptr<LOGFONT[]> lfs(new LOGFONT[n]);
				if (lpGetFontResourceInfo(fnam.c_str(), &dwSize, lfs.get(), 2)) {
					//フォント名
					UnicodeString font_nam;
					for (int i=0; i<n; i++) {
						if (i>0) font_nam.UCAT_T("; ");
						font_nam.cat_sprintf(_T("%s"), lfs[i].lfFaceName);
					}
					add_PropLine(_T("タイトル"), font_nam, lst);
					ok = true;
				}
			}
		}
	}

	if (!ok) usr_SH->get_PropInf(fnam, lst);
}
//---------------------------------------------------------------------------
//フォント名を取得
//---------------------------------------------------------------------------
UnicodeString get_FontName(UnicodeString fnam)
{
	UnicodeString ret_str;

	std::unique_ptr<TStringList> lst(new TStringList());
	get_FontInf(fnam, lst.get());
	for (int i=0; i<lst->Count; i++) {
		UnicodeString lbuf = lst->Strings[i];
		if (StartsText("タイトル:", Trim(lbuf))) {
			ret_str = Trim(get_tkn_m(lbuf, ':', ';'));
			break;
		}
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//ドライブリストを取得
//  戻り値 : 新たに追加されたドライブ情報
//---------------------------------------------------------------------------
drive_info *get_DriveInfoList()
{
	drive_info *new_drive = NULL;

	DWORD d_bit  = ::GetLogicalDrives();	//利用可能なドライブをビットマスク形式で取得
	DWORD d_flag = 0x00000001;
	for (int d_n = _T('A'); d_n <= _T('Z'); d_n++,d_flag<<=1) {
		UnicodeString dstr;
		dstr.sprintf(_T("%c:\\"), d_n);
		int idx = DriveInfoList->IndexOf(dstr);
		drive_info *dp;
		if ((d_bit & d_flag)==0) {
			//存在しなくなったドライブ情報を削除
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
		dp->accessible = is_drive_accessible(dstr);		//アクセス可能
		dp->ejectable  = EjectDrive(dstr, false);		//取り外し可能

		//種類
		dp->drv_type = ::GetDriveType(dstr.c_str());
		switch (dp->drv_type) {
		case DRIVE_REMOVABLE:	dp->type_str.USET_T("リムーバブル・メディア");	break;
		case DRIVE_FIXED:		dp->type_str.USET_T("ハードディスク");			break;
		case DRIVE_REMOTE:		dp->type_str.USET_T("ネットワーク・ドライブ");	break;
		case DRIVE_CDROM:		dp->type_str.USET_T("CD-ROMドライブ");			break;
		case DRIVE_RAMDISK:		dp->type_str.USET_T("RAMディスク");	dp->is_RAM = true;	break;
		default: 				dp->type_str = EmptyStr;
		}

		//ボリューム名、ファイルシステム
		if (dp->accessible)
			dp->volume = get_volume_info(dstr, &dp->f_system);
		else
			dp->volume = dp->f_system = EmptyStr;

		dp->bus_type = EmptyStr;
		dp->is_SSD	 = dp->is_RAM = false;
		if (dp->drv_type!=DRIVE_REMOTE) {
			//接続I/Fを取得
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

					//SSDの判別 (Windows 7 以降)
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
							if (dp->is_SSD) dp->type_str.USET_T("ソリッドステート");
						}
						else rq_chk_ram = true;
					}
				}
				else rq_chk_ram = true;

				//RAMディスクの推測
				if (rq_chk_ram && dp->accessible && dp->drv_type==DRIVE_FIXED && dp->bus_type.IsEmpty()) {
					MEMORYSTATUSEX stt_ex;
					stt_ex.dwLength = sizeof(stt_ex);
					if (::GlobalMemoryStatusEx(&stt_ex)) {
						__int64 sTotal = DiskSize((char)dstr[1] - 'A' + 1);
						//サイズが物理メモリ使用容量より小さい
						if (sTotal<=(__int64)(stt_ex.ullTotalPhys - stt_ex.ullAvailPhys)) {
							dp->is_RAM	 = true;
							dp->type_str.USET_T("RAMディスク");
						}
					}
				}

				::CloseHandle(hDrive);
			}
		}

		//UNC(リモート)
		dp->unc = EmptyStr;
		if (dp->drv_type==DRIVE_REMOTE) {
			DWORD InfoSize = 1000;
			std::unique_ptr<BYTE[]> inf(new BYTE[InfoSize]);
			LPUNIVERSAL_NAME_INFO pInf = (LPUNIVERSAL_NAME_INFO)inf.get();
			if (WNetGetUniversalName(dstr.c_str(), UNIVERSAL_NAME_INFO_LEVEL, pInf, &InfoSize)==NO_ERROR)
				dp->unc = pInf->lpUniversalName;
		}

		//アイコン
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
//指定ドライブの情報を取得
//---------------------------------------------------------------------------
drive_info *get_DriveInfo(
	UnicodeString dstr)		//先頭がドライブ文字の文字列
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
//ボリューム名を更新
//---------------------------------------------------------------------------
void update_DriveVolume()
{
	for (int i=0; i<DriveInfoList->Count; i++) {
		drive_info *dp = (drive_info *)DriveInfoList->Objects[i];
		if (dp->accessible) dp->volume = get_volume_info(dp->drive_str);
	}
}

//---------------------------------------------------------------------------
//ドライブ容量ログの内容を取得
//---------------------------------------------------------------------------
void get_DriveLogList(
	UnicodeString dstr,	//対象ドライブ
	TStringList *lst,
	double &r_min,		//最小使用率
	double &r_max)		//最大使用率
{
	update_DriveLog(DriveLogName.IsEmpty());

	//指定ドライブのみ抽出
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
//ドライブ容量ログの更新
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

		//本日のデータを探す
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
//アーカイブから指定ディレクトリ以下の全項目のリストを取得
//---------------------------------------------------------------------------
void get_ArcList(
	UnicodeString anam,		//アーカイブのファイル名
	UnicodeString dnam,		//対象ディレクトリ名
	TStrings *f_lst,		//[o] ファイル名リスト
	TStrings *d_lst)		//[o] (空)ディレクトリ名リスト
{
	if (usr_ARC->OpenArc(anam)) {
		dnam = IncludeTrailingPathDelimiter(dnam);
		arc_find_inf inf;
		if (usr_ARC->FindFirstEx(EmptyStr, &inf, true)) {
			do {
				UnicodeString fnam = inf.f_name;
				if (!StartsText(dnam, fnam)) continue;
				fnam.Delete(1, dnam.Length());
				//ディレクトリ
				if (inf.is_dir || fnam.IsEmpty()) {
					if (d_lst->IndexOf(inf.f_name)==-1) d_lst->Add(inf.f_name);
				}
				//ファイル
				else {
					if (f_lst->IndexOf(inf.f_name)==-1) f_lst->Add(inf.f_name);
				}
			} while (usr_ARC->FindNextEx(&inf, true));
		}
		usr_ARC->CloseArc();
	}
}

//---------------------------------------------------------------------------
//汎用リストの項目高を設定 (行間 = 1/3)
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
//標準リストの設定 (行間 = ListInterLn)
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
//シンプルスクロールバーの設定
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
		//リストボックスのサイズ調整
		if (sp->AssoListBox) {
			//幅
			int wd = sp->ParentPanel->ClientWidth;
			if ((is_fl && HideScrBar) || ScrBarStyle>0) wd += (std_wd + 2);
			sp->AssoListBox->Width = wd;
			//高さ
			if (sp->Flag & USCRPNL_FLAG_HS) {
				int hi = sp->ParentPanel->ClientHeight;
				if (ScrBarStyle>0) hi += (std_wd + 2);
				sp->AssoListBox->Height = hi;
			}
		}
		//グリッドのサイズ調整
		else if (sp->AssoStrGrid) {
			//幅
			int wd = sp->ParentPanel->ClientWidth;
			if (ScrBarStyle>0) wd += (std_wd + 2);
			sp->AssoStrGrid->Width = wd;
			//高さ
			if (sp->Flag & USCRPNL_FLAG_HS) {
				int hi = sp->ParentPanel->ClientHeight;
				if (ScrBarStyle>0) hi += (std_wd + 2);
				sp->AssoStrGrid->Height = hi;
			}
		}
	}
}

//---------------------------------------------------------------------------
//拡張子の幅・位置を調整 (検索中に使用)
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
//ファイルリストをリストボックスに設定
// (仮想リストボックス対応)
//---------------------------------------------------------------------------
void assign_FileListBox(
	TListBox *lp,
	TStringList *lst,
	int idx,				//インデックス (default = -1)
	UsrScrollPanel *sp)		//シンプルスクロールバー (default = NULL)
{
	if (lp->Style==lbVirtualOwnerDraw)
		lp->Count = lst->Count;
	else
		lp->Items->Assign(lst);

	if (idx!=-1) ListBoxSetIndex(lp, idx);

	if (sp) sp->UpdateKnob();
}

//---------------------------------------------------------------------------
//仮想ファイルリストボックスの更新(最小で250ms間隔)
//---------------------------------------------------------------------------
void update_VirtualFileListBox(
	TStrings *lst, TListBox *lp,
	int idx,				//インデックス、指定した場合強制更新 (default = -1)
	UsrScrollPanel *sp)		//シンプルスクロールバー (default = NULL)
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
//ファイル検索リストの取得
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

	//サブディレクトリを検索
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

	//ファイルを検索
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

			//アーカイブ内検索
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

							//条件成立
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
								//拡張子の幅・位置調整
								set_FextWidth(fp, tag);
							}
							lst->AddObject(fp->f_name, (TObject*)fp);
							FindCount = (lst->Count>1)? lst->Count - 1 : 0;
							add_cnt++;
						} while (usr_ARC->FindNextEx(&inf, true));
					}
					usr_ARC->CloseArc();

					//リストボックス更新(仮想)
					if (add_cnt>0) update_VirtualFileListBox(lst, lp);
				}
			}
			else FindPath  = pnam;

			//※FindFirst での誤マッチを除外
			//  例: システム内で *.asp が *.aspx にマッチ (謎)
			if ((!is_dir || !USAME_TS(lst_stt->find_Mask, "*.*")) && !str_match(lst_stt->find_Mask, fnam)) continue;

			//標準検索
			if (!check_file_std(fnam, sr.TimeStamp, sr.Size, sr.Attr, lst_stt)) continue;
			//拡張ファイル検索
			if (!is_dir && !check_file_ex(pnam + fnam, lst_stt)) continue;

			//条件成立
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
				//拡張子の幅・位置調整
				set_FextWidth(fp, tag);
			}
			lst->AddObject(fp->f_name, (TObject*)fp);
			FindCount = (lst->Count>1)? lst->Count - 1 : 0;
			add_cnt++;
		} while(FindNext(sr)==0 && !FindAborted);
		FindClose(sr);

		//リストボックス更新(仮想)
		if (add_cnt>0) update_VirtualFileListBox(lst, lp);
	}
}

//---------------------------------------------------------------------------
//ディレクトリ検索リストの取得
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
				//タイムスタンプ
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
				//内容
				if (match && lst_stt->find_ct_mode>0) {
					bool is_empty = is_EmptyDir(pnam + dnam);
					switch (lst_stt->find_ct_mode) {
					case 1: match = is_empty;  break;
					case 2: match = !is_empty; break;
					}
				}
				//属性
				if (match && lst_stt->find_at_mode>0) {
					switch (lst_stt->find_at_mode) {
					case 1: match = (sr.Attr & lst_stt->find_attr); break;
					case 2: match = (sr.Attr & lst_stt->find_attr); break;
					}
				}

				//条件成立
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
					//リストボックス更新(仮想)
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
//すべてのサブディレクトリを取得
//---------------------------------------------------------------------------
void get_SubDirs(UnicodeString pnam, TStrings *lst, TStatusBar *stt_bar, int stt_idx)
{
	if (!dir_exists(pnam)) return;
	pnam = IncludeTrailingPathDelimiter(pnam);
	if (stt_bar && stt_idx>=0 && stt_idx<stt_bar->Panels->Count) {
		stt_bar->Panels->Items[stt_idx]->Text = pnam;
		stt_bar->Repaint();
	}

	//サブディレクトリを検索
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
//ディレクトリ容量の計算
//  戻り値: ディレクトリ容量
//---------------------------------------------------------------------------
__int64 get_DirSize(
	UnicodeString dnam,
	int *f_cnt,			//ファイル数へのポインタ
	int *d_cnt,			//ディレクトリ数へのポインタ
	__int64 *o_size)	//占有サイズへのポインタ
{
	if (CalcAborted) return -1;

	__int64 d_size = 0;
	int     clu_sz = get_ClusterSize(dnam);	//クラスタサイズ
	__int64 clu_n  = 0;						//クラスタ数

	dnam = IncludeTrailingPathDelimiter(dnam);
	UnicodeString sea_str = UAPP_T(dnam, "*");
	if (sea_str.Length()>=MAX_PATH) sea_str.Insert("\\\\?\\", 1);

	TSearchRec sr;
	if (FindFirst(sea_str, faAnyFile, sr)==0) {
		do {
			//サブディレクトリを検索
			if (sr.Attr & faDirectory) {
				if (!ContainsStr("..", sr.Name)) {
					d_size += get_DirSize(dnam + sr.Name, f_cnt, d_cnt, o_size);
					++(*d_cnt);
				}
			}
			//ファイル
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
//仮想ディレクトリ容量の計算
//  戻り値: ディレクトリ容量
//---------------------------------------------------------------------------
__int64 get_ArcDirSize(
	UnicodeString anam,		//アーカイブのファイル名
	UnicodeString dnam,		//対象ディレクトリ名
	int *f_cnt,				//ファイル数へのポインタ
	int *d_cnt)				//ディレクトリ数へのポインタ
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
					//ディレクトリ
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
//キャッシュから指定ファイルのアイコンを削除
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
//拡張子依存アイコンを取得 (キャッシュを利用)
//---------------------------------------------------------------------------
HICON get_fext_icon(
	UnicodeString fext)		//拡張子 .xxx	(default = EmptyStr: フォルダアイコン)
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
//スモールアイコンを描画 (スレッドで取得)
//  CachedIcoList を使用
//  ジャンクション/シンボリックリンクにも対応
//---------------------------------------------------------------------------
bool draw_SmallIcon(
	file_rec *fp,
	TCanvas *cv, int x, int y,
	bool force_cache)	//強制的にキャシュ
{
	if (!is_selectable(fp)) return false;

	HICON hIcon  = NULL;
	bool handled = false;

	//通常ディレクトリ
	if (fp->is_dir && !fp->is_sym)
		hIcon = get_fext_icon();
	else {
		UnicodeString fext = LowerCase(fp->f_ext);
		//実ファイル依存
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
						CachedIcoList->Add(fnam);	//スレッドに取得を要求
				}
				IconRWLock->EndWrite();
			}
			else hIcon = get_fext_icon(fext);
		}
		//拡張子依存
		else {
			if (fext.IsEmpty()) fext = ".nyanfi";
			hIcon = get_fext_icon(fext);
		}
	}

	if (handled) return true;
	if (!hIcon)  return false;

	//描画
	::DrawIconEx(cv->Handle, x, y, hIcon, SIcoSize, SIcoSize, 0, NULL, DI_NORMAL);
	return true;
}
//---------------------------------------------------------------------------
//指定ファイルのスモールアイコンを描画
//  MenuBtnIcoList を使用
//---------------------------------------------------------------------------
bool draw_SmallIcon2(
	UnicodeString fnam,
	TCanvas *cv, int x, int y)
{
	HICON hIcon;

	UnicodeString fext = LowerCase(get_extension(fnam));
	if (fnam.Pos('*')==1) fnam = EmptyStr;

	//ディレクトリ
	if (ends_PathDlmtr(fnam))
		hIcon = get_fext_icon();
	//実ファイル依存
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
	//拡張子依存
	else hIcon = get_fext_icon(fext);

	if (!hIcon)  return false;

	//描画
	::DrawIconEx(cv->Handle, x, y, hIcon, SIcoSize, SIcoSize, 0, NULL, DI_NORMAL);
	return true;
}

//---------------------------------------------------------------------------
//表示用スモールアイコンを取得してイメージリストに追加
//  UsrIcoList に登録して再利用
//---------------------------------------------------------------------------
int add_IconImage(
	UnicodeString fnam,		//ファイル名 (環境パスに対応)
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
//コマンドからファイル名を取得
//  ファイル名にパスがない場合、環境パスから検索
//---------------------------------------------------------------------------
UnicodeString get_file_from_cmd(UnicodeString s)
{
	//先頭コマンドを取得
	s = REPLACE_TS(s, ":\\", "\f");

	UnicodeString cmd;
	int p = 1;
	bool in_qut = false;
	while (p<=s.Length()) {
		WideChar c = s[p];
		//引用符外
		if (!in_qut) {
			if (c==':') break;
			if (c=='\"') in_qut = true;
			cmd.cat_sprintf(_T("%c"), c);
		}
		//引用符内
		else {
			if (c=='\"') in_qut = false;
			cmd.cat_sprintf(_T("%c"), c);
		}
		p++;
	}

	cmd = Trim(REPLACE_TS(cmd, "\f", ":\\"));

	//ファイル名を取得
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
//実行ファイル名を取得してコントロールに設定 (.lnk からの抽出に対応)
//---------------------------------------------------------------------------
void SetExtNameToCtrl(UnicodeString fnam, TWinControl *cp, 
	bool nbt_sw)	//.nbt に対応 (default = false)
{
	UnicodeString fext = get_extension(fnam);
	if (test_LnkExt(fext)) {
		UnicodeString xnam, prm, fld;
		int shw;
		usr_SH->get_LnkInf(fnam, NULL, &xnam, &prm, &fld, &shw);
		if (test_ExeExt(get_extension(xnam)))
			fnam = xnam;
		else {
			msgbox_WARN("リンク先が .exe ファイルではありません");
			return;
		}
	}

	if (nbt_sw && test_FileExt(fext, _T(".nbt")))
		fnam = "@" + to_relative_name(fnam);

	//コントロールに設定
	if		(class_is_Edit(cp))			((TEdit*)cp)->Text		  = fnam;
	else if (class_is_LabeledEdit(cp))	((TLabeledEdit*)cp)->Text = fnam;
	else if (class_is_ComboBox(cp))		((TComboBox*)cp)->Text	  = fnam;
}

//---------------------------------------------------------------------------
//メニューファイルを読み込む
//---------------------------------------------------------------------------
bool load_MenuFile(UnicodeString fnam, TStringList *lst)
{
	fnam = rel_to_absdir(cv_env_str(fnam));

	if (load_text_ex(fnam, lst)!=0) {
		for (int i=0; i<lst->Count; i++) {
			UnicodeString lbuf = Trim(lst->Strings[i]);
			if (lbuf.IsEmpty() || USTARTS_TS(';', lbuf)) continue;
			TStringDynArray m_buf = split_strings_tab(lbuf);
			//EditMenuFile コマンド
			if (m_buf.Length>1 && USAME_TI(m_buf[1], "EditMenuFile")) m_buf[1].sprintf(_T("FileEdit_\"%s\""), fnam.c_str());
			//アイコンファイル (環境パスに対応)
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
//ワークリストをファイルから読み込む
//　ファイル名は実行パスからの相対指定でも可
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
				//セパレータ
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
//ワークリストを保存
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
//指定拡張子やキーワードにマッチする次のファイルを検索
//---------------------------------------------------------------------------
int find_NextFile(
	TStringList *lst,
	int idx,
	UnicodeString fext,		//拡張子リスト						(default = EmptyStr)
	UnicodeString keywd,	//キーワード(正規表現)				(default = EmptyStr)
	bool skip_dir,			//ディレクトリをスキップ			(default = true)
	bool circular,			//見つからなかったら先頭から再検索	(default = true)
	bool case_sns)			//大小文字を区別					(default = false)
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
//指定拡張子やキーワードにマッチする前のファイルを検索
//---------------------------------------------------------------------------
int find_PrevFile(
	TStringList *lst,
	int idx,
	UnicodeString fext,		//拡張子リスト
	UnicodeString keywd,	//キーワード(正規表現)
	bool skip_dir,			//ディレクトリをスキップ
	bool circular,			//見つからなかったら最後から再検索
	bool case_sns)			//大小文字を区別
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
//関連付けされているアプリのリストを取得
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
//関連付けリストからメニュー用リストを作成
//---------------------------------------------------------------------------
void make_AssoMenuList(TStringDynArray app_lst, TStringList *lst)
{
	for (int i=0; i<app_lst.Length; i++) {
		UnicodeString anam = app_lst[i];
		UnicodeString lbuf;
		//コマンド
		if (USTARTS_TI("ExeCommands_", anam))
			lbuf = get_CmdDesc(anam, true);
		//コマンドファイル
		else if (remove_top_AT(anam))
			lbuf = get_base_name(anam);
		//エイリアス
		else if (remove_top_Dollar(anam)) {
			UnicodeString inam;	//アイコン
			//外部ツール
			TStringDynArray itm_buf = record_of_csv_list(ExtToolList, anam, 4, EXTTOOL_CSVITMCNT);
			if (itm_buf.Length==EXTTOOL_CSVITMCNT) {
				lbuf = itm_buf[0];
				inam = itm_buf[1];
			}
			//追加メニュー
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
		//アプリケーション
		else {
			UnicodeString dsc_str = split_dsc(anam);
			lbuf = def_if_empty(dsc_str, get_base_name(anam));
			lbuf.cat_sprintf(_T("\t\t%s"), anam.c_str());
		}
		lst->Add(lbuf);
	}
}

//---------------------------------------------------------------------------
//空欄なら無効色に
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
//ファイル名の色を取得
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
//拡張子別カラーを取得
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
	//拡張子なし(ドットファイルを含む)
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
//タイムスタンプ色を取得
//---------------------------------------------------------------------------
TColor get_TimeColor(TDateTime dt, TColor col_def)
{
	TDateTime dt0 = Now();
	double dd  = dt0 - dt;

	TColor col_t =
			(dd <= 1.0/24) ? col_Tim1H :	//1時間以内
			(dd <= 3.0/24) ? col_Tim3H : 	//3時間以内
			(dd <= 1.0)	   ? col_Tim1D :	//1日以内
			(dd <= 3.0)	   ? col_Tim3D :	//3日以内
			(dd <= 7.0)	   ? col_Tim7D :	//7日以内
			(dd <= (double)(dt0 - IncMonth(dt0, -1))) ? col_Tim1M :	//1ヶ月以内
			(dd <= (double)(dt0 - IncMonth(dt0, -3))) ? col_Tim3M :	//3ヶ月以内
			(dd <= (double)(dt0 - IncMonth(dt0, -6))) ? col_Tim6M :	//6ヶ月以内
			(dd <= (double)(dt0 - IncYear( dt0, -1))) ? col_Tim1Y :	//1年以内
			(dd <= (double)(dt0 - IncYear( dt0, -3))) ? col_Tim3Y :	//3年以内
						     						    col_TimOld;

	if (col_t==Graphics::clNone) col_t = col_def;

	return col_t;
}
//---------------------------------------------------------------------------
//サイズ色を取得
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
//ディレクトリ名またはワークリスト名を取得
//---------------------------------------------------------------------------
UnicodeString get_DirNwlName(UnicodeString pnam)
{
	if (!ends_PathDlmtr(pnam) && test_NwlExt(get_extension(pnam)))
		return ChangeFileExt(ExtractFileName(pnam), EmptyStr);
	else
		return (is_root_dir(pnam)? pnam : get_dir_name(pnam));
}

//---------------------------------------------------------------------------
//ファイル名表示文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_DispName(file_rec *fp)
{
	UnicodeString ret_str;

	if (fp) {
		if (fp->is_dir)
			ret_str.sprintf(_T("[%s]"), get_dir_name(fp->f_name).c_str());
		else if (fp->is_dummy && is_separator(fp->alias))
			ret_str.USET_T("< セパレータ >");
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
//登録名に置換
//---------------------------------------------------------------------------
UnicodeString get_RegDirName(UnicodeString pnam)
{
	pnam = IncludeTrailingPathDelimiter(pnam);
	if (SameText(pnam, LibraryPath)) return "ライブラリ\\";

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
//UNCパスをネットワークドライブ名パスに変換
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
//ネットワークドライブ名パスをUNCパスに変換
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
// *._sf ファイルからパスを取得
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
//登録ディレクトリのパス項目を取得
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
//登録ディレクトリの指定項目をセパレータを考慮して先頭へ移動
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
//ファイルリストの上下左右を取得
//---------------------------------------------------------------------------
UnicodeString get_LRUD_str(int tag, bool both)
{
	return both ? (DivFileListUD? "上下" : "左右")
				: (DivFileListUD? ((tag==0)? "上" : "下") : ((tag==0)? "左" : "右"));
}

//---------------------------------------------------------------------------
//削除制限されたディレクトリか?
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
//ファイル情報リストを取得
//---------------------------------------------------------------------------
void GetFileInfList(
	file_rec *fp,
	bool force)			//強制的に取得 (default = false)
{
	if (!fp) return;

	flist_stt *lst_stt = (fp->tag!=-1)? &ListStt[fp->tag] : NULL;

	TStringList *i_list = fp->inf_list;
	i_list->Clear();
	fp->prv_text  = EmptyStr;
	fp->tail_text = EmptyStr;

	//ファイル名
	UnicodeString lbuf;
	if (fp->is_up) {
		if (lst_stt) lbuf = lst_stt->is_Find? "<結果リスト>" : "<カレント>";
	}
	else {
		lbuf = fp->b_name + fp->f_ext;
		if (!fp->alias.IsEmpty()) lbuf.cat_sprintf(_T("  <%s>"), fp->alias.c_str());
		//監視中?
		if (!fp->is_dir && !fp->is_virtual && !fp->is_ftp) {
			for (int i=0; i<WatchTailList->Count; i++) {
				if (SameText(get_pre_tab(WatchTailList->Strings[i]), fp->f_name)) {
					lbuf.UCAT_T("  (監視中)");
					break;
				}
			}
		}
	}
	i_list->Add(lbuf);

	//パス名
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

	//属性、サイズ、タイムスタンプ
	i_list->Add(get_FileInfStr(fp));

	//警告
	UnicodeString warn_str = make_PropLine(_T("警告"), "名前");
	UnicodeString tmp;
	if (fp->f_attr==faInvalid)
		i_list->Add(make_PropLine(_T("警告"), "存在しません"));
	if (fp->f_name.Length()>=MAX_PATH)
		i_list->Add(tmp.sprintf(_T("%sが長すぎます(%u)"), warn_str.c_str(), fp->f_name.Length()));
	if (ContainsStr(fp->f_name, L"\u202e"))
		i_list->Add(tmp.sprintf(_T("%sに制御文字「RLO」が含まれています"), warn_str.c_str()));
	if (USTARTS_TS(' ', fp->b_name))
		i_list->Add(tmp.sprintf(_T("%sの先頭に空白があります"), warn_str.c_str()));
	if (UENDS_TS(' ', fp->b_name))
		i_list->Add(tmp.sprintf(_T("%sの末尾に空白があります"), warn_str.c_str()));
	if (contains_s(fp->b_name, _T("  ")))
		i_list->Add(tmp.sprintf(_T("%sに連続した空白が含まれています"), warn_str.c_str()));

	//ディレクトリ内情報
	if (fp->is_dir && lst_stt) {
		//FTPリモート
		if (fp->is_ftp) {
			add_PropLine(_T("種類"), (fp->is_sym? "シンボリックリンク" : "ディレクトリ"), i_list);
			//ファイル数、ディレクトリ数
			if (fp->is_up) {
				if (lst_stt->f_cnt!=-1)
					add_PropLine(_T("ファイル数"), get_size_str_B(lst_stt->f_cnt, 0), i_list);
				if (lst_stt->d_cnt!=-1)
					add_PropLine(_T("ディレクトリ数"), get_size_str_B(lst_stt->d_cnt, 0), i_list);
			}
		}
		//その他
		else if (fp->tag!=-1) {
			if (fp->is_sym) {
				unsigned int tag;
				UnicodeString lnam = get_ReparsePointTarget(fp->f_name, tag);
				if (tag!=0) {
					add_PropLine(_T("種類"),
						(tag==IO_REPARSE_TAG_MOUNT_POINT)? "ジャンクション" : "シンボリックリンク", i_list);
					add_PropLine(_T("リンク先"), lnam, i_list);
				}
			}
			else {
				tmp = "ディレクトリ";
				if (!fp->is_virtual && is_ProtectDir(fp->is_up? fp->p_name : fp->f_name)) tmp.UCAT_T(" (削除制限)");
				add_PropLine(_T("種類"), tmp, i_list);
			}

			//タグ
			if (!fp->is_virtual) {
				UnicodeString tags = usr_TAG->GetTags(fp->f_name);
				if (!tags.IsEmpty()) add_PropLine(_T("タグ"), tags, i_list);
			}

			//ファイル、ディレクトリ数
			int f_cnt  = fp->is_up? lst_stt->f_cnt : fp->f_count;
			int d_cnt  = fp->is_up? lst_stt->d_cnt : fp->d_count;
			__int64 f_size	 = fp->is_up? lst_stt->cur_total : fp->f_size;
			__int64 o_size	 = fp->is_up? lst_stt->occ_total : fp->o_size;
			__int64 drv_size = lst_stt->drive_Total;
			if (f_cnt!=-1) {
				lbuf = make_PropLine(_T("ファイル数"), get_size_str_B(f_cnt, 0));
				if (fp->is_up && lst_stt->sub_counted)
					lbuf.cat_sprintf(_T(" / %s"), get_size_str_B(lst_stt->f_cnt_total, 0).c_str());
				i_list->Add(lbuf);
			}
			if (d_cnt!=-1) {
				lbuf = make_PropLine(_T("ディレクトリ数"), get_size_str_B(d_cnt, 0));
				if (fp->is_up && lst_stt->sub_counted)
					lbuf.cat_sprintf(_T(" / %s"), get_size_str_B(lst_stt->d_cnt_total, 0).c_str());
				i_list->Add(lbuf);
			}

			//サイズ
			if (f_size>0) {
				i_list->Add(get_PropTitle(_T("合計サイズ")).cat_sprintf(_T("%s (%s)"),
					get_size_str_G(f_size, 10, SizeDecDigits, 1).c_str(), get_size_str_B(f_size, 5).c_str()));
				if (!fp->is_virtual && o_size>0) {
					i_list->Add(get_PropTitle(_T("占有サイズ")).cat_sprintf(_T("%s (%s)"),
						get_size_str_G(o_size, 10, SizeDecDigits, 1).c_str(), get_size_str_B(o_size, 5).c_str()));
					__int64 g_size = o_size - f_size;
					i_list->Add(get_PropTitle(_T("クラスタgap")).cat_sprintf(_T("%s (%s) %4.1f%%"),
						get_size_str_G(g_size, 10, SizeDecDigits, 1).c_str(), get_size_str_B(g_size, 5).c_str(),
						100.0 * g_size/o_size));
					if (drv_size>0)
						i_list->Add(get_PropTitle(_T("ドライブ占有率")).cat_sprintf(_T("%6.2f%%"), 100.0 * o_size/drv_size));
				}
			}

			//説明
			if (!fp->is_virtual) {
				UnicodeString cfg_nam = get_dotNaynfi(fp->is_up? fp->p_name : fp->f_name);
				if (file_exists(cfg_nam)) {
					std::unique_ptr<TStringList> cfg_lst(new TStringList());
					load_text_ex(cfg_nam, cfg_lst.get());
					add_PropLine_if(_T("説明"), cfg_lst->Values["Description"], i_list);
				}
			}
		}

		//拡張子別ファイル数
		if (fp->is_up) {
			TStringList *lst = GetFileList(fp->tag);
			add_FExtInfList(lst, i_list);
		}

		//メモ
		UnicodeString memo = IniFile->GetMarkMemo(fp->r_name);
		if (!memo.IsEmpty()) {
			i_list->Add(EmptyStr);	//セパレータ
			add_PropLine(_T("メモ"), memo, i_list);
		}
	}

	if (fp->is_dir) return;

	fp->is_video = test_FileExt(fp->f_ext, FEXT_VIDEO);

	if (!force) {
		//ファイル情報を取得しないパスのチェック
		if (is_NoInfPath(fp, NoInfPath)) return;
		//ファイル情報を取得する拡張子のチェック
		if (!FExtGetInf.IsEmpty() && !test_FileExt(fp->f_ext, FExtGetInf)) return;
		//ファイル情報を取得しない拡張子のチェック
		if (test_FileExt(fp->f_ext, FExtNoInf)) return;
		//RARに対する制限
		if (LimitGetRarInf && test_FileExt(fp->f_ext, _T(".rar")) && fp->f_size>GetInfLimitSize) return;
		//処理中か?
		if (is_Processing(fp)) return;
	}

	//--------------------------
	//ファイル情報を取得
	//--------------------------
	std::unique_ptr<TStringList> i_lst(new TStringList());
	//._SF
	if (test_FileExt(fp->f_ext, _T("._sf"))) {
		add_PropLine_if(_T("パス"), get_PathFrom_SF(fp), i_list);
	}
	//その他
	else if (get_FileInfList(fp, i_lst.get()))
		i_list->AddStrings(i_lst.get());
}

//---------------------------------------------------------------------------
//指定したファイル情報の値文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_FileInfValue(
	file_rec *fp,
	UnicodeString tit,		//タイトル
	UnicodeString dlmt)		// dlmt: 値の区切り文字 (default = EmptyStr)
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
//ファイル情報の描画
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

	//基本情報
	if ((lp->Tag & LBTAG_OPT_FIF1) && Index<3) {
		cv->Font->Color = use_fgsel? col_fgSelItem : col_fgInf;
		if (Index==1)
			PathNameOut(lbuf, cv, xp, yp);
		else
			cv->TextOut(xp, yp, lbuf);
	}
	//ファイル情報
	else {
		int w_max = 0;
		int idx0 = (lp->Tag & LBTAG_OPT_FIF1)? 3: 0;
		for (int i=idx0; i<lp->Count; i++)
			w_max = std::max(w_max, get_TextWidth(cv, get_tkn(lp->Items->Strings[i], _T(": ")), is_irreg));

		//セパレータ
		if (lbuf.IsEmpty())
			draw_separateLine(cv, Rect, 1);
		//項目
		else {
			//名前
			UnicodeString namstr = split_tkn(lbuf, _T(": "));
			xp = Rect.Left + 2 + w_max - get_TextWidth(cv, namstr, is_irreg);
			cv->Font->Color = use_fgsel? col_fgSelItem :
				USTARTS_TS('.', Trim(namstr))? get_ExtColor(get_tkn_r(namstr, '.')) :
				contains_wd_i(namstr, _T("エラー|警告"))? col_Error : col_fgInfNam;
			namstr.UCAT_T(": ");
			cv->TextOut(xp, yp, namstr);	xp += get_TextWidth(cv, namstr, is_irreg);
			//内容
			if ((lp->Tag & LBTAG_OPT_FIF2) && contains_s(namstr, _T("日時:"))) {
				try {
					cv->Font->Color = use_fgsel? col_fgSelItem : get_TimeColor(TDateTime(lbuf), col_fgInf);
				}
				catch (EConvertError &e) {
					cv->Font->Color = use_fgsel? col_fgSelItem : col_fgInf;
				}
			}
			else if (USAME_TS(Trim(namstr), "タグ:"))
				cv->Font->Color = use_fgsel? col_fgSelItem : col_fgTagNam;
			else if (USAME_TS(Trim(namstr), "改行コード:"))
				cv->Font->Color = use_fgsel? col_fgSelItem : USAME_TS(lbuf, "混在")? col_Error : col_fgInf;
			else if (USAME_TS(Trim(namstr), "デバッグ状態:"))
				cv->Font->Color = use_fgsel? col_fgSelItem : contains_s(lbuf, _T("終了"))? col_Error : col_fgInf;
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

			if ((lp->Tag & LBTAG_FIF_LIST) && USAME_TS(Trim(namstr), "場所:"))
				PathNameOut(lbuf, cv, xp, yp);
			else
				EmphasisTextOut(lbuf, EmptyStr, cv, xp, yp);
		}
	}
}

//---------------------------------------------------------------------------
//カラー設定リストの描画
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

	//カラー
	TRect rc = Rect;  rc.Right = rc.Left + 30;
	cv->Brush->Color = (TColor)col_lst->Values[col_nam].ToIntDef(clBlack);
	if (cv->Brush->Color!=Graphics::clNone)
		cv->FillRect(rc);
	else {
		cv->Brush->Color = scl_BtnFace;
		cv->FillRect(rc);
		out_Text(cv, rc.Left + 2, yp, _T("無効"), scl_WindowText);
	}

	//境界線
	cv->Brush->Color = scl_Window;
	if (brk) draw_separateLine(cv, Rect);
}

//---------------------------------------------------------------------------
//入力欄の描画(疑似キャレット付き)
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

	//キャレット
	draw_Line(cv, xp, 2, xp, pp->ClientHeight-2, ScaledInt(1), col_Cursor);
}

//---------------------------------------------------------------------------
//ファイルサイズを文字列に
//---------------------------------------------------------------------------
UnicodeString get_FileSizeStr(__int64 sz, bool trim)
{
	UnicodeString ret_str = ShowByteSize ? get_size_str_B(sz, 14)
										 : get_size_str_G(sz, ((SizeFormatMode>0)? 7 : 10), SizeDecDigits, SizeFormatMode);
	if (trim) ret_str = Trim(ret_str);
	return ret_str;
}

//---------------------------------------------------------------------------
//ファイル情報文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_FileInfStr(file_rec *fp,
	bool with_atr)	//属性を付加	(default = true)
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
//拡張子別ファイル数情報を追加
//---------------------------------------------------------------------------
void add_FExtInfList(
	TStringList *f_lst,	//[i] ファイル名リスト (file_rec 付にも対応)
	TStringList *lst)	//[o] 情報リスト
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

		//追加
		if (x_lst->Count>0) {
			x_lst->Sort();
			lst->Add(EmptyStr);
			for (int i=0; i<x_lst->Count; i++)
				lst->Add(get_PropTitle(x_lst->Strings[i]).cat_sprintf(_T("%u"), (int)x_lst->Objects[i]));
		}
	}
}

//---------------------------------------------------------------------------
//ファイル情報リストを取得
//---------------------------------------------------------------------------
bool get_FileInfList(
	file_rec *fp,
	TStringList *lst)	//[o] 情報リスト
{
	if (!fp) return false;

	try {
		lst->Clear();

		UnicodeString fnam = (fp->is_virtual || fp->is_ftp)? fp->tmp_name : fp->f_name;
		if (!file_exists(fnam)) Abort();

		//----------------------
		//項目の種類
		//----------------------
		UnicodeString tnam = usr_SH->get_FileTypeStr(fnam, true);
		//NyanFi 固有のファイル
		if (SameText(fnam, IniFile->FileName))					tnam.UCAT_T(" [使用中の設定]");
		else if (SameText(fnam, rel_to_absdir(ReplaceLogName)))	tnam.UCAT_T(" [文字列置換ログ]");
		else if (SameText(fnam, rel_to_absdir(GrepFileName)))	tnam.UCAT_T(" [GREPログ]");
		else if (SameText(ExtractFilePath(fnam), ExePath)) {
			UnicodeString nnam = ExtractFileName(fnam);
			if		(SameText(nnam, TAGDATA_FILE))	tnam.UCAT_T(" [タグデータ]");
			else if (SameText(nnam, HILT_INI_FILE))	tnam.UCAT_T(" [構文強調表示定義]");
			else if (SameText(nnam, DIR_HIST_FILE))	tnam.UCAT_T(" [全体ディレクトリ履歴]");
			else if (SameText(nnam, DISTR_FILE))	tnam.UCAT_T(" [振り分け登録]");
			else if (SameText(nnam, CALC_INI_FILE))	tnam.UCAT_T(" [電卓の定義]");
			else if (SameText(nnam, WEBMAP_TPLT))	tnam.UCAT_T(" [マップ表示テンプレート]");
			else if (SameText(nnam, DRVLOG_FILE))	tnam.UCAT_T(" [ドライブ容量ログ]");
			else if (SameText(nnam, RENLOG_FILE))	tnam.UCAT_T(" [改名ログ]");
			else if (SameText(nnam, "NyanFi.exe"))	tnam.UCAT_T(" [実行中]");
			else if (is_match_regex_i(nnam, _T("^tasklog\\d?(~\\d)?\\.txt"))) tnam.UCAT_T(" [タスクログ]");
		}

		add_PropLine(_T("種類"), tnam, lst);

		//----------------------
		//タグ
		//----------------------
		if (!fp->is_virtual && !fp->is_ftp) {
			UnicodeString tags = usr_TAG->GetTags(fnam);
			if (!tags.IsEmpty()) add_PropLine(_T("タグ"), tags, lst);
		}

		UnicodeString fext = fp->f_ext;
		//------------------------------
		//関連づけされているプログラム
		//------------------------------
		if (!test_FileExt(fext, FEXT_EXECUTE FEXT_APPINFO _T(".nbt"))) {
			_TCHAR pszFile[MAX_PATH*2];
			DWORD dwOut = MAX_PATH*2;
			if (::AssocQueryString(ASSOCF_NOTRUNCATE, ASSOCSTR_EXECUTABLE,
				fext.c_str(), _T("open"), pszFile, &dwOut)==S_OK)
			{
				lst->Add(UnicodeString().sprintf(_T("%s%s  /%s"),
					get_PropTitle(_T("プログラム")).c_str(),
					ExtractFileName(pszFile).c_str(), ExtractFileDir(pszFile).c_str()));
			}
		}

		//----------------------
		//ファイル情報
		//----------------------
		std::unique_ptr<TStringList> ref_lst(new TStringList());

		if (test_AppInfExt(fext))			get_AppInf(fnam, lst);
		//アイコン
		else if (test_IcoExt(fext))			get_IconInf(fnam, lst);
		//WebP
		else if (USAME_TI(fext, ".webp"))	get_WebpInf(fnam, lst);
		//表示可能な画像
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
					if (!contains_s(lst->Text, _T("画像サイズ: ")) && get_img_size(fnam, &i_wd, &i_hi))
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
			UnicodeString tmp = "ビット レート";
			if (!contains_s(lst->Text, _T("長さ: "))) tmp.Insert("長さ,", 1);
			usr_SH->get_PropInf(fnam, lst, tmp);
		}
		//MP3
		else if (test_Mp3Ext(fext)) {
			usr_SH->get_PropInf(fnam, lst, "長さ,ビット レート");
			ID3_GetInf(fnam, lst);
		}
		//Opus
		else if (USAME_TI(fext, ".opus")) {
			usr_SH->get_PropInf(fnam, lst, "長さ");
			get_OpusInf(fnam, lst);
		}
		//CDA
		else if (USAME_TI(fext, ".cda")) {
			get_CdaInf(fnam, lst);
		}
		//ショートカット
		else if (test_LnkExt(fext)) {
			UnicodeString lnam;
			usr_SH->get_LnkInf(fnam, lst, &lnam);
			//リンク先のファイル情報
			if (!lnam.IsEmpty() && file_exists(lnam) && !dir_exists(lnam) && test_AppInfExt(ExtractFileExt(lnam))) {
				lst->Add(EmptyStr);	//セパレータ
				get_AppInf(lnam, lst);
			}
		}
		else if (USAME_TI(fext, ".url")) {
			std::unique_ptr<UsrIniFile> url_file(new UsrIniFile(fnam));
			add_PropLine(_T("URL"), url_file->ReadString("InternetShortcut", "URL"), lst);
			add_PropLine_if(_T("IconFile"), url_file->ReadString("InternetShortcut", "IconFile"), lst);
		}
		//アーカイブ
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
				add_PropLine(_T("ファイル数"), get_size_str_B(fcnt, 0), lst);
				if (org_size>0) add_PropLine(_T("元サイズ"), get_size_str_G(org_size, 8, SizeDecDigits), lst);
				if (ratio>0)	lst->Add(get_PropTitle(_T("圧縮率")).cat_sprintf(_T("%5.1f %"), ratio));
				if (is_enc)		lst->Add("パスワード保護");
				//拡張子別ファイル数
				add_FExtInfList(r_lst.get(), lst);
			}
			else {
				add_list_errmsg(lst, "不正なアーカイブです");
				InhReload++;	//エラーの繰り返しを防ぐため
			}
		}
		//XML
		else if (test_FileExt(fext, FEXT_XML)) {
			get_xml_inf(fnam, lst);
			if (test_LibExt(fext)) get_LibraryList(fnam, lst, true);
		}
		//HTML文書
		else if (test_HtmlExt(fext)) get_HtmlInf(fnam, lst);
		//スタイルシート
		else if (USAME_TI(fext, ".css")) {
			UnicodeString lbuf = Trim(get_top_line(fnam));
			if (USTARTS_TI("@charset", lbuf))
				add_PropLine(_T("charset"), exclude_quot(Trim(get_tkn(lbuf.Delete(1, 8), ';'))), lst);
		}
		//C++Builder プロジェクト、ソース
		else if (test_FileExt(fext, _T(".cbproj.dproj.cpp.pas.dfm.fmx.h")))
			get_BorlandInf(fnam, lst);
		//NyanFi コマンドファイル
		else if (test_NbtExt(fext)) {
			UnicodeString lbuf = Trim(get_top_line(fnam));
			if (remove_top_s(lbuf, ';')) add_PropLine(_T("説明"), lbuf, lst);
			get_ref_CmdFile(fnam, ref_lst.get());
		}
		//フォント
		else if (test_FontExt(fext)) get_FontInf(fnam, lst);
		//xdoc2txt
		else if (xd2tx_PropExt(fext)) xd2tx_GetInfo(fnam, lst);
		//tags
		else if (USAME_TI(fp->n_name, "tags")) get_TagsInf(fnam, lst);
		//その他
		else if (!test_FileExt(fext, _T(".pdf.nwl"))) {
			//動画
			if (USAME_TS(usr_SH->get_PropInf(fnam, lst), "ビデオ")) fp->is_video = true;
			//実行可能ファイル(チェック)
			else get_AppInf(fnam, lst);
		}

		//メモ
		UnicodeString memo = IniFile->GetMarkMemo(fp->r_name);
		if (!memo.IsEmpty()) {
			if (lst->Count>0 && !lst->Strings[lst->Count - 1].IsEmpty()) lst->Add(EmptyStr);	//セパレータ
			add_PropLine(_T("メモ"), memo, lst);
		}

		//----------------------
		//テキストプレビュー
		//----------------------
		fp->prv_text  = EmptyStr;
		fp->tail_text = EmptyStr;
		int code_page = 0;
		UnicodeString line_brk;
		bool has_bom;
		bool is_text = is_TextFile(fnam, &code_page, &line_brk, &has_bom);

		if (ShowTextPreview) {
			//リッチテキスト
			if (test_FileExt(fext, _T(".rtf.wri"))) {
				if (TempRichEdit) {
					TempRichEdit->Lines->LoadFromFile(fnam);
					fp->prv_text = TempRichEdit->Lines->Text;
					//末尾内容を設定
					if (ShowTailPreview && PrvTxtTailLn>0) {
						int tp = std::max(TempRichEdit->Lines->Count - PrvTxtTailLn, 0);
						for (int i=tp; i<TempRichEdit->Lines->Count; i++)
							fp->tail_text += TempRichEdit->Lines->Strings[i] + "\r\n";
					}
				}
			}
			//通常テキスト
			else if (is_text) {
				std::unique_ptr<TStringList> f_buf(new TStringList());
				int lmit_size = (PrvTxtInfLn>0)? std::max(PrvTxtInfLn*1024, TXT_DETECT_SIZE) : 0;		//***
				double ave_lsz = 0;
				code_page = load_text_ex(fnam, f_buf.get(), 0, lmit_size, false, NULL, &ave_lsz);
				int ln_cnt = f_buf->Count;

				//エラー
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

					//コードページ
					if (code_page>0) {
						UnicodeString cp_str = make_PropLine(_T("コードページ"), code_page);
						UnicodeString cnam	 = get_NameOfCodePage(code_page);
						if (!cnam.IsEmpty()) {
							cp_str.cat_sprintf(_T(" / %s"), cnam.c_str());
							if (has_bom) cp_str.UCAT_T(" BOM付");
						}
						lst->Add(cp_str);
					}

					//改行コード
					add_PropLine_if(_T("改行コード"), line_brk, lst);

					//行数
					bool over = (PrvTxtInfLn>0 && ln_cnt>PrvTxtInfLn);
					if (over) {
						if (ave_lsz>0 && PrvTxtInfLn>=1000) {
							int n = fp->f_size / ave_lsz;
							n -= n % (int)pow(10, floor(log10(n/10.0)));
							add_PropLine(_T("推定行数"), "約" + get_size_str_B(n, 0), lst);
						}
					}
					else {
						add_PropLine(_T("行数"),   get_size_str_B(ln_cnt, 0), lst);
						add_PropLine(_T("文字数"), get_size_str_B(f_buf->Text.Length(), 0), lst);
					}

					//プレビュー内容を設定
					if (over) {
						for (int i=0; i<PrvTxtInfLn; i++) fp->prv_text += f_buf->Strings[i] + "\r\n";
						fp->prv_text += "\r\n";	//最大行数+1にして超過判定
					}
					else
						fp->prv_text = f_buf->Text;

					//末尾内容を設定
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
			//コードページ
			if (code_page>0) {
				UnicodeString cp_str = make_PropLine(_T("コードページ"), code_page);
				UnicodeString cnam = get_NameOfCodePage(code_page);
				if (!cnam.IsEmpty()) {
					cp_str.cat_sprintf(_T(" / %s"), cnam.c_str());
					if (has_bom) cp_str.UCAT_T(" BOM付");
				}
				lst->Add(cp_str);
			}
			//改行コード
			add_PropLine_if(_T("改行コード"), line_brk, lst);
		}

		//コマンドファイルの参照情報
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
//項目は選択可能か？
//---------------------------------------------------------------------------
bool is_selectable(file_rec *fp)
{
	if (!fp)						return false;
	if (fp->is_dummy && is_separator(fp->alias))	return true;	//セパレータ
	if (fp->is_dummy || fp->is_up)	return false;
	if (fp->b_name.IsEmpty())		return false;
	return true;
}
//---------------------------------------------------------------------------
//選択可能なら選択
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
//ファイルが処理中か?
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
//取得が抑止されているパスか？
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
//UTF8チェック
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
//メモリストリームのコードページを取得
//  戻り値: コードページ (0:不明/ -1:バイナリ)
//---------------------------------------------------------------------------
int get_MemoryCodePage(
	TMemoryStream *ms,
	bool *has_bom)		//[o] BOM有り	(default = NULL)
{
	int code_page = 0;
	if (has_bom) *has_bom = false;

	int src_size = ms->Size;
	if (src_size==0) return 932;

	//予備判定
	BYTE *bp = (BYTE*)ms->Memory;
	int x00_cnt	= 0;	//0x00 の個数
	int z_rpt	= 0;	//0x00 の連続数
	int z_max	= 0;	//0x00 の最大連続数
	int esc_cnt = 0;	//ESC の個数
	int asc_cnt = 0;	//ASCII の個数
	int ltn_cnt = 0;	//ISO8859(Latin-1を想定) の個数
	int b7_cnt	= 0;	//b7==1 の個数

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
	if (z_max>3 || (src_size<3 && x00_cnt>0)) return -1;	//バイナリー

	//BOMの有無をチェック
	if		(bp[0]==0xfe && bp[1]==0xff)				code_page = 1201;		//UTF-16BE BOM付
	else if (bp[0]==0xff && bp[1]==0xfe)				code_page = 1200;		//UTF-16 (LE) BOM付
	else if (bp[0]==0xef && bp[1]==0xbb && bp[2]==0xbf)	code_page = CP_UTF8;	//UTF-8 BOM付

	if (code_page>0) {
		if (has_bom) *has_bom = true;
		return code_page;
	}

	//BOM無し
	if (check_UTF8(bp, src_size)>0) code_page = CP_UTF8;	//UTF-8 BOM無し
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

	//UTF-16か?
	int be_asc = 1, le_asc = 1;
	int be_hk  = 1, le_hk  = 1;		//ひらがな、カタカナ
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
//メモリストリームのコードページを判定し、文字列として取得
//---------------------------------------------------------------------------
UnicodeString get_MemoryStrins(TMemoryStream *ms)
{
	//コードページ取得
	ms->Seek(0, soFromBeginning);
	int code_page = get_MemoryCodePage(ms);
	if (code_page==0) code_page = 932;

	//バッファに読み込み
	TBytes Bytes;
	Bytes.Length = ms->Size;
	ms->Seek(0, soFromBeginning);
	ms->Read(Bytes, ms->Size);

	//エンコード
	std::unique_ptr<TStringList> lst(new TStringList());
	std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
	return enc->GetString(Bytes, 0, Bytes.Length);
}

//---------------------------------------------------------------------------
//ストリームの改行コードを取得
//---------------------------------------------------------------------------
UnicodeString get_StreamLineBreak(
	TStream *ms,
	int  code_page,
	bool strict)	//完全チェック	(default = false);
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
				//完全チェック(拡張検索中)の場合は中断を受け付ける
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
	if ((rn_cnt + r_cnt + n_cnt)>0)			return "混在";
	if (strict)								return "無し";
											return EmptyStr;
}

//---------------------------------------------------------------------------
//ファイルのコードページを取得
//  戻り値: コードページ (0: 不明)
//---------------------------------------------------------------------------
int get_FileCodePage(
	UnicodeString fnam, 
	UnicodeString *line_brk,	//[o] 改行		(default = NULL)
	bool *has_bom)				//[o] BOM有り	(default = NULL)
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

	//その他
	if (code_page==0 || line_brk || has_bom) {
		try {
			std::unique_ptr<TFileStream>   fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
			std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
			ms->CopyFrom(fs.get(), std::min<__int64>(fs->Size, TXT_DETECT_SIZE));
			int cpag = get_MemoryCodePage(ms.get(), has_bom);
			if (code_page==0) code_page = cpag;
			//改行コード
			if (line_brk) *line_brk = get_StreamLineBreak(ms.get(), cpag);
		}
		catch (...) {
			;
		}
	}
	return code_page;
}

//---------------------------------------------------------------------------
//ファイルはテキストか?
//---------------------------------------------------------------------------
bool is_TextFile(
	UnicodeString fnam,
	int *code_page,				//[o] コードページ	(default = NULL)
	UnicodeString *line_brk,	//[o] 改行			(default = NULL)
	bool *has_bom)				//[o] BOM有り		(default = NULL)
{
	if (!file_exists(fnam)) return false;

	//明らかにテキストではない拡張子ははねる
	if (test_NonTxtExt(get_extension(fnam))) return false;

	//コードページを調べる(取得できなかったら非テキストとみなす)
	int cp = get_FileCodePage(fnam, line_brk, has_bom);
	if (code_page) *code_page = cp;
	return (cp>0);
}

//---------------------------------------------------------------------------
//文字列リストの行数、コメント、空白行をカウント
//  戻り値: 総行数
//---------------------------------------------------------------------------
int CountListLines(
	TStringList *lst,		//文字列リスト
	UnicodeString fnam,		//ファイル名
	int *rem_cnt,			//[o] コメント
	int *blk_cnt)			//[o] 空白行
{
	*blk_cnt = 0;

	std::unique_ptr<TStringList> ln_lst(new TStringList());	//１行コメント
	std::unique_ptr<TStringList> bg_lst(new TStringList());	//ブロック開始
	std::unique_ptr<TStringList> ed_lst(new TStringList());	//ブロック終了
	//コメントあり
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
	//コメント無し
	else {
		*rem_cnt = -1;
		for (int i=0; i<lst->Count; i++) {
			if (Trim(lst->Strings[i]).IsEmpty()) (*blk_cnt)++;
		}
	}

	return lst->Count;
}

//---------------------------------------------------------------------------
//実行可能ファイルか? (プラグインなどのチェック用)
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
//ビデオ/オーディオ・ファイルか？
//---------------------------------------------------------------------------
bool is_AudioVideo(UnicodeString fnam)
{
	if (test_FileExt(ExtractFileExt(fnam), FEXT_DURATION)) return true;
	if (contained_wd_i(_T("ビデオ|オーディオ"), usr_SH->get_PropInf(fnam))) return true;
	return false;
}

//---------------------------------------------------------------------------
//ビュアーで表示可能か？ (アイコンは除く)
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
//アイコン抽出可能か?
//---------------------------------------------------------------------------
bool is_ExtractIcon(file_rec *fp)
{
	return test_FileExt(fp->f_ext, FEXT_ICONVIEW);
}

//---------------------------------------------------------------------------
//アーカイブの拡張子か?
//---------------------------------------------------------------------------
bool test_ArcExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_ARCHIVE) || (usr_ARC->Use7zDll && test_FileExt(fext, FExt7zDll));
}
//---------------------------------------------------------------------------
//対応アーカイブ(SPIを含む)の拡張子か?
//---------------------------------------------------------------------------
bool test_ArcAxExt(UnicodeString fext)
{
	return (test_ArcExt(fext) || SPI->TestFExt(fext, true));
}
//---------------------------------------------------------------------------
//Exif情報取得可能の拡張子か?
//---------------------------------------------------------------------------
bool test_ExifExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_EXIF);
}
//---------------------------------------------------------------------------
//HTML文書の拡張子か?
//---------------------------------------------------------------------------
bool test_HtmlExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_HTML);
}
//---------------------------------------------------------------------------
//フォントの拡張子か?
//---------------------------------------------------------------------------
bool test_FontExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_FONT);
}
//---------------------------------------------------------------------------
//ライブラリの拡張子か?
//---------------------------------------------------------------------------
bool test_LibExt(UnicodeString fext)
{
	return USAME_TI(fext, ".library-ms");
}
//---------------------------------------------------------------------------
//.exe か?
//---------------------------------------------------------------------------
bool test_ExeExt(UnicodeString fext)
{
	return USAME_TI(fext, ".exe");
}
//---------------------------------------------------------------------------
//ショートカットの拡張子か?
//---------------------------------------------------------------------------
bool test_LnkExt(UnicodeString fext)
{
	return USAME_TI(fext, ".lnk");
}
//---------------------------------------------------------------------------
//アイコンの拡張子か?
//---------------------------------------------------------------------------
bool test_IcoExt(UnicodeString fext)
{
	return USAME_TI(fext, ".ico");
}
//---------------------------------------------------------------------------
//Jpegの拡張子か?
//---------------------------------------------------------------------------
bool test_JpgExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_JPEG);
}
//---------------------------------------------------------------------------
//GIFの拡張子か?
//---------------------------------------------------------------------------
bool test_GifExt(UnicodeString fext)
{
	return USAME_TI(fext, ".gif");
}
//---------------------------------------------------------------------------
//PNGの拡張子か?
//---------------------------------------------------------------------------
bool test_PngExt(UnicodeString fext)
{
	return USAME_TI(fext, ".png");
}
//---------------------------------------------------------------------------
//メタファイルの拡張子か?
//---------------------------------------------------------------------------
bool test_MetaExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_META);
}
//---------------------------------------------------------------------------
//MP3の拡張子か?
//---------------------------------------------------------------------------
bool test_Mp3Ext(UnicodeString fext)
{
	return USAME_TI(fext, ".mp3");
}
//---------------------------------------------------------------------------
//MSI対応サウンドの拡張子か?
//---------------------------------------------------------------------------
bool test_MciSndExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_MCISND);
}

//---------------------------------------------------------------------------
//コマンドファイルの拡張子か?
//---------------------------------------------------------------------------
bool test_NbtExt(UnicodeString fext)
{
	return USAME_TI(fext, ".nbt");
}
//---------------------------------------------------------------------------
//ワークリストの拡張子か?
//---------------------------------------------------------------------------
bool test_NwlExt(UnicodeString fext)
{
	return USAME_TI(fext, ".nwl");
}

//---------------------------------------------------------------------------
//テキストの拡張子か?
//---------------------------------------------------------------------------
bool test_TxtExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_TEXT);
}
//---------------------------------------------------------------------------
//テキストファイルか? (拡張子 or ファイル名で判定)
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
//非テキストの拡張子か?
//---------------------------------------------------------------------------
bool test_NonTxtExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_NONTEXT + WicFextStr);
}

//---------------------------------------------------------------------------
//アプリ情報取得可能な拡張子か?
//---------------------------------------------------------------------------
bool test_AppInfExt(UnicodeString fext)
{
	return test_FileExt(fext, FEXT_APPINFO);
}

//---------------------------------------------------------------------------
//ファイルから画像サイズを取得
//---------------------------------------------------------------------------
bool get_img_size(UnicodeString fnam, unsigned int *wd, unsigned int *hi)
{
	bool size_ok = WIC_get_img_size(fnam, wd, hi) || SPI->GetImgSize(fnam, wd, hi);

	if (!size_ok && test_ExifExt(get_extension(fnam))) {
		std::unique_ptr<TStringList> i_lst(new TStringList());	//ファイル情報用
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
//画像ファイルを読み込む
//戻り値 失敗 = 0/  成功 = LOADED_BY_STD、LOADED_BY_WIC、LOADED_BY_SPI
//---------------------------------------------------------------------------
int load_ImageFile(
	UnicodeString fnam, Graphics::TBitmap *o_bmp,
	int wic_type,		//				(default = WICIMG_PREVIEW)
	TColor trans_bg)	//透過Gの背景色	(default = clNone)
{
	int res = 0;
	try {
		o_bmp->Canvas->Lock();
		try {
			if (fnam.IsEmpty()) Abort();
			UnicodeString fext = get_extension(fnam);
			//PNGは独自に処理(アルファチャネル対応)
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
			//SPI優先ならSPIで
			else if (UseSpiFirst && SPI->LoadImage(fnam, o_bmp))
				res = LOADED_BY_SPI;
			//WICで
			else if (WIC_load_image(fnam, o_bmp, wic_type))
				res = test_FileExt(fext, FEXT_WICSTD)? LOADED_BY_STD : LOADED_BY_WIC;
			//SPI優先でないならSPIで
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
//ViewFileList で有効な項目数を取得
//---------------------------------------------------------------------------
int get_ViewCount()
{
	int cnt = 0;
	for (int i=0; i<ViewFileList->Count; i++)
		if (!((file_rec*)ViewFileList->Objects[i])->failed) cnt++;
	return cnt;
}

//---------------------------------------------------------------------------
//ファイルの削除
//---------------------------------------------------------------------------
bool delete_File(
	UnicodeString fnam,
	bool use_trash)		//ごみ箱を使用	(default = false)
{
	bool res;
	//ごみ箱に入れる
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
	//削除
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
//コマンド部分を取得
//---------------------------------------------------------------------------
UnicodeString get_CmdStr(UnicodeString s)
{
	return starts_AT(s)? s : get_tkn(s, '_');
}
//---------------------------------------------------------------------------
//パラメータ部分を取得
//---------------------------------------------------------------------------
UnicodeString get_PrmStr(UnicodeString s)
{
	return get_tkn_r(s, '_');
}

//---------------------------------------------------------------------------
//キーに割り当てられているコマンドを取得
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
//コマンドに対する説明文字列を取得
//---------------------------------------------------------------------------
UnicodeString get_CmdDesc(
	UnicodeString cmd,
	bool only_inf,			//説明のみ取得 (default = false)
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

	//説明
	UnicodeString dsc = split_dsc(prm);
	if (dsc.IsEmpty()) {
		if (contained_wd_i(_T("ExeExtMenu|ExeExtTool"), cmd)) {
			//該当するメニュー項目を取得
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
		//エイリアス
		else if (starts_Dollar(cmd)) {
			UnicodeString alias = exclude_top(cmd);
			//追加メニュー
			for (int i=0; i<menu_list->Count && dsc.IsEmpty(); i++) {
				TStringDynArray itm_buf = get_csv_array(menu_list->Strings[i], EXTMENU_CSVITMCNT, true);
				if (!SameText(alias, itm_buf[3])) continue;
				dsc = REPLACE_TS(itm_buf[0], "&", "");
				break;
			}
			//外部ツール
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
//コマンドパラメータから説明文字列を削除
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
//ColorList から個々の配色を取得
//---------------------------------------------------------------------------
void set_col_from_ColorList()
{
	struct {
		TColor *vp;			//変数へのポインタ
		const _TCHAR *key;	//キー名=デフォルト
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
//編集履歴を追加
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
//呼び出しホットキーの登録
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
		case ID_CALL_HOTKEY: GlobalErrMsg.UCAT_T("\r\n「NyanFi 呼び出し」の設定を確認してください。");	break;
		case ID_APP_HOTKEY:  GlobalErrMsg.UCAT_T("\r\n「AppList コマンド」の設定を確認してください。");	break;
		}
		return false;
	}
}

//---------------------------------------------------------------------------
//フォームのタイトルを設定
//---------------------------------------------------------------------------
void set_FormTitle(TForm *frm, const _TCHAR *s)
{
	frm->Caption = s;
}

//---------------------------------------------------------------------------
//確認メッセージ
//---------------------------------------------------------------------------
bool msgbox_Sure(UnicodeString msg, bool ask)
{
	if (!ask) return true;
	unsigned int flag = (SureCancel? MB_YESNOCANCEL : MB_YESNO) | MB_ICONQUESTION;
	if (SureDefNo) flag |= MB_DEFBUTTON2;
	return (Application->MessageBox(msg.c_str(), _T("確認"), flag)==IDYES);
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
//警告音
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
//タイトルバーの点滅
//---------------------------------------------------------------------------
void flash_win(
	int cnt,	//点滅回数 default = 3
	int tm)		//点滅間隔 default = 250
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
//タイトルバーの閉じるボタンを有効/無効化
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
//音量ミュート
//---------------------------------------------------------------------------
bool mute_Volume(
	UnicodeString prm)	// ON/OFF/GET(取得のみ) 空の場合はトグル
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
//サウンド再生
// ファイル名は実行パスからの相対指定でも可
// ファイルが存在しないときは何も鳴らさない
//---------------------------------------------------------------------------
bool play_sound(UnicodeString fnam)
{
	fnam = rel_to_absdir(fnam);
	if (!file_exists(fnam)) return false;

	::sndPlaySound(fnam.c_str(), SND_ASYNC);
	return true;
}

//---------------------------------------------------------------------------
//MCIによるサウンド再生(WAV、WMA、MP3、MID)
//！メイン側で MM_MCINOTIFY を処理
//---------------------------------------------------------------------------
bool play_sound_ex(
	UnicodeString fnam,		//ファイル名(実行パスからの相対指定でも可)
	bool limit)				//秒数制限する (default = true)
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
//プレイリストに追加
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

	//ディレクトリ
	if (dir_exists(lnam)) {
		std::unique_ptr<TStringList> fbuf(new TStringList());
		get_files(lnam, _T("*.*"), fbuf.get(), true);
		for (int i=0; i<fbuf->Count; i++) add_PlayFile(fbuf->Strings[i]);
	}
	//ファイル
	else if (file_exists(lnam)) {
		//サウンドファイル
		if (add_PlayFile(lnam))
			;
		//リストファイル
		else if (is_TextFile(lnam)) {
			std::unique_ptr<TStringList> fbuf(new TStringList());
			if (load_text_ex(lnam, fbuf.get())!=0) {
				PlayListFile = lnam;
				for (int i=0; i<fbuf->Count; i++) {
					UnicodeString inam = get_pre_tab(fbuf->Strings[i]);
					//ディレクトリ
					if (dir_exists(inam)) {
						std::unique_ptr<TStringList> fbuf(new TStringList());
						get_files(inam, _T("*.*"), fbuf.get(), true);
						for (int i=0; i<fbuf->Count; i++) add_PlayFile(fbuf->Strings[i]);
					}
					//ファイル
					else if (file_exists(inam)) add_PlayFile(inam);
				}
			}
		}
	}

	return (PlayList->Count>last_cnt);
}

//---------------------------------------------------------------------------
//プレイリストを再生
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
		//シャッフル
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
//オーディオ/ビデオファイルの長さ(ms)を取得
//---------------------------------------------------------------------------
int get_duration(UnicodeString fnam)
{
	int t = -1;
	UnicodeString fext = get_extension(fnam);

	//MCI で取得
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

	//プロパティから取得
	if (t==-1) t = usr_SH->get_Duration(fnam) * 1000;

	return t;
}

//---------------------------------------------------------------------------
//タブを考慮して文字列幅を取得
//---------------------------------------------------------------------------
int get_TabTextWidth(
	UnicodeString s, TCanvas *cv,
	int tab_wd)		//タブストップ幅
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
//文字列の描画
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
//マッチ語リストを取得
//---------------------------------------------------------------------------
int get_MatchWordList(
	UnicodeString lbuf,	//対象文字列
	UnicodeString kwd,	//検索語 (空白区切りによるAND検索対応)
	bool migemo_sw,		//Migemoを使用
	bool regex_sw,		//正規表現
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
//語の強調表示 (URL強調有り)
//---------------------------------------------------------------------------
void EmphasisTextOut(
	UnicodeString s,		//表示文字列
	TStringList *kw_lst,	//強調語のリスト
	TCanvas *cv,
	int &x,				//[i/o] 表示X位置
	int y,				//[i]	表示Y位置
	bool case_sns,		//大小文字を区別   (default = false)
	bool only_top,		//先頭の語だけ強調 (default = false)
	TColor fg,			//強調文字色 (default = col_fgEmp)
	TColor bg)			//強調背景色 (default = col_bgEmp)
{
	if (s.IsEmpty()) return;

	int s_len = s.Length();

	std::unique_ptr<TColor[]> FgCol(new TColor[s_len + 1]);
	std::unique_ptr<TColor[]> BgCol(new TColor[s_len + 1]);

	for (int i=0; i<=s_len; i++) {	//0 は 現在色
		FgCol[i] = cv->Font->Color;
		BgCol[i] = cv->Brush->Color;
	}

	//URL強調
	if (s.Pos("://")) {	//URL 処理時間短縮のための前チェック
		TMatch mt = TRegEx::Match(s, URL_MATCH_PTN);
		if (mt.Success) for (int j=0,p=mt.Index; j<mt.Length; j++,p++) FgCol[p] = col_URL;
	}

	//キーワード強調
	if (kw_lst) {
		int ofs = 1;
		for (;;) {
			int p1 = 0;	//見つかった語の末尾位置 + 1 の最後尾
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

	//コントロールコード
	for (int i=1; i<=s_len; i++) {
		if (iscntrl(s[i]) && (s[i]!='\t')) {
			s[i] = (WideChar)((int)s[i] + 0x40);
			FgCol[i] = col_Ctrl;
		}
	}

	//文字列描画
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
	UnicodeString kwd,	//強調語
	TCanvas *cv,
	int &x,				//[i/o] 表示X位置
	int y,				//[i]	表示Y位置
	bool case_sns,		//大小文字を区別   (default = false)
	bool only_top,		//先頭の語だけ強調 (default = false)
	TColor fg,			//強調文字色 (default = col_fgEmp)
	TColor bg)			//強調背景色 (default = col_bgEmp)
{
	std::unique_ptr<TStringList> kwd_lst(new TStringList());
	kwd_lst->Text = kwd;
	EmphasisTextOut(s, kwd_lst.get(), cv, x, y, case_sns, only_top, fg, bg);
}

//---------------------------------------------------------------------------
//タブや改行を含む文字列の描画
//---------------------------------------------------------------------------
void TabCrTextOut(
	UnicodeString s,	//表示文字列
	TCanvas *cv,
	int &x,				//[i/o] 表示X位置
	int y,				//[i]	表示Y位置
	TColor fg,			//文字色
	int max_x)			//制限位置 (default = 0 : 無視)
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
		//文字列
		int p1 = mts.Item[i].Index;
		if (p1>p0) {
			sbuf = s.SubString(p0, p1 - p0);
			cv->Font->Color = fg;
			int w = get_TextWidth(cv, sbuf, is_irreg);
			cv->TextRect(Rect(x, y, x + w, yh), x, y, sbuf);
			x += w;
		}
		//タブ or 改行
		bool is_tab = USAME_TS(s.SubString(p1, 1), "\t");
		sbuf = is_tab? L"\u21c0" : L"\u21b5";
		cv->Font->Color = is_tab? col_TAB : col_CR;
		cv->TextOut(x, y, sbuf);
		cv->Font->Color = fg;
		x += get_TextWidth(cv, sbuf, is_irreg);
		if (max_x>0 && x>=max_x) break;
		p0 = p1 + 1;
	}

	//文字列
	if (p0<=slen && (max_x==0 || x<max_x)) {
		sbuf = s.SubString(p0, slen - p0 + 1);
		cv->Font->Color = fg;
		int w = get_TextWidth(cv, sbuf, is_irreg);
		cv->TextRect(Rect(x, y, x + w, yh), x, y, sbuf);
		x += w;
	}
}

//---------------------------------------------------------------------------
//罫線、タブを含む行の描画
//---------------------------------------------------------------------------
void RuledLnTextOut(
	UnicodeString s,		//表示文字列
	TCanvas *cv,
	TRect &rc,				//表示位置 (rc.Left 更新)
	TColor fg,				//文字色
	int tab_wd,				//タブ幅			(default = 8)
	TStringList *kw_lst)	//強調語のリスト	(default = NULL)
{
	bool is_irreg = IsIrregularFont(cv->Font);

	int slen = s.Length();
	int xp	 = rc.Left + 2;
	int yp	 = rc.Top + 1;
	UnicodeString sbuf;

	//タブ幅を計算
	TEXTMETRIC tm;
	::GetTextMetrics(cv->Handle , &tm);
	int w_tab = tab_wd * tm.tmAveCharWidth;

	TMatchCollection mts = TRegEx::Matches(s, "[\t―─│┌┐┘└├┬┤┴┼]");
	int p0 = 1;
	for (int i=0; i<mts.Count && p0<=slen; i++) {
		//文字列
		int p1 = mts.Item[i].Index;
		if (p1>p0) {
			sbuf = s.SubString(p0, p1 - p0);
			cv->Font->Color = fg;
			EmphasisTextOut(sbuf, kw_lst, cv, xp, yp);
		}
		//罫線
		sbuf = s.SubString(p1, 1);
		cv->Pen->Style = psSolid;
		cv->Pen->Width = 1;
		cv->Pen->Color = col_HR;
		int w  = get_TextWidth(cv, sbuf, is_irreg);
		int xc = xp + w/2;
		int yc = rc.Top + (rc.Bottom - rc.Top) / 2;
		if (starts_tchs(_T("│├┤"), sbuf)) {
			cv->MoveTo(xc, rc.Top);  cv->LineTo(xc, rc.Bottom);	//縦線
			if (starts_tchs(_T("├┤"), sbuf)) {
				cv->MoveTo(xc, yc); cv->LineTo(USAME_TS(sbuf, "├")? xc + w : xp - 1, yc);
			}
		}
		else if (starts_tchs(_T("―─┼┬┴"), sbuf)) {
			cv->MoveTo(xp, yc);  cv->LineTo(xp + w, yc);	//横線
			if (USAME_TS(sbuf, "┼")) {
				cv->MoveTo(xc, rc.Top);  cv->LineTo(xc, rc.Bottom);
			}
			else if (starts_tchs(_T("┬┴"), sbuf)) {
				cv->MoveTo(xc, yc); cv->LineTo(xc, USAME_TS(sbuf, "┬")? rc.Bottom : rc.Top - 1);
			}
		}
		else if (starts_tchs(_T("┌┐"), sbuf)) {
			cv->MoveTo(xc, rc.Bottom); cv->LineTo(xc, yc);
			cv->LineTo(USAME_TS(sbuf, "┌")? xp + w : xp - 1, yc);
		}
		else if (starts_tchs(_T("└┘"), sbuf)) {
			cv->MoveTo(xc, rc.Top);  cv->LineTo(xc, yc);
			cv->LineTo(USAME_TS(sbuf, "└")? xp + w : xp - 1, yc);
		}
		//タブ
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

	//文字列
	if (p0<=slen && xp<rc.Right) {
		sbuf = s.SubString(p0, slen - p0 + 1);
		cv->Font->Color = fg;
		EmphasisTextOut(sbuf, kw_lst, cv, xp, yp);
	}

	rc.Left = std::min(xp, (int)rc.Right);
}

//---------------------------------------------------------------------------
//テキストプレビューを描画
//---------------------------------------------------------------------------
void PrvTextOut(
	TListBox *lp,			//リストボックス
	int idx,				//インデックス
	TCanvas *cv, TRect &rc,
	TColor fg,				//文字色
	int tab_wd,				//タブ幅
	TStringList *kw_lst,	//強調語のリスト			(default = NULL)
	UnicodeString fnam,		//コメント判定用ファイル名	(default = EmptyStr)
	bool en_mlt)			//複数行コメントに対応		(default = true)
{
	//コメントの判定
	int p = (EmpComment && fg!=col_fgSelItem)?
				UserHighlight->GetCommentPos(fnam, lp, idx, en_mlt) : 0;

	UnicodeString s = lp->Items->Strings[idx];
	if (s.Length()>1024) s = s.SubString(1, 1024).UCAT_T("…");	//*** 文字数制限

	//コメント有り
	if (p>0 && p<=s.Length()) {
		if (p>1) RuledLnTextOut(s.SubString(1, p - 1), cv, rc, fg, tab_wd, kw_lst);
		RuledLnTextOut(s.SubString(p, s.Length() - p + 1), cv, rc, col_Comment, tab_wd, kw_lst);
	}
	//コメント無し
	else {
		RuledLnTextOut(s, cv, rc, fg, tab_wd, kw_lst);
	}
}

//---------------------------------------------------------------------------
//行番号を描画
//---------------------------------------------------------------------------
void LineNoOut(TCanvas *cv, TRect &rc, UnicodeString l_str)
{
	//背景
	int w_ln = get_CharWidth(cv, 6, ScaledInt(4));
	TRect l_rc = rc;
	cv->Brush->Color = col_bgLineNo;
	l_rc.Right = l_rc.Left + w_ln;
	cv->FillRect(l_rc);

	//番号
	if (!l_str.IsEmpty()) {
		cv->Font->Color = col_LineNo;
		cv->TextOut(
			l_rc.Left + w_ln - get_TextWidth(cv, l_str, IsIrregularFont(cv->Font)) - get_CharWidth(cv, 1)/2,
			l_rc.Top + 1,
			l_str);
	}

	//境界線
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
//パス名を描画
//  \ → /、明度調整
//---------------------------------------------------------------------------
void PathNameOut(
	UnicodeString s,		//表示文字列
	TStringList *kw_lst,	//強調語のリスト (不要ならNULL)
	TCanvas *cv,
	int &x, int y,			//表示位置 (x は更新)
	bool mgn_sw)			//true = / の前後にマージンを入れる (default = true)
{
	if (s.IsEmpty()) return;

	int s_len = s.Length();
	std::unique_ptr<TColor[]> FgCol(new TColor[s_len + 1]);
	std::unique_ptr<TColor[]> BgCol(new TColor[s_len + 1]);

	for (int i=0; i<=s_len; i++) {	//0 は 現在色
		FgCol[i] = cv->Font->Color;
		BgCol[i] = cv->Brush->Color;
	}

	//キーワード強調
	if (kw_lst) {
		int ofs = 1;
		for (;;) {
			int p1 = 0;	//見つかった語の末尾位置 + 1 の最後尾
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

	//文字列描画
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
	int &x, int y,		//表示位置 (x は更新)
	int w)				//制限幅 (default = 0 : 無制限)
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
//ファイル名を描画
//---------------------------------------------------------------------------
void FileNameOut(
	TCanvas *cv, TRect &rc,
	UnicodeString fnam,		//ファイル名
	bool use_fgsel,			//選択色を使用
	bool to_slash,			//\ を / に置換	(default = false)
	TStringList *kw_lst)	//強調語のリスト	(default = NULL)
{
	UnicodeString dnam = ExtractFilePath(fnam);
	//ディレクトリ名
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

	//ファイル名
	RuledLnTextOut(fnam, cv, rc, use_fgsel? col_fgSelItem : get_ExtColor(get_extension(fnam)), 8, kw_lst);
}

//---------------------------------------------------------------------------
//ステータスバーの背景を描画
//---------------------------------------------------------------------------
bool draw_SttBarBg(TStatusBar *sp, TMessage &msg)
{
	if (col_bgSttBar==scl_BtnFace) return false;

	std::unique_ptr<TCanvas> cv(new TCanvas());
	cv->Handle = (HDC)msg.WParam;
	cv->Brush->Color = col_bgSttBar;
	cv->FillRect(sp->ClientRect);
	//上端境界
	cv->Pen->Color = SelectWorB(col_bgSttBar, 0.25);
	cv->Pen->Width = 1;
	cv->Pen->Style = psSolid;
	cv->MoveTo(0, 0);
	cv->LineTo(sp->ClientWidth, 0);

	msg.Result = 1;
	return true;
}
//---------------------------------------------------------------------------
//情報ヘッダの背景を描画
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
//ヘッダーセクションの描画(ソート方向マーク付)
//---------------------------------------------------------------------------
void draw_SortHeader(
	THeaderControl *hp, THeaderSection *sp,
	TRect rc,
	int mk_mode, 		//0:無し/ 1:▲/ -1:▼
	bool use_syscol)	//システム色を使用		(default = false : 設定色)
{
	TCanvas *cv = hp->Canvas;
	cv->Font->Assign(hp->Font);
	int xp = rc.Left + 4;
	int yp = rc.Top + get_TopMargin(cv) + 1;

	cv->Brush->Color = use_syscol? scl_BtnFace : col_bgListHdr;
	cv->Font->Color  = use_syscol? scl_BtnText : col_fgListHdr;

	//背景
	cv->FillRect(rc);
	//テキスト
	if (rc.Width()>8) cv->TextOut(xp, yp, sp->Text);	//$$$

	//区切り線
	if (sp->Index < hp->Sections->Count-1) {
		cv->Pen->Style = psSolid;
		cv->Pen->Width = 1;
		cv->Pen->Color = SelectWorB(cv->Brush->Color, 0.25);
		cv->MoveTo(rc.Right - 2, rc.Top);  cv->LineTo(rc.Right - 2, rc.Bottom);
	}

	//マーク
	if (mk_mode!=0) {
		xp += cv->TextWidth(sp->Text) + 4;
		draw_SortMark(cv, xp, yp + 1, (mk_mode==1), cv->Font->Color);
	}
}

//---------------------------------------------------------------------------
//リストボックスのラインカーソルを描画
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
	//カーソル
	bool is_focused = State.Contains(odFocused);
	if (Index==lp->ItemIndex && (is_focused || !lp->Focused())) {
		int lw = is_focused? std::max(CursorWidth, 1) : 1;
		int yp = Rect.Bottom - lw;
		draw_Line(lp->Canvas, Rect.Left, yp, Rect.Right, yp, lw, col_Cursor, is_focused? psSolid : psDot);
	}
	if (is_focused) lp->Canvas->DrawFocusRect(Rect);
}
//---------------------------------------------------------------------------
//グリッドのラインカーソルを描画
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
//画像分割グリッドを描画
//---------------------------------------------------------------------------
void draw_ImgGrid(TCanvas *cv, Graphics::TBitmap *bmp, bool ext_ln)
{
	//分割グリッド
	cv->Pen->Style = psSolid;
	cv->Pen->Width = 1;
	cv->Pen->Color = col_ImgGrid;
	//横
	for (int i=1; i<ImgGridHorzN; i++) {
		int x = bmp->Width/ImgGridHorzN * i;
		cv->MoveTo(x, 0);  cv->LineTo(x, bmp->Height);
	}
	//縦
	for (int i=1; i<ImgGridVertN; i++) {
		int y = bmp->Height/ImgGridVertN * i;
		cv->MoveTo(0, y);  cv->LineTo(bmp->Width, y);
	}
	//外形線
	if (ext_ln) {
		cv->Brush->Color = col_ImgGrid;
		cv->FrameRect(Rect(0, 0, bmp->Width, bmp->Height));
	}
}

//---------------------------------------------------------------------------
//バーグラフの描画
//  0〜50% 緑→赤 100%
//---------------------------------------------------------------------------
void draw_BarGraph(TCanvas *cv, TRect rc, double r)
{
	TColor org_col		= cv->Brush->Color;
	TBrushStyle org_stl = cv->Brush->Style;

	//背景
	cv->Brush->Color = col_bgPrgBar;
	cv->FillRect(rc);

	//バー
	rc.Right = rc.Left + rc.Width() * r;
	cv->Brush->Color =
		(r<=0.5)? clLime : TColor(RGB(std::min((int)((r - 0.5)*1020), 255), std::min((int)((1.0 - r)*1020), 255), 0));
	cv->FillRect(rc);

	cv->Brush->Color = org_col;
	cv->Brush->Style = org_stl;
}

//---------------------------------------------------------------------------
//進捗バーの描画
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
//下部タブの描画
//※クラシック以外のテーマで下部タブが正しく描画されない不具合の対策
//---------------------------------------------------------------------------
void draw_BottomTab(TCustomTabControl *Control, int idx, const TRect rc, bool active)
{
	TTabControl *tp = (TTabControl*)Control;
	TCanvas *cv = tp->Canvas;
	//背景
	cv->Brush->Color = scl_BtnFace;
	cv->FillRect(rc);
	//文字
	UnicodeString titstr = tp->Tabs->Strings[idx];
	cv->Font->Color = scl_BtnText;
	cv->TextOut(
		rc.Left + (rc.Width() - cv->TextWidth(titstr))/2,
		rc.Bottom - cv->TextHeight(titstr) - 4,
		titstr);
}

//---------------------------------------------------------------------------
//テキストをファイルに保存
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
//文字コードを判定(指定)してテキストファイルを読み込む
//  戻り値: コードページ
//  制限有りの場合、最終行が TXLIMIT_MARK
//---------------------------------------------------------------------------
int load_text_ex(
	UnicodeString fnam,			//ファイル名
	TStringList *f_buf,			//取得バッファ
	int code_page,				//コードページ		(default = 0 : 取得)
	int limit_size,				//最大読込サイズ	(default = 0 : 無制限)
	bool force_txt,				//NULL文字を空白に置換して強制読込 サイズ制限時のみ有効 (default = false)
	UnicodeString *line_brk,	//[o] 改行コード	(default = NULL);
	double *ave_lsz)			//[o] 平均行サイズ	(制限時のみ)
{
	GlobalErrMsg = EmptyStr;
	f_buf->Clear();

	try {
		//コードページを取得
		if (code_page==0) code_page = get_FileCodePage(fnam);

		//メモリストリームにサイズ制限して読み込む
		if (limit_size>0) {
			std::unique_ptr<TFileStream>   fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
			std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
			ms->CopyFrom(fs.get(), std::min<__int64>(fs->Size, limit_size));

			//UTF8の場合、文字の途中で切れていると読み込みに失敗するため、端数バイトを切り詰める
			if (code_page==65001 && ms->Size<fs->Size) {
				BYTE mbuf[8];
				ms->Seek(-8, soFromEnd);
				if (ms->Read(mbuf, 8)==8) {
					int n = 0;
					for (int i=7; i>=0; i--,n++) {
						if ((mbuf[i] & 0x80)==0x00) break;	//1バイト文字
						if ((mbuf[i] & 0xc0)==0xc0) {		//先頭バイト
							n++; break;
						}
					}
					if (n>0) ms->Size = ms->Size - n;
				}
			}

			//強制表示のために NULL文字を空白に置換
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
				//その他
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

			//改行コード
			if (line_brk) *line_brk = force_txt? EmptyStr : get_StreamLineBreak(ms.get(), code_page);

			//テキストとして読み込む
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
					f_buf->Add(TXLIMIT_MARK);	//行数制限マーク
				}
			}

			if (ave_lsz && f_buf->Count>0) *ave_lsz = 1.0 * ms->Size / f_buf->Count;
		}
		//すべて読み込む
		else {
			//改行コード
			if (!force_txt && line_brk) {
				std::unique_ptr<TFileStream>   fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));
				std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
				ms->CopyFrom(fs.get(), std::min<__int64>(fs->Size, TXT_DETECT_SIZE));
				*line_brk = get_StreamLineBreak(ms.get(), code_page);
			}

			//読み込み
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
//テキストファイルの末尾を指定行数分読み込む
//  戻り値: コードページ	0 = エラー
//---------------------------------------------------------------------------
int load_text_tail(
	UnicodeString fnam,
	TStringList *f_buf,
	int code_page,				//コードページ		(default = 0 : 取得)
	int limit_ln,				//最大読込行数		(default = 100)
	bool reverse,				//逆順表示			(default = false);
	UnicodeString *line_brk)	//[o] 改行コード	(default = NULL);
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

		//テキストとして読み込む
		std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
		f_buf->LoadFromStream(fs.get(), enc.get());

		//逆順
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
//テキストファイルの末尾の追加内容をチェック
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

		//テキストとして読み込む
		std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
		lst->LoadFromStream(fs.get(), enc.get());
		//キーワードで絞り込み
		if (!kwd.IsEmpty()) {
			int i=0;
			//正規表現
			if (is_regex_slash(kwd)) {
				kwd = exclude_top_end(kwd);
				TRegExOptions opt; opt << roIgnoreCase;
				while (i < lst->Count) if (TRegEx::IsMatch(lst->Strings[i], kwd, opt)) i++; else lst->Delete(i);
			}
			//通常
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
//テキストファイルの先頭行を取得
//---------------------------------------------------------------------------
UnicodeString get_top_line(UnicodeString fnam)
{
	UnicodeString ret_str;
	try {
		std::unique_ptr<TFileStream> fs(new TFileStream(fnam, fmOpenRead | fmShareDenyNone));

		//コードページ取得
		std::unique_ptr<TMemoryStream> ms(new TMemoryStream());
		ms->CopyFrom(fs.get(), std::min<__int64>(fs->Size, TXT_DETECT_SIZE));
		int code_page = get_MemoryCodePage(ms.get());
		if (code_page==0) Abort();

		//先頭行のサイズ取得
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

		//バッファに読み込み
		TBytes Bytes;
		Bytes.Length = size;
		ms->Seek(0, soFromBeginning);
		ms->Read(Bytes, size);

		//エンコード
		std::unique_ptr<TEncoding> enc(TEncoding::GetEncoding(code_page));
		ret_str = enc->GetString(Bytes, 0, Bytes.Length);
	}
	catch (...) {
		ret_str = EmptyStr;
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//XMLの宣言情報を取得する
//---------------------------------------------------------------------------
void get_xml_inf(
	UnicodeString fnam,		//ファイル名 or "<?xml " を含む行内容
	TStringList *lst)		//取得バッファ(名前=値 の形式で格納);
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
//autorun.inf からアイコンファイル名を取得
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
//テキストエディタで開く
//---------------------------------------------------------------------------
bool open_by_TextEditor(UnicodeString fnam, int lno)
{
	try {
		GlobalErrMsg = EmptyStr;
		if (!file_exists(fnam))   SysErrAbort(ERROR_FILE_NOT_FOUND);
		UnicodeString editor = get_actual_path(TextEditor);
		if (!file_exists(editor)) UserAbort(USTR_AppNotFound);

		//起動パラメータの構成
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

		//編集履歴を更新
		if (!ContainsText(fnam, TempPathA)) add_TextEditHistory(fnam);
		return true;
	}
	catch (EAbort &e) {
		GlobalErrMsg = e.Message;
		return false;
	}
}

//---------------------------------------------------------------------------
//外部テキストビュアーで開く
//---------------------------------------------------------------------------
bool open_by_ExtTextViewer(UnicodeString fnam, int lno)
{
	try {
		GlobalErrMsg = EmptyStr;
		if (!file_exists(fnam))   SysErrAbort(ERROR_FILE_NOT_FOUND);
		UnicodeString viewer = get_actual_path(ExtTxViewer);
		if (!file_exists(viewer)) UserAbort(USTR_AppNotFound);

		//起動パラメータの構成
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
//文字列をファイル名と行番号に分離
//---------------------------------------------------------------------------
bool divide_FileName_LineNo(
	UnicodeString &fnam,	//[i]   ファイル名を含む文字列(ctags も可)
							//[o]   パス付きファイル名
	int &lno,				//[i/o] 行番号
	UnicodeString rnam,		//[i]   基準デフォルト名 (default = EmptyStr/ ExePath)
	int pos)				//[i]   取得開始位置	 (default = 0)
{
	GlobalErrMsg = EmptyStr;

	rnam = ExtractFilePath(rnam);
	if (rnam.IsEmpty()) rnam = ExePath;

	do {
		fnam = Trim(fnam);
		if (fnam.IsEmpty()) break;

		//html リンク
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

		//マークダウン
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

		//ローカルファイル
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

		//c インクルード
		if (contains_s(fnam, _T("#include"))) {
			fnam = exclude_quot(Trim(get_tkn_r(fnam, _T("#include"))));
			if (contains_s(fnam, _T('<'))) fnam = get_tkn_m(fnam, '<', '>');
			lno = 1;	break;
		}

		//ctags フォーマット
		TMatch mt = TRegEx::Match(fnam, "^.+\\t.+\\.\\w+\\t(\\d+|/.+/)", opt);
		if (mt.Success) {
			UnicodeString lbuf = get_post_tab(get_tkn(fnam, ";\""));
			UnicodeString nptn = get_post_tab(lbuf);
			fnam = get_pre_tab(lbuf);
			//パターン
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
			//行番号
			else
				lno = nptn.ToIntDef(1);
			break;
		}

		//一般
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

		//行番号を取得
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
		GlobalErrMsg.USET_T("ジャンプ先が取得できません");
	else if (!file_exists(fnam))
		GlobalErrMsg = SysErrorMessage(ERROR_FILE_NOT_FOUND);

	return GlobalErrMsg.IsEmpty();
}

//---------------------------------------------------------------------------
//tags ファイルを取得
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
//タグ名からタグ情報を取得
//	タグ名 \t ファイル名 \t 検索パター ン/行番号;"〜 形式のリストを取得
//---------------------------------------------------------------------------
int get_FileNameByTag(
	UnicodeString tnam,		//[i]   タグ名
	UnicodeString &rnam,	//[i/o] 基準デフォルト名
	TStringList *lst,		//[o]   結果リスト
	UnicodeString onam)		//対象ファイル	(default = EmptyStr)
{
	GlobalErrMsg = EmptyStr;

	try {
		if (tnam.IsEmpty()) UserAbort(USTR_NoParameter);
		tnam.UCAT_T("\t");

		UnicodeString tags = get_tags_file(rnam);
		if (tags.IsEmpty()) throw EAbort(LoadUsrMsg(USTR_NotFound, _T("tags ファイル")));
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
//HtmConv をデフォルトで初期化
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
// 指定ファイルを実行/開く
//---------------------------------------------------------------------------
bool Execute_ex(
	UnicodeString cmd,		//実行ファイル/ フォルダ名 or CLSId or shell コマンド(末尾が\)
	UnicodeString prm,		//パラメータ				(default = EmptyStr)
	UnicodeString wdir,		//作業ディレクトリ			(default = EmptyStr)
	UnicodeString opt,		//オプション H=非表示/ W=終了待ち/ O=コンソール出力を取得/ L=ログに出力
							//			 A=管理者として実行 (W,O 無効)
							//							(default = EmptyStr)
	DWORD *exit_code,		//  exit_code: 終了コード	(default = NULL)
	TStringList *o_lst)		//  o_lst: コンソール出力	(default = NULL)
{
	GlobalErrMsg  = EmptyStr;
	if (cmd.IsEmpty()) return false;

	try {
		if (!wdir.IsEmpty() && is_root_dir(wdir)) wdir = IncludeTrailingPathDelimiter(wdir);

		cmd = exclude_quot(cmd);
		if (ends_PathDlmtr(cmd)) {
			if (USTARTS_TS("::{", cmd) || USTARTS_TI("shell:", cmd)) //CLSID or shell コマンド
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
		//名前なしパイプを作成
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
			//終了待ち
			if (contains_wd_i(opt, _T("W|O|L"))) {
				bool exited = false;

				//コンソール出力の取り込み
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
								//ログに出力
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
							//ログに出力
							if (l_sw) {
								std::unique_ptr<TMemoryStream> mbuf(new TMemoryStream());
								mbuf->Write(buf.get(), len);
								log_buf += get_MemoryStrins(mbuf.get());
								AddLog(log_buf, false, true);
								Application->ProcessMessages();
							}
						}
					}

					//出力内容を設定
					if (ms->Size>0 && o_lst) o_lst->Text = get_MemoryStrins(ms.get());
					exited = true;
				}
				//終了待ち
				else if (contains_s(opt, _T('W'))) {
					while (::WaitForSingleObject(pi.hProcess, 50)==WAIT_TIMEOUT) Sleep(MIN_INTERVAL);
					exited = true;
				}

				//終了状態を取得
				if (exited && exit_code) {
					if (!::GetExitCodeProcess(pi.hProcess, exit_code)) SysErrAbort(GetLastError());
				}
			}
			::CloseHandle(pi.hThread);
			::CloseHandle(pi.hProcess);
			if (hRead)  ::CloseHandle(hRead);
			if (hWrite) ::CloseHandle(hWrite);
		}
		//起動失敗
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
//実行/待機中のタスク数を取得
//---------------------------------------------------------------------------
int get_BusyTaskCount()
{
	int busy_cnt = 0;
	//実行中タスク
	for (int i=0; i<MAX_TASK_THREAD; i++) {
		TTaskThread *tp = TaskThread[i];
		if (tp && !tp->TaskCancel) busy_cnt++;
	}
	//予約
	busy_cnt += TaskReserveList->Count;
	return busy_cnt;
}

//---------------------------------------------------------------------------
//強制実行を含む最大タスク数を取得
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
//空きタスクがあるか?
//---------------------------------------------------------------------------
bool has_EmptyTask(
	bool ex_sw)	//最大タスク数を超えて取得
{
	bool res = false;
	int maxn = ex_sw? std::min((MaxTasks + 2), MAX_TASK_THREAD): MaxTasks;
	for (int i=0; i<maxn && !res ; i++) if (!TaskThread[i]) res = true;
	return res;
}

//---------------------------------------------------------------------------
//タスクスレッドを取得
//空きがななかったり保留中の場合は予約
//---------------------------------------------------------------------------
TTaskThread * CreTaskThread(
	TaskConfig **cp,
	bool reserve,		//予約する					(default = true)
	bool force,			//保留中でも強制取得		(default = false)
	bool ext_sw)		//最大タスク数を超えて取得	(default = false)
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
//タスクの開始または予約
//---------------------------------------------------------------------------
void ActivateTask(TTaskThread *tp, TaskConfig *cp)
{
	UnicodeString msg;
	if (tp) {
		tp->TaskStart();
		if (cp) StartLog(msg.sprintf(_T("%s開始  %s"), cp->CmdStr.c_str(), cp->InfStr.c_str()), tp->Tag);
	}
	else if (cp) {
		TaskReserveList->Add(cp);
		StartLog(msg.sprintf(_T("%s予約  %s"), cp->CmdStr.c_str(), cp->InfStr.c_str()));
	}
}

//---------------------------------------------------------------------------
//予約保留項目の実行
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
	bool ext_sw)	//最大タスク数を超えて開始	(default = false)
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
//タスクに低速実行を要求
//---------------------------------------------------------------------------
void RequestSlowTask()
{
	for (int i=0; i<MAX_TASK_THREAD; i++) {
		TTaskThread *tp = TaskThread[i];
		if (tp) tp->ReqTaskSlow = true;
	}
}

//---------------------------------------------------------------------------
//ログ用に最新のエラーメッセージを取得
//---------------------------------------------------------------------------
UnicodeString get_LogErrMsg(
	UnicodeString msg,	//メッセージ (default = EmptyStr)
	bool add_cr)		//改行を挿入 (default = true)
{
	UnicodeString ret_str;

	if (LogErrMsg) {
		if (!msg.IsEmpty())
			ret_str = msg;
		else {
			int msg_id = GetLastError();
			if (msg_id!=NO_ERROR) ret_str = SysErrorMessage(msg_id);
			remove_top_text(ret_str, _T("%1 は"));
		}

		if (add_cr && !ret_str.IsEmpty() && !USTARTS_TS("\r\n", ret_str))
			ret_str.Insert("\r\n    ", 1);
	}

	return ret_str;
}
//---------------------------------------------------------------------------
void set_LogErrMsg(
	UnicodeString &msg,	//メッセージ
	UnicodeString s)	//追加メッセージ (default = EmptyStr)
{
	if (USTARTS_TI("Abort", s)) s = EmptyStr;
	if (!msg.IsEmpty()) msg[1] = contains_s(s, _T("中断しました"))? 'C' : 'E';
	msg += get_LogErrMsg(s);
}

//---------------------------------------------------------------------------
//グリッドやリストに文字列を描画する際の上部マージンを取得
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
//時計用文字列を取得
//---------------------------------------------------------------------------
UnicodeString GetClockStr()
{
	UnicodeString ret_str;
	if (!SttClockFmt.IsEmpty()) {
		ret_str = SttClockFmt;
		//バッテリー
		UnicodeString lbuf, s, tmp;
		ret_str = ReplaceStr(ret_str, "$BP", tmp.sprintf(_T("\"%s%\""), get_BatteryPercentStr().c_str()));
		ret_str = ReplaceStr(ret_str, "$BT", tmp.sprintf(_T("\"%s\""),  get_BatteryTimeStr().c_str()));
		//電源状態
		if (contains_s(ret_str, _T("$PS("))) {
			lbuf = split_tkn(ret_str, _T("$PS(")).UCAT_T("\"");
			s    = split_tkn(ret_str, ')');
			SYSTEM_POWER_STATUS ps;
			::GetSystemPowerStatus(&ps);
			lbuf += ((ps.ACLineStatus==1)? get_tkn(s, ':') : get_tkn_r(s, ':')).UCAT_T("\"");
			ret_str.Insert(lbuf, 1);
		}
		//接続状態
		if (contains_s(ret_str, _T("$NS("))) {
			lbuf = split_tkn(ret_str, _T("$NS(")).UCAT_T("\"");
			s    = split_tkn(ret_str, ')');
			lbuf += (InternetConnected()? get_tkn(s, ':') : get_tkn_r(s, ':')).UCAT_T("\"");
			ret_str.Insert(lbuf, 1);
		}

		//日付カウントダウン
		while (contains_s(ret_str, _T("$CD("))) {
			lbuf = split_tkn(ret_str, _T("$CD(")).UCAT_T("\"");
			s    = split_tkn(ret_str, ')');
			if (s.Length()<=5) s = FormatDateTime("yyyy/", Now()) + s;
			TDateTime dt;
			if (str_to_DateTime(split_tkn(s, ';'), &dt)) {
				if (s.IsEmpty()) s = format_Date(dt);
				int n = dt - Today();
				if (n==0)
					s.UCAT_T("当日");
				else if (n>0)
					s.cat_sprintf(_T("まで%s日"), get_size_str_B(n, 0).c_str());
				else
					s.cat_sprintf(_T("から%s日"), get_size_str_B(-n, 0).c_str());
			}
			lbuf += s.UCAT_T("\"");
			ret_str.Insert(lbuf, 1);
		}

		//日時 (引用符内の文字列は変換されず、引用符を外してそのまま表示)
		ret_str = format_DateTimeEx(ret_str, Now());	//$EN で英語に
	}
	return ret_str;
}

//---------------------------------------------------------------------------
// コマンドファイルの改名をオプション設定に反映
//---------------------------------------------------------------------------
void RenameOptCmdFile()
{
	while (RenCmdFileList->Count > 0) {
		UnicodeString lbuf	  = RenCmdFileList->Strings[0];
		UnicodeString org_nam = split_pre_tab(lbuf);
		UnicodeString new_nam = lbuf;
		UnicodeString new_rel = to_relative_name(lbuf);
		if (!org_nam.IsEmpty() && !new_rel.IsEmpty()) {
			//イベント
			for (int i=0; i<MAX_EVENT_CMD; i++) {
				if (SameText(org_nam, rel_to_absdir(get_tkn_r(*(EventCmdList[i].sp), '@'))))
					*(EventCmdList[i].sp) = "@" + new_rel;
			}
			//リスト
			int idx = CmdFileList->IndexOf(org_nam);
			if (idx!=-1) CmdFileList->Strings[idx] = new_nam;

			//追加メニュー
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
			//キー設定
			for (int i=0; i<KeyFuncList->Count; i++) {
				UnicodeString vbuf = KeyFuncList->ValueFromIndex[i];
				if (!USTARTS_TI("ExeCommands_", vbuf)) continue;
				UnicodeString vstr = split_tkn(vbuf, '_').UCAT_T("_");
				UnicodeString dsc  = split_dsc(vbuf);
				if (!dsc.IsEmpty()) vstr.cat_sprintf(_T(":%s:"), dsc.c_str());
				if (!remove_top_AT(vbuf)) continue;
				if (is_same_file(org_nam, vbuf)) KeyFuncList->ValueFromIndex[i] = vstr.cat_sprintf(_T("@%s"), new_rel.c_str());
			}
			//関連付け(OpenByApp)
			for (int i=0; i<AssociateList->Count; i++) {
				UnicodeString vbuf = exclude_quot(AssociateList->ValueFromIndex[i]);
				if (!remove_top_AT(vbuf)) continue;
				if (is_same_file(org_nam, vbuf)) AssociateList->ValueFromIndex[i] = "@" + new_rel;
			}
			//関連付け(OpenStandard)
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
//コマンドファイルの参照情報を取得
//---------------------------------------------------------------------------
UnicodeString get_ref_CmdFile(UnicodeString fnam, TStringList *lst)
{
	UnicodeString ref_str = "＜参照元＞";
	//キー設定
	int K_cnt = 0;
	for (int i=0; i<KeyFuncList->Count; i++) {
		UnicodeString vbuf = KeyFuncList->ValueFromIndex[i];
		if (!remove_top_text(vbuf, _T("ExeCommands_")))	continue;
		UnicodeString dsc = split_dsc(vbuf);
		if (!remove_top_AT(vbuf)) 						continue;
		if (!is_same_file(fnam, exclude_quot(vbuf)))	continue;
		K_cnt++;
		if (lst) {
			add_PropLine(ref_str, "キー", lst);
			UnicodeString tmp = make_PropLine(_T("設定"), KeyFuncList->Names[i]);
			if (!dsc.IsEmpty()) tmp.cat_sprintf(_T(" : %s"), dsc.c_str());
			lst->Add(tmp);
		}
	}
	//追加メニュー
	int M_cnt = 0;
	for (int i=0; i<ExtMenuList->Count; i++) {
		TStringDynArray itm_buf = get_csv_array(ExtMenuList->Strings[i], EXTMENU_CSVITMCNT, true);
		if (is_separator(itm_buf[0]) || !USAME_TI(itm_buf[1], "ExeCommands")) continue;
		UnicodeString prm = itm_buf[2];
		if (!remove_top_AT(prm)) 					continue;
		if (!is_same_file(fnam, exclude_quot(prm))) continue;
		M_cnt++;
		if (lst) {
			lst->Add(make_PropLine(ref_str, "追加メニュー") + (equal_0(itm_buf[4])? " (非表示)" : ""));
			add_PropLine(_T("タイトル"), itm_buf[0], lst);
			UnicodeString alias = itm_buf[3];
			if (!alias.IsEmpty()) {
				add_PropLine(_T("エイリアス"), alias, lst);
				add_PropLine_if(_T("キー"), get_Alias_KeyStr(alias), lst);
			}
		}
	}
	//ツールボタン
	int B_cnt = 0;
	for (int ti=0; ti<3; ti++) {
		TStringList *tblst = (ti==1)? ToolBtnListV : (ti==2)? ToolBtnListI : ToolBtnList;
		for (int i=0; i<tblst->Count; i++) {
			UnicodeString vbuf = get_csv_item(tblst->Strings[i], 1);
			if (!remove_top_AT(vbuf))		continue;
			if (!is_same_file(fnam, vbuf))	continue;
			B_cnt++;
			if (lst) lst->Add(make_PropLine(ref_str, "ツールボタン(") + ((ti==1)? "TV)" : (ti==2)? "IV)" : "FL)"));
		}
	}

	//関連付け(OpenByApp)
	int A_cnt = 0;
	for (int i=0; i<AssociateList->Count; i++) {
		UnicodeString vbuf = AssociateList->ValueFromIndex[i];
		vbuf = exclude_quot(vbuf);
		if (!remove_top_AT(vbuf))		continue;
		if (!is_same_file(fnam, vbuf))	continue;
		A_cnt++;
		if (lst) add_PropLine(ref_str, "関連付け(OpenByApp): " + AssociateList->Names[i], lst);
	}
	//関連付け(OpenStandard)
	int S_cnt = 0;
	for (int i=0; i<OpenStdCmdList->Count; i++) {
		UnicodeString vbuf = OpenStdCmdList->ValueFromIndex[i];
		if (!remove_top_text(vbuf, _T("ExeCommands_@"))) continue;
		if (!is_same_file(fnam, vbuf))	continue;
		S_cnt++;
		if (lst) add_PropLine(ref_str, "関連付け(OpenStandard): " + OpenStdCmdList->Names[i], lst);
	}
	//イベント
	int E_cnt = 0;
	for (int i=0; i<MAX_EVENT_CMD; i++) {
		if (!is_same_file(fnam, get_tkn_r(*(EventCmdList[i].sp), '@'))) continue;
		E_cnt++;
		if (lst) {
			UnicodeString tmp = EventCmdList[i].dsc;
			remove_top_s(tmp, '|');
			add_PropLine(ref_str, "イベント: " + tmp, lst);
		}
	}
	for (int i=0; i<MAX_TIMER_EVENT; i++) {
		UnicodeString vbuf = OnTimerEvent[i];
		if (!remove_top_AT(vbuf))		continue;
		if (!is_same_file(fnam, vbuf))	continue;
		E_cnt++;
		if (lst) {
			add_PropLine(ref_str, "タイマーイベント", lst);
			add_PropLine(_T("実行条件"), Timer_Condition[i], lst);
		}
	}

	UnicodeString ret_str;
	if (K_cnt>0) ret_str.UCAT_T("K");
	if (M_cnt>0) ret_str.UCAT_T("M");
	if (B_cnt>0) ret_str.UCAT_T("B");
	if (A_cnt>0) ret_str.UCAT_T("A");
	if (S_cnt>0) ret_str.UCAT_T("S");
	if (E_cnt>0) ret_str.UCAT_T("E");

	//実行回数
	if (lst) {
		int idx = CmdFileList->IndexOf(fnam);
		if (idx!=-1) {
			cmdf_rec *cp = (cmdf_rec*)CmdFileList->Objects[idx];
			add_PropLine(_T("実行回数"), cp->exe_count, lst);
		}
	}

	return ret_str;
}

//---------------------------------------------------------------------------
//エイリアスを参照しているキーを取得(カンマ区切り文字列)
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
//説明や引用符を外すしてパラメータを抽出
//---------------------------------------------------------------------------
UnicodeString extract_ExeParam(UnicodeString prm, UnicodeString *dsc)
{
	//説明を取得/削除
	UnicodeString dsc_str = split_dsc(prm);
	if (dsc) *dsc = dsc_str;

	//引用符を外す
	return exclude_quot(prm);
}

//---------------------------------------------------------------------------
//マスク設定用文字列を作成
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
//ログのヘッダを作成
//---------------------------------------------------------------------------
UnicodeString make_LogHdr(
	UnicodeString cmd,		//コマンド識別文字列
	UnicodeString fnam,		//名前
	bool is_dir,			//fnam が ディレクトリ名 (default = false)
	int wd)					//表示幅 (default = 0)
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
//ログに改名情報を付加
//---------------------------------------------------------------------------
void set_RenameLog(UnicodeString &msg, UnicodeString fnam)
{
	if (msg.IsEmpty()) return;

	msg[1] = 'R';
	msg.cat_sprintf(_T(" ---> %s"), ExtractFileName(fnam).c_str());
}

//---------------------------------------------------------------------------
//ログリストボックスの更新
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
//ログに開始情報を表示
//---------------------------------------------------------------------------
void StartLog(
	UnicodeString msg,	//メーセージ (\t は " ---> " に置換)
	int task_no)		//タスク番号 (default = -1)
{
	if (msg.IsEmpty())
		LogBufList->Add(EmptyStr);
	else {
		//前が空行でなければ、空行を挿入
		if (LogBufList->Count>0 && !LogBufList->Strings[LogBufList->Count - 1].IsEmpty()) LogBufList->Add(EmptyStr);
		//開始表示
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
//ログに終了情報を表示
//---------------------------------------------------------------------------
void EndLog(UnicodeString msg, UnicodeString inf)
{
	if (contained_wd_i(_T("圧縮|解凍"), msg)) NotifyPrimNyan(UAPP_T(msg, "が終了しました"));

	AddLog(msg.cat_sprintf(_T("終了%s"), inf.c_str()), true);
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
//実行エラーを表示
//---------------------------------------------------------------------------
void ExeErrLog(UnicodeString fnam, UnicodeString msg)
{
	UnicodeString lbuf = make_LogHdr(_T("EXEC"), fnam); lbuf[1] = 'E';
	AddLog(lbuf + get_LogErrMsg(msg));
}

//---------------------------------------------------------------------------
//ログを出力
//---------------------------------------------------------------------------
void AddLog(
	UnicodeString msg,	//メッセージ
	bool with_t,		//タイム表示   (default = false)
	bool raw)			//そのまま出力 (default = false)
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
//LogBufStr の内容を画面に出力
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
//描画の一時停止
// 多重設定に対応
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
//描画停止の解除
// すべて解除されたときに描画再開
//---------------------------------------------------------------------------
void set_RedrawOn(TWinControl *cp)
{
	bool flag = false;
	int idx = RedrawList->IndexOfObject((TObject*)cp);
	if (idx!=-1) {
		//1段階解除
		int n = RedrawList->Strings[idx].ToIntDef(0);
		if (n>0) n--;
		if (n==0) {
			//すべて解除された
			RedrawList->Delete(idx);
			flag = true;
		}
		else RedrawList->Strings[idx] = IntToStr(n);
	}
	else flag = true;

	//描画再開
	if (flag) {
		cp->Perform(WM_SETREDRAW, 1, 0);
		::InvalidateRect(cp->Handle, NULL, TRUE);
	}
}

//---------------------------------------------------------------------------
//フォームに対する特殊なキー処理
//---------------------------------------------------------------------------
bool __fastcall SpecialKeyProc(
	TForm *frm,
	WORD &Key,
	TShiftState Shift,
	const _TCHAR *topic)	//ヘルプトピック (default = NULL)
{
	UnicodeString KeyStr   = get_KeyStr(Key);		if (KeyStr.IsEmpty()) return false;
	UnicodeString ShiftStr = get_ShiftStr(Shift);
	KeyStr = ShiftStr + KeyStr;

	//Shift+F10 によるポップアップメニュー表示
	//※コンボボックスでデフォルトのメニューが出てしまう現象に対応
	//  OnMessage で拾えないようなのでここで処理
	if (USAME_TI(KeyStr, "Shift+F10")) {
		if (UserModule->ShowPopupMenu()) Key = 0;
		return true;
	}

	//ヘルプ
	if (equal_F1(KeyStr) && topic) {
		HtmlHelpTopic(topic);
		Key = 0;
		return true;
	}

	//キーによるダイアログの移動/サイズ変更
	if (ShiftStr.IsEmpty()) return false;

	bool handled = true;
	//移動
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
	//サイズ変更
	else if (SameText(ShiftStr, DlgSizeShift) && (frm->BorderStyle==bsSizeable || frm->BorderStyle==bsSizeToolWin)) {
		switch (Key) {
		case VK_UP:    frm->Height = frm->Height - DlgSizePrm;	break;
		case VK_DOWN:  frm->Height = frm->Height + DlgSizePrm;	break;
		case VK_LEFT:  frm->Width  = frm->Width  - DlgSizePrm;	break;
		case VK_RIGHT: frm->Width  = frm->Width  + DlgSizePrm;	break;
		default:	   handled = false;
		}
	}
	//閉じる
	else if (USAME_TI(KeyStr, "Alt+F4"))
		frm->Close();
	else
		handled = false;

	if (handled) {
		//コンボボックスがドロップダウンしていたら閉じる
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
	int hlp_cnt)	//ヘルプコンテキスト
{
	UnicodeString topic;
	if (hlp_cnt>0) topic.sprintf(_T("hid00%03u.htm"), hlp_cnt);

	return SpecialKeyProc(frm, Key, Shift, topic.c_str());
}

//---------------------------------------------------------------------------
//特殊な編集処理
//  カーソル位置の大文字化/小文字化
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
				if		(USAME_TI(KeyStr, "Ctrl+U")) s[p] = toupper(s[p]);	//大文字化
				else if (USAME_TI(KeyStr, "Ctrl+L")) s[p] = tolower(s[p]);	//小文字化
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
//拡張入力ボックス
//---------------------------------------------------------------------------
UnicodeString inputbox_ex(
	const _TCHAR *tit,		//タイトル
	const _TCHAR *prm,		//プロンプト
	UnicodeString def,		//デフォルト	(default = EmptyStr)
	bool num_only)			//数字のみ入力	(default = false)
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
	UnicodeString def_s,	//デフォルト文字列	(default = EmptyStr)
	int def_n)				//デフォルト数値	(default = 0)
{
	return inputbox_ex(tit, prm, def_s, true).ToIntDef(def_n);
}
//---------------------------------------------------------------------------
bool input_query_ex(
	const _TCHAR *tit,		//タイトル
	const _TCHAR *prm,		//プロンプト
	UnicodeString *value,
	int width,				//表示幅指定	(default = 0 : なし)
	bool num_only,			//数字のみ入力	(default = false)
	UnicodeString hint)		//ヒント		(default = EmptyStr)
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
	int tid,				//メッセージID
	const _TCHAR *prm,
	UnicodeString *value,
	int width,
	bool num_only,
	UnicodeString hint)
{
	return input_query_ex(LoadUsrMsg(tid).c_str(), prm, value, width, num_only, hint);
}

//---------------------------------------------------------------------------
//ディレクトリ入力ボックス
//---------------------------------------------------------------------------
UnicodeString inputbox_dir(const _TCHAR *tit, const _TCHAR *cmd)
{
	InpDirDlg->Caption	  = tit;
	InpDirDlg->CommandStr = cmd;
	return ((InpDirDlg->ShowModal()==mrOk)? InpDirDlg->InpDirComboBox->Text : EmptyStr);
}

//---------------------------------------------------------------------------
//0〜9キーか?
//---------------------------------------------------------------------------
bool is_Num0to9(UnicodeString keystr)
{
	return (keystr.Length()==1 && CompareStr(keystr, "0")>=0 && CompareStr(keystr, "9")<=0);
}

//---------------------------------------------------------------------------
//頭文字サーチキーか?
//  keystr に正規表現パターンを返す
//---------------------------------------------------------------------------
bool is_IniSeaKey(UnicodeString &keystr)
{
	if (keystr.IsEmpty()) return false;
	UnicodeString k; k.sprintf(_T("F:%s"), keystr.c_str());
	if (!IniSeaShift.IsEmpty() && !remove_top_text(k, IniSeaShift)) return false;
	if (k.Length()!=1) return false;

	if (_istalpha(k[1]) || USAME_TS(k, "＼") || ((IniSeaByNum || IniSeaBySign) && _istdigit(k[1]))) {
		k = REPLACE_TS(k, "＼", "_");
		if (IniSeaBySign) {
			//Shift+数字キーの記号もサーチ
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
//インクリメンタルサーチのキーワードを更新
//  画面モードによって制限文字を切り替え、JP/USキーボード対応
//---------------------------------------------------------------------------
bool update_IncSeaWord(
	UnicodeString &kwd,		//[i/o] キーワード
	UnicodeString keystr,	//[i]   入力キー
	bool fl_sw)				//[i]   true = ファイル名に使えない文字を考慮
{
	bool is_char = false;
	bool is_cap  = false;

	remove_text(keystr, _T("10Key_"));
	keystr = REPLACE_TS(keystr, "＝", "=");

	if (remove_top_text(keystr, KeyStr_Shift)) {
		if (keystr.Length()==1) {
			//Shift+英字
			if (_istalpha(keystr[1])) {
				is_char = true; is_cap = true;
			}
			//Shift+数字
			else if (_istdigit(keystr[1])) {
				std::unique_ptr<TStringList> r_lst(new TStringList());
				//ファイル名
				if (fl_sw) {
					r_lst->Text = is_JpKeybd()?
						"0\t\n1\t!\n2\t”\n3\t#\n4\t$\n5\t%\n6\t&\n7\t'\n8\t(\n9\t)\n" : //JP
						"0\t)\n1\t!\n2\t@\n3\t#\n4\t$\n5\t%\n6\t^\n7\t&\n8\t\n9\t(\n";	 //US
				}
				//テキスト
				else {
					r_lst->Text = is_JpKeybd()?
						"0\t\n1\t!\n2\t\"\n3\t#\n4\t$\n5\t%\n6\t&\n7\t'\n8\t(\n9\t)\n" : //JP
						"0\t)\n1\t!\n2\t@\n3\t#\n4\t$\n5\t%\n6\t^\n7\t&\n8\t*\n9\t(\n";	 //US
				}
				keystr	= replace_str_by_list(keystr, r_lst.get());
				is_char = !keystr.IsEmpty();
			}
			//Shift+記号
			else {
				std::unique_ptr<TStringList> r_lst(new TStringList());
				//ファイル名
				if (fl_sw) {
					r_lst->Text = is_JpKeybd()?
						"-\t=\n^\t~\n\\\t\n@\t`\n[\t{\n]\t}\n;\t+\n＼\t_\n,\t\n.\t\n/\t\n" :	//JP
						"`\t~\n-\t_\n=\t+\n[\t{\n]\t}\n;\t\n'\t\n\\\t\n,\t\n.\t\n/\t\n";		//US
				}
				//テキスト
				else {
					r_lst->Text = is_JpKeybd()?
						"-\t=\n^\t~\n\\\t|\n@\t`\n[\t{\n]\t}\n;\t+\n:\t*\n＼\t_\n,\t<\n.\t>\n/\t?\n" :	//JP
						"`\t~\n-\t_\n=\t+\n[\t{\n]\t}\n;\t:\n'\t\"\n\\\t|\n,\t<\n.\t>\n/\t?\n";	//US
				}
				keystr	= replace_str_by_list(keystr, r_lst.get());
				is_char = !keystr.IsEmpty();
			}
		}
	}
	else if (keystr.Length()==1) {
		//英数字
		if (_istalnum(keystr[1]))
			is_char = true;
		//記号
		else {
			//ファイル名
			if (fl_sw)
				is_char = ContainsStr((is_JpKeybd()? "-^@[];,." : "-=[];,.`'"), keystr);
			//テキスト
			else
				is_char = ContainsStr((is_JpKeybd()? "-^\\@[];:,./" : "-=[];,.`'\\/"), keystr);
		}
	}
	else if (USAME_TI(keystr, "SPACE")) {
		keystr.USET_T(" ");
		is_char = true;
	}

	//一文字後退
	if (contained_wd_i(_T("BKSP|Ctrl+H"), keystr)) {
		if (!kwd.IsEmpty()) delete_end(kwd);
		return true;
	}
	//一文字追加
	if (is_char) {
		keystr = is_cap? keystr.UpperCase() : keystr.LowerCase();
		kwd += keystr;
		return true;
	}

	return false;
}

//---------------------------------------------------------------------------
//カーソルキーをコマンドに変換
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
//単独の通常コマンドか？
//---------------------------------------------------------------------------
bool is_OneNrmCmd(
	UnicodeString cmds,
	bool no_prm)			//パラメータ無し (default = false)
{
	if (starts_AT(cmds))	 							return false;
	if (pos_r_q_colon(cmds)!=0)							return false;
	if (CommandList->IndexOfName(get_CmdStr(cmds))==-1)	return false;
	if (no_prm && !get_PrmStr(cmds).IsEmpty())	 		return false;
														return true;
}

//---------------------------------------------------------------------------
//フィルタ欄からの一覧のカーソル移動
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
//リストボックスでのコマンド処理
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

	//移動
	int cmd_id = idx_of_word_i(
		_T("CursorDown|CursorUp|CursorTop|TextTop|CursorEnd|TextEnd|")				//0〜5
		_T("PageDown|PageUp|ScrollDown|ScrollUp|ScrollCursorDown|ScrollCursorUp"),	//6〜11
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
	//選択
	else if (lp->MultiSelect && USAME_TI(cmd, "Select")) {
		if (idx!=-1) {
			lp->Selected[idx] = !lp->Selected[idx];
			ListBoxCursorDown(lp);
		}
	}
	//すべて選択
	else if (lp->MultiSelect && (USTARTS_TI("SelAll",cmd) || USAME_TI(cmd, "SelectAll"))) {
		ListBoxSelectAll(lp, lp->SelCount==0);
	}
	//コピー
	else if (USAME_TI(cmd, "ClipCopy")) {
		//選択行コピー
		if (lp->MultiSelect && lp->SelCount>0) {
			cursor_HourGlass();
			std::unique_ptr<TStringList> cb_buf(new TStringList());
			for (int i=0; i<lp->Count; i++) if (lp->Selected[i]) cb_buf->Add(lp->Items->Strings[i]);
			lp->Perform(WM_SETREDRAW, 0, 0);
			lp->ClearSelection();
			lp->Perform(WM_SETREDRAW, 1, 0);
			::InvalidateRect(lp->Handle, NULL, TRUE);
			if (USAME_TI(prm, "AD")) cb_buf->Text.Insert(Clipboard()->AsText, 1);	//追加
			cursor_Default();
			copy_to_Clipboard(cb_buf->Text);
		}
		//1行コピー
		else if (lp->Tag & LBTAG_OPT_LCPY) {
			if (idx!=-1) copy_to_Clipboard(lp->Items->Strings[idx]);
		}
	}
	//URLを開く
	else if (USAME_TI(cmd, "OpenURL")) {
		if (idx!=-1) Execute_ex(extract_URL(lp->Items->Strings[idx]));
	}
	//ズーム
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
	//フォントサイズ変更
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
//クリップボードにコピー
//コピー情報のヒント表示を行う
//---------------------------------------------------------------------------
void copy_to_Clipboard(UnicodeString s)
{
	if (s.IsEmpty()) return;

	Clipboard()->AsText = s;
	::SendMessage(MainHandle, WM_NYANFI_CLPCOPIED, get_line_count(s) | 0x80000000, str_len_unicode(s));
	//WParam | 0x80000000 でテキストであることを通知
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
//最初に起動された NyanFi のハンドルを取得
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
// ウィイドウは NyanFi のメイン画面か?
//---------------------------------------------------------------------------
bool IsNyanfiWnd(HWND hWnd)
{
	return USAME_TS(get_WndClassName(hWnd), "TNyanFiForm");
}
//---------------------------------------------------------------------------
// ウィイドウは二重起動された NyanFi のメイン画面か?
//---------------------------------------------------------------------------
bool IsNyanfi2Wnd(HWND hWnd)
{
	return (USAME_TS(get_WndClassName(hWnd), "TNyanFiForm")
			&& is_match_regex(get_WndText(hWnd), _T("NyanFi-\\d+$")));
}

//---------------------------------------------------------------------------
//最初に起動された NyanFi へ文字列を送出
//---------------------------------------------------------------------------
bool NotifyPrimNyan(UnicodeString msg)
{
	if (IsPrimary || !ShowDuplNotify) return false;

	HWND hPrimeWnd = get_PrimNyanWnd();
	if (!hPrimeWnd) return false;

	UnicodeString lbuf;
	lbuf.sprintf(_T("NyanFi-%u\r\n　%s　"), NyanFiIdNo, msg.c_str());

	COPYDATASTRUCT cd;
	cd.dwData = CPYDTID_DPL_MSG;
	cd.cbData = sizeof(_TCHAR) * (lbuf.Length() + 1);
	cd.lpData = lbuf.c_str();
	::SendMessage(hPrimeWnd, WM_COPYDATA, 0, (LPARAM)&cd);
	return true;
}

//---------------------------------------------------------------------------
//モーダル表示のウィンドウを取得
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

	//表示中のモーダルフォームを探す
	for (int i=0; i<Screen->FormCount; i++) {
		TForm *fp = Screen->Forms[i];
		if (!fp->Visible || fp==Application->MainForm || !fp->Floating || !fp->Enabled || fp->FormStyle==fsStayOnTop) continue;
		if (fp->ClassNameIs("TExTxtViewer")) continue;
		top_wnd = fp->Handle;
		break;
	}

	//なければ表示中のメッセージボックスを探す
	if (!top_wnd && !::IsWindowEnabled(MainHandle)) ::EnumWindows(EnumModalWndProc , (LPARAM)&top_wnd);

	return top_wnd;
}

//---------------------------------------------------------------------------
//他の NyanFi を終了
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

	for (int i=0; i<20; i++) {	//タイムアウト10秒
		Application->ProcessMessages();
		cnt = 0;
		::EnumWindows(CountNyanWndProc, (LPARAM)&cnt);
		if (cnt==0) break;
		Sleep(500);
	}

	return (cnt==0);
}

//---------------------------------------------------------------------------
//NyanFiの一覧を取得
// 連番(1〜),ハンドル,タイトル
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
//新規起動時のタイトルを取得
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
//前後のNyanFiのハンドルを取得
//---------------------------------------------------------------------------
HWND get_NyanFiWnd(
	int rel_idx)	//1=次/ -1=前
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
//NyanFiをアクティブに
// 戻り値: false=複数のインスタンスがない
//---------------------------------------------------------------------------
bool activate_NyanFi(
	int idx,	//rel? 相対(1=次/ -1=前) : 絶対
	bool rel)	//相対指定	(default = true);
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
//複数のNyanFiがある
//---------------------------------------------------------------------------
bool IsDuplicated()
{
	std::unique_ptr<TStringList> lst(new TStringList());
	return (get_NyanFiList(lst.get())>1);
}

//---------------------------------------------------------------------------
//別ウィンドウのテキストビュアーのリストを取得
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
//別ウィンドウのテキストビュアーがあればフォーカスを移す
//---------------------------------------------------------------------------
bool focus_ExViewer()
{
	std::unique_ptr<TStringList> lst(new TStringList());
	bool ok = (get_ExViewerList(lst.get()) > 0);
	if (ok) ((TForm *)lst->Objects[0])->SetFocus();
	return ok;
}
//---------------------------------------------------------------------------
//別ウィンドウのすべてのテキストビュアーを閉じる
//---------------------------------------------------------------------------
void close_all_ExViewer(
	TForm *frm)		//除外するフォーム
{
	for (int i=0; i<Screen->FormCount; i++) {
		TForm *fp = Screen->Forms[i];
		if (fp->ClassNameIs("TExTxtViewer") && fp!=frm) fp->Close();
	}
}

//---------------------------------------------------------------------------
//ヘルプウィンドウを取得
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
//メニューを取得
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
//ヘルプ処理
//---------------------------------------------------------------------------
void HtmlHelpTopic(
	const _TCHAR *topic)	// topic: ファイル名[#ID]
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
	UnicodeString fnam,		//ヘルプファイル名
	UnicodeString kwd)		//キーワード
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
	bool only_fg)	//フォアグランドの場合にのみ閉じる (default = false)
{
	if (lpfHtmlHelp) {
		HWND hWnd = get_HelpWnd();
		if (only_fg && ::GetForegroundWindow()!=hWnd) hWnd = NULL;
		if (hWnd) lpfHtmlHelp(NULL, NULL, HH_CLOSE_ALL, 0);
	}
}

//---------------------------------------------------------------------------
//2地点間の距離を取得 (Hubenyの公式を利用)
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
//緯度経度を指定して Google Maps を開く
//---------------------------------------------------------------------------
bool OpenGoogleMaps(
	double lat,		//lat: 緯度
	double lng,		//lng: 経度
	UnicodeString fnam)
{
	try {
		GlobalErrMsg  = EmptyStr;

		UnicodeString lbuf;
		//テンプレート
		UnicodeString tnam = ExePath + WEBMAP_TPLT;
		if (file_exists(tnam)) {
			std::unique_ptr<TStringList> fbuf(new TStringList());
			if (load_text_ex(tnam, fbuf.get())==0) UserAbort(USTR_FaildProc);
			lbuf = fbuf->Text;
		}
		//デフォルト
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

			lbuf = ReplaceStr(lbuf, "$Title$",	   !fnam.IsEmpty()? "$FileName$の地図" : "指定地点の地図");
			lbuf = ReplaceStr(lbuf, "$FileRef$",   !fnam.IsEmpty()? "<p><a href=\"file:///$PathName$\">$PathName$</a></p>\r\n" : "");
		}

		lbuf = ReplaceStr(lbuf, "$Latitude$",  UnicodeString().sprintf(_T("%.8f") ,lat));
		lbuf = ReplaceStr(lbuf, "$Longitude$", UnicodeString().sprintf(_T("%.8f") ,lng));
		lbuf = ReplaceStr(lbuf, "$FileName$",  ExtractFileName(fnam));
		lbuf = ReplaceStr(lbuf, "$PathName$",  fnam);

		std::unique_ptr<TStringList> lst(new TStringList());
		lst->Text = lbuf;

		UnicodeString hnam;  hnam.sprintf(_T("%s" WEBMAP_FILE), TempPathA.c_str());
		if (!saveto_TextUTF8(hnam, lst.get())) throw EAbort(LoadUsrMsg(USTR_FaildSave, _T("地図表示用一時ファイル")));

		Execute_ex("file:///" + hnam);
		return true;
	}
	catch (EAbort &e) {
		GlobalErrMsg = e.Message;
		return false;
	}
}

//---------------------------------------------------------------------------
//暗号処理
//---------------------------------------------------------------------------
#define MAX_XORBIT_SIZE 128

unsigned char CipherXorPtn[MAX_XORBIT_SIZE];

//---------------------------------------------------------------------------
//暗号化のためのビットパターンを作成
//  Windowsユーザ名 + システムドライブのシリアルナンバーを取得
//  文字ごとにビット反転させ、128個になるまで繰り返しつなぐ
//  先頭から順番に、それ以降の値の合計の下位バイトを代入していく
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
//暗号化
//  1. 文字列を逆順にひっくり返す
//  2. 文字ごとにビットパターンとXOR
//  3. 2桁16進数の文字列にして結合
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
//復号化
//  1.文字列の頭から2文字ずつ取って16進数として数値化
//  2.ビットパターンとXOR
//  3.文字に直して逆順に結合
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
