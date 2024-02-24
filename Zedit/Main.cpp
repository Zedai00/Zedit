#ifndef UNICODE
#define UNICODE
#endif 

#include "MenuDef.h"
#include "FuncDef.h"
#include "Includes.h"
#include "Globals.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Zedit";
	Register(hInstance, CLASS_NAME);
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
	SetFont(hWnd);
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
	case WM_SIZE:
		RedrawText();
		return 0;
	case WM_COMMAND:
		switch (wParam) {
		case FILE_MENU_EXIT:
			DestroyWindow(hWnd);
			break;
		case FILE_MENU_SAVE:
			SaveFile();
			break;
		case FILE_MENU_SAVE_AS:
			openedFile = false;
			SaveFile();
			break;
		case FILE_MENU_OPEN:
			OpenFile();
			break;
		case FILE_MENU_NEW:
			ClearText();
			break;
		}
		return 0;
	case WM_DESTROY:
		DeleteObject(hFont);
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

void RegisterClass(HINSTANCE hInstance, LPCWSTR CLASS_NAME) {
	WNDCLASS wc = { };
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	RegisterClass(&wc);
}

LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYUP:
		// Check if Ctrl key is pressed along with 'S' key
		if ((GetKeyState(VK_CONTROL) & 0x8000) && (wParam == 'S' || wParam == 's'))
		{
			// Handle Ctrl+S (save) event
			SaveFile(); // Assuming you have a function for saving
			return 0; // We handled the message
		}
		break;

	default:
		// Call the original window procedure for other messages
		return CallWindowProc(OldEditProc, hWnd, uMsg, wParam, lParam);
	}

	// If the message was not handled, call the original window procedure
	return CallWindowProc(OldEditProc, hWnd, uMsg, wParam, lParam);
}

void SubclassEditControl(HWND hEdit)
{
	// Store the original window procedure of the edit control
	OldEditProc = (WNDPROC)SetWindowLongPtr(hEdit, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
}

void RedrawText() {
	RECT rect;
	GetClientRect(hWnd, &rect); // Get the client area dimensions

	// Resize the existing text editor control to fit the new client area
	SetWindowPos(hEdit, NULL, 0, 0, rect.right, rect.bottom, SWP_NOMOVE | SWP_NOZORDER);
}

void SetFont(HWND wnd) {
	hFont = CreateFont(
		16,                 // Height of the font
		0,                  // Width of the font
		0,                  // Angle of escapement
		0,                  // Orientation angle
		FW_NORMAL,          // Font weight
		FALSE,              // Italic
		FALSE,              // Underline
		FALSE,              // Strikeout
		DEFAULT_CHARSET,    // Character set
		OUT_DEFAULT_PRECIS, // Output precision
		CLIP_DEFAULT_PRECIS,// Clipping precision
		DEFAULT_QUALITY,    // Output quality
		DEFAULT_PITCH,      // Pitch and family
		L"Arial"            // Font name
	);

	// Set the font for the edit control
	SendMessage(wnd, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void PrepareFileDialog() {
	wchar_t file_name[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = file_name;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 100;
	ofn.lpstrFilter = L"All Files\0*.*\0";
	ofn.nFilterIndex = 1;
}

void OpenFile() {
	CreateFile();
	GetOpenFileName(&ofn);
	wcscpy_s(global_file_name, ofn.lpstrFile);
	SetFileTitle();
	DisplayFile(global_file_name);
	openedFile = true;
}

void SaveFile() {
	CreateFile();
	if (openedFile) {
		WriteFile(global_file_name);
	}
	else {
		GetSaveFileName(&ofn);
		wcscpy_s(global_file_name, ofn.lpstrFile);
		SetFileTitle();
		WriteFile(ofn.lpstrFile);
		openedFile = true;
	}
}

void SetFileTitle() {
	wchar_t fileName[MAX_PATH];
	wcscpy_s(fileName, global_file_name);
	PathStripPath(fileName);
	SetWindowText(hWnd, fileName);
}

void WriteFile(LPWSTR path) {
	std::wofstream file(path);
	int length = GetWindowTextLength(hEdit);
	wchar_t* buffer = new wchar_t[length + 1];
	GetWindowText(hEdit, buffer, length + 1);
	file << buffer;
	delete[] buffer;
	file.close();
}

void DisplayFile(LPWSTR path) {
	std::wifstream file(path);
	std::wstring content((std::istreambuf_iterator<wchar_t>(file)), std::istreambuf_iterator<wchar_t>());
	SetWindowText(hEdit, content.c_str());
}

void ClearText() {
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
	AppendMenu(hFileMenu, MF_STRING, FILE_MENU_SAVE_AS, L"Save As");
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
	SetFont(hEdit);
	SubclassEditControl(hEdit);

}
