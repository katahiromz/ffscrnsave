// FFscrnsave - A ScreenSaver viewer for Windows
// License: MIT

// Detect memory leaks (for Debug and MSVC)
#if defined(_MSC_VER) && !defined(NDEBUG) && !defined(_CRTDBG_MAP_ALLOC)
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlwapi.h>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <string>
#include <tchar.h>
#include <strsafe.h>

#define CLASSNAME  L"ffscrnsave"

inline WORD get_lang_id(void)
{
    return PRIMARYLANGID(LANGIDFROMLCID(GetThreadLocale()));
}

// localization
LPCTSTR get_text(INT id)
{
#ifdef JAPAN
    if (get_lang_id() == LANG_JAPANESE) // Japone for Japone
    {
        switch (id)
        {
        case 0: return TEXT("ffscrnsave バージョン 1.2 by 片山博文MZ");
        case 1:
            return TEXT("使用方法: ffscrnsave [オプション] your_file.scr\n")
                   TEXT("オプション:\n")
                   TEXT("  -i INPUT.scr            入力ファイルを指定します。\n")
                   TEXT("  -x WIDTH                表示される幅を設定します。\n")
                   TEXT("  -y HEIGHT               表示される高さを設定します。\n")
                   TEXT("  -left LEFT              ウィンドウの左端の位置を指定します。\n")
                   TEXT("                          (デフォルトでは中央に配置されます)\n")
                   TEXT("  -top TOP                ウィンドウの上端の位置を指定します。\n")
                   TEXT("                          (デフォルトでは中央に配置されます)\n")
                   TEXT("  -fs                     全画面モードで開始します。\n")
                   TEXT("  -noborder               枠のないウィンドウを作成します。\n")
                   TEXT("  -window_title TITLE     ウィンドウのタイトルを設定します。\n")
                   TEXT("                          (デフォルトでは入力ファイル名が使用されます)\n")
                   TEXT("  -help                   このヘルプメッセージを表示します。\n")
                   TEXT("  -version                バージョン情報を表示します。");
        case 2: return TEXT("エラー: ファイル名が未指定です。\n");
        case 3: return TEXT("エラー: クラスの登録に失敗しました。\n");
        case 4: return TEXT("エラー: メインウィンドウの作成に失敗しました。\n");
        case 5: return TEXT("エラー: オプション -x は引数が必要です。\n");
        case 6: return TEXT("エラー: オプション -y は引数が必要です。\n");
        case 7: return TEXT("エラー: オプション -left は引数が必要です。\n");
        case 8: return TEXT("エラー: オプション -top は引数が必要です。\n");
        case 9: return TEXT("エラー: オプション -window_title は引数が必要です。\n");
        case 10: return TEXT("エラー: 引数が多すぎます。\n");
        case 11: return TEXT("エラー: オプション -i は引数が必要です。\n");
        case 12: return TEXT("エラー: '%s' は不正な引数です。\n");
        }
    }
    else // The others are Let's la English
#endif
    {
        switch (id)
        {
        case 0: return TEXT("ffscrnsave version 1.2 by katahiromz");
        case 1:
            return TEXT("Usage: ffscrnsave [Options] your_file.scr\n")
                   TEXT("\n")
                   TEXT("Options:\n")
                   TEXT("  -i INPUT.scr          Specify the input file.\n")
                   TEXT("  -x WIDTH              Set the displayed width.\n")
                   TEXT("  -y HEIGHT             Set the displayed height.\n")
                   TEXT("  -left LEFT            Specify the x position of the window's left edge\n")
                   TEXT("                        (default is centered).\n")
                   TEXT("  -top TOP              Specify the y position of the window's top edge\n")
                   TEXT("                        (default is centered).\n")
                   TEXT("  -fs                   Start in fullscreen mode.\n")
                   TEXT("  -noborder             Create a borderless window.\n")
                   TEXT("  -window_title TITLE   Set the window title (default is the input\n")
                   TEXT("                        filename).\n")
                   TEXT("  -help                 Display this help message.\n")
                   TEXT("  -version              Display version information.");
        case 2: return TEXT("ERROR: No filename specified.\n");
        case 3: return TEXT("ERROR: Failed to register classes.\n");
        case 4: return TEXT("ERROR: Failed to create the main window.\n");
        case 5: return TEXT("ERROR: Option -x needs an operand.\n");
        case 6: return TEXT("ERROR: Option -y needs an operand.\n");
        case 7: return TEXT("ERROR: Option -left needs an operand.\n");
        case 8: return TEXT("ERROR: Option -top needs an operand.\n");
        case 9: return TEXT("ERROR: Option -window_title needs an operand.\n");
        case 10: return TEXT("ERROR: Too many arguments.\n");
        case 11: return TEXT("ERROR: Option -i needs an operand.\n");
        case 12: return TEXT("ERROR: '%s' is invalid argument.\n");
        }
    }

    assert(0);
    return nullptr;
}

HWND g_hMainWnd = NULL;

void version(void)
{
    _putts(get_text(0));
}

void usage(void)
{
    _putts(get_text(1));
}

BOOL startSaver(HWND hwnd, LPCWSTR filename)
{
    std::wstring fname = filename;
    if (GetFileAttributesW(fname.c_str()) == INVALID_FILE_ATTRIBUTES)
        fname += L".scr";

    WCHAR szBuf[MAX_PATH * 2];
    StringCchPrintfW(szBuf, _countof(szBuf), L"\"%s\" /p %u", fname.c_str(), (UINT)(UINT_PTR)hwnd);

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

void OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    SetCursor(LoadCursor(NULL, IDC_ARROW));
}

