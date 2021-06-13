#include "stdafx.h" 
#include "NotifyIcon.h"
#include <string>
#include <stdexcept>
#include <iostream>
#include "resource.h"

#define WM_ICON_NOTIFY WM_APP+10

#define CONTROL_CREATE_CALLBACK 1
#define CONTROL_ACTIVATE_CALLBACK 2
#define MAPPING_CALLBACK_OFFSET 100
#define MAPPING_RESERVED_CALLBACK 200

const int CONTROL_MODIFIER = MOD_CONTROL;
const UINT CONTROL_CREATE_STATE_KEY = 0x4D;
const UINT CONTROL_ACTIVATE_STATE_KEY = 0xDE;

static HWND windowMap[38];
static HWND lastWindow; // store the last window we activated with winMarks

static enum CONTROL_STATE {
    NORMAL = 0,
    CREATE = 1,
    ACTIVATE = 2
};

static CONTROL_STATE state = NORMAL;


HINSTANCE hInst;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// TODO: reformat file, the formatting went to shit for some reason

std::string GetLastErrorAsString();
void handleCreateAction(HWND hWnd, WPARAM wParam);
void handleActivateAction(HWND hWnd, WPARAM wParam);
void registerControlHotkeys(HWND hWnd);
void deregisterControlHotkeys(HWND hWnd);
void registerPossibleHotkeyCharacters(HWND hWnd);
void deregisterPossibleHotkeyCharacters(HWND hWnd);
void registerReservedActionHotkeys(HWND hWnd);
void deregisterReservedActionHotkeys(HWND hWnd);
void popWindow(HWND target);


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

    if (!initializeIcon(hWnd,
            ::LoadIcon(hInstance, (LPCTSTR)IDI_ICON1),
            WM_ICON_NOTIFY,
            _T("WinMarks"),
            IDC_TRAY_MENU))
    {
        printf("Tray Icon failed to create\n");
        return false;
    }
    
    registerControlHotkeys(hWnd);

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
                 case CONTROL_CREATE_CALLBACK:
                     printf("Entered create state\n");
                     state = CREATE;
                     deregisterControlHotkeys(hWnd);
                     registerPossibleHotkeyCharacters(hWnd);
                     break;
                 case CONTROL_ACTIVATE_CALLBACK:
                     printf("Entered call state\n");
                     state = ACTIVATE;
                     deregisterControlHotkeys(hWnd);
                     registerPossibleHotkeyCharacters(hWnd);
                     registerReservedActionHotkeys(hWnd);
                     break;
                 default:
                     switch(state) {
                         case CREATE: {
                             handleCreateAction(hWnd, wParam);
                             break;
                         }
                         case ACTIVATE: {
                             handleActivateAction(hWnd, wParam);
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
            // Pretty sure this is auto cleaned up but yolo
            deregisterControlHotkeys(hWnd);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void handleCreateAction(HWND hWnd, WPARAM wParam) {
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
     registerControlHotkeys(hWnd);
     state = NORMAL;
}

void handleActivateAction(HWND hWnd, WPARAM wParam) {
    int callbackId = static_cast<int>(wParam);
     _tprintf(_T("activate window for key %d\n"), callbackId);
     if (callbackId >= MAPPING_CALLBACK_OFFSET && callbackId < MAPPING_RESERVED_CALLBACK) {
         HWND target = windowMap[(int)wParam];
         // do focus
         popWindow(target);
     } else if (callbackId >= MAPPING_RESERVED_CALLBACK) {
        // TODO: we only have one of these atm update the logic later
        popWindow(lastWindow);
     }
     deregisterPossibleHotkeyCharacters(hWnd);
     deregisterReservedActionHotkeys(hWnd);
     registerControlHotkeys(hWnd);
     state = NORMAL;
}

void popWindow(HWND target) {
     if (NULL != target) {

         HWND currentWindow = GetForegroundWindow();

         // If the window is minimized we need to restore
        if (IsIconic(target)) {
             ShowWindow(target, SW_RESTORE);

         // If window is already active lets minimize it
         } else if (currentWindow == target) {
             ShowWindow(target, SW_MINIMIZE);
             // Pop our last managed window instead of z based
             target = lastWindow;

         // show the window
         } else {
             ShowWindow(target, SW_SHOW);
         }

         SetForegroundWindow(target);
         lastWindow = currentWindow;
     } else {
         // TODO: notify error or log it somewhere for fixing later
         ShowInfo(NULL, "No window assigned!", true);
     }
}

// Register the hotkeys that are used to enter states
void registerControlHotkeys(HWND hWnd) {
    if (!RegisterHotKey(hWnd,
         CONTROL_CREATE_CALLBACK,
         CONTROL_MODIFIER | MOD_NOREPEAT,
         CONTROL_CREATE_STATE_KEY)) {
         ShowInfo(NULL, "Control hotkey failed to assign at \"CTRL+M\"\n", true);
         throw std::runtime_error("Failed to assign control hotkey");
    }

     if (!RegisterHotKey(
         hWnd,
         CONTROL_ACTIVATE_CALLBACK,
         CONTROL_MODIFIER | MOD_NOREPEAT,
         CONTROL_ACTIVATE_STATE_KEY))
     {
         ShowInfo(NULL, "Control hotkey failed to assign at \"CTRL+'\"\n", true);
         throw std::runtime_error("Failed to assign control hotkey");
     }
}

// deregister the hotkeys that are used in states
// used when entering a control state and cleanup
void deregisterControlHotkeys(HWND hWnd) {
    if(!UnregisterHotKey(
        hWnd,
        CONTROL_CREATE_CALLBACK)) { 
         throw std::runtime_error("Failed to unassign control hotkey");
    }

    if(!UnregisterHotKey(
        hWnd,
        CONTROL_ACTIVATE_CALLBACK)) {
         throw std::runtime_error("Failed to unassign control hotkey");
    }
}

std::string GetLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
        return std::string(); //No error message has been recorded
    }
    
    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    
    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);
    
    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);
            
    return message;
}

