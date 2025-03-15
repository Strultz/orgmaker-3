#include <windows.h>
#include <stdio.h>
#include "Setting.h"
#include "DefOrg.h"
#include "Sound.h"
#include "OrgData.h"

#define MINIAUDIO_IMPLEMENTATION
#define MA_NO_ENCODING
#include "miniaudio.h"

#define SE_MAX	512

// ƒVƒ“ƒ{ƒ‹’è‹`.
#define	SMPFRQ			48000				//!< ƒTƒ“ƒvƒŠƒ“ƒOŽü”g”.
#define	BUFSIZE			((SMPFRQ * 4) / 10)	//!< ƒf[ƒ^ƒoƒbƒtƒ@ƒTƒCƒY (100ms‘Š“–).

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, y, z) MIN(MAX((x), (y)), (z))

static struct S_Sound
{
	signed char samples[4];
    long ring;
    
    signed char* buffer;
	size_t frames;
	long position;
	float sub_position;
	float advance_delta;
    long silence_count;
	bool playing;
	bool played_before;
	bool looping;
    
	float volume;
	float pan_l;
	float pan_r;
	float volume_l;
	float volume_r;
	float target_volume_l;
	float target_volume_r;
	long vol_ticks;
    
    signed long stop_in;

	struct S_Sound* next;
};

static S_Sound* sound_list_head;
static ma_device device;
static ma_mutex mutex;
static ma_mutex organya_mutex;

static unsigned long output_frequency;
static unsigned long vol_ticks;

static unsigned short organya_timer;
static unsigned long organya_countdown;

static bool exporting = false;

static inline long mmodi(long x, long n) { return ((x %= n) < 0) ? x + n : x; }
static inline float mmodf(float x, float n) { return ((x = fmodf(x, n)) < 0) ? x + n : x; }

static float MillibelToScale(long volume) {
	volume = CLAMP(volume, -10000, 0);
	return pow(10.0, volume / 2000.0);
}

static void S_SetSoundFrequency(S_Sound* sound, unsigned long long frequency) {
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	sound->advance_delta = (float)frequency / output_frequency;

	ma_mutex_unlock(&mutex);
}

static void S_SetSoundVolume(S_Sound* sound, long volume) {
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	sound->volume = MillibelToScale(volume);

	sound->target_volume_l = sound->pan_l * sound->volume;
	sound->target_volume_r = sound->pan_r * sound->volume;

	if (!sound->playing || !sound->played_before) {
		sound->volume_l = sound->target_volume_l;
		sound->volume_r = sound->target_volume_r;
		sound->vol_ticks = 0;
	}
	else {
		sound->vol_ticks = vol_ticks;
	}

	ma_mutex_unlock(&mutex);
}

static void S_SetSoundPan(S_Sound* sound, long pan) {
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	sound->pan_l = MillibelToScale(-pan);
	sound->pan_r = MillibelToScale(pan);

	sound->target_volume_l = sound->pan_l * sound->volume;
	sound->target_volume_r = sound->pan_r * sound->volume;

	if (!sound->playing || !sound->played_before) {
		sound->volume_l = sound->target_volume_l;
		sound->volume_r = sound->target_volume_r;
		sound->vol_ticks = 0;
	}
	else {
		sound->vol_ticks = vol_ticks;
	}

	ma_mutex_unlock(&mutex);
}

static S_Sound* S_CreateSound(unsigned int frequency, const unsigned char* samples, size_t length) {
	S_Sound* sound = (S_Sound*)malloc(sizeof(S_Sound));
	if (sound == NULL)
		return NULL;

	sound->buffer = (signed char*)malloc(length);
	if (sound->buffer == NULL) {
		free(sound);
		return NULL;
	}

	for (size_t i = 0; i < length; ++i)
		sound->buffer[i] = samples[i] - 0x80;
    
    memset(sound->samples, 0, 4);

	sound->frames = length;
	sound->playing = false;
	sound->played_before = false;
	sound->position = 0;
    sound->ring = 0;
    sound->sub_position = 0;
    sound->silence_count = 0;

	S_SetSoundFrequency(sound, frequency);
	S_SetSoundVolume(sound, 0);
	S_SetSoundPan(sound, 0);

	ma_mutex_lock(&mutex);

	sound->next = sound_list_head;
	sound_list_head = sound;

	ma_mutex_unlock(&mutex);

	return sound;
}

void S_DestroySound(S_Sound* sound)
{
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	for (S_Sound** sound_pointer = &sound_list_head; *sound_pointer != NULL; sound_pointer = &(*sound_pointer)->next)
	{
		if (*sound_pointer == sound)
		{
			*sound_pointer = sound->next;
			break;
		}
	}

	free(sound->buffer);
	free(sound);

	ma_mutex_unlock(&mutex);
}

static void S_PlaySound(S_Sound* sound, bool looping) {
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);
    
    if (!sound->playing) {
		sound->position = 0;

		if (sound->silence_count == 0) {
			sound->sub_position = 0;
		}
        
        sound->playing = true;
	}

	sound->looping = looping;
	sound->stop_in = 0;

	ma_mutex_unlock(&mutex);
}

static void S_PlaySoundFor(S_Sound* sound, int ms) {
	if (sound == NULL || ms == 0)
		return;

	ma_mutex_lock(&mutex);

	if (!sound->playing) {
		sound->position = 0;

		if (sound->silence_count == 0) {
			sound->sub_position = 0;
		}

		sound->playing = true;
	}

	sound->looping = true;
	sound->stop_in = (ms * output_frequency) / 1000;

	ma_mutex_unlock(&mutex);
}

static void S_StopSound(S_Sound* sound) {
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

    if (sound->playing) {
        sound->playing = false;
        sound->silence_count = 4;
    }

	ma_mutex_unlock(&mutex);
}

static void S_RewindSound(S_Sound* sound) {
	if (sound == NULL)
		return;

	ma_mutex_lock(&mutex);

	sound->position = 0;

	ma_mutex_unlock(&mutex);
}

// This is for exporting
static void S_ResetSounds() {
	ma_mutex_lock(&mutex);

	for (S_Sound* sound = sound_list_head; sound != NULL; sound = sound->next) {
		sound->playing = false;
		sound->looping = false;
		sound->played_before = false;
		sound->silence_count = 0;
		sound->ring = 0;
		sound->position = 0;
		sound->sub_position = 0;
		sound->volume = 1.0F;
		sound->pan_l = 1.0F;
		sound->pan_r = 1.0F;
		sound->target_volume_l = sound->pan_l * sound->volume;
		sound->target_volume_r = sound->pan_r * sound->volume;
		sound->volume_l = sound->target_volume_l;
		sound->volume_r = sound->target_volume_r;
		sound->vol_ticks = 0;
	}

	ma_mutex_unlock(&mutex);
}

