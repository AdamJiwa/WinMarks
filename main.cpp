// #include "stdafx.h"
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

static HWND windowMap[38];
// TODO: make this an enum
static int state = 0;

void registerPossibleHotkeyCharacters();
void deregisterPossibleHotkeyCharacters();

int _cdecl _tmain (
    int argc, 
    TCHAR *argv[])
{
    if (RegisterHotKey(
        NULL,
        1,
        MOD_CONTROL | MOD_NOREPEAT,
        0x4D))
    {
        _tprintf(_T("Createion Hotkey \"CTRL+M\" registered\n"));
    }
 
    if (RegisterHotKey(
        NULL,
        39,
        MOD_CONTROL | MOD_NOREPEAT,
        0xDE)) // 0xDE
    {
        _tprintf(_T("Activation Hotkey \"CTRL+'\" registered\n"));
    } else {
        _tprintf(_T("Activation Hotkey failed to assign at \"CTRL+'\"\n"));
    }

    MSG msg = {0};
    HWND hwndThis = {0};
    while (GetMessage(&msg, hwndThis, 0, 0) != 0)
    {
        if (msg.message == WM_HOTKEY)
        {
            switch((int)msg.wParam) {
                case 1:
                    printf("Entered create state\n");
                    state = 1;
                    registerPossibleHotkeyCharacters();
                    break;
                case 39:
                    printf("Entered call state\n");
                    state = 2;
                    registerPossibleHotkeyCharacters();
                    break;
                default:
                    switch(state) {
                        case 1: {
                            _tprintf(_T("assign current window to %d\n"), (int)msg.wParam);

                            HWND caller = GetForegroundWindow();
                            if (NULL != caller) {
                                int cTxtLen = GetWindowTextLength(caller);
                                char * pszMem = (PSTR) VirtualAlloc((LPVOID) NULL, (DWORD) (cTxtLen + 1), MEM_COMMIT, PAGE_READWRITE);
                                GetWindowText(caller, pszMem, cTxtLen + 1);
                                _tprintf(_T("Got window title %s of length %d\n"), pszMem, cTxtLen);
                                windowMap[(int)msg.wParam] = caller;
                            } else {
                                _tprintf(_T("got no active window?\n"));
                            }
                            deregisterPossibleHotkeyCharacters();
                            state = 0;
                            break;
                        }
                        case 2: {
                            _tprintf(_T("activate window for key %d\n"), (int)msg.wParam);
                            HWND target = windowMap[(int)msg.wParam];
                            // do focus
                            if (NULL != target) {
                                ShowWindow(target, 5);
                                SetForegroundWindow(target);
                            } else {
                                printf("target is null???");
                            }
                            deregisterPossibleHotkeyCharacters();
                            state = 0;
                            break;
                        }
                        default:
                            break;
                    }
                break;
            }
        }
    } 
 
    return 0;
}

void registerPossibleHotkeyCharacters() {

    printf("Registering possible assign keys\n");

    // Do number keys
    UINT key = 0x30; // 0 key

    int keyIdOffset = 2;

    for (int i=0; i<10; i++) {
        if(!RegisterHotKey(
            NULL,
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
            NULL,
            keyIdOffset+i,
            MOD_NOREPEAT,
            key+i))
        {
            _tprintf(_T("error registering 0x%02x hotkey\n"), key+i);
        }
    }
}


void deregisterPossibleHotkeyCharacters() {

    printf("Deregistering possible assign keys\n");
    // Do number keys
    UINT key = 0x30; // 0 key

    int keyIdOffset = 2;

    for (int i=0; i<10; i++) {
        if(UnregisterHotKey(
            NULL,
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
            NULL,
            keyIdOffset+i))
        {
            // _tprintf(_T("unregistered 0x%02x hotkey\n"), key+i);
        } else {
            _tprintf(_T("failed to unregister 0x%02x hotkey\n"), key+i);
        }
    }
}
