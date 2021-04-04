#include "stdafx.h"
#include "NotifyIcon.h"

NOTIFYICONDATA _lpData;

bool initializeIcon(HWND hWnd, HICON icon, UINT uCallbackMessage, LPCTSTR szToolTip, UINT uID) {
    memset(&_lpData, 0, sizeof(_lpData));

    // TODO: Seems weird
    _lpData.cbSize = sizeof(NOTIFYICONDATA);
    _lpData.hWnd = hWnd;
    // Seems like an abuse of this attribute
    _lpData.uID = uID;
    _lpData.hIcon = icon;
    _lpData.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    _lpData.uCallbackMessage = uCallbackMessage;
    strncpy(_lpData.szTip, szToolTip, sizeof(szToolTip));

    if (!Shell_NotifyIcon(NIM_ADD, &_lpData)) {
        return false;
    }
    return true;
}


void ShowInfo(LPCTSTR title, LPCTSTR text, bool quiet) {
    ShowNifInfo(title, text, NIIF_INFO, quiet);
}

void ShowWarning(LPCTSTR title, LPCTSTR text) {
    ShowNifInfo(title, text, NIIF_WARNING, false);
}

void ShowError(LPCTSTR title, LPCTSTR text) {
    ShowNifInfo(title, text, NIIF_ERROR, false);
}

void ShowNifInfo(LPCTSTR title, LPCTSTR text, UINT icon, bool quiet)
{
    _lpData.uFlags = NIF_INFO; 
    _lpData.dwInfoFlags = icon;
    if (quiet) {
        _lpData.dwInfoFlags = _lpData.dwInfoFlags | NIIF_NOSOUND;
    }
    if (title) {
        strncpy(_lpData.szInfoTitle, title, 64);
    }
    strncpy(_lpData.szInfo, text, 256);

    Shell_NotifyIcon(NIM_MODIFY, &_lpData);
}

// TODO: why does the notification need to return LRESULT
LRESULT NotificationCallback(HINSTANCE hInstance, WPARAM wParam, LPARAM lParam) {
    if (wParam != _lpData.uID) return 0L;

    if (LOWORD(lParam) == WM_RBUTTONUP) {
        HMENU hMenu = ::LoadMenu(hInstance, MAKEINTRESOURCE(_lpData.uID));
        if (!hMenu)
            ShowError(NULL, _T("Failed to load menu resource"));

        HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
        if (!hSubMenu)
        {
            ::DestroyMenu(hMenu);
            ShowError(NULL, _T("Failed to load sub menu resource"));
            return 0;
        }

        POINT pos;
        GetCursorPos(&pos);

        ::SetForegroundWindow(_lpData.hWnd);
        ::TrackPopupMenu(hSubMenu, 0, pos.x, pos.y, 0, _lpData.hWnd, NULL);

        DestroyMenu(hMenu);
    } else if (LOWORD(lParam) == WM_LBUTTONDBLCLK) {

        // TODO: settings window
    }

    return 1;
}
