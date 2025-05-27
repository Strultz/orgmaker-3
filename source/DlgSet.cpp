#include "Setting.h"
#include "DefOrg.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <stdexcept>

#include "Sound.h"
#include "resource.h"
#include "OrgData.h"
#include "Gdi.h"
#include "Filer.h"
#include <commctrl.h>
#include "Mouse.h"
#include "rxoFunction.h"

#define PI 3.14159265358979323846

#define NUMGRID		8
#define NUMGRIDA		11
#define MAXWAVE		100
#define maxx(a, b) ((a) > (b) ? (a) : (b))

//extern HWND hDlgEZCopy;
extern char gSelectedTheme[MAX_PATH];
extern char gSelectedWave[MAX_PATH];

extern char TrackN[];

extern HBITMAP waveBmp; // azy

int volChangeLength = 10;
bool volChangeUseNoteLength = true;
bool volChangeSetNoteLength = false;

typedef struct{
	char name[20];
	char line;
	char dot;
}GRID;
GRID grid[NUMGRIDA] = {
	{"，ustom",0,0},
	{"4/4",4,4},
	{"4/3",4,3},
	{"3/4",3,4},
	{"3/3",3,3},
	{"4/6",4,6},
	{"3/6",3,6},
	{"4/2",4,2},
	{"4/8",4,8},
	{"4/12",4,12},
	{"5/4",5,4},
};

int btn_wave[NUMGRID] = {
	IDC_WAVE1, IDC_WAVE2, IDC_WAVE3, IDC_WAVE4,
	IDC_WAVE5, IDC_WAVE6, IDC_WAVE7, IDC_WAVE8,
};
int txt_freq[NUMGRID] = {
	IDC_FREQ1, IDC_FREQ2, IDC_FREQ3, IDC_FREQ4,
	IDC_FREQ5, IDC_FREQ6, IDC_FREQ7, IDC_FREQ8,
};
int spin_freq[NUMGRID] = {
	IDC_SPIN1, IDC_SPIN2, IDC_SPIN3, IDC_SPIN4,
	IDC_SPIN5, IDC_SPIN6, IDC_SPIN7, IDC_SPIN8,
};
int check_pipi[NUMGRID] = {
	IDC_PI1, IDC_PI2, IDC_PI3, IDC_PI4,
	IDC_PI5, IDC_PI6, IDC_PI7, IDC_PI8,
};
int dd_drambox[NUMGRID] = {
	IDC_WAVEQ, IDC_WAVEW, IDC_WAVEE, IDC_WAVER,
	IDC_WAVET, IDC_WAVEY, IDC_WAVEU, IDC_WAVEI,
};

int txt_Pan[]={
	IDC_PANPOT1, IDC_PANPOT2 , IDC_PANPOT3, IDC_PANPOT4, IDC_PANPOT5, IDC_PANPOT6, IDC_PANPOT7, IDC_PANPOT8, 
	IDC_PANPOT9, IDC_PANPOT10, IDC_PANPOT11, IDC_PANPOT12, IDC_PANPOT13, IDC_PANPOT14, IDC_PANPOT15, IDC_PANPOT16
};

int txt_Vol[]={
	IDC_VOLUME1, IDC_VOLUME2, IDC_VOLUME3, IDC_VOLUME4, IDC_VOLUME5, IDC_VOLUME6, IDC_VOLUME7, IDC_VOLUME8,
	IDC_VOLUME9, IDC_VOLUME10, IDC_VOLUME11, IDC_VOLUME12, IDC_VOLUME13, IDC_VOLUME14, IDC_VOLUME15, IDC_VOLUME16
};

int Preset1_Pan[]={
	6, 6, 5, 4, 8, 9, 3, 7,
	6, 7, 5, 5, 6, 6, 8, 4
};
int Preset1_Vol[]={
	180, 210, 170, 150, 150, 190, 120, 140,
	180, 190, 225, 230, 195, 200, 180, 190
};


//char *strTrack[] ={"１", "２", "３", "４", "５", "６", "７", "８",  "Ｑ", "Ｗ", "Ｅ", "Ｒ", "Ｔ", "Ｙ", "Ｕ", "Ｉ"};	// 2014.10.19 D
char *strTrack[MAXTRACK];	// 2014.10.19 A
//char *strNNNTrack[] ={"[１]", "[２]", "[３]", "[４]", "[５]", "[６]", "[７]", "[８]",  "[Ｑ]", "[Ｗ]", "[Ｅ]", "[Ｒ]", "[Ｔ]", "[Ｙ]", "[Ｕ]", "[Ｉ]"};	// 2014.10.19 D
char *strNNNTrack[MAXTRACK];	// 2014.10.19 A

int SamplePlayHeight = 36; //サンプル音を鳴らす高さ

//int SaveWithInitVolFile;	//曲データと…セーブするか。

extern HINSTANCE hInst;//インスタンスハンドル
extern void ReloadBitmaps();
extern void GetApplicationPath(char* path);

double GetNormDistRand(){
	double x, y;
	double s, t;
	double r1, r2;

	// 一様分布に従う擬似乱数 x, y を作る
	do { x = (double)rand()/(double)RAND_MAX; } while (x == 0.0); // x が 0 になるのを避ける
	y = (double)rand()/(double)RAND_MAX;

	// s, t の計算
	s = sqrt(-2.0 * log(x)); // C, Java では log が自然対数
	t = 2.0 * PI * y;

	// 標準正規分布に従う擬似乱数 r1, r2
	r1 = s * cos(t);  r2 = s * sin(t);
	return r2;
}

void SetText(HWND hdwnd, int ID_TEXTBOX, char *str)
{
	SetDlgItemText(hdwnd, ID_TEXTBOX , str);
}

void SetText(HWND hdwnd, int ID_TEXTBOX, int iValue)
{
	char str[128] = {NULL};
	itoa(iValue,str,10);
	SetDlgItemText(hdwnd, ID_TEXTBOX , str);
}

int GetText(HWND hdwnd, int ID_TEXTBOX)
{
	char str[32] = {NULL};
	GetDlgItemText(hdwnd,ID_TEXTBOX,str,31);
	int r;
	r = atoi(str);
	return r;
}

void EnableDialogWindow(int iValue = TRUE)
{
	//EnableWindow(hDlgPlayer,iValue);
	//EnableWindow(hDlgTrack,iValue);
	//EnableWindow(hDlgEZCopy,iValue);

}


//ダイアログ内容の初期化
static bool gPropChanged = false;

void InitSettingDialog(HWND hdwnd)
{
	char str[128] = {NULL};
	long i,a;
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);

	itoa(mi.wait,str,10);
	SetDlgItemText(hdwnd,IDD_SETWAIT,str);

	snprintf(str, 128, "%.3f", ((mi.wait > 0 && mi.dot > 0) ? (60000.0 / (double)(mi.wait * mi.dot)) : 0));
	SetDlgItemText(hdwnd, IDC_BPM, str);

	TCHAR *q, *p;
	p = MessageString[IDS_GRID_STRING];
	for(i = 0; i < NUMGRIDA; i++){
		for(q = grid[i].name; *p != 0; p++, q++)*q = *p;
		*q = 0; p++;
	}

	for(i = 0; i < NUMGRIDA; i++)
		SendDlgItemMessage(hdwnd,IDD_LB1,LB_ADDSTRING,0,(LPARAM)grid[i].name);

	for(i = 0; i < NUMGRIDA; i++)
		if(mi.line == grid[i].line && mi.dot == grid[i].dot)break;

	if(i == NUMGRIDA) i = 0;
	
	if(i > 0){
		a = grid[i].line;
		itoa(a,str,10);
		SetDlgItemText(hdwnd,IDD_GRIDEDIT1,str);
		a = grid[i].dot;
		itoa(a,str,10);
		SetDlgItemText(hdwnd,IDD_GRIDEDIT2,str);
		EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT1), FALSE);
		EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT2), FALSE);
	}else{
		EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT1), TRUE);
		EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT2), TRUE);
		a = mi.line;
		itoa(a,str,10);
		SetDlgItemText(hdwnd,IDD_GRIDEDIT1,str);
		a = mi.dot;
		itoa(a,str,10);
		SetDlgItemText(hdwnd,IDD_GRIDEDIT2,str);
	}

	SendDlgItemMessage(hdwnd,IDD_LB1,LB_SETCURSEL,i,0);
	a = mi.repeat_x / (mi.dot * mi.line);
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_REP_MEAS,str);
	a = mi.end_x / (mi.dot * mi.line);
	itoa(a,str,10);
	SetDlgItemText(hdwnd,IDD_END_MEAS,str);
	// this one is better
	a = mi.repeat_x % (mi.dot * mi.line);
	itoa(a, str, 10);
	SetDlgItemText(hdwnd, IDD_REP_BEAT, str);
	a = mi.end_x % (mi.dot * mi.line);
	itoa(a, str, 10);
	SetDlgItemText(hdwnd, IDD_END_BEAT, str);

	HWND w = GetDlgItem(hdwnd, IDC_WAITSPIN);
	SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(2000, 1));
	SendMessage(w, UDM_SETPOS, 0, mi.wait);
	w = GetDlgItem(hdwnd, IDC_BPMSPIN);
	SendMessage(w, UDM_SETRANGE32, 1, 60000);
	SendMessage(w, UDM_SETPOS32, 0, (mi.wait > 0 && mi.dot > 0) ? (int)(60000.0 / (double)(mi.wait * mi.dot)) : 0);

	w = GetDlgItem(hdwnd, IDC_BPMSPIN2);
	SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(99, 1));
	SendMessage(w, UDM_SETPOS, 0, mi.line);
	w = GetDlgItem(hdwnd, IDC_BPMSPIN3);
	SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(99, 1));
	SendMessage(w, UDM_SETPOS, 0, mi.dot);

	w = GetDlgItem(hdwnd, IDC_STARTMEASSPIN);
	SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(999, 1));
	SendMessage(w, UDM_SETPOS, 0, (mi.dot > 0) ? mi.repeat_x / mi.dot : 0);
	w = GetDlgItem(hdwnd, IDC_STARTSPIN);
	SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(9801, 1));
	SendMessage(w, UDM_SETPOS, 0, (mi.dot > 0) ? mi.repeat_x % mi.dot : 0);
	w = GetDlgItem(hdwnd, IDC_ENDMEASSPIN);
	SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(999, 1));
	SendMessage(w, UDM_SETPOS, 0, (mi.dot > 0) ? mi.end_x / mi.dot : 0);
	w = GetDlgItem(hdwnd, IDC_ENDSPIN);
	SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(9801, 1));
	SendMessage(w, UDM_SETPOS, 0, (mi.dot > 0) ? mi.end_x % mi.dot : 0);
}

