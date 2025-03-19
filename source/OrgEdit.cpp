#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Setting.h"
#include "Gdi.h"
#include "DefOrg.h"
#include "OrgData.h"
#include "rxoFunction.h"
#include "Scroll.h"
#include "Sound.h"

#define PI 3.14159265358979323846
/*
//ï“èWópç\ë¢ëÃ///////////////
typedef struct{//ÅúÉRÉsÅ[ç\ë¢ëÃ
	char track1;//Ç±ÇÃÉgÉâÉbÉNÇÃ
	long x1_1;//Ç±Ç±Ç©ÇÁ
	long x1_2;//Ç±Ç±Ç‹Ç≈Ç
	char track2;//Ç±ÇÃÉgÉâÉbÉNÇÃ
	long x2;//Ç±Ç±Ç…
	long num;//âΩâÒ
}NOTECOPY;

typedef struct{//ÅúÉgÉâÉìÉXÉ|Å[ÉgÇ‚ÉpÉìÉ|ÉbÉg
	char track;//Ç±ÇÃÉgÉâÉbÉNÇÃ
	long x1;//Ç±Ç±Ç©ÇÁ
	long x2;//Ç±Ç±Ç‹Ç≈Ç
	unsigned char a;//Ç±ÇÍÇæÇØ 
	unsigned char mode;//ÇΩÇ∑ÅiÇ–Ç≠Åj
}PARCHANGE;
*/

extern int volChangeLength;
extern bool volChangeUseNoteLength;
extern bool volChangeSetNoteLength;

extern char timer_sw;
extern int sACrnt;

void ChangeTrackPlus(int iValue) {
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);

	char str[8];

	RECT rect = { 64,0,WWidth,WHeight };//çXêVÇ∑ÇÈóÃàÊ(ÉgÉâÉbÉNïœçX)


	org_data.track += iValue;
	org_data.track = (org_data.track + 16) % 16;
	setRecentTrack(org_data.track); //A 2010.09.23 

	if (timer_sw == 0) PlayOrganKey(36, org_data.track, mi.tdata[org_data.track].freq, 80);
	if (sACrnt) {
		if (tra >= 0) {
			tra = org_data.track;
		}
	}
	//ëIëÉgÉâÉbÉNï\é¶
	//itoa(org_data.track, str, 10);

	/*if (sACrnt) {
		if(tra>=0){
			tra = org_data.track;
			char wk[256],wk2[256];

			if(ful==0){
				wsprintf(wk2,MessageString[IDS_STRING104], TrackN[tra]);
				strcpy(wk,wk2);
				wsprintf(wk2,MessageString[IDS_STRING105]);
				strcat(wk,wk2);
				SetEZCWindowMessage(wk);
			}
			else if(ful==1){
				wsprintf(wk2,MessageString[IDS_STRING106]);
				strcpy(wk,wk2);
				SetEZCWindowMessage(wk);
			}
		}
	}*/

	//SetDlgItemText(hDlgTrack,IDE_VIEWTRACK,TrackCode[org_data.track]);
	//SetDlgItemText(hdwnd,IDE_VIEWTRACK,str);
	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);
	//SetFocus(hWnd);
	UpdateToolbarStatus();
	UpdateStatusBar(false);
}


void ChangeTrack(int iTrack) {
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);

	char str[8];
	int i;
	RECT rect = { 64,0,WWidth,WHeight };//çXêVÇ∑ÇÈóÃàÊ(ÉgÉâÉbÉNïœçX)

	i = iTrack;
	org_data.track = i;
	setRecentTrack(org_data.track); //A 2010.09.23 

	if (timer_sw == 0) PlayOrganKey(36, i, mi.tdata[i].freq, 80);
	if (sACrnt) {
		if (tra >= 0) {
			tra = org_data.track;
		}
	}
	//ëIëÉgÉâÉbÉNï\é¶
	/*itoa(org_data.track, str, 10);
	if(sACrnt){
		if(tra>=0){
			tra = org_data.track;
			char wk[256],wk2[256];

			if(ful==0){
				wsprintf(wk2,MessageString[IDS_STRING104], TrackN[tra]);
				strcpy(wk,wk2);
				wsprintf(wk2,MessageString[IDS_STRING105]);
				strcat(wk,wk2);
				SetEZCWindowMessage(wk);
			}
			else if(ful==1){
				wsprintf(wk2,MessageString[IDS_STRING106]);
				strcpy(wk,wk2);
				SetEZCWindowMessage(wk);
			}
		}
	}*/
	//SetDlgItemText(hDlgTrack,IDE_VIEWTRACK,TrackCode[org_data.track]);
	//SetDlgItemText(hdwnd,IDE_VIEWTRACK,str);
	//org_data.PutMusic();
	//RedrawWindow(hWnd,&rect,NULL,RDW_INVALIDATE|RDW_ERASENOW);

	UpdateToolbarStatus();
	UpdateStatusBar(false);
	//SetFocus(hWnd);
}

