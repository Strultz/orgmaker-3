#define _WIN32_WINNT	0x0501
#define WINVER			0x0501
#define _WIN32_WINDOWS	0x0501
#define BUF_SIZE 256
//#define HENKOU_NO_SHIRUSHI	"[change ants]"

#define MAIN_WINDOW "WINDOW"
#define COMMON_WINDOW "COMMONDIALOG"
#define TRACK_WINDOW "TRACK"
#define PLAY_WINDOW "PLAYWINDOW"
#define COPY_WINDOW "COPYWINDOW"
#define INIT_DATA	"INIT_PAN_VOL"
#define MIDI_EXPORT	"STANDARD_MIDI_EXPORT"
#define TOOLBAR_POS "TOOLBAR_POS"

#define CDCD_INIT 0
#define CDCD_EXIT 1
#define CDCD_LOAD 2

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include <windows.h>
#include <winuser.h>
#include <CommCtrl.h>
#include <wininet.h>
#include <string>
#include <thread>

#include "Setting.h"
#include "DefOrg.h"
#include "resource.h"
#include "Gdi.h"
#include "OrgData.h"
#include "Scroll.h"
#include "Mouse.h"
#include "Click.h"
#include <stdio.h>
#include <string.h>
#include "Filer.h"
#include "rxoFunction.h"

#include "Sound.h"
#include "Timer.h"

#include "Toolbar.h"

static char tVerName[64];
static bool canUpdateCheck = true;

void CheckUpdate(bool act) {
	if (!canUpdateCheck) return;
	canUpdateCheck = false;

	HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
	BOOL bResult = FALSE;
	DWORD dwBytesRead;
	char buffer[1024];

	hSession = InternetOpen("OrgMaker/3.x", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (hSession == NULL) {
		canUpdateCheck = true;
		return;
	}

	hConnect = InternetConnect(hSession, "api.github.com", INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hConnect == NULL) {
		InternetCloseHandle(hSession);
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 1, 0); //MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
		canUpdateCheck = true;
		return;
	}

	hRequest = HttpOpenRequest(hConnect, "GET", "/repos/Strultz/orgmaker-3/releases/latest", NULL, NULL, NULL, 0, 0);
	if (hRequest == NULL) {
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hSession);
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 1, 0); //MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
		canUpdateCheck = true;
		return;
	}

	bResult = HttpSendRequest(hRequest, NULL, 0, NULL, 0);
	if (!bResult) {
		InternetCloseHandle(hRequest);
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hSession);
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 1, 0); //MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
		canUpdateCheck = true;
		return;
	}

	std::string content;
	while ((bResult = InternetReadFile(hRequest, buffer, sizeof(buffer), &dwBytesRead)) && dwBytesRead > 0) {
		content.append(buffer, dwBytesRead);
	}

	InternetCloseHandle(hRequest);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hSession);

	// I'm not adding some large ass json library so this is what you get
	size_t f = content.find("\"tag_name\":");
	if (f == std::string::npos) {
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 2, 0);
		canUpdateCheck = true;
		return;
	}

	std::string s1 = content.substr(f + 12);
	if (s1.substr(0, 1) == " ") s1 = s1.substr(1);

	size_t nq = s1.find_first_of('\"');
	if (f == std::string::npos) {
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 2, 0);
		canUpdateCheck = true;
		return;
	}

	s1 = s1.substr(0, nq);

	// Now we have the tag name
	if (s1 == "3.1.0" || s1[0] != '3') { // It's never gonna be 4
		if (act) PostMessage(hWnd, OWM_UPDATESTATUS, 3, 0); //MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
		canUpdateCheck = true;
		return;
	} else {
		strncpy(tVerName, s1.c_str(), 64);
		PostMessage(hWnd, OWM_UPDATESTATUS, 0, (LPARAM)tVerName);
	}
	canUpdateCheck = true;
	return;
}

//main procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK AreaWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void OpenSongProperties(HWND hwnd);
//BOOL CALLBACK DialogSetting(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogDefault(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogDelete(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogCopy(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogCopy2(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogPan(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogTrans(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogVolume(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogWave(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogPlayer(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogTrack(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogNoteUsed(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogMemo(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogHelp(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK DialogEZCopy(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogSwap(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogTheme(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogWavExport(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogWaveDB(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogDecayLength(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogAdvPaste(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogSelPan(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogSelVol(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogSelTra(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogDefaults(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam);


void SetModified(bool mod);

//Declare global variables here
HINSTANCE hInst;//instance handle
HWND hWnd;//main window handle
//HWND hDlgTrack;
//HWND hDlgEZCopy;
HWND hDlgHelp = NULL;
BOOL actApp;

bool gPlayMidNote = true;

bool gIsDrawing = false;
bool gFileModified = false;
bool gFileUnsaved = true;

long MAXHORZRANGE = 0x7FFFFFFF;

int WWidth = WINDOWWIDTH, WHeight = WINDOWHEIGHT;
int realWidth = WINDOWWIDTH;
int realHeight = WINDOWHEIGHT;

static const char szClassName[] = "OrgMaker3";
static const char szAreaClass[] = "OrgArea";

char lpszName[MAX_PATH+30];// = "Organya 2 - ";//Name to register on Windows

ORGDATA org_data;//main data
SCROLLDATA scr_data;//scroll data
MOUSEDATA mouse_data;//mouse data
extern char music_file[];//file name
int sGrid = 0;
int sACrnt = 0;
char TrackN[] = "12345678QWERTYUI";

int sMetronome = 0;
int sSmoothScroll = 0;

extern int volChangeLength;
extern bool volChangeUseNoteLength;
extern bool volChangeSetNoteLength;

extern bool gLMBDown;
extern bool gRMBDown;
extern bool gMMBDown;

static bool hasMouseCapture = false;

int toolbarSavedX[2] = { 100, 100 };
int toolbarSavedY[2] = { 100, 160 };

static bool autoCheckUpdate = true;
static bool floatingToolbars = false;

extern void ChangeTrack(int iTrack);
extern void ChangeTrackPlus(int iValue);
extern char timer_sw; //Playing?
//extern int EZCopyWindowState; //Easy copy status
//extern RECT CmnDialogWnd;
//extern int SaveWithInitVolFile;	//Song data and... save.
extern int Menu_Recent[];
extern int iDragMode;
extern int iDlgRepeat; //Iteration count obtained from the dialog
extern char strMIDI_TITLE[256];
extern char strMIDI_AUTHOR[256];
extern unsigned char ucMIDIProgramChangeValue[MAXTRACK];

//for saving window size
RECT WinRect;
CHAR *buf;
CHAR app_path[BUF_SIZE];
CHAR num_buf[BUF_SIZE];
//to this point

HWND hwndArea = NULL;
HWND hwndRebar = NULL;
HWND hwndStatus = NULL;

HWND hwndToolbar = NULL;
HWND hwndTrackbar = NULL;
HWND hwndToolbarPopup = NULL;
HWND hwndTrackbarPopup = NULL;

int rebarHeight;

SAVEDNOTE gClipboardData;
NOTECOPY nc_Select;
int tra = -256, ful = 0, haba = 0;

static const int playbar_controls_menu[67 - 6 - 6] = {
	IDM_SORTMUSICNOTE, IDM_DLGDELETE, IDM_DLGCOPY, IDM_DLGCOPY2, IDM_DLGSWAP,
	IDM_2BAI, IDM_3BAI, IDM_2BUNNO1, IDM_3BUNNO1, IDM_CT_L1, IDM_CT_L2, IDM_CT_L3,
	IDM_CT_L4, IDM_CT_L5, IDM_CT_L6, IDM_CT_L7, IDM_CT_L8,
	IDM_CT_L9, IDM_CT_L10, IDM_CT_L11, IDM_CT_L12, IDM_CT_L13,
	/*IDM_CT_L14, IDM_CT_L15, IDM_CT_L16, IDM_CT_L17, IDM_CT_L18,
	IDM_CT_L19,*/ IDM_CT_S1, IDM_CT_S2, IDM_CT_S3, IDM_CT_S4,
	IDM_CT_S5, IDM_CT_S6, IDM_CT_S7, IDM_CT_S8, IDM_CT_S9,
	IDM_CT_S10, IDM_CT_S11, IDM_CT_S12, IDM_CT_S13, /*IDM_CT_S14,
	IDM_CT_S15, IDM_CT_S16, IDM_CT_S17, IDM_CT_S18, IDM_CT_S19,*/
	IDM_CT_S20, IDM_CT_OCT_DOWN, IDM_CT_OCT_UP, IDM_CT_PAN_R, IDM_CT_PAN_L,
	IDM_CT_PAN_REVERSE, IDM_CT_TRANS_UP, IDM_CT_TRANS_DOWN, IDM_CT_VOL_PLUS, IDM_CT_VOL_MINUS,
	IDM_CT_VOLWARIAI_UP, IDM_CT_VOLWARIAI_DOWN, IDM_ML_PAN_R, IDM_ML_PAN_L, IDM_ML_TRANS_UP,
	IDM_ML_TRANS_DOWN, IDM_ML_VOL_PLUS, IDM_ML_VOL_MINUS, IDM_DR_VOL_PLUS, IDM_DR_VOL_MINUS
};

static const int playbar_controls_select[9]{
	ID_SELECTION_CLEAR, ID_SELECTION_DELETE,ID_SELECTION_INSERT,ID_SELECTION_VOLUME,ID_SELECTION_PANNING,
	ID_SELECTION_TRANSPOSE, IDM_SELECT_RESET, IDM_SELECT_CUT, IDM_SELECT_COPY
};

static const int playbar_controls_toolbar[4] = {
	IDC_LEFT, IDC_RIGHT, IDC_LEFTSTEP, IDC_RIGHTSTEP
};

static const int iChgTrackBtn[16] = {
	IDM_TRACK1, IDM_TRACK2, IDM_TRACK3, IDM_TRACK4, IDM_TRACK5, IDM_TRACK6, IDM_TRACK7, IDM_TRACK8,
	IDM_TRACKQ, IDM_TRACKW, IDM_TRACKE, IDM_TRACKR, IDM_TRACKT, IDM_TRACKY, IDM_TRACKU, IDM_TRACKI,
};

static bool lastUpdCheck = false;
void UpdateToolbarStatus() {
	HMENU hMenu;
	hMenu = GetMenu(hWnd);

	// disable editor features while song is playing
	bool enabled = timer_sw == 0;
	if (lastUpdCheck != enabled) {
		for (int i = 0; i < 67 - 6 - 6; ++i) {
			EnableMenuItem(hMenu, playbar_controls_menu[i], MF_BYCOMMAND | (enabled ? MF_ENABLED : MF_GRAYED));
		}

		for (int i = 0; i < 4; ++i) {
			SendMessage(hwndToolbar, TB_ENABLEBUTTON, playbar_controls_toolbar[i], enabled);
		}

		// change icon to pause while playing
		SendMessage(hwndToolbar, TB_CHANGEBITMAP, IDM_PLAYPAUSE, enabled ? 12 : 13);

		lastUpdCheck = enabled;
	}

	// special cases
	EnableMenuItem(hMenu, IDM_SAVEOVER, MF_BYCOMMAND | (gFileModified ? MF_ENABLED : MF_GRAYED));
	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_SAVEOVER, gFileModified);

	EnableMenuItem(hMenu, IDM_UNDO, MF_BYCOMMAND | (enabled && org_data.UndoEnable ? MF_ENABLED : MF_GRAYED));
	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_UNDO, enabled && org_data.UndoEnable);
	EnableMenuItem(hMenu, IDM_REDO, MF_BYCOMMAND | (enabled && org_data.RedoEnable ? MF_ENABLED : MF_GRAYED));
	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_REDO, enabled && org_data.RedoEnable);

	for (int i = 0; i < 9; ++i) {
		EnableMenuItem(hMenu, playbar_controls_select[i], MF_BYCOMMAND | (enabled && tra >= 0 ? MF_ENABLED : MF_GRAYED));
	}

	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_SELECT_CUT, enabled && tra >= 0);
	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_SELECT_COPY, enabled && tra >= 0);

	EnableMenuItem(hMenu, IDM_SELECT_PASTE, MF_BYCOMMAND | (enabled && gClipboardData.track1 != -1 ? MF_ENABLED : MF_GRAYED));
	EnableMenuItem(hMenu, IDM_SELECT_ADVPASTE, MF_BYCOMMAND | (enabled && gClipboardData.track1 != -1 ? MF_ENABLED : MF_GRAYED));
	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_SELECT_PASTE, enabled && gClipboardData.track1 != -1);
	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_SELECT_ADVPASTE, enabled && gClipboardData.track1 != -1);

	// min/max zoom
	EnableMenuItem(GetMenu(hWnd), IDM_LOUPE_MINUS, NoteWidth > 4 ? MF_ENABLED : MF_GRAYED);
	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_LOUPE_MINUS, NoteWidth > 4);
	EnableMenuItem(GetMenu(hWnd), IDM_LOUPE_PLUS, NoteWidth < 16 ? MF_ENABLED : MF_GRAYED);
	SendMessage(hwndToolbar, TB_ENABLEBUTTON, IDM_LOUPE_PLUS, NoteWidth < 16);

	for (int i = 0; i < MAXTRACK; ++i) {
		SendMessage(hwndTrackbar, TB_CHECKBUTTON, iChgTrackBtn[i], org_data.track == i);
		SendMessage(hwndTrackbar, TB_CHANGEBITMAP, iChgTrackBtn[i], org_data.mute[i] ? i + 16 : i);
	}
}
int previewOctave = 3;