void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    HWND hChild = GetTopWindow(hwnd);
    if (hChild)
    {
        RECT rc;
        GetClientRect(hwnd, &rc);
        MoveWindow(hChild, 0, 0, rc.right, rc.bottom, TRUE);
        PostMessage(hChild, WM_DISPLAYCHANGE, 32, MAKELONG(rc.right, rc.bottom));
    }
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
        HANDLE_MSG(hwnd, WM_MOUSEMOVE, OnMouseMove);
        HANDLE_MSG(hwnd, WM_KEYDOWN, OnKey);
        HANDLE_MSG(hwnd, WM_KEYUP, OnKey);
        HANDLE_MSG(hwnd, WM_SIZE, OnSize);
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
            style = WS_POPUPWINDOW;
        else
            style = WS_OVERLAPPEDWINDOW;

        RECT rcWork = { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
        SIZE sizWork = { (rcWork.right - rcWork.left), (rcWork.bottom - rcWork.top) };

        if (x <= 0)
            x = CW_USEDEFAULT;
        if (y <= 0)
            y = CW_USEDEFAULT;

        if (fullscreen)
        {
            left = rcWork.left;
            top = rcWork.top;
            x = sizWork.cx;
            y = sizWork.cy;
        }
        else
        {
            if (x != CW_USEDEFAULT && y == CW_USEDEFAULT)
            {
                y = x * 600 / 800;
            }
            else if (x == CW_USEDEFAULT && y != CW_USEDEFAULT)
            {
                x = y * 800 / 600;
            }
            else if (x == CW_USEDEFAULT && y == CW_USEDEFAULT)
            {
                x = sizWork.cx * 2 / 3;
                y = sizWork.cy * 2 / 3;
            }

            if (left == CW_USEDEFAULT)
                left = (sizWork.cx - x) / 2;
            if (top == CW_USEDEFAULT)
                top  = (sizWork.cy - y) / 2;
        }

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

    int run(void)
    {
        InitCommonControls();

        if (filename.empty())
        {
            _ftprintf(stderr, get_text(2));
            return -1;
        }

        WCHAR szFileName[MAX_PATH];
        LPWSTR pchFilePart;
        GetFullPathNameW(filename.c_str(), _countof(szFileName), szFileName, &pchFilePart);

        if (window_title.empty())
            window_title = pchFilePart;

        if (!registerClasses())
        {
            _ftprintf(stderr, get_text(3));
            return -2;
        }

        if (!createMainWnd())
        {
            _ftprintf(stderr, get_text(4));
            return -3;
        }

        FreeConsole();

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
                _ftprintf(stderr, get_text(5));
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
                _ftprintf(stderr, get_text(6));
                return -1;
            }
        }

        if (lstrcmpiW(arg, L"-left") == 0 || lstrcmpiW(arg, L"--left") == 0)
        {
            arg = argv[++iarg];
            if (iarg < argc)
            {
                ffscrnsave.left = (INT)wcstol(arg, NULL, 0);
                continue;
            }
            else
            {
                _ftprintf(stderr, get_text(7));
                return -1;
            }
        }

        if (lstrcmpiW(arg, L"-top") == 0 || lstrcmpiW(arg, L"--top") == 0)
        {
            arg = argv[++iarg];
            if (iarg < argc)
            {
                ffscrnsave.top = (INT)wcstol(arg, NULL, 0);
                continue;
            }
            else
            {
                _ftprintf(stderr, get_text(8));
                return -1;
            }
        }

        if (lstrcmpiW(arg, L"-window_title") == 0 || lstrcmpiW(arg, L"--window_title") == 0)
        {
            arg = argv[++iarg];
            if (iarg < argc)
            {
                ffscrnsave.window_title = arg;
                continue;
            }
            else
            {
                _ftprintf(stderr, get_text(9));
                return -1;
            }
        }

        if (lstrcmpiW(arg, L"-i") == 0)
        {
            arg = argv[++iarg];
            if (iarg < argc)
            {
                if (ffscrnsave.filename.empty())
                {
                    ffscrnsave.filename = arg;
                    continue;
                }
                _ftprintf(stderr, get_text(10));
                return -1;
            }
            else
            {
                _ftprintf(stderr, get_text(11));
                return -1;
            }
        }

        if (lstrcmpiW(arg, L"-fs") == 0 || lstrcmpiW(arg, L"--fs") == 0)
        {
            ffscrnsave.fullscreen = true;
            continue;
        }

        if (lstrcmpiW(arg, L"-noborder") == 0 || lstrcmpiW(arg, L"--noborder") == 0)
        {
            ffscrnsave.noborder = true;
            continue;
        }

        if (arg[0] == L'-')
        {
            _ftprintf(stderr, get_text(12), arg);
            return -1;
        }

        if (ffscrnsave.filename.empty())
        {
            ffscrnsave.filename = arg;
            continue;
        }
        else
        {
            _ftprintf(stderr, get_text(10));
            return -1;
        }
    }

    return ffscrnsave.run();
}

#include <clocale>

int main(void)
{
    // Unicode console output support
    std::setlocale(LC_ALL, "");

    INT myargc;
    LPWSTR *myargv = CommandLineToArgvW(GetCommandLineW(), &myargc);
    INT ret = wmain(myargc, myargv);
    LocalFree(myargv);

    // Detect handle leaks (for Debug)
#if (_WIN32_WINNT >= 0x0500) && !defined(NDEBUG)
    TCHAR szText[MAX_PATH];
    wnsprintf(szText, _countof(szText), TEXT("GDI Objects: %ld, User Objects: %ld\n"),
              GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS),
              GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS));
    OutputDebugString(szText);
#endif

    // Detect memory leaks (for Debug and MSVC)
#if defined(_MSC_VER) && !defined(NDEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    return ret;
}
