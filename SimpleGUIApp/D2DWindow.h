#pragma once
#include "BaseWindow.h"

class D2DWindow :
    public BaseWindow<D2DWindow>
{
private:
    static INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
    
    // Initialize device-independent resources.
    HRESULT CreateDeviceIndependentResources();

    // Initialize device-dependent resources.
    HRESULT CreateDeviceResources();

    // Release device-dependent resource.
    void DiscardDeviceResources();

    // Draw content.
    HRESULT OnRender();

    // Resize the render target.
    void OnResize(UINT width, UINT height);

    void OnDpiChange(uint16_t dpi, RECT* rec);

    HRESULT LayoutText();

    void DrawText();

public:
    D2DWindow();
    ~D2DWindow();

    virtual BOOL Create(
        PCWSTR className,
        PCWSTR lpWindowName,
        DWORD dwStyle,
        DWORD dwExStyle = 0,
        int x = CW_USEDEFAULT,
        int y = CW_USEDEFAULT,
        int nWidth = CW_USEDEFAULT,
        int nHeight = CW_USEDEFAULT,
        HWND hWndParent = 0,
        HMENU hMenu = 0,
        LPWSTR menu = nullptr,
        HCURSOR cursor = nullptr,
        HICON icon = nullptr,
        HICON iconSmall = nullptr
    );

    void Initialize(int nCmdShow);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    int RunMessageLoop();

private:
    HACCEL m_hAccelTable = nullptr;
    float m_dpiScaleFactor = 0.f;

    ID2D1Factory* m_pDirect2dFactory;
    ID2D1HwndRenderTarget* m_pRenderTarget;
    ID2D1SolidColorBrush* m_pLightSlateGrayBrush;
    ID2D1SolidColorBrush* m_pCornflowerBlueBrush;
    ID2D1SolidColorBrush* m_pPurpleBrush = nullptr;
    IDWriteFactory* m_pDWriteFactory = nullptr;
    IDWriteTextFormat* m_pTextHeadingFormat = nullptr;
    IDWriteTextFormat* m_pTextBodyFormat = nullptr;

    const wchar_t* m_textHeading = L"This is a simple app!";
    const wchar_t* m_textBody = L"In this simple app we use Direct2D and DirectWrite!";
};