void UpdateStatusBar(bool measonly) {
	MUSICINFO mi;
	char msg[256];
	int meas = 0, step = 0;
	long pl;

	org_data.GetMusicInfo(&mi);

	if (timer_sw == 0) {
		scr_data.GetScrollPosition(&pl, NULL);
	} else {
		org_data.GetPlayPos(NULL, &pl);
	}

	if (mi.dot != 0 && mi.line != 0) {
		meas = pl / (mi.dot * mi.line);
		step = pl % (mi.dot * mi.line);
	}

	if (!measonly) {
		snprintf(msg, 256, "Keyboard Octave: %d", previewOctave + 1);
		SendMessage(hwndStatus, SB_SETTEXT, 1, (LPARAM)msg);

		switch (NoteWidth) {
		case 4:		SendMessage(hwndStatus, SB_SETTEXT, 2, (LPARAM)"Zoom: 25%"); break;
		case 6:		SendMessage(hwndStatus, SB_SETTEXT, 2, (LPARAM)"Zoom: 37.5%"); break;
		case 8:		SendMessage(hwndStatus, SB_SETTEXT, 2, (LPARAM)"Zoom: 50%"); break;
		case 10:	SendMessage(hwndStatus, SB_SETTEXT, 2, (LPARAM)"Zoom: 62.5%"); break;
		case 12:	SendMessage(hwndStatus, SB_SETTEXT, 2, (LPARAM)"Zoom: 75%"); break;
		case 14:	SendMessage(hwndStatus, SB_SETTEXT, 2, (LPARAM)"Zoom: 87.5%"); break;
		case 16:	SendMessage(hwndStatus, SB_SETTEXT, 2, (LPARAM)"Zoom: 100%"); break;
		}

		snprintf(msg, 256, "Channel %s", TrackCode[org_data.track]);
		SendMessage(hwndStatus, SB_SETTEXT, 3, (LPARAM)msg);

		snprintf(msg, 256, "Wait: %d (%.3f BPM)", mi.wait, mi.wait <= 0 || mi.dot <= 0 ? 0 : 60000.0 / (double)(mi.wait * mi.dot));
		SendMessage(hwndStatus, SB_SETTEXT, 4, (LPARAM)msg);
	}

	snprintf(msg, 256, "Meas %d:%d", meas, step);
	SendMessage(hwndStatus, SB_SETTEXT, 5, (LPARAM)msg);
}

void SaveIniFile();

//opening flash
//#define WAITFLASH	256
//BOOL    CALLBACK DialogFlash(HWND, UINT, WPARAM, LPARAM);
int gWidthWindow;
int gHeightWindow;

int gDrawDouble;	//draw both trackgroups

int iChgTrackKey[16] = {
	ID_AC_1,	ID_AC_2,	ID_AC_3,	ID_AC_4,	ID_AC_5,	ID_AC_6,	ID_AC_7,	ID_AC_8,
	ID_AC_Q,	ID_AC_W,	ID_AC_E,	ID_AC_R,	ID_AC_T,	ID_AC_Y,	ID_AC_U,	ID_AC_I,
};

int iMuteKey[16]={
	ID_AC_S1, ID_AC_S2, ID_AC_S3, ID_AC_S4, ID_AC_S5, ID_AC_S6, ID_AC_S7, ID_AC_S8,
	ID_AC_SQ, ID_AC_SW, ID_AC_SE, ID_AC_SR, ID_AC_ST, ID_AC_SY, ID_AC_SU, ID_AC_SI, 
};

int iSoloKey[16] = {
	ID_AC_L1, ID_AC_L2, ID_AC_L3, ID_AC_L4, ID_AC_L5, ID_AC_L6, ID_AC_L7, ID_AC_L8,
	ID_AC_LQ, ID_AC_LW, ID_AC_LE, ID_AC_LR, ID_AC_LT, ID_AC_LY, ID_AC_LU, ID_AC_LI,
};

TCHAR strSize[128]; //for Debug	// 2010.08.14 A

int iKeyPhase[128];
int iCurrentPhase;
int iCast[256];

int iKeyPushDown[256]; // 2010.09.22 A

char strMIDIFile[MAX_PATH];

char gSelectedTheme[MAX_PATH];
char gSelectedWave[MAX_PATH];

static HACCEL Ac;

bool OpenDoSave(HWND hwnd, bool savenew) {
	char res;
	if (savenew || gFileUnsaved) {
		res = GetFileNameSave(hwnd, MessageString[IDS_STRING62]); //"Save As"
		if (res == MSGCANCEL) return false;
		if (res == MSGEXISFILE) {
			//if(MessageBox(hwnd,"Do you want to overwrite?","There is a file with the same name",MB_YESNO | MB_ICONEXCLAMATION)	// 2014.10.19 D
			if (msgbox(hwnd, IDS_NOTIFY_OVERWRITE, IDS_INFO_SAME_FILE, MB_YESNO | MB_ICONEXCLAMATION)	// 2014.10.19 A
				== IDNO) return false;
		}
	}
	org_data.SaveMusicData();
	SetModified(false);
	gFileUnsaved = false;
	return true;
}

int CancelDeleteCurrentData(int iMessagePattern = 1){
	int res;
	if(iChangeFinish!=0){	// A 2010.09.22
		if(gFileModified){
			//Confirm the end when there is a change. // A 2010.09.22
			/*if(iMessagePattern == 0){
				//if(MessageBox(hWnd,"Any unsaved content will be discarded. Initialize?", "Initialization confirmation",MB_OKCANCEL | MB_ICONASTERISK)==IDCANCEL)return 1;	// 2014.10.19 D
				res = msgbox(hWnd,IDS_NOTIFY_INITIALIZE, IDS_NOTIFY_TITLE_INITALIZE,MB_YESNOCANCEL | MB_ICONWARNING);	// 2014.10.19 A
			}else if(iMessagePattern == 1){
				//if(MessageBox(hWnd,"Any unsaved content will be discarded. Are you sure you want to quit?", "end confirmation",MB_OKCANCEL | MB_ICONASTERISK)==IDCANCEL)return 1;	// 2014.10.19 D
				res = msgbox(hWnd,IDS_NOTIFY_EXIT, IDS_NOTIFY_TITLE_EXIT,MB_YESNOCANCEL | MB_ICONWARNING);	// 2014.10.19 A
			}else if(iMessagePattern == 2){
				//if(MessageBox(hWnd,"Any unsaved content will be discarded. do you want to load the file?", "load confirmation",MB_OKCANCEL | MB_ICONASTERISK)==IDCANCEL)return 1;	// 2014.10.19 D
				res = msgbox(hWnd,IDS_NOTIFY_LOAD, IDS_NOTIFY_TITLE_LOAD,MB_YESNOCANCEL | MB_ICONWARNING);	// 2014.10.19 A
			}*/
			//res = msgbox(hWnd, IDS_NOTIFY_UNSAVED, IDS_NOTIFY_TITLE_UNSAVED, MB_YESNOCANCEL | MB_ICONWARNING);	// 2014.10.19 A
			TCHAR strMesssage[2048];
			wsprintf(strMesssage, MessageString[IDS_NOTIFY_UNSAVED], music_file);
			res = MessageBox(hWnd, strMesssage, MessageString[IDS_NOTIFY_TITLE_UNSAVED], MB_YESNOCANCEL | MB_ICONWARNING);
			if (res == IDCANCEL) return 1;
			if (res == IDYES) return OpenDoSave(hWnd, false) ? 0 : 1;
		}
	} 
	return 0;
}

void UpdateToolbars(bool floating) {
	if (floating) {
		if (hwndRebar) {
			DestroyWindow(hwndRebar);
		}
		hwndRebar = NULL;
		rebarHeight = 0;
	}
	else if (!hwndRebar) {
		hwndRebar = CreateRebar(hWnd);
		rebarHeight = GetBarHeight(hwndRebar);
	}
	
	if (hwndToolbar) DestroyWindow(hwndToolbar);
	if (hwndTrackbar) DestroyWindow(hwndTrackbar);
	if (hwndToolbarPopup) {
		GetWindowRect(hwndToolbarPopup, (LPRECT)&WinRect);
		toolbarSavedX[0] = WinRect.left;
		toolbarSavedY[0] = WinRect.top;
		DestroyWindow(hwndToolbarPopup);
	}
	if (hwndTrackbarPopup) {
		GetWindowRect(hwndTrackbarPopup, (LPRECT)&WinRect);
		toolbarSavedX[1] = WinRect.left;
		toolbarSavedY[1] = WinRect.top;
		DestroyWindow(hwndTrackbarPopup);
	}

	HWND wnds[4] = { NULL,NULL,NULL,NULL };
	CreateToolbars(floating ? NULL : hwndRebar, wnds);

	hwndToolbar = wnds[0];
	hwndToolbarPopup = wnds[1];
	hwndTrackbar = wnds[2];
	hwndTrackbarPopup = wnds[3];

	SendMessage(hWnd, WM_SIZE, 0, MAKELPARAM(realWidth, realHeight));
}

void InitBitmaps() {
	InitBitmap("MUSIC", BMPMUSIC);//piano roll
	InitBitmap("NOTE", BMPNOTE);//Note
	InitBitmap("NUMBER", BMPNUMBER);//number
	InitBitmap("PAN", BMPPAN);//Pan and volume

	if (hwndToolbar) {
		SetToolbarImageList(hwndToolbar, "TOOLBAR_ICONS", TOOLBAR_MAIN_BUTTON_COUNT);
		InvalidateRect(hwndToolbar, NULL, TRUE);
	}
	if (hwndTrackbar) {
		SetToolbarImageList(hwndTrackbar, "TRACKBAR_ICONS", TOOLBAR_CHANNEL_BUTTON_COUNT);
		InvalidateRect(hwndTrackbar, NULL, TRUE);
	}
}

void ReloadBitmaps() {
	MUSICINFO mi;
	RECT rect = { 0,0,WWidth,WHeight };//Area to update (track change)
	org_data.GetMusicInfo(&mi);
	
	InitBitmaps();
	InitCursor();
	//LoadPlayerBitmaps(hDlgPlayer);
	//LoadTrackBitmaps(hDlgTrack);

	//MakeMusicParts(mi.line, mi.dot);
	//MakePanParts(mi.line, mi.dot);

	//org_data.PutMusic();
	//RedrawWindow(hWnd, &rect, NULL, RDW_INVALIDATE | RDW_ERASENOW);
}

void GetApplicationPath(char* path) {
	char pth[MAX_PATH];
	char drv[MAX_PATH];
	char dir[MAX_PATH];
	DWORD length = GetModuleFileName(NULL, pth, MAX_PATH);
	_splitpath(pth, drv, dir, NULL, NULL);
	strcat(drv, dir);
	strcpy(path, drv);
}

// Oops!
LONG WINAPI OrgCrashHandler(EXCEPTION_POINTERS* ep) {
	MessageBox(NULL, "A fatal error has occurred. The program will now exit.", "OrgMaker Crash", MB_OK | MB_ICONERROR);

	return EXCEPTION_EXECUTE_HANDLER;
}

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR dropfile,int nCmdShow)
{//main function
	WNDCLASSEX wc;
//	MessageBox(hWnd,dropfile,"Drap",MB_OK);
#ifndef _DEBUG
	SetUnhandledExceptionFilter(OrgCrashHandler);
#endif
	InitMMTimer();  // 2010.09.21
	strSize[0]=0;	// 2010.08.14 A
	for(int jjj=0;jjj<128;jjj++)iKeyPhase[jjj]=-1;
	iCurrentPhase=0;
	int i, vvv;
	for(vvv=0;vvv<256;vvv++){
		iCast[vvv]=0xFFFF;
		iKeyPushDown[vvv]=0;
	}

	//load strings for messages
	AllocMessageStringBuffer();

	//initial file name
	strcpy(music_file, MessageString[IDS_DEFAULT_ORG_FILENAME]);

	iCast['Z']= 0;
	iCast['S']= 1;
	iCast['X']= 2;
	iCast['D'] = 3;
	iCast['C']= 4; //C … C sound
	iCast['V']= 5; //     D
	iCast['G']= 6;
	iCast['B']= 7; //     E
	iCast['H'] = 8;
	iCast['N']= 9; //     F
	iCast['J']= 10;
	iCast['M']= 11; //     G
	iCast[VK_OEM_COMMA]=12; //,    A
	iCast['L']= 13;
	iCast[VK_OEM_PERIOD]=14; //.    B
	iCast[VK_OEM_1]=15; //／   C
	iCast[VK_OEM_2]=16; //:
    
	LoadString(GetModuleHandle(NULL), IDS_TITLE, lpszName, sizeof(lpszName) / sizeof(lpszName[0]));

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;//CS_DBLCLKS| CS_OWNDC;//Application style
	wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInst = hInstance;
	wc.hIcon         = LoadIcon(hInst,"ICON");//big icon
	wc.hIconSm       = LoadIcon(hInst,"ICON");//small icon
	wc.hCursor       = LoadCursor(hInst,"CURSOR");//default cursor
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//window color
	wc.lpszMenuName  = "ORGANYAMENU";//menu
	wc.lpszClassName = szClassName;
	if (!RegisterClassEx(&wc)) return FALSE;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)AreaWndProc;
	wc.hInstance = hInst = hInstance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(hInst, "CURSOR");
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = szAreaClass;
	if (!RegisterClassEx(&wc)) return FALSE;

	int wnd_width;///Specify the width of the window here.
	int wnd_height;
	gWidthWindow = wnd_width = GetSystemMetrics(SM_CXFRAME)*2+//width of the frame
		GetSystemMetrics(SM_CXHSCROLL)+//scrollbar width
		WWidth;
	gHeightWindow = wnd_height = GetSystemMetrics(SM_CYFRAME)*2 +//frame height
		GetSystemMetrics(SM_CYCAPTION)+//caption height
		GetSystemMetrics(SM_CYMENU)+//menu bar height
		GetSystemMetrics(SM_CYVSCROLL)+//scrollbar height
		WHeight;

	GetModuleFileName(hInstance, app_path,BUF_SIZE - 1);
	buf=app_path + lstrlen(app_path) - 4;
	if(lstrcmpi(buf,".exe")==0){
		lstrcpy(buf,".ini");
	}else{
		lstrcat(app_path,".ini");
	}
	//NoteWidth = 16; //Frame designation
	NoteWidth =         GetPrivateProfileInt(MAIN_WINDOW,"NoteWidth",16,app_path);
	NoteWidth = (NoteWidth > 16) ? 16: ( (NoteWidth<4) ? 4: NoteWidth );

	WinRect.left=       GetPrivateProfileInt(MAIN_WINDOW,"left",0,app_path);
	WinRect.top=        GetPrivateProfileInt(MAIN_WINDOW,"top",0,app_path);
	WinRect.right=      GetPrivateProfileInt(MAIN_WINDOW,"right",640,app_path);
	WinRect.bottom=     GetPrivateProfileInt(MAIN_WINDOW,"bottom",480,app_path);

	/*CmnDialogWnd.left = GetPrivateProfileInt(COMMON_WINDOW, "left", 20, app_path);
	CmnDialogWnd.top=	GetPrivateProfileInt(COMMON_WINDOW,"top",	20,app_path);
	CmnDialogWnd.right=	GetPrivateProfileInt(COMMON_WINDOW,"right",	550,app_path);
	CmnDialogWnd.bottom=GetPrivateProfileInt(COMMON_WINDOW,"bottom",560,app_path);*/

	iDlgRepeat =        GetPrivateProfileInt(MIDI_EXPORT,"Repeat",1,app_path);

	char strtmp[128];
	SYSTEMTIME stTime; GetLocalTime(&stTime); //stTime.wYear get a year in // 2014.10.18
	sprintf(strtmp, "(C) AUTHOR xxxxx, %d", stTime.wYear); //, put the year after

	//GetPrivateProfileString(MIDI_EXPORT, "Author", "(C) AUTHOR xxxxx, 2014", strMIDI_AUTHOR, 255, app_path);	// 2045.01.18 D
	GetPrivateProfileString(MIDI_EXPORT, "Author", strtmp, strMIDI_AUTHOR, 255, app_path);	// 2045.01.18 A
	GetPrivateProfileString(MIDI_EXPORT, "Title", MessageString[IDS_DEFAULT_MIDI_TITLE], strMIDI_TITLE, 255, app_path);
	for (i = 0; i < 8; i++) ucMIDIProgramChangeValue[i] = 255;

	unsigned long ul = WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX;

	realWidth = WinRect.right;
	realHeight = WinRect.bottom;

	// main window
	hWnd = CreateWindow(szClassName,
			"OrgMaker 3",
			ul,
			WinRect.left, WinRect.top, realWidth, realHeight,
            NULL, NULL, hInst, NULL);


	if (!hWnd)
		return FALSE;

	hwndStatus = CreateStatusBar(hWnd);
	CreateToolbarClass();

	hwndArea = CreateWindow(szAreaClass, "", WS_CHILD | WS_VISIBLE, 0,
		rebarHeight, WinRect.right - WinRect.left, WinRect.bottom - WinRect.top - rebarHeight - GetBarHeight(hwndStatus),
		hWnd, NULL, hInst, NULL);

