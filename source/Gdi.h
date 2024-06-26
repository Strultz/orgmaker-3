#include <windows.h>
#define MAXBITMAP		64

//void PutMusic(void);
//GDIの初期化
BOOL StartGDI(HWND hwnd);

BOOL ResizeGDI(HWND hwnd);

//GDIの開放
void EndGDI(void);
//画像のロード(リソースから)
HBITMAP InitBitmap(char *name,int bmp_no);
void InitCursor();
//いわゆるフリップ
void RefleshScreen(HDC hdc);
void PutRect(RECT* rect, int color);
void PutBitmap(long x,long y, RECT *rect, int bmp_no);
void PutBitmapCenter16(long x,long y, RECT *rect, int bmp_no);
////////////////////////////////
///以下はユニークな関数//////////
bool MakeMusicParts(unsigned char line,unsigned char dot);
void PutMusicParts(long x,long y);
void PutPanParts(long x);
void MakePanParts(unsigned char line,unsigned char dot);

void PutSelectParts(long x);

//以下はチト特殊。音符を描くときのみに用いることとする。
void Dw_BeginToDraw(void);
void Dw_FinishToDraw(void);
void Dw_PutBitmap(long x,long y, RECT *rect, int bmp_no);
int  Dw_PutBitmap_Head(long x,long y, RECT *rect, int bmp_no, int iNoteLength);
void Dw_PutBitmap_Center(long x,long y, RECT *rect, int bmp_no); //2014.05.26 A

void LoadSingleBitmap(HWND hdwnd, int item, int wdth, int hght, const char* name);
void GenerateWaveGraphic(char *wave100);

extern bool gIsDrawing;

