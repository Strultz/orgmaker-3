#pragma once
#include <Windows.h>

#define TOOLBAR_MAIN_BUTTON_COUNT 27
#define TOOLBAR_CHANNEL_BUTTON_COUNT 17

void CreateToolbarClass(void);
HWND CreateRebar(HWND hwnd);
void CreateToolbars(HWND hwndRebar, HWND outHwnd[4]);
void SetToolbarImageList(HWND hwndToolbar, const char* imageName, int imageCount);

int GetBarWidth(HWND hwndBar);
int GetBarHeight(HWND hwndBar);

HWND CreateStatusBar(HWND hwnd);