//	DialogBox(hInst,"DLGFLASH",NULL,DialogFlash);

	Ac = LoadAccelerators(hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR1)); //アクセスキー

	char sDir[MAX_PATH];
	GetApplicationPath(sDir);
	strcat(sDir, "themes");
	CreateDirectory(sDir, NULL);

	GetPrivateProfileString(MAIN_WINDOW, "CurrentThemePath", NULL, gSelectedTheme, MAX_PATH, app_path); //2023.06.10
	if (strlen(gSelectedTheme) > 0) {
		DWORD dwAttrib = GetFileAttributes(gSelectedTheme);
		if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
			gSelectedTheme[0] = 0; // Theme is gone
	}
	GetApplicationPath(sDir);
	strcat(sDir, "soundbanks");
	CreateDirectory(sDir, NULL);

	GetPrivateProfileString(MAIN_WINDOW, "CurrentWavePath", NULL, gSelectedWave, MAX_PATH, app_path); //2024.05.19
	if (strlen(gSelectedWave) > 0) {
		DWORD dwAttrib = GetFileAttributes(gSelectedWave);
		if (dwAttrib == INVALID_FILE_ATTRIBUTES || dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
			gSelectedWave[0] = 0; // soundbank is gone
	}

//Image initialization //////////
	if (!StartGDI(hwndArea)) { //GDI ready
		MessageBox(hWnd, "Graphics engine failed to initalize.", "OrgMaker Error", MB_ICONERROR | MB_OK);
		QuitMMTimer();
		if (hwndRebar) DestroyWindow(hwndRebar);
		DestroyWindow(hwndStatus);
		DestroyWindow(hwndArea);
		DestroyWindow(hWnd);
		return 0;
	}

	InitBitmaps();
	InitCursor();
//Sound initialization ///////
	if (!InitDirectSound(hWnd)) {
		MessageBox(hWnd, "Sound engine failed to initalize.", "OrgMaker Error", MB_ICONERROR | MB_OK);
		QuitMMTimer();
		EndGDI();
		if (hwndRebar) DestroyWindow(hwndRebar);
		DestroyWindow(hwndStatus);
		DestroyWindow(hwndArea);
		DestroyWindow(hWnd);
		return 0;
	}
	LoadWaveData100(gSelectedWave); // Load the current soundbank, or the default
	GenerateWaveGraphic(wave_data);
	scr_data.InitScroll();

	InitSoundObject("METRO01", 1);
	InitSoundObject("METRO02", 2);
	InitSoundObject("CAT", 3);
	
	//hDlgPlayer = CreateDialog(hInst,"PLAYER",hWnd,DialogPlayer);
	//hDlgTrack = CreateDialog(hInst,"TRACK",hWnd,DialogTrack);
	//hDlgEZCopy = CreateDialog(hInst,"COPYBD",hWnd,DialogEZCopy);

	HMENU hMenu;
	hMenu = GetMenu(hWnd);

	//hDlgShortCutList = CreateDialog(hInst,"DLGSHORTCUTINFO",hWnd,DialogShortCut);
	/*WinRect.left = GetPrivateProfileInt(TRACK_WINDOW, "left", 200, app_path);
	WinRect.top=GetPrivateProfileInt(TRACK_WINDOW,"top",200,app_path);
	SetWindowPos(hDlgTrack,NULL,WinRect.left,WinRect.top,0,0,SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);*/
	/*WinRect.left = GetPrivateProfileInt(PLAY_WINDOW, "left", 280, app_path);
	WinRect.top=GetPrivateProfileInt(PLAY_WINDOW,"top",280,app_path);
	SetWindowPos(hDlgPlayer,NULL,WinRect.left,WinRect.top,0,0,SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);*/
	WinRect.left=GetPrivateProfileInt(COPY_WINDOW,"left",180,app_path);
	WinRect.top=GetPrivateProfileInt(COPY_WINDOW,"top",380,app_path);
	/*SetWindowPos(hDlgEZCopy, NULL, WinRect.left, WinRect.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
	EZCopyWindowState = GetPrivateProfileInt(COPY_WINDOW,"show",1,app_path);
	if (EZCopyWindowState == 0) {
		ShowWindow(hDlgEZCopy, SW_HIDE);
		//CheckMenuItem(hMenu, IDM_EZCOPYVISIBLE, (MF_BYCOMMAND | MFS_UNCHECKED));
	}
	else {
		ShowWindow(hDlgEZCopy, SW_SHOWNOACTIVATE);
		//CheckMenuItem(hMenu, IDM_EZCOPYVISIBLE, (MF_BYCOMMAND | MFS_CHECKED));
	}*/
	//SaveWithInitVolFile = GetPrivateProfileInt(INIT_DATA,"autosave",0,app_path);
	//ChangeAutoLoadMode(SaveWithInitVolFile);
	org_data.InitOrgData();

	
	LoadRecentFromIniFile();
	ChangeGridMode(GetPrivateProfileInt(MAIN_WINDOW,"GridMode",0,app_path));
	ChangeSelAlwaysCurrent(GetPrivateProfileInt(MAIN_WINDOW,"AlwaysCurrent",0,app_path));
	ChangeDrawDouble(GetPrivateProfileInt(MAIN_WINDOW,"DrawDouble",1,app_path));
	ChangeDragMode(GetPrivateProfileInt(MAIN_WINDOW,"DragMode",1,app_path));
	ChangeEnablePlaying(GetPrivateProfileInt(MAIN_WINDOW,"EnablePlaying",1,app_path));
	ChangeFinish(GetPrivateProfileInt(MAIN_WINDOW,"QuitMessage",1,app_path));
	ChangeSlideOverlapNoteMode(GetPrivateProfileInt(MAIN_WINDOW,"SlideOverlapNoteMode",1,app_path));
	ChangePushStratchNOTE(GetPrivateProfileInt(MAIN_WINDOW,"EnablePressNoteStretch",1,app_path));
	ChangeNoteEnlarge(GetPrivateProfileInt(MAIN_WINDOW,"NoteEnlarge",1,app_path)); //2014.05.28 Make note heads stand out when zoomed out
	ChangeMetronomeMode(GetPrivateProfileInt(MAIN_WINDOW,"Metronome",0,app_path)); //2023.06.09
	ChangeScrollMode(GetPrivateProfileInt(MAIN_WINDOW, "SmoothScroll", 1, app_path)); //2023.06.09

	volChangeLength = GetPrivateProfileInt(MAIN_WINDOW, "VolChangeLength", 10, app_path);
	volChangeUseNoteLength = GetPrivateProfileInt(MAIN_WINDOW, "VolChangeUseNoteLength", 1, app_path);
	volChangeSetNoteLength = GetPrivateProfileInt(MAIN_WINDOW, "VolChangeSetNoteLength", 0, app_path);

	autoCheckUpdate = GetPrivateProfileInt(MAIN_WINDOW, "AutoCheckUpdates", 1, app_path);
	floatingToolbars = GetPrivateProfileInt(MAIN_WINDOW, "FloatingToolbars", 0, app_path);
	gPlayMidNote = GetPrivateProfileInt(MAIN_WINDOW, "PlayMidNote", 1, app_path);

	
	CheckMenuItem(hMenu, IDM_AUTOCHECKUPDATES, MF_BYCOMMAND | (autoCheckUpdate ? MFS_CHECKED : MFS_UNCHECKED));
	CheckMenuItem(hMenu, IDM_FLOATTOOLBARS, MF_BYCOMMAND | (floatingToolbars ? MFS_CHECKED : MFS_UNCHECKED));
	CheckMenuItem(hMenu, IDM_PLAY_NOTES_MID, MF_BYCOMMAND | (gPlayMidNote ? MFS_CHECKED : MFS_UNCHECKED));

	for (i = 0; i < MAXTRACK; ++i) {
		snprintf(strtmp, 128, "Channel%dDefaultVol", i);
		org_data.def_volume[i] = GetPrivateProfileInt(MAIN_WINDOW, strtmp, 200, app_path);

		snprintf(strtmp, 128, "Channel%dDefaultPan", i);
		org_data.def_pan[i] = GetPrivateProfileInt(MAIN_WINDOW, strtmp, 6, app_path);
	}
	
	//org_data.PutMusic();//View sheet music

	if(GetPrivateProfileInt(MAIN_WINDOW,"WindowState",0,app_path)==1){
		ShowWindow(hWnd,SW_MAXIMIZE);
	}else{
		ShowWindow(hWnd,nCmdShow);//Main window display
	}
	UpdateWindow(hWnd);//Update main window

	toolbarSavedX[0] = GetPrivateProfileInt(TOOLBAR_POS, "ToolbarMainX", 100, app_path);
	toolbarSavedY[0] = GetPrivateProfileInt(TOOLBAR_POS, "ToolbarMainY", 100, app_path);
	toolbarSavedX[1] = GetPrivateProfileInt(TOOLBAR_POS, "ToolbarChannelX", 100, app_path);
	toolbarSavedY[1] = GetPrivateProfileInt(TOOLBAR_POS, "ToolbarChannelY", 160, app_path);

	DragAcceptFiles(hWnd,TRUE);//Now allow dragging
	//Generate player dialog (modalless)

	//if(GetPrivateProfileInt(MAIN_WINDOW,"WindowState",0,app_path)==1){
	//	ShowWindow(hWnd,SW_MAXIMIZE);
	//}

	SetModified(false);//title name set
	//ClearVirtualCB(); //Initialize the virtual clipboard
	ClearUndo();

	memset(&gClipboardData, 0, sizeof(gClipboardData));
	gClipboardData.track1 = -1;

	//Show to Player
	//MUSICINFO mi;
	//org_data.GetMusicInfo( &mi );
	//SetDlgItemInt(hDlgTrack,IDE_VIEWWAIT,mi.wait,TRUE );
	//SetDlgItemInt(hDlgTrack,IDE_VIEWTRACK,0,TRUE );
	//SetDlgItemText(hDlgTrack,IDE_VIEWTRACK,"1");
	UpdateStatusBar(false);

	FILE *fp;
	char kfn[MAX_PATH],gfn[MAX_PATH];
	if(dropfile[0]!=0){
		strcpy(kfn,dropfile);
		int ttt;
		if(dropfile[0]=='\"'){	//remove quotes
			ttt = 1;
		}else{
			ttt = 0;
		}
		strcpy(gfn,&kfn[ttt]);
		int ggg = strlen(gfn)-ttt;
		gfn[ggg]=0;
		//MessageBox(hWnd,gfn,"Error(Load)",MB_OK);

		if (!org_data.FileCheckBeforeLoad(gfn)) { //A 2010.09.25 If the file is in Org format
			strcpy(music_file, gfn);
			if (org_data.LoadMusicData() == TRUE) { //C 2010.09.25 Judgment added
				SetModified(false);//title name set
				gFileUnsaved = false;
				//DetectPreciseMode();
				//org_data.GetMusicInfo(&mi);
				//SetDlgItemInt(hDlgTrack, IDE_VIEWWAIT, mi.wait, TRUE);
				//SetDlgItemText(hDlgTrack, IDE_VIEWTRACK, "1");
				UpdateStatusBar(false);
				SelectReset();
				//org_data.PutMusic();
			}
			else {
				//Because it was not an ORG format file //A 2010.9.25
				//File name clear
				strcpy(music_file, MessageString[IDS_DEFAULT_ORG_FILENAME]);
			}
		}

		/*fp=fopen(gfn,"rb");
		if(fp==NULL){
			//MessageBox(hWnd,"Failed to read","Error(Load)",MB_OK); //D 2010.09.28
		}else{
			char pass_chek[3];
			bool b_OrgFile = false;
			fread(&pass_chek[0], sizeof(char), 3, fp);
			b_OrgFile = (pass_chek[0]==0x4F) && (pass_chek[1]==0x72) && (pass_chek[2]==0x67); //'O', 'r', 'g'
			fclose(fp);
			//memcpy(music_file,dropfile,MAX_PATH);
			if(b_OrgFile){ //A 2010.09.25 If the file is in Org format
				strcpy(music_file, gfn);
				if(org_data.LoadMusicData()==TRUE){ //C 2010.09.25 Judgment added
					SetModified(false);//title name set
                    gFileUnsaved = false;
					//DetectPreciseMode();
					org_data.GetMusicInfo( &mi );
					SetDlgItemInt(hDlgTrack,IDE_VIEWWAIT,mi.wait,TRUE );
					SetDlgItemText(hDlgTrack,IDE_VIEWTRACK,"1");
					ClearEZC_Message();
					SelectReset();
					org_data.PutMusic();
				}else{
					//Because it was not an ORG format file //A 2010.9.25
					//File name clear
					strcpy(music_file, MessageString[IDS_DEFAULT_ORG_FILENAME]);
				}
			}
		}*/
	}

	QuitMMTimer(); //A 2010.09.21

	UpdateToolbars(floatingToolbars);
	UpdateToolbarStatus();

	org_data.PutMusic();
	if (RefleshScreen(hWnd)) {
		if (autoCheckUpdate) {
			std::thread t(CheckUpdate, false);
			t.detach();
		}

		while (TRUE) {
			org_data.PutMusic();
			if (!RefleshScreen(hWnd)) {
				break;
			}
		}
	}

	//Generate message loop (main loop)
	/*while(GetMessage(&msg,NULL,0,0)){
//		if(!TranslateAccelerator(hwnd,hAccel,&msg)){
		//Unless the message is for a dialog
		if(!TranslateAccelerator(hWnd,Ac,&msg))
        {
			if(!IsDialogMessage(hDlgPlayer,&msg)){
				if(!IsDialogMessage(hDlgTrack,&msg)){
					if(!IsDialogMessage(hDlgEZCopy,&msg)){
						if(!IsDialogMessage(hDlgHelp,&msg)){
							TranslateMessage(&msg);//keyboard available
							DispatchMessage(&msg);//Return control to Windows
						}
					}				
				}
			}
				//TranslateMessage(&msg);
				//DispatchMessage(&msg);
        }
	}*/
	//MessageBox(NULL, "message loop exited", "OK", MB_OK);

	DestroyAcceleratorTable(Ac);
	return 0; //application ends here
}

