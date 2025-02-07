#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Setting.h"
#include "Gdi.h"
#include "DefOrg.h"
#include "OrgData.h"
#include "rxoFunction.h"
#include "Scroll.h"

#define PI 3.14159265358979323846
/*
//•ÒW—p\‘¢‘Ì///////////////
typedef struct{//œƒRƒs[\‘¢‘Ì
	char track1;//‚±‚Ìƒgƒ‰ƒbƒN‚Ì
	long x1_1;//‚±‚±‚©‚ç
	long x1_2;//‚±‚±‚Ü‚Å‚ğ
	char track2;//‚±‚Ìƒgƒ‰ƒbƒN‚Ì
	long x2;//‚±‚±‚É
	long num;//‰½‰ñ
}NOTECOPY;

typedef struct{//œƒgƒ‰ƒ“ƒXƒ|[ƒg‚âƒpƒ“ƒ|ƒbƒg
	char track;//‚±‚Ìƒgƒ‰ƒbƒN‚Ì
	long x1;//‚±‚±‚©‚ç
	long x2;//‚±‚±‚Ü‚Å‚ğ
	unsigned char a;//‚±‚ê‚¾‚¯ 
	unsigned char mode;//‚½‚·i‚Ğ‚­j
}PARCHANGE;
*/

extern int volChangeLength;
extern bool volChangeUseNoteLength;
extern bool volChangeSetNoteLength;

BOOL OrgData::DelateNoteData(PARCHANGE *pc)
{
	NOTELIST *np;
	NOTELIST *np_from;//‚±‚¢‚Â‚Æ
	NOTELIST *np_to;//‚±‚¢‚Â‚ğŒq‚®
	NOTELIST *np_clear;//K”öíœ—p
	long note_x1,note_x2;//‹È‚Ì”ÍˆÍ‚ğ•Û‘¶
	np = info.tdata[pc->track].note_list;
	if(np == NULL)return FALSE;//‰¹•„‚ª–³‚¯‚ê‚ÎI—¹

	np = info.tdata[pc->track].note_list;
	note_x1 = np->x;//Å‰‚Ì‚˜‚ğ‹L˜^
	while(np->to != NULL)np = np->to;//‚¨‚í‚è‚ğŒŸõ
	note_x2 = np->x;//‚¨‚í‚è‚ğ•Û‘¶
	if(note_x1 > pc->x2 || pc->x1 > note_x2){//”ÍˆÍ‚ªŠy•ˆ‚©‚ç—£‚ê‚Ä‚¢‚é
		return FALSE;
	}else if(note_x1 < pc->x1 && pc->x2 < note_x2){//”ÍˆÍ‚ªŠy•ˆ‚É“à•ï‚³‚ê‚Ä‚¢‚éB
		np = info.tdata[pc->track].note_list;
		while(np->x < pc->x1)np = np->to;//”ÍˆÍ‚Ì“ª‚ğŒŸõ
		np_from = np->from;//¶’[‚ğ•Û‘¶(Ú‘±—p)
		while(np->x <= pc->x2){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;//”ÍˆÍ‚ÌK‚ğŒŸõ
		}
		np_to = np;//‰E’[‚ğ•Û‘¶(Ú‘±—p)
		//—¼’[‚ğÚ‘±
		np_from->to = np_to;
		np_to->from = np_from;
		return TRUE;

	}else if(pc->x1 <= note_x1 && pc->x2 < note_x2){//Á‚·”ÍˆÍ‚Ì“ª‚É‰¹‚ª‘¶İ‚µ‚È‚¢B
		np = info.tdata[pc->track].note_list;
		while(np->x <= pc->x2){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;//”ÍˆÍ‚ÌK‚ğŒŸõ
		}
		np_to = np;//‰E’[‚ğ•Û‘¶(Ú‘±—p)
		np->from = NULL;//‚±‚¢‚Â‚ğ“ª‚É‚·‚é
		info.tdata[pc->track].note_list = np;//ƒŠƒXƒg‚É“o˜^
		return TRUE;
	}else if(note_x1 < pc->x1 && note_x2 <= pc->x2 ){//Á‚·”ÍˆÍ‚ÌK‚É‰¹‚ª‘¶İ‚µ‚È‚¢B
		np = info.tdata[pc->track].note_list;
		while(np->to->x < pc->x1)np = np->to;//”ÍˆÍ‚Ì“ª‚ğŒŸõ
		np_clear = np;//ÅŒã‚É‚È‚é‰¹•ˆ‚ÌƒAƒhƒŒƒX
		np = np->to;
		while(np != NULL){
			np->length = 0;
			np->y=KEYDUMMY; np->pan=PANDUMMY; np->volume=VOLDUMMY;
			np = np->to;
		}
		np_clear->to = NULL;
		return TRUE;
	}else if(note_x1 >= pc->x1 && note_x2 <= pc->x2 ){//Š®‘SÁ‹
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
			np = np->to;
			wp++;
		}

		sn->data[j].size = copy_num;
		sn->data[j].data = work;
	}
	return TRUE;
}

