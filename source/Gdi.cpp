#include <stdio.h>

#include "Setting.h"
#include "DefOrg.h"
#include "OrgData.h"
#include "Gdi.h"

#include <ddraw.h>

#define MAXBITMAP		64

static LPDIRECTDRAW directDraw;
static LPDIRECTDRAWSURFACE frontBuffer;
static LPDIRECTDRAWSURFACE backBuffer;
static LPDIRECTDRAWSURFACE hBmp[MAXBITMAP];

static struct {
	char name[20];
	BOOL file;
	BOOL system;
	int width;
	int height;
} bitmapMeta[MAXBITMAP];

static LPDIRECTDRAWCLIPPER clipper;
static RECT screenRect;

HBITMAP waveBmp; // for dialog

extern int gDrawDouble;	//両方のトラックグループを描画する
extern int rebarHeight;

extern RECT WinRect; //ウィンドウサイズ保存用 A 2010.09.22
extern int NoteWidth;
extern int NoteEnlarge_Until_16px;
extern char gSelectedTheme[MAX_PATH];

BOOL StartGDI(HWND hwnd) {
	int nScreenWidth = GetSystemMetrics(SM_CXMAXTRACK);
    int nScreenHeight = GetSystemMetrics(SM_CYMAXTRACK);

	if (DirectDrawCreate(NULL, &directDraw, NULL) != DD_OK)
		return FALSE;

	directDraw->SetCooperativeLevel(hwnd, DDSCL_NORMAL);

	memset(bitmapMeta, 0, sizeof(bitmapMeta));

	DDSURFACEDESC ddsd;
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	ddsd.dwBackBufferCount = 0;

	if (directDraw->CreateSurface(&ddsd, &frontBuffer, NULL) != DD_OK)
		return FALSE;

	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = nScreenWidth;
	ddsd.dwHeight = nScreenHeight;

	if (directDraw->CreateSurface(&ddsd, &backBuffer, NULL) != DD_OK)
		return FALSE;

	directDraw->CreateClipper(0, &clipper, NULL);
	clipper->SetHWnd(0, hwnd);
	frontBuffer->SetClipper(clipper);

	return TRUE;
}

BOOL ResizeGDI(HWND hwnd) {
	if (backBuffer != NULL) {
		backBuffer->Release();
		backBuffer = NULL;
	}

	DDSURFACEDESC ddsd;
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = WWidth;
	ddsd.dwHeight = WHeight;

	if (directDraw->CreateSurface(&ddsd, &backBuffer, NULL) != DD_OK)
		return FALSE;

	return TRUE;
}

void EndGDI(void) {
	int i;
	for (i = 0; i < MAXBITMAP; i++) {
		if (hBmp[i] != NULL) {
			hBmp[i]->Release();
			hBmp[i] = NULL;
		}
	}

	if (frontBuffer != NULL) {
		frontBuffer->Release();
		frontBuffer = NULL;
	}
	if (backBuffer != NULL) {
		backBuffer->Release();
		backBuffer = NULL;
	}

	if (directDraw != NULL)
	{
		directDraw->Release();
		directDraw = NULL;
	}

	memset(bitmapMeta, 0, sizeof(bitmapMeta));
}

HBITMAP GetThemedBitmap(const char* name, int flags) {
	bool useTheme = false;
	char str[MAX_PATH + 20];

	if (strlen(gSelectedTheme) > 0)
		useTheme = true;

	memset(str, '\0', sizeof(str));

	if (useTheme)
		sprintf(str, "%s\\%s%s", gSelectedTheme, name, ".bmp");
	else
		strcpy(str, name);

	HANDLE handle = LoadImage(useTheme ? NULL : GetModuleHandle(NULL), str, IMAGE_BITMAP, 0, 0, useTheme ? (LR_LOADFROMFILE | flags) : flags);
	if (handle == NULL && useTheme) { // fallback
		handle = LoadImage(GetModuleHandle(NULL), name, IMAGE_BITMAP, 0, 0, flags);
		if (handle == NULL) {
			return NULL;
		}
	}
	else if (handle == NULL) {
		return NULL;
	}

	return (HBITMAP)handle;
}

