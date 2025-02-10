#include <string.h>
#include "Setting.h"
#include "DefOrg.h"
#include "Gdi.h"
#include "OrgData.h"
#include "Scroll.h"
#include "Sound.h"
#include "resource.h"

#include "rxoFunction.h"

extern int sACrnt;	//�͈͑I���͏�ɶ����ׯ�

extern char timer_sw;
extern int sSmoothScroll;

//�����\����������������������������������������������������������
RECT note_rect[] = {
	{  0,  0, 16,  6},//�L�[����(�ҏW)
	{  0,  6, 16, 12},//�L�[����
	{ 16,  0, 32,  4},//�K��(�ҏW)
	{ 16,  4, 32,  8},//�K��
	{ 16, 16, 32, 21},//�p���E�{�����[��
	{  0, 32, 16, 44},//Repeat(5)
	{ 16, 32, 32, 44},//end
};

RECT note_blue_rect[]={ //��������̃L�[�����i���j
	{ 0,  48, 16,  54},
	{16,  48, 32,  54},
	{ 0,  56, 16,  62},
	{16,  56, 32,  62},
	{ 0,  64, 16,  70},
	{16,  64, 32,  70},
	{ 0,  72, 16,  78},
	{16,  72, 32,  78},
	{ 0,  80, 16,  86},
	{16,  80, 32,  86},
	{ 0,  88, 16,  94},
	{16,  88, 32,  94},
	{ 0,  96, 16, 102},
	{16,  96, 32, 102},
	{ 0, 104, 16, 110},
	{16, 104, 32, 110},
	{ 0, 112, 16, 118},
	{16, 112, 32, 118},
	{ 0, 120, 16, 126},
	{16, 120, 32, 126},
	{ 0, 128, 16, 134},
	{16, 128, 32, 134},
	{ 0, 136, 16, 142},
	{16, 136, 32, 142},
	{ 0, 144, 16, 150},
	{16, 144, 32, 150},
	{ 0, 152, 16, 158},
	{16, 152, 32, 158},
	{ 0, 160, 16, 166},
	{16, 160, 32, 166},
	{ 0, 168, 16, 174},
	{16, 168, 32, 174},
};

/*
RECT note_tail_rect[]={ //������
	{32,   0, 64,   4},
	{32,   4, 64,   8},
	{32,   8, 64,  12},
	{32,  12, 64,  16},
	{32,  16, 64,  20},
	{32,  20, 64,  24},
	{32,  24, 64,  28},
	{32,  28, 64,  32},
	{32,  32, 64,  36},
	{32,  36, 64,  40},
	{32,  40, 64,  44},
	{32,  44, 64,  48},
	{32,  48, 64,  52},
	{32,  52, 64,  56},
	{32,  56, 64,  60},
	{32,  60, 64,  64},
	{32,  64, 64,  68}, //yobun
};
*/
RECT note_tail_rect[]={ //������
	{32,   0, 48,   4},
	{32,   4, 48,   8},
	{32,   8, 48,  12},
	{32,  12, 48,  16},
	{32,  16, 48,  20},
	{32,  20, 48,  24},
	{32,  24, 48,  28},
	{32,  28, 48,  32},
	{32,  32, 48,  36},
	{32,  36, 48,  40},
	{32,  40, 48,  44},
	{32,  44, 48,  48},
	{32,  48, 48,  52},
	{32,  52, 48,  56},
	{32,  56, 48,  60},
	{32,  60, 48,  64},
	{32,  64, 48,  68}, //yobun
};
RECT msc_rect[] = {
	{  0,  0, 64,144+16},//����
	{ 64,  0, 80,144+16},//���߃��C��
	{ 80,  0, 96,144+16},//�ꔏ���C��
	{ 96,  0,112,144+16},//1/16���C��
	{112+  0,  0,112+ 64,144+16},//����
	{112+ 64,  0,112+ 80,144+16},//���߃��C��
	{112+ 80,  0,112+ 96,144+16},//�ꔏ���C��
	{112+ 96,  0,112+112,144+16},//1/16���C��
};
RECT num_rect[] = {
	{  0,  0,  8, 12},
	{  8,  0, 16, 12},
	{ 16,  0, 24, 12},
	{ 24,  0, 32, 12},
	{ 32,  0, 40, 12},
	{ 40,  0, 48, 12},
	{ 48,  0, 56, 12},
	{ 56,  0, 64, 12},
	{ 64,  0, 72, 12},
	{ 72,  0, 80, 12},

	{  0, 12,  8, 24},
	{  8, 12, 16, 24},
	{ 16, 12, 24, 24},
	{ 24, 12, 32, 24},
	{ 32, 12, 40, 24},
	{ 40, 12, 48, 24},
	{ 48, 12, 56, 24},
	{ 56, 12, 64, 24},
	{ 64, 12, 72, 24},
	{ 72, 12, 80, 24},
};
RECT rc_SelArea[] ={
	{0, 176, 16, 187}, //�I��͈�
	{16, 176, 32, 187}, //�I��͈�
	{32, 176, 48, 187}, //�I��͈�
	{0, 192, 16, 203},  //�I��͈�(����Tr)
	{16, 192, 32, 203}, //�I��͈�(����Tr)
	{32, 192, 48, 203}, //�I��͈�(����Tr)
	{32, 160, 48, 171}, //FULL
	{32, 144, 48, 155}, //�P�}�X
};