BOOL SystemTask(void)
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if (!GetMessage(&msg, NULL, 0, 0))
			return FALSE;

		if (!TranslateAccelerator(hWnd, Ac, &msg)) {
			if (!hwndToolbarPopup || !IsDialogMessage(hwndToolbarPopup, &msg)) {
				if (!hwndTrackbarPopup || !IsDialogMessage(hwndTrackbarPopup, &msg)) {
					if (!hDlgHelp || !IsDialogMessage(hDlgHelp, &msg)) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
			}
		}
	}

	return TRUE;
}

void ShowStatusMessage(void)
{
	/*char Mess[16];
	if(sACrnt)strcpy(Mess,"Current");
	else strcpy(Mess,"");
	SetDlgItemText(hDlgEZCopy, IDC_STATUS1, Mess);

	if(sGrid)strcpy(Mess,"Grid");
	else strcpy(Mess,"");
	SetDlgItemText(hDlgEZCopy, IDC_STATUS2, Mess);

	if(iDragMode)strcpy(Mess,"Drag");
	else strcpy(Mess,"");
	SetDlgItemText(hDlgEZCopy, IDC_STATUS3, Mess);*/

	UpdateToolbarStatus();
	UpdateStatusBar(false);


	//SetDlgItemText(hDlgEZCopy, IDC_STATUS, Mess);

}

LRESULT CALLBACK AreaWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_SIZE:
		WWidth = LOWORD(lParam);	//Client area size
		WHeight = HIWORD(lParam);
		return 0;
	case WM_MOUSEMOVE:
		if (!hasMouseCapture) {
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.hwndTrack = hwndArea;
			tme.dwFlags = TME_LEAVE;
			TrackMouseEvent(&tme);
			hasMouseCapture = true;
		}
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_COMMAND:
	case WM_DROPFILES:
		SendMessage(hWnd, message, wParam, lParam);
		return 0;
	case WM_MOUSELEAVE:
		if (gLMBDown)
			SendMessage(hWnd, WM_LBUTTONUP, wParam, lParam);
		if (gRMBDown)
			SendMessage(hWnd, WM_RBUTTONUP, wParam, lParam);
		if (gMMBDown)
			SendMessage(hWnd, WM_MBUTTONUP, wParam, lParam);
		hasMouseCapture = false;
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

void ClipboardPaste(int no, int flags) {
	if (gClipboardData.track1 == -1 || no <= 0) {
		return;
	}

	long x_scroll;
	scr_data.GetScrollPosition(&x_scroll, NULL);

	SetUndo();

	nc_Select.x1_1 = (tra >= 0 && nc_Select.x1_1 == nc_Select.x1_2) ? nc_Select.x1_1 : x_scroll;
	nc_Select.x1_2 = nc_Select.x1_1 + (gClipboardData.length * no) - 1;
	org_data.PasteNoteData(&gClipboardData, org_data.track, nc_Select.x1_1, no, flags);

	if (gClipboardData.track1 == gClipboardData.track2) {
		org_data.CheckNoteTail(org_data.track);
		tra = org_data.track;
		ful = 0;
	}
	else {
		for (int i = gClipboardData.track1; i <= gClipboardData.track2; ++i) {
			org_data.CheckNoteTail(i);
		}
		tra = gClipboardData.track1;
		ful = 1;
	}

	ShowStatusMessage();
}

static int Bn[] = {
	ID_AC_NUM1,
	ID_AC_NUM2,
	ID_AC_NUM3,
	ID_AC_NUM4,
	ID_AC_NUM5,
	ID_AC_NUM6,
	ID_AC_C_NUM1,
	ID_AC_C_NUM2,
	ID_AC_C_NUM3,
	ID_AC_C_NUM4,
	ID_AC_C_NUM5,
	ID_AC_C_NUM6,
};

static int Hni[] = {
	1,2,3,4,8,16,
	1,2,3,4,8,16,
};

static bool IsSolo(char track) {
	bool un = true;
	for (int j = 0; j < MAXTRACK; ++j) {
		if (org_data.mute[j] != (track != j)) {
			un = false;
			break;
		}
	}
	return un;
}
static bool IsAnyMuted(void) {
	bool un = true;
	for (int j = 0; j < MAXTRACK; ++j) {
		if (org_data.mute[j]) {
			un = false;
			break;
		}
	}
	return !un;
}
static bool IsAnyUnmuted(void) {
	bool un = true;
	for (int j = 0; j < MAXTRACK; ++j) {
		if (!org_data.mute[j]) {
			un = false;
			break;
		}
	}
	return !un;
}
static bool IsAnyMutedSide(int side) {
	bool un = true;
	for (int j = (side == 0 ? 0 : MAXMELODY); j < (side == 0 ? MAXMELODY : MAXTRACK); ++j) {
		if (org_data.mute[j]) {
			un = false;
			break;
		}
	}
	return !un;
}
static bool IsAnyUnmutedSide(int side) {
	bool un = true;
	for (int j = (side == 0 ? 0 : MAXMELODY); j < (side == 0 ? MAXMELODY : MAXTRACK); ++j) {
		if (!org_data.mute[j]) {
			un = false;
			break;
		}
	}
	return !un;
}

//main procedure
LRESULT CALLBACK WndProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
//	char str[64];
	int i, j;	// 2014.10.18 Added j
	char res;
	bool AfterReSize=false;
	RECT rect = {0,0,WWidth,WHeight};//Area to update (track change)
	MUSICINFO mi;
	MINMAXINFO *pmmi;
	HMENU hMenu;
	SCROLLINFO si;

	


