#include <Windows.h>
#include <commctrl.h>

extern HINSTANCE hInst;

HWND CreateRebar(HWND hWnd)
{
    // Initialize common controls.
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    HWND hwndRebar = CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER,
        0, 0, 100, 50, hWnd, NULL, hInst, NULL);

    // create toolbar
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | TBSTYLE_TOOLTIPS,
        0, 0, 0, 0, hwndRebar, (HMENU)0, hInst, NULL);

    HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR16 | ILC_MASK, 3, 0);

    SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImageList);
    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

    TBBUTTON tbb[4] =
    {
        {0,0,TBSTATE_ENABLED,TBSTYLE_BUTTON},
        {1,1,TBSTATE_ENABLED,TBSTYLE_BUTTON},
        {2,2,TBSTATE_ENABLED,TBSTYLE_BUTTON},
    };

    SendMessage(hWndToolbar, (UINT)TB_ADDBUTTONS, 3, (LPARAM)&tbb);

    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);
    ShowWindow(hWndToolbar, SW_SHOW);

    // Initialize band info.
    REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };
    rbBand.cbSize = REBARBANDINFO_V3_SIZE;
    rbBand.fMask = RBBIM_STYLE | RBBIM_TEXT | RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE | RBBIM_COLORS;

    rbBand.fStyle = RBBS_GRIPPERALWAYS;

    // Get the height of the toolbar.
    DWORD dwBtnSize = (DWORD)SendMessage(hWndToolbar, TB_GETBUTTONSIZE, 0, 0);

    // Set values unique to the band with the toolbar.
    rbBand.clrFore = RGB(233, 233, 233);
    rbBand.clrBack = RGB(200, 200, 200);
    rbBand.lpText = TEXT("");
    rbBand.hwndChild = hWndToolbar;
    rbBand.cyChild = LOWORD(dwBtnSize);
    rbBand.cyMinChild = LOWORD(dwBtnSize);
    rbBand.cxMinChild = 3 * HIWORD(dwBtnSize);
    // The default width is the width of the buttons.
    rbBand.cx = 0;

    // Add the band
    SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);

    return hwndRebar;
}

int GetRebarHeight(HWND hwndRebar) {
    // check the rebar size
    WINDOWPLACEMENT wp;
    GetWindowPlacement(hwndRebar, &wp);
    return wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
}