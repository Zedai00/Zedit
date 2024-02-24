#pragma once
#include <windows.h>

HWND hWnd;
HWND hEdit;
HFONT hFont;
WNDPROC OldEditProc;
bool openedFile = false;
wchar_t global_file_name[MAX_PATH] = L"";
OPENFILENAME ofn;