void registerPossibleHotkeyCharacters(HWND hWnd) {

    printf("Registering possible assign keys\n");

    // Do number keys
    UINT key = 0x30; // 0 key

    for (int i=0; i<10; i++) {
        if(!RegisterHotKey(
            hWnd,
            MAPPING_CALLBACK_OFFSET + i,
            MOD_NOREPEAT,
            key + i))
        {
            // ShowInfo(NULL, "Control hotkey failed to assign at \"CTRL+'\"\n", true);
            _tprintf(_T("error registering 0x%02x hotkey\n"), key+i);
        }
    }

    // Do Character keys
    key = 0x41; // a key
    int characterKeyOffset = 12;

    for (int i=0; i<27; i++) {
        if(!RegisterHotKey(
            hWnd,
            MAPPING_CALLBACK_OFFSET + characterKeyOffset + i,
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

    for (int i=0; i<10; i++) {
        if (UnregisterHotKey(
            hWnd,
            MAPPING_CALLBACK_OFFSET + i))
        {
            // _tprintf(_T("unregistered 0x%02x hotkey\n"), key+i);
        } else {
            _tprintf(_T("failed to unregister 0x%02x hotkey\n"), key+i);
        }
    }

    // Do Character keys
    key = 0x41; // a key
    int characterKeyOffset = 12;

    for (int i=0; i<27; i++) {
        if (!UnregisterHotKey(
            hWnd,
            MAPPING_CALLBACK_OFFSET + characterKeyOffset + i)) {
            _tprintf(_T("failed to unregister 0x%02x hotkey\n"), key+i);
        }
    }
}

void registerReservedActionHotkeys(HWND hWnd) {
    // TODO: update the last reserved callback thing etc
     if (!RegisterHotKey(
         hWnd,
         MAPPING_RESERVED_CALLBACK,
         MOD_NOREPEAT,
         0xDE)) {
         _tprintf(_T("Activation Hotkey failed to assign at \"CTRL+'\"\n"));
     }
}

void deregisterReservedActionHotkeys(HWND hWnd) {
    if(!UnregisterHotKey(
        hWnd,
        MAPPING_RESERVED_CALLBACK)) {
        _tprintf(_T("failed to unregister 0x%02x hotkey\n"), 0xDE);
    }
}