RECT rc_CurrentSelect = {0, 208, 64, 224}; //2014.04.30
RECT rc_ActivePAN = {0, 224, 64, 296}; //2014.05.01 A
RECT rc_ActiveVOL = {0, 296, 64, 368}; //2014.05.01 A

RECT rc_TCPY[]={
	{32,88,48,94},
	{32,104,48,110},
	{32,96,48,100},
	{32,100,48,104},
};

RECT rc_PushKB[] ={    // 2010.09.23 A �����ꂽ����
	{112, 132, 208, 144}, //
	{112, 120, 208, 132}, //
	{112, 108, 208, 120}, //
	{112, 96, 208, 108}, //
	{112, 84, 208, 96}, //
	{112, 72, 208, 84}, //
	{112, 60, 208, 72}, //
	{112, 48, 208, 60}, //
	{112, 36, 208, 48}, //
	{112, 24, 208, 36}, //
	{112, 12, 208, 24}, //
	{112, 0, 208, 12}, //
};

RECT rc_PushKB2[] ={	// 2010.09.23 A �����ꂽ���Ձi���錮�Քw�i���j
	{112, 132, 176, 144}, //
	{112, 120, 176, 132}, //
	{112, 108, 176, 120}, //
	{112, 96, 176, 108}, //
	{112, 84, 176, 96}, //
	{112, 72, 176, 84}, //
	{112, 60, 176, 72}, //
	{112, 48, 176, 60}, //
	{112, 36, 176, 48}, //
	{112, 24, 176, 36}, //
	{112, 12, 176, 24}, //
	{112, 0, 176, 12}, //
};

RECT rc_PushKB3[] ={    // 2010.09.24 A �����ꂽ����(�I�N�^�[�u�Ⴂ�j
	{208, 132, 272, 144}, //
	{208, 120, 272, 132}, //
	{208, 108, 272, 120}, //
	{208, 96, 272, 108}, //
	{208, 84, 272, 96}, //
	{208, 72, 272, 84}, //
	{208, 60, 272, 72}, //
	{208, 48, 272, 60}, //
	{208, 36, 272, 48}, //
	{208, 24, 272, 36}, //
	{208, 12, 272, 24}, //
	{208, 0, 272, 12}, //
};


extern NOTECOPY nc_Select; //�I��͈�
extern int tra, ful ,haba; 
extern int gDrawDouble;	//�����̃g���b�N�O���[�v��`�悷��

extern int iKeyPushDown[256]; // 2010.09.22 A �s�A�m�L�[�̉�����


