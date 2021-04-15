#include "stdafx.h" 
#include "NotifyIcon.h"
#include <string>
#include "resource.h"

#define WM_ICON_NOTIFY WM_APP+10

static HWND windowMap[38];
// TODO: make this an enum
static int state = 0;


HINSTANCE hInst;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void registerPossibleHotkeyCharacters(HWND hWnd);
void deregisterPossibleHotkeyCharacters(HWND hWnd);


int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    MSG msg;
    HACCEL hAccelTable;

    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return false;
    }

    hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)_T("WinMarks"));

    ShowInfo(NULL, "WinMarks has started", true);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    
    return msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = NULL; //LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTDIALOGAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL; //MAKEINTRESOURCEW(IDC_TESTDIALOGAPP);
    wcex.lpszClassName  = (LPCWSTR)_T("WinMarks");
    wcex.hIconSm        = NULL; //LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;

    hInst = hInstance;

    // TODO: is the window class name nullable?
    hWnd = CreateWindowW((LPCWSTR)"WinMarks", (LPCWSTR)"WinMarks", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        printf("failed to create window handle\n");
        return false;
    }

    /* if (!TrayIcon.Create(hInstance,
            hWnd,
            WM_ICON_NOTIFY,
            _T("WinMarks"),
            ::LoadIcon(hInstance, (LPCTSTR)IDI_ICON1),
            IDC_TRAY_MENU))
    {*/
    if (!initializeIcon(hWnd,
            ::LoadIcon(hInstance, (LPCTSTR)IDI_ICON1),
            WM_ICON_NOTIFY,
            _T("WinMarks"),
            IDC_TRAY_MENU))
    {
        printf("Tray Icon failed to create\n");
        return false;
    }
    
    if (RegisterHotKey(
         hWnd,
         1,
         MOD_CONTROL | MOD_NOREPEAT,
         0x4D))
    {
        _tprintf(_T("Createion Hotkey \"CTRL+M\" registered\n"));
    } else {
        return false;
    }
  
     if (RegisterHotKey(
         hWnd,
         39,
         MOD_CONTROL | MOD_NOREPEAT,
         0xDE)) // 0xDE
     {
         _tprintf(_T("Activation Hotkey \"CTRL+'\" registered\n"));
     } else {
         _tprintf(_T("Activation Hotkey failed to assign at \"CTRL+'\"\n"));
         return false;
     }

    return true;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_ICON_NOTIFY:
            return NotificationCallback(hInst, wParam, lParam);

        case WM_COMMAND:
            wmId = LOWORD(wParam);
            wmEvent = HIWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_EXIT:
                    DestroyWindow(hWnd);
                    break;
                default:
                    return DefWindowProc(hWnd, message, wParam, lParam);
            }
            break;
        case WM_HOTKEY:
         {
             switch((int)wParam) {
                 case 1:
                     printf("Entered create state\n");
                     state = 1;
                     registerPossibleHotkeyCharacters(hWnd);
                     break;
                 case 39:
                     printf("Entered call state\n");
                     state = 2;
                     registerPossibleHotkeyCharacters(hWnd);
                     break;
                 default:
                     switch(state) {
                         case 1: {
                             _tprintf(_T("assign current window to %d\n"), (int)wParam);
 
                             HWND caller = GetForegroundWindow();
                             if (NULL != caller) {
                                 int cTxtLen = GetWindowTextLength(caller);
                                 char * pszMem = (PSTR) VirtualAlloc((LPVOID) NULL, (DWORD) (cTxtLen + 1), MEM_COMMIT, PAGE_READWRITE);
                                 GetWindowText(caller, pszMem, cTxtLen + 1);
                                 _tprintf(_T("Got window title %s of length %d\n"), pszMem, cTxtLen);
                                 windowMap[(int)wParam] = caller;
                             } else {
                                 _tprintf(_T("got no active window?\n"));
                             }
                             deregisterPossibleHotkeyCharacters(hWnd);
                             state = 0;
                             break;
                         }
                         case 2: {
                             _tprintf(_T("activate window for key %d\n"), (int)wParam);
                             HWND target = windowMap[(int)wParam];
                             // do focus
                             if (NULL != target) {
                                 if (IsIconic(target)) {
                                     ShowWindow(target, 9);
                                 } else {
                                     ShowWindow(target, 5);
                                 }
                                 SetForegroundWindow(target);
                             } else {
                                 printf("target is null???");
                             }
                             deregisterPossibleHotkeyCharacters(hWnd);
                             state = 0;
                             break;
                         }
                         default:
                             break;
                     }
                 break;
             }
             break;
         }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void registerPossibleHotkeyCharacters(HWND hWnd) {

    printf("Registering possible assign keys\n");

    // Do number keys
    UINT key = 0x30; // 0 key

    int keyIdOffset = 2;

    for (int i=0; i<10; i++) {
        if(!RegisterHotKey(
            hWnd,
            keyIdOffset+i,
            MOD_NOREPEAT,
            key+i))
        {
            _tprintf(_T("error registering 0x%02x hotkey\n"), key+i);
        }
    }

    // Do Character keys
    key = 0x41; // a key

    keyIdOffset = 12;

    for (int i=0; i<27; i++) {
        if(!RegisterHotKey(
            hWnd,
            keyIdOffset+i,
            MOD_NOREPEAT,
            key+i))
        {
            _tprintf(_T("error registering 0x%02x hotkey\n"), key+i);
        }
    }
}


void deregisterPossibleHotkeyCharacters(HWND hWnd) {

    printf("Deregistering possible assign keys\n");
    // Do number keys
    UINT key = 0x30; // 0 key

    int keyIdOffset = 2;

    for (int i=0; i<10; i++) {
        if(UnregisterHotKey(
            hWnd,
            keyIdOffset+i))
        {
            // _tprintf(_T("unregistered 0x%02x hotkey\n"), key+i);
        } else {
            _tprintf(_T("failed to unregister 0x%02x hotkey\n"), key+i);
        }
    }

    // Do Character keys
    key = 0x41; // a key

    keyIdOffset = 12;

    for (int i=0; i<27; i++) {
        if(UnregisterHotKey(
            hWnd,
            keyIdOffset+i))
        {
            // _tprintf(_T("unregistered 0x%02x hotkey\n"), key+i);
        } else {
            _tprintf(_T("failed to unregister 0x%02x hotkey\n"), key+i);
        }
    }
}
