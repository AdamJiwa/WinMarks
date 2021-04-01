#include  <ShellAPI.h>

bool initializeIcon(HWND hWnd, HICON icon, UINT uCallbackMessage, LPCTSTR szToolTip, UINT uID);
void ShowInfo(LPCTSTR title, LPCTSTR text, bool quiet);
void ShowWarning(LPCTSTR title, LPCTSTR text);
void ShowError(LPCTSTR title, LPCTSTR text);
void ShowNifInfo(LPCTSTR title, LPCTSTR text, UINT icon, bool quiet);
LRESULT NotificationCallback(HINSTANCE hInstance, WPARAM wParam, LPARAM lParam);