BOOL OrgData::DelateNoteData(PARCHANGE *pc)
{
	NOTELIST *np;
	NOTELIST *np_from;//Ç±Ç¢Ç¬Ç∆
	NOTELIST *np_to;//Ç±Ç¢Ç¬ÇåqÇÆ
	NOTELIST *np_clear;//êKîˆçÌèúóp
	long note_x1,note_x2;//ã»ÇÃîÕàÕÇï€ë∂
	np = info.tdata[pc->track].note_list;
	if(np == NULL)return FALSE;//âπïÑÇ™ñ≥ÇØÇÍÇŒèIóπ

	np = info.tdata[pc->track].note_list;
	note_x1 = np->x;//ç≈èâÇÃÇòÇãLò^
	while(np->to != NULL)np = np->to;//Ç®ÇÌÇËÇåüçı
	note_x2 = np->x;//Ç®ÇÌÇËÇï€ë∂
	if(note_x1 > pc->x2 || pc->x1 > note_x2){//îÕàÕÇ™äyïàÇ©ÇÁó£ÇÍÇƒÇ¢ÇÈ
		return FALSE;
	}else if(note_x1 < pc->x1 && pc->x2 < note_x2){//îÕàÕÇ™äyïàÇ…ì‡ïÔÇ≥ÇÍÇƒÇ¢ÇÈÅB
		np = info.tdata[pc->track].note_list;
		while(np->x < pc->x1)np = np->to;//îÕàÕÇÃì™Çåüçı
		np_from = np->from;//ç∂í[Çï€ë∂(ê⁄ë±óp)
		while(np->x <= pc->x2){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;//îÕàÕÇÃêKÇåüçı
		}
		np_to = np;//âEí[Çï€ë∂(ê⁄ë±óp)
		//óºí[Çê⁄ë±
		np_from->to = np_to;
		np_to->from = np_from;
		return TRUE;

	}else if(pc->x1 <= note_x1 && pc->x2 < note_x2){//è¡Ç∑îÕàÕÇÃì™Ç…âπÇ™ë∂ç›ÇµÇ»Ç¢ÅB
		np = info.tdata[pc->track].note_list;
		while(np->x <= pc->x2){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;//îÕàÕÇÃêKÇåüçı
		}
		np_to = np;//âEí[Çï€ë∂(ê⁄ë±óp)
		np->from = NULL;//Ç±Ç¢Ç¬Çì™Ç…Ç∑ÇÈ
		info.tdata[pc->track].note_list = np;//ÉäÉXÉgÇ…ìoò^
		return TRUE;
	}else if(note_x1 < pc->x1 && note_x2 <= pc->x2 ){//è¡Ç∑îÕàÕÇÃêKÇ…âπÇ™ë∂ç›ÇµÇ»Ç¢ÅB
		np = info.tdata[pc->track].note_list;
		while(np->to->x < pc->x1)np = np->to;//îÕàÕÇÃì™Çåüçı
		np_clear = np;//ç≈å„Ç…Ç»ÇÈâπïàÇÃÉAÉhÉåÉX
		np = np->to;
		while(np != NULL){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;
		}
		np_clear->to = NULL;
		return TRUE;
	}else if(note_x1 >= pc->x1 && note_x2 <= pc->x2 ){//äÆëSè¡ãé
		np = info.tdata[pc->track].note_list;
		while(np != NULL){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;
		}
		info.tdata[pc->track].note_list = NULL;
		return TRUE;
	}
	return TRUE;
}

BOOL OrgData::GrabNoteData(SAVEDNOTE* sn, char track1, char track2, long x1, long x2)
{
	int i, j;
	NOTELIST* np;
	NOTELIST* work;
	NOTELIST* wp;
	long ind_x;
	long copy_num;

	if (sn == NULL || x1 < 0 || x2 < x1) {
		return FALSE;
	}

	sn->length = x2 + 1 - x1;
	sn->track1 = track1;
	sn->track2 = track2;

	for (j = track1; j <= track2; ++j) {
		copy_num = GetNoteNumber(j, x1, x2);
		if (copy_num == 0) {
			sn->data[j].data = NULL;
			continue; // No notes here
		}

		wp = work = (NOTELIST*)malloc(sizeof(NOTELIST) * copy_num);
		if (work == NULL) {
			return FALSE;
		}

		ind_x = x1;
		np = info.tdata[j].note_list;
		while (np != NULL && np->x < x1) np = np->to;
		if (np == NULL) {
			free(work);
			sn->data[j].data = NULL;
			continue;
		}

		for (i = 0; i < copy_num; i++) {
			wp->length = np->length;
			wp->pan = np->pan;
			wp->volume = np->volume;
			wp->y = np->y;
			wp->x = np->x - ind_x;
			wp->from = wp->to = NULL;
			np = np->to;
			wp++;
		}

		sn->data[j].size = copy_num;
		sn->data[j].data = work;
	}
	return TRUE;
}

// I'm tired of this
NOTELIST* OrgData::FindOrgNote(char track, int x) {
	if (track < 0 || track > 15) return NULL;
	NOTELIST* rnp = info.tdata[track].note_list;
	while (rnp != NULL && rnp->x < x) {
		rnp = rnp->to;
	}
	if (rnp == NULL) return NULL;
	if (rnp->x == x) return rnp;
	return NULL;
}

NOTELIST* OrgData::CreateOrgNote(char track, int x) {
	if (track < 0 || track > 15) return NULL;
	NOTELIST* nfrom = info.tdata[track].note_list;
	if (nfrom != NULL) {
		while (nfrom->x < x && nfrom->to != NULL) {
			nfrom = nfrom->to;
		}
		if (nfrom->x == x) return NULL; // note exists here
	}
	NOTELIST* mnp = SearchNote(track);
	if (mnp == NULL) return NULL;
	if (nfrom == NULL) {
		info.tdata[track].note_list = mnp;
		mnp->from = NULL;
		mnp->to = NULL;
		mnp->x = x;
		mnp->y = KEYDUMMY;
		mnp->volume = VOLDUMMY;
		mnp->pan = PANDUMMY;
		mnp->length = 1;
		return mnp;
	}
	else if (nfrom->x > x) {
		mnp->from = nfrom->from;
		mnp->to = nfrom;
		mnp->x = x;
		mnp->y = KEYDUMMY;
		mnp->volume = VOLDUMMY;
		mnp->pan = PANDUMMY;
		mnp->length = 1;
		if (nfrom->from == NULL) {
			info.tdata[track].note_list = mnp;
		}
		else {
			nfrom->from->to = mnp;
		}
		nfrom->from = mnp;
		return mnp;
	}
	else if (nfrom->to == NULL) {
		mnp->from = nfrom;
		nfrom->to = mnp;
		mnp->to = NULL;
		mnp->x = x;
		mnp->y = KEYDUMMY;
		mnp->volume = VOLDUMMY;
		mnp->pan = PANDUMMY;
		mnp->length = 1;
		return mnp;
	}
	return NULL;
}

void OrgData::DeleteOrgNote(char track,NOTELIST* note) {
	if (track < 0 || track > 15) return;
	if (note == NULL) return;
	if (note->from == NULL) {
		if (note != info.tdata[track].note_list) {
			return;
		}
		info.tdata[track].note_list = note->to;
	}
	else {
		note->from->to = note->to;
	}
	if (note->to != NULL) {
		note->to->from = note->from;
	}
	//free(note);
	note->length = 0;
}

