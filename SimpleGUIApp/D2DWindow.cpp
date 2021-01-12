#include "D2DWindow.h"
#include "resource.h"

D2DWindow::D2DWindow() :
    BaseWindow(),
    m_dpiScaleFactor(0.f),
    m_hAccelTable(nullptr),
    m_pDirect2dFactory(nullptr),
    m_pRenderTarget(nullptr),
    m_pLightSlateGrayBrush(nullptr),
    m_pCornflowerBlueBrush(nullptr)
{
}

D2DWindow::~D2DWindow()
{
    SafeRelease(&m_pDirect2dFactory);
    SafeRelease(&m_pDWriteFactory);
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
    SafeRelease(&m_pPurpleBrush);
    SafeRelease(&m_pTextHeadingFormat);
    SafeRelease(&m_pTextBodyFormat);
}

BOOL D2DWindow::Create(
    PCWSTR className,
    PCWSTR lpWindowName,
    DWORD dwStyle,
    DWORD dwExStyle,
    int x,
    int y,
    int nWidth,
    int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    LPWSTR menu,
    HCURSOR cursor,
    HICON icon,
    HICON iconSmall
)
{
    HRESULT hr;

    // Initialize device-indpendent resources, such
    // as the Direct2D factory.
    hr = CreateDeviceIndependentResources();

    if (SUCCEEDED(hr)) {
        if (BaseWindow::Create(className, lpWindowName, dwStyle, dwExStyle,
            CW_USEDEFAULT, CW_USEDEFAULT, 1280, 768, hWndParent, hMenu,
            menu, cursor, icon, iconSmall)) {

            // Correct the window size if the DPI differs from 96
            UINT dpi = GetDpiForWindow(m_hwnd);

            RECT rect;
            // Get the current rectangle for the window
            GetWindowRect(m_hwnd, &rect);

            if (AdjustWindowRectExForDpi(&rect, dwStyle, menu != nullptr, dwExStyle, dpi)) {
                OnDpiChange(dpi, &rect);
            }
            
            Initialize(SW_NORMAL);

            RECT msize;

            if (GetCurrentMonitorSize(m_hwnd, &msize)) {
                wchar_t buf[256];
                swprintf_s(buf, 256, L"Width: %u, Height: %u", msize.right - msize.left, msize.bottom - msize.top);
                MessageBox(m_hwnd, buf, L"Monitor Size!", MB_OK);
            }

            return TRUE;
        }
    }

    return FALSE;
}

void D2DWindow::Initialize(int nCmdShow) {
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}


int D2DWindow::RunMessageLoop() {
    m_hAccelTable = LoadAccelerators(m_hinst, MAKEINTRESOURCE(IDC_SIMPLEGUIAPP));

    MSG msg = {};

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, m_hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

LRESULT D2DWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SIZE:
            {
                UINT width = LOWORD(lParam);
                UINT height = HIWORD(lParam);
                OnResize(width, height);
            }
            return 0;
        case WM_DISPLAYCHANGE:
            {
                InvalidateRect(m_hwnd, NULL, FALSE);
            }
            return 0;
        case WM_COMMAND:
            {
                int wmId = LOWORD(wParam);
                // Parse the menu selections:
                switch (wmId)
                {
                    case IDM_ABOUT:
                        DialogBox(m_hinst, MAKEINTRESOURCE(IDD_ABOUTBOX), m_hwnd, About);
                        break;
                    case IDM_FILE_EXIT:
                        DestroyWindow(m_hwnd);
                        break;
                    default:
                        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
                }
            }
            return 0;
        case WM_DPICHANGED:
            {
                uint16_t dpi = HIWORD(wParam);
                RECT* recWinSize = (RECT*)lParam;

                // Adjust the window size/position to match the new DPI
                OnDpiChange(dpi, recWinSize);
                // Resize the D2D render target
                OnResize(recWinSize->right - recWinSize->left, recWinSize->bottom - recWinSize->top);
                // Invalid the client area
                InvalidateRect(m_hwnd, NULL, FALSE);

                wchar_t boxText[256] = L"";
                swprintf_s(boxText, 200, L"Received WM_DPICHANGED with DPI: %d", dpi);

                const wchar_t* boxCaption = L"Caught a window message!";
                MessageBoxExW(m_hwnd, boxText, boxCaption, MB_OK, 0);
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 1;

        case WM_PAINT:
            {
                OnRender();
                ValidateRect(m_hwnd, nullptr);
            }
            return 0;

        default:
            return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }

    return TRUE;
}

