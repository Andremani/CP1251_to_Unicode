#pragma once

ATOM                MainWindowRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void Draw(HWND hWnd, HDC hdc);
void ConvertCP1251CodeToText(HWND hWnd);
void ConvertTextToCP1251Code(HWND hWnd);

#define MAX_LOADSTRING 100
#define ID_CONVERT_TO_CP1251_BUTTON 200 // Control IDs for UI elements
#define ID_CONVERT_TO_UNICODE_BUTTON 201
#define ID_UNICODE_TEXT_EDIT 202
#define ID_CP1251_CODE_EDIT 203
#define COLOR_BACKGROUND RGB(117, 39, 39)