static void S_MixSounds(float* stream, size_t frames_total) {
	for (S_Sound* sound = sound_list_head; sound != NULL; sound = sound->next) {
		if (sound->playing || sound->silence_count > 0) {
			float* stream_pointer = stream;

			sound->played_before = true;
            
			for (size_t frames_done = 0; frames_done < frames_total; ++frames_done) {
				// Update volume ramp
				if (sound->vol_ticks > 0) {
					sound->volume_l += (sound->target_volume_l - sound->volume_l) / (float)sound->vol_ticks;
					sound->volume_r += (sound->target_volume_r - sound->volume_r) / (float)sound->vol_ticks;

					--sound->vol_ticks;
				}

				// Perform lagrange interpolation
				const float subsample = sound->sub_position;
				const long sp = sound->position;

				const long margin = sound->ring - 2;

				const float sample_a = (float)sound->samples[mmodi(margin - 1, 4)] / (float)(1 << 7);
				const float sample_b = (float)sound->samples[mmodi(margin, 4)] / (float)(1 << 7);
				const float sample_c = (float)sound->samples[mmodi(margin + 1, 4)] / (float)(1 << 7);
				const float sample_d = (float)sound->samples[mmodi(margin + 2, 4)] / (float)(1 << 7);

				const float c0 = sample_b;
				const float c1 = sample_c - 1 / 3.0 * sample_a - 1 / 2.0 * sample_b - 1 / 6.0 * sample_d;
				const float c2 = 1 / 2.0 * (sample_a + sample_c) - sample_b;
				const float c3 = 1 / 6.0 * (sample_d - sample_a) + 1 / 2.0 * (sample_b - sample_c);

				const float interpolated_sample = ((c3 * subsample + c2) * subsample + c1) * subsample + c0;

				// Mix, and apply volume
				*stream_pointer++ += interpolated_sample * sound->volume_l;
				*stream_pointer++ += interpolated_sample * sound->volume_r;

				// Increment sample
				const long last_position = sound->position;

				sound->sub_position += sound->advance_delta;
                sound->position += (long)sound->sub_position;
                sound->sub_position = mmodf(sound->sub_position, 1.0F);

				if (sound->position > last_position) {
					/* Update ring buffer position and write new sample(s) */
					for (int i = 0; i < (sound->position - last_position); ++i) {
						sound->ring = (sound->ring + 1) % 4;

						if (sound->playing) {
							if (sound->looping) {
								sound->samples[sound->ring] = (signed long)sound->buffer[(last_position + i) % sound->frames];
							}
							else {
								sound->samples[sound->ring] = (signed long)((last_position + i) >= sound->frames ? 0 : sound->buffer[(last_position + i)]);
							}
						}
						else {
							sound->samples[sound->ring] = 0;
							--sound->silence_count;
						}
					}
				}

				// Stop or loop sample once it's reached its end
				if (sound->playing) {
                    if (sound->stop_in > 0) {
						--sound->stop_in;
						if (sound->stop_in == 0) {
							sound->looping = FALSE;
						}
					}
					if (sound->position >= sound->frames)
					{
						if (sound->looping)
						{
							sound->position = sound->position % sound->frames;
						}
						else
						{
							sound->playing = FALSE;
							sound->silence_count = 4;
						}
					}
				}
				else {
					sound->position = 0;
				}
			}
		}
	}
}

void S_SetOrganyaTimer(unsigned short timer)
{
	ma_mutex_lock(&organya_mutex);

	organya_timer = timer;
	organya_countdown = 0;

	ma_mutex_unlock(&organya_mutex);
}


// DirectSound\‘¢‘Ì 
//LPDIRECTSOUND       lpDS = NULL;            // DirectSoundƒIƒuƒWƒFƒNƒg
//LPDIRECTSOUNDBUFFER lpPRIMARYBUFFER = NULL; // ˆêŽžƒoƒbƒtƒ@
S_Sound *lpSECONDARYBUFFER[SE_MAX] = {NULL};
S_Sound *lpORGANBUFFER[8][8][2] = {NULL};
S_Sound *lpDRAMBUFFER[8] = {NULL};

//˜^‰¹—p
//HANDLE						CapEvent[2];			//!< “ü—ÍƒCƒxƒ“ƒgEƒIƒuƒWƒFƒNƒg.
//DWORD						CapBufSize;				//!< ƒLƒƒƒvƒ`ƒƒƒoƒbƒtƒ@EƒTƒCƒY.
//DWORD						GetPos;					//!< ƒLƒƒƒvƒ`ƒƒƒoƒbƒtƒ@‚Ì“Ç‚Ýž‚ÝŠJŽnˆÊ’u.
//DWORD						PutPos;					//!< ƒLƒƒƒvƒ`ƒƒƒoƒbƒtƒ@‚Ì‘‚«ž‚ÝŠJŽnˆÊ’u.
//BYTE*						DataBuff;				//!< ƒf[ƒ^ƒoƒbƒtƒ@.
//LPDIRECTSOUNDCAPTURE 		CapDev;					//!< IDirectSoundCaptureƒCƒ“ƒ^[ƒtƒFƒCƒX ƒ|ƒCƒ“ƒ^.
//LPDIRECTSOUNDCAPTUREBUFFER	CapBuf;					//!< IDirectSoundBufferƒCƒ“ƒ^[ƒtƒFƒCƒX ƒ|ƒCƒ“ƒ^.

//DWORD						OutBufSize;				//!< ƒXƒgƒŠ[ƒ€ƒoƒbƒtƒ@EƒTƒCƒY.

extern int s_solo;

static void S_Callback(ma_device* device, void* output_stream, const void* input_stream, ma_uint32 frames_total)
{
	(void)device;
	(void)input_stream;

	if (exporting)
		return;

	float* stream = (float *)output_stream;
	size_t frames_done = 0;
	while (frames_done != frames_total) {
		float mix_buffer[0x400 * 2];
		size_t subframes = MIN(0x400, frames_total - frames_done);
		memset(mix_buffer, 0, subframes * sizeof(float) * 2);

		ma_mutex_lock(&organya_mutex);

		if (organya_timer == 0) {
			ma_mutex_lock(&mutex);
			S_MixSounds(mix_buffer, subframes);
			ma_mutex_unlock(&mutex);
		}
		else {
			unsigned int subframes_done = 0;
			while (subframes_done != subframes) {
				if (organya_countdown == 0) {
					organya_countdown = (organya_timer * output_frequency) / 1000;
					org_data.PlayData();
				}
				const unsigned int frames_to_do = MIN(organya_countdown, subframes - subframes_done);
				ma_mutex_lock(&mutex);
				S_MixSounds(mix_buffer + subframes_done * 2, frames_to_do);
				ma_mutex_unlock(&mutex);
				subframes_done += frames_to_do;
				organya_countdown -= frames_to_do;
			}
		}

		ma_mutex_unlock(&organya_mutex);

		for (size_t i = 0; i < subframes * 2; ++i) {
			*stream++ = mix_buffer[i];
		}
		frames_done += subframes;
	}
}

