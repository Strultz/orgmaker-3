/*****							  *****/
/*****   DirectSoundサポート関数   *****/
/*****							  *****/

//#include <dsound.h>

typedef struct {
	short wave_size;//波形のサイズ
	short oct_par;//オクターブを実現する掛け率(/8)
	short oct_size;//オクターブを実現する掛け率(/8)
} OCTWAVE;

extern OCTWAVE oct_wave[8];
extern short freq_tbl[12];
extern short pan_tbl[13];
extern char *wave_data;

// DirectSoundの開始
BOOL InitDirectSound(HWND hwnd);
BOOL LoadSoundObject( char *file_name, int no);
// DirectSoundの終了
void EndDirectSound(void);
void ReleaseSoundObject(int no);//バッファの開放
BOOL InitSoundObject(LPCSTR resname, int no);
// サウンドの再生
void PlaySoundObject(int no, int mode);
void ChangeSoundVolume(int no, long volume);//9999がMAX2195?がﾉｰﾏﾙ
void ChangeSoundFrequency(int no, DWORD rate);//300がMAX300がﾉｰﾏﾙ
void ChangeSoundPan(int no, long pan);

BOOL MakeSoundObject8(char *wavep,char track, char pipi);

void ChangeOrganFrequency(unsigned char key,char track,DWORD a);//9999がMAXで2195?がﾉｰﾏﾙ
void ChangeOrganVolume(int no, long volume,char track);//300がMAXで300がﾉｰﾏﾙ
void ChangeOrganPan(unsigned char key, unsigned char pan,char track);//0が←で6がﾉｰﾏﾙ11が→
void PlayOrganObject(unsigned char key, int mode,char track,DWORD freq, bool pipi);
void ResumeOrganObject(unsigned char key, char track, DWORD freq, bool pipi, int played_ms);
void PlayOrganKey(unsigned char key,char track,DWORD freq, int Nagasa = 80);
BOOL LoadWaveData100(const char *file);
BOOL InitWaveData100(void);
BOOL DeleteWaveData100(void);

BOOL MakeOrganyaWave(char track,char wave_no, char pipi);

//BOOL LoadDramObject(char *file_name, int no);
BOOL InitDramObject(char drum, int no);
void ReleaseDramObject(char track);
void ChangeDramFrequency(unsigned char key,char track);
void ChangeDramPan(unsigned char pan,char track);//512がMAXで256がﾉｰﾏﾙ
void ChangeDramVolume(long volume,char track);//300がMAXで300がﾉｰﾏﾙ
void PlayDramObject(unsigned char key, int mode,char track);
void ResumeDramObject(unsigned char key, char track, int played_ms);


void Rxo_ShowDirectSoundObject(HWND hwnd);
void Rxo_StopAllSoundNow(void);

void Rxo_PlayKey(unsigned char key,char track,DWORD freq, int Phase);
void Rxo_StopKey(unsigned char key,char track, int Phase);

void S_SetOrganyaTimer(unsigned short timer);

void SetupExportBuffer(unsigned long sample_rate, size_t frames_total, size_t fade_frames);
void EndExportBuffer(void);
void ExportOrganyaBuffer(float* output_stream, size_t do_frames);
void SetExportChannel(int track);
