#pragma once

void CreateToolbarClass(void);
HWND CreateRebar(HWND hwnd);
void CreateToolbars(HWND hwndRebar, HWND outHwnd[4]);

int GetBarWidth(HWND hwndBar);
int GetBarHeight(HWND hwndBar);

HWND CreateStatusBar(HWND hwnd);