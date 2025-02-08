#include <Windows.h>
#include <commctrl.h>

#include "resource.h"
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

    rbBand.fStyle = RBBS_BREAK;

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

HWND CreateRebar(HWND hWnd, HWND *outHwndToolbar, HWND* outHwndTrackbar)
{
    // Initialize common controls.
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    HWND hwndRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL,
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_BORDER | RBS_AUTOSIZE | RBS_BANDBORDERS | CCS_NODIVIDER,
        0, 0, 0, 0, hWnd, NULL, hInst, NULL);

    TBBUTTON tbb1[26] =
    {
        {0, IDM_INIT, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"New (Ctrl+N)"},
        {1, IDM_LOAD, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Open (Ctrl+O)"},
        {2, IDM_SAVEOVER, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Save (Ctrl+S)"},
        {3, IDM_SAVENEW, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Save As (Ctrl+Shift+S)"},
        {0, 0, 0, BTNS_SEP},
        {4, IDM_DLGSETTING, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Song Properties (F2)"},
        {0, 0, 0, BTNS_SEP},
        {6, IDM_SELECT_CUT, 0,BTNS_BUTTON, {}, 0, (INT_PTR)"Cut (Ctrl+X)"},
        {7, IDM_SELECT_COPY, 0,BTNS_BUTTON, {}, 0, (INT_PTR)"Copy (Ctrl+C)"},
        {8, IDM_SELECT_PASTE, 0, BTNS_BUTTON, {}, 0, (INT_PTR)"Paste (Ctrl+V)"},
        {0, 0, 0, BTNS_SEP},
        {19, IDM_UNDO, 0,BTNS_BUTTON, {}, 0, (INT_PTR)"Undo (Ctrl+Z)"},
        {20, IDM_REDO, 0, BTNS_BUTTON, {}, 0, (INT_PTR)"Redo (Ctrl+Y)"},
        {0, 0, 0, BTNS_SEP},
        {17, IDM_LOUPE_PLUS, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Zoom In (F7)"},
        {18, IDM_LOUPE_MINUS, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Zoom Out (F8)"},
        {0, 0, 0, BTNS_SEP},
        {9, IDC_START, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"To Song Start (Ctrl+Left)"},
        {10, IDC_LEFT, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Previous Measure (Shift+Left)"},
        {11, IDC_LEFTSTEP, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Backwards (Left)"},
        {12, IDC_PLAYPAUSE, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Play/Pause (F5)"},
        {14, IDC_RIGHTSTEP, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Forwards (Right)"},
        {15, IDC_RIGHT, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Next Measure (Shift+Right)"},
        {16, IDC_END, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"To Song End (Ctrl+Right)"},
        {0, 0, 0, BTNS_SEP},
        {5, IDC_PREFERENCES, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Preferences"},
    };
    *outHwndToolbar = CreateToolbar(hwndRebar, "TOOLBAR_ICONS", 26, tbb1);

    TBBUTTON tbb2[17] =
    {
        {0, IDM_TRACK1, TBSTATE_ENABLED | TBSTATE_CHECKED, BTNS_BUTTON, {}, 0, (INT_PTR)"Channel 1 (1)"},
        {1, IDM_TRACK2, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel 2 (2)"},
        {2, IDM_TRACK3, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel 3 (3)"},
        {3, IDM_TRACK4, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel 4 (4)"},
        {4, IDM_TRACK5, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel 5 (5)"},
        {5, IDM_TRACK6, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel 6 (6)"},
        {6, IDM_TRACK7, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel 7 (7)"},
        {7, IDM_TRACK8, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel 8 (8)"},
        {0, 0, 0, BTNS_SEP},
        {8, IDM_TRACKQ, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel Q (Q)"},
        {9, IDM_TRACKW, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel W (W)"},
        {10, IDM_TRACKE, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel E (E)"},
        {11, IDM_TRACKR, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel R (R)"},
        {12, IDM_TRACKT, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel T (T)"},
        {13, IDM_TRACKY, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel Y (Y)"},
        {14, IDM_TRACKU, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel U (U)"},
        {15, IDM_TRACKI, TBSTATE_ENABLED,BTNS_BUTTON, {}, 0, (INT_PTR)"Channel I (I)"},
    };
    *outHwndTrackbar = CreateToolbar(hwndRebar, "TRACKBAR_ICONS", 17, tbb2);

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