//ウエイトの設定
BOOL SetWait(HWND hdwnd, MUSICINFO *mi)
{
	char str[128] = {NULL};
	long a;
	GetDlgItemText(hdwnd,IDD_SETWAIT,str,7);
	a = atol(str);
	if(a > 2000 || a < 1){
		//MessageBox(hdwnd,"1～2000にしてください","ERROR(ウエイト)",MB_OK);	// 2014.10.19 D
		msgbox(hdwnd,IDS_VALUESET12000,IDS_ERROR_WAIT,MB_OK | MB_ICONWARNING);	// 2014.10.19 A
		return FALSE;
	}
	mi->wait = (unsigned short)a;
	return TRUE;
}

//表示グリッドの設定
BOOL SetGrid(HWND hdwnd,MUSICINFO *mi)
{
	int i;
	//選択グリッドの設定///////////////////////
	i = SendDlgItemMessage(hdwnd, IDD_LB1,LB_GETCURSEL,0,0);//インデックスを得る
//	EndDialog(hdwnd,0);
	if(i>0){
		mi->dot = grid[i].dot;
		mi->line = grid[i].line;
	}else{
		char str[128] = {NULL};
		long a,b;
		GetDlgItemText(hdwnd,IDD_GRIDEDIT1,str,3);
		a = atol(str);
		if(a<=0 || a>=128){
			//MessageBox(hdwnd,"拍子の数値がおかしいです。","ERROR(拍子)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_WARNING_HYOUSHI,IDS_ERROR_HYOUSHI,MB_OK | MB_ICONWARNING);	// 2014.10.19 A
			return FALSE;
		}
		GetDlgItemText(hdwnd,IDD_GRIDEDIT2,str,3);
		b = atol(str);
		if(b<=0 || b>=128){
			//MessageBox(hdwnd,"分割の数値がおかしいです。","ERROR(分割)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_WARNING_BUNKATSU,IDS_ERROR_BUNKATSU,MB_OK | MB_ICONWARNING);	// 2014.10.19 A
			return FALSE;
		}

		mi->line = (unsigned char)a;
		mi->dot = (unsigned char)b;

	}
	return TRUE;
}
//リピート範囲の設定
BOOL SetRepeat(HWND hdwnd, MUSICINFO *mi)
{
	char str[128] = {NULL};
	long a,b;
	GetDlgItemText(hdwnd,IDD_REP_MEAS,str,7);
	a = atol(str);
	GetDlgItemText(hdwnd, IDD_REP_BEAT, str, 7);
	b = atol(str);
	mi->repeat_x = (unsigned short)a * (mi->dot * mi->line) + b;
	GetDlgItemText(hdwnd,IDD_END_MEAS,str,7);
	a = atol(str);
	GetDlgItemText(hdwnd, IDD_END_BEAT, str, 7);
	b = atol(str);
	mi->end_x = (unsigned short)a * (mi->dot * mi->line) + b;
	if(mi->end_x <= mi->repeat_x){
		//MessageBox(hdwnd,"あたま＜おわり に設定してください","ERROR(リピート範囲)",MB_OK);	// 2014.10.19 D
		msgbox(hdwnd,IDS_WARNING_FROM_TO,IDS_ERROR_REPERT,MB_OK | MB_ICONWARNING);	// 2014.10.19 A
		return FALSE;
	}
	return TRUE;
}
//追加周波数の設定
/*int freqbox[MAXTRACK] = {
	IDD_SETFREQ0,
	IDD_SETFREQ1,
	IDD_SETFREQ2,
	IDD_SETFREQ3,
	IDD_SETFREQ4,
	IDD_SETFREQ5,
	IDD_SETFREQ6,
	IDD_SETFREQ7,

	IDD_SETFREQ8,
	IDD_SETFREQ9,
	IDD_SETFREQ10,
	IDD_SETFREQ11,
	IDD_SETFREQ12,
	IDD_SETFREQ13,
	IDD_SETFREQ14,
	IDD_SETFREQ15,
};*/
/*BOOL SetTrackFreq(HWND hdwnd, MUSICINFO* mi)
{
	char str[128] = {NULL};
	unsigned short a;
	bool msg = false;
	for(int i = 0; i < MAXMELODY; i++){
		GetDlgItemText(hdwnd,freqbox[i],str,7);
		a = (unsigned short)atol(str);
//削除 2014.05.02
//		if(a < 100 || a > 1900){
//			MessageBox(hdwnd,"100～1900に設定","ERROR(周波数)",MB_OK);
//			return FALSE;
//		}
		if(!msg && (a < 100 || a > 1900)){
			msg = true;
			//MessageBox(hdwnd,"100～1900に設定したほうがいいですよ。","お知らせ(周波数)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_VALUESET1001900,IDS_INFO_TITLE_FREQ,MB_OK);	// 2014.10.19 A
//			return FALSE;
		}
		mi->tdata[i].freq = a;
	}
	return TRUE;
}
BOOL SetPipiCheck(HWND hdwnd, MUSICINFO *mi)
{
	for(int i = 0; i < MAXMELODY; i++){
		
		if( IsDlgButtonChecked( hdwnd, check_pipi[i] ) )
			mi->tdata[i].pipi = 1;
		else mi->tdata[i].pipi = 0;
	}
	return TRUE;
}*/

//曲の設定