//	char name[MAX_PATH];	// 2010.12.01 MOVED
//	strcpy(name, MessageString[IDS_DEFAULT_ORG_FILENAME]);
	char str[128];
	
	switch(message){
	case OWM_UPDATESTATUS: {
		switch (wParam) {
		case 0: {
			char* ver = (char*)lParam;

			char msg[256];
			snprintf(msg, 256, "Version %s is now available. Would you like to download it?", ver);
			int h = MessageBox(hWnd, msg, "OrgMaker Update", MB_ICONINFORMATION | MB_OKCANCEL);
			if (h == IDOK) {
				snprintf(msg, 256, "https://github.com/Strultz/orgmaker-3/releases/tag/%s", ver);
				ShellExecute(NULL, "open", msg, NULL, NULL, SW_SHOWNORMAL);
			}
			break;
		}
		case 2:
			MessageBox(hWnd, "Recieved invalid response from server. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
			break;
		case 3:
			MessageBox(hWnd, "No new updates are available.", "OrgMaker Update", MB_ICONINFORMATION | MB_OK);
			break;
		default:
		case 1:
			MessageBox(hWnd, "Request failed. Please try again later.", "OrgMaker Update", MB_ICONERROR | MB_OK);
			break;
		}
	}
	case OWM_TBCONTEXTMENU: {
		for (i = 0; i < MAXTRACK; i++) {
			if (wParam == iChgTrackBtn[i]) {
				hMenu = CreatePopupMenu();

				snprintf(str, 128, "Select\t%c", TrackN[i]);
				AppendMenu(hMenu, MF_STRING, iChgTrackKey[i], str);

				snprintf(str, 128, "%sute\tShift+%c", org_data.mute[i] ? "Unm" : "M", TrackN[i]);
				AppendMenu(hMenu, MF_STRING, iMuteKey[i], str);

				if (!IsSolo(i)) {
					snprintf(str, 128, "%s", "Solo");
					AppendMenu(hMenu, MF_STRING, iSoloKey[i], str);
				}
				/*if (IsAnyUnmuted()) {
					snprintf(str, 128, "%s", "Mute All");
					AppendMenu(hMenu, MF_STRING, IDC_MUTE_ALL, str);
				}*/
				if (IsAnyMuted()) {
					snprintf(str, 128, "%s", "Unmute All");
					AppendMenu(hMenu, MF_STRING, IDC_UNMUTE_ALL, str);
				}
				if (IsAnyUnmutedSide(i / 8)) {
					snprintf(str, 128, "Mute %s Channels", (i < MAXMELODY) ? "Melody" : "Percussion");
					AppendMenu(hMenu, MF_STRING, (i < MAXMELODY) ? IDC_MUTE_MELO : IDC_MUTE_PERC, str);
				}
				if (IsAnyMutedSide(i / 8)) {
					snprintf(str, 128, "Unmute %s Channels", (i < MAXMELODY) ? "Melody" : "Percussion");
					AppendMenu(hMenu, MF_STRING, (i < MAXMELODY) ? IDC_UNMUTE_MELO : IDC_UNMUTE_PERC, str);
				}

				TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, LOWORD(lParam), HIWORD(lParam), 0, hwnd, NULL);
				DestroyMenu(hMenu);
			}
		}
		break;
	}
	case WM_COMMAND:
		for (i = 0; i < MAXTRACK; i++) {
			if (LOWORD(wParam) == iChgTrackKey[i]){
				ChangeTrack(i);
				//return FALSE;
				//SendDlgItemMessage(hDlgTrack , IDC_TRACK0 , BM_CLICK , 0, 0);
			}
			else if (LOWORD(wParam) == iChgTrackBtn[i]) {
				if (GetKeyState(VK_MENU) & 0x8000) { // Side
					char d = (org_data.mute[i] == 0);
					for (j = 0; j < (i < 8 ? MAXMELODY : MAXDRAM); ++j) {
						org_data.mute[j + (i < 8 ? 0 : 8)] = d;
					}
					UpdateToolbarStatus();
				} else if (GetKeyState(VK_CONTROL) & 0x8000) { // Solo
					bool un = IsSolo(i);
					for (j = 0; j < MAXTRACK; ++j) {
						org_data.mute[j] = (!un && i != j);
					}
					UpdateToolbarStatus();
				} else if (org_data.track == i || GetKeyState(VK_SHIFT) & 0x8000) { // Mute
					MuteTrack(i);
				} else { // Select
					ChangeTrack(i);
				}
			}
			else if (LOWORD(wParam) == iMuteKey[i]) {
				MuteTrack(i);
			}
			else if (LOWORD(wParam) == iSoloKey[i]) {
				bool un = IsSolo(i);
				for (j = 0; j < MAXTRACK; ++j) {
					org_data.mute[j] = (!un && i != j);
				}
				UpdateToolbarStatus();
			}
		}
		switch (LOWORD(wParam)) {
		case IDC_MUTE_ALL:
			for (j = 0; j < MAXTRACK; ++j) {
				org_data.mute[j] = true;
			}
			UpdateToolbarStatus();
			break;
		case IDC_UNMUTE_ALL:
			for (j = 0; j < MAXTRACK; ++j) {
				org_data.mute[j] = false;
			}
			UpdateToolbarStatus();
			break;
		case IDC_MUTE_MELO:
			for (j = 0; j < MAXMELODY; ++j) {
				org_data.mute[j] = true;
			}
			UpdateToolbarStatus();
			break;
		case IDC_UNMUTE_MELO:
			for (j = 0; j < MAXMELODY; ++j) {
				org_data.mute[j] = false;
			}
			UpdateToolbarStatus();
			break;
		case IDC_MUTE_PERC:
			for (j = MAXMELODY; j < MAXTRACK; ++j) {
				org_data.mute[j] = true;
			}
			UpdateToolbarStatus();
			break;
		case IDC_UNMUTE_PERC:
			for (j = MAXMELODY; j < MAXTRACK; ++j) {
				org_data.mute[j] = false;
			}
			UpdateToolbarStatus();
			break;
		}
		/*if (LOWORD(wParam) == IDM_EZCOPYVISIBLE || LOWORD(wParam) == ID_AC_SHOWEZCOPY) {
			hMenu = GetMenu(hWnd);
			if(EZCopyWindowState==0){
				EZCopyWindowState=1;
				ShowWindow(hDlgEZCopy, SW_SHOWNOACTIVATE);
				//CheckMenuItem(hMenu, IDM_EZCOPYVISIBLE, (MF_BYCOMMAND | MFS_CHECKED));
			}else{
				EZCopyWindowState=0;
				ShowWindow(hDlgEZCopy, SW_HIDE);
				//CheckMenuItem(hMenu, IDM_EZCOPYVISIBLE, (MF_BYCOMMAND | MFS_UNCHECKED));
			}
		}*/
		for(i=0;i<10;i++){	//recently used files
			if(LOWORD(wParam)==Menu_Recent[i]){
				if (CancelDeleteCurrentData(CDCD_LOAD)) break;
				if (!SetLoadRecentFile(i)) {
					msgbox(hWnd, IDS_STRING64, IDS_ERROR_LOAD, MB_OK | MB_ICONWARNING);
					break;
				}

				if (timer_sw != 0) // Stop playing song
					StopPlayingSong();

				ClearUndo(); // 2023.06.10 Someone forgot to put this here
				org_data.InitOrgData();
				org_data.LoadMusicData();
				SetModified(false);//title name set
                gFileUnsaved = false;
				//DetectPreciseMode();
				//org_data.GetMusicInfo( &mi );
				//SetDlgItemInt(hDlgTrack,IDE_VIEWWAIT,mi.wait,TRUE );
				//SetDlgItemText(hDlgTrack,IDE_VIEWTRACK,"1");
				UpdateStatusBar(false);
				SelectReset();
				//org_data.PutMusic();
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

				//for(i=0;i<8;i++)ucMIDIProgramChangeValue[i]=255;	// 2014.10.18 D
				for(j=0;j<8;j++)ucMIDIProgramChangeValue[j]=255;	// 2014.10.18 A
				break;	// 2014.10.18 A
			}
		}
		if(timer_sw==0){ //no while playing
			switch(LOWORD(wParam)){
			case IDM_SORTMUSICNOTE: //to sort
				SetUndo();
				SortMusicNote();
				break;
				/*case IDM_DLGDELETE://
			case ID_AC_DELETE:
				//DialogBox(hInst,"DLGDELETE",hwnd,DialogDelete);
				break;
			case IDM_DLGCOPY://
			case ID_AC_COPYDLG:
				//DialogBox(hInst,"DLGCOPY",hwnd,DialogCopy);
				break;
			case IDM_DLGCOPY2://
			case ID_AC_COPY2:
				//DialogBox(hInst,"DLGCOPY2",hwnd,DialogCopy2);
				break;*/
			case ID_SELECTION_PANNING://
			case ID_AC_DLG_PAN:
				if (tra >= 0)
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DLGSELPAN), hwnd, DialogSelPan);
				break;
			case ID_SELECTION_TRANSPOSE://
			case ID_AC_DLG_TRANS:
				if (tra >= 0)
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DLGSELTRA), hwnd, DialogSelTra);
				break;
			case ID_SELECTION_VOLUME://
			case ID_AC_DLG_VOL:
				if (tra >= 0)
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DLGSELVOL), hwnd, DialogSelVol);
				//DialogBox(hInst,"DLGVOLUME",hwnd,DialogVolume);
				break;
			case ID_AC_STPLAY:
				StartPlayingSong();
				break;
			//case IDM_DLGWAVE://Show settings dialog
			//case ID_AC_WAVESELECT:
				//DialogBox(hInst,"DLGWAVE",hwnd,DialogWave);
				//break;
			case IDM_DLGSWAP:
			case ID_AC_DLG_SWAP:
				DialogBox(hInst,"DLGSWAP",hwnd,DialogSwap);
				break;
			
			case IDM_2BAI:
				SetUndo();
				org_data.EnlargeAllNotes(2);
				scr_data.SetHorzScroll(0);org_data.SetPlayPointer(0);SetFocus(hWnd);//頭出し
				//org_data.PutMusic();
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
				UpdateStatusBar(false);
				break;
			case IDM_3BAI:
				SetUndo();
				org_data.EnlargeAllNotes(3);
				scr_data.SetHorzScroll(0);org_data.SetPlayPointer(0);SetFocus(hWnd);//頭出し
				//org_data.PutMusic();
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
				UpdateStatusBar(false);
				break;
			case IDM_2BUNNO1:
				SetUndo();
				org_data.ShortenAllNotes(2);
				scr_data.SetHorzScroll(0);org_data.SetPlayPointer(0);SetFocus(hWnd);//頭出し				
				//org_data.PutMusic();
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
				UpdateStatusBar(false);
				break;
			case IDM_3BUNNO1:
				SetUndo();
				org_data.ShortenAllNotes(3);
				scr_data.SetHorzScroll(0);org_data.SetPlayPointer(0);SetFocus(hWnd);//頭出し				
				//org_data.PutMusic();
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
				UpdateStatusBar(false);
				break;
			case IDM_CT_L1: //Linear debility IDM_CT_L1 to 9 must be consecutive numbers!
			case IDM_CT_L2: //Convex weakness
			case IDM_CT_L3: //Convex weakness 2
			case IDM_CT_L4: //Normalize
			case IDM_CT_L5: //Gradation
			case IDM_CT_L6: //vibrato
			case IDM_CT_L7: //
			case IDM_CT_L8: //
			case IDM_CT_L9: //
			case IDM_CT_L10:
			case IDM_CT_L11:
			case IDM_CT_L12:
			case IDM_CT_L13:
			case IDM_CT_L14:
			case IDM_CT_L15:
			case IDM_CT_L16:
			case IDM_CT_L17:
			case IDM_CT_L18:
			case IDM_CT_L19:
				SetUndo();
				VolumeDecayEdit(1, -4, LOWORD(wParam) - IDM_CT_L1 + 1); //If the first argument is 0, it will be returned empty, so
				break;
			case IDM_CT_S0: 
			case IDM_CT_S1: 
			case IDM_CT_S2: 
			case IDM_CT_S3: 
			case IDM_CT_S4: //filling in holes
			case IDM_CT_S5: 
			case IDM_CT_S6: 
			case IDM_CT_S7: 
			case IDM_CT_S8: 
			case IDM_CT_S9: 
			case IDM_CT_S10: 
			case IDM_CT_S11: 
			case IDM_CT_S12: 
			case IDM_CT_S13: 
			case IDM_CT_S14: 
			case IDM_CT_S15: 
			case IDM_CT_S16: 
			case IDM_CT_S17:
			case IDM_CT_S18:
			case IDM_CT_S19: 
			case IDM_CT_S20: //Clear
				SetUndo();
				VolumeDecayEdit(1, -4, LOWORD(wParam) - IDM_CT_S1 + 1 + 20);
				break;
			case ID_AC_C0://Clear
				SetUndo();
				VolumeDecayEdit(1, -4, IDM_CT_S20     - IDM_CT_S1 + 1 + 20); 
				break;
			case ID_AC_C1: //Ctrl+1
			case ID_AC_C2:
			case ID_AC_C3:
			case ID_AC_C4:
			case ID_AC_C5:
			case ID_AC_C6:
			case ID_AC_C7:
			case ID_AC_C8:
			case ID_AC_C9:
				SetUndo();
				VolumeDecayEdit(1, -4, LOWORD(wParam) - ID_AC_C1 + 1 + 20);
				break;
			case ID_AC_CA0:
			case ID_AC_CA1:
			case ID_AC_CA2:
			case ID_AC_CA3:
				SetUndo();
				VolumeDecayEdit(1, -4, LOWORD(wParam) - ID_AC_CA0 + 1 + 20 + 9);
				break;
			case ID_AC_PRESS_LEFT: //instead of left click
				ClickProcL(wParam, lParam);
				break;
			case ID_AC_PRESS_RIGHT: //instead of right click
				ClickProcR(wParam, lParam);
				break;
			case ID_AC_CT_KEY_OCT_DOWN: //octave down
			case IDM_CT_OCT_DOWN:
				SetUndo();
				TransportNote(-12 , -4 );
				break;
			case ID_AC_CT_KEY_OCT_UP:
			case IDM_CT_OCT_UP:
				SetUndo();
				TransportNote(12 , -4 );
				break;
			case ID_AC_CT_PAN_R:
			case IDM_CT_PAN_R:
				SetUndo();
				PanEdit( 1,-4 );
				break;
			case ID_AC_CT_PAN_L:
			case IDM_CT_PAN_L:
				SetUndo();
				PanEdit( -1,-4 );
				break;
			case IDM_CT_PAN_REVERSE: //PAN reversal
			case ID_AC_PAN_REVERSE:
				SetUndo();
				PanEdit( 254, -4 );
				break;
			case ID_AC_CT_KEY_UP:
			case IDM_CT_TRANS_UP:
				SetUndo();
				TransportNote(1 , -4 );
				break;
			case ID_AC_CT_KEY_DOWN:
			case IDM_CT_TRANS_DOWN:
				SetUndo();
				TransportNote(-1 , -4 );
				break;
			case IDM_CT_VOL_PLUS:
			case ID_AC_CT_VOL_UP:
				SetUndo();
				VolumeEdit(5,-4);
				break;
			case IDM_CT_VOL_MINUS:
			case ID_AC_CT_VOL_DOWN:
				SetUndo();
				VolumeEdit(-5,-4);
				break;
			case IDM_CT_VOLWARIAI_UP:
			case ID_AC_VOLWARIAI_UP:
				SetUndo();
				VolumeWariaiEdit(144,-4);
				break;
			case IDM_CT_VOLWARIAI_DOWN:
			case ID_AC_VOLWARIAI_DOWN:
				SetUndo();
				VolumeWariaiEdit(112,-4);
				break;
			case IDM_ML_PAN_R:
				SetUndo();
				PanEdit( 1,-1 );
				break;
			case IDM_ML_PAN_L:
				SetUndo();
				PanEdit( -1,-1 );
				break;
			case IDM_ML_TRANS_UP:
			case ID_AC_ML_KEY_UP:
				SetUndo();
				TransportNote(1 , -1 );
				break;
			case IDM_ML_TRANS_DOWN:
			case ID_AC_ML_KEY_DOWN:
				SetUndo();
				TransportNote(-1 , -1 );
				break;
			case IDM_ML_VOL_PLUS:
			case ID_AC_ML_VOL_UP:
				SetUndo();
				VolumeEdit(5,-1);
				break;
			case IDM_ML_VOL_MINUS:
			case ID_AC_ML_VOL_DOWN:
				SetUndo();
				VolumeEdit(-5,-1);
				break;
			case IDM_DR_VOL_PLUS:
			case ID_AC_DR_VOL_UP:
				SetUndo();
				VolumeEdit(5,-2);
				break;
			case IDM_DR_VOL_MINUS:
			case ID_AC_DR_VOL_DOWN:
				SetUndo();
				VolumeEdit(-5,-2);
				break;
			case IDM_UNDO:
			case ID_AC_UNDO:
				ReplaseUndo();
				//org_data.PutMusic();//View sheet music
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
				UpdateStatusBar(false);
				break;
			case IDM_REDO:
			case ID_AC_REDO:
				ReplaceRedo();
				//org_data.PutMusic();//View sheet music
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
				UpdateStatusBar(false);
				break;
			case IDC_LEFT:
			case ID_AC_MEASBACK:
				scr_data.HorzScrollProc(SB_PAGELEFT, 0);
				//SendMessage(hDlgPlayer, WM_COMMAND, IDC_LEFT, NULL);
				break;
			case IDC_RIGHT:
			case ID_AC_MEASNEXT:
				scr_data.HorzScrollProc(SB_PAGERIGHT, 0);
				//SendMessage(hDlgPlayer, WM_COMMAND, IDC_RIGHT, NULL);
				break;
			case IDC_LEFTSTEP:
				scr_data.HorzScrollProc(SB_LINELEFT, 0);
				break;
			case IDC_RIGHTSTEP:
				scr_data.HorzScrollProc(SB_LINERIGHT, 0);
				break;
			case ID_SELECTION_DELETE:
			case ID_AC_SELECT_BACKDEL: //2014.04.13
				SetUndo();
				EZ_DeleteAndTrim();
				break;
			case ID_SELECTION_INSERT:
			case ID_AC_SELECT_INSERT: //2014.04.13
				SetUndo();
				EZ_Insert();
				break;
			case ID_SELECTION_CLEAR:
			case ID_AC_DELETEKEY: //Add 2014/04/12
				SetUndo();
				EZ_Delete();
				break;
			case ID_AC_CUT:
			case IDM_SELECT_CUT:
			case ID_AC_COPY:
			case IDM_SELECT_COPY: {
				if (tra >= 0) {
					for (int i = 0; i < MAXTRACK; ++i) {
						if (gClipboardData.data[i].data) {
							free(gClipboardData.data[i].data);
							gClipboardData.data[i].data = NULL;
						}
					}
					long t1;
					long t2;
					if(ful==1){
						if(sACrnt==0){
							t1=0;
							t2=MAXTRACK - 1;
						}else{
							if(org_data.track<MAXMELODY){
								t1=0;
								t2=MAXMELODY - 1;
							}else{
								t1=MAXMELODY;
								t2=MAXTRACK - 1;
							}
						}
					}else{
						t1 = t2 = tra;
					}
					org_data.GrabNoteData(&gClipboardData, t1, t2, nc_Select.x1_1, nc_Select.x1_2);
					UpdateToolbarStatus();

					if (LOWORD(wParam) == IDM_SELECT_CUT || LOWORD(wParam) == ID_AC_CUT) {
						SetUndo();
						for (int i = t1; i <= t2; ++i) {
							PARCHANGE pc;
							pc.track = i;
							pc.x1 = nc_Select.x1_1;
							pc.x2 = nc_Select.x1_2;
							org_data.DelateNoteData(&pc);
						}
					}

					SelectReset();
				}
				break;
			}
			case ID_AC_NUM7:
			case ID_AC_PASTE:
			case IDM_SELECT_PASTE:
				ClipboardPaste(1, PF_PASTE_ALL);
				break;
			case ID_AC_NUM8:
				ClipboardPaste(2, PF_PASTE_ALL);
				break;
			case ID_AC_NUM9:
				ClipboardPaste(3, PF_PASTE_ALL);
				break;
			case ID_AC_NUMPLUS:
				ClipboardPaste(4, PF_PASTE_ALL);
				break;
			case ID_AC_ADVPASTE:
			case IDM_SELECT_ADVPASTE:
				if (gClipboardData.track1 == -1) {
					break;
				}

				DialogBox(hInst, MAKEINTRESOURCE(IDD_DLGADVPASTE), hWnd, DialogAdvPaste);
				break;
			}
		}else{
			//only while playing
			//nothing especially
		}
		//Possible even while playing
		switch(LOWORD(wParam)){
		case IDM_LOUPE_MINUS:
		case ID_AC_LOUPE_MINUS: // i hate this whole codebase so much i want to redo everything
			NoteWidth -= 2;
			if (NoteWidth < 4) NoteWidth = 4;
			org_data.PutBackGround();
			org_data.GetMusicInfo(&mi);
			scr_data.ChangeHorizontalRange(mi.end_x);
			//org_data.PutMusic();//View sheet music
			//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			UpdateToolbarStatus();
			UpdateStatusBar(false);
			break;
		case IDM_LOUPE_PLUS:
		case ID_AC_LOUPE_PLUS:
			NoteWidth += 2;
			if (NoteWidth > 16) NoteWidth = 16;
			org_data.PutBackGround();
			org_data.GetMusicInfo(&mi);
			scr_data.ChangeHorizontalRange(mi.end_x);
			//org_data.PutMusic();//View sheet music
			//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			UpdateToolbarStatus();
			UpdateStatusBar(false);
			break;
		case IDM_PLAYPAUSE:
			if (timer_sw) {
				StopPlayingSong();
			} else {
				StartPlayingSong();
			}
			break;
		case IDM_PREFERENCES:
			// TODO
			break;
		case IDM_DLGSETTING://Show settings dialog
		case ID_AC_SETTEMPO:
			StopPlayingSong();
			Rxo_StopAllSoundNow();
			OpenSongProperties(hWnd);
			//DialogBox(hInst,"DLGSETTING",hwnd,DialogSetting);
			break;
		case IDC_START:
		case ID_AC_HOMEBACK: //home
			StopPlayingSong();
			scr_data.SetHorzScroll(0);
			org_data.SetPlayPointer(0);
			//SendMessage(hDlgPlayer, WM_COMMAND, IDC_START, NULL);
			break;
		case IDC_END:
		case ID_AC_TOEND: //home
			StopPlayingSong();
			org_data.GetMusicInfo(&mi);
			scr_data.SetHorzScroll(mi.end_x);
			org_data.SetPlayPointer(mi.end_x);
			//SendMessage(hDlgPlayer, WM_COMMAND, IDC_END, NULL);
			break;
		case IDM_INIT:
		case ID_AC_INIT:
			StopPlayingSong();
			//if(MessageBox(hwnd,"Any unsaved content will be discarded. Initialize?","Initialization confirmation",MB_OKCANCEL)==IDCANCEL)break; //2010.09.25 A
			if (CancelDeleteCurrentData(CDCD_INIT))break;
			ClearUndo();
			memset(music_file, 0, MAX_PATH);
			strcpy(music_file, MessageString[IDS_DEFAULT_ORG_FILENAME]);
			//for(i = 0; i < 12; i++){
			//	music_file[i] = name[i];
			//}
			org_data.InitOrgData();
			org_data.SetPlayPointer(0);
			scr_data.SetHorzScroll(0);
			//reflected in the player
			UpdateStatusBar(false);
			//SetDlgItemText(hDlgPlayer, IDE_VIEWWAIT, "128");
			//(hDlgPlayer, IDE_VIEWMEAS, "0");
			//SetDlgItemText(hDlgPlayer, IDE_VIEWXPOS, "0");
			SetModified(false);
			gFileUnsaved = true;
			//MessageBox(hwnd,"initialized","Message",MB_OK);
			SelectReset();
			//org_data.PutMusic();
			//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			for (i = 0;i < 8;i++)ucMIDIProgramChangeValue[i] = 255;

			break;
		case IDM_EXIT:
			//if(iChangeFinish!=0){	// A 2010.09.22
			//	char cc[512],*chn;
			//	GetWindowText(hWnd,cc,512);
			//	chn = strstr(cc, HENKOU_NO_SHIRUSHI);
			//	if(chn!=NULL){
			//		//Confirm the end when there is a change. // A 2010.09.22
			//		if(MessageBox(hwnd,"Any unsaved content will be discarded. Are you sure you want to quit?","End confirmation",MB_OKCANCEL | MB_ICONASTERISK)==IDCANCEL)break;
			//	}
			//}
			if (CancelDeleteCurrentData(CDCD_EXIT))break;
			SaveIniFile();
			QuitMMTimer();
			PostQuitMessage(0);
			EndDirectSound();
			org_data.ReleaseNote();
			EndGDI();
			//if (!hDlgPlayer)DestroyWindow(hDlgPlayer);
			//if (!hDlgTrack)DestroyWindow(hDlgTrack);
			//if (hDlgEZCopy) DestroyWindow(hDlgEZCopy);
			if (hDlgHelp) DestroyWindow(hDlgHelp);

			if (hwnd) DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		case IDM_DLGUSED://
			DialogBox(hInst, "DLGUSED", hwnd, DialogNoteUsed);
			break;
		case IDM_DLGMEMO://
			//PlaySoundObject(3, 1);
			DialogBox(hInst, "DLGMEMO", hwnd, DialogMemo);
			break;
		case IDM_DLGTHEMES:
			DialogBox(hInst, "DLGTHEMES", hwnd, DialogTheme);
			break;
		case IDM_DLGWAVEDBS:
			StopPlayingSong();
			DialogBox(hInst, "DLGWAVEDBS", hwnd, DialogWaveDB);
			break;
		case ID_AC_STBACK:
			StopPlayingSong();
			scr_data.SetHorzScroll(0);
			org_data.SetPlayPointer(0);
			break;
		case IDM_DLGHELP://
		case ID_AC_HELP:
			//LoadFromResource(IDR_HELPHTML);
			if (!hDlgHelp) {
				hDlgHelp = CreateDialog(hInst, "DLGHELP", hwnd, DialogHelp);
			}
			ShowWindow(hDlgHelp, SW_SHOWNOACTIVATE);
			//DialogBox(hInst,"DLGHELP",hwnd,DialogHelp);
			break;
		case IDM_SAVEOVER:
		case ID_AC_MENUOVERSAVE:
			if (gFileModified)
				OpenDoSave(hwnd, false);
			break;
		case IDM_SAVENEW://Save As
		case ID_AC_MENUNEWSAVE:
			OpenDoSave(hwnd, true);
			/*res = GetFileNameSave(hwnd,MessageString[IDS_STRING62]); //"Save As"
			if(res == MSGCANCEL)break;
			if(res == MSGEXISFILE){
				//if(MessageBox(hwnd,"Do you want to overwrite?","There is a file with the same name",MB_YESNO | MB_ICONEXCLAMATION)	// 2014.10.19 D
				if(msgbox(hwnd,IDS_NOTIFY_OVERWRITE,IDS_INFO_SAME_FILE,MB_YESNO | MB_ICONEXCLAMATION)	// 2014.10.19 A
					==IDNO)break;
			}
			org_data.SaveMusicData();
			SetModified(false);//title name set
			gFileUnsaved = false;*/
			break;
		case IDM_EXPORT_MIDI: //Export 2014.05.11
		case ID_AC_MIDI:
			StopPlayingSong();

			res = GetFileNameMIDI(hwnd, MessageString[IDS_STRING63], strMIDIFile);//"Export in standard MIDI format"
			if (res == MSGCANCEL)break;
			if (res == MSGEXISFILE) {
				//if(MessageBox(hwnd,"Do you want to overwrite?","There is a file with the same name",MB_YESNO | MB_ICONEXCLAMATION)	// 2014.10.19 D
				if (msgbox(hwnd, IDS_NOTIFY_OVERWRITE, IDS_INFO_SAME_FILE, MB_YESNO | MB_ICONEXCLAMATION)	// 2014.10.19 A
					== IDNO)break;
			}
			org_data.ExportMIDIData(strMIDIFile, iDlgRepeat);
			//SetTitlebarText();//title name set
			//ResetTitlebarChange();
			break;
		case IDM_EXPORT_WAV:
		case ID_AC_WAV:
			StopPlayingSong();
			DialogBox(hInst, "DLGWAVEXPORT", hwnd, DialogWavExport);
			break;
		case ID_AC_LOAD_MOST_RECENT:
			StopPlayingSong();
			SendMessage(hWnd, WM_COMMAND, Menu_Recent[0], 0);
			break;
		case IDM_DCLEN:
			DialogBox(hInst, "DLGDCLEN", hwnd, DialogDecayLength);
			break;
		case IDM_DUMMY_TATE_SEPARATOR: //do nothing
			break;
		case IDM_LOAD:
		case ID_AC_MENUOPEN:
			StopPlayingSong();
			//case IDM_LOAD2:
			//case ID_AC_LOAD2:
			if (CancelDeleteCurrentData(CDCD_LOAD)) break;
			/*i = 0;
			if(LOWORD(wParam)==IDM_LOAD2 || LOWORD(wParam)==ID_AC_LOAD2)i=1; */
			if (GetFileNameLoad(hWnd, MessageString[IDS_STRING61]/*,i*/) != MSGLOADOK) break;//"Load song data"

			if (timer_sw != 0) // Stop playing song
				StopPlayingSong();

			ClearUndo();
			org_data.InitOrgData();
			org_data.LoadMusicData();
			SetModified(false);//title name set
			gFileUnsaved = false;
			//DetectPreciseMode();

			//Show to Player
			UpdateStatusBar(false);

			SelectReset();
			//org_data.PutMusic();
			//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

			for (i = 0;i < 8;i++)ucMIDIProgramChangeValue[i] = 255;
			break;
		case IDM_SELECT_ALL:
		case ID_AC_SELECT_ALL:
			SelectAll(0);
			break;
		case IDM_SELECT_ALL_ALL:
			SelectAll(1);
			break;
		case IDM_SELECT_RESET:
		case ID_AC_SELECT_RESET:
		case ID_AC_SELECT_CLEAR:
			SelectReset();
			break;
		case ID_AC_SOCTUP:
			scr_data.VertScrollProc(SB_PAGEUP, 0);
			break;
		case ID_AC_SOCTDOWN:
			scr_data.VertScrollProc(SB_PAGEDOWN, 0);
			break;
		case IDM_RECENT_CLEAR:
			ClearRecentFile();
			break;
		case IDM_GRIDMODE:
		case ID_AC_GRIDMODE:
			ChangeGridMode();
			break;
		/*case IDM_AUTOLOADPVI:
			ChangeAutoLoadMode();
			break;*/
		case IDM_METRONOME:
			ChangeMetronomeMode();
			break;
		case IDM_SMOOTHSCROLL:
			ChangeScrollMode();
			break;
		case IDM_ALWAYS_CURRENT:
		case ID_AC_ALWAYS_CURRENT:
			ChangeSelAlwaysCurrent();
			//org_data.PutMusic();//View sheet music
			//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			break;
		case ID_AC_DRAWDOUBLE:
		case IDM_DRAWDOUBLE:
			ChangeDrawDouble();
			//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			break;
		case IDM_NOTE_ENLARGE: //&Make note heads stand out when zoomed out 2014.05.28
			ChangeNoteEnlarge();
			//org_data.PutMusic();//View sheet music
			//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			break;
		case IDM_ENABLEPLAYING: //You can hit the keyboard while playing 2010.09.23 A
			ChangeEnablePlaying();
			break;
		case IDM_CHANGEFINISH: //Confirm when finished 2010.09.23 A
			ChangeFinish();
			break;
		case IDM_STOPNOWALL:
		case ID_AC_KILLSOUND:
			StopPlayingSong();
			Rxo_StopAllSoundNow();
			memset(iKeyPhase, -1, sizeof(iKeyPhase));
			memset(iKeyPushDown, 0, sizeof(iKeyPushDown));
			break;
		case ID_AC_DRAGMODE:
		case IDM_DRAGMODE:
			ChangeDragMode();
			break;
		case IDM_SLIDEOVERLAPNOTES: //2014.05.06 A
		case ID_AC_SLIDEOVERLAPNOTES:
			ChangeSlideOverlapNoteMode();
			//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			break;
		case IDM_PRESSNOTE:
		case ID_AC_PRESSNOTE:
			ChangePushStratchNOTE();
			break;
			//↓	// 2010.12.01 A
		case ID_AC_NUM1:
		case ID_AC_NUM2:
		case ID_AC_NUM3:
		case ID_AC_NUM4:
		case ID_AC_NUM5:
		case ID_AC_NUM6:
		case ID_AC_C_NUM1:
		case ID_AC_C_NUM2:
		case ID_AC_C_NUM3:
		case ID_AC_C_NUM4:
		case ID_AC_C_NUM5:
		case ID_AC_C_NUM6: {
			int bt = -1;
			long scr_h, scr_v;
			for (i = 0;i < 12;i++) {
				if (LOWORD(wParam) == Bn[i])bt = i;
			}
			if (bt >= 0) { //＋１とかのボタンが押された。
				int a, b, d, bb;
				//char str[5];
				org_data.GetMusicInfo(&mi);
				scr_data.GetScrollPosition(&scr_h, &scr_v);
				//GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,str,4);//範囲from
				//b = atol(str);
				b = scr_h / (mi.dot * mi.line);
				bb = b + Hni[bt];
				haba = Hni[bt];
				a = b * mi.dot * mi.line; //開始点
				d = bb * mi.dot * mi.line - 1; //終了点
				nc_Select.x1_1 = a;
				nc_Select.x1_2 = d;
				if (bt >= 6)ful = 1;else ful = 0; //全トラック？
				tra = org_data.track;
				//org_data.CopyNoteDataToCB(&nc_Select, tra, ful);
				//if(ful==0)wsprintf(CpHelp,"トラック%c の%d小節～%d小節までを",TrackN[tra],b,bb);	// 2014.10.19 D
				//if (ful == 0)wsprintf(CpHelp, MessageString[IDS_STRING74], TrackN[tra], b, bb);	// 2014.10.19 A
				//else wsprintf(CpHelp,"全トラックの%d小節～%d小節までを",b,bb);	// 2014.10.19 D
				//else wsprintf(CpHelp, MessageString[IDS_STRING75], b, bb);	// 2014.10.19 A
				//org_data.PutMusic();
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			}
			break;
		}
		case ID_AC_DEFAULT:
		case IDM_DLGDEFAULT://Show Default Dialog
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DLGDEFAULT), hwnd, DialogDefaults);
			//DialogBox(hInst, "DLGDEFAULT", hwnd, DialogDefault);
			break;
		case IDM_GITHUB:
			ShellExecute(NULL, "open", "https://github.com/Strultz/orgmaker-3", NULL, NULL, SW_SHOWNORMAL);
			break;
		case IDM_CHECKUPD: {
			std::thread t(CheckUpdate, true);
			t.detach();
			break;
		}
		case IDM_AUTOCHECKUPDATES:
			hMenu = GetMenu(hWnd);
			autoCheckUpdate = !autoCheckUpdate;
			CheckMenuItem(hMenu, IDM_AUTOCHECKUPDATES, MF_BYCOMMAND | (autoCheckUpdate ? MFS_CHECKED : MFS_UNCHECKED));
			break;
		case IDM_FLOATTOOLBARS:
			hMenu = GetMenu(hWnd);
			floatingToolbars = !floatingToolbars;
			CheckMenuItem(hMenu, IDM_FLOATTOOLBARS, MF_BYCOMMAND | (floatingToolbars ? MFS_CHECKED : MFS_UNCHECKED));
			UpdateToolbars(floatingToolbars);
			break;
		case IDM_PLAY_NOTES_MID:
			hMenu = GetMenu(hWnd);
			gPlayMidNote = !gPlayMidNote;
			CheckMenuItem(hMenu, IDM_PLAY_NOTES_MID, MF_BYCOMMAND | (gPlayMidNote ? MFS_CHECKED : MFS_UNCHECKED));
			break;
		}

		break;
	case WM_ACTIVATEAPP:
		actApp = wParam;
		break;
	case WM_ENTERMENULOOP:
		SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"");
		break;
	case WM_EXITMENULOOP:
		SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"For help, press F1");
		break;
	case WM_MENUSELECT:
		switch(LOWORD(wParam)){
		default: SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)""); break;
		case IDM_INIT:              SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Create a new document"); break;
		case IDM_LOAD:              SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Open an existing document"); break;
		case IDM_RECENT1:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[0]); break;
		case IDM_RECENT2:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[1]); break;
		case IDM_RECENT3:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[2]); break;
		case IDM_RECENT4:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[3]); break;
		case IDM_RECENT5:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[4]); break;
		case IDM_RECENT6:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[5]); break;
		case IDM_RECENT7:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[6]); break;
		case IDM_RECENT8:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[7]); break;
		case IDM_RECENT9:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[8]); break;
		case IDM_RECENT0:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)RecentFileName[9]); break;
		case IDM_RECENT_CLEAR:      SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Clear the recent file list"); break;
		case IDM_SAVEOVER:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Save the active document"); break;
		case IDM_SAVENEW:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Save the active document with a new name"); break;
		case IDM_EXPORT_MIDI:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Export the active document to a MIDI file"); break;
		case IDM_EXPORT_WAV:        SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Export the active document to a WAV file"); break;
		case IDM_EXIT:              SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Exit OrgMaker"); break;

		case IDM_UNDO:              SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Undo the last action"); break;
		case IDM_REDO:              SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Redo the previously undone action"); break;
		case IDM_SELECT_CUT:        SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Cut the selection and put it on the Clipboard"); break;
		case IDM_SELECT_COPY:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Copy the selection and put it on the Clipboard"); break;
		case IDM_SELECT_PASTE:      SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Insert Clipboard contents"); break;
		case IDM_SELECT_ADVPASTE:   SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"TODO"); break;
		case IDM_SELECT_ALL:        SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select everything on the current channel"); break;
		case IDM_SELECT_RESET:      SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Clear the current selection"); break;
		case IDM_SELECT_ALL_ALL:    SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select the entire document"); break;
		case ID_AC_NUM1:            SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 1 measure on the current channel"); break;
		case ID_AC_NUM2:            SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 2 measures on the current channel"); break;
		case ID_AC_NUM3:            SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 3 measures on the current channel"); break;
		case ID_AC_NUM4:            SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 4 measures on the current channel"); break;
		case ID_AC_NUM5:            SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 8 measures on the current channel"); break;
		case ID_AC_NUM6:            SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 16 measures on the current channel"); break;
		case ID_AC_C_NUM1:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 1 measure on all channels"); break;
		case ID_AC_C_NUM2:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 2 measures on all channels"); break;
		case ID_AC_C_NUM3:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 3 measures on all channels"); break;
		case ID_AC_C_NUM4:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 4 measures on all channels"); break;
		case ID_AC_C_NUM5:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 8 measures on all channels"); break;
		case ID_AC_C_NUM6:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select 16 measures on all channels"); break;
		case ID_SELECTION_VOLUME:   SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Modify volume within the selection"); break;
		case ID_SELECTION_PANNING:  SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Modify panning within the selection"); break;
		case ID_SELECTION_TRANSPOSE:SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Transpose notes within the selection"); break;
		case ID_SELECTION_INSERT:   SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Insert empty space in the selection"); break;
		case ID_SELECTION_CLEAR:    SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Remove notes in the selection"); break;
		case ID_SELECTION_DELETE:   SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Remove and trim the selection"); break;
		case IDM_DLGSWAP:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Swap the data of two channels"); break;
		case IDM_CT_S20:            SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Clear all volume change events in the selection/channel"); break;
		case IDM_DCLEN:             SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Edit note decay length and settings"); break;
		case IDM_CT_PAN_R:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Pan the selection/channel to the right"); break;
		case IDM_CT_PAN_L:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Pan the selection/channel to the left"); break;
		case IDM_CT_PAN_REVERSE:    SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Invert the panning of selection/channel"); break;
		case IDM_CT_VOL_PLUS:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Increase the volume of the selection/channel"); break;
		case IDM_CT_VOL_MINUS:      SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Decrease the volume of the selection/channel"); break;
		case IDM_CT_VOLWARIAI_UP:   SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Increase the volume of the selection/channel by 12.5%"); break;
		case IDM_CT_VOLWARIAI_DOWN: SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Decrease the volume of the selection/channel by 12.5%"); break;
		case IDM_CT_TRANS_UP:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Transpose the selection/channel up by one semitone"); break;
		case IDM_CT_TRANS_DOWN:     SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Transpose the selection/channel down by one semitone"); break;
		case IDM_CT_OCT_UP:         SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Transpose the selection/channel up by one octave"); break;
		case IDM_CT_OCT_DOWN:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Transpose the selection/channel down by one octave"); break;
		case IDM_ML_PAN_R:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Pan all melody channels to the right"); break;
		case IDM_ML_PAN_L:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Pan all melody channels to the left"); break;
		case IDM_ML_VOL_PLUS:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Increase the volume of all melody channels"); break;
		case IDM_ML_VOL_MINUS:      SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Decrease the volume of all melody channels"); break;
		case IDM_ML_TRANS_UP:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Transpose all melody channels up by one semitone"); break;
		case IDM_ML_TRANS_DOWN:     SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Transpose all melody channels down by one semitone"); break;
		case IDM_DR_VOL_PLUS:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Increase the volume of all percussion channels"); break;
		case IDM_DR_VOL_MINUS:      SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Decrease the volume of all percussion channels"); break;
		case IDM_2BAI:              SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Expand song by 2x"); break;
		case IDM_3BAI:              SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Expand song by 3x"); break;
		case IDM_2BUNNO1:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Shrink song by 1/2x"); break;
		case IDM_3BUNNO1:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Shrink song by 1/3x"); break;
		case IDM_GRIDMODE:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Snap the selection to the beat grid"); break;
		case IDM_ALWAYS_CURRENT:    SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Always select the current channel"); break;
		case IDM_DRAGMODE:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Extend notes when dragging from their head"); break;
		case IDM_PRESSNOTE:         SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Extend notes when clicking on their head"); break;
		case IDM_STOPNOWALL:        SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Stop all playing sounds"); break;

		case IDM_DLGSETTING:        SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Configure the active document"); break;
		case IDM_DLGUSED:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"View how many events this song uses"); break;

		case IDM_LOUPE_PLUS:        SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Zoom the piano roll in"); break;
		case IDM_LOUPE_MINUS:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Zoom the piano roll out"); break;
		//case IDM_PREFERENCES:       SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Open OrgMaker preferences"); break;
		case IDM_CHANGEFINISH:      SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Confirm unsaved changes on exit"); break;
		case IDM_FLOATTOOLBARS:     SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Pop out toolbars into their own window"); break;
		case IDM_AUTOCHECKUPDATES:  SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Automatically check for new versions on launch"); break;
		case IDM_ENABLEPLAYING:     SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Allow keyboard playback while the song is playing"); break;
		case IDM_SMOOTHSCROLL:      SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Scroll by steps while the song is playing"); break;
		case IDM_NOTE_ENLARGE:      SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Enlarge note heads while zoomed out"); break;
		case IDM_SLIDEOVERLAPNOTES: SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Shift overlapping notes on other channels"); break;
		case IDM_DRAWDOUBLE:        SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Show ghost notes from melody or percussion channels"); break;
		case IDM_DLGDEFAULT:        SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Modify default volume/panning values"); break;
		case IDM_METRONOME:         SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Plays a sound on every beat/measure"); break;
		case IDM_DLGTHEMES:         SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select a custom visual style"); break;
		case IDM_DLGWAVEDBS:        SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Select a custom sound bank"); break;

		case IDM_DLGHELP:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Open the Help menu"); break;
		case IDM_GITHUB:            SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Open the GitHub page for OrgMaker 3"); break;
		case IDM_CHECKUPD:          SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"Check for a new version of OrgMaker"); break;
		case IDM_DLGMEMO:           SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)"About OrgMaker"); break;
		}
		break;
	case WM_DROPFILES://file drop
		//SetWindowPos(hWnd, HWND_TOP, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE );
		//SendMessage(hWnd, WM_PAINT, 0, 0);
		//SetFocus(hWnd);
		SetForegroundWindow(hwnd);
		if(CancelDeleteCurrentData(CDCD_LOAD))break;
		//DragQueryFile((HDROP)wParam,0,music_file,MAX_PATH);	// 2014.05.22 D
		DragQueryFile((HDROP)wParam,0,strMIDIFile,MAX_PATH);	// 2014.05.22 A
		if(org_data.FileCheckBeforeLoad(strMIDIFile)){
			msgbox(hwnd, IDS_STRING64, IDS_ERROR_LOAD, MB_OK | MB_ICONWARNING);
			//SetDlgItemText(hDlgEZCopy, IDC_MESSAGE, MessageString[IDS_STRING64]); // The file cannot be opened or is in an invalid format.
			break;
		}
		strcpy(music_file, strMIDIFile);

		if (timer_sw != 0) // Stop playing song
			StopPlayingSong();

		ClearUndo();
