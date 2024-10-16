#include "Setting.h" 
#include "DefOrg.h"
#include "Scroll.h"
#include "OrgData.h"
#include <stdio.h>
#include "resource.h"
//スクロールバーの移動値
#define MAXVERTRANGE	(72-12)//8オクターブ

#define BUF_SIZE 256
#define MAIN_WINDOW "WINDOW"


extern HWND hDlgPlayer;
extern char timer_sw;

extern CHAR app_path[BUF_SIZE];
extern CHAR num_buf[BUF_SIZE];

BOOL ScrollData::InitScroll(void)
{
	scr_info.cbSize = sizeof(SCROLLINFO);
	scr_info.fMask = SIF_RANGE | SIF_PAGE;
	scr_info.nMin = 0;
	scr_info.nMax = MAXHORZRANGE;
	scr_info.nPage = 4;
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);//横
	scr_info.cbSize = sizeof(SCROLLINFO);
	scr_info.fMask = SIF_RANGE | SIF_PAGE;
	scr_info.nMax = MAXVERTRANGE;
	scr_info.nPage = 4;
	SetScrollInfo(hWnd,SB_VERT,&scr_info,1);//縦
	hpos = 0;//水平スクロール値
	//vpos = MAXVERTRANGE-27;//垂直初期値
	//vpos = GetPrivateProfileInt(MAIN_WINDOW,"VPOS",MAXVERTRANGE-27,app_path);
	vpos = GetPrivateProfileInt(MAIN_WINDOW,"VPOS",MAXVERTRANGE-27,app_path);

	//以下はvposを有効にする処理
	scr_info.fMask = SIF_POS;
	scr_info.nPos = vpos;
	SetScrollInfo(hWnd,SB_VERT,&scr_info,1);

	return TRUE;
}
void ScrollData::ChangeVerticalRange(int WindowHeight){ //ウィンドウサイズを元にスクロール可能域を設定
	if(WindowHeight>0){
		int ap;
		ap = (WindowHeight - 158)/12;

		scr_info.nMax = 100 - ap;
		vScrollMax = scr_info.nMax;
	}else{
		scr_info.nMax = MAXVERTRANGE;

	}
	scr_info.cbSize = sizeof(SCROLLINFO);
	scr_info.fMask = SIF_RANGE;
	SetScrollInfo(hWnd, SB_VERT, &scr_info, TRUE);//縦
	return;
}
void ScrollData::ChangeHorizontalRange(int range) { //ウィンドウサイズを元にスクロール可能域を設定
	MAXHORZRANGE = range;
	if (hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
	scr_info.cbSize = sizeof(SCROLLINFO);
	scr_info.fMask = SIF_RANGE | SIF_PAGE;
	scr_info.nMin = 0;
	scr_info.nMax = MAXHORZRANGE;
	scr_info.nPage = 4;
	SetScrollInfo(hWnd, SB_HORZ, &scr_info, 1);
}
void ScrollData::AttachScroll(void)
{
	
}
void ScrollData::SetIniFile()
{
	wsprintf(num_buf,"%d",vpos);
	WritePrivateProfileString(MAIN_WINDOW,"VPOS",num_buf,app_path);

}

void ScrollData::SetHorzScroll(long x)
{
	//RECT rect = {0,0,WWidth,WHeight};//更新する領域
	hpos = x;
	if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
	scr_info.fMask = SIF_POS;//nPosを有効に
	scr_info.nPos = hpos;
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);
	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
}

void ScrollData::PrintHorzPosition(void)
{
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	
	char str[12];
	itoa(hpos / (mi.dot * mi.line),str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,str);
	itoa(hpos % (mi.dot * mi.line),str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWXPOS,str);

}

void ScrollData::HorzScrollProc(WPARAM wParam){
	RECT rect = {0,0,WWidth,WHeight};//更新する領域
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);

	switch(LOWORD(wParam)){
	case SB_LINERIGHT://右へ
		hpos++;
		if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
		break;
	case SB_LINELEFT://左へ
		hpos--;
		if(hpos < 0)hpos = 0;
		break;
	case SB_THUMBPOSITION:
		hpos = HIWORD(wParam);//現在位置を取得
		break;
	case SB_THUMBTRACK:
		hpos = HIWORD(wParam);//現在位置を取得
		break;
	case SB_PAGERIGHT://右へ
		hpos = (hpos / (mi.dot * mi.line) + 1) * (mi.dot * mi.line);
		if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
		break;
	case SB_PAGELEFT://左へ
	{
		int mk = (mi.dot * mi.line);
		int cap = (hpos / mk) * mk;
		if (hpos > cap) hpos = cap;
		else hpos = cap - mk;

		if (hpos < 0)hpos = 0;
		break;
	}

	}
	//プレイヤーに反映
	PrintHorzPosition();
	if(timer_sw == 0){
		org_data.SetPlayPointer(hpos);
	}
	scr_info.fMask = SIF_POS;//nPosを有効に
	scr_info.nPos = hpos;
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);
	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	//以下はテスト用