BOOL CALLBACK DialogSetting(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnm;
	static bool updateWait = false;
	static bool updateBPM = false;

	int i, j;
	double iBPM;
	int iWAIT;
	char str[128] = {NULL};
	RECT rect = {64,0,WWidth,WHeight};//更新する領域
	MUSICINFO mi,mg;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		updateWait = true;
		updateBPM = true;
		InitSettingDialog(hdwnd);
		//EnableDialogWindow(FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDD_LB1:
			if (HIWORD(wParam) == LBN_SELCHANGE) { //ﾘｽﾄﾎﾞｯｸｽでの選択変更
				i = SendDlgItemMessage(hdwnd, IDD_LB1, LB_GETCURSEL, 0, 0);//インデックスを得る
				if (i == 0) {
					EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT1), TRUE);
					EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT2), TRUE);
				}
				else {
					EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT1), FALSE);
					EnableWindow(GetDlgItem(hdwnd, IDD_GRIDEDIT2), FALSE);
				}
				PropSheet_Changed(GetParent(hdwnd), hdwnd);
				gPropChanged = true;
			}
			break;
		case IDC_BPM:
		case IDD_SETWAIT:
			org_data.GetMusicInfo(&mi);

			// Update wait when BPM is updated
			if (LOWORD(wParam) == IDC_BPM && (HIWORD(wParam) == EN_UPDATE || HIWORD(wParam) == EN_KILLFOCUS))
			{
				if (updateBPM) {
					updateBPM = false;
					break;
				}

				GetDlgItemText(hdwnd, IDC_BPM, str, 128);

				iBPM = 0;
				try {
					iBPM = std::stod(str);
				} catch (const std::invalid_argument&) { // No
				} catch (const std::out_of_range&) {
				}

				iWAIT = 0;
				if (iBPM > 0 && mi.dot > 0) {
					iWAIT = (int)round(60000.0 / iBPM / (double)mi.dot);
				}

				snprintf(str, 128, "%d", iWAIT);

				updateWait = true;
				SetDlgItemText(hdwnd, IDD_SETWAIT, str);
			}

			// Update BPM when wait is updated
			if ((LOWORD(wParam) == IDD_SETWAIT && (HIWORD(wParam) == EN_UPDATE || HIWORD(wParam) == EN_KILLFOCUS))
				|| (LOWORD(wParam) == IDC_BPM && HIWORD(wParam) == EN_KILLFOCUS))
			{
				if (updateWait) {
					updateWait = false;
					break;
				}

				GetDlgItemText(hdwnd, IDD_SETWAIT, str, 128);

				iWAIT = atoi(str);
				iBPM = 0;
				if (iWAIT > 0 && mi.dot > 0) {
					iBPM = 60000.0 / (double)(iWAIT * mi.dot);
				}

				snprintf(str, 128, "%.3f", iBPM);

				updateBPM = true;
				SetDlgItemText(hdwnd, IDC_BPM, str);
			}

			// This is just to reformat it so it's consistent
			if (LOWORD(wParam) == IDD_SETWAIT && HIWORD(wParam) == EN_KILLFOCUS) {
				GetDlgItemText(hdwnd, IDD_SETWAIT, str, 128);
				iWAIT = atoi(str);
				snprintf(str, 128, "%d", iWAIT);
				updateWait = true;
				SetDlgItemText(hdwnd, IDD_SETWAIT, str);
			}

			// Fallthrough
		case IDD_REP_MEAS: case IDD_END_MEAS:
		case IDD_REP_BEAT: case IDD_END_BEAT:
		case IDD_GRIDEDIT1: case IDD_GRIDEDIT2:
			if (HIWORD(wParam) == EN_SETFOCUS) {
				PostMessage(GetDlgItem(hdwnd, LOWORD(wParam)), EM_SETSEL, 0, -1);
			} else if (HIWORD(wParam) == EN_UPDATE) {
				PropSheet_Changed(GetParent(hdwnd), hdwnd);
				gPropChanged = true;
			}
			break;
		}
		break;
	case WM_NOTIFY: {
		lpnm = (LPNMHDR)lParam;
		switch (lpnm->code) {
		case PSN_KILLACTIVE: {
			bool error = false;
			if (!SetWait(hdwnd, &mi)) {
				error = true;
			}
			if (!SetGrid(hdwnd, &mi)) {
				error = true;
			}
			if (!SetRepeat(hdwnd, &mi)) {
				error = true;
			}
			SetWindowLong(hdwnd, DWL_MSGRESULT, error);
			return error;
		}
		case PSN_APPLY: {
			bool error = false;
			org_data.GetMusicInfo(&mi);
			if (!SetWait(hdwnd, &mi)) error = true;
			if (!SetGrid(hdwnd, &mi)) error = true;
			if (!SetRepeat(hdwnd, &mi)) error = true;
			if (error) {
				SetWindowLong(hdwnd, DWL_MSGRESULT, PSNRET_INVALID);
			}
			else {
				SetWindowLong(hdwnd, DWL_MSGRESULT, PSNRET_NOERROR);
				org_data.SetMusicInfo(&mi, SETGRID | SETWAIT | SETREPEAT);

				// update the bpm
				HWND sw = GetDlgItem(hdwnd, IDD_SETWAIT);
				SendMessage(sw, WM_COMMAND, MAKEWPARAM(IDD_SETWAIT, EN_KILLFOCUS), (LPARAM)sw);
			}
			return error;
		}
		case UDN_DELTAPOS: {
			LPNMUPDOWN lud = (LPNMUPDOWN)lParam;
			HWND buddy = (HWND)SendMessage(lpnm->hwndFrom, UDM_GETBUDDY, 0, 0);
			if (buddy) {
				if (lpnm->idFrom != IDC_BPMSPIN) {
					int v = GetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), NULL, FALSE);
					v += lud->iDelta;

					switch (lpnm->idFrom) {
					case IDC_WAITSPIN:
						if (v < 1) v = 1;
						if (v > 2000) v = 2000;
						break;
					case IDC_BPMSPIN2:
					case IDC_BPMSPIN3:
						if (v < 1) v = 1;
						if (v > 99) v = 99;
						break;
					case IDC_STARTMEASSPIN:
					case IDC_ENDMEASSPIN:
						if (v < 0) v = 0;
						if (v >= 999) {
							v = 999;
							int relid = lpnm->idFrom == IDC_STARTMEASSPIN ? IDD_REP_BEAT : IDD_END_BEAT;
							SetDlgItemInt(hdwnd, relid, 0, TRUE);
						}
						break;
					case IDC_STARTSPIN:
					case IDC_ENDSPIN: {
						org_data.GetMusicInfo(&mi);

						int relid = lpnm->idFrom == IDC_STARTSPIN ? IDD_REP_MEAS : IDD_END_MEAS;
						int m = GetDlgItemInt(hdwnd, relid, NULL, TRUE);
						if (m == 999 && lud->iDelta > 0) {
							v = 0;
							break;
						}

						while (v < 0 && mi.dot * mi.line != 0) {
							v += mi.dot * mi.line;

							m -= 1;
							if (m < 0) {
								m = 0;
								v = 0;
							}

							SetDlgItemInt(hdwnd, relid, m, TRUE);
						}

						while (v >= mi.dot * mi.line && mi.dot * mi.line != 0) {
							v -= mi.dot * mi.line;

							m += 1;
							if (m >= 999) {
								m = 999;
								v = 0;
							}

							SetDlgItemInt(hdwnd, relid, m, TRUE);
						}

						if (v < 0) v = 0;
						break;
					}
					}

					SetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), v, FALSE);
					SendMessage(lpnm->hwndFrom, UDM_SETPOS, 0, v);
				}
				else {
					GetDlgItemText(hdwnd, IDC_BPM, str, 128);

					iBPM = 0;
					try {
						iBPM = std::stod(str);
					} catch (const std::invalid_argument&) { // No
					} catch (const std::out_of_range&) {
					}

					iBPM += lud->iDelta;

					org_data.GetMusicInfo(&mi);
					if (mi.dot > 0) {
						iWAIT = (int)round(60000.0 / iBPM / (double)mi.dot);
						if (iWAIT < 1) iBPM = 60000.0 / (double)(1 * mi.dot);
						if (iWAIT > 2000) iBPM = 60000.0 / (double)(2000 * mi.dot);
					}

					snprintf(str, 128, "%.3f", iBPM);
					SetDlgItemText(hdwnd, IDC_BPM, str);
					SendMessage(lpnm->hwndFrom, UDM_SETPOS32, 0, (int)iBPM);
				}

				PropSheet_Changed(GetParent(hdwnd), hdwnd);
				gPropChanged = true;
				return 0;
			}
			return 1;
		}
		}
	}
	}
	return 0;
}
char *dram_name[] = {
	"Bass01",
	"Bass02",
	"Snare01",
	"Snare02",
	"Tom01",

	"HiClose",
	"HiOpen",
	"Crash",
	"Per01",
	"Per02",

	"Bass03",
	"Tom02",
	"Bass04", //新規追加
	"Bass05",
	"Snare03",

	"Snare04",
	"HiClose02",
	"HiOpen02",
	"HiClose03",
	"HiOpen03",

	"Crash02",
	"RevSym01",
	"Ride01",
	"Tom03",
	"Tom04",

	"OrcDrm01",	
	"Bell",
	"Cat" ,
	"Bass06",		//さらに追加
	"Bass07",

	"Snare05",
	"Snare06",
	"Snare07",
	"Tom05",
	"HiOpen04",

	"HiClose04",
	"Clap01",
	"Pesi01",
	"Quick01",
	"Bass08" ,		//懲りずに追加	// 2011.10.17 

	"Snare08",
	"HiClose05",
    //"Per03",        // 2024.08.24
};

//アルファベット順変換定義
unsigned char Wave_no_to_List_no[] = {
	0,1,10,12,13,28,29,39,26,27,36,7,20,5,16,18,35,41,6,17,19,34,25,8,9,37,38,21,22,2,3,14,15,30,31,32,40,4,11,23,24,33
};
unsigned char List_no_to_Wave_no[] = {
	0,1,29,30,37,13,18,11,23,24,2,38,3,4,31,32,14,19,15,20,12,27,28,39,40,22,8,9,5,6,33,34,35,41,21,16,10,25,26,7,36,17
};
/*unsigned char Wave_no_to_List_no[]={
	0,1,10,12,13,28,29,39,26,27,36,7,20,5,16,18,35,41,6,17,19,34,25,8,9,42,37,38,21,22,2,3,14,15,30,31,32,40,4,11,23,24,33
};
unsigned char List_no_to_Wave_no[]={
	0,1,30,31,38,13,18,11,23,24,2,39,3,4,32,33,14,19,15,20,12,28,29,40,41,22,8,9,5,6,34,35,36,42,21,16,10,26,27,7,37,17,25
};*/

static bool gOkay = false;
static unsigned char gWaveTrack = 0;
static unsigned char gWaveSelect = 0;
static unsigned char gAllWaveSel[MAXMELODY] = { 0 };
static char* strTone[] = { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-" };

BOOL CALLBACK DialogWaveSel(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LPNMHDR lpnm;
	MUSICINFO mi;
	char tone[4] = { 0 };

	switch (message) {
	case WM_INITDIALOG: {
		gWaveSelect = gAllWaveSel[gWaveTrack];

		SendMessage(GetDlgItem(hdwnd, IDC_SLIDERPITCH), TBM_SETPOS, TRUE, SamplePlayHeight);
		SendMessage(GetDlgItem(hdwnd, IDC_SLIDERPITCH), TBM_SETRANGE, 0, MAKELPARAM(0, 95));
		SendMessage(GetDlgItem(hdwnd, IDC_SLIDERPITCH), TBM_SETLINESIZE, 0, 1);
		SendMessage(GetDlgItem(hdwnd, IDC_SLIDERPITCH), TBM_SETPAGESIZE, 0, 12);
		SendMessage(GetDlgItem(hdwnd, IDC_SLIDERPITCH), TBM_SETTICFREQ, 12, 0);

		snprintf(tone, 4, "%s%d", strTone[SamplePlayHeight % 12], (SamplePlayHeight / 12) + 1);
		SetDlgItemText(hdwnd, IDC_PREVPITCH, tone);
		return 1;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			gOkay = true;
			EndDialog(hdwnd, 0);
			break;
		case IDCANCEL:
			EndDialog(hdwnd, 0);
			break;
		}
		break;
	case WM_LBUTTONDOWN: {
		org_data.GetMusicInfo(&mi);

		int x = LOWORD(lParam);
		int y = HIWORD(lParam);

		if (y < 491) {
			if (x > 190) x -= 4;
			x = (x - 6) / 36;
			if (x < 0) x = 0;
			if (x > 9) x = 9;

			if (y > 246) y -= 4;
			y = (y - 4) / 48;
			if (y < 0) y = 0;
			if (y > 9) y = 9;

			// Invalidate Old, Rect
			int boxX = 8 + (gWaveSelect % 10) * 36 + ((gWaveSelect % 10) > 4 ? 4 : 0);
			int boxY = 16 + (gWaveSelect / 10) * 48 + ((gWaveSelect / 10) > 4 ? 4 : 0);

			RECT rcSel = { boxX - 4, boxY - 4, boxX + 36, boxY + 36 };
			InvalidateRect(hdwnd, &rcSel, FALSE);

			gWaveSelect = (unsigned char)(x % 10 + y * 10);

			// Invalidate New, Rect
			boxX = 8 + (gWaveSelect % 10) * 36 + ((gWaveSelect % 10) > 4 ? 4 : 0);
			boxY = 16 + (gWaveSelect / 10) * 48 + ((gWaveSelect / 10) > 4 ? 4 : 0);

			rcSel = { boxX - 4, boxY - 4, boxX + 36, boxY + 36 };
			InvalidateRect(hdwnd, &rcSel, FALSE);

			MakeOrganyaWave(gWaveTrack, gWaveSelect, mi.tdata[gWaveTrack].pipi);
			PlayOrganKey(SamplePlayHeight, gWaveTrack, mi.tdata[gWaveTrack].freq, 240);
		}
		break;
	}
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hdwnd, &ps);

		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, waveBmp);

		BITMAP bitmap;
		GetObject(waveBmp, sizeof(bitmap), &bitmap);
		BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

		HBRUSH hbr = CreateSolidBrush(RGB(0, 255, 0));

		int boxX = 8 + (gWaveSelect % 10) * 36 + ((gWaveSelect % 10) > 4 ? 4 : 0);
		int boxY = 16 + (gWaveSelect / 10) * 48 + ((gWaveSelect / 10) > 4 ? 4 : 0);

		RECT rc = { boxX - 2, boxY - 2, boxX, boxY + 34 };
		FillRect(hdc, &rc, hbr);
		rc = { boxX + 32, boxY - 2, boxX + 34, boxY + 34 };
		FillRect(hdc, &rc, hbr);
		rc = { boxX, boxY - 2, boxX + 32, boxY };
		FillRect(hdc, &rc, hbr);
		rc = { boxX, boxY + 32, boxX + 32, boxY + 34 };
		FillRect(hdc, &rc, hbr);

		DeleteObject(hbr);

		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);

		EndPaint(hdwnd, &ps);
		break;
	}
	case WM_HSCROLL: {
		if (lParam != NULL && GetWindowLong((HWND)lParam, GWL_ID) == IDC_SLIDERPITCH) {
			SamplePlayHeight = SendMessage(GetDlgItem(hdwnd, IDC_SLIDERPITCH), TBM_GETPOS, 0, 0);
			snprintf(tone, 4, "%s%d", strTone[SamplePlayHeight % 12], (SamplePlayHeight / 12) + 1);
			SetDlgItemText(hdwnd, IDC_PREVPITCH, tone);
		}
		break;
	}
	}
	return 0;
}

