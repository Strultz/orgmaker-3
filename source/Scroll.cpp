#include "Setting.h" 
#include "DefOrg.h"
#include "Scroll.h"
#include "OrgData.h"
#include <stdio.h>
#include "resource.h"
//�X�N���[���o�[�̈ړ��l
#define MAXVERTRANGE	(72-12)//8�I�N�^�[�u

#define BUF_SIZE 256
#define MAIN_WINDOW "WINDOW"


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
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);//��
	scr_info.cbSize = sizeof(SCROLLINFO);
	scr_info.fMask = SIF_RANGE | SIF_PAGE;
	scr_info.nMax = MAXVERTRANGE;
	scr_info.nPage = 4;
	SetScrollInfo(hWnd,SB_VERT,&scr_info,1);//�c
	hpos = 0;//�����X�N���[���l
	//vpos = MAXVERTRANGE-27;//���������l
	//vpos = GetPrivateProfileInt(MAIN_WINDOW,"VPOS",MAXVERTRANGE-27,app_path);
	vpos = GetPrivateProfileInt(MAIN_WINDOW,"VPOS",MAXVERTRANGE-27,app_path);

	//�ȉ���vpos��L���ɂ��鏈��
	scr_info.fMask = SIF_POS;
	scr_info.nPos = vpos;
	SetScrollInfo(hWnd,SB_VERT,&scr_info,1);

	return TRUE;
}
void ScrollData::ChangeVerticalRange(int WindowHeight){ //�E�B���h�E�T�C�Y�����ɃX�N���[���\���ݒ�
	if(WindowHeight>0){
		int ap;
		ap = (WindowHeight - 158)/12;

		scr_info.nMax = 100 - ap;
		vScrollMax = scr_info.nMax;
		if (vScrollMax < 0) vScrollMax = 0;
	}else{
		scr_info.nMax = MAXVERTRANGE;

	}
	scr_info.cbSize = sizeof(SCROLLINFO);
	scr_info.fMask = SIF_RANGE;
	SetScrollInfo(hWnd, SB_VERT, &scr_info, TRUE);//�c
	return;
}
void ScrollData::ChangeHorizontalRange(int range) { //�E�B���h�E�T�C�Y�����ɃX�N���[���\���ݒ�
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
	//RECT rect = {0,0,WWidth,WHeight};//�X�V����̈�
	hpos = x;
	if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
	scr_info.fMask = SIF_POS;//nPos��L����
	scr_info.nPos = hpos;
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);
	UpdateStatusBar();
	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
}

void ScrollData::PrintHorzPosition(void)
{
	/*MUSICINFO mi;
	org_data.GetMusicInfo(&mi);
	
	char str[12];
	itoa(hpos / (mi.dot * mi.line),str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,str);
	itoa(hpos % (mi.dot * mi.line),str,10);
	SetDlgItemText(hDlgPlayer,IDE_VIEWXPOS,str);*/

	UpdateStatusBar();

}

void ScrollData::HorzScrollProc(WPARAM wParam){
	RECT rect = {0,0,WWidth,WHeight};//�X�V����̈�
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);

	switch(LOWORD(wParam)){
	case SB_LINERIGHT://�E��
		hpos++;
		if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
		if (hpos < 0)hpos = 0;
		break;
	case SB_LINELEFT://����
		hpos--;
		if(hpos < 0)hpos = 0;
		break;
	case SB_THUMBPOSITION:
		hpos = HIWORD(wParam);//���݈ʒu���擾
		break;
	case SB_THUMBTRACK:
		hpos = HIWORD(wParam);//���݈ʒu���擾
		break;
	case SB_PAGERIGHT://�E��
		hpos = (hpos / (mi.dot * mi.line) + 1) * (mi.dot * mi.line);
		if(hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
		if (hpos < 0)hpos = 0;
		break;
	case SB_PAGELEFT://����
	{
		int mk = (mi.dot * mi.line);
		int cap = (hpos / mk) * mk;
		if (hpos > cap) hpos = cap;
		else hpos = cap - mk;

		if (hpos < 0)hpos = 0;
		break;
	}

	}
	//�v���C���[�ɔ��f
	PrintHorzPosition();
	if(timer_sw == 0){
		org_data.SetPlayPointer(hpos);
	}
	scr_info.fMask = SIF_POS;//nPos��L����
	scr_info.nPos = hpos;
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);
	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	//�ȉ��̓e�X�g�p
//	char str[80];
//	HDC hdc;
//	hdc = GetDC(hWnd);
//	sprintf(str,"Horizontal:%4d",hpos);
//	TextOut(hdc,200,1,str,strlen(str));
//	ReleaseDC(hWnd,hdc);
}
void ScrollData::VertScrollProc(WPARAM wParam){
	RECT rect = {0,0,WWidth,WHeight};//�X�V����̈�
	switch(LOWORD(wParam)){
	case SB_LINEDOWN://����
		vpos++;
		if(vpos > vScrollMax)vpos = vScrollMax;
		if (vpos < 0)vpos = 0;
		break;
	case SB_LINEUP://���
		vpos--;
		if(vpos < 0)vpos = 0;
		break;
	case SB_THUMBPOSITION:
		vpos = HIWORD(wParam);//���݈ʒu���擾
		break;
	case SB_THUMBTRACK:
		vpos = HIWORD(wParam);//���݈ʒu���擾
		break;
	case SB_PAGEDOWN://����
		vpos += 12;
		if(vpos > vScrollMax)vpos = vScrollMax;
		if (vpos < 0)vpos = 0;
		break;
	case SB_PAGEUP://���
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
	case SB_PAGEDOWN://����
		vpos += 6;
		if(vpos > MAXVERTRANGE)vpos = MAXVERTRANGE;
		break;
	case SB_PAGEUP://���
		vpos -= 6;
		if(vpos < 0)vpos = 0;
		break;
	}
	*/
	
	if(zDelta<0){
		if(fwKeys && MK_CONTROL){
			hpos += 4;
			if (hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
			if (hpos < 0)hpos = 0;
		}else{
			vpos+=4;
			if(vpos > vScrollMax)vpos = vScrollMax;
			if (vpos < 0)vpos = 0;
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
	scr_info.fMask = SIF_POS;//nPos��L����
	scr_info.nPos = vpos;
	SetScrollInfo(hWnd,SB_VERT,&scr_info,1);
	scr_info.fMask = SIF_POS;//nPos��L����
	scr_info.nPos = hpos;
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);

	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);


}

void ScrollData::KeyScroll(int iDirection)
{
	RECT rect = {0,0,WWidth,WHeight};//�X�V����̈�
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
	if (hpos > MAXHORZRANGE)hpos = MAXHORZRANGE;
	if (vpos > vScrollMax)vpos = vScrollMax;
	if (hpos < 0)hpos = 0;
	if (vpos < 0)vpos = 0;

	PrintHorzPosition();
	scr_info.fMask = SIF_POS;//nPos��L����
	scr_info.nPos = vpos;
	SetScrollInfo(hWnd,SB_VERT,&scr_info,1);
	scr_info.fMask = SIF_POS;//nPos��L����
	scr_info.nPos = hpos;
	SetScrollInfo(hWnd,SB_HORZ,&scr_info,1);

	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

}