NOTELIST* OrgData::FindLastOrgNoteKey(char track, int x) {
	if (track < 0 || track > 15) return NULL;

	NOTELIST* rnp = info.tdata[track].note_list;

	if (rnp == NULL) return NULL;

	while (rnp->to != NULL && rnp->to->x < x) {
		rnp = rnp->to;
	}

	while (rnp != NULL && rnp->y == KEYDUMMY) {
		rnp = rnp->from;
	}

	if (rnp == NULL) return NULL;

	if (x >= rnp->x) return rnp;

	return NULL;
}

NOTELIST* OrgData::FindOrgNoteLength(char track, int x) {
	if (track < 0 || track > 15) return NULL;

	NOTELIST* rnp = info.tdata[track].note_list;

	if (rnp == NULL) return NULL;

	while (rnp->to != NULL && rnp->to->x < x) {
		rnp = rnp->to;
	}

	while (rnp != NULL && rnp->y == KEYDUMMY) {
		rnp = rnp->from;
	}

	if (rnp == NULL) return NULL;

	if (x >= rnp->x && x < rnp->x + rnp->length) return rnp;

	return NULL;
}

BOOL OrgData::PasteNoteData(SAVEDNOTE* sn, char track, long x, long num, int pasteFlags)
{
	int i, j, k;
	long copy_num;
	SAVEDTRACK* st;

	for (k = sn->track1; k <= sn->track2; ++k) {
		st = &sn->data[k];

		copy_num = st->size;
		if (st->data == NULL) {
			continue;
		}

		if (sn->track1 != sn->track2) {
			track = k;
		}

		if (!(pasteFlags & PF_MIX_PASTE)) {
			for (j = x; j < x + (sn->length * num); ++j) {
				NOTELIST* note = FindOrgNote(track, j);
				if (note) {
					if (pasteFlags & PF_PASTE_NOTE) {
						note->y = KEYDUMMY;
						note->length = 1;
					}
					if (pasteFlags & PF_PASTE_VOL) {
						note->volume = VOLDUMMY;
					}
					if (pasteFlags & PF_PASTE_PAN) {
						note->pan = PANDUMMY;
					}

					if (note->y == KEYDUMMY && note->volume == VOLDUMMY && note->pan == PANDUMMY) {
						DeleteOrgNote(track, note);
					}
				}
			}
		}

		for (j = 0; j < num; ++j) {
			int lastVolumeCheck = 0;
			int lastPanCheck = 0;
			NOTELIST* work = st->data;
			for (i = 0; i < copy_num && (work->x + x) < x + (sn->length * num); ++i) {
				// create or find note
				NOTELIST* note = FindOrgNote(track, work->x + x + (sn->length * j));
				if (!note) {
					note = CreateOrgNote(track, work->x + x + (sn->length * j));
					if (!note) {
						// OOPS
						return FALSE;
					}
				}

				// copy values
				if (pasteFlags & PF_PASTE_NOTE) {
					note->y = work->y;
					note->length = work->length;
				}
				if (pasteFlags & PF_PASTE_VOL) {
					note->volume = work->volume;
				}
				if (pasteFlags & PF_PASTE_PAN) {
					note->pan = work->pan;
				}

				// make sure volume exists!
				if (note->y == KEYDUMMY) {
					note->length = 1;
				}

				lastVolumeCheck = note->volume;

				// delete empty events
				if (note->y == KEYDUMMY && note->volume == VOLDUMMY && note->pan == PANDUMMY) {
					DeleteOrgNote(track, note);
				}

				++work;
			}
		}
	}
	return TRUE;
}