// DirectSound‚ÌŠJŽn 
BOOL InitDirectSound(HWND hwnd)
{
//    int i;
    /*DSBUFFERDESC dsbd;

    // DirectDraw‚Ì‰Šú‰»
    if(DirectSoundCreate(NULL, &lpDS, NULL) != DS_OK){
		lpDS = NULL;
		return(FALSE);
	}
    lpDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);

    // ˆêŽŸƒoƒbƒtƒ@‚Ì‰Šú‰»
    ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER; // | DSBCAPS_CTRLPOSITIONNOTIFY;
    lpDS->CreateSoundBuffer(&dsbd, &lpPRIMARYBUFFER, NULL);*/

//    for(i = 0; i < SE_MAX; i++) lpSECONDARYBUFFER[i] = NULL;
	
	//ƒLƒƒƒvƒ`ƒƒƒoƒbƒtƒ@‚Ìì¬ ‘æˆêˆø”NULL‚ÅƒfƒtƒHƒ‹ƒgB‚±‚ê‚Í‚Ç‚¤‚©B
//	if( DirectSoundCaptureCreate( NULL, &CapDev, NULL ) != S_OK ){
//		return FALSE;
//	}
//	dsbd.dwFlags = 0; //ƒZƒJƒ“ƒ_ƒŠƒoƒbƒtƒ@
//	CapDev->CreateCaptureBuffer(&dsbd, &CapBuf, NULL);

    sound_list_head = NULL;

	ma_device_config config = ma_device_config_init(ma_device_type_playback);
	config.playback.pDeviceID = NULL;
	config.playback.format = ma_format_f32;
	config.playback.channels = 2;
	config.sampleRate = 0;
	config.dataCallback = S_Callback;
	config.pUserData = NULL;
	config.periodSizeInMilliseconds = 10;
	//config.performanceProfile = ma_performance_profile_conservative;

	if (ma_device_init(NULL, &config, &device) == MA_SUCCESS)
	{
		output_frequency = device.sampleRate;

		// Should be 4 MS
		vol_ticks = (long)((float)output_frequency * 0.004F);

		if (ma_mutex_init(&mutex) == MA_SUCCESS)
		{
			if (ma_mutex_init(&organya_mutex) == MA_SUCCESS)
			{
				if (ma_device_start(&device) == MA_SUCCESS) {
					memset(lpSECONDARYBUFFER, 0, sizeof(lpSECONDARYBUFFER));
					memset(lpORGANBUFFER, 0, sizeof(lpORGANBUFFER));
					memset(lpDRAMBUFFER, 0, sizeof(lpDRAMBUFFER));

					return TRUE;
				}

				ma_mutex_uninit(&organya_mutex);
			}

			ma_mutex_uninit(&mutex);
		}

		ma_device_uninit(&device);
	}

	return FALSE;
}

// DirectSound‚ÌI—¹ 
void EndDirectSound(void)
{
    int i;

    for(i = 0; i < 8; i++){
        if(lpSECONDARYBUFFER[i] != NULL){
			S_DestroySound(lpSECONDARYBUFFER[i]);
			lpSECONDARYBUFFER[i] = NULL;
		}
    }
    /*if(lpPRIMARYBUFFER != NULL){
		lpPRIMARYBUFFER->Release();
		lpPRIMARYBUFFER = NULL;
	}
    if(lpDS != NULL){
		lpDS->Release();
		lpDS = NULL;
	}*/
//	if( CapBuf ){
//		CapBuf->Stop();
//	}
//    if(CapDev != NULL){
//		CapDev->Release();
//		CapDev = NULL;
//	}

	ma_device_stop(&device);
	ma_mutex_uninit(&organya_mutex);
	ma_mutex_uninit(&mutex);
	ma_device_uninit(&device);
}
void ReleaseSoundObject(int no){
	if(lpSECONDARYBUFFER[no] != NULL){
		S_DestroySound(lpSECONDARYBUFFER[no]);
		lpSECONDARYBUFFER[no] = NULL;
	}
}


// ƒTƒEƒ“ƒh‚ÌÝ’è 
BOOL InitSoundObject(LPCSTR resname, int no)
{
    /*HRSRC hrscr;
    DSBUFFERDESC dsbd;
    DWORD *lpdword;//ƒŠƒ\[ƒX‚ÌƒAƒhƒŒƒX
    // ƒŠƒ\[ƒX‚ÌŒŸõ
    if((hrscr = FindResource(NULL, resname, "WAVE")) == NULL)
                                                    return(FALSE);
    // ƒŠƒ\[ƒX‚ÌƒAƒhƒŒƒX‚ðŽæ“¾
    lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));
	// “ñŽŸƒoƒbƒtƒ@‚Ì¶¬
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = 
		DSBCAPS_STATIC|
		DSBCAPS_GLOBALFOCUS
		|DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)lpdword+0x36);//WAVEƒf[ƒ^‚ÌƒTƒCƒY
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(lpdword+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no],
								NULL) != DS_OK) return(FALSE);
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
    // “ñŽŸƒoƒbƒtƒ@‚ÌƒƒbƒN
    lpSECONDARYBUFFER[no]->Lock(0, *(DWORD*)((BYTE*)lpdword+0x36),
                        &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	// ‰¹Œ¹ƒf[ƒ^‚ÌÝ’è
	CopyMemory(lpbuf1, (BYTE*)lpdword+0x3a, dwbuf1);
    if(dwbuf2 != 0) CopyMemory(lpbuf2, (BYTE*)lpdword+0x3a+dwbuf1, dwbuf2);
	// “ñŽŸƒoƒbƒtƒ@‚ÌƒƒbƒN‰ðœ
	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 

    return(TRUE);*/

	ReleaseSoundObject(no);

	HRSRC hrscr;
	unsigned char* lpdword;
	if ((hrscr = FindResource(NULL, resname, "WAVE")) == NULL)
		return FALSE;
	lpdword = (unsigned char*)LockResource(LoadResource(NULL, hrscr));

	lpSECONDARYBUFFER[no] = S_CreateSound(22050, lpdword + 0x2C, *(unsigned int*)(lpdword+0x28));

	if (lpSECONDARYBUFFER[no] == NULL)
		return FALSE;

	S_RewindSound(lpSECONDARYBUFFER[no]);

	return TRUE;
}

