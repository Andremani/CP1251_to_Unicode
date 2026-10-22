#include "framework.h"
#include "resource.h"
#include "CP1251_to_Unicode.h"

#include <windowsx.h>
#include <regex>

// Global variables:
HINSTANCE processHandle;
WCHAR title[MAX_LOADSTRING];
WCHAR mainWindowClassName[MAX_LOADSTRING];

HWND buttonConvertToCP1251;
HWND buttonConvertToUnicode;
HWND editViewUnicode;
HWND editViewCP1251;

int windowPixelSizeX;
int windowPixelSizeY;

HFONT headersFont;
HFONT regularFont;

/// <summary>
/// Main function
/// </summary>
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Global strings initializing
    LoadStringW(hInstance, IDS_APP_TITLE, title, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CP1251TOUNICODE, mainWindowClassName, MAX_LOADSTRING);
    MainWindowRegisterClass(hInstance);

    // Application init
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CP1251TOUNICODE));

    MSG msg;

    // Main cycle
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

/// <summary>
/// Resisters window class
/// </summary>
ATOM MainWindowRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(COLOR_BACKGROUND); // (HBRUSH)(COLOR_WINDOW)     //+1 firstly RGB(117, 39, 39)
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CP1251TOUNICODE);
    wcex.lpszClassName  = mainWindowClassName;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   processHandle = hInstance;

   int xres = GetSystemMetrics(SM_CXSCREEN);
   int yres = GetSystemMetrics(SM_CYSCREEN);

   windowPixelSizeX = xres / 2;
   windowPixelSizeY = yres / 2;

   HWND hWnd = CreateWindowW(mainWindowClassName, title, WS_OVERLAPPEDWINDOW,
       xres/2 - windowPixelSizeX/2,
       yres/2 - windowPixelSizeY/2,
       windowPixelSizeX,
       windowPixelSizeY,
       nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

/// <summary>
/// Handles window events
/// - WM_CREATE   - Initialization, when creating UI
/// - WM_COMMAND  - Handle menu option
/// - WM_PAINT    - Draw window
/// - WM_DESTROY  - Destroy and return
/// </summary>
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        {
        headersFont = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Bahnschrift")); \
            if (!headersFont) { headersFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT); }

        regularFont = CreateFont(18, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Bahnschrift"));\
        if (!regularFont) {   regularFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);    }

        buttonConvertToCP1251 = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed (Button/Edit/ComboBox etc.)
            L"Преобразовать текст в код cp1251",      // Text on element (or title)
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
            windowPixelSizeX * 0.15,         // X position 
            windowPixelSizeY * 0.45,         // Y position 
            windowPixelSizeX * 0.3,        // Width
            windowPixelSizeY * 0.075,        // Height
            hWnd,     // Parent window handler
            (HMENU)ID_CONVERT_TO_CP1251_BUTTON,       // HMENU: Assign appropriate control ID
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), // Pointer to Parent Application
            NULL);

        buttonConvertToUnicode = CreateWindow(
            L"BUTTON",
            L"Преобразовать код cp1251 в текст",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            windowPixelSizeX * 0.85 - windowPixelSizeX * 0.3,
            windowPixelSizeY * 0.45,
            windowPixelSizeX * 0.3,
            windowPixelSizeY * 0.075,
            hWnd,
            (HMENU)ID_CONVERT_TO_UNICODE_BUTTON,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        editViewUnicode = CreateWindow(
            L"EDIT",
            L"",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | WS_VSCROLL | ES_MULTILINE,
            windowPixelSizeX * 0.05,
            windowPixelSizeY * 0.15,
            windowPixelSizeX * 0.9,
            windowPixelSizeY * 0.275,
            hWnd,
            (HMENU)ID_UNICODE_TEXT_EDIT,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        editViewCP1251 = CreateWindow(
            L"EDIT",
            L"",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER | ES_LEFT | WS_VSCROLL | ES_MULTILINE,
            windowPixelSizeX * 0.05,
            windowPixelSizeY * 0.55,
            windowPixelSizeX * 0.9,
            windowPixelSizeY * 0.275,
            hWnd,
            (HMENU)ID_CP1251_CODE_EDIT,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);

        SendMessage(buttonConvertToCP1251, WM_SETFONT, (WPARAM)regularFont, 0);
        SendMessage(buttonConvertToUnicode, WM_SETFONT, (WPARAM)regularFont, 0);
        SendMessage(editViewUnicode, WM_SETFONT, (WPARAM)regularFont, 0);
        SendMessage(editViewCP1251, WM_SETFONT, (WPARAM)regularFont, 0);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Button click handlers
            switch (wmId)
            {
            case ID_CONVERT_TO_CP1251_BUTTON:
                if (HIWORD(wParam) == BN_CLICKED)
                    ConvertTextToCP1251Code(hWnd);
                break;
            case ID_CONVERT_TO_UNICODE_BUTTON:
                if (HIWORD(wParam) == BN_CLICKED)
                    ConvertCP1251CodeToText(hWnd);
                break;
            // Menu options
            case IDM_ABOUT:
                DialogBox(processHandle, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            Draw(hWnd, hdc);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        DeleteObject(regularFont);
        DeleteObject(headersFont);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

/// <summary>
/// About program handler
/// </summary>
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void Draw(HWND hWnd, HDC hdc)
{
    LPCWSTR head = L"Кириллица <—> Код cp1251";

    SetTextAlign(hdc, TA_CENTER);
    SetBkColor(hdc, COLOR_BACKGROUND);
    SetTextColor(hdc, RGB(230,230,230));
    SelectObject(hdc, headersFont);
    TextOut(hdc, windowPixelSizeX/2, 30, head, _tcslen(head));
    
    HPEN myhpen = CreatePen(PS_SOLID, 7, RGB(0, 0, 0));
    SelectPen(hdc, myhpen);
    POINT pt;
    MoveToEx(hdc, windowPixelSizeX * 0.12, windowPixelSizeY * 0.45, &pt);
    LineTo(hdc, windowPixelSizeX * 0.12, windowPixelSizeY * 0.525);
    MoveToEx(hdc, windowPixelSizeX * 0.11, windowPixelSizeY * 0.49, &pt);
    LineTo(hdc, windowPixelSizeX * 0.12, windowPixelSizeY * 0.525);
    MoveToEx(hdc, windowPixelSizeX * 0.13, windowPixelSizeY * 0.49, &pt);
    LineTo(hdc, windowPixelSizeX * 0.12, windowPixelSizeY * 0.525);
    
    MoveToEx(hdc, windowPixelSizeX * (1-0.12), windowPixelSizeY * 0.525, &pt);
    LineTo(hdc, windowPixelSizeX * (1-0.12), windowPixelSizeY * 0.45);
    MoveToEx(hdc, windowPixelSizeX * (1-0.11), windowPixelSizeY * 0.485, &pt);
    LineTo(hdc, windowPixelSizeX * (1-0.12), windowPixelSizeY * 0.45);
    MoveToEx(hdc, windowPixelSizeX * (1-0.13), windowPixelSizeY * 0.485, &pt);
    LineTo(hdc, windowPixelSizeX * (1-0.12), windowPixelSizeY * 0.45);
}

void ConvertCP1251CodeToText(HWND hWnd)
{
    //Get text from Code edit

    int temp_length = SendMessage(editViewCP1251, WM_GETTEXTLENGTH, NULL, NULL);
    temp_length++;
    LPWSTR temp_str = new wchar_t[temp_length];
    SendMessage(editViewCP1251, WM_GETTEXT, temp_length, (LPARAM)temp_str);
    std::wstring wstr(temp_str);
    delete[] temp_str;

    //Search cyrillc codes

    std::wregex wreg(L"&#\\d+;");
    std::wsregex_iterator witer = std::wsregex_iterator(wstr.begin(), wstr.end(), wreg, std::regex_constants::match_default);
    std::vector<std::wstring> cyrillic_codes;

    for (; witer != std::wsregex_iterator(); witer++)
    {
        std::wsmatch match = *witer;
        cyrillic_codes.push_back(match.str());
    }
    
    //Converting

    if (!cyrillic_codes.empty())
    {
        int codes_amount = cyrillic_codes.size();
        std::vector<LPWSTR> cyrillic_symbols;

        for (int i = 0; i < codes_amount; i++)
        {
            cyrillic_codes[i].erase(0, 2);
            cyrillic_codes[i].pop_back();
            
            char codes_char = std::stoi(cyrillic_codes[i]);

            int need_len = MultiByteToWideChar(1251, MB_PRECOMPOSED,
                &codes_char, 1,
                NULL, 0 // 0 makes this function return required buffer size for this conversion
            );

            cyrillic_symbols.push_back(new wchar_t[need_len+1]);

            MultiByteToWideChar(1251, MB_PRECOMPOSED,
                &codes_char, 1,
                cyrillic_symbols[i], need_len
            );
            cyrillic_symbols[i][need_len] = '\0';
        }

        //Output compilation

        for (int i = 0; i < codes_amount; i++)
        {
            wstr = std::regex_replace(wstr, wreg, cyrillic_symbols[i], std::regex_constants::match_flag_type::format_first_only);
            delete[] cyrillic_symbols[i];
        }

        //Set text to Code edit

        SendMessage(editViewUnicode, WM_SETTEXT, NULL, (LPARAM)wstr.c_str());
    }
}

void ConvertTextToCP1251Code(HWND hWnd) 
{
    //Get text from Text edit

    int temp_length = SendMessage(editViewUnicode, WM_GETTEXTLENGTH, NULL, NULL);
    temp_length++;
    LPWSTR temp_str = new wchar_t[temp_length];
    SendMessage(editViewUnicode, WM_GETTEXT, temp_length, (LPARAM)temp_str);
    std::wstring wstr(temp_str);
    delete[] temp_str;

    //Search cyrillc symbols

    std::wregex wreg(L"[А-Яа-яЁёІіЎўЄєЇїҐґЂђЃѓЉљЊњЌќЋћЏџЈјЅѕ]");
    std::wsregex_iterator witer = std::wsregex_iterator(wstr.begin(), wstr.end(), wreg, std::regex_constants::match_default);
    std::vector<std::wstring> cyrillic_symbols;

    for (; witer != std::wsregex_iterator(); witer++)
    {
        std::wsmatch match = *witer;
        cyrillic_symbols.push_back( match.str() );
    }
    
    //Converting

    if (!cyrillic_symbols.empty()) 
    {
        int symbols_amount = cyrillic_symbols.size();

        LPCWSTR match_temp;
        char codes_lpstr;
        unsigned char* codes_char = new unsigned char[symbols_amount];

        for (int i = 0; i < symbols_amount; i++)
        {
            match_temp = cyrillic_symbols[i].c_str();

            WideCharToMultiByte(1251, WC_NO_BEST_FIT_CHARS,
                match_temp, cyrillic_symbols[i].length(),
                &codes_lpstr, 1,
                NULL, NULL
            );

            codes_char[i] = (unsigned char)codes_lpstr;
        }

        //Output compilation

        std::wstring code;
        
        for (int i = 0; i < symbols_amount; i++)
        {
            code = L"&#" + std::to_wstring(codes_char[i]) + L";";
            wstr = std::regex_replace(wstr, wreg, code, std::regex_constants::match_flag_type::format_first_only);
            
        }
        delete[] codes_char;

        //Set text to Code edit

        SendMessage(editViewCP1251, WM_SETTEXT, NULL, (LPARAM)wstr.c_str());
    }
}