BOOL CALLBACK DialogWave(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	char bfr[256];
	int i;
	MUSICINFO mi;
	LPNMHDR lpnm;

	switch (message) {
	case WM_INITDIALOG:
		org_data.GetMusicInfo(&mi);
		for (i = 0; i < MAXMELODY; ++i) {
			gAllWaveSel[i] = mi.tdata[i].wave_no;

			snprintf(bfr, 256, "Wave-%02d", gAllWaveSel[i]);
			SetDlgItemText(hdwnd, btn_wave[i], bfr);
			SetDlgItemInt(hdwnd, txt_freq[i], mi.tdata[i].freq, FALSE);
			CheckDlgButton(hdwnd, check_pipi[i], mi.tdata[i].pipi);

			HWND w = GetDlgItem(hdwnd, spin_freq[i]);
			SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(2000, 0));
			SendMessage(w, UDM_SETPOS, 0, mi.tdata[i].freq);
		}

		return 1;
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case EN_UPDATE:
		case EN_SETFOCUS:
			switch (LOWORD(wParam)) {
			case IDC_FREQ1:
			case IDC_FREQ2:
			case IDC_FREQ3:
			case IDC_FREQ4:
			case IDC_FREQ5:
			case IDC_FREQ6:
			case IDC_FREQ7:
			case IDC_FREQ8:
				if (HIWORD(wParam) == EN_UPDATE) {
					PropSheet_Changed(GetParent(hdwnd), hdwnd);
					gPropChanged = true;
				} else {
					PostMessage(GetDlgItem(hdwnd, LOWORD(wParam)), EM_SETSEL, 0, -1);
				}
				break;
			}
			break;
		case BN_CLICKED:
			switch (LOWORD(wParam)) {
			case IDC_WAVE1: i = 0; break;
			case IDC_WAVE2: i = 1; break;
			case IDC_WAVE3: i = 2; break;
			case IDC_WAVE4: i = 3; break;
			case IDC_WAVE5: i = 4; break;
			case IDC_WAVE6: i = 5; break;
			case IDC_WAVE7: i = 6; break;
			case IDC_WAVE8: i = 7; break;
			case IDC_PI1:
			case IDC_PI2:
			case IDC_PI3:
			case IDC_PI4:
			case IDC_PI5:
			case IDC_PI6:
			case IDC_PI7:
			case IDC_PI8:
				PropSheet_Changed(GetParent(hdwnd), hdwnd);
				gPropChanged = true;
			default: i = -1; break;
			}

			if (i >= 0) {
				org_data.GetMusicInfo(&mi);
				gWaveTrack = i;

				gOkay = false;
				DialogBox(hInst, "DLGWAVESEL", hdwnd, DialogWaveSel);
				if (gOkay) {
					gAllWaveSel[i] = gWaveSelect;

					snprintf(bfr, 256, "Wave-%02d", gAllWaveSel[i]);
					SetDlgItemText(hdwnd, btn_wave[i], bfr);

					PropSheet_Changed(GetParent(hdwnd), hdwnd);
					gPropChanged = true;

					gOkay = false;
				}
			}
			break;
		}
		break;
	case WM_NOTIFY:
		lpnm = (LPNMHDR)lParam;
		switch (lpnm->code) {
		case PSN_KILLACTIVE: {
			bool error = false;
			SetWindowLong(hdwnd, DWL_MSGRESULT, error);
			return error;
		}
		case PSN_APPLY: {
			bool error = false;

			org_data.GetMusicInfo(&mi);
			for (i = 0; i < MAXMELODY; ++i) {
				mi.tdata[i].wave_no = gAllWaveSel[i];
				int v = GetDlgItemInt(hdwnd, txt_freq[i], NULL, FALSE);
				if (v < 0 || v > 65535) {
					v = 1000;
				}
				mi.tdata[i].freq = v;
				mi.tdata[i].pipi = IsDlgButtonChecked(hdwnd, check_pipi[i]);
			}

			if (error) {
				SetWindowLong(hdwnd, DWL_MSGRESULT, PSNRET_INVALID);
			}
			else {
				SetWindowLong(hdwnd, DWL_MSGRESULT, PSNRET_NOERROR);
				org_data.SetMusicInfo(&mi, SETFREQ | SETPIPI | SETWAVE);
				for (i = 0; i < MAXMELODY; ++i) {
					MakeOrganyaWave(i, mi.tdata[i].wave_no, mi.tdata[i].pipi);
				}
			}
			return error;
		}
		case PSN_QUERYCANCEL: {
			org_data.GetMusicInfo(&mi);
			for (i = 0; i < MAXMELODY; ++i) {
				MakeOrganyaWave(i, mi.tdata[i].wave_no, mi.tdata[i].pipi);
			}
			break;
		}
		case UDN_DELTAPOS: {
			LPNMUPDOWN lud = (LPNMUPDOWN)lParam;
			HWND buddy = (HWND)SendMessage(lpnm->hwndFrom, UDM_GETBUDDY, 0, 0);
			if (buddy) {
				int v = GetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), NULL, FALSE);
				v += lud->iDelta * 10;

				if (v < 0) v = 0;
				if (v > 2000) v = 2000;

				SetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), v, FALSE);
				SendMessage(lpnm->hwndFrom, UDM_SETPOS, 0, v);

				PropSheet_Changed(GetParent(hdwnd), hdwnd);
				gPropChanged = true;
				return 0;
			}
			return 1;
		}
		}
		break;
	}
	return 0;
}

BOOL CALLBACK DialogPerc(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int i, j;
	MUSICINFO mi;
	LPNMHDR lpnm;

	switch (message) {
	case WM_INITDIALOG:
		org_data.GetMusicInfo(&mi);
		for (i = 0; i < MAXDRAM; ++i) {
			HWND cc = GetDlgItem(hdwnd, dd_drambox[i]);
			SendMessage(cc, WM_SETREDRAW, 0, 0);
			SendMessage(cc, CB_RESETCONTENT, 0, 0);
			for (j = 0; j < NUMDRAMITEM; ++j) {
				SendMessage(cc, CB_ADDSTRING, 0, (LPARAM)dram_name[Wave_no_to_List_no[j]]);
			}
			SendMessage(cc, WM_SETREDRAW, 1, 0);
			SendMessage(cc, CB_SETCURSEL, List_no_to_Wave_no[mi.tdata[i + MAXMELODY].wave_no], 0);
		}
		return 1;
	case WM_COMMAND:
		switch (HIWORD(wParam)) {
		case CBN_SELCHANGE:
			switch (LOWORD(wParam)) {
			case IDC_WAVEQ: i = 8; break;
			case IDC_WAVEW: i = 9; break;
			case IDC_WAVEE: i = 10; break;
			case IDC_WAVER: i = 11; break;
			case IDC_WAVET: i = 12; break;
			case IDC_WAVEY: i = 13; break;
			case IDC_WAVEU: i = 14; break;
			case IDC_WAVEI: i = 15; break;
			default: i = -1; break;
			}

			if (i >= 0) {
				int ndx = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
				InitDramObject(Wave_no_to_List_no[ndx], i - MAXMELODY);
				PlayOrganKey(36, i, 1000, 240);
			}
			
			PropSheet_Changed(GetParent(hdwnd), hdwnd);
			gPropChanged = true;
			break;
		}
		break;
	case WM_NOTIFY:
		lpnm = (LPNMHDR)lParam;
		switch (lpnm->code) {
		case PSN_KILLACTIVE: {
			bool error = false;

			for (i = 0; i < MAXDRAM; ++i) {
				HWND cc = GetDlgItem(hdwnd, dd_drambox[i]);
				int ndx = SendMessage(cc, CB_GETCURSEL, 0, 0);
				if (ndx < 0 || ndx >= NUMDRAMITEM) {
					MessageBox(hdwnd, "Invalid instrument", "Error", MB_ICONWARNING | MB_OK);
					error = true;
				}
			}

			SetWindowLong(hdwnd, DWL_MSGRESULT, error);
			return error;
		}
		case PSN_APPLY: {
			bool error = false;

			org_data.GetMusicInfo(&mi);
			for (i = 0; i < MAXDRAM; ++i) {
				HWND cc = GetDlgItem(hdwnd, dd_drambox[i]);
				int ndx = SendMessage(cc, CB_GETCURSEL, 0, 0);
				if (ndx < 0 || ndx >= NUMDRAMITEM) {
					MessageBox(hdwnd, "Invalid instrument", "Error", MB_ICONWARNING | MB_OK);
					error = true;
				}
				mi.tdata[i + MAXMELODY].wave_no = Wave_no_to_List_no[ndx];
			}

			if (error) {
				SetWindowLong(hdwnd, DWL_MSGRESULT, PSNRET_INVALID);
			}
			else {
				SetWindowLong(hdwnd, DWL_MSGRESULT, PSNRET_NOERROR);
				org_data.SetMusicInfo(&mi, SETWAVE);
				for (i = 0; i < MAXDRAM; ++i) {
					InitDramObject(mi.tdata[i + MAXMELODY].wave_no, i);
				}
			}
			return error;
		}
		case PSN_QUERYCANCEL: {
			org_data.GetMusicInfo(&mi);
			for (i = 0; i < MAXDRAM; ++i) {
				InitDramObject(mi.tdata[i + MAXMELODY].wave_no, i);
			}
			break;
		}
		}
		break;
	}
	return 0;
}

