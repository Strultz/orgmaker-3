#include <stdio.h>

#include "Setting.h"
#include "DefOrg.h"
#include "resource.h"
#include "OrgData.h"
#include "Gdi.h"
#include "Timer.h"
#include "Sound.h"
#include "Scroll.h"

extern HWND hDlgTrack;
extern HWND hWnd;
extern char gSelectedTheme[MAX_PATH];

int mute_name[MAXTRACK] = {
	IDC_MUTE0,
	IDC_MUTE1,
	IDC_MUTE2,
	IDC_MUTE3,
	IDC_MUTE4,
	IDC_MUTE5,
	IDC_MUTE6,
	IDC_MUTE7,
	IDC_MUTE8,
	IDC_MUTE9,
	IDC_MUTE10,
	IDC_MUTE11,
	IDC_MUTE12,
	IDC_MUTE13,
	IDC_MUTE14,
	IDC_MUTE15,
};

void LoadPlayerBitmaps(HWND hdwnd) {
	LoadSingleBitmap(hdwnd, IDC_START, 24, 24, "B_HEAD");
	LoadSingleBitmap(hdwnd, IDC_END, 24, 24, "B_END");
	LoadSingleBitmap(hdwnd, IDC_PLAY, 40, 24, "B_PLAY");
	LoadSingleBitmap(hdwnd, IDC_STOP, 40, 24, "B_STOP");
	LoadSingleBitmap(hdwnd, IDC_LEFT, 24, 24, "B_LEFT");
	LoadSingleBitmap(hdwnd, IDC_RIGHT, 24, 24, "B_RIGHT");
	LoadSingleBitmap(hdwnd, IDC_LEFTSTEP, 24, 24, "B_LEFTSTEP");
	LoadSingleBitmap(hdwnd, IDC_RIGHTSTEP, 24, 24, "B_RIGHTSTEP");
}
//簡単なダイアログ関数
BOOL CALLBACK DialogPlayer(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam){
	int i;
	HMENU hmenu,hsubmenu;
	MUSICINFO mi;
	HFONT hFont1;
	HWND  hCtrlWnd;
	long hp,vp;
	char str[32];
	switch(message){
	case WM_INITDIALOG://ダイアログが呼ばれた
		org_data.GetMusicInfo(&mi);
		itoa(mi.wait,str,10);
		SetDlgItemText(hdwnd,IDE_VIEWWAIT,str);
		org_data.SetPlayPointer(0);//頭出し
		//プレイヤーに表示
		//hFont1 = CreateFont(0, 0, 0, 0, 900, 0, 0, 0, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "ＭＳ ゴシック");
		hFont1 = CreateFont(18, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY, DEFAULT_PITCH, "Arial Black");
		hCtrlWnd = GetDlgItem(hdwnd, IDE_VIEWMEAS);
		SendMessage(hCtrlWnd, WM_SETFONT, (WPARAM)hFont1, MAKELPARAM(FALSE, 0)); /* フォントの変更 */
		SetDlgItemText(hdwnd,IDE_VIEWMEAS,"0");

		hCtrlWnd = GetDlgItem(hdwnd, IDE_VIEWXPOS);
		SendMessage(hCtrlWnd, WM_SETFONT, (WPARAM)hFont1, MAKELPARAM(FALSE, 0)); /* フォントの変更 */
		SetDlgItemText(hdwnd,IDE_VIEWXPOS,"0");
		DeleteObject(hFont1); //フォントオブジェクトの削除

		//画像張りつけ
/*
		hBmp = (HBITMAP)LoadImage( hInst, "HEAD", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_START, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "PLAY", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_PLAY, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "STOP", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_STOP, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "LEFT", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_LEFT, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
		hBmp = (HBITMAP)LoadImage( hInst, "RIGHT", IMAGE_BITMAP, 32, 16, LR_DEFAULTCOLOR );
		SendDlgItemMessage( hdwnd, IDC_RIGHT, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp );
*/
		LoadPlayerBitmaps(hdwnd);
		return 1;
	case WM_MOUSEMOVE:
		if(wParam & MK_LBUTTON){
			ReleaseCapture();
			SendMessage( hdwnd, WM_SYSCOMMAND, SC_MOVE | 2, 0 );

		}
	case WM_COMMAND:
		switch(LOWORD(wParam)){
		}
	case WM_CLOSE:
		//DeleteObject(hFont1); //フォントオブジェクトの削除
		return 1;
	}
	return 0;
}