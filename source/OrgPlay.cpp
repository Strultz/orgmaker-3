#include "Setting.h"
#include "DefOrg.h"
#include "OrgData.h"
#include "Sound.h"
#include "resource.h"
#include "Scroll.h"
#include "Timer.h"
#include "CommCtrl.h"

extern bool gPlayMidNote;
extern HWND hwndToolbar;
extern int sSmoothScroll;
extern int iKeyPushDown[256];
extern unsigned char old_key[];
extern bool gNoteHighlights;

char timer_sw = 0;
long oplay_p;
long play_p;//現在再生位置（キャンバス）
NOTELIST *np[MAXTRACK];//現在再生準備の音符
long now_leng[MAXMELODY] = {NULL};//再生中音符の長さ
int s_solo = -1;
//DWORD lastDrawTime = -1;
//DWORD drawCatch = 0;
extern int sMetronome;
extern int NoteWidth;
void OrgData::PlayData(void)
{
	char end_cnt = MAXTRACK;
//	PlaySoundObject(1,1);
	//メロディの再生
	for(int i = 0; i < MAXMELODY; i++){
//	int i = 6;
		if(np[i] != NULL && play_p == np[i]->x) {//音が来た。
			if (s_solo != -1 && s_solo != i)
				continue;

			if(mute[i] == 0) {
				if(np[i]->y != KEYDUMMY) {
					if (gNoteHighlights && this->track == i) {
						if (old_key[i] != 255) iKeyPushDown[old_key[i]] = 0;
						iKeyPushDown[np[i]->y] = 1;
					}
					PlayOrganObject(np[i]->y,-1,i,info.tdata[i].freq, info.tdata[i].pipi);
					now_leng[i] = np[i]->length;
				}
			}
			if(np[i]->pan != PANDUMMY) ChangeOrganPan(np[i]->y,np[i]->pan,i);
			if(np[i]->volume != VOLDUMMY) ChangeOrganVolume(np[i]->y,np[i]->volume * 100 / 0x7F,i);
			np[i] = np[i]->to;//次の音符を指す
		}
		if(now_leng[i] == 0) {
			if (gNoteHighlights && this->track == i && old_key[i] != 255) iKeyPushDown[old_key[i]] = 0;
			PlayOrganObject(NULL,2,i,info.tdata[i].freq, info.tdata[i].pipi);
		}
		if(now_leng[i] > 0) now_leng[i]--;
	}
	//ドラムの再生
	for(int i = MAXMELODY; i < MAXTRACK; i++) {
		if (s_solo != -1 && s_solo != i)
			continue;

		if(np[i] != NULL && play_p == np[i]->x) {//音が来た。
			if(np[i]->y != KEYDUMMY) {//ならす
				if (mute[i] == 0) {
					if (gNoteHighlights && this->track == i) {
						if (old_key[i] != 255) iKeyPushDown[old_key[i]] = 0;
						iKeyPushDown[np[i]->y] = 1;
					}
					PlayDramObject(np[i]->y, 1, i - MAXMELODY);
				}
			}
			if(np[i]->pan != PANDUMMY) ChangeDramPan(np[i]->pan,i-MAXMELODY);
			if(np[i]->volume != VOLDUMMY) ChangeDramVolume(np[i]->volume * 100 / 0x7F,i-MAXMELODY);
			np[i] = np[i]->to;//次の音符を指す
		}

		if (gNoteHighlights && this->track == i && !IsDramPlaying(i - MAXMELODY) && old_key[i] != 255) {
			iKeyPushDown[old_key[i]] = 0;
		}
	}
	//プレイヤーに表示
	//自動スクロール
	/*if(actApp){//アクティブの時だけ
		
	}*/

	if (sMetronome) {
		if (play_p % (info.line * info.dot) == 0) {
			PlaySoundObject(1, 1);
			//PlaySound("METRO01", GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
		}
		else if (play_p % info.dot == 0) {
			PlaySoundObject(2, 1);
			//PlaySound("METRO02", GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
		}
	}
	oplay_p = play_p;
	play_p++;
	if(play_p >= info.end_x) {
		int p = oplay_p;
		play_p = info.repeat_x; // ++されるので
		SetPlayPointer(play_p);
		oplay_p = p;
	}

}
void OrgData::SetPlayPointer(long x)
{
	int i;
	for(i = 0; i < MAXTRACK; i++){
		np[i] = info.tdata[i].note_list;
		while(np[i] != NULL && np[i]->x < x)np[i] = np[i]->to; // 見るべき音符を設定		
	}
	play_p = x;
	oplay_p = x;
}
void OrgData::GetPlayPos(long* playpos, long *oplaypos) {
	if (playpos != NULL) *playpos = play_p;
	if (oplaypos != NULL) *oplaypos = oplay_p;
}