int WINAPI SheetCallback(HWND hwnd, UINT uMsg, LPARAM lParam)
{
	if (uMsg == PSCB_BUTTONPRESSED && (lParam == PSBTN_APPLYNOW || lParam == PSBTN_OK)) {
		if (gPropChanged) {
			SetUndo();
			gPropChanged = false;
		}
	}

	return 0;
}

void OpenSongProperties(HWND hwnd) {
	PROPSHEETHEADER psh;
	PROPSHEETPAGE psp[3];
    
    psp[0].dwSize      = sizeof(PROPSHEETPAGE);
    psp[0].dwFlags     = PSP_USETITLE;
    psp[0].hInstance   = hInst;
    psp[0].pszTemplate = MAKEINTRESOURCE(IDD_DLGSETTING);
    psp[0].pfnDlgProc  = DialogSetting;
	psp[0].pszTitle	   = MAKEINTRESOURCE(IDS_SP_TAB1);
    psp[0].lParam      = 0;
    psp[0].pfnCallback = NULL;
    psp[1].dwSize = sizeof(PROPSHEETPAGE);
    psp[1].dwFlags     = PSP_USETITLE;
    psp[1].hInstance   = hInst;
    psp[1].pszTemplate = MAKEINTRESOURCE(IDD_DLGWAVE);
    psp[1].pfnDlgProc  = DialogWave;
    psp[1].pszTitle    = MAKEINTRESOURCE(IDS_SP_TAB2);
    psp[1].lParam      = 0;
    psp[1].pfnCallback = NULL;
    psp[2].dwSize = sizeof(PROPSHEETPAGE);
    psp[2].dwFlags     = PSP_USETITLE;
    psp[2].hInstance   = hInst;
    psp[2].pszTemplate = MAKEINTRESOURCE(IDD_DLGPERC);
    psp[2].pfnDlgProc  = DialogPerc;
    psp[2].pszTitle    = MAKEINTRESOURCE(IDS_SP_TAB3);
    psp[2].lParam      = 0;
    psp[2].pfnCallback = NULL;
    
    psh.dwSize      = sizeof(PROPSHEETHEADER);
    psh.dwFlags     = PSH_PROPSHEETPAGE | PSH_NOCONTEXTHELP | PSH_USECALLBACK;
    psh.hwndParent  = hwnd;
    psh.hInstance   = hInst;
    psh.pszCaption  = "Song Properties";
    psh.nPages      = sizeof(psp) / sizeof(PROPSHEETPAGE);
    psh.nStartPage  = 0;
    psh.ppsp        = (LPCPROPSHEETPAGE)&psp;
	psh.pfnCallback = SheetCallback;
    
    PropertySheet(&psh);
}


int usebox[MAXTRACK] = {
	IDE_USE0,
	IDE_USE1,
	IDE_USE2,
	IDE_USE3,
	IDE_USE4,
	IDE_USE5,
	IDE_USE6,
	IDE_USE7,

	IDE_USE8,
	IDE_USE9,
	IDE_USE10,
	IDE_USE11,
	IDE_USE12,
	IDE_USE13,
	IDE_USE14,
	IDE_USE15,
};
int leftbox[MAXTRACK] = {
	IDE_LEFT0,
	IDE_LEFT1,
	IDE_LEFT2,
	IDE_LEFT3,
	IDE_LEFT4,
	IDE_LEFT5,
	IDE_LEFT6,
	IDE_LEFT7,

	IDE_LEFT8,
	IDE_LEFT9,
	IDE_LEFT10,
	IDE_LEFT11,
	IDE_LEFT12,
	IDE_LEFT13,
	IDE_LEFT14,
	IDE_LEFT15,
};
BOOL CALLBACK DialogNoteUsed(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i;
	char str[10] = {NULL};
	long u,l;
//	unsigned short a;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		for(i = 0; i < MAXTRACK; i++){
			org_data.GetNoteUsed(&u,&l,i);
			itoa(u,str,10);
			SetDlgItemText(hdwnd,usebox[i],str);
			itoa(l,str,10);
			SetDlgItemText(hdwnd,leftbox[i],str);
		}
		EnableDialogWindow(FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
		case IDOK:
			EnableDialogWindow(TRUE);
			EndDialog(hdwnd,0);
			return 1;
		}
	}
	return 0;
}

BOOL CALLBACK DialogTheme(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int i, j;
	switch (message) {
	case WM_INITDIALOG://ダイアログが呼ばれた
	{
		int cursel = 0;
		SendDlgItemMessage(hdwnd, IDD_THEMES, LB_ADDSTRING, 0, (LPARAM)"OrgMaker 3 (default)");

		WIN32_FIND_DATA fdFile;
		HANDLE hFind = NULL;
		
		char sDir[MAX_PATH];
		GetApplicationPath(sDir);
		strcat(sDir, "themes");

		char sPath[MAX_PATH];
		sprintf(sPath, "%s\\*.*", sDir);

		i = 0;
		if ((hFind = FindFirstFile(sPath, &fdFile)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (strcmp(fdFile.cFileName, ".") != 0
					&& strcmp(fdFile.cFileName, "..") != 0)
				{
					sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);

					if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // It should be a theme folder
					{
						SendDlgItemMessage(hdwnd, IDD_THEMES, LB_ADDSTRING, 0, (LPARAM)fdFile.cFileName);
						++i;
						if (strlen(gSelectedTheme) > 0 && strcmp(sPath, gSelectedTheme) == 0) {
							cursel = i;
						}
					}
				}
			} while (FindNextFile(hFind, &fdFile));

			FindClose(hFind);
		}

		SendDlgItemMessage(hdwnd, IDD_THEMES, LB_SETCURSEL, (WPARAM)cursel, 0);

		EnableDialogWindow(FALSE);
		return 1;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case IDOK:
			gSelectedTheme[0] = 0;

			char sDir[MAX_PATH];
			GetApplicationPath(sDir);

			i = SendDlgItemMessage(hdwnd, IDD_THEMES, LB_GETCURSEL, 0, 0);
			if (i != 0) {
				j = SendDlgItemMessage(hdwnd, IDD_THEMES, LB_GETTEXTLEN, i, 0);
				if (j != LB_ERR) {
					char* nam = (char*)malloc(j + 1);
					if (nam != NULL) {
						memset(nam, '\0', sizeof(nam));
						SendDlgItemMessage(hdwnd, IDD_THEMES, LB_GETTEXT, i, (LPARAM)nam);
						strcat(sDir, "themes\\");
						strcat(sDir, nam);
						free(nam);
					}
				}

				DWORD dwAttrib = GetFileAttributes(sDir);
				if (dwAttrib != INVALID_FILE_ATTRIBUTES && dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
					strcpy(gSelectedTheme, sDir);
			}

			ReloadBitmaps();

			EnableDialogWindow(TRUE);
			EndDialog(hdwnd, 0);
			return 1;
		case IDCANCEL:
			EnableDialogWindow(TRUE);
			EndDialog(hdwnd, 0);
			return 1;
		case IDC_OPNTHMFLD:
			char themePath[MAX_PATH];
			GetApplicationPath(themePath);
			strcat(themePath, "themes");
			ShellExecute(NULL, "open", themePath, NULL, NULL, SW_SHOWDEFAULT);
			break;
		}
		return 1;
	}
	}
	return 0;
}

