#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include "MenuDef.h"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <algorithm>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void AddMenus();
void AddTextEditor();
void openFile(int);
void display_file(LPWSTR path);
void write_file(LPWSTR path);
void clear_text();

HWND hWnd;
HWND hEdit;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Zedit";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	hWnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Zedit",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hWnd == NULL)
	{
		return 0;
	}
	AddMenus();
	AddTextEditor();
	ShowWindow(hWnd, nCmdShow);

	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (wParam) {
		case FILE_MENU_EXIT:
			DestroyWindow(hWnd);
			break;
		case FILE_MENU_SAVE:
			openFile(0);
			break;
		case FILE_MENU_OPEN:
			openFile(1);
			break;
		case FILE_MENU_NEW:
			clear_text();
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void openFile(int choice) {
	OPENFILENAME ofn;
	wchar_t file_name[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = file_name;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 100;
	ofn.lpstrFilter = L"All Files\0*.*\0";
	ofn.nFilterIndex = 1;

	if (choice == 0) {
		GetSaveFileName(&ofn);
		write_file(ofn.lpstrFile);
	}
	else if (choice == 1) {
		GetOpenFileName(&ofn);
		display_file(ofn.lpstrFile);
	}
	else {

	}
}

void write_file(LPWSTR path) {
	std::wofstream file(path);
	int length = GetWindowTextLength(hEdit);
	wchar_t* buffer = new wchar_t[length + 1];
	GetWindowText(hEdit, buffer, length + 1);
	file << buffer;
	delete[] buffer;
	file.close();
}

void display_file(LPWSTR path) {
	std::wifstream file(path);
	std::wstring content((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>());
	SetWindowText(hEdit, content.c_str());
}

void clear_text() {
	SetWindowText(hEdit, NULL);
}

void AddMenus() {
	HMENU hMenu = CreateMenu();

	HMENU hFileMenu = CreateMenu();
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_NEW, L"New");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_OPEN, L"Open");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_SAVE, L"Save");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_SAVE, L"Save As");
	AppendMenu(hFileMenu, MF_SEPARATOR, NULL, NULL);
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_EXIT, L"Exit");
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, L"File");
	AppendMenu(hMenu, MF_STRING, 2, L"Help");
	SetMenu(hWnd, hMenu);
}

void GetWindowDimensions(int& width, int& height) {
	RECT rect;
	GetWindowRect(hWnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
}

void AddTextEditor() {
	int width, height;
	GetWindowDimensions(width, height);
	hEdit = CreateWindow(L"Edit", NULL, WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE | WS_CHILD, CW_USEDEFAULT, CW_USEDEFAULT, width, height, hWnd, NULL, NULL, NULL);
}