//extern LPDIRECTDRAW            lpDD;	// DirectDrawƒIƒuƒWƒFƒNƒg
BOOL LoadSoundObject(char *file_name, int no)
{
	/*DWORD i;
	DWORD file_size = 0;
	char check_box[58];
	FILE *fp;
	if((fp=fopen(file_name,"rb"))==NULL){
//		char msg_str[64];				//”’lŠm”F—p
//		lpDD->FlipToGDISurface(); //ƒƒbƒZ[ƒW•\Ž¦‚Ì•û‚ÉƒtƒŠƒbƒv
//		sprintf(msg_str,"%s‚ªŒ©“–‚½‚è‚Ü‚¹‚ñ",file_name);
//		MessageBox(hWND,msg_str,"title",MB_OK);
//		SetCursor(FALSE); // ƒJ[ƒ\ƒ‹Á‹Ž
		return(FALSE);
	}
	for(i = 0; i < 58; i++){
		fread(&check_box[i],sizeof(char),1,fp);
	}
	if(check_box[0] != 'R')return(FALSE);
	if(check_box[1] != 'I')return(FALSE);
	if(check_box[2] != 'F')return(FALSE);
	if(check_box[3] != 'F')return(FALSE);
	file_size = *((DWORD *)&check_box[4]);

	DWORD *wp;
	wp = (DWORD*)malloc(file_size);//ƒtƒ@ƒCƒ‹‚Ìƒ[ƒNƒXƒy[ƒX‚ðì‚é
	fseek(fp,0,SEEK_SET);
	for(i = 0; i < file_size; i++){
		fread((BYTE*)wp+i,sizeof(BYTE),1,fp);
	}
	fclose(fp);
	//ƒZƒJƒ“ƒ_ƒŠƒoƒbƒtƒ@‚Ì¶¬
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_STATIC|DSBCAPS_GLOBALFOCUS|DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)wp+0x36);//WAVEƒf[ƒ^‚ÌƒTƒCƒY
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(wp+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpSECONDARYBUFFER[no],
								NULL) != DS_OK){
		free(wp);
		return(FALSE);	
	}
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
	HRESULT hr;
	hr = lpSECONDARYBUFFER[no]->Lock(0, file_size-58,
							&lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	if(hr != DS_OK){
		free(wp);
		return (FALSE);
	}
	CopyMemory(lpbuf1, (BYTE*)wp+0x3a, dwbuf1);//+3a‚Íƒf[ƒ^‚Ì“ª
	if(dwbuf2 != 0)	CopyMemory(lpbuf2, (BYTE*)wp+0x3a+dwbuf1, dwbuf2);
	lpSECONDARYBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	
	free(wp);
	return(TRUE);*/
	return FALSE;
}

// ƒTƒEƒ“ƒh‚ÌÄ¶ 
void PlaySoundObject(int no, int mode)
{
    if(lpSECONDARYBUFFER[no] != NULL){
		switch(mode){
		case 0: // ’âŽ~
			S_StopSound(lpSECONDARYBUFFER[no]);
			S_RewindSound(lpSECONDARYBUFFER[no]);
			break;
		case 1: // Ä¶
			S_StopSound(lpSECONDARYBUFFER[no]);
			S_RewindSound(lpSECONDARYBUFFER[no]);
            S_PlaySound(lpSECONDARYBUFFER[no], false);
            break;
		case -1: // ƒ‹[ƒvÄ¶
			S_PlaySound(lpSECONDARYBUFFER[no], true);
			break;
		}
    }
}

void ChangeSoundFrequency(int no, DWORD rate)//100‚ªMIN9999‚ªMAX‚Å2195?‚ªÉ°ÏÙ
{
	if(lpSECONDARYBUFFER[no] != NULL)
		S_SetSoundFrequency(lpSECONDARYBUFFER[no], rate);
}
void ChangeSoundVolume(int no, long volume)//300‚ªMAX‚Å300‚ªÉ°ÏÙ
{
	if(lpSECONDARYBUFFER[no] != NULL)
		S_SetSoundVolume(lpSECONDARYBUFFER[no], (volume-300)*8);
}
void ChangeSoundPan(int no, long pan)//512‚ªMAX‚Å256‚ªÉ°ÏÙ
{
	if(lpSECONDARYBUFFER[no] != NULL)
		S_SetSoundPan(lpSECONDARYBUFFER[no], (pan-256)*10);
}

/////////////////////////////////////////////
//¡ƒIƒ‹ƒK[ƒjƒƒ¡¡¡¡¡¡¡¡¡¡¡¡///////
/////////////////////


OCTWAVE oct_wave[8] = {
	{256,  1, 4},//0 Oct
	{256,  2, 8},//1 Oct
	{128,  4, 12},//2 Oct
	{128,  8, 16},//3 Oct
	{ 64, 16, 20},//4 Oct
	{ 32, 32, 24},
	{ 16, 64, 28},
	{  8,128, 32},
};

//WAVEFORMATEX format_tbl2 = { WAVE_FORMAT_PCM, 1, 22050, 22050, 1, 8, 0 };	// 22050Hz Format

