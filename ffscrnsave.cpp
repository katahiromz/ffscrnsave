#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <strsafe.h>
#include <string>

#define CLASSNAME  L"ffscrnsave"

HWND g_hMainWnd = NULL;

BOOL OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    LPCWSTR psz = (LPCWSTR)lParam;
    SetDlgItemTextW(hwnd, edt1, psz);
    return TRUE;
}

void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDOK:
    case IDCANCEL:
        EndDialog(hwnd, id);
        break;
    }
}

INT_PTR CALLBACK
DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_INITDIALOG, OnInitDialog);
        HANDLE_MSG(hwnd, WM_COMMAND, OnCommand);
    }
    return 0;
}

void message(INT uType, LPCWSTR fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    WCHAR buf[1024];
    StringCchVPrintfW(buf, _countof(buf), fmt, va);
    DialogBoxParamW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(1), NULL, DialogProc, (LPARAM)buf);
    va_end(va);
}

void version(void)
{
    message(MB_ICONINFORMATION, L"ffscrnsave version 0.1 by katahiromz");
}

void usage(void)
{
    message(MB_ICONINFORMATION,
        L"Usage: ffscrnsave [Options] your_file.scr\r\n"
        L"Options:\r\n"
        L"  -left LEFT               Set the x position for the left of the window (default is a centered window).\r\n"
        L"  -top TOP                 Set the y position for the top of the window (default is a centered window).\r\n"
        L"  -x WIDTH                 Force displayed width.\r\n"
        L"  -y HEIGHT                Force displayed height.\r\n"
        L"  -fs                      Start in fullscreen mode.\r\n"
        L"  -noborder                Borderless window.\r\n"
        L"  -window_title \"TITLE\"    Set window title (default is the input filename).\r\n"
    );
}

BOOL startSaver(HWND hwnd, LPCWSTR filename)
{
    WCHAR szBuf[MAX_PATH * 2];
    StringCchPrintfW(szBuf, _countof(szBuf), L"\"%s\" /p %u", filename, (UINT)(UINT_PTR)hwnd);

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { NULL };
    if (!CreateProcessW(NULL, szBuf, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
        return FALSE;

    WaitForInputIdle(pi.hProcess, 3000);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return TRUE;
}

void exitSaver(HWND hwnd)
{
    HWND hChild = GetTopWindow(hwnd);
    if (hChild)
        SendMessageW(hChild, WM_CLOSE, 0, 0);
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    g_hMainWnd = hwnd;

    SetCursor(LoadCursor(NULL, IDC_ARROW));

    return TRUE;
}

void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    exitSaver(hwnd);
}

void OnDestroy(HWND hwnd)
{
    g_hMainWnd = NULL;
    PostQuitMessage(0);
}

void OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
    if (fDown)
        DestroyWindow(hwnd);
}

int OnMouseActivate(HWND hwnd, HWND hwndTopLevel, UINT codeHitTest, UINT msg)
{
    DestroyWindow(hwnd);
    return MA_ACTIVATE;
}

void OnNCMouseMove(HWND hwnd, int x, int y, UINT codeHitTest)
{
    SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    SetCursor(LoadCursor(NULL, IDC_ARROW));
}

LRESULT CALLBACK
WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        HANDLE_MSG(hwnd, WM_CREATE, OnCreate);
        HANDLE_MSG(hwnd, WM_LBUTTONDOWN, OnLButtonDown);
        HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK, OnLButtonDown);
        HANDLE_MSG(hwnd, WM_MOUSEACTIVATE, OnMouseActivate);
        HANDLE_MSG(hwnd, WM_NCMOUSEMOVE, OnNCMouseMove);
        HANDLE_MSG(hwnd, WM_MOUSEMOVE, OnMouseMove);
        HANDLE_MSG(hwnd, WM_KEYDOWN, OnKey);
        HANDLE_MSG(hwnd, WM_KEYUP, OnKey);
        HANDLE_MSG(hwnd, WM_DESTROY, OnDestroy);
    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

BOOL registerClasses(void)
{
    WNDCLASSEXW wcx = { sizeof(wcx) };
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcx.lpfnWndProc = WindowProc;
    wcx.hInstance = GetModuleHandleW(NULL);
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = NULL;
    wcx.lpszClassName = CLASSNAME;
    wcx.hIconSm = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
        0
    );
    return RegisterClassExW(&wcx);
}