BOOL OrgData::PasteNoteData(SAVEDNOTE* sn, char track, long x, long num)
{
	int i, j, k;
	PARCHANGE pc;
	NOTELIST* np;
	NOTELIST* p_list1, * p_list2;
	NOTELIST* work;
	NOTELIST* wp;
	long copy_num;
	SAVEDTRACK* st;

	for (k = sn->track1; k <= sn->track2; ++k) {
		st = &sn->data[k];

		copy_num = st->size;
		work = st->data;
		if (work == NULL) {
			continue;
		}

		if (sn->track1 != sn->track2) {
			track = k;
		}

		pc.track = track;
		pc.x1 = x;
		pc.x2 = x + sn->length * num - 1;
		DelateNoteData(&pc);

		np = p_list1 = p_list2 = SearchNote(info.tdata[track].note_p);
		if (np == NULL) {
			return FALSE;
		}
		np->length = 1;
		for (i = 1; i < copy_num * num; i++) {
			np = SearchNote(info.tdata[track].note_p);
			if (np == NULL) {
				break;
			}
			np->length = 1;
			p_list2->to = np;
			np->from = p_list2;
			p_list2 = np;
		}

		long index;
		np = p_list1;
		for (j = 0; j < num;j++) {
			wp = work;
			index = x + sn->length * j;
			for (i = 0; i < copy_num; i++) {
				np->length = wp->length;
				np->pan = wp->pan;
				np->volume = wp->volume;
				np->y = wp->y;

				np->x = wp->x + index;

				np = np->to;
				wp++;
				if (np == NULL) break;
			}
			if (np == NULL) break;
		}

		np = info.tdata[track].note_list;
		if (np == NULL) {
			info.tdata[track].note_list = p_list1;
			p_list1->from = NULL;
			p_list2->to = NULL;
		}
		else {
			if (np->x > p_list2->x) {
				np->from = p_list2;
				p_list2->to = np;
				p_list1->from = NULL;
				info.tdata[track].note_list = p_list1;
				continue;
			}
			while (np->to != NULL && np->to->x < x)np = np->to;
			if (np->to == NULL) {
				np->to = p_list1;
				p_list1->from = np;
				p_list2->to = NULL;
				continue;
			}
			else {
				np->to->from = p_list2;
				p_list2->to = np->to;
				np->to = p_list1;
				p_list1->from = np;
				continue;
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
	for(i = 0; i < copy_num; i++){//‚Pƒy[ƒXƒg
		wp->length = np->length;
		wp->pan    = np->pan;
		wp->volume = np->volume;
		wp->y      = np->y;
		//ƒCƒ“ƒfƒbƒNƒX’l‚Í·‚µˆø‚­
		wp->x      = np->x - ind_x;
		//Ÿ‚Ö
		np = np->to;
		wp++;
	}

	//ƒy[ƒXƒgæ‚ğƒNƒŠƒA
	pc.track = nc->track2;//‚±‚Ìƒgƒ‰ƒbƒN‚Ì
	pc.x1 = nc->x2;//‚±‚±‚©‚ç
	pc.x2 = nc->x2 + (nc->x1_2+1 - nc->x1_1)*nc->num-1;//‚±‚±‚Ü‚Å‚ğx1_1+1(x1_2‚Í|‚P‚µ‚½’l)
	DelateNoteData(&pc);//Á‚·

	//ƒy[ƒXƒgƒŠƒXƒg‚Ì¶¬
	np = p_list1 = p_list2 = SearchNote(info.tdata[nc->track2].note_p);
	if(np == NULL){//–¢g—p‰¹•ˆ‚ª•s‘«
		free( work );
		return FALSE;
	}
	np->length = 1;//‰¼¶¬
	for(i = 1; i < copy_num*nc->num; i++){
		np = SearchNote(info.tdata[nc->track2].note_p);
		if(np == NULL){//–¢g—p‰¹•ˆ‚ª•s‘«
			//free( work );
			//return FALSE;
			break;
		}
		np->length = 1;//‰¼¶¬
		p_list2->to = np;
		np->from = p_list2;
		p_list2 = np;
	}

	long index;
	//ƒy[ƒXƒgƒŠƒXƒg‚Éƒy[ƒXƒg
	np = p_list1;
	for(j = 0; j < nc->num;j++){//ƒy[ƒXƒg‰ñ”
		wp = work;
		index = nc->x2+ (nc->x1_2 - nc->x1_1 +1)*j;
		for(i = 0; i < copy_num; i++){//‚Pƒy[ƒXƒg
			np->length    = wp->length;
			np->pan       = wp->pan;
			np->volume    = wp->volume; 
			np->y         = wp->y; 
			//ƒCƒ“ƒfƒbƒNƒX’l‚ğ‰Á‚¦‚é
			np->x         = wp->x + index;

			//Ÿ‚Ö
			np = np->to;
			wp++;
			if (np == NULL) break;
		}
		if (np == NULL) break;
	}



	np = info.tdata[nc->track2].note_list;
	if(np == NULL){//Š®‘SV‹K
		info.tdata[nc->track2].note_list = p_list1;
		p_list1->from = NULL;
		p_list2->to = NULL;
	}else{
		//‘}“ü‚·‚×‚«êŠ‚ğŒŸo
//		np = info.tdata[nc->track2].note_list;
		if(np->x > p_list2->x){//“ª‚É’Ç‰Á
			np->from = p_list2;
			p_list2->to = np;
			p_list1->from = NULL;
			info.tdata[nc->track2].note_list = p_list1;
			free( work );
			return TRUE;
		}
		while(np->to != NULL && np->to->x < nc->x2)np = np->to;
		if(np->to == NULL){//K‚É’Ç‰Á
			np->to = p_list1;
			p_list1->from = np;
			p_list2->to = NULL;
			free( work );
			return TRUE;
		}else{//‘}“ü
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
//ƒpƒ“‚ÍÅ¬‚Å‚OÅ‘å‚ª‚P‚Q‚Æ‚È‚é
BOOL OrgData::ChangePanData(PARCHANGE *pc)
{
	int i;
	long num = 0;
	NOTECOPY nc;
	NOTELIST *np;
	//‰¹•ˆ‚Ì”‚ğŒŸo
	nc.x1_1 = pc->x1;
	nc.x1_2 = pc->x2;
	num = GetNoteNumber(pc->track, nc.x1_1, nc.x1_2);

	np = info.tdata[pc->track].note_list;
	if(np == NULL || num == 0)return FALSE;
	while(np != NULL && np->x < pc->x1)np = np->to;
	if(pc->a == 254){ //2014.05.03 ‹t“]
		for(i = 0; i < num; i++){
			if(np->pan == PANDUMMY);
			else np->pan = 12 - np->pan;
			np = np->to;
		}
		
	}else{
		if(pc->mode == MODEPARADD){//‰ÁZƒ‚[ƒh
			for(i = 0; i < num; i++){
				if(np->pan == PANDUMMY);
				else if(np->pan + pc->a > 12)np->pan = 12;
				else np->pan += pc->a;
				np = np->to;
			}
		}else{//Œ¸Zƒ‚[ƒh
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

//ƒL[‚Í95‚ªMAX
BOOL OrgData::ChangeTransData(PARCHANGE *pc)
{
	int i;
	long num = 0;
	NOTECOPY nc;
	NOTELIST *np;
	//‰¹•ˆ‚Ì”‚ğŒŸo
	nc.x1_1 = pc->x1;
	nc.x1_2 = pc->x2;
	num = GetNoteNumber(pc->track, nc.x1_1, nc.x1_2);

	np = info.tdata[pc->track].note_list;
	if(np == NULL || num == 0)return FALSE;
	while(np != NULL && np->x < pc->x1)np = np->to;
	if(pc->mode == MODEPARADD){//‰ÁZƒ‚[ƒh
		for(i = 0; i < num; i++){
			if(np->y == KEYDUMMY);
			else if(np->y + pc->a > 95)np->y = 95;
			else np->y += pc->a;
			np = np->to;
		}
	}else{//Œ¸Zƒ‚[ƒh
		for(i = 0; i < num; i++){
			if(np->y == KEYDUMMY);
			else if(np->y - pc->a < 0)np->y = 0;
			else np->y -= pc->a;
			np = np->to;
		}
	}
	return TRUE;
}
//ƒ”ƒHƒŠƒ…[ƒ€‚Í254‚ªMAX
BOOL OrgData::ChangeVolumeData(PARCHANGE *pc, char mode)
{
	int i,j;
	long num = 0, lFirstx = 0, lLastx = 0;
	NOTECOPY nc;
	NOTELIST *np;
	unsigned char uc, ucMax = 0, ucMin = 254, ucFirstFlg = 255, ucFirst, ucLast;
	double d = VOLDUMMY, dnum, di, dnorm = 0, dnormnum = 0, dt;
	//‰¹•ˆ‚Ì”‚ğŒŸo
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

	//“ª‚¾‚µ
	while (np != NULL && np->x < pc->x1)np = np->to;

	//ucMax, ucMin‚ğ’²‚×‚é
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
	if(dnormnum > 0)dnorm = dnorm / dnormnum; //•½‹Ï’l

	//Šª‚«–ß‚µ ‚Æ “ª‚¾‚µ
	np = info.tdata[pc->track].note_list;
	while(np != NULL && np->x < pc->x1)np = np->to;

	if (mode == 1 && !volChangeUseNoteLength && pc->mode >= MODEDECAY && pc->mode < MODEDECAY + 20) {
		lLastx = lFirstx + vlen - 1;
	}

	if(pc->mode == MODEPARADD){//‰ÁZƒ‚[ƒh
		for(i = 0; i < num; i++){
			if(np->volume == VOLDUMMY);
			else if(np->volume + pc->a > 254)np->volume = 254;
			else np->volume += pc->a;
			np = np->to;
		}
	}else if(pc->mode == MODEPARSUB){//Œ¸Zƒ‚[ƒh
		for(i = 0; i < num; i++){
			if(np->volume == VOLDUMMY);
			else if(np->volume - pc->a < 0)np->volume = 0;
			else np->volume -= pc->a;
			np = np->to;
		}
	}else if(pc->mode == MODEMULTIPLY){//æZƒ‚[ƒh
		// Šù‘¶‚Ìvolume‚ğ a / 128”{ ‚·‚é
		for(i = 0; i < num; i++){
			d = (double)np->volume * (double) pc->a / (double)128.0;
			if(d>254.0)d=254.0; else if(d<0)d=0;

			uc = (unsigned char)d;

			if(np->volume == VOLDUMMY);
			else np->volume = uc;
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 3)){ //üŒ`Œ¸Š 2014.05.01
		for(i = 0; i < vnum; i++){
			if(np->volume != VOLDUMMY){
				d = ((double)ucMax - (double)0) / (double)(vlen - 0) * (double)(vlen - 0 - i); if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}
	/*else if (pc->mode == (MODEDECAY + 14)) { // LINEAR 10 L
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
	else if(pc->mode == (MODEDECAY + 12)){ //‚Qæã¸ 2014.05.17
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
	}else if(pc->mode == (MODEDECAY + 13)){ //üŒ`ã¸ 2014.05.17
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
	}else if(pc->mode == (MODEDECAY + 2)){ //ã‚É“ÊŒ¸Š 2014.05.01
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
	}else if(pc->mode == (MODEDECAY + 1)){ //ã‚É“ÊŒ¸Š 2014.05.01
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
	}else if(pc->mode == (MODEDECAY + 4)){ //‰º‚É“ÊŒ¸Š 2014.05.01
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
	}else if(pc->mode == (MODEDECAY + 5)){ //‰º‚É“ÊŒ¸Š 2014.05.01
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
	}else if(pc->mode == (MODEDECAY + 6)){ //ƒm[ƒ}ƒ‰ƒCƒY 2014.05.01
		for(i = 0; i < vnum; i++){
			//di = (double)i; dnum = (double)num;
			if(np->volume != VOLDUMMY){
				//np->volume = (unsigned char)dnorm;
				np->volume = org_data.def_volume[pc->track];//‰Šú‰»
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 11)){ //ƒOƒ‰ƒf[ƒVƒ‡ƒ“ 2014.05.01
		for(i = 0; i < vnum; i++){
			di = (double)i; dnum = (double)vlen;
			if(np->volume != VOLDUMMY && vlen > 1){
				d = ((double)ucFirst - (double)ucLast) / (double)(vlen - 1) * (double)(vlen - 1 - i) + (double)ucLast ;
				if(d>254.0)d=254.0; else if(d<0)d=0;
				np->volume = (unsigned char)d;
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 8)){ //ÅŒã‚ÅŒ¸Š
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
	}else if(pc->mode == (MODEDECAY + 9)){ //ƒrƒuƒ‰[ƒg(ƒx[ƒX‚Íã‚É“ÊŒ¸Šƒpƒ^[ƒ“) 2014.05.01
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
	}else if(pc->mode == (MODEDECAY + 10)){ //ƒ‰ƒ“ƒ_ƒ€ 2014.05.15
		for(i = 0; i < vnum; i++){
			double x, y;
			double s, t;
			double r1, r2;
			di = (double)i; dnum = (double)vlen;
			if(np->volume != VOLDUMMY){
				// ˆê—l•ª•z‚É]‚¤‹[——” x, y ‚ğì‚é
				do { x = (double)rand()/(double)RAND_MAX; } while (x == 0.0); // x ‚ª 0 ‚É‚È‚é‚Ì‚ğ”ğ‚¯‚é
				y = (double)rand()/(double)RAND_MAX;
    
				// s, t ‚ÌŒvZ
				s = sqrt(-2.0 * log(x)); // C, Java ‚Å‚Í log ‚ª©‘R‘Î”
				t = 2.0 * PI * y;
    
				// •W€³‹K•ª•z‚É]‚¤‹[——” r1, r2
				r1 = s * cos(t);  r2 = s * sin(t);
				double dval = (double)np->volume + (i%2==1 ? r1 : r2) * 12.0;
				if(dval < 0)dval = 0; else if(dval > 250)dval = 250.0f;

				np->volume = (unsigned char)dval;
		
			}
			np = np->to;
		}
	}else if(pc->mode == (MODEDECAY + 7)){ //ƒÔ‚Ì‚æ‚¤‚È‚rš‹Èü 2014.05.15
		//if(num < 4)dnum = (double)num;
		//else if(num == 4)dnum = (double)num-0.5f;
		dnum = (double)vlen / 2.0;
		long halfnum = (vlen +1) / 2; if (halfnum <= 0) halfnum = 1;

		for(i = 0; i < halfnum && i < vnum; i++){ //ã‚É“Ê
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

		for(i = halfnum; i < vnum; i++){ //‰º‚É“Ê
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
//‰¹•ˆ‚ÌK”ö‚ğ‘ƒ`ƒFƒbƒN
BOOL OrgData::CheckNoteTail(char tr)
{
	NOTELIST *np;
	NOTELIST *tail,*head;
	np = info.tdata[tr].note_list;
//	MessageBox(hWnd,test[track],test[track],MB_OK);
	while(np != NULL){
		if(np->y != KEYDUMMY){//’·‚³‚Ì‚ ‚é‰¹•ˆ‚È‚ç
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
//				np = head->from;// =to ‚ğŒ©‰z‚µ‚Ä
			}
		}
		np = np->to;
	}
	return TRUE;	
}

//[V]‰¹•„‚ğPower”{‚Éˆø‚«L‚Î‚·
BOOL OrgData::EnlargeAllNotes(int Power)
{
	if(Power<=0) return FALSE;
	int i,j;
	NOTELIST *np;
	for(i=0;i<16;i++){
		np = info.tdata[i].note_list; //‰‚ß‚Ì‰¹•„
		if(np != NULL){
			while(np != NULL){
				j = np->x * Power;
				np->x = j; //”{‚µƒ}[ƒX
				j = np->length * Power;
				if(j>255)j=255;
				if(info.tdata[i].pipi != 0)j = np->length;
				if(np->y != KEYDUMMY /*&& i < MAXMELODY*/) np->length = (unsigned char)j; //2014.05.02 Modify
				np = np->to;
			}
		}
		
	}
	for(i=0;i<16;i++){ //”O‚Ì‚½‚ßK”ö‚ğŒ©‚Æ‚­‚©B
		CheckNoteTail(i);
	}
	info.wait = info.wait / Power; if(info.wait <=0) info.wait = 1;
	info.repeat_x = info.repeat_x * Power;
	info.end_x = info.end_x * Power;
	j = info.dot * Power;
	if(j < 256 && j > 0) info.dot = (unsigned char)j;
	scr_data.ChangeHorizontalRange(info.dot * info.line * MAXHORZMEAS);
	//MakeMusicParts(info.line,info.dot);//ƒp[ƒc‚ğ¶¬
	//MakePanParts(info.line,info.dot);

	return TRUE;
}
//[V]‰¹•„‚ğ1/Power”{‚Ék‚±‚ß‚é
BOOL OrgData::ShortenAllNotes(int Power)
{
	if(Power<=1)return FALSE;
	int i,j,k;
	NOTELIST *np, *np2;
	for(i=0;i<16;i++){
		np = info.tdata[i].note_list; //‰‚ß‚Ì‰¹•„
		if(np != NULL){
			while(np != NULL){
				np2 = np->to;
				k = np->x % Power; //‚±‚ê‚ª0‚Å‚È‚¢‚Æ‚«‚Í–³ğŒ‚ÅÁ‚·‚æB
				if(k==0){
					j = np->x / Power;			
					np->x = j; //”{‚µƒ}[ƒX

					j = np->length / Power;
					//’·‚³‚P‚Ì‰¹•„‚Í‚©‚í‚¢‚»‚¤‚¾c‚µ‚Ä‚â‚ë‚¤B
					np->length = (unsigned char)(j <= 0 ? 1 : j);
				}else{
					//‰¹•„‚ğÁ‚·‚æB
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
	for(i=0;i<16;i++){ //”O‚Ì‚½‚ßK”ö‚ğŒ©‚Æ‚­‚©B
		CheckNoteTail(i);
	}

	info.wait = info.wait * Power;
	 
	info.repeat_x = info.repeat_x / Power;
	info.end_x = info.end_x / Power;

	j = info.dot / Power;
	if(j < 256 && j > 0) info.dot = (unsigned char)j;
	scr_data.ChangeHorizontalRange(info.dot * info.line * MAXHORZMEAS);
	//MakeMusicParts(info.line,info.dot);//ƒp[ƒc‚ğ¶¬
	//MakePanParts(info.line,info.dot);

	return TRUE;
	
}

//[V]‹ó”’‚ğ–„‚ß‚é 2014.05.01
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
					iFlg = 0; //ÅŒã‚Å‚È‚¯‚ê‚Î‚í‚´‚í‚´’Ç‰Á‚µ‚È‚­‚Ä‚à‚¢‚¢‚Å‚µ‚å‚¤B

				if(Function < 20){
					if(iFlg == 1){
						if(FALSE == SetVolume(tmpx, np->volume)){
							return FALSE;
						}
					}
				}else if(Function == 20){
					if(iFlg == 0){
						if(FALSE == CutVolume(tmpx, 0)){ //‘æ“ñˆø”‚ÍˆÓ–¡‚È‚¢‚Æv‚¤B
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
		}else if(Function == 20){ //¸Ø±
			//‰½‚à‚µ‚È‚¢
		}
	}
	return TRUE;
}

//iFullTrack==1‚Ì‚Æ‚«‚ÍiTrack‚Í–³‹B
bool OrgData::CopyNoteDataToCB(NOTECOPY *nc, int iTrack, int iFullTrack )
{
	/*
	int i,j,t,n;
	PARCHANGE pc;//ƒy[ƒXƒg—ÌˆæƒNƒŠƒA—p
	NOTELIST *np;
//	NOTELIST *p_in1,*p_in2;//‘}“ü‚·‚×‚«êŠ
	NOTELIST *p_list1,*p_list2;//‘}“ü‚·‚×‚«ƒŠƒXƒg
	NOTELIST *work;//ƒ[ƒNƒGƒŠƒA
	NOTELIST *wp;//ƒ[ƒNƒ|ƒCƒ“ƒ^
	long ind_x;//ƒCƒ“ƒfƒbƒNƒX’lix)
	long copy_num;//ƒRƒs[‚·‚é‰¹•ˆ‚Ì”

	ClearVirtualCB();
	AddStartToVirtualCB(); //ƒNƒŠƒbƒvƒ{[ƒh‚É‘ã“üŠJn
	AddIntegerToVirtualCB(iFullTrack);
	AddIntegerToVirtualCB(iTrack);
	AddIntegerToVirtualCB(nc->x1_1); //‚±‚±‚©‚ç
	AddIntegerToVirtualCB(nc->x1_2); //‚±‚±‚Ü‚Å

	//‚ ‚Æ‚Í‚Æ‚É‚©‚­AƒNƒŠƒbƒvƒ{[ƒh‚É“à—e‚ğˆÚ‚·

	ind_x = nc->x1_1;

	for(t=0;t<MAXTRACK;t++){
		if(iFullTrack==1 || iTrack==t){
			copy_num = GetNoteNumber(t,nc);//‚»‚Ì”ÍˆÍ‚Ég‚í‚ê‚Ä‚¢‚é‰¹•„‚Ì”‚ğŒŸo
			AddIntegerToVirtualCB(copy_num); //‰¹•„‚Í‰½ŒÂH

			if(copy_num == 0){
				AddTrackSeparater(); //—‚ğ‘}“ü
				continue;
			}

			//wp = work = (NOTELIST *)malloc(sizeof(NOTELIST)*copy_num);//ƒ[ƒN—p‚É—Ìˆæ‚ğŠm•Û
			//ƒ[ƒN—Ìˆæ‚ÉƒRƒs[
			np = info.tdata[nc->track1].note_list;
			while(np != NULL && np->x < nc->x1_1)np = np->to;
			if(np == NULL){
				//free( work );
				AddTrackSeparater(); //—‚ğ‘}“ü
				continue;//ƒRƒs[Œ³‚É‰¹•ˆ–³‚µ(“ª‚ğŒŸõ’†‚ÉOUT)...‚»‚ñ‚Èó‹µ‚ ‚é‚Ì‚©H
			}
			for(i = 0; i < copy_num; i++){//‚Pƒy[ƒXƒg
				//wp->length = np->length;
				//wp->pan    = np->pan;
				//wp->volume = np->volume;
				//wp->y      = np->y;
				//ƒCƒ“ƒfƒbƒNƒX’l‚Í·‚µˆø‚­
				//wp->x      = np->x - ind_x;
				//Ÿ‚Ö
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
			AddTrackSeparater(); //—‚ğ‘}“ü
		}
	}
	SetClipBoardFromVCB(); //^‚ÌƒNƒŠƒbƒvƒ{[ƒh‚É
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

	unsigned char uctmp;
	uctmp = def_pan[pc->track1];
	def_pan[pc->track1] = def_pan[pc->track2];
	def_pan[pc->track2] = uctmp;

	uctmp = def_volume[pc->track1];
	def_volume[pc->track1] = def_volume[pc->track2];
	def_volume[pc->track2] = uctmp;

	//char ctmp;
	//ctmp = mute[pc->track1];
	//mute[pc->track1] = mute[pc->track2];
	//mute[pc->track2] = ctmp;


	return TRUE;
}