//BYTE format_tbl3[] = {0x01,0x00,0x01,0x00,0x44,0xac,0x00,//441000Hz‚ÌFormat
//0x00,0x44,0xac,0x00,0x00,0x08,0x00,0x00,0x00,0x66,0x61};
BOOL MakeSoundObject8(char *wavep,char track, char pipi )
{
	DWORD i,j,k;
	unsigned long wav_tp;//WAVƒe[ƒuƒ‹‚ð‚³‚·ƒ|ƒCƒ“ƒ^
	DWORD wave_size;//256;
	DWORD data_size;
	BYTE *wp;
	BYTE *wp_sub;
	int work;
	//ƒZƒJƒ“ƒ_ƒŠƒoƒbƒtƒ@‚Ì¶¬
	//DSBUFFERDESC dsbd;

	for(j = 0; j < 8; j++){
		for(k = 0; k < 2; k++){
			wave_size = oct_wave[j].wave_size;
			if( pipi )data_size = wave_size * oct_wave[j].oct_size;
			else data_size = wave_size;
			/*ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
			dsbd.dwSize = sizeof(DSBUFFERDESC);
			dsbd.dwFlags = DSBCAPS_STATIC|
					DSBCAPS_GLOBALFOCUS|
					DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;//DSBCAPS_STATIC|DSBCAPS_CTRLDEFAULT;
			dsbd.dwBufferBytes = data_size;//file_size;
			dsbd.lpwfxFormat = (LPWAVEFORMATEX)(&format_tbl2);
				if(lpDS->CreateSoundBuffer(&dsbd, &lpORGANBUFFER[track][j][k],//j = se_no
										NULL) != DS_OK) return(FALSE);*/
			wp = (BYTE*)malloc(data_size);//ƒtƒ@ƒCƒ‹‚Ìƒ[ƒNƒXƒy[ƒX‚ðì‚é
			wp_sub = wp;
			wav_tp = 0;
			for(i = 0; i < data_size; i++){
				work = *(wavep+wav_tp);
				work+=0x80;
				*wp_sub=(BYTE)work;
				wav_tp += 256/wave_size;
				if( wav_tp > 255 ) wav_tp -= 256;
				wp_sub++;
			}
			lpORGANBUFFER[track][j][k] = S_CreateSound(22050, wp, data_size);

			free(wp);

			if (lpORGANBUFFER[track][j][k] == NULL)
				return FALSE;

			S_RewindSound(lpORGANBUFFER[track][j][k]);
		}
	}
	return(TRUE);
}
//2.1.0‚Å ®”Œ^‚©‚ç¬”“_Œ^‚É•ÏX‚µ‚Ä‚Ý‚½B20140401
short freq_tbl[12] = { 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494 };
//double freq_tbl[12] = {261.62556530060, 277.18263097687, 293.66476791741, 311.12698372208, 329.62755691287, 349.22823143300, 369.99442271163, 391.99543598175, 415.30469757995, 440.00000000000, 466.16376151809, 493.88330125612};
void ChangeOrganFrequency(unsigned char key,char track, DWORD a)
{
	//double tmpDouble;
	for(int j = 0; j < 8; j++)
		for(int i = 0; i < 2; i++){
			//dmmult = (0.98f + ((double)a / 50000.0f));
			//tmpDouble = (((double)oct_wave[j].wave_size * freq_tbl[key]) * (double)oct_wave[j].oct_par) / (8.00f * (2.0f - dmmult));
			
			if (lpORGANBUFFER[track][j][i] != NULL)
				S_SetSoundFrequency(lpORGANBUFFER[track][j][i], //1000‚ð+ƒ¿‚ÌƒfƒtƒHƒ‹ƒg’l‚Æ‚·‚é
					(DWORD)((oct_wave[j].wave_size * freq_tbl[key]) * oct_wave[j].oct_par) / 8 + (a - 1000)
//					((oct_wave[j].wave_size*freq_tbl[key])*oct_wave[j].oct_par)/8 + (a-1000)
				);
		}
}
short pan_tbl[13] = {0,43,86,129,172,215,256,297,340,383,426,469,512}; 
unsigned char old_key[MAXTRACK] = {255,255,255,255,255,255,255,255};//Ä¶’†‚Ì‰¹
unsigned char key_on[MAXTRACK] = {0,0,0,0,0,0,0,0};//ƒL[ƒXƒCƒbƒ`
unsigned char key_twin[MAXTRACK] = {0,0,0,0,0,0,0,0};//¡Žg‚Á‚Ä‚¢‚éƒL[(˜A‘±Žž‚ÌƒmƒCƒY–hŽ~‚Ìˆ×‚É“ñ‚Â—pˆÓ)
void ChangeOrganPan(unsigned char key, unsigned char pan,char track)//512‚ªMAX‚Å256‚ªÉ°ÏÙ
{
	if(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]] != NULL && old_key[track] != 255)
		S_SetSoundPan(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], (pan_tbl[pan]-256)*10);
}
void ChangeOrganVolume(int no, long volume,char track)//300‚ªMAX‚Å300‚ªÉ°ÏÙ
{
	if(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]] != NULL && old_key[track] != 255)
		S_SetSoundVolume(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], (volume-255)*8);
}
// ƒTƒEƒ“ƒh‚ÌÄ¶ 
void PlayOrganObject(unsigned char key, int mode,char track,DWORD freq, bool pipi)
{
	
    if(lpORGANBUFFER[track][key/12][key_twin[track]] != NULL){
		switch(mode){
		case 0: // ’âŽ~
			if (old_key[track] != 255) {
				S_StopSound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]);
				S_RewindSound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]]);
			}
			break;
		case 1: // Ä¶
//			if(key_on == 1 && no == old_key/12)//
//				lpORGANBUFFER[old_key/12]->Stop();
//				ChangeOrganFrequency(key%12);//Žü”g”‚ðÝ’è‚µ‚Ä
//				lpORGANBUFFER[no]->Play(0, 0, 0);
//			if(key_on == 1 && no == old_key/12){//–Â‚Á‚Ä‚éWAV‚ª“¯‚¶WAVNO‚È‚ç
//				old_key = key;
//				ChangeOrganFrequency(key%12);//Žü”g”‚ð•Ï‚¦‚é‚¾‚¯
//			}
			break;
		case 2: // •à‚©‚¹’âŽ~
			if(old_key[track] != 255){
				if (!pipi)
					S_PlaySound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], false);
				old_key[track] = 255;
			}
            break;
		case -1:
			if(old_key[track] == 255){//V‹K–Â‚ç‚·
				ChangeOrganFrequency(key%12,track,freq);//Žü”g”‚ðÝ’è‚µ‚Ä
				S_PlaySound(lpORGANBUFFER[track][key / 12][key_twin[track]], !pipi);
				old_key[track] = key;
				key_on[track] = 1;
			}else if(key_on[track] == 1 && old_key[track] == key){//“¯‚¶‰¹
				//¡‚È‚Á‚Ä‚¢‚é‚Ì‚ð•à‚©‚¹’âŽ~
				if (!pipi)
					S_PlaySound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], false);
				key_twin[track] ^= 1;
                
				S_PlaySound(lpORGANBUFFER[track][key / 12][key_twin[track]], !pipi);
			}else{//ˆá‚¤‰¹‚ð–Â‚ç‚·‚È‚ç
				if (!pipi)
					S_PlaySound(lpORGANBUFFER[track][old_key[track] / 12][key_twin[track]], false);
				key_twin[track] ^= 1;
                
				ChangeOrganFrequency(key%12,track,freq);//Žü”g”‚ðÝ’è‚µ‚Ä
				S_PlaySound(lpORGANBUFFER[track][key / 12][key_twin[track]], !pipi);
				old_key[track] = key;
			}
			break;
		}
    }
}
//ƒIƒ‹ƒK[ƒjƒƒƒIƒuƒWƒFƒNƒg‚ðŠJ•ú
void ReleaseOrganyaObject(char track){
	for(int i = 0; i < 8; i++){
		if(lpORGANBUFFER[track][i][0] != NULL){
			S_DestroySound(lpORGANBUFFER[track][i][0]);
			lpORGANBUFFER[track][i][0] = NULL;
		}
		if(lpORGANBUFFER[track][i][1] != NULL){
			S_DestroySound(lpORGANBUFFER[track][i][1]);
			lpORGANBUFFER[track][i][1] = NULL;
		}
	}
}
//”gŒ`ƒf[ƒ^‚ðƒ[ƒh
//char wave_data[100*256];

