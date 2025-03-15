#include "Setting.h"
#include "DefOrg.h"
//#include <string.h>
//#include <stdio.h>
#include "resource.h"
#include "OrgData.h"
#include "Gdi.h"
#include "Timer.h"
#include "Sound.h"
#include <string.h>
#include "Click.h"
#include "Scroll.h"
#include "rxoFunction.h"
#include <stdio.h>

extern int mute_name[];

extern char timer_sw;
extern HWND hWnd;
//extern HWND hDlgEZCopy;
//int EZCopyWindowState;
extern int gDrawDouble;	//�����̃g���b�N�O���[�v��`�悷��
extern int iDragMode;

int Bn[]={
	IDC_CTB1,
	IDC_CTB2,
	IDC_CTB3,
	IDC_CTB4,
	IDC_CTB5,
	IDC_CTB6,
	IDC_CTB7,
	IDC_CTB8,
	IDC_CTB9,
	IDC_CTB10,
	IDC_CTB11,
	IDC_CTB12,
};

int Hni[]={
	1,2,3,4,8,16,
	1,2,3,4,8,16,	
};

extern NOTECOPY nc_Select;
extern int tra, ful, haba;
int sGrid = 0;	//�͈͑I���̓O���b�h�P�ʂ�
int sACrnt = 0;	//�͈͑I���͏�ɶ����ׯ�

char CpHelp[512];
void ShowStatusMessage(void);


char TrackN[]="12345678QWERTYUI";




BOOL CALLBACK DialogEZCopy(HWND hdwnd, UINT message, WPARAM wParam, LPARAM lParam){
	RECT rect = {64,0,WWidth,WHeight};//�X�V����̈�(�g���b�N�ύX)
	int i,bt;
	long scr_h,scr_v;
	MUSICINFO mi;
	switch(message){
	case WM_INITDIALOG://�_�C�A���O���Ă΂ꂽ
		return 1;
	case WM_CLOSE:
		ShowWindow(hdwnd, SW_HIDE);
		//EZCopyWindowState = 0;
		return 1;
	case WM_RBUTTONDOWN:
		SelectReset();
		//org_data.PutMusic();
		//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
		break;
	case WM_MOUSEMOVE:
		if(wParam & MK_LBUTTON){
			ReleaseCapture();
			SendMessage( hdwnd, WM_SYSCOMMAND, SC_MOVE | 2, 0 );

		}
		break;
	case WM_COMMAND:
		if(timer_sw==0){
			bt=-1;
			for(i=0;i<12;i++){
				if(LOWORD(wParam) == Bn[i])bt=i;
			}
			if(bt>=0){ //�{�P�Ƃ��̃{�^���������ꂽ�B
				int a,b,d,bb;
				//char str[5];
				org_data.GetMusicInfo(&mi);
				scr_data.GetScrollPosition(&scr_h,&scr_v);
				//GetDlgItemText(hDlgPlayer,IDE_VIEWMEAS,str,4);//�͈�from
				//b = atol(str);
				b = scr_h / (mi.dot * mi.line);
				bb = b + Hni[bt];
				haba = Hni[bt];
				a = b * mi.dot * mi.line; //�J�n�_
				d = bb* mi.dot * mi.line - 1; //�I���_
				nc_Select.x1_1 = a;
				nc_Select.x1_2 = d;
				if(bt>=6)ful=1;else ful=0; //�S�g���b�N�H
				tra = org_data.track;
				//org_data.CopyNoteDataToCB(&nc_Select, tra, ful);
				//if(ful==0)wsprintf(CpHelp,"�g���b�N%c ��%d���߁`%d���߂܂ł�",TrackN[tra],b,bb);	// 2014.10.19 D
				if(ful==0)wsprintf(CpHelp,MessageString[IDS_STRING74],TrackN[tra],b,bb);	// 2014.10.19 A
				//else wsprintf(CpHelp,"�S�g���b�N��%d���߁`%d���߂܂ł�",b,bb);	// 2014.10.19 D
				else wsprintf(CpHelp,MessageString[IDS_STRING75],b,bb);	// 2014.10.19 A
				//org_data.PutMusic();
				//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
			}
			switch(LOWORD(wParam)){
			case IDC_PST1:
				SetUndo();
				//EZ_Paste(1);
				break;
			case IDC_PST2:
				SetUndo();
				//EZ_Paste(2);
				break;
			case IDC_PST3:
				SetUndo();
				//EZ_Paste(3);
				break;
			case IDC_PST4:
				SetUndo();
				//EZ_Paste(4);
				break;
			case IDC_DELETEBUTTON:
				SetUndo();
				//EZ_Delete();
				break;
			case IDC_INSERTBUTTON:
				SetUndo();
				//EZ_Insert();
				break;
			case IDC_DELETEBUTTON_2:
				SetUndo();
				//EZ_DeleteAndTrim();
				break;
			case IDC_BTNCLOSE:
				ShowWindow(hdwnd, SW_HIDE);
				//EZCopyWindowState = 0;
				return 1;

				break;
			}
		}
		ShowStatusMessage();
		SetFocus(hWnd);
		return 1;
	}
	return 0;
}

