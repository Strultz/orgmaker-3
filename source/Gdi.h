#include <windows.h>
#define MAXBITMAP		64

//void PutMusic(void);
//GDIの初期化
BOOL StartGDI(HWND hwnd);

BOOL ResizeGDI(HWND hwnd);

//GDIの開放
void EndGDI(void);
//画像のロード(リソースから)
BOOL InitBitmap(char *name, int no);
void InitCursor();
//いわゆるフリップ
BOOL RefleshScreen(HWND hwnd);
void PutRect(RECT* rect, int color);
void PutBitmap(long x, long y, RECT *rect, int bmp_no);
void PutBitmap2(long x, long y, RECT* rect, int bmp_no);
void PutBitmapCenter16(long x, long y, RECT* rect, int bmp_no);
void PutBitmapHead(long x, long y, RECT* rect, int bmp_no, int noteLength);
void PutBitmapCenter(long x, long y, RECT *rect, int bmp_no);

void LoadSingleBitmap(HWND hdwnd, int item, int wdth, int hght, const char* name);
void GenerateWaveGraphic(char *wave100);

extern bool gIsDrawing;