BOOL CALLBACK DialogWaveDB(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	MUSICINFO mi;
	int i, j;
	switch (message) {
	case WM_INITDIALOG://ダイアログが呼ばれた
	{
		int cursel = 0;
		SendDlgItemMessage(hdwnd, IDD_WAVEDBS, LB_ADDSTRING, 0, (LPARAM)"Organya (default)");

		WIN32_FIND_DATA fdFile;
		HANDLE hFind = NULL;

		char sDir[MAX_PATH];
		GetApplicationPath(sDir);
		strcat(sDir, "soundbanks");

		char sPath[MAX_PATH];
		sprintf(sPath, "%s\\*.wdb", sDir);

		i = 0;
		if ((hFind = FindFirstFile(sPath, &fdFile)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (strcmp(fdFile.cFileName, ".") != 0
					&& strcmp(fdFile.cFileName, "..") != 0)
				{
					sprintf(sPath, "%s\\%s", sDir, fdFile.cFileName);

					if (!(fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // It should be a .wdb file
					{
						SendDlgItemMessage(hdwnd, IDD_WAVEDBS, LB_ADDSTRING, 0, (LPARAM)fdFile.cFileName);
						++i;
						if (strlen(gSelectedWave) > 0 && strcmp(sPath, gSelectedWave) == 0) {
							cursel = i;
						}
					}
				}
			} while (FindNextFile(hFind, &fdFile));

			FindClose(hFind);
		}

		SendDlgItemMessage(hdwnd, IDD_WAVEDBS, LB_SETCURSEL, (WPARAM)cursel, 0);

		EnableDialogWindow(FALSE);
		return 1;
	}
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case IDOK:
			gSelectedWave[0] = 0;

			char sDir[MAX_PATH];
			GetApplicationPath(sDir);

			i = SendDlgItemMessage(hdwnd, IDD_WAVEDBS, LB_GETCURSEL, 0, 0);
			if (i != 0) {
				j = SendDlgItemMessage(hdwnd, IDD_WAVEDBS, LB_GETTEXTLEN, i, 0);
				if (j != LB_ERR) {
					char* nam = (char*)malloc(j + 1);
					if (nam != NULL) {
						memset(nam, '\0', sizeof(nam));
						SendDlgItemMessage(hdwnd, IDD_WAVEDBS, LB_GETTEXT, i, (LPARAM)nam);
						strcat(sDir, "soundbanks\\");
						strcat(sDir, nam);
						free(nam);
					}
				}

				DWORD dwAttrib = GetFileAttributes(sDir);
				if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
					strcpy(gSelectedWave, sDir);
			}

			LoadWaveData100(gSelectedWave);
			GenerateWaveGraphic(wave_data);

			org_data.GetMusicInfo(&mi);

			for (j = 0; j < MAXMELODY; j++)
				MakeOrganyaWave(j, mi.tdata[j].wave_no, mi.tdata[j].pipi);
			for (j = MAXMELODY; j < MAXTRACK; j++) {
				i = mi.tdata[j].wave_no;
				InitDramObject(i, j - MAXMELODY);
			}

			EnableDialogWindow(TRUE);
			EndDialog(hdwnd, 0);
			return 1;
		case IDCANCEL:
			EnableDialogWindow(TRUE);
			EndDialog(hdwnd, 0);
			return 1;
		case IDC_OPNTHMFLD:
			char wavePath[MAX_PATH];
			GetApplicationPath(wavePath);
			strcat(wavePath, "soundbanks");
			ShellExecute(NULL, "open", wavePath, NULL, NULL, SW_SHOWDEFAULT);
			break;
		}
		return 1;
	}
	}
	return 0;
}

static HFONT hFont = NULL;
BOOL CALLBACK DialogMemo(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rc;

	switch(message){
	case WM_INITDIALOG: {
		LOGFONT lf = { 0 };
		SystemParametersInfo(SPI_GETICONTITLELOGFONT, sizeof(LOGFONT), &lf, 0);
		lf.lfWeight = FW_BOLD;
		hFont = CreateFontIndirect(&lf);

		HWND hText = GetDlgItem(hdwnd, IDC_BOLDLABEL);
		SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);

		EnableDialogWindow(FALSE);

		RECT rc;
		GetWindowRect(hdwnd, &rc);

		DWORD style = (DWORD)GetWindowLong(hdwnd, GWL_STYLE);
		DWORD exStyle = (DWORD)GetWindowLong(hdwnd, GWL_EXSTYLE);

		RECT rcNew = { 0, 0, 375, 0 };
		AdjustWindowRectEx(&rcNew, style, FALSE, exStyle);

		SetWindowPos(hdwnd, NULL, 0, 0, rcNew.right - rcNew.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER | SWP_FRAMECHANGED);

		HRSRC hrscr = FindResource(NULL, "TEXTABOUT", "TEXT");// リソースの検索
		if (hrscr)
		{
			DWORD len = SizeofResource(NULL, hrscr);

			char* text = (char *)malloc(len + 1);
			if (text == NULL) {
				// Ooops!
				return 0;
			}

			char *cc = (char *)LockResource(LoadResource(NULL, hrscr));// リソースのアドレスを取得
			memcpy(text, cc, len);
			text[len] = '\0';

			SetDlgItemText(hdwnd, IDC_ABOUTTEXT, text);
			free(text);
		}

		SendMessage(GetDlgItem(hdwnd, IDC_ABOUTTEXT), EM_SETSEL, -1, 0);
		SendMessage(GetDlgItem(hdwnd, IDC_ABOUTTEXT), EM_SCROLLCARET, 0, 0);

		return 1;

		return 1;
	}
	case WM_DESTROY: {
		if (hFont)
			DeleteObject(hFont);
		break;
	}
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
		case IDOK:
			EnableDialogWindow(TRUE);
			EndDialog(hdwnd,0);
			return 1;
		}


		return 1;
	
	}
	return 0;
}

static unsigned long sample_rate = 44100;
static unsigned long loop_count = 0;
static unsigned long fade_mseconds = 0;
static BOOL isEnded = FALSE;

BOOL CALLBACK DialogExportProgress(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {
		return 1;
	}
	case WM_DESTROY: {
		isEnded = TRUE;
		return 1;
	}
	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL) {
			DestroyWindow(hdwnd);
			isEnded = TRUE;
		}
		return 1;
	}
	return 0;
}

#define WRITE_16_LE(p, v) \
    do { \
        *p++ = ((v) & 0xFF); \
        *p++ = ((v) >> 8 & 0xFF); \
    } while(0)

#define WRITE_32_LE(p, v) \
    do { \
        *p++ = ((v) & 0xFF); \
        *p++ = ((v) >> 8 & 0xFF); \
        *p++ = ((v) >> 16 & 0xFF); \
        *p++ = ((v) >> 24 & 0xFF); \
    } while(0)

int ExportWave(unsigned int samples, char* strPath) {
	char header[44] = { 0 };
	unsigned int streamsize = samples * sizeof(float) * 2;

	FILE* fl = fopen(strPath, "wb");

	if (fl != NULL)
	{
		isEnded = false;

		HWND window = CreateDialog(hInst, MAKEINTRESOURCE(IDD_EXPORTPROG), hWnd, DialogExportProgress);
		if (window == NULL)
		{
			fclose(fl);
			return 1;
		}

		char displayText[MAX_PATH + 30];

		char* slash = strrchr(strPath, '/');
		char* backslash = strrchr(strPath, '\\');

		char* name = strPath;
		if (slash == NULL && backslash != NULL) {
			name = &backslash[1];
		}
		else if (slash != NULL && backslash == NULL) {
			name = &slash[1];
		}
		else if (slash != NULL && backslash != NULL) {
			name = slash < backslash ? &backslash[1] : &slash[1];
		}

		snprintf(displayText, MAX_PATH + 30, "Exporting to %s (0%%)", name);
		SetDlgItemText(window, IDC_EXPRFILENAME, displayText);

		HWND hProg = GetDlgItem(window, IDC_EXPRPROGRESS);
		if (hProg == NULL) {
			fclose(fl);
			return 1;
		}

		SendMessage(hProg, PBM_SETRANGE32, 0, samples);
		SendMessage(hProg, PBM_SETSTEP, 0x400, 0);

		MSG msg;
		while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (isEnded) {
			fclose(fl);
			DeleteFile(strPath);
			return 2;
		}

		char* hp = header;
		WRITE_32_LE(hp, 0x46464952); /* "RIFF" */
		WRITE_32_LE(hp, 36 + streamsize); /* "RIFF" length */
		WRITE_32_LE(hp, 0x45564157); /* "WAVE" */
		WRITE_32_LE(hp, 0x20746D66); /* "fmt " */
		WRITE_32_LE(hp, 0x10); /* "fmt " length */
		WRITE_16_LE(hp, 3); /* floating point PCM format */
		WRITE_16_LE(hp, 2); /* 2 channels for stereo */
		WRITE_32_LE(hp, sample_rate); /* Samples per second */
		WRITE_32_LE(hp, sample_rate * sizeof(float) * 2); /* Bytes per second */
		WRITE_16_LE(hp, 8); /* Bytes per sample */
		WRITE_16_LE(hp, 32); /* Bits per sample */
		WRITE_32_LE(hp, 0x61746164); /* "data" */
		WRITE_32_LE(hp, streamsize); /* "data" length */
		fwrite(header, 1, 44, fl);

		SetupExportBuffer(sample_rate, samples, fade_mseconds * sample_rate / 1000);

		float sampleData[0x400 * 2] = { 0 };
		unsigned int samplesLeft = samples;

		while (samplesLeft > 0)
		{
			unsigned int todo = min(0x400, samplesLeft);
			ExportOrganyaBuffer(sampleData, todo);
			fwrite(sampleData, sizeof(float), todo * 2, fl);
			samplesLeft -= todo;

			unsigned int progress = ((samples - samplesLeft) * 100) / samples;

			// Update label
			snprintf(displayText, MAX_PATH + 30, "Exporting to %s (%d%%)", name, progress);
			SetDlgItemText(window, IDC_EXPRFILENAME, displayText);

			// Hack to make the bar update immediately
			SendMessage(hProg, PBM_SETPOS, (samples - samplesLeft) + 1, 0);
			// Update progress bar
			SendMessage(hProg, PBM_SETPOS, (samples - samplesLeft), 0);

			// Update dialog
			// TODO doing this here is kinda bad. The rendering should probably be on a separate thread
			while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			if (isEnded) {
				// Window was closed Cancel.
				EndExportBuffer();
				fclose(fl);

				// Delete the unfinished export
				DeleteFile(strPath);
				return 2;
			}
		}
			
		EndExportBuffer();

		DestroyWindow(window);
		fclose(fl);
		return 0;
	}

	return 1;
}