// Message handler for about box.
INT_PTR CALLBACK D2DWindow::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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


void D2DWindow::OnDpiChange(uint16_t dpi, RECT* rec) {
    m_dpiScaleFactor = (float)dpi / USER_DEFAULT_SCREEN_DPI;

    SetWindowPos(m_hwnd, NULL,
        rec->left, rec->top,
        rec->right - rec->left,
        rec->bottom - rec->top,
        SWP_NOACTIVATE | SWP_NOZORDER);
}

HRESULT D2DWindow::OnRender()
{
    HRESULT hr = S_OK;

    hr = CreateDeviceResources();
    
    if (SUCCEEDED(hr)) {
        m_pRenderTarget->BeginDraw();
        m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
        m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        D2D1_SIZE_F rtSize = m_pRenderTarget->GetSize();

        // Draw a grid background.
        int width = static_cast<int>(rtSize.width);
        int height = static_cast<int>(rtSize.height);

        for (int x = 0; x < width; x += 10)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(static_cast<FLOAT>(x), 0.0f),
                D2D1::Point2F(static_cast<FLOAT>(x), rtSize.height),
                m_pLightSlateGrayBrush,
                0.5f
            );
        }

        for (int y = 0; y < height; y += 10)
        {
            m_pRenderTarget->DrawLine(
                D2D1::Point2F(0.0f, static_cast<FLOAT>(y)),
                D2D1::Point2F(rtSize.width, static_cast<FLOAT>(y)),
                m_pLightSlateGrayBrush,
                0.5f
            );
        }

        DrawText();

        // Draw two rectangles.
        D2D1_RECT_F rectangle1 = D2D1::RectF(
            rtSize.width / 2 - 50.0f,
            rtSize.height / 2 + 0.0f,
            rtSize.width / 2 + 50.0f,
            rtSize.height / 2 + 100.0f
        );

        D2D1_RECT_F rectangle2 = D2D1::RectF(
            rtSize.width / 2 - 100.0f,
            rtSize.height / 2 - 50.0f,
            rtSize.width / 2 + 100.0f,
            rtSize.height / 2 + 150.0f
        );

        // Draw a filled rectangle.
        m_pRenderTarget->FillRectangle(&rectangle1, m_pLightSlateGrayBrush);

        // Draw the outline of a rectangle.
        m_pRenderTarget->DrawRectangle(&rectangle2, m_pCornflowerBlueBrush);

        hr = m_pRenderTarget->EndDraw();
    }

    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        DiscardDeviceResources();
    }
    else if (FAILED(hr)) {
        DiscardDeviceResources();
    }

    return hr;
}

void D2DWindow::OnResize(UINT width, UINT height) {
    if (m_pRenderTarget) {
        m_pRenderTarget->Resize(D2D1::SizeU(width, height));
    }
}


HRESULT D2DWindow::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;

    // Create a Direct2D factory.
    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pDirect2dFactory);

    if (SUCCEEDED(hr))
    {
        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&m_pDWriteFactory));
    }

    if (SUCCEEDED(hr)) {
        hr = LayoutText();
    }

    return hr;
}