BOOL InitBitmap(char *name, int no) {
	if (no >= MAXBITMAP)
		return FALSE;

	HBITMAP handle = GetThemedBitmap(name, LR_CREATEDIBSECTION);
	if (handle == NULL)
		return FALSE;

	BITMAP bitmap;
	GetObject(handle, sizeof(BITMAP), &bitmap);

	DDSURFACEDESC ddsd;
	memset(&ddsd, 0, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = bitmap.bmWidth;
	ddsd.dwHeight = bitmap.bmHeight;

	if (hBmp[no] == NULL) {
		if (directDraw->CreateSurface(&ddsd, &hBmp[no], NULL) != DD_OK) {
			DeleteObject(handle);
			return FALSE;
		}
	}

	HDC hdc = CreateCompatibleDC(NULL);
	HGDIOBJ hgdiobj = SelectObject(hdc, handle);

	HDC hdc2;
	hBmp[no]->GetDC(&hdc2);
	StretchBlt(hdc2, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
	hBmp[no]->ReleaseDC(hdc2);

	SelectObject(hdc, hgdiobj);
	DeleteDC(hdc);

	DDCOLORKEY ddcolorkey;
	ddcolorkey.dwColorSpaceLowValue = RGB(0x00, 0x4C, 0x00);
	ddcolorkey.dwColorSpaceHighValue = RGB(0x00, 0x4C, 0x00);

	hBmp[no]->SetColorKey(DDCKEY_SRCBLT, &ddcolorkey);
	hBmp[no]->SetClipper(clipper);

	DeleteObject(handle);

	bitmapMeta[no].file = (strlen(gSelectedTheme) > 0);
	bitmapMeta[no].system = FALSE;
	bitmapMeta[no].width = bitmap.bmWidth;
	bitmapMeta[no].height = bitmap.bmHeight;
	strncpy(bitmapMeta[no].name, name, 20-1);

	return TRUE;
}

void InitCursor(void) {
	bool useTheme = false;
	char str[MAX_PATH + 20];
	if (strlen(gSelectedTheme) > 0)
		useTheme = true;

	memset(str, '\0', sizeof(str));

	if (useTheme)
		sprintf(str, "%s\\CURSOR%s", gSelectedTheme, ".cur");
	else
		strcpy(str, "CURSOR");

	HCURSOR ccur = (HCURSOR)LoadImage(useTheme ? NULL : GetModuleHandle(NULL), str, IMAGE_CURSOR, 0, 0, useTheme ? LR_LOADFROMFILE : 0);
	if (ccur == NULL && useTheme) { // fallback
		ccur = (HCURSOR)LoadImage(GetModuleHandle(NULL), "CURSOR", IMAGE_CURSOR, 0, 0, 0);
	}

	ccur = (HCURSOR)SetClassLongPtr(hWnd, GCLP_HCURSOR, (LONG)ccur);
	if (ccur != NULL)
		DestroyCursor(ccur);
}

BOOL SystemTask(void);

void RestoreSurfaces(void) {
    if (frontBuffer == NULL)
		return;

	if (backBuffer == NULL)
		return;
    
    if (frontBuffer->IsLost() == DDERR_SURFACELOST) {
		frontBuffer->Restore();
    }
    
    if (backBuffer->IsLost() == DDERR_SURFACELOST) {
		backBuffer->Restore();
    }
    
    for (int i = 0; i < MAXBITMAP; ++i) {
        if (hBmp[i] != NULL) {
            if (hBmp[i]->IsLost() == DDERR_SURFACELOST) {
                hBmp[i]->Restore();
                
                if (!bitmapMeta[i].system) {
                    InitBitmap(bitmapMeta[i].name, i);
                }
            }
        }
    }
}

BOOL RefleshScreen(HWND hwnd, BOOL mainLoopUpdate) {
	static DWORD timePrev;
	static DWORD timeNow;
	static DWORD lek = 0;

	if (!mainLoopUpdate)
	{
		// for WM_TIMER
		timeNow = timeGetTime();

		if (timeNow < timePrev + (lek % 3 != 0) + 0x10)
		{
			return FALSE;
		}

		lek += 1;
		org_data.PutMusic();
	}
	else
	{
		lek += 1;

		while (TRUE)
		{
			if (!SystemTask())
				return FALSE;

			timeNow = timeGetTime();

			if (timeNow >= timePrev + (lek % 3 != 0) + 0x10)
				break;

			Sleep(1);
		}
	}
	
	if (timeNow >= timePrev + 100)
		timePrev = timeNow;
	else
		timePrev += (lek % 3 != 0) + 0x10;

	static RECT dst;
	static POINT pnt;

	GetClientRect(hwnd, &dst);
	pnt.x = dst.left;
	pnt.y = dst.top;

	ClientToScreen(hwnd, &pnt);
	dst.left = pnt.x;
	dst.top = pnt.y + rebarHeight;
	dst.right = dst.left + WWidth;
	dst.bottom = dst.top + WHeight;

	screenRect.left = 0;
	screenRect.top = 0;
	screenRect.right = WWidth;
	screenRect.bottom = WHeight;

	frontBuffer->Blt(&dst, backBuffer, &screenRect, DDBLT_WAIT, NULL);

	RestoreSurfaces();

	return TRUE;
}

void PutRect(RECT *rect, int color)
{
	static DDBLTFX ddbltfx;
	memset(&ddbltfx, 0, sizeof(DDBLTFX));
	ddbltfx.dwSize = sizeof(DDBLTFX);
	ddbltfx.dwFillColor = color;

	static RECT rc;
	rc.left = rect->left;
	rc.top = rect->top;
	rc.right = rect->right;
	rc.bottom = rect->bottom;

	if (rc.left < 0)
		rc.left = 0;
	if (rc.top < 0)
		rc.top = 0;
	if (rc.right > WWidth)
		rc.right = WWidth;
	if (rc.bottom > WHeight)
		rc.bottom = WHeight;

	backBuffer->Blt(&rc, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
}

void PutBitmap(long x, long y, RECT *rect, int bmp_no)
{
	static RECT rcWork;
	static RECT rcSet;

	rcWork.left = rect->left;
	rcWork.top = rect->top;
	rcWork.right = rect->right;
	rcWork.bottom = rect->bottom;

	if (rcWork.right - rcWork.left > bitmapMeta[bmp_no].width) {
		rcWork.right = rcWork.left + bitmapMeta[bmp_no].width;
	}
	if (rcWork.bottom - rcWork.top > bitmapMeta[bmp_no].height) {
		rcWork.bottom = rcWork.top + bitmapMeta[bmp_no].height;
	}
	if (x + rect->right - rect->left > WWidth)
		rcWork.right -= (x + rect->right - rect->left) - WWidth;
	if (y + rect->bottom - rect->top > WHeight)
		rcWork.bottom -= (y + rect->bottom - rect->top) - WHeight;
	if (x < 0) {
		rcWork.left -= x;
		x = 0;
	}
	if (y < 0) {
		rcWork.top -= y;
		y = 0;
	}

	rcSet.left = x;
	rcSet.top = y;
	rcSet.right = x + rcWork.right - rcWork.left;
	rcSet.bottom = y + rcWork.bottom - rcWork.top;

	backBuffer->Blt(&rcSet, hBmp[bmp_no], &rcWork, DDBLT_WAIT, NULL);
}

void PutBitmapCenter16(long x, long y, RECT* rect, int bmp_no)
{
	static RECT rcWork;

	if (rect->right - rect->left != 16 || NoteWidth == 16) {
		PutBitmap(x, y, rect, bmp_no);
		return;
	}

	int ww = NoteWidth - 4;

	rcWork.top = rect->top;
	rcWork.bottom = rect->bottom;

	rcWork.left = rect->left;
	rcWork.right = rcWork.left + 2;
	PutBitmap(x, y, &rcWork, bmp_no);
	rcWork.left = rect->left + 2;
	rcWork.right = rcWork.left + ww;
	PutBitmap(x + 2, y, &rcWork, bmp_no);
	rcWork.left = rect->left + 14;
	rcWork.right = rcWork.left + 2;
	PutBitmap(x + 2 + ww, y, &rcWork, bmp_no);
}

void PutBitmap2(long x, long y, RECT* rect, int bmp_no)
{
	static RECT rcWork;

	if (NoteWidth == 16) {
		PutBitmap(x, y, rect, bmp_no);
		return;
	}

	int ww = NoteWidth - 4;

	rcWork.top = rect->top;
	rcWork.bottom = rect->bottom;

	rcWork.left = rect->left;
	rcWork.right = rcWork.left + 2;
	PutBitmap(x, y, &rcWork, bmp_no);
	if (ww > 0) {
		rcWork.left = rect->left + 2;
		rcWork.right = rcWork.left + ww;
		PutBitmap(x + 2, y, &rcWork, bmp_no);
	}
	rcWork.left = rect->left + 14;
	rcWork.right = rcWork.left + 2;
	PutBitmap(x + 2 + ww, y, &rcWork, bmp_no);
}

void PutBitmapHead(long x, long y, RECT* rect, int bmp_no, int noteLength)
{
	static RECT rcWork;

	int totalLength = NoteWidth * noteLength;
	int bitWidth = totalLength;

	if (bitWidth > 16)
		bitWidth = 16;

	if (NoteEnlarge_Until_16px == 0) {
		bitWidth = NoteWidth;
		totalLength = bitWidth;
	}

	if (NoteWidth == 16 || totalLength >= 16) {
		PutBitmap(x, y, rect, bmp_no);
		return;
	}

	int ww = bitWidth - 4;

	rcWork.top = rect->top;
	rcWork.bottom = rect->bottom;

	rcWork.left = rect->left;
	rcWork.right = rcWork.left + 2;
	PutBitmap(x, y, &rcWork, bmp_no);
	if (ww > 0) {
		rcWork.left = rect->left + 2;
		rcWork.right = rcWork.left + ww;
		PutBitmap(x + 2, y, &rcWork, bmp_no);
	}
	rcWork.left = rect->left + 14;
	rcWork.right = rcWork.left + 2;
	PutBitmap(x + 2 + ww, y, &rcWork, bmp_no);
}

void PutBitmapCenter(long x, long y, RECT *rect, int bmp_no)
{
	static RECT rcWork;

	if (NoteWidth == 16) {
		PutBitmap(x, y, rect, bmp_no);
		return;
	}

	int ww = NoteWidth / 2;

	rcWork.top = rect->top;
	rcWork.bottom = rect->bottom;

	rcWork.left = rect->left;
	rcWork.right = rcWork.left + ww;
	PutBitmap(x, y, &rcWork, bmp_no);
	rcWork.left = rect->left + 16 - ww;
	rcWork.right = rcWork.left + ww;
	PutBitmap(x + ww, y, &rcWork, bmp_no);
	rcWork.left = rect->left + 7;
	rcWork.right = rcWork.left + 2;
	PutBitmap(x + ww - 1, y, &rcWork, bmp_no);
}

void LoadSingleBitmap(HWND hdwnd, int item, int wdth, int hght, const char* name) {
	bool useTheme = (strlen(gSelectedTheme) > 0);

	HANDLE hBmp;
	HANDLE hBmp2;
	char str[MAX_PATH + 20];
	memset(str, '\0', sizeof(str));
	// File name for theme
	if (useTheme) sprintf(str, "%s\\%s%s", gSelectedTheme, name, ".bmp");
	else strcpy(str, name);
	// Load it
	hBmp = (HBITMAP)LoadImage(useTheme ? NULL : hInst, str, IMAGE_BITMAP, wdth, hght, useTheme ? (LR_LOADFROMFILE | LR_DEFAULTCOLOR) : LR_DEFAULTCOLOR);
	if (hBmp == NULL && useTheme) // fallback, if the theme failed
		hBmp = (HBITMAP)LoadImage(hInst, name, IMAGE_BITMAP, wdth, hght, LR_DEFAULTCOLOR);
	// Now set it
	hBmp2 = (HBITMAP)SendDlgItemMessage(hdwnd, item, BM_SETIMAGE, IMAGE_BITMAP, (long)hBmp);
	if (hBmp2 != NULL) DeleteObject(hBmp2);
}

void GenerateWaveGraphic(char *wave100) {
	if (waveBmp != NULL) DeleteObject(waveBmp);

	HBITMAP waveImg = (HBITMAP)LoadImage(hInst, "WAVE100", IMAGE_BITMAP, 377, 491, LR_DEFAULTCOLOR); // BG
	if (waveImg == NULL) return;

	HDC hdc = GetDC(hWnd);
	waveBmp = CreateCompatibleBitmap(hdc, 377, 491);
	if (waveBmp == NULL) {
		ReleaseDC(hWnd, hdc);
		return;
	}

	HDC toDC = CreateCompatibleDC(hdc);
	HDC fromDC = CreateCompatibleDC(hdc);
	HBITMAP toold = (HBITMAP)SelectObject(toDC, waveBmp);
	HBITMAP fromold = (HBITMAP)SelectObject(fromDC, waveImg);

	SelectObject(toDC, GetStockObject(DC_PEN));
	SetDCPenColor(toDC, RGB(0x00, 0xFF, 0x00));

	BitBlt(toDC, 0, 0, 377, 491, fromDC, 0, 0, SRCCOPY);
	for (int i = 0; i < 100; ++i) {
		int x = 8 + (i % 10) * 36 + ((i % 10) > 4 ? 4 : 0); // box position
		int y = 16 + (i / 10) * 48 + ((i / 10) > 4 ? 4 : 0);

		for (int j = 0; j < 256; ++j) {
			int sample = wave100[i * 256 + j]; // Get the sample
			if (j == 0)
				MoveToEx(toDC, x + (j / 8), y + 16 + (-sample / 8), NULL);
			else
				LineTo(toDC, x + (j / 8), y + 16 + (-sample / 8)); // Draw line
		}
	}

	SelectObject(toDC, toold);
	SelectObject(fromDC, fromold);
	DeleteDC(toDC);
	DeleteDC(fromDC);
	ReleaseDC(hWnd, hdc);
	DeleteObject(waveImg);
}