//	char str[80];
//	HDC hdc;
//	hdc = GetDC(hWnd);
//	sprintf(str,"Horizontal:%4d",hpos);
//	TextOut(hdc,200,1,str,strlen(str));
//	ReleaseDC(hWnd,hdc);
}
void ScrollData::VertScrollProc(WPARAM wParam){
	RECT rect = {0,0,WWidth,WHeight};//更新する領域
	switch(LOWORD(wParam)){
	case SB_LINEDOWN://下へ
		vpos++;
		if(vpos > vScrollMax)vpos = vScrollMax;
		break;
	case SB_LINEUP://上へ
		vpos--;
		if(vpos < 0)vpos = 0;
		break;
	case SB_THUMBPOSITION:
		vpos = HIWORD(wParam);//現在位置を取得
		break;
	case SB_THUMBTRACK:
		vpos = HIWORD(wParam);//現在位置を取得
		break;
	case SB_PAGEDOWN://下へ
		vpos += 12;
		if(vpos > vScrollMax)vpos = vScrollMax;
		break;
	case SB_PAGEUP://上へ
		vpos -= 12;
		if(vpos < 0)vpos = 0;
		break;
	}
	PrintHorzPosition();
	scr_info.fMask = SIF_POS;//Enable nPos
	scr_info.nPos = vpos;
	SetScrollInfo(hWnd,SB_VERT,&scr_info,1);
	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	//below is for testing
//	char str[80];
//	HDC hdc;
//	hdc = GetDC(hWnd);
//	sprintf(str,"Vertical:%4d",vpos);
//	TextOut(hdc,100,1,str,strlen(str));
//	ReleaseDC(hWnd,hdc);
}
//Get scroll position
void ScrollData::GetScrollPosition(long *hp,long *vp)
{
	*hp = hpos;
	*vp = vpos;
}

void ScrollData::WheelScrollProc(LPARAM lParam, WPARAM wParam){
	RECT rect = {0,0,WWidth,WHeight};//Area to update

	int fwKeys, zDelta, xPos,yPos;

	fwKeys = LOWORD(wParam);    // key flags  MK_CONTROL
	zDelta = (short) HIWORD(wParam);    // wheel rotation
	xPos = (short) LOWORD(lParam);    // horizontal position of pointer
	yPos = (short) HIWORD(lParam);    // vertical position of pointer

	/*
	switch(LOWORD(wParam)){
	case SB_LINEDOWN://Downwards
		vpos++;
		if(vpos > MAXVERTRANGE)vpos = MAXVERTRANGE;
		break;
	case SB_LINEUP://up
		vpos--;
		if(vpos < 0)vpos = 0;
		break;
	case SB_THUMBPOSITION:
		vpos = HIWORD(wParam);//Get current position
		break;
	case SB_THUMBTRACK:
		vpos = HIWORD(wParam);//Get current position
		break;
	case SB_PAGEDOWN://下へ
		vpos += 6;
		if(vpos > MAXVERTRANGE)vpos = MAXVERTRANGE;
		break;
	case SB_PAGEUP://上へ
		vpos -= 6;
		if(vpos < 0)vpos = 0;
		break;
	}
	*/
	
	if(zDelta<0){
		if(fwKeys && MK_CONTROL){
			hpos += 4;
			if (hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
		}else{
			vpos+=4;
			if(vpos > vScrollMax)vpos = vScrollMax;
		}
	}else{
		if(fwKeys && MK_CONTROL){
			hpos -= 4;
			if (hpos < 0)hpos = 0;
		}else{
			vpos-=4;
			if(vpos < 0)vpos = 0;
		}
	}
	PrintHorzPosition();
	scr_info.fMask = SIF_POS;//nPosを有効に
	scr_info.nPos = vpos;
	SetScrollInfo(hWnd,SB_VERT,&scr_info,1);
	scr_info.fMask = SIF_POS;//nPosを有効に
	scr_info.nPos = hpos;
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);

	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);


}

void ScrollData::KeyScroll(int iDirection)
{
	RECT rect = {0,0,WWidth,WHeight};//更新する領域
	switch(	iDirection ){
	case DIRECTION_UP:
		vpos--;
		break;
	case DIRECTION_DOWN:
		vpos++;
		break;
	case DIRECTION_LEFT:
		hpos--;
		break;
	case DIRECTION_RIGHT:
		hpos++;
		break;
	}
	if(hpos < 0)hpos = 0;
	if(vpos > vScrollMax)vpos = vScrollMax;
	if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
	if(vpos < 0)vpos = 0;

	PrintHorzPosition();
	scr_info.fMask = SIF_POS;//nPosを有効に
	scr_info.nPos = vpos;
	SetScrollInfo(hWnd,SB_VERT,&scr_info,1);
	scr_info.fMask = SIF_POS;//nPosを有効に
	scr_info.nPos = hpos;
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);

	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

}