HRESULT D2DWindow::LayoutText() {
    HRESULT hr = m_pDWriteFactory->CreateTextFormat(
            L"Segoe UI",                // Font family name.
            NULL,                       // Font collection (NULL sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_DEMI_BOLD,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            48.0f, // 36 
            L"en-us",
            &m_pTextHeadingFormat);
    
    if (SUCCEEDED(hr))
    {
        hr = m_pDWriteFactory->CreateTextFormat(
            L"Segoe UI",                // Font family name.
            NULL,                       // Font collection (NULL sets it to use the system font collection).
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            36.0f, // 18
            L"en-us",
            &m_pTextBodyFormat
        );
    }

    // Center align (horizontally) the text.
    if (SUCCEEDED(hr))
    {
        hr = m_pTextHeadingFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pTextHeadingFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    // Left justify (horizontally) the text.
    if (SUCCEEDED(hr))
    {
        hr = m_pTextBodyFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    }

    if (SUCCEEDED(hr))
    {
        hr = m_pTextBodyFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }

    return hr;
}

void D2DWindow::DrawText() {
    float dpi = (float)GetDpiForWindow(m_hwnd) / USER_DEFAULT_SCREEN_DPI;

    RECT rc;
    GetClientRect(m_hwnd, &rc);

    int curr_y = rc.top;

    RECT rcHeading;
    {
        int top_margin = 20, bot_margin = 20, bbox_height = 80;
        curr_y += top_margin;

        int bbox_width = rc.right - rc.left;
        int height_left = rc.bottom - curr_y;

        rcHeading.left = rc.left;
        rcHeading.right = rc.right;

        rcHeading.top = curr_y;
        curr_y += bbox_height;
        
        rcHeading.bottom = curr_y;
        curr_y += bot_margin;
    }

    RECT rcBody;
    {
        int top_margin = 20, bot_margin = 20, bbox_height = 120;
        curr_y += top_margin;

        int bbox_width = rc.right - rc.left;
        int height_left = rc.bottom - curr_y;

        rcBody.left = rc.left;
        rcBody.right = rc.right;

        rcBody.top = curr_y;
        curr_y += bbox_height;

        rcBody.bottom = curr_y;
        curr_y += bot_margin;
    }

    D2D1_RECT_F layoutRectHeading = D2D1::RectF(
        static_cast<FLOAT>(rcHeading.left) / dpi,
        static_cast<FLOAT>(rcHeading.top) / dpi,
        static_cast<FLOAT>(rcHeading.right) / dpi,
        static_cast<FLOAT>(rcHeading.bottom) / dpi
    );

    D2D1_RECT_F layoutRectBody = D2D1::RectF(
        static_cast<FLOAT>(rcBody.left) / dpi,
        static_cast<FLOAT>(rcBody.top) / dpi,
        static_cast<FLOAT>(rcBody.right) / dpi,
        static_cast<FLOAT>(rcBody.bottom) / dpi
    );

    m_pRenderTarget->DrawText(m_textHeading, static_cast<UINT32>(wcslen(m_textHeading)),
        m_pTextHeadingFormat, &layoutRectHeading, m_pPurpleBrush);

    m_pRenderTarget->DrawText(m_textBody, static_cast<UINT32>(wcslen(m_textBody)),
        m_pTextBodyFormat, &layoutRectBody, m_pPurpleBrush);

}


HRESULT D2DWindow::CreateDeviceResources() {
    HRESULT hr = S_OK;

    if (!m_pRenderTarget)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(
            rc.right - rc.left,
            rc.bottom - rc.top
        );

        // Create a Direct2D render target.
        hr = m_pDirect2dFactory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size),
            &m_pRenderTarget
        );

        if (SUCCEEDED(hr))
        {
            // Create a gray brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::LightSlateGray),
                &m_pLightSlateGrayBrush
            );
        }

        if (SUCCEEDED(hr))
        {
            // Create a blue brush.
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::CornflowerBlue),
                &m_pCornflowerBlueBrush
            );
        }

        if (SUCCEEDED(hr)) {
            hr = m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(D2D1::ColorF::Purple),
                &m_pPurpleBrush
            );
        }
    }

    return hr;
}

void D2DWindow::DiscardDeviceResources()
{
    SafeRelease(&m_pRenderTarget);
    SafeRelease(&m_pLightSlateGrayBrush);
    SafeRelease(&m_pCornflowerBlueBrush);
    SafeRelease(&m_pPurpleBrush);
}