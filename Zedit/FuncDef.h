#pragma once
#include <Windows.h>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void AddMenus();
void AddTextEditor();
void PrepareFileDialog();
void OpenFile();
void SaveFile();
void DisplayFile(LPWSTR path);
void WriteFile(LPWSTR path);
void ClearText();
void SetFileTitle();
void RedrawText();
void SetFont(HWND wnd);
void Register(HINSTANCE, LPCWSTR);
