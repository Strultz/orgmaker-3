#pragma once

HWND CreateRebar(HWND hWnd);
void CreateToolbars(HWND hwndRebar, HWND outHwnd[4]);
int GetBarHeight(HWND hwndBar);

HWND CreateStatusBar(HWND hWnd);