char *wave_data = NULL;

struct {
	int length;
	unsigned char* data;
} drumsData[NUMDRAMITEM];

BOOL InitWaveData100(void)
{
	if (wave_data == NULL) wave_data = (char *)malloc(sizeof(char) * 256 * 100);
	if (wave_data == NULL) return FALSE;
    HRSRC hrscr;
	unsigned char *bybuffer;//ƒŠƒ\[ƒX‚ÌƒAƒhƒŒƒX
    // ƒŠƒ\[ƒX‚ÌŒŸõ
    if((hrscr = FindResource(NULL, "WAVE100", "WAVE")) == NULL)
                                                    return(FALSE);
    // ƒŠƒ\[ƒX‚ÌƒAƒhƒŒƒX‚ðŽæ“¾
	bybuffer = (unsigned char *)LockResource(LoadResource(NULL, hrscr));

	memcpy(wave_data, bybuffer,100*256);
	bybuffer += (100 * 256);

	for (int i = 0; i < NUMDRAMITEM; ++i) {
        if (drumsData[i].data != NULL) {
			free(drumsData[i].data);
			drumsData[i].data = NULL;
		}
        
		int length = (bybuffer[3] << 24) | (bybuffer[2] << 16) | (bybuffer[1] << 8) | bybuffer[0];
		bybuffer += 4;

		unsigned char* drumsample = (unsigned char*)malloc(length);

		if (drumsample == NULL) {
			return FALSE;
		}

		memcpy(drumsample, bybuffer, length);
		bybuffer += length;

		drumsData[i].length = length;

		drumsData[i].data = drumsample;
	}

	return TRUE;
}
BOOL LoadWaveData100(const char *file)
{
	unsigned char bytes[4];

	if (strlen(file) <= 0) {
		return InitWaveData100(); // Init from resource
	}
	if (wave_data == NULL) wave_data = (char *)malloc(sizeof(char) * 256 * 100);
	if (wave_data == NULL) return FALSE;
	FILE *fp = fopen(file, "rb");
	if (fp == NULL) {
		return InitWaveData100(); // Init from resource
	}
//	wave_data = new char[100*256];
	if (fread(wave_data, sizeof(char), 256 * 100, fp) == 0) {
		fclose(fp);
		return FALSE;
	}

	for (int i = 0; i < NUMDRAMITEM; ++i) {
        if (drumsData[i].data != NULL) {
			free(drumsData[i].data);
			drumsData[i].data = NULL;
		}
        
		if (fread(bytes, 4, 1, fp) == 0) {
			memset(drumsData, 0, sizeof(drumsData));
			fclose(fp);
			return FALSE;
		}

		int length = (bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0];

		unsigned char* drumsample = (unsigned char*)malloc(length);

		if (drumsample == NULL) {
			fclose(fp);
			return FALSE;
		}

		if (fread(drumsample, sizeof(unsigned char), length, fp) == 0) {
            free(drumsample);
            fclose(fp);
            return FALSE;
        }

		drumsData[i].length = length;

		drumsData[i].data = drumsample;
	}

	fclose(fp);
	return TRUE;
}
BOOL DeleteWaveData100(void)
{
//	delete wave_data;
	free(wave_data);
	wave_data = NULL;
	for (int i = 0; i < NUMDRAMITEM; ++i) {
		free(drumsData[i].data);
		drumsData[i].data = NULL;
	}
	return TRUE;
}
//”gŒ`‚ð‚P‚O‚OŒÂ‚Ì’†‚©‚ç‘I‘ð‚µ‚Äì¬
BOOL MakeOrganyaWave(char track,char wave_no, char pipi)
{
	if(wave_no > 99)return FALSE;
	ReleaseOrganyaObject(track);
	MakeSoundObject8(&wave_data[0] + wave_no*256,track, pipi);	
	return TRUE;
}
/////////////////////////////////////////////
//¡ƒIƒ‹ƒK[ƒjƒƒƒhƒ‰ƒ€ƒX¡¡¡¡¡¡¡¡///////
/////////////////////
//ƒIƒ‹ƒK[ƒjƒƒƒIƒuƒWƒFƒNƒg‚ðŠJ•ú
void ReleaseDramObject(char track){
	for(int i = 0; i < 8; i++){
		if(lpDRAMBUFFER[track] != NULL){
			S_DestroySound(lpDRAMBUFFER[track]);
			lpDRAMBUFFER[track] = NULL;
		}
	}
}
// ƒTƒEƒ“ƒh‚ÌÝ’è 
BOOL InitDramObject(char drum, int no)
{
    //HRSRC hrscr;
    //DSBUFFERDESC dsbd;
    //DWORD *lpdword;//ƒŠƒ\[ƒX‚ÌƒAƒhƒŒƒX
    // ƒŠƒ\[ƒX‚ÌŒŸõ
	ReleaseDramObject(no); //‚±‚±‚É‚¨‚¢‚Ä‚Ý‚½B

	if (drum < 0 || drum >= NUMDRAMITEM || drumsData[drum].data == NULL) {
		return FALSE;
	}

	lpDRAMBUFFER[no] = S_CreateSound(22050, drumsData[drum].data, drumsData[drum].length);

	if (lpDRAMBUFFER[no] == NULL)
		return FALSE;

	S_RewindSound(lpDRAMBUFFER[no]);

    /*if((hrscr = FindResource(NULL, resname, "WAVE")) == NULL)
                                                    return(FALSE);
    // ƒŠƒ\[ƒX‚ÌƒAƒhƒŒƒX‚ðŽæ“¾
    lpdword = (DWORD*)LockResource(LoadResource(NULL, hrscr));*/
	// “ñŽŸƒoƒbƒtƒ@‚Ì¶¬
	/*ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = 
		DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	dsbd.dwBufferBytes = drumsData[drum].length; // WAVEƒf[ƒ^‚ÌƒTƒCƒY
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(&format_tbl2);*/

	/*if (lpDS->CreateSoundBuffer(&dsbd, &lpDRAMBUFFER[no],NULL) != DS_OK) return FALSE;*/
    /*LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
    // “ñŽŸƒoƒbƒtƒ@‚ÌƒƒbƒN
    lpDRAMBUFFER[no]->Lock(0, drumsData[drum].length, &lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	// ‰¹Œ¹ƒf[ƒ^‚ÌÝ’è
	CopyMemory(lpbuf1, drumsData[drum].data, dwbuf1);
    if(dwbuf2 != 0){
		CopyMemory(lpbuf2, drumsData[drum].data + dwbuf1, dwbuf2);
	}

	// “ñŽŸƒoƒbƒtƒ@‚ÌƒƒbƒN‰ðœ
	lpDRAMBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	lpDRAMBUFFER[no]->SetCurrentPosition(0);*/

    return TRUE;
}

