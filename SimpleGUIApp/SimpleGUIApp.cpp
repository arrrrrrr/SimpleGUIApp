// SimpleGUIApp.cpp : Defines the entry point for the application.
//

#include "SimpleGUIApp.h"
#include "D2DWindow.h"

#define MAX_LOADSTRING 100

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    wchar_t szWindowClass[MAX_LOADSTRING];
    wchar_t szWindowName[MAX_LOADSTRING];

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szWindowName, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SIMPLEGUIAPP, szWindowClass, MAX_LOADSTRING);

    // Initialize Resources
    HCURSOR cursor = LoadCursor(nullptr, IDC_ARROW);
    HICON icon = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_SIMPLEGUIAPP));
    HICON iconSmall = LoadIcon(nullptr, MAKEINTRESOURCE(IDI_SMALL));
    LPWSTR menuName = MAKEINTRESOURCE(IDC_SIMPLEGUIAPP);

    // Initialize Main Window Class
    DWORD wndFlags = WS_OVERLAPPEDWINDOW;
    DWORD wndExFlags = 0UL;

    if SUCCEEDED(CoInitialize(nullptr))
    {
        {
            D2DWindow window;

            if (!window.Create(szWindowClass, szWindowName, wndFlags, wndExFlags,
                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr,
                menuName,
                cursor,
                icon,
                iconSmall))
            {
                wprintf(L"Error creating window\n");
                return 0;
            }


            window.RunMessageLoop();
        }

        CoUninitialize();
    }

    return 0;
    
}