//�L�[�{�[�h�ƃs�A�m���[���̈ʒu
void OrgData::PutNumber(void)
{
	MUSICINFO mi;
	long scr_h,scr_v;
	GetMusicInfo(&mi);
	scr_data.GetScrollPosition(&scr_h, &scr_v);

	int k = info.dot * info.line;
	int v = (WWidth / NoteWidth) / k + (scr_h % k != 0 ? 1 : 0);

	int off = (-(scr_h % k) * NoteWidth);

	for (int i = 0; i <= v; i++) {
		long scalepos = scr_h / (mi.dot * mi.line);
		scalepos += i;
		if (scalepos < 0 || scalepos > 0x7FFFFFFF) continue;

		int digits = 3;
		int max = 1000;
		while (scalepos >= max) {
			max *= 10;
			digits++;
		}

		for (int j = 0; j < digits; ++j) {
			max /= 10;
			int num = scalepos / max;
			scalepos -= num * max;

			int x = k * i * NoteWidth + (j * 8) + KEYWIDTH + 1 + off;
			if (x >= 0) {
				PutBitmap(x, 0, &num_rect[num], BMPNUMBER);
				if (WHeight > 550) {
					PutBitmap(x, WHeight + 288 - WHNM - 12, &num_rect[num], BMPNUMBER);
				}
			}
		}
	}
}
void OrgData::PutRepeat(void)
{
	long scr_h;
	long x;
	scr_data.GetScrollPosition(&scr_h, NULL);
	x = (info.repeat_x - scr_h)*NoteWidth + KEYWIDTH;
	if (x >= 0)
		PutBitmap(x, WHeight + 276 - WHNM - (WHeight > 550 ? 12 : 0), &note_rect[5], BMPNOTE);
	x = (info.end_x - scr_h)*NoteWidth + KEYWIDTH;
	if (x >= 0)
		PutBitmap(x,WHeight+276-WHNM-(WHeight > 550 ? 12 : 0),&note_rect[6],BMPNOTE);
}