/*
BOOL LoadDramObject(char *file_name, int no)
{
	DWORD i;
	DWORD file_size = 0;
	char check_box[58];
	FILE *fp;
	ReleaseDramObject(no);
	if((fp=fopen(file_name,"rb"))==NULL){
//		char msg_str[64];				//”’lŠm”F—p
//		lpDD->FlipToGDISurface(); //ƒƒbƒZ[ƒW•\Ž¦‚Ì•û‚ÉƒtƒŠƒbƒv
//		sprintf(msg_str,"%s‚ªŒ©“–‚½‚è‚Ü‚¹‚ñ",file_name);
//		MessageBox(hWND,msg_str,"title",MB_OK);
//		SetCursor(FALSE); // ƒJ[ƒ\ƒ‹Á‹Ž
		return(FALSE);
	}
	for(i = 0; i < 58; i++){
		fread(&check_box[i],sizeof(char),1,fp);
	}
	if(check_box[0] != 'R')return(FALSE);
	if(check_box[1] != 'I')return(FALSE);
	if(check_box[2] != 'F')return(FALSE);
	if(check_box[3] != 'F')return(FALSE);
	file_size = *((DWORD *)&check_box[4]);

	DWORD *wp;
	wp = (DWORD*)malloc(file_size);//ƒtƒ@ƒCƒ‹‚Ìƒ[ƒNƒXƒy[ƒX‚ðì‚é
	fseek(fp,0,SEEK_SET);
	for(i = 0; i < file_size; i++){
		fread((BYTE*)wp+i,sizeof(BYTE),1,fp);
	}
	fclose(fp);
	//ƒZƒJƒ“ƒ_ƒŠƒoƒbƒtƒ@‚Ì¶¬
	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC);
	dsbd.dwFlags = DSBCAPS_STATIC|
		DSBCAPS_GLOBALFOCUS
		|DSBCAPS_CTRLDEFAULT;
	dsbd.dwBufferBytes = *(DWORD*)((BYTE*)wp+0x36);//WAVEƒf[ƒ^‚ÌƒTƒCƒY
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(wp+5); 
	if(lpDS->CreateSoundBuffer(&dsbd, &lpDRAMBUFFER[no],
								NULL) != DS_OK) return(FALSE);	
    LPVOID lpbuf1, lpbuf2;
    DWORD dwbuf1, dwbuf2;
	HRESULT hr;
	hr = lpDRAMBUFFER[no]->Lock(0, file_size-58,
							&lpbuf1, &dwbuf1, &lpbuf2, &dwbuf2, 0); 
	if(hr != DS_OK)return (FALSE);
	CopyMemory(lpbuf1, (BYTE*)wp+0x3a, dwbuf1);//+3a‚Íƒf[ƒ^‚Ì“ª
	if(dwbuf2 != 0)	CopyMemory(lpbuf2, (BYTE*)wp+0x3a+dwbuf1, dwbuf2);
	lpDRAMBUFFER[no]->Unlock(lpbuf1, dwbuf1, lpbuf2, dwbuf2); 
	
	free(wp);
	return(TRUE);
}*/
void ChangeDramFrequency(unsigned char key,char track)
{
	if (lpDRAMBUFFER[track] != NULL)
		S_SetSoundFrequency(lpDRAMBUFFER[track], key*800+100);
}
void ChangeDramPan(unsigned char pan,char track)//512‚ªMAX‚Å256‚ªÉ°ÏÙ
{
	if (lpDRAMBUFFER[track] != NULL)
		S_SetSoundPan(lpDRAMBUFFER[track], (pan_tbl[pan]-256)*10);
}
void ChangeDramVolume(long volume,char track)//
{
	if (lpDRAMBUFFER[track] != NULL)
		S_SetSoundVolume(lpDRAMBUFFER[track], (volume-255)*8);
}
// ƒTƒEƒ“ƒh‚ÌÄ¶ 
void PlayDramObject(unsigned char key, int mode,char track)
{
	
    if(lpDRAMBUFFER[track] != NULL){
		switch(mode){
		case 0: // ’âŽ~
			S_StopSound(lpDRAMBUFFER[track]);
			S_RewindSound(lpDRAMBUFFER[track]);
			break;
		case 1: // Ä¶
			S_StopSound(lpDRAMBUFFER[track]);
			S_RewindSound(lpDRAMBUFFER[track]);
			ChangeDramFrequency(key,track);//Žü”g”‚ðÝ’è‚µ‚Ä
			S_PlaySound(lpDRAMBUFFER[track], false);
			break;
		case 2: // •à‚©‚¹’âŽ~
            break;
		case -1:
			break;
		}
    }
}

void PlayOrganKey(unsigned char key,char track,DWORD freq,int Nagasa)
{
	if (key >= 96) return;
	if (track < MAXMELODY && lpORGANBUFFER[track][key/12][0] != NULL){
		DWORD wait = timeGetTime();
		ChangeOrganFrequency(key%12,track,freq);//Žü”g”‚ðÝ’è‚µ‚Ä
		S_SetSoundVolume(lpORGANBUFFER[track][key / 12][0], ((200 * 100 / 0x7F) - 255) * 8);
		S_SetSoundPan(lpORGANBUFFER[track][key / 12][0], 0);
		S_PlaySoundFor(lpORGANBUFFER[track][key / 12][0], Nagasa);
//		lpORGANBUFFER[track][key/12][0]->Play(0, 0, 0); //C 2010.09.23 ‘¦Žž’âŽ~‚·‚éB
	} else if (lpDRAMBUFFER[track - MAXMELODY] != NULL) {
		S_StopSound(lpDRAMBUFFER[track - MAXMELODY]);
		S_RewindSound(lpDRAMBUFFER[track - MAXMELODY]);
		ChangeDramFrequency(key, track - MAXMELODY);//Žü”g”‚ðÝ’è‚µ‚Ä
		S_SetSoundVolume(lpDRAMBUFFER[track - MAXMELODY], ((200 * 100 / 0x7F) - 255) * 8);
		S_SetSoundPan(lpDRAMBUFFER[track - MAXMELODY], 0);
		S_PlaySound(lpDRAMBUFFER[track - MAXMELODY], false);
	}
}