void StartPlayingSong(void) {
	MUSICINFO mi;
	long hp;
	if (!timer_sw) {
		Rxo_StopAllSoundNow();
		scr_data.GetScrollPosition(&hp, NULL);
		org_data.SetPlayPointer(hp);
		InitMMTimer();
		org_data.GetMusicInfo(&mi);
		StartTimer(mi.wait);

		// Set up mid note track stuff
		if (gPlayMidNote) {
			for (int i = 0; i < MAXMELODY; i++) {
				if (s_solo != -1 && s_solo != i)
					continue;

				unsigned char vol = VOLDUMMY;
				unsigned char pan = PANDUMMY;

				// Pizzicato channels still have weird behavior that this doesn't account for,
				// but I'm not gonna fix it because I don't feel like it
				NOTELIST* note = mi.tdata[i].pipi ? org_data.FindLastOrgNoteKey(i, play_p) : org_data.FindOrgNoteLength(i, play_p);
				if (note != NULL && note->x < play_p && org_data.mute[i] == 0) {
					unsigned char y = note->y;

					now_leng[i] = (note->x + note->length) - play_p;
					int played_len = (play_p - note->x) * mi.wait;

					ResumeOrganObject(y, i, mi.tdata[i].freq, mi.tdata[i].pipi, played_len);

					while (note != NULL && note->x < play_p) {
						if (note->volume != VOLDUMMY)
							vol = note->volume;

						if (note->pan != PANDUMMY)
							pan = note->pan;

						note = note->to;
					}

					if (pan != PANDUMMY) ChangeOrganPan(y, pan, i);
					if (vol != VOLDUMMY) ChangeOrganVolume(y, vol * 100 / 0x7F, i);
				}
			}
			for (int i = MAXMELODY; i < MAXTRACK; i++) {
				if (s_solo != -1 && s_solo != i)
					continue;

				unsigned char vol = VOLDUMMY;
				unsigned char pan = PANDUMMY;

				NOTELIST* note = org_data.FindLastOrgNoteKey(i, play_p);
				if (note != NULL && note->x < play_p && org_data.mute[i] == 0) {
					int played_len = (play_p - note->x) * mi.wait;

					ResumeDramObject(note->y, i - MAXMELODY, played_len);

					while (note != NULL && note->x < play_p) {
						if (note->volume != VOLDUMMY)
							vol = note->volume;

						if (note->pan != PANDUMMY)
							pan = note->pan;

						note = note->to;
					}

					if (pan != PANDUMMY) ChangeDramPan(pan, i - MAXMELODY);
					if (vol != VOLDUMMY) ChangeDramVolume(vol * 100 / 0x7F, i - MAXMELODY);
				}
			}
		}

		timer_sw = 1;
		UpdateToolbarStatus();
	}
}
void StopPlayingSong(void) {
	if (timer_sw) {
		QuitMMTimer();
		Rxo_StopAllSoundNow();
		timer_sw = 0;

		MUSICINFO mi;
		org_data.GetMusicInfo(&mi);

		long playPos;
		org_data.GetPlayPos(NULL, &playPos);
		scr_data.SetHorzScroll(sSmoothScroll ? playPos : ((playPos / (mi.dot * mi.line)) * (mi.dot * mi.line)));

		UpdateToolbarStatus();
	}
}