BOOL CALLBACK DialogWavExport(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MUSICINFO mi;
	char res;
	char strPath[MAX_PATH] = {NULL};
	LPNMHDR lpnm;

	switch (message) {
	case WM_INITDIALOG: {
		char str[10] = { NULL };
		itoa(sample_rate, str, 10);
		SetDlgItemText(hdwnd, IDE_SAMPLE_RATE, str);
		itoa(loop_count, str, 10);
		SetDlgItemText(hdwnd, IDE_LOOP_COUNT, str);
		itoa(fade_mseconds, str, 10);
		SetDlgItemText(hdwnd, IDE_FADE_MSECONDS, str);
		CheckDlgButton(hdwnd, IDC_EXPORTSEPARATE, 0);

		HWND w = GetDlgItem(hdwnd, IDC_SPIN3);
		SendMessage(w, UDM_SETRANGE32, 100, 192000);
		SendMessage(w, UDM_SETPOS32, 0, sample_rate);
		w = GetDlgItem(hdwnd, IDC_SPIN2);
		SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(256, 0));
		SendMessage(w, UDM_SETPOS, 0, loop_count);
		w = GetDlgItem(hdwnd, IDC_SPIN1);
		SendMessage(w, UDM_SETRANGE32, 0, 60000);
		SendMessage(w, UDM_SETPOS32, 0, fade_mseconds);

		EnableDialogWindow(FALSE);
		return 1;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			bool separateChannels = IsDlgButtonChecked(hdwnd, IDC_EXPORTSEPARATE);
			char str[10] = { NULL };
			GetDlgItemText(hdwnd, IDE_SAMPLE_RATE, str, 7);
			sample_rate = atol(str);
			GetDlgItemText(hdwnd, IDE_LOOP_COUNT, str, 7);
			loop_count = atol(str);
			GetDlgItemText(hdwnd, IDE_FADE_MSECONDS, str, 7);
			fade_mseconds = atol(str);

			if (sample_rate < 1000 || sample_rate > 192000) {
				msgbox(hdwnd, IDS_STRING122, IDS_ERROR, MB_OK); // Not in range
				return 1;
			}
			if (loop_count < 0) {
				msgbox(hdwnd, IDS_STRING121, IDS_ERROR, MB_OK);
				return 1;
			}
			if (fade_mseconds < 0) fade_mseconds = 0;

			res = GetFileNameExportWav(hdwnd, MessageString[IDS_STRING124], strPath); // "WAV Export"
			if (res == MSGCANCEL) return 1;
			if (res == MSGEXISFILE) {
				if (msgbox(hdwnd, IDS_NOTIFY_OVERWRITE, IDS_INFO_SAME_FILE, MB_YESNO | MB_ICONEXCLAMATION) == IDNO) return 1;
			}

			EndDialog(hdwnd, 0);

			org_data.GetMusicInfo(&mi, 1);
			unsigned int samples = ((mi.wait * sample_rate) / 1000) * (mi.end_x + ((mi.end_x - mi.repeat_x) * loop_count)) + (fade_mseconds * sample_rate / 1000);
			
			if (separateChannels) {
				for (int i = 0; i < MAXTRACK; ++i) {
					if (mi.tdata[i].note_list == NULL) {
						continue;
					}

					char* pC;
					char tstr[15] = { NULL };
					char chPath[MAX_PATH] = { NULL };

					snprintf(chPath, MAX_PATH, "%s", strPath);
					snprintf(tstr, 15, " - Track %c.wav", TrackN[i]);

					pC = strrchr(chPath, '.');
					if (pC != NULL) pC[0] = '\0';

					strncat(chPath, tstr, MAX_PATH - 1);

					SetExportChannel(i);

					int res = ExportWave(samples, chPath);
					if (res != 0) {
						if (res == 1) { // error
							msgbox(hdwnd, IDS_STRING121, IDS_STRING120, MB_OK);
						}

						// end now
						SetExportChannel(-1);
						EnableDialogWindow(TRUE);
						return 1;
					}
				}

				SetExportChannel(-1);
			}

			int res = ExportWave(samples, strPath);
			if (res != 0) {
				if (res == 1) { // error
					msgbox(hdwnd, IDS_STRING121, IDS_STRING120, MB_OK);
				}
				break;
			}

			EnableDialogWindow(TRUE);
			return 1;
		}
		case IDCANCEL:
			EnableDialogWindow(TRUE);
			EndDialog(hdwnd, 0);
			return 1;
		}
		return 1;
	case WM_NOTIFY: {
		lpnm = (LPNMHDR)lParam;
		switch (lpnm->code) {
		case UDN_DELTAPOS: {
			LPNMUPDOWN lud = (LPNMUPDOWN)lParam;
			HWND buddy = (HWND)SendMessage(lpnm->hwndFrom, UDM_GETBUDDY, 0, 0);
			if (buddy) {
				int v = GetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), NULL, FALSE);

				switch (lpnm->idFrom) {
				case IDC_SPIN3:
					v += lud->iDelta * 100;
					if (v < 100) v = 100;
					if (v > 192000) v = 192000;

					SetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), v, FALSE);
					SendMessage(lpnm->hwndFrom, UDM_SETPOS32, 0, v);
					break;
				case IDC_SPIN2:
					v += lud->iDelta;
					if (v < 0) v = 0;
					if (v > 256) v = 256;

					SetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), v, FALSE);
					SendMessage(lpnm->hwndFrom, UDM_SETPOS, 0, v);
					break;
				case IDC_SPIN1:
					v += lud->iDelta * 100;
					if (v < 0) v = 0;
					if (v > 60000) v = 60000;

					SetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), v, FALSE);
					SendMessage(lpnm->hwndFrom, UDM_SETPOS32, 0, v);
					break;
				}

				return 0;
			}
			return 1;
		}
		}
	}
	}
	return 0;
}

BOOL CALLBACK DialogDecayLength(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR lpnm;

	switch (message) {
	case WM_INITDIALOG: {
		char str[10] = { NULL };
		itoa(volChangeLength, str, 10);
		SetDlgItemText(hdwnd, IDE_VOL_LENGTH, str);
		CheckDlgButton(hdwnd, IDC_VOL_USENOTELEN, volChangeUseNoteLength ? 1 : 0);
		CheckDlgButton(hdwnd, IDC_VOL_SETNOTELEN, volChangeSetNoteLength ? 1 : 0);

		HWND w = GetDlgItem(hdwnd, IDC_SPINDECAY);
		SendMessage(w, UDM_SETRANGE, 0, MAKELPARAM(9999, 1));
		SendMessage(w, UDM_SETPOS, 0, volChangeLength);

		EnableDialogWindow(FALSE);
		return 1;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			char str[10] = { NULL };

			GetDlgItemText(hdwnd, IDE_VOL_LENGTH, str, 7);
			volChangeLength = atol(str);
			volChangeLength = volChangeLength < 1 ? 1 : volChangeLength;

			volChangeUseNoteLength = IsDlgButtonChecked(hdwnd, IDC_VOL_USENOTELEN);
			volChangeSetNoteLength = IsDlgButtonChecked(hdwnd, IDC_VOL_SETNOTELEN);

			EnableDialogWindow(TRUE);
			EndDialog(hdwnd, 0);
			return 1;
		}
		case IDCANCEL:
			EnableDialogWindow(TRUE);
			EndDialog(hdwnd, 0);
			return 1;
		}
		return 1;
	case WM_NOTIFY: {
		lpnm = (LPNMHDR)lParam;
		switch (lpnm->code) {
		case UDN_DELTAPOS: {
			LPNMUPDOWN lud = (LPNMUPDOWN)lParam;
			HWND buddy = (HWND)SendMessage(lpnm->hwndFrom, UDM_GETBUDDY, 0, 0);
			if (buddy) {
				int v = GetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), NULL, FALSE);
				v += lud->iDelta;
				if (v < 1) v = 1;
				if (v > 9999) v = 9999;

				SetDlgItemInt(hdwnd, GetWindowLong(buddy, GWL_ID), v, FALSE);
				SendMessage(lpnm->hwndFrom, UDM_SETPOS, 0, v);
				return 0;
			}
			return 1;
		}
		}
	}
	}
	return 0;
}

/*bool CheckDefPanVol(HWND hdwnd, int WithSet)
{
	int t,a,b;
	for(t=0;t<MAXTRACK;t++){
		a = GetText(hdwnd , txt_Pan[t]);
		if(a > 12 || a < 0){
			//MessageBox(hdwnd,"0～12にしてください","ERROR(パン)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_VALUESET012,IDS_ERROR_PAN,MB_OK);	// 2014.10.19 A
			return false;
		}
		b = GetText(hdwnd , txt_Vol[t]);
		if(b > 254 || b < 0){
			//MessageBox(hdwnd,"0～254にしてください","ERROR(ボリューム)",MB_OK);	// 2014.10.19 D
			msgbox(hdwnd,IDS_VALUESET0254,IDS_ERROR_VOLUME,MB_OK);	// 2014.10.19 A
			return false;
		}
	}
	if(WithSet>0){
		for(t=0;t<MAXTRACK;t++){
			a = GetText(hdwnd , txt_Pan[t]);
			b = GetText(hdwnd , txt_Vol[t]);
			org_data.def_pan[t] = (unsigned char)a;
			org_data.def_volume[t] = (unsigned char)b;
		}
	}
	return true;
}*/