//2010.08.14 A
void Rxo_PlayKey(unsigned char key,char track,DWORD freq, int Phase)
{
	if (key >= 96) return;
	if (track < MAXMELODY && lpORGANBUFFER[track][key/12][Phase] != NULL) {
		ChangeOrganFrequency(key%12,track,freq);
		S_SetSoundVolume(lpORGANBUFFER[track][key/12][Phase], ((200 * 100 / 0x7F) - 255) * 8);
		S_SetSoundPan(lpORGANBUFFER[track][key/12][Phase], 0);
		S_PlaySound(lpORGANBUFFER[track][key/12][Phase], true);
	} else if (lpDRAMBUFFER[track - MAXMELODY] != NULL) {
		S_StopSound(lpDRAMBUFFER[track - MAXMELODY]);
		S_RewindSound(lpDRAMBUFFER[track - MAXMELODY]);
		ChangeDramFrequency(key, track - MAXMELODY);//Žü”g”‚ðÝ’è‚µ‚Ä
		S_SetSoundVolume(lpDRAMBUFFER[track - MAXMELODY], ((200 * 100 / 0x7F) - 255) * 8);
		S_SetSoundPan(lpDRAMBUFFER[track - MAXMELODY], 0);
		S_PlaySound(lpDRAMBUFFER[track - MAXMELODY], false);
	}
}
//2010.08.14 A
void Rxo_StopKey(unsigned char key,char track, int Phase)
{
	if (key >= 96) return;
	if (track < MAXMELODY && lpORGANBUFFER[track][key/12][Phase] != NULL) {
		//lpORGANBUFFER[track][key/12][Phase]->Play(0, 0, 0);	// 2010.08.14 D
		S_StopSound(lpORGANBUFFER[track][key/12][Phase]);	// 2010.08.14 A
		S_RewindSound(lpORGANBUFFER[track][key / 12][Phase]);	// 2010.08.14 A
	} else if (lpDRAMBUFFER[track - MAXMELODY] != NULL) {
		S_StopSound(lpDRAMBUFFER[track - MAXMELODY]);
		S_RewindSound(lpDRAMBUFFER[track - MAXMELODY]);
	}	
}

//ƒfƒoƒbƒO—pB‚¢‚ë‚ñ‚Èó‘Ô‚ðo—ÍB
void Rxo_ShowDirectSoundObject(HWND hwnd)
{
	// ??
}

//‰¹‚ð‚·‚®‚É‘S•”Ž~‚ß‚é
void Rxo_StopAllSoundNow(void)
{
	int i,j,k;
	for (i = 0; i < SE_MAX; i++)
		if (lpSECONDARYBUFFER[i] != NULL) {
			S_StopSound(lpSECONDARYBUFFER[i]);
			S_RewindSound(lpSECONDARYBUFFER[i]);
		}
	
	for (i = 0; i < 8; i++){
		for (j = 0; j < 8; j++) {
			for (k = 0; k < 2; k++) {
				if (lpORGANBUFFER[i][j][k] != NULL) {
					S_StopSound(lpORGANBUFFER[i][j][k]);
					S_RewindSound(lpORGANBUFFER[i][j][k]);
				}
			}
		}
		if (lpDRAMBUFFER[i] != NULL) {
			S_StopSound(lpDRAMBUFFER[i]);
			S_RewindSound(lpDRAMBUFFER[i]);
		}
	}
	for (i = 0; i < MAXTRACK; i++)
		old_key[i]=255; //2014.05.02 A ‚±‚ê‚Å“ª‚ª•Ï‚È‰¹‚É‚È‚ç‚È‚­‚·‚éB
}

extern int sMetronome;

void ExportOrganyaBuffer(unsigned long sample_rate, float* output_stream, size_t frames_total, size_t fade_frames) {
	MUSICINFO mi;
	org_data.GetMusicInfo(&mi);

	output_frequency = sample_rate;
	vol_ticks = (long)((float)output_frequency * 0.004F);

	exporting = true;

	Rxo_StopAllSoundNow();
    S_ResetSounds();
    
	org_data.SetPlayPointer(0);

	ma_mutex_lock(&organya_mutex);

	int lastMetro = sMetronome;
	sMetronome = 0;

	organya_timer = mi.wait;
	organya_countdown = 0;

	float* stream = output_stream;
	size_t frames_done = 0;
	while (frames_done != frames_total) {
		float mix_buffer[0x400 * 2];
		size_t subframes = MIN(0x400, frames_total - frames_done);
		memset(mix_buffer, 0, subframes * sizeof(float) * 2);
		if (organya_timer == 0) {
			ma_mutex_lock(&mutex);
			S_MixSounds(mix_buffer, subframes);
			ma_mutex_unlock(&mutex);
		}
		else {
			unsigned int subframes_done = 0;
			while (subframes_done != subframes) {
				if (organya_countdown == 0) {
					organya_countdown = (organya_timer * output_frequency) / 1000;
					org_data.PlayData();
				}
				const unsigned int frames_to_do = MIN(organya_countdown, subframes - subframes_done);
				ma_mutex_lock(&mutex);
				S_MixSounds(mix_buffer + subframes_done * 2, frames_to_do);
				ma_mutex_unlock(&mutex);
				subframes_done += frames_to_do;
				organya_countdown -= frames_to_do;
			}
		}

		float fd = 1.0F;
		for (size_t i = 0; i < subframes * 2; ++i) {
			if (fade_frames > 0 && frames_done + i / 2 > frames_total - fade_frames) {
				if (i % 2 == 0)
					fd = ((float)(fade_frames - ((frames_done + i / 2) - (frames_total - fade_frames))) / (float)fade_frames);
				mix_buffer[i] = mix_buffer[i] * fd;
			}

			*stream++ = mix_buffer[i];
		}
		frames_done += subframes;
	}

	Rxo_StopAllSoundNow();

	organya_countdown = 0;
	organya_timer = 0;

	sMetronome = lastMetro;

	ma_mutex_unlock(&organya_mutex);

	output_frequency = device.sampleRate;
	vol_ticks = (long)((float)output_frequency * 0.004F);

	exporting = false;
}

void SetExportChannel(int track) {
	s_solo = track;
}
