#include <Windows.h>
#include <commctrl.h>

#include "Toolbar.h"

extern HINSTANCE hInst;

static HWND CreateToolbar(HWND hwndRebar, const char *iconBitmap, int buttonCount, TBBUTTON tbb[]) {
    if (hwndRebar == NULL) {
        return NULL;
    }

    // create toolbar
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
        WS_CHILD | WS_CLIPSIBLINGS | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | TBSTYLE_LIST | WS_CLIPCHILDREN | CCS_NODIVIDER | CCS_NOPARENTALIGN,
        0, 0, 0, 0, hwndRebar, (HMENU)0, hInst, NULL);

    HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_MASK | ILC_COLOR32, buttonCount, 0);
    HBITMAP hBmp = LoadBitmap(hInst, iconBitmap);
    ImageList_AddMasked(hImageList, hBmp, RGB(0xFF, 0x00, 0xFF));

    SendMessage(hWndToolbar, TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_MIXEDBUTTONS | TBSTYLE_EX_DRAWDDARROWS | TBSTYLE_EX_HIDECLIPPEDBUTTONS);

    SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImageList);
    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hWndToolbar, TB_SETMAXTEXTROWS, 0, 0);

    SendMessage(hWndToolbar, (UINT)TB_ADDBUTTONS, buttonCount, (LPARAM)tbb);
    SendMessage(hWndToolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(18, 18));

    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
    ShowWindow(hWndToolbar, SW_SHOW);

    // Initialize band info.
    REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };
    rbBand.cbSize = REBARBANDINFO_V3_SIZE;
    rbBand.fMask = RBBIM_STYLE | RBBIM_SIZE | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_IDEALSIZE;

    rbBand.fStyle = RBBS_NOGRIPPER | RBBS_BREAK;

    SIZE s;
    if (!SendMessage(hWndToolbar, TB_GETMAXSIZE, 0, (LPARAM)&s)) {
        s.cx = s.cy = 0;
    }

    rbBand.hwndChild = hWndToolbar;
    rbBand.cxMinChild = s.cy;
    rbBand.cyMinChild = s.cy;
    rbBand.cxIdeal = s.cx;
    rbBand.cx = s.cx;

    // Add the band
    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

    return hWndToolbar;
}

HWND CreateRebar(HWND hWnd)
{
    // Initialize common controls.
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    HWND hwndRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER | RBS_AUTOSIZE | RBS_BANDBORDERS | CCS_NODIVIDER,
        0, 0, 0, 0, hWnd, NULL, hInst, NULL);

    TBBUTTON tbb[24] =
    {
        {0,0,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"New"},
        {1,1,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Open"},
        {2,2,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Save"},
        {3,3,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Save As"},
        {0,0,0,BTNS_SEP},
        {4,4,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Song Properties"},
        {0,0,0,BTNS_SEP},
        {6,6,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Cut"},
        {7,7,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Copy"},
        {8,8,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Paste"},
        {0,0,0,BTNS_SEP},
        {9,9,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Song Start"},
        {10,10,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Previous Measure"},
        {11,11,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Backwards"},
        {12,12,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Play"},
        {13,13,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Pause"},
        {14,14,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Forwards"},
        {15,15,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Next Measure"},
        {16,16,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Song End"},
        {0,0,0,BTNS_SEP},
        {17,17,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Zoom In"},
        {18,18,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Zoom Out"},
        {0,0,0,BTNS_SEP},
        {5,5,TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Preferences"},
    };
    CreateToolbar(hwndRebar, "TOOLBAR_ICONS", 24, tbb);

    RECT rcWindow;
    GetClientRect(hWnd, &rcWindow);
    MoveWindow(hwndRebar, 0, 0, rcWindow.right - rcWindow.left, GetRebarHeight(hwndRebar), TRUE);

    return hwndRebar;
}

int GetRebarHeight(HWND hwndRebar) {
    // check the rebar size
    WINDOWPLACEMENT wp;
    GetWindowPlacement(hwndRebar, &wp);
    return wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
}

HWND CreateStatusBar(HWND hWnd) {
    return NULL;
}