//�����̕\��
void OrgData::PutNotes(int TPCY, bool vol)
{
	int i,j,k,t,addY;
	int n; //2010.09.23 A
	NOTELIST *p;
	long xpos,ypos;
	long scr_h,scr_v;
	unsigned char line,dot;
	//unsigned char trkbit[MAXMELODY];  
	//unsigned char trkmaskLower[MAXMELODY];
	//unsigned char trkmaskUpper[MAXMELODY];
	MUSICINFO mi;
	
	/*
	for(i=0,k=1; i<MAXMELODY && i<8; i++,k*=2){
		trkbit[i]       = (unsigned char)k;         // 1,     2,   4,   8,   16, ...
		trkmaskLower[i] = (unsigned char)(k-1);     // 0,     1,   3,   7,   15, ...
		trkmaskUpper[i] = (unsigned char)(256-2*k); // 254, 252, 248, 240,  224, ...
	}
	*/
	//�K�v�ȃf�[�^���擾
	GetMusicInfo(&mi);line = mi.line;dot = mi.dot;
	scr_data.GetScrollPosition(&scr_h,&scr_v);

	//for(i=0;i<128*92;i++)noteon[i] = 0;

	/*
	//�����̏d�Ȃ����m�F���� //A 2014.05.03
	memset((void *)noteon, 0, 128*12*8);
	for(n = 0; n < MAXMELODY; n++){
		k = getRecentTrack(n , 0);
		if(k == track)continue;
		if((p = info.tdata[k].note_list) == NULL)continue;//������������Β��~
		while(p != NULL && p->to != NULL){//�ǂ�����\�����邩
			xpos = (p->x - line*dot*scr_h)*16 + KEYWIDTH;
			if(xpos >= KEYWIDTH)break;//�\���̈�ɓ�����
			p = p->to;
		}
		//�K����\������ׂɁA��O�̉�������E�E�E�B
		if(p == NULL)continue;
		if(p->from != NULL)p = p->from;
		while(p->from != NULL && p->y == KEYDUMMY)p = p->from;
		//���������Ȃ��Ȃ邩�AX���W���\���̈�𒴂���܂ŕ\���B
		while(p != NULL){
			//xmax128
			if(p->y != KEYDUMMY){
				for(e = p->x - line*dot*scr_h; (e < 128) && (e < (p->x - line*dot*scr_h + p->length) ); e++){
					noteon[p->y * 128 + e] |= trkbit[n];
				}
			}
			p = p->to;
		}
	}
	*/

	//�ҏW���Ȃ��������ɕ\��///////////
	if (!vol) {
		for (n = 0; n < MAXMELODY; n++) {
			k = getRecentTrack(n, 0);
			if (k == track)continue;
			if ((p = info.tdata[k].note_list) == NULL)continue;//������������Β��~
			while (p != NULL && p->to != NULL) {//�ǂ�����\�����邩
				xpos = (p->x - scr_h) * NoteWidth + KEYWIDTH;
				if (xpos >= KEYWIDTH)break;//�\���̈�ɓ�����
				p = p->to;
			}
			//�K����\������ׂɁA��O�̉�������E�E�E�B
			if (p == NULL)continue;
			if (p->from != NULL)p = p->from;
			while (p->from != NULL && p->y == KEYDUMMY)p = p->from;
			//���������Ȃ��Ȃ邩�AX���W���\���̈�𒴂���܂ŕ\���B
			while (p != NULL) {
				ypos = (95 - p->y - scr_v) * 12;//����0�ɂȂ�95���ő�
				xpos = (p->x - scr_h) * NoteWidth + KEYWIDTH;
				t = (p->y % 12); if (t == 1 || t == 3 || t == 6 || t == 8 || t == 10)t = 1;else t = 0;
				if (xpos > WWidth)break;//�\���̈�𒴂����B
				if (ypos >= 0 && ypos < WHeight + 286 - WHNM) {//�\���͈�YPOS
					addY = 0;
					/*
					for(i = p->length-1; i >= 0; i--){//�K���ӂ��߂�(>0)���܂�(==0)���� //A 2014.05.06
						e = p->x - line*dot*scr_h + i;
						unsigned char uct = noteon[p->y * 128 + e];
						if((uct & trkmaskLower[track]) && track < n){ //��Tr��菬����Tr�ԍ��̉�����������
							addY-=2; break;
						}else if((uct & trkmaskLower[track]) && track > n){ //��Tr���傫��Tr�ԍ��̉�����������
							addY+=2; break;
						}
					}
					addY=-n*3; if(addY>=0)addY++;
					*/
					if (iSlideOverlapNotes != 0) {
						addY = -k / 2 + 2; //if(addY>=0)addY++;
						if (k >= 4)addY--;
					}

					for (j = 0, i = p->length - 1; i > 0; i--, j++) {//�K��
						if (TPCY == 0) PutBitmap2(xpos + j * NoteWidth + NoteWidth, ypos + 3 + addY, &note_tail_rect[k + 8], BMPNOTE);
						else PutBitmap2(xpos + j * NoteWidth + NoteWidth, ypos + 3 + addY, &rc_TCPY[2 + t], BMPNOTE);
					}
					if (TPCY == 0) PutBitmapHead(xpos, ypos + 2 + addY, &note_blue_rect[k], BMPNOTE, p->length);//����(�V)
					else PutBitmapHead(xpos, ypos + 2 + addY, &rc_TCPY[t], BMPNOTE, p->length);//����(�V)
				}
				p = p->to;
			}
		}
	}
	//�ҏW���鉹����\��///////////
	if((p = info.tdata[track].note_list) == NULL || TPCY==1){
		//return;//������������Β��~
	}else{
		while(p != NULL && p->to != NULL){//�ǂ�����\�����邩
			xpos = (p->x - scr_h)*NoteWidth + KEYWIDTH;
			if(xpos >= KEYWIDTH)break;
			p = p->to;
		}
		//�K����\������ׂɁA��O�̉�������E�E�E�B
		if(p == NULL){
			//return;
		}else{
			if(p->from != NULL)p = p->from;
			while(p->from != NULL && p->y == KEYDUMMY)p = p->from;
			//���������Ȃ��Ȃ邩�AX���W���\���̈�𒴂���܂ŕ\���B
			while(p != NULL){
				ypos = (95 - p->y - scr_v)*12;//����0�ɂȂ�95���ő�
				xpos = (p->x - scr_h)*NoteWidth + KEYWIDTH;
				if(xpos > WWidth)break;//�\���̈�𒴂����B
				if(!vol && ypos >= 0 && ypos < WHeight+286-WHNM){//�\���͈�YPOS
					//tBitmap(xpos,ypos+2,&note_rect[0],BMPNOTE);//����
					for(j = 0,i = p->length-1; i > 0; i--,j++){//�K��
						//PutBitmap(xpos+j*16+16,ypos+3,&note_rect[2],BMPNOTE);
						PutBitmap2(xpos+j*NoteWidth+NoteWidth,ypos+3,&note_tail_rect[track],BMPNOTE);
					}
					PutBitmapHead(xpos,ypos+2,&note_blue_rect[track+16],BMPNOTE,p->length);//����(�V)
				}
				if (vol) {
					if (p->pan != PANDUMMY) {
						ypos = WHeight + 351 - WHNM - (p->pan * 5);//�p��
						PutBitmapCenter(xpos, ypos, &note_rect[4], BMPNOTE);
					}
					if (p->volume != VOLDUMMY) {
						ypos = WHeight + 426 - WHNM - (p->volume / 4);//�{�����[��
						PutBitmapCenter(xpos, ypos, &note_rect[4], BMPNOTE);
					}
				}
				p = p->to;
			}
		}
	}
}
//����(�h����)�̕\��
void OrgData::PutNotes2(int TPCY, bool vol)
{
	int i,j,k,t,addY;
	int n; //2010.09.23 A
	NOTELIST *p;
	long xpos,ypos;
	long scr_h,scr_v;
	unsigned char line,dot;
	MUSICINFO mi;
	//�K�v�ȃf�[�^���擾
	GetMusicInfo(&mi);line = mi.line;dot = mi.dot;
	scr_data.GetScrollPosition(&scr_h,&scr_v);

	//�ҏW���Ȃ��������ɕ\��///////////
	if (!vol) {
		for (n = MAXMELODY; n < MAXTRACK; n++) {
			k = getRecentTrack(n - MAXMELODY, 1);
			if (k == track)continue;
			if ((p = info.tdata[k].note_list) == NULL) {
				//			MessageBox(hWnd,"","",MB_OK);
				continue;//������������Β��~
			}
			while (p != NULL && p->to != NULL) {//�ǂ�����\�����邩
				xpos = (p->x - scr_h) * NoteWidth + KEYWIDTH;
				if (xpos >= KEYWIDTH)break;
				p = p->to;
			}
			//�K����\������ׂɁA��O�̉�������E�E�E�B
			if (p == NULL)continue;
			if (p->from != NULL)p = p->from;
			while (p->from != NULL && p->y == KEYDUMMY)p = p->from;
			//���������Ȃ��Ȃ邩�AX���W���\���̈�𒴂���܂ŕ\���B
			while (p != NULL) {
				ypos = (95 - p->y - scr_v) * 12;//����0�ɂȂ�95���ő�
				xpos = (p->x - scr_h) * NoteWidth + KEYWIDTH;
				t = (p->y % 12); if (t == 1 || t == 3 || t == 6 || t == 8 || t == 10)t = 1;else t = 0;
				if (xpos > WWidth)break;//�\���̈�𒴂����B
	//			if(ypos >= 0 && ypos < 286+WDWHEIGHTPLUS){//�\���͈�YPOS
				if (ypos >= 0 && ypos < WHeight + 286 - WHNM) {//�\���͈�YPOS
					addY = 0;
					/*if(iSlideOverlapNotes!=0){
						addY=-(k-MAXMELODY)/2+2; //if(addY>=0)addY++;
						if((k-MAXMELODY)>=4)addY--;
					}*/
					for (j = 0, i = p->length - 1; i > 0; i--, j++) {//�K��
						if (TPCY == 0) PutBitmap2(xpos + j * NoteWidth + NoteWidth, ypos + 3 + addY, &note_tail_rect[k], BMPNOTE);
						else PutBitmap2(xpos + j * NoteWidth + NoteWidth, ypos + 3 + addY, &rc_TCPY[2 + t], BMPNOTE);
					}
					if (TPCY == 0) PutBitmapHead(xpos, ypos + 2 + addY, &note_blue_rect[k], BMPNOTE, p->length);//����
					else PutBitmapHead(xpos, ypos + 2 + addY, &rc_TCPY[0 + t], BMPNOTE, p->length);//����
				}
				p = p->to;
			}
		}
	}
	//�ҏW���鉹����\��///////////
	if((p = info.tdata[track].note_list) == NULL || TPCY==1){
		//return;//������������Β��~
	}else{
		while(p != NULL && p->to != NULL){//�ǂ�����\�����邩
			xpos = (p->x - scr_h)*16 + KEYWIDTH;
			if(xpos >= KEYWIDTH)break;
			p = p->to;
		}
		//�K����\������ׂɁA��O�̉�������E�E�E�B
		if(p == NULL){
			//return;
		}else{
			if(p->from != NULL)p = p->from;
			while(p->from != NULL && p->y == KEYDUMMY)p = p->from;
			//���������Ȃ��Ȃ邩�AX���W���\���̈�𒴂���܂ŕ\���B
			while(p != NULL){
				ypos = (95 - p->y - scr_v)*12;//����0�ɂȂ�95���ő�
				xpos = (p->x - scr_h)*NoteWidth + KEYWIDTH;
				if(xpos > WWidth)break;//�\���̈�𒴂����B
				if(!vol && ypos >= 0 && ypos < WHeight+286-WHNM){//�\���͈�YPOS
					//PutBitmap(xpos,ypos+2,&note_rect[0],BMPNOTE);//����
					//PutBitmap2(xpos,ypos+2,&note_blue_rect[track+16],BMPNOTE);//����	// 2014.05.27 D
					for(j = 0,i = p->length-1; i > 0; i--,j++){//�K��
						PutBitmap2(xpos+j*NoteWidth+NoteWidth,ypos+3,&note_tail_rect[track-8],BMPNOTE);
					}
					PutBitmapHead(xpos,ypos+2,&note_blue_rect[track+16],BMPNOTE,p->length);//����	// 2014.05.27 A
				}
				if (vol) {
					if (p->pan != PANDUMMY) {
						ypos = WHeight + 351 - WHNM - (p->pan * 5);//�p��
						PutBitmapCenter(xpos, ypos, &note_rect[4], BMPNOTE);
					}
					if (p->volume != VOLDUMMY) {
						ypos = WHeight + 426 - WHNM - (p->volume / 4);//�{�����[��
						PutBitmapCenter(xpos, ypos, &note_rect[4], BMPNOTE);
					}
				}
				p = p->to;
			}
		}
	}
}
//�y���̕\��
void OrgData::PutMusic(void)
{
	//if (gIsDrawing) return;
	//gIsDrawing = true;

	RECT brect;
	int j;
	int i; // A 2010.09.24
	long x;
	long hpos,vpos,vpos2;

	brect = {0, 0, WWidth, WHeight};
	PutRect(&brect, 0);

	scr_data.GetScrollPosition(&hpos,&vpos);
	vpos2=vpos;
	vpos = -(vpos%12)*12;
	//�����ȍ~�Ɋy���\�����L�q
	x = (-(hpos % (info.dot * info.line)) * NoteWidth) + KEYWIDTH;

	for (j = 0; j < (WHeight / 144) + 1; ++j) {
		for (int i = 0; i < (WWidth / NoteWidth) + (info.line * info.dot) + 1; ++i) { // 15
			if (i % (info.line * info.dot) == 0) {
				brect = { 64, 0, 64 + NoteWidth, 144 };
				PutBitmap(x + i * NoteWidth, j * 144 + vpos, &brect, BMPMUSIC);
			}
			else if (i % info.dot == 0) {
				brect = { 64 + 16, 0, (64 + 16) + NoteWidth, 144 };
				PutBitmap(x + i * NoteWidth, j * 144 + vpos, &brect, BMPMUSIC);
			}
			else {
				if (NoteWidth >= 8) {
					brect = { 64 + 32, 0, (64 + 32) + NoteWidth, 144 };
					PutBitmap(x + i * NoteWidth, j * 144 + vpos, &brect, BMPMUSIC);
				}
				else {
					brect = { 64 + 32 + 1, 0, (64 + 32 + 1) + NoteWidth, 144 };
					PutBitmap(x + i * NoteWidth, j * 144 + vpos, &brect, BMPMUSIC);
				}
			}
		}
	}

	//for (j = 0; j < 8; j++) PutMusicParts(x, j * 144 + vpos);//�y��
	/*if (x < KEYWIDTH) {
		for (j = 0; j < 8; j++)PutMusicParts(x + WWidth, j * 144 + vpos);//�y��
		PutPanParts(x + WWidth);//�p�����C��
	}*/

	//�L�[�{�[�h���Ձi���ʔw�i�����炷�����j
	for(j = 0; j < 96; j++){ // 2010.09.22 A
		if(iKeyPushDown[j]!=0){
			PutBitmap(0,  (95 - j - vpos2)*12, &rc_PushKB[j%12],BMPMUSIC);//����
		}
	}

	if(gDrawDouble==0){
		if(track < MAXMELODY)PutNotes(0, false);
		else PutNotes2(0, false);
	}else{
		if(track < MAXMELODY){
			PutNotes2(1, false);	//�������\��
			PutNotes(0, false);		//���̕\��
		}
		else{
			PutNotes(1, false);	//�������\��
			PutNotes2(0, false);	//���̕\��
		}
		
	}

	PutNumber();
	PutRepeat();

	for(j = 0; j < (WHeight / 144) + 1; j++) PutBitmap(0, j*144 + vpos, &msc_rect[0], BMPMUSIC);

	//�L�[�{�[�h���Ձi���Օ����j
	for(j = 0; j < 96 ; j++){ // 2010.09.22 A
		if(iKeyPushDown[j]!=0){
			//A 2010.09.24
			for(i = 0; i < (WHeight / 144) + 1; i++){
				PutBitmap(0,  (95 - (j%12)- i*12 - vpos2)*12, &rc_PushKB3[j%12],BMPMUSIC);//�I�N�^�[�u�Ⴂ����
			}
			PutBitmap(0,  (95 - j - vpos2)*12, &rc_PushKB2[j%12],BMPMUSIC);//����
		}
	}

	for (i = 0; i < 8; i++) {
		PutBitmap(55, (95 - vpos2 - i * 12) * 12, &num_rect[i + 11], BMPNUMBER);
	}

	for (i = 0; i < (WWidth / NoteWidth) + (info.line * info.dot) + 1; i++) { // 15
		if (i % (info.line * info.dot) == 0){
			brect = { 64, 0, 64 + NoteWidth, 144 + 16 };
			PutBitmap(x + i * NoteWidth, WHeight + 288 - WHNM, &brect, BMPPAN);
		} else if (i % info.dot == 0) {
			brect = { 64 + 16, 0, (64 + 16) + NoteWidth, 144 + 16 };
			PutBitmap(x + i * NoteWidth, WHeight + 288 - WHNM, &brect, BMPPAN);
		} else {
			if (NoteWidth >= 8) {
				brect = { 64 + 32, 0, (64 + 32) + NoteWidth, 144 + 16 };
				PutBitmap(x + i * NoteWidth, WHeight + 288 - WHNM, &brect, BMPPAN);
			} else {
				brect = { 64 + 32 + 1, 0, (64 + 32 + 1) + NoteWidth, 144 + 16 };
				PutBitmap(x + i * NoteWidth, WHeight + 288 - WHNM, &brect, BMPPAN);
			}
		}
	}

	//PutPanParts(x);//�p�����C��

	if (gDrawDouble == 0) {
		if (track < MAXMELODY)PutNotes(0, true);
		else PutNotes2(0, true);
	}
	else {
		if (track < MAXMELODY) {
			PutNotes2(1, true);	//�������\��
			PutNotes(0, true);		//���̕\��
		}
		else {
			PutNotes(1, true);	//�������\��
			PutNotes2(0, true);	//���̕\��
		}

	}

	PutSelectArea();

	PutBitmap(0,WHeight+288-WHNM,&msc_rect[0],BMPPAN);//�p��

	if(sACrnt){ //2014.04.30
		PutBitmap(0,WHeight+288-WHNM+144,&rc_CurrentSelect,BMPNOTE);//�͈͑I���͏�ɶ����ׯ��̂Ƃ�
	}
	if(iActivatePAN){ //2014.05.01
		PutBitmap(0,WHeight+288-WHNM,&rc_ActivePAN, BMPNOTE);
	}
	if(iActivateVOL){ //2014.05.01
		PutBitmap(0,WHeight+288-WHNM+72,&rc_ActiveVOL, BMPNOTE);
	}

	if (timer_sw) {
		long playPos;
		org_data.GetPlayPos(NULL, &playPos);

		//char str[12];
		//char oldstr[12];


		/*itoa(playPos / (info.dot * info.line), str, 10);
		GetDlgItemText(hDlgPlayer, IDE_VIEWMEAS, oldstr, 10);
		if (strcmp(str, oldstr) != 0) SetDlgItemText(hDlgPlayer, IDE_VIEWMEAS, str);

		itoa(playPos % (info.dot * info.line), str, 10);
		GetDlgItemText(hDlgPlayer, IDE_VIEWXPOS, oldstr, 10);
		if (strcmp(str, oldstr) != 0) SetDlgItemText(hDlgPlayer, IDE_VIEWXPOS, str);*/

		/*if (sSmoothScroll) {
			/*DWORD dwNowTime;
			dwNowTime = timeGetTime();
			// Only draw if ms have passed, to prevent lags
			if (dwNowTime - lastDrawTime >= drawCatch) { // 50 fps (cave story reference)
				if (play_p != info.end_x) scr_data.SetHorzScroll(play_p);
				else scr_data.SetHorzScroll(info.repeat_x);
				lastDrawTime = timeGetTime();
				drawCatch = lastDrawTime - dwNowTime;
				if (drawCatch > 500) drawCatch = 500;
			}*//*
			/*if (playPos != info.end_x) scr_data.SetHorzScroll(playPos);
			else scr_data.SetHorzScroll(info.repeat_x);*//*
		}*/

		scr_data.SetHorzScroll(sSmoothScroll ? playPos : ((playPos / (info.dot * info.line)) * (info.dot * info.line)));
	}
	//gIsDrawing = false;
}