BOOL OrgData::CopyNoteData(NOTECOPY *nc)
{
	int i,j;
	PARCHANGE pc;
	NOTELIST *np;
	NOTELIST *p_list1,*p_list2;
	NOTELIST* work;
	NOTELIST *wp;
	long ind_x;
	long copy_num;

	if (nc->num == 0)
		return FALSE;

	copy_num = GetNoteNumber(nc->track1, nc->x1_1, nc->x1_2);
	if (copy_num == 0)
		return FALSE;

	wp = work = (NOTELIST *)malloc(sizeof(NOTELIST)*copy_num);
	ind_x = nc->x1_1;
	
	np = info.tdata[nc->track1].note_list;
	while(np != NULL && np->x < nc->x1_1)np = np->to;
	if(np == NULL){
		free( work );
		return TRUE;
	}
	for(i = 0; i < copy_num; i++){//ÇPÉyÅ[ÉXÉg
		wp->length = np->length;
		wp->pan    = np->pan;
		wp->volume = np->volume;
		wp->y      = np->y;
		//ÉCÉìÉfÉbÉNÉXílÇÕç∑Çµà¯Ç≠
		wp->x      = np->x - ind_x;
		//éüÇ÷
		np = np->to;
		wp++;
	}

	//ÉyÅ[ÉXÉgêÊÇÉNÉäÉA
	pc.track = nc->track2;//Ç±ÇÃÉgÉâÉbÉNÇÃ
	pc.x1 = nc->x2;//Ç±Ç±Ç©ÇÁ
	pc.x2 = nc->x2 + (nc->x1_2+1 - nc->x1_1)*nc->num-1;//Ç±Ç±Ç‹Ç≈Çx1_1+1(x1_2ÇÕÅ|ÇPÇµÇΩíl)
	DelateNoteData(&pc);//è¡Ç∑

	//ÉyÅ[ÉXÉgÉäÉXÉgÇÃê∂ê¨
	np = p_list1 = p_list2 = SearchNote(nc->track2);
	if(np == NULL){//ñ¢égópâπïàÇ™ïsë´
		free( work );
		return FALSE;
	}
	np->length = 1;//âºê∂ê¨
	for(i = 1; i < copy_num*nc->num; i++){
		np = SearchNote(nc->track2);
		if(np == NULL){//ñ¢égópâπïàÇ™ïsë´
			//free( work );
			//return FALSE;
			break;
		}
		np->length = 1;//âºê∂ê¨
		p_list2->to = np;
		np->from = p_list2;
		p_list2 = np;
	}

	long index;
	//ÉyÅ[ÉXÉgÉäÉXÉgÇ…ÉyÅ[ÉXÉg
	np = p_list1;
	for(j = 0; j < nc->num;j++){//ÉyÅ[ÉXÉgâÒêî
		wp = work;
		index = nc->x2+ (nc->x1_2 - nc->x1_1 +1)*j;
		for(i = 0; i < copy_num; i++){//ÇPÉyÅ[ÉXÉg
			np->length    = wp->length;
			np->pan       = wp->pan;
			np->volume    = wp->volume; 
			np->y         = wp->y; 
			//ÉCÉìÉfÉbÉNÉXílÇâ¡Ç¶ÇÈ
			np->x         = wp->x + index;

			//éüÇ÷
			np = np->to;
			wp++;
			if (np == NULL) break;
		}
		if (np == NULL) break;
	}



	np = info.tdata[nc->track2].note_list;
	if(np == NULL){//äÆëSêVãK
		info.tdata[nc->track2].note_list = p_list1;
		p_list1->from = NULL;
		p_list2->to = NULL;
	}else{
		//ë}ì¸Ç∑Ç◊Ç´èÍèäÇåüèo
//		np = info.tdata[nc->track2].note_list;
		if(np->x > p_list2->x){//ì™Ç…í«â¡
			np->from = p_list2;
			p_list2->to = np;
			p_list1->from = NULL;
			info.tdata[nc->track2].note_list = p_list1;
			free( work );
			return TRUE;
		}
		while(np->to != NULL && np->to->x < nc->x2)np = np->to;
		if(np->to == NULL){//êKÇ…í«â¡
			np->to = p_list1;
			p_list1->from = np;
			p_list2->to = NULL;
			free( work );
			return TRUE;
		}else{//ë}ì¸
			np->to->from = p_list2;
			p_list2->to = np->to;
			np->to = p_list1;
			p_list1->from = np;
			free( work );
			return TRUE;
		}

	}
	free( work );
	return TRUE;
}
//ÉpÉìÇÕç≈è¨Ç≈ÇOç≈ëÂÇ™ÇPÇQÇ∆Ç»ÇÈ
BOOL OrgData::ChangePanData(PARCHANGE *pc)
{
	int i;
	long num = 0;
	NOTECOPY nc;
	NOTELIST *np;
	//âπïàÇÃêîÇåüèo
	nc.x1_1 = pc->x1;
	nc.x1_2 = pc->x2;
	num = GetNoteNumber(pc->track, nc.x1_1, nc.x1_2);

	np = info.tdata[pc->track].note_list;
	if(np == NULL || num == 0)return FALSE;
	while(np != NULL && np->x < pc->x1)np = np->to;
	if(pc->a == 254){ //2014.05.03 ãtì]
		for(i = 0; i < num; i++){
			if(np->pan == PANDUMMY);
			else np->pan = 12 - np->pan;
			np = np->to;
		}
		
	}else{
		if(pc->mode == MODEPARADD){//â¡éZÉÇÅ[Éh
			for(i = 0; i < num; i++){
				if(np->pan == PANDUMMY);
				else if(np->pan + pc->a > 12)np->pan = 12;
				else np->pan += pc->a;
				np = np->to;
			}
		}else{//å∏éZÉÇÅ[Éh
			for(i = 0; i < num; i++){
				if(np->pan == PANDUMMY);
				else if(np->pan - pc->a < 0)np->pan = 0;
				else np->pan -= pc->a;
				np = np->to;
			}
		}
	}
	return TRUE;
}