//		MessageBox(hWnd,music_file,"",MB_OK);
		org_data.InitOrgData();
		org_data.LoadMusicData();
		//org_data.PutMusic();//View sheet music
		//DetectPreciseMode();
		//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		//Show to Player
		UpdateStatusBar(false);
		SetModified(false);//title name set
        gFileUnsaved = false;

		break;
	case WM_CLOSE:
		//if(iChangeFinish!=0){	// A 2010.09.22
		//	char cc[512],*chn;
		//	GetWindowText(hWnd,cc,512);
		//	chn = strstr(cc, HENKOU_NO_SHIRUSHI);
		//	if(chn!=NULL){
		//		//Confirm the end when there is a change.// A 2010.09.22
		//		if(MessageBox(hwnd,"Any unsaved content will be discarded. Are you sure you want to quit?","end confirmation",MB_OKCANCEL| MB_ICONASTERISK)==IDCANCEL)break;
		//	}
		//}
		if(CancelDeleteCurrentData(CDCD_EXIT))break;

		SaveIniFile();
		QuitMMTimer();
		PostQuitMessage(0);
		break;
	case WM_QUIT:
		break;
	case WM_DESTROY://for post-processing
		EndDirectSound();
		org_data.ReleaseNote();
		DeleteWaveData100(); //Added 20140401 Normally, it seems to be called in the order of WM_CLOSE ・> WM_DESTROY ・> WM_QUIT.
		EndGDI();
		//if(!hDlgPlayer)DestroyWindow(hDlgPlayer);
		//if(!hDlgTrack)DestroyWindow(hDlgTrack);
		//if(hDlgEZCopy) DestroyWindow(hDlgEZCopy);
		if(hwnd) DestroyWindow(hwnd);
		PostQuitMessage(0);
		FreeMessageStringBuffer();	// 2014.10.19 
		break;
	case WM_KEYDOWN://keyboard pressed
		switch(wParam){
		case VK_PRIOR:
			scr_data.VertScrollProc(SB_PAGEUP, 0);
			break;
		case VK_NEXT:
			scr_data.VertScrollProc(SB_PAGEDOWN, 0);
			break;
		case VK_UP:
			scr_data.KeyScroll(DIRECTION_UP);
			break;
		case VK_DOWN:
			scr_data.KeyScroll(DIRECTION_DOWN);
			break;
		case VK_LEFT:
			if (timer_sw == 0) scr_data.KeyScroll(DIRECTION_LEFT);
			break;
		case VK_RIGHT:
			if (timer_sw == 0) scr_data.KeyScroll(DIRECTION_RIGHT);
			break;
		case VK_F5:
		//case VK_NUMPAD0:
			if (timer_sw == 0) StartPlayingSong();
			else StopPlayingSong();
			break;
		//case VK_HOME:
		//	SendMessage(hDlgPlayer , WM_COMMAND , IDC_START , NULL);
		//	break;
		case 'Z':
		case 'S':
		case 'X':
		case 'D':
		case 'C':
		case 'V':
		case 'G':
		case 'B':
		case 'H':
		case 'N':
		case 'J':
		case 'M':
		case VK_OEM_COMMA:
		case 'L':
		case VK_OEM_PERIOD:
		case VK_OEM_1:
		case VK_OEM_2:

			if((lParam & 0x40000000) ==0 && (timer_sw==0 || iChangeEnablePlaying!=0)){
				org_data.GetMusicInfo(&mi);
				iKeyPhase[iCast[wParam]]=iCurrentPhase;
				iCurrentPhase=-iCurrentPhase + 1;

				char key = iCast[wParam] + previewOctave * 12;
				if (key < 0) break;
				if (key > 95) break;

				Rxo_PlayKey(key, org_data.track, mi.tdata[org_data.track].freq, iKeyPhase[iCast[wParam]]);
				iKeyPushDown[key] = 1;
				//org_data.PutMusic();//Redrawing sheet music
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

			}

			break;
		case VK_MULTIPLY:
			if ((lParam & 0x40000000) == 0 && (timer_sw == 0 || iChangeEnablePlaying != 0)) {
				++previewOctave;
				if (previewOctave > 7)
					previewOctave = 7;
				Rxo_StopAllSoundNow();
				memset(iKeyPhase, -1, sizeof(iKeyPhase));
				memset(iKeyPushDown, 0, sizeof(iKeyPushDown));
				UpdateStatusBar(false);
			}
			break;
		case VK_DIVIDE:
			if ((lParam & 0x40000000) == 0 && (timer_sw == 0 || iChangeEnablePlaying != 0)) {
				--previewOctave;
				if (previewOctave < 0)
					previewOctave = 0;
				Rxo_StopAllSoundNow();
				memset(iKeyPhase, -1, sizeof(iKeyPhase));
				memset(iKeyPushDown, 0, sizeof(iKeyPushDown));
				UpdateStatusBar(false);
			}
			break;
		}
		break;