/*bool LoadPVIFile(HWND hdwnd, char* FileName)
{
	FILE *fp;
	fp = fopen(FileName, "rt");
	if(fp==NULL) return false;
	int t,r;
	for(t=0;t<MAXTRACK;t++){
		fscanf(fp,"%d",&r);
		SetText(hdwnd,txt_Pan[t],r);
		fscanf(fp,"%d",&r);
		SetText(hdwnd,txt_Vol[t],r);
	}
	fclose(fp);
	return true;
}


bool SavePVIFile(HWND hdwnd, char *FileName)
{
	FILE *fp;
	fp = fopen(FileName, "wt");
	if(fp==NULL) return false;
	int t,r;
	for(t=0;t<MAXTRACK;t++){
		r = GetText(hdwnd , txt_Pan[t]);
		fprintf(fp,"%d\n",r);
		r = GetText(hdwnd , txt_Vol[t]);
		fprintf(fp,"%d\n",r);
	}
	fclose(fp);
	return true;
}

bool GetSetPVInitFile(HWND hdwnd, int IsSave, int iQuiet)
{
	OPENFILENAME ofn;
	FILE *fp;
	char init_file[512];
	int ml;
	strcpy(init_file, music_file);
	ml = strlen(init_file);
	if(ml>4){
		if(init_file[ml-4]=='.'){ //pvi付加
			//init_file[ml-4]='.';
			init_file[ml-3]='p';
			init_file[ml-2]='v';
			init_file[ml-1]='i';
		}
	}

	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner   = hdwnd;
	ofn.hInstance   = hInst;
	//ofn.lpstrFilter = "パン／ボリューム初期値データ[*.pvi]\0*.pvi\0全ての形式 [*.*]\0*.*\0\0"; //パン／ボリューム初期値データ[*.pvi]\0*.pvi\0全ての形式 [*.*]\0*.*\0\0	// 2014.10.19 D
	ofn.lpstrFilter = MessageString[IDS_STRING108]; //パン／ボリューム初期値データ[*.pvi]\0*.pvi\0全ての形式 [*.*]\0*.*\0\0	// 2014.10.19 A
	ofn.lpstrFile   = init_file;
	ofn.nMaxFile    = MAX_PATH;
	ofn.lpstrDefExt = "pvi";
	
	if(IsSave>0){ //セーブ
		ofn.Flags       = OFN_CREATEPROMPT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
		ofn.lpstrTitle  = MessageString[IDS_STRING65]; //パン／ボリューム初期値データの保存
		if(GetSaveFileName(&ofn));	//InvalidateRect(hwnd,NULL,TRUE);
		else return false;			//キャンセルで0が返る
		
	}else{			//ロード
		ofn.Flags       = OFN_HIDEREADONLY;
		ofn.lpstrTitle  = MessageString[IDS_STRING66]; //パン／ボリューム初期値データの読み込み
		if(GetOpenFileName(&ofn));	//InvalidateRect(hwnd,NULL,TRUE);
		else return false;			//キャンセルで0が返る
	}
	fp = fopen(init_file,"rb");		//検査
	if(fp == NULL){
		if(IsSave==0){
			//if(iQuiet == 0) MessageBox(hdwnd,"ファイルにアクセスできません","",MB_OK);	// 2014.10.19 D
			if(iQuiet == 0) msgbox(hdwnd,IDS_WARNING_ACCESS_FILE,IDS_ERROR,MB_OK);	// 2014.10.19 A
			return false;			//指定ファイルが存在しない
		}
	}else{
		fclose(fp);
	}
	bool b;
	if(IsSave==0){
		b = LoadPVIFile(hdwnd, init_file);
	}else{
		b = SavePVIFile(hdwnd, init_file);
	}
	return b;
	
}



BOOL CALLBACK DialogDefault(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	char str[32] = {NULL};
	int i,a,b;
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		//リピート範囲の初期化//////////////////
		/*
		itoa(org_data.def_pan,str,10);
		SetDlgItemText(hdwnd,IDD_DEFPAN,str);
		itoa(org_data.def_volume,str,10);
		SetDlgItemText(hdwnd,IDD_DEFVOLUME,str);
		*
		for(i=0;i<MAXTRACK;i++){
			SetText(hdwnd, txt_Pan[i] , (int) org_data.def_pan[i] );
			SetText(hdwnd, txt_Vol[i] , (int) org_data.def_volume[i] );
			
		}
		EnableDialogWindow(FALSE);
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDC_PANPOT1: case IDC_PANPOT2: case IDC_PANPOT3: case IDC_PANPOT4: case IDC_PANPOT5: case IDC_PANPOT6: case IDC_PANPOT7: case IDC_PANPOT8: case IDC_PANPOT9: case IDC_PANPOT10: case IDC_PANPOT11: case IDC_PANPOT12: case IDC_PANPOT13: case IDC_PANPOT14: case IDC_PANPOT15: case IDC_PANPOT16:
		case IDC_VOLUME1: case IDC_VOLUME2: case IDC_VOLUME3: case IDC_VOLUME4: case IDC_VOLUME5: case IDC_VOLUME6: case IDC_VOLUME7: case IDC_VOLUME8: case IDC_VOLUME9: case IDC_VOLUME10: case IDC_VOLUME11: case IDC_VOLUME12: case IDC_VOLUME13: case IDC_VOLUME14: case IDC_VOLUME15: case IDC_VOLUME16:
			if(HIWORD(wParam) == EN_SETFOCUS)PostMessage(GetDlgItem(hdwnd, LOWORD(wParam)), EM_SETSEL, 0, -1); //フォーカス時にテキストを全選択する	// 2014.10.19 
			return 1;
		case IDC_DEF_NOWLOAD:
			GetSetPVInitFile(hdwnd, 0, 0);
			break;
		case IDC_DEF_NOWSAVE:
			GetSetPVInitFile(hdwnd, 1, 0);
			break;
		case IDCANCEL:
			EndDialog(hdwnd,0);
			EnableDialogWindow(TRUE);
			return 1;
		case IDC_BTN_SORT:
			a = GetText(hdwnd,txt_Pan[0]);
			b = GetText(hdwnd,txt_Vol[0]);
			for(i=0;i<MAXTRACK;i++){
				SetText(hdwnd, txt_Pan[i] , a );
				SetText(hdwnd, txt_Vol[i] , b );
			}			
			break;
		case IDC_PRESET1:
			for(i=0;i<MAXTRACK;i++){
				SetText(hdwnd, txt_Pan[i] , Preset1_Pan[i] );
				SetText(hdwnd, txt_Vol[i] , Preset1_Vol[i] );
			}
			break;
		case IDC_PRESET2:
			for(i=0;i<MAXTRACK;i++){
				b = GetText(hdwnd,txt_Vol[i]);
				SetText(hdwnd, txt_Vol[i] , (b*95/100) );
			}
			break;
		case IDC_PRESET3:
			for(i=0;i<MAXTRACK;i++){
				a = (int)(GetNormDistRand() * 2.0 + 6.0); a = (a < 0) ? 0 : ((a > 12) ? 12 : a);
				SetText(hdwnd, txt_Pan[i] , a );
			}
			break;
		case IDC_PRESET4:
			for(i=0;i<MAXTRACK;i++){
				b = (int)(GetNormDistRand() * 18.0 + 185.0); b = (b < 0) ? 0 : ((b > 253) ? 253 : b);;
				SetText(hdwnd, txt_Vol[i] , b );
			}
			break;
		case IDC_PRESET5:
			for(i=0;i<MAXTRACK;i++){
				SetText(hdwnd, txt_Pan[i] , 6 );
				SetText(hdwnd, txt_Vol[i] , 200 );
			}
			break;
		case IDOK:
			/*
			//パンデフォルト値
			GetDlgItemText(hdwnd,IDD_DEFPAN,str,7);
			i = atol(str);
			if(i > 12 || i < 0){
				MessageBox(hdwnd,"0～12にしてください","ERROR(パン)",MB_OK);
				return FALSE;
			}
			org_data.def_pan = (unsigned char)i;
			//パンデフォルト値
			GetDlgItemText(hdwnd,IDD_DEFVOLUME,str,7);
			i = atol(str);
			if(i > 254 || i < 0){
				MessageBox(hdwnd,"0～254にしてください","ERROR(ボリューム)",MB_OK);
				return FALSE;
			}
			org_data.def_volume = (unsigned char)i;
			*
			if( !CheckDefPanVol(hdwnd, 1) )return FALSE; //検査とセッティング
			EndDialog(hdwnd,0);
			EnableDialogWindow(TRUE);
			return 1;
		}
	}
	return 0;
}*/
BOOL CALLBACK DialogHelp(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HRSRC hrscr;
	HWND haDlg;
	MINMAXINFO *pmmi;
	//DWORD *lpdword;//リソースのアドレス
	char *cc,*ce,*cf;
	//int i;
	int DlgWidth, DlgHeight;

	switch(message){
	case WM_INITDIALOG: {//ダイアログが呼ばれた
		//EnableDialogWindow(FALSE); //D 2014.05.25
		hrscr = FindResource(NULL, "TEXTHELP", "TEXT");// リソースの検索
		if (hrscr)
		{
			DWORD len = SizeofResource(NULL, hrscr);

			char* text = (char*)malloc(len + 1);
			if (text == NULL) {
				// Ooops!
				return 0;
			}

			cc = (char*)LockResource(LoadResource(NULL, hrscr));// リソースのアドレスを取得
			memcpy(text, cc, len);
			text[len] = '\0';

			SetDlgItemText(hdwnd, IDC_HELPTEXT, text);
			free(text);
		}

		SendMessage(GetDlgItem(hdwnd, IDC_HELPTEXT), EM_SETSEL, -1, 0);
		SendMessage(GetDlgItem(hdwnd, IDC_HELPTEXT), EM_SCROLLCARET, 0, 0);

		return 1;
	}
	case WM_SIZE:
		DlgWidth  = LOWORD(lParam);	//クライアント領域のサイズ
		DlgHeight = HIWORD(lParam);
		haDlg = GetDlgItem(hdwnd, IDC_HELPTEXT);
		SetWindowPos(haDlg, HWND_TOP, 1, 1, DlgWidth-4, DlgHeight-36, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER );
		
		return 1;
	case WM_GETMINMAXINFO:
		pmmi = (MINMAXINFO *)lParam;
		if ( pmmi )
		{
			pmmi->ptMinTrackSize.x = 550;  // 最小幅
			pmmi->ptMinTrackSize.y = 200;  // 最小高
		}
		return 1;
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case IDCANCEL:
			//EnableDialogWindow(TRUE); //D 2014.05.25
			//EndDialog(hdwnd,0);
			ShowWindow(hdwnd, SW_HIDE);
			return 1;
		}
		return 1;
	
	}
	return 0;
}