//ÉLÅ[ÇÕ95Ç™MAX
BOOL OrgData::ChangeTransData(PARCHANGE *pc)
{
	int i;
	long num = 0;
	NOTECOPY nc;
	NOTELIST *np;
	//âπïàÇÃêîÇåüèo
	nc.x1_1 = pc->x1;
	nc.x1_2 = pc->x2;
	num = GetNoteNumber(pc->track, nc.x1_1, nc.x1_2);

	np = info.tdata[pc->track].note_list;
	if(np == NULL || num == 0)return FALSE;
	while(np != NULL && np->x < pc->x1)np = np->to;
	if(pc->mode == MODEPARADD){//â¡éZÉÇÅ[Éh
		for(i = 0; i < num; i++){
			if(np->y == KEYDUMMY);
			else if(np->y + pc->a > 95)np->y = 95;
			else np->y += pc->a;
			np = np->to;
		}
	}else{//å∏éZÉÇÅ[Éh
		for(i = 0; i < num; i++){
			if(np->y == KEYDUMMY);
			else if(np->y - pc->a < 0)np->y = 0;
			else np->y -= pc->a;
			np = np->to;
		}
	}
	return TRUE;
}
//ÉîÉHÉäÉÖÅ[ÉÄÇÕ254Ç™MAX
BOOL OrgData::ChangeVolumeData(PARCHANGE *pc, char mode)
{
	int i,j;
	long num = 0, lFirstx = 0, lLastx = 0;
	NOTECOPY nc;
	NOTELIST *np;
	unsigned char uc, ucMax = 0, ucMin = 254, ucFirstFlg = 255, ucFirst, ucLast;
	double d = VOLDUMMY, dnum, di, dnorm = 0, dnormnum = 0, dt;
	//âπïàÇÃêîÇåüèo
	nc.x1_1 = pc->x1;
	nc.x1_2 = pc->x2;
	num = GetNoteNumber(pc->track, nc.x1_1, nc.x1_2);

	int vlen = volChangeLength;
	if (volChangeUseNoteLength || mode != 1) {
		vlen = num;
	}

	dnum = vlen - 1.0f;

	int vnum = vlen >= num ? num : vlen;

	if (mode == 1 && !volChangeUseNoteLength && pc->mode >= MODEDECAY && pc->mode < MODEDECAY + 20) {
		np = info.tdata[pc->track].note_list;
		if (np == NULL || num == 0 || vlen == 0)return FALSE;

		while (np != NULL && np->x < pc->x1)np = np->to;

		if (np != NULL && np->y != KEYDUMMY && volChangeSetNoteLength) {
			int lin = (pc->x2 - pc->x1 + 1);
			np->length = pc->mode == (MODEDECAY + 8) ? (lin >= vlen ? vlen : lin) : vnum;
		}
		
		if (vlen < num) {
			for (i = 0; i < vlen; ++i) {
				np = np->to;
			}
			for (i = vlen; i < num; ++i) {
				np->volume = VOLDUMMY;
				np->length = 0;
				if (np->to) np->to->from = np->from;
				if (np->from) np->from->to = np->to;
				np = np->to;
			}
		}
	}

	np = info.tdata[pc->track].note_list;
	if (np == NULL || num == 0 || vlen == 0)return FALSE;

	//ì™ÇæÇµ
	while (np != NULL && np->x < pc->x1)np = np->to;

	//ucMax, ucMinÇí≤Ç◊ÇÈ
	for(i = 0; i < vnum; i++){
		if(np->volume != VOLDUMMY){
			if(ucFirstFlg == 255){ucFirst = np->volume; lFirstx = np->x; ucFirstFlg = 0;}
			ucLast = np->volume;
			lLastx = np->x;
			if(np->volume > ucMax)ucMax = np->volume; else if(np->volume < ucMin)ucMin = np->volume;
			dnorm += (double)np->volume;
			dnormnum += 1.0f;
		}
		np = np->to;
	}
	if(dnormnum > 0)dnorm = dnorm / dnormnum; //ïΩãœíl

	//ä™Ç´ñﬂÇµ Ç∆ ì™ÇæÇµ
	np = info.tdata[pc->track].note_list;
	while(np != NULL && np->x < pc->x1)np = np->to;

	if (mode == 1 && !volChangeUseNoteLength && pc->mode >= MODEDECAY && pc->mode < MODEDECAY + 20) {
		lLastx = lFirstx + vlen - 1;
	}

	if(pc->mode == MODEPARADD){//â¡éZÉÇÅ[Éh
		for(i = 0; i < num; i++){
			if(np->volume == VOLDUMMY);
			else if(np->volume + pc->a > 254)np->volume = 254;
			else np->volume += pc->a;
			np = np->to;
		}
	}else if(pc->mode == MODEPARSUB){//å∏éZÉÇÅ[Éh
		for(i = 0; i < num; i++){
			if(np->volume == VOLDUMMY);
			else if(np->volume - pc->a < 0)np->volume = 0;
			else np->volume -= pc->a;
			np = np->to;
		}
	}else if(pc->mode == MODEMULTIPLY){//èÊéZÉÇÅ[Éh
		// ä˘ë∂ÇÃvolumeÇ a / 128î{ Ç∑ÇÈ
		for(i = 0; i < num; i++){
			d = (double)np->volume * (double) pc->a / (double)128.0;
			if(d>254.0)d=254.0; else if(d<0)d=0;

			uc = (unsigned char)d;

			if(np->volume == VOLDUMMY);
			else np->volume = uc;
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 3)){ //ê¸å`å∏êä 2014.05.01
		for(i = 0; i < vnum; i++){
			if(np->volume != VOLDUMMY){
				d = ((double)ucMax - (double)0) / (double)(vlen - 0) * (double)(vlen - 0 - i); if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}
	/*else if (pc->mode == (MODEDECAY + 14)) { //ê LINEAR 10 L
		for (i = 0; i < num; i++) {
			if (np->volume != VOLDUMMY) {
				if (d == 0) {
					np->volume = VOLDUMMY;
					if (np->y == KEYDUMMY && np->pan == PANDUMMY) {
						np->length = 0;
						if (np->to) np->to->from = np->from;
						if (np->from) np->from->to = np->to;
					}
				}
				else {
					d = ((double)ucMax - (double)0) / (double)(10 - 0) * (double)(10 - 0 - i); if (d > 254.0)d = 254.0; else if (d < 0)d = 0;
					np->volume = (unsigned char)d;
				}
			}
			np = np->to;
		}*/
	else if(pc->mode == (MODEDECAY + 12)){ //ÇQèÊè„è∏ 2014.05.17
		for(i = 0; i < vnum; i++){
			if(np->volume != VOLDUMMY){
				if(vlen > 1){
					d = (double)(ucMax) / (double)((vlen - 1)*(vlen - 1))  * (double)(i*i); if(d>254.0)d=254.0; else if(d<0)d=0;
				}else{
					d = ucMax;
				}
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 13)){ //ê¸å`è„è∏ 2014.05.17
		for(i = 0; i < vnum; i++){
			if(np->volume != VOLDUMMY){
				if(vlen > 1){
					d = (double)ucMax / (double)(vlen - 1) * (double)i; if(d>254.0)d=254.0; else if(d<0)d=0;
				}else{
					d = ucMax;
				}
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 2)){ //è„Ç…ì å∏êä 2014.05.01
		if(vlen < 4)dnum = (double)vlen;
		else if(vlen == 4)dnum = (double)vlen-0.5f;
		for(i = 0; i < vnum; i++){
			di = (double)i;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = 1.0f - ((exp( (di/dnum)*(di/dnum) ) - 1.0f) / 1.718281828459045);
				}else d = 1.0f;
				d = d * (double)ucMax; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 1)){ //è„Ç…ì å∏êä 2014.05.01
		if(vlen < 4)dnum = (double)vlen -0.4f;
		else if(vlen == 4)dnum = (double)vlen -0.5f;
		else dnum = vlen - 0.8f;
		for(i = 0; i < vnum; i++){
			di = (double)i;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = 1.0f - ((exp( (di/dnum)*(di/dnum)*(di/dnum)*(di/dnum) ) - 1.0f) / 1.718281828459045);
				}else d = 1.0f;
				d = d * (double)ucMax; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 4)){ //â∫Ç…ì å∏êä 2014.05.01
		for(i = 0; i < vnum; i++){
			di = (double)i; dnum = (double)vlen;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = (exp( pow(((dnum-di)/dnum), 2)) - 1.0f) / 1.718281828459045;
				}else d = 1.0f;
				d = d * (double)ucMax; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 5)){ //â∫Ç…ì å∏êä 2014.05.01
		for(i = 0; i < vnum; i++){
			di = (double)i; dnum = (double)vlen;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = (exp( pow(((dnum-di)/dnum), 4)) - 1.0f) / 1.718281828459045;
				}else d = 1.0f;
				d = d * (double)ucMax; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 6)){ //ÉmÅ[É}ÉâÉCÉY 2014.05.01
		for(i = 0; i < vnum; i++){
			//di = (double)i; dnum = (double)num;
			if(np->volume != VOLDUMMY){
				//np->volume = (unsigned char)dnorm;
				np->volume = org_data.def_volume[pc->track];//èâä˙âª
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 11)){ //ÉOÉâÉfÅ[ÉVÉáÉì 2014.05.01
		for(i = 0; i < vnum; i++){
			di = (double)i; dnum = (double)vlen;
			if(np->volume != VOLDUMMY && vlen > 1){
				d = ((double)ucFirst - (double)ucLast) / (double)(vlen - 1) * (double)(vlen - 1 - i) + (double)ucLast ;
				if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 8)){ //ç≈å„Ç≈å∏êä
		dt = (double)np->volume;
		j = lLastx - lFirstx; 
		for(i = 0; i < vnum; i++){
			di = (double)i; dnum = (double)vlen;
			if(np->volume != VOLDUMMY){
				//d = dt;
				switch(j){
				case 0:
					d = dt;
					break;
				case 1:
					if     (lFirstx + 0 == np->x)d = dt;
					else if(lFirstx + 1 == np->x)d = dt * 0.92;
					break;
				case 2:
					if     (lFirstx + 0 == np->x)d = dt;
					else if(lFirstx + 1 == np->x)d = dt * 0.95;
					else if(lFirstx + 2 == np->x)d = dt * 0.87;
					break;
				case 3:
					if     (lFirstx + 0 == np->x)d = dt;
					else if(lFirstx + 1 == np->x)d = dt * 0.95;
					else if(lFirstx + 2 == np->x)d = dt * 0.90;
					else if(lFirstx + 3 == np->x)d = dt * 0.75;
					break;
				default:
					if     (lLastx - 2 == np->x)d = dt * 0.92;
					else if(lLastx - 1 == np->x)d = dt * 0.87;
					else if(lLastx - 0 == np->x)d = dt * 0.55;
					else d = dt;
					break;

				}

				if(d>254.0)d=254.0; else if(d<0)d=0;

				np->volume = (unsigned char)d;
				if(d == dt) dt = (double)np->volume;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 9)){ //ÉrÉuÉâÅ[Ég(ÉxÅ[ÉXÇÕè„Ç…ì å∏êäÉpÉ^Å[Éì) 2014.05.01
		dt = 1.02;
		for(i = 0; i < vnum; i++){
			di = (double)i; dnum = (double)vlen - 0.8f;
			dt += 0.0075; if(dt>1.3)dt=1.3;
			if(np->volume != VOLDUMMY && vlen > 1){
				if(dnum > 0){
					d = 1.0f - ((exp( (di/dnum)*(di/dnum) ) - 1.0f) / 1.718281828459045);
				}else d = 1.0f;
				d = d * (double)ucMax; 
				if(i%2==0)d=d*dt;else d=d/dt;
				
				if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 10)){ //ÉâÉìÉ_ÉÄ 2014.05.15
		for(i = 0; i < vnum; i++){
			double x, y;
			double s, t;
			double r1, r2;
			di = (double)i; dnum = (double)vlen;
			if(np->volume != VOLDUMMY){
				// àÍólï™ïzÇ…è]Ç§ã[éóóêêî x, y ÇçÏÇÈ
				do { x = (double)rand()/(double)RAND_MAX; } while (x == 0.0); // x Ç™ 0 Ç…Ç»ÇÈÇÃÇîÇØÇÈ
				y = (double)rand()/(double)RAND_MAX;
    
				// s, t ÇÃåvéZ
				s = sqrt(-2.0 * log(x)); // C, Java Ç≈ÇÕ log Ç™é©ëRëŒêî
				t = 2.0 * PI * y;
    
				// ïWèÄê≥ãKï™ïzÇ…è]Ç§ã[éóóêêî r1, r2
				r1 = s * cos(t);  r2 = s * sin(t);
				double dval = (double)np->volume + (i%2==1 ? r1 : r2) * 12.0;
				if(dval < 0)dval = 0; else if(dval > 250)dval = 250.0f;

				np->volume = (unsigned char)dval;
		
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 7)){ //É‘ÇÃÇÊÇ§Ç»Çréöã»ê¸ 2014.05.15
		//if(num < 4)dnum = (double)num;
		//else if(num == 4)dnum = (double)num-0.5f;
		dnum = (double)vlen / 2.0;
		long halfnum = (vlen +1) / 2; if (halfnum <= 0) halfnum = 1;

		for(i = 0; i < halfnum && i < vnum; i++){ //è„Ç…ì 
			di = (double)i;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = 1.0f - ((exp( (di/dnum)*(di/dnum) ) - 1.0f) / 1.718281828459045);
				}else d = 1.0f;
				d = d * ((double)ucMax / 2.0) + ((double)ucMax / 2.0); if( d > 254.0) d = 254.0; else if(d < 0)d = 0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}

		for(i = halfnum; i < vnum; i++){ //â∫Ç…ì 
			di = (double)(i - halfnum); dnum = (double)vlen / 2.0;
			if(np->volume != VOLDUMMY){
				if(dnum > 0){
					d = (exp( pow(((dnum-di)/dnum), 2)) - 1.0f) / 1.718281828459045;
				}else d = 1.0f;
				d = d * (double)ucMax / 2.0; if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}
	return TRUE;
}

char *test[16]={
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"10",
	"11",
	"12",
	"13",
	"14",
	"15",
};
//âπïàÇÃêKîˆÇëçÉ`ÉFÉbÉN
BOOL OrgData::CheckNoteTail(char tr)
{
	NOTELIST *np;
	NOTELIST *tail,*head;
	np = info.tdata[tr].note_list;
//	MessageBox(hWnd,test[track],test[track],MB_OK);
	while(np != NULL){
		if(np->y != KEYDUMMY){//í∑Ç≥ÇÃÇ†ÇÈâπïàÇ»ÇÁ
			tail = np;
			head = np->to;
			while(head != NULL && head->y == KEYDUMMY){
	//			MessageBox(hWnd,test[track],test[track],MB_OK);
				head = head->to;
			}
			if(head == NULL)return TRUE;
			else{
				if(tail->x + tail->length > head->x){
					tail->length = (unsigned char)(head->x - tail->x);
				}
//				np = head->from;// =to Çå©âzÇµÇƒ
			}
		}
		np = np->to;
	}
	return TRUE;	
}

//[êV]âπïÑÇPowerî{Ç…à¯Ç´êLÇŒÇ∑
BOOL OrgData::EnlargeAllNotes(int Power)
{
	if(Power<=0) return FALSE;
	int i,j;
	NOTELIST *np;
	for(i=0;i<16;i++){
		np = info.tdata[i].note_list; //èâÇﬂÇÃâπïÑ
		if(np != NULL){
			while(np != NULL){
				j = np->x * Power;
				np->x = j; //î{ÇµÉ}Å[ÉX
				j = np->length * Power;
				if(j>255)j=255;
				if(info.tdata[i].pipi != 0)j = np->length;
				if(np->y != KEYDUMMY /*&& i < MAXMELODY*/) np->length = (unsigned char)j; //2014.05.02 Modify
				np = np->to;
			}
		}
		
	}
	for(i=0;i<16;i++){ //îOÇÃÇΩÇﬂêKîˆÇå©Ç∆Ç≠Ç©ÅB
		CheckNoteTail(i);
	}
	info.wait = info.wait / Power; if(info.wait <=0) info.wait = 1;
	info.repeat_x = info.repeat_x * Power;
	info.end_x = info.end_x * Power;
	j = info.dot * Power;
	if(j < 256 && j > 0) info.dot = (unsigned char)j;
	scr_data.ChangeHorizontalRange(info.end_x);
	//MakeMusicParts(info.line,info.dot);//ÉpÅ[ÉcÇê∂ê¨
	//MakePanParts(info.line,info.dot);

	return TRUE;
}
//[êV]âπïÑÇ1/Powerî{Ç…èkÇ±ÇﬂÇÈ
BOOL OrgData::ShortenAllNotes(int Power)
{
	if(Power<=1)return FALSE;
	int i,j,k;
	NOTELIST *np, *np2;
	for(i=0;i<16;i++){
		np = info.tdata[i].note_list; //èâÇﬂÇÃâπïÑ
		if(np != NULL){
			while(np != NULL){
				np2 = np->to;
				k = np->x % Power; //Ç±ÇÍÇ™0Ç≈Ç»Ç¢Ç∆Ç´ÇÕñ≥èåèÇ≈è¡Ç∑ÇÊÅB
				if(k==0){
					j = np->x / Power;			
					np->x = j; //î{ÇµÉ}Å[ÉX

					j = np->length / Power;
					//í∑Ç≥ÇPÇÃâπïÑÇÕÇ©ÇÌÇ¢ÇªÇ§ÇæécÇµÇƒÇ‚ÇÎÇ§ÅB
					np->length = (unsigned char)(j <= 0 ? 1 : j);
				}else{
					//âπïÑÇè¡Ç∑ÇÊÅB
					PARCHANGE p;
					p.track = i;
					p.x1 = np->x;
					p.x2 = p.x1 ;
					DelateNoteData(&p);
				}
				np = np2;
			}
		}
		
	}
	for(i=0;i<16;i++){ //îOÇÃÇΩÇﬂêKîˆÇå©Ç∆Ç≠Ç©ÅB
		CheckNoteTail(i);
	}

	info.wait = info.wait * Power;
	 
	info.repeat_x = info.repeat_x / Power;
	info.end_x = info.end_x / Power;

	j = info.dot / Power;
	if(j < 256 && j > 0) info.dot = (unsigned char)j;
	scr_data.ChangeHorizontalRange(info.end_x);
	//MakeMusicParts(info.line,info.dot);//ÉpÅ[ÉcÇê∂ê¨
	//MakePanParts(info.line,info.dot);

	return TRUE;
	
}

//[êV]ãÛîíÇñÑÇﬂÇÈ 2014.05.01
BOOL OrgData::EnsureEmptyArea(PARCHANGE *pc, int Function)
{
	int j, iLength, iFlg;
	int iBufMax;
	long num = 0, tmpx;
	NOTECOPY nc;
	NOTELIST *np, *p, *npnext;
	PARCHANGE tmpc;

	nc.x1_1 = pc->x1; nc.x1_2 = pc->x2; num = GetNoteNumber(pc->track, nc.x1_1, nc.x1_2);
	np = info.tdata[pc->track].note_list;
	if(np == NULL || num == 0)return FALSE;
	
	iBufMax =  (pc->x2 - pc->x1 + 4096);

	while(np != NULL && np->x < pc->x1)np = np->to;

	for(;np != NULL && np->x <= pc->x2; np = npnext){
		iLength = np->length;
		npnext = np->to;

		for(j = 2; j <= iLength; j++){
			iFlg = 1;
			tmpx = np->x + j - 1;
			if(tmpx >= pc->x1  &&  tmpx <= pc->x2){
				for(p = np; p!= NULL && p->x <= np->x+iLength-1; p = p->to){
					if(p->x == tmpx){
						iFlg = 0;
						break;
					}
				}

				int l_len = (volChangeUseNoteLength ? iLength : volChangeLength);
				if(Function == 8 && iFlg == 1 && (tmpx >= np->x + l_len || tmpx < np->x + l_len - 3))
					iFlg = 0; //ç≈å„Ç≈Ç»ÇØÇÍÇŒÇÌÇ¥ÇÌÇ¥í«â¡ÇµÇ»Ç≠ÇƒÇ‡Ç¢Ç¢Ç≈ÇµÇÂÇ§ÅB

				if(Function < 20){
					if(iFlg == 1){
						if(FALSE == SetVolume(tmpx, np->volume)){
							return FALSE;
						}
					}
				}else if(Function == 20){
					if(iFlg == 0){
						if(FALSE == CutVolume(tmpx, 0)){ //ëÊìÒà¯êîÇÕà”ñ°Ç»Ç¢Ç∆évÇ§ÅB
							return FALSE;
						}
					}

				}
			}
		}
		if(Function >= 1 && Function < 20){
			tmpc.track = pc->track;  tmpc.x1 = np->x;  tmpc.x2 = np->x + np->length - 1;  tmpc.a = 1;  
			tmpc.mode = (unsigned char)(MODEDECAY + Function);
			if(np->y!=KEYDUMMY)ChangeVolumeData(&tmpc, 1);
		}else if(Function == 20){ //∏ÿ±
			//âΩÇ‡ÇµÇ»Ç¢
		}
	}
	return TRUE;
}

//iFullTrack==1ÇÃÇ∆Ç´ÇÕiTrackÇÕñ≥éãÅB
bool OrgData::CopyNoteDataToCB(NOTECOPY *nc, int iTrack, int iFullTrack )
{
	/*
	int i,j,t,n;
	PARCHANGE pc;//ÉyÅ[ÉXÉgóÃàÊÉNÉäÉAóp
	NOTELIST *np;
//	NOTELIST *p_in1,*p_in2;//ë}ì¸Ç∑Ç◊Ç´èÍèä
	NOTELIST *p_list1,*p_list2;//ë}ì¸Ç∑Ç◊Ç´ÉäÉXÉg
	NOTELIST *work;//ÉèÅ[ÉNÉGÉäÉA
	NOTELIST *wp;//ÉèÅ[ÉNÉ|ÉCÉìÉ^
	long ind_x;//ÉCÉìÉfÉbÉNÉXílÅix)
	long copy_num;//ÉRÉsÅ[Ç∑ÇÈâπïàÇÃêî

	ClearVirtualCB();
	AddStartToVirtualCB(); //ÉNÉäÉbÉvÉ{Å[ÉhÇ…ë„ì¸äJén
	AddIntegerToVirtualCB(iFullTrack);
	AddIntegerToVirtualCB(iTrack);
	AddIntegerToVirtualCB(nc->x1_1); //Ç±Ç±Ç©ÇÁ
	AddIntegerToVirtualCB(nc->x1_2); //Ç±Ç±Ç‹Ç≈

	//Ç†Ç∆ÇÕÇ∆Ç…Ç©Ç≠ÅAÉNÉäÉbÉvÉ{Å[ÉhÇ…ì‡óeÇà⁄Ç∑

	ind_x = nc->x1_1;

	for(t=0;t<MAXTRACK;t++){
		if(iFullTrack==1 || iTrack==t){
			copy_num = GetNoteNumber(t,nc);//ÇªÇÃîÕàÕÇ…égÇÌÇÍÇƒÇ¢ÇÈâπïÑÇÃêîÇåüèo
			AddIntegerToVirtualCB(copy_num); //âπïÑÇÕâΩå¬ÅH

			if(copy_num == 0){
				AddTrackSeparater(); //ÅóÇë}ì¸
				continue;
			}

			//wp = work = (NOTELIST *)malloc(sizeof(NOTELIST)*copy_num);//ÉèÅ[ÉNópÇ…óÃàÊÇämï€
			//ÉèÅ[ÉNóÃàÊÇ…ÉRÉsÅ[
			np = info.tdata[nc->track1].note_list;
			while(np != NULL && np->x < nc->x1_1)np = np->to;
			if(np == NULL){
				//free( work );
				AddTrackSeparater(); //ÅóÇë}ì¸
				continue;//ÉRÉsÅ[å≥Ç…âπïàñ≥Çµ(ì™ÇåüçıíÜÇ…OUT)...ÇªÇÒÇ»èÛãµÇ†ÇÈÇÃÇ©ÅH
			}
			for(i = 0; i < copy_num; i++){//ÇPÉyÅ[ÉXÉg
				//wp->length = np->length;
				//wp->pan    = np->pan;
				//wp->volume = np->volume;
				//wp->y      = np->y;
				//ÉCÉìÉfÉbÉNÉXílÇÕç∑Çµà¯Ç≠
				//wp->x      = np->x - ind_x;
				//éüÇ÷
				AddIntegerToVirtualCB((int)np->length);
				AddIntegerToVirtualCB((int)np->pan);
				AddIntegerToVirtualCB((int)np->volume);
				AddIntegerToVirtualCB((int)np->y);
				n = np->x - ind_x;
				AddIntegerToVirtualCB(n);
				np = np->to;
				//wp++;
			}
			//free( work );
			AddTrackSeparater(); //ÅóÇë}ì¸
		}
	}
	SetClipBoardFromVCB(); //ê^ÇÃÉNÉäÉbÉvÉ{Å[ÉhÇ…
*/
	return true;
}


BOOL OrgData::SwapTrack(NOTECOPY *pc)
{
	TRACKDATA tmp;
	char w1 = info.tdata[pc->track1].wave_no; // crash fix
	char w2 = info.tdata[pc->track2].wave_no;
	
	memcpy(&tmp, &info.tdata[ pc->track1 ], sizeof(TRACKDATA));
	memcpy(&info.tdata[ pc->track1 ], &info.tdata[ pc->track2 ], sizeof(TRACKDATA));
	memcpy(&info.tdata[ pc->track2 ], &tmp, sizeof(TRACKDATA));

	if (pc->track1 / 8 != pc->track2 / 8) { // crash fix
		info.tdata[pc->track1].wave_no = w1;
		info.tdata[pc->track2].wave_no = w2;
	}

	/*unsigned char uctmp;
	uctmp = def_pan[pc->track1];
	def_pan[pc->track1] = def_pan[pc->track2];
	def_pan[pc->track2] = uctmp;

	uctmp = def_volume[pc->track1];
	def_volume[pc->track1] = def_volume[pc->track2];
	def_volume[pc->track2] = uctmp;*/

	//char ctmp;
	//ctmp = mute[pc->track1];
	//mute[pc->track1] = mute[pc->track2];
	//mute[pc->track2] = ctmp;


	return TRUE;
}