//				wsprintf(strSize , "U lParam = %x" , lParam);
//				RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	case WM_KEYUP: //keyboard released
		switch(wParam) {
		case 'Z':
		case 'S':
		case 'X':
		case 'D':
		case 'C':
		case 'V':
		case 'G':
		case 'B':
		case 'H':
		case 'N':
		case 'J':
		case 'M':
		case VK_OEM_COMMA:
		case 'L':
		case VK_OEM_PERIOD:
		case VK_OEM_1:
		case VK_OEM_2:
			if((timer_sw==0 || iChangeEnablePlaying!=0)){
				if (iKeyPhase[iCast[wParam]] == -1)
					break;

				char key = iCast[wParam] + previewOctave * 12;
				if (key < 0) break;
				if (key > 95) break;

				Rxo_StopKey(key, org_data.track, iKeyPhase[iCast[wParam]]);
				iKeyPushDown[key] = 0;

				iKeyPhase[iCast[wParam]] = -1;
				//org_data.PutMusic();//Redrawing sheet music
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			}
			break;
		}
		break;

    case WM_LBUTTONDOWN://mouse (left) pressed
		ClickProcL(wParam, lParam);
		break;
    case WM_RBUTTONDOWN://mouse (right) pressed
		ClickProcR(wParam, lParam);
		break;
    case WM_MBUTTONDOWN://mouse (middle) pressed
		ClickProcM(wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		MouseDrag(wParam, lParam);
		break;
    case WM_LBUTTONUP://mouse (left) released
		LButtonUP(wParam, lParam);
		break;
    case WM_RBUTTONUP://mouse (right) released
		RButtonUP(wParam, lParam);
		//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		break;
	case WM_MBUTTONUP:
		MButtonUP(wParam, lParam);
		break;
	case WM_HSCROLL:
		ZeroMemory(&si, sizeof(si));
		si.cbSize = sizeof(si);
		si.fMask = SIF_TRACKPOS;
		if (GetScrollInfo(hwndArea, SB_HORZ, &si))
			scr_data.HorzScrollProc(LOWORD(wParam), si.nTrackPos);
		break;
	case WM_VSCROLL:
		ZeroMemory(&si, sizeof(si));
		si.cbSize = sizeof(si);
		si.fMask = SIF_TRACKPOS;
		if (GetScrollInfo(hwndArea, SB_VERT, &si))
			scr_data.VertScrollProc(LOWORD(wParam), si.nTrackPos);
		break;
	case WM_MOUSEWHEEL:
		scr_data.WheelScrollProc(lParam, wParam);		
		break;
	case WM_GETMINMAXINFO:
			pmmi = (MINMAXINFO *)lParam;
			if ( pmmi )
			{
				pmmi->ptMinTrackSize.x = 420;  // Minimum margin
				pmmi->ptMinTrackSize.y = 480;  // minimum height
				//pmmi->ptMaxTrackSize.x = 1024; // maximum size
				//pmmi->ptMaxTrackSize.y = 768;  // maximum height
			}
		
		break;
	case 0x020C: //weird button press
		//MessageBox(hWnd,"Failed to read","Error(Load)",MB_OK);
		switch HIWORD(wParam){
		case 0x0001: //▼
			ChangeTrackPlus(1);
			break;
		case 0x0002: //▲
			ChangeTrackPlus(-1);
			break;
		}
		break;
	case WM_NOTIFY: {
		LPNMHDR nmh = (LPNMHDR)lParam;
		switch (nmh->code) {
		case RBN_HEIGHTCHANGE: {
			rebarHeight = GetBarHeight(hwndRebar);
			SetWindowPos(hwndArea, HWND_TOP, 0, rebarHeight, realWidth, realHeight - rebarHeight - GetBarHeight(hwndStatus), 0);

			if (!org_data.PutBackGround())break;
			scr_data.ChangeVerticalRange(WHeight);
			break;
		}
		}
		break;
	}
	case WM_SIZE: {
		SendMessage(hwndStatus, WM_SIZE, 0, 0);
		if (hwndRebar) SendMessage(hwndRebar, WM_SIZE, 0, 0);
		rebarHeight = GetBarHeight(hwndRebar);

		realWidth = LOWORD(lParam);
		realHeight = HIWORD(lParam);
		SetWindowPos(hwndArea, HWND_TOP, 0, rebarHeight, realWidth, realHeight - rebarHeight - GetBarHeight(hwndStatus), 0);

		int swidths[] = { realWidth - 578, realWidth - 466, realWidth - 388, realWidth - 320, realWidth - 150, -1 };
		SendMessage(hwndStatus, SB_SETPARTS, sizeof(swidths) / sizeof(int), (LPARAM)swidths);


		if (!org_data.PutBackGround())break;
		//org_data.PutMusic();

//				wsprintf(strSize , "Height = %d" , (WHeight - 158)/12);
//				RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		org_data.GetMusicInfo(&mi);
		scr_data.ChangeHorizontalRange(mi.end_x);
		scr_data.ChangeVerticalRange(WHeight);
		//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

		break;
	}
	case WM_SIZING: //resizing
		//org_data.PutBackGround();
		//org_data.PutMusic();
		
		//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		
		//AfterReSize = true;
		break;
	//below is for testing
/*	case WM_MOUSEMOVE:
		char str[80];
		long mouse_x,mouse_y;
//		HDC hdc;
		hdc = GetDC(hWnd);
		mouse_data.GetMousePosition(&mouse_x,&mouse_y);
		sprintf(str,"Mouse X:%4d Mouse Y:%4d  ",mouse_x,mouse_y);
		TextOut(hdc,500,16,str,strlen(str));
		ReleaseDC(hWnd,hdc);
		break;
*/
	default: return DefWindowProc(hwnd, message, wParam, lParam);//以外だったら
	}

	return FALSE;
}
//Show filename in title bar
void SetTitlebarText()
{
	char set_name[MAX_PATH + 30]; // display space in title

	char* slash = strrchr(music_file, '/');
	char* backslash = strrchr(music_file, '\\');

	char* name = music_file;
	if (slash == NULL && backslash != NULL) {
		name = &backslash[1];
	} else if (slash != NULL && backslash == NULL) {
		name = &slash[1];
	} else if (slash != NULL && backslash != NULL) {
		name = slash < backslash ? &backslash[1] : &slash[1];
	}

#ifdef _DEBUG
	snprintf(set_name, MAX_PATH + 30, "%s%s - %s [Debug]", gFileModified ? "*" : "", name, lpszName);
#else
	snprintf(set_name, MAX_PATH + 30, "%s%s - %s", gFileModified ? "*" : "", name, lpszName);
#endif

	SetWindowText(hWnd, set_name);
}

