#pragma once

HWND CreateRebar(HWND hWnd);
void CreateToolbars(HWND hwndRebar, HWND outHwnd[4]);
int GetRebarHeight(HWND hwndRebar);

HWND CreateStatusBar(HWND hWnd);