typedef struct FFSCRNSAVE
{
    bool noborder = false, fullscreen = false;
    std::wstring filename, window_title;
    INT x = CW_USEDEFAULT, y = CW_USEDEFAULT;
    INT left = CW_USEDEFAULT, top = CW_USEDEFAULT;
    HWND hWnd;

    BOOL createMainWnd(void)
    {
        DWORD style, exstyle = WS_EX_TOPMOST;

        if (noborder || fullscreen)
        {
            style = WS_POPUPWINDOW;
        }
        else
        {
            style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
        }

        RECT rcWork;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
        SIZE sizWork = { (rcWork.right - rcWork.left), (rcWork.bottom - rcWork.top) };

        if (fullscreen)
        {
            left = 0;
            top = 0;
            x = GetSystemMetrics(SM_CXSCREEN);
            y = GetSystemMetrics(SM_CYSCREEN);
        }
        else
        {
            if (x == CW_USEDEFAULT)
                x = sizWork.cx * 2 / 3;
            if (y == CW_USEDEFAULT)
                y = sizWork.cy * 2 / 3;
            if (left == CW_USEDEFAULT)
                left = (sizWork.cx - x) / 2;
            if (top == CW_USEDEFAULT)
                top  = (sizWork.cy - y) / 2;
        }

        if (x <= 0)
            x = sizWork.cx * 2 / 3;
        if (y <= 0)
            y = sizWork.cy * 2 / 3;

        RECT rc;
        rc.left = left;
        rc.top = top;
        rc.right = left + x;
        rc.bottom = top + y;
        AdjustWindowRectEx(&rc, style, FALSE, exstyle);

        hWnd = CreateWindowExW(exstyle, CLASSNAME, window_title.c_str(), style,
            rc.left, rc.top,
            rc.right - rc.left, rc.bottom - rc.top,
            NULL, NULL, GetModuleHandleW(NULL), NULL);
        if (!hWnd)
            return FALSE;

        ShowWindow(hWnd, SW_SHOWNORMAL);
        UpdateWindow(hWnd);

        if (!startSaver(hWnd, filename.c_str()))
            return FALSE;

        return TRUE;
    }

    int just_do_it(void)
    {
        InitCommonControls();

        if (filename.empty())
        {
            message(MB_ICONERROR, L"ERROR: No filename specified.");
            return -1;
        }

        WCHAR szFileName[MAX_PATH];
        GetFullPathNameW(filename.c_str(), _countof(szFileName), szFileName, NULL);

        if (window_title.empty())
            window_title = szFileName;

        if (!registerClasses())
        {
            message(MB_ICONERROR, L"ERROR: Failed to register classes.");
            return -2;
        }

        if (!createMainWnd())
        {
            message(MB_ICONERROR, L"ERROR: Failed to create the main window.");
            return -3;
        }

        MSG msg;
        while (GetMessageW(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        return (INT)msg.wParam;
    }
} FFSCRNSAVE;

int wmain(INT argc, LPWSTR *argv)
{
    if (argc <= 1)
    {
        usage();
        return 0;
    }

    FFSCRNSAVE ffscrnsave;
    for (INT iarg = 1; iarg < argc; ++iarg)
    {
        LPWSTR arg = argv[iarg];
        if (lstrcmpiW(arg, L"-help") == 0 || lstrcmpiW(arg, L"--help") == 0)
        {
            usage();
            return 0;
        }
        if (lstrcmpiW(arg, L"-version") == 0 || lstrcmpiW(arg, L"--version") == 0)
        {
            version();
            return 0;
        }
        if (lstrcmpiW(arg, L"-x") == 0)
        {
            arg = argv[++iarg];
            if (iarg < argc)
            {
                ffscrnsave.x = (INT)wcstoul(arg, NULL, 0);
                continue;
            }
            else
            {
                message(MB_ICONERROR, L"ERROR: Option -x needs an option.");
                return -1;
            }
        }
        if (lstrcmpiW(arg, L"-y") == 0)
        {
            arg = argv[++iarg];
            if (iarg < argc)
            {
                ffscrnsave.y = (INT)wcstoul(arg, NULL, 0);
                continue;
            }
            else
            {
                message(MB_ICONERROR, L"ERROR: Option -y needs an option.");
                return -1;
            }
        }
        if (lstrcmpiW(arg, L"-left") == 0)
        {
            arg = argv[++iarg];
            if (iarg < argc)
            {
                ffscrnsave.left = (INT)wcstol(arg, NULL, 0);
                continue;
            }
            else
            {
                message(MB_ICONERROR, L"ERROR: Option -left needs an option.");
                return -1;
            }
        }
        if (lstrcmpiW(arg, L"-top") == 0)
        {
            arg = argv[++iarg];
            if (iarg < argc)
            {
                ffscrnsave.top = (INT)wcstol(arg, NULL, 0);
                continue;
            }
            else
            {
                message(MB_ICONERROR, L"ERROR: Option -top needs an option.");
                return -1;
            }
        }
        if (lstrcmpiW(arg, L"-window_title") == 0)
        {
            arg = argv[++iarg];
            if (iarg < argc)
            {
                ffscrnsave.window_title = arg;
                continue;
            }
            else
            {
                message(MB_ICONERROR, L"ERROR: Option -window_title needs an option.");
                return -1;
            }
        }
        if (lstrcmpiW(arg, L"-fs") == 0)
        {
            ffscrnsave.fullscreen = true;
            continue;
        }
        if (lstrcmpiW(arg, L"-noborder") == 0)
        {
            ffscrnsave.noborder = true;
            continue;
        }

        if (arg[0] == L'-')
        {
            message(MB_ICONERROR, L"ERROR: '%s' is invalid argument.", arg);
            return -1;
        }

        if (ffscrnsave.filename.empty())
        {
            ffscrnsave.filename = arg;
            continue;
        }
        else
        {
            message(MB_ICONERROR, L"ERROR: Too many argument.");
            return -1;
        }
    }

    return ffscrnsave.just_do_it();
}

INT WINAPI
WinMain(HINSTANCE   hInstance,
        HINSTANCE   hPrevInstance,
        LPSTR       lpCmdLine,
        INT         nCmdShow)
{
    INT myargc;
    LPWSTR *myargv = CommandLineToArgvW(GetCommandLineW(), &myargc);
    INT ret = wmain(myargc, myargv);
    LocalFree(myargv);
    return ret;
}