int GetCurrentMeasure() {
	long x_scroll;
	scr_data.GetScrollPosition(&x_scroll, NULL);

	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);

	if (mi.dot != 0 && mi.line != 0) {
		return x_scroll / (mi.dot * mi.line);
	}
	return 0;
}

int GetCurrentStep() {
	long x_scroll;
	scr_data.GetScrollPosition(&x_scroll, NULL);

	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);

	if (mi.dot != 0 && mi.line != 0) {
		return x_scroll % (mi.dot * mi.line);
	}
	return 0;
}

void SetModified(bool mod) {
	gFileModified = mod;
	SetTitlebarText();
	UpdateToolbarStatus();
}

void SaveIniFile()
{
	int i;
	WINDOWPLACEMENT WPM;
	WPM.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(hWnd, &WPM);
	if(WPM.showCmd == SW_SHOWMAXIMIZED)i=1;else i=0;
	ShowWindow( hWnd, SW_RESTORE );	//original size

	GetWindowRect(hWnd,(LPRECT)&WinRect);
	wsprintf(num_buf,"%d",WinRect.left);
	WritePrivateProfileString(MAIN_WINDOW,"left",num_buf,app_path);
	wsprintf(num_buf,"%d",WinRect.top);
	WritePrivateProfileString(MAIN_WINDOW,"top",num_buf,app_path);
	wsprintf(num_buf,"%d",WinRect.right - WinRect.left);
	WritePrivateProfileString(MAIN_WINDOW,"right",num_buf,app_path);
	wsprintf(num_buf,"%d",WinRect.bottom - WinRect.top);
	WritePrivateProfileString(MAIN_WINDOW,"bottom",num_buf,app_path);
	wsprintf(num_buf,"%d",i);
	WritePrivateProfileString(MAIN_WINDOW,"WindowState",num_buf,app_path);
	wsprintf(num_buf,"%d",sGrid);
	WritePrivateProfileString(MAIN_WINDOW,"GridMode",num_buf,app_path);
	wsprintf(num_buf,"%d",sACrnt);
	WritePrivateProfileString(MAIN_WINDOW,"AlwaysCurrent",num_buf,app_path);
	wsprintf(num_buf,"%d",gDrawDouble);
	WritePrivateProfileString(MAIN_WINDOW,"DrawDouble",num_buf,app_path);
	wsprintf(num_buf,"%d",iDragMode);
	WritePrivateProfileString(MAIN_WINDOW,"DragMode",num_buf,app_path);
	wsprintf(num_buf, "%d", iPushStratch);
	WritePrivateProfileString(MAIN_WINDOW,"EnablePressNoteStretch",num_buf,app_path);
	wsprintf(num_buf, "%d", iChangeEnablePlaying);
	WritePrivateProfileString(MAIN_WINDOW,"EnablePlaying",num_buf,app_path);
	wsprintf(num_buf,"%d",iChangeFinish);
	WritePrivateProfileString(MAIN_WINDOW,"QuitMessage",num_buf,app_path);
	wsprintf(num_buf,"%d",iSlideOverlapNotes);
	WritePrivateProfileString(MAIN_WINDOW,"SlideOverlapNoteMode",num_buf,app_path);
	wsprintf(num_buf,"%d",NoteWidth);
	WritePrivateProfileString(MAIN_WINDOW,"NoteWidth",num_buf,app_path);
	wsprintf(num_buf,"%d",NoteEnlarge_Until_16px);
	WritePrivateProfileString(MAIN_WINDOW,"NoteEnlarge",num_buf,app_path);
	wsprintf(num_buf,"%d",sMetronome);
	WritePrivateProfileString(MAIN_WINDOW,"Metronome",num_buf,app_path);
	wsprintf(num_buf, "%d", sSmoothScroll);
	WritePrivateProfileString(MAIN_WINDOW, "SmoothScroll", num_buf, app_path);
	wsprintf(num_buf, "%d", volChangeLength);
	WritePrivateProfileString(MAIN_WINDOW, "VolChangeLength", num_buf, app_path);
	wsprintf(num_buf, "%d", volChangeUseNoteLength);
	WritePrivateProfileString(MAIN_WINDOW, "VolChangeUseNoteLength", num_buf, app_path);
	wsprintf(num_buf, "%d", volChangeSetNoteLength);
	WritePrivateProfileString(MAIN_WINDOW, "VolChangeSetNoteLength", num_buf, app_path);
	wsprintf(num_buf, "%d", autoCheckUpdate);
	WritePrivateProfileString(MAIN_WINDOW, "AutoCheckUpdates", num_buf, app_path);
	wsprintf(num_buf, "%d", floatingToolbars);
	WritePrivateProfileString(MAIN_WINDOW, "FloatingToolbars", num_buf, app_path);
	wsprintf(num_buf, "%d", gPlayMidNote);
	WritePrivateProfileString(MAIN_WINDOW, "PlayMidNote", num_buf, app_path);

	

	WritePrivateProfileString(MAIN_WINDOW, "CurrentThemePath", gSelectedTheme, app_path);
	WritePrivateProfileString(MAIN_WINDOW, "CurrentWavePath", gSelectedWave, app_path);

	char strtmp[128];
	for (i = 0; i < MAXTRACK; ++i) {
		snprintf(strtmp, 128, "Channel%dDefaultVol", i);
		wsprintf(num_buf, "%d", org_data.def_volume[i]);
		WritePrivateProfileString(MAIN_WINDOW, strtmp, num_buf, app_path);

		snprintf(strtmp, 128, "Channel%dDefaultPan", i);
		wsprintf(num_buf, "%d", org_data.def_pan[i]);
		WritePrivateProfileString(MAIN_WINDOW, strtmp, num_buf, app_path);
	}

	/*GetWindowRect(hDlgTrack, (LPRECT)&WinRect);
	wsprintf(num_buf,"%d",WinRect.left);
	WritePrivateProfileString(TRACK_WINDOW,"left",num_buf,app_path);
	wsprintf(num_buf,"%d",WinRect.top);
	WritePrivateProfileString(TRACK_WINDOW,"top",num_buf,app_path);*/

	/*GetWindowRect(hDlgPlayer, (LPRECT)&WinRect);
	wsprintf(num_buf,"%d",WinRect.left);
	WritePrivateProfileString(PLAY_WINDOW,"left",num_buf,app_path);
	wsprintf(num_buf,"%d",WinRect.top);
	WritePrivateProfileString(PLAY_WINDOW,"top",num_buf,app_path);*/
	/*GetWindowRect(hDlgEZCopy, (LPRECT)&WinRect);
	wsprintf(num_buf,"%d",WinRect.left);
	WritePrivateProfileString(COPY_WINDOW,"left",num_buf,app_path);
	wsprintf(num_buf,"%d",WinRect.top);
	WritePrivateProfileString(COPY_WINDOW,"top",num_buf,app_path);
	wsprintf(num_buf,"%d",EZCopyWindowState);
	WritePrivateProfileString(COPY_WINDOW,"show",num_buf,app_path);*/

	if (hwndToolbarPopup) {
		GetWindowRect(hwndToolbarPopup, (LPRECT)&WinRect);
		toolbarSavedX[0] = WinRect.left;
		toolbarSavedY[0] = WinRect.top;
	}
	if (hwndTrackbarPopup) {
		GetWindowRect(hwndTrackbarPopup, (LPRECT)&WinRect);
		toolbarSavedX[1] = WinRect.left;
		toolbarSavedY[1] = WinRect.top;
	}

	wsprintf(num_buf,"%d", toolbarSavedX[0]);
	WritePrivateProfileString(TOOLBAR_POS,"ToolbarMainX",num_buf,app_path);
	wsprintf(num_buf,"%d", toolbarSavedY[0]);
	WritePrivateProfileString(TOOLBAR_POS,"ToolbarMainY",num_buf,app_path);
	wsprintf(num_buf,"%d", toolbarSavedX[1]);
	WritePrivateProfileString(TOOLBAR_POS,"ToolbarChannelX",num_buf,app_path);
	wsprintf(num_buf,"%d", toolbarSavedY[1]);
	WritePrivateProfileString(TOOLBAR_POS,"ToolbarChannelY",num_buf,app_path);

	wsprintf(num_buf,"%d",iDlgRepeat );
	WritePrivateProfileString(MIDI_EXPORT,"Repeat",num_buf,app_path);

	WritePrivateProfileString(MIDI_EXPORT,"Author", strMIDI_AUTHOR, app_path);
	WritePrivateProfileString(MIDI_EXPORT,"Title", strMIDI_TITLE, app_path);

	//wsprintf(num_buf,"%d",SaveWithInitVolFile );
	//WritePrivateProfileString(INIT_DATA,"autosave",num_buf,app_path);



	SaveRecentFilesToInifile();
	scr_data.SetIniFile();

}

/*
//opening flash
BOOL CALLBACK DialogFlash(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message){
	case WM_INITDIALOG:
		int i;
		long screen_w;
		long screen_h;
		long my_w;
		long my_h;
		long x,y,w,h;

		//screen size
		screen_w = GetSystemMetrics(SM_CXSCREEN);
		screen_h = GetSystemMetrics(SM_CYSCREEN);
		//window size
		my_w = gWidthWindow;
		my_h = gHeightWindow;

		MoveWindow( hDlg, screen_w/2, screen_h/2, 1, 1, TRUE );
		ShowWindow( hDlg, SW_SHOW );
		for( i = 1; i <= WAITFLASH; i++ ){
			x = screen_w/2 - ( my_w/2 * i/WAITFLASH );
			y = screen_h/2 - ( my_h/2 * i/WAITFLASH );
			w = my_w * i / WAITFLASH;
			h = my_h * i / WAITFLASH;
			MoveWindow( hDlg, x, y, w, h, TRUE );
		}
		ShowWindow(hWnd,SW_SHOW);
		EndDialog(hDlg,0);
		return 1;
	default:return FALSE;
	}
	return TRUE;
}
*/