//�I��͈͂̕\��
void OrgData::PutSelectArea()
{
	if(tra<0)return; //�I������Ăւ�
	long ypos;
	long scr_h,scr_v;
	unsigned char line,dot;
	MUSICINFO mi;
	//�K�v�ȃf�[�^���擾
	GetMusicInfo(&mi);line = mi.line;dot = mi.dot;
	scr_data.GetScrollPosition(&scr_h,&scr_v);

	int xSelS, xSelE,xx,t;
	t = 0; if(ful==0 && tra!=track)t=3;
	xSelS = (nc_Select.x1_1 - scr_h)*NoteWidth + KEYWIDTH; //�I���J�n�_
	xSelE = (nc_Select.x1_2 - scr_h)*NoteWidth + KEYWIDTH; //�I���I���_
	ypos = WHeight - 13;
	for(xx=KEYWIDTH;xx<=WWidth+NoteWidth;xx+=NoteWidth){
		if(xx==xSelS){
			if(xx==xSelE)PutBitmapCenter16(xx,ypos,&rc_SelArea[7],BMPNOTE);
			else PutBitmapCenter16(xx,ypos,&rc_SelArea[0+t],BMPNOTE);
		}else if(xx>xSelS && xx<xSelE){
			if(((xx-KEYWIDTH)/NoteWidth + scr_h)%4==0 && ful==1 ){
				PutBitmapCenter16(xx,ypos,&rc_SelArea[6],BMPNOTE); //FULL
			}else{
				PutBitmapCenter16(xx,ypos,&rc_SelArea[1+t],BMPNOTE);
			}
		}else if(xx==xSelE){
			PutBitmapCenter16(xx,ypos,&rc_SelArea[2+t],BMPNOTE);
		}
	}


}

void OrgData::RedrawSelectArea()
{
	//long x;
	//long hpos, vpos;
	//scr_data.GetScrollPosition(&hpos, &vpos);
	//x = (-(hpos % (info.dot * info.line)) * NoteWidth) + KEYWIDTH;
	//PutSelectParts(x);//�p�����C��
	//PutSelectArea();

}
