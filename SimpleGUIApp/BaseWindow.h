#pragma once
#include "common.h"

template <class DERIVED_TYPE>
class BaseWindow
{
public:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        DERIVED_TYPE* pThis = NULL;

        if (uMsg == WM_NCCREATE)
        {
            CREATESTRUCT* pCreate = (CREATESTRUCT*)lParam;
            pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hwnd = hwnd;
        }
        else
        {
            pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        if (pThis)
        {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else
        {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    BaseWindow() : m_hwnd(NULL) { }

    virtual BOOL Create(
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
        m_hinst = GetModuleHandle(nullptr);

        if (!SetClassName(className)) {
            return FALSE;
        }

        WNDCLASSEXW wcex = { 0 };

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = DERIVED_TYPE::WindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(LONG_PTR);
        wcex.hInstance = m_hinst;
        wcex.hIcon = icon;
        wcex.hCursor = cursor;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = menu;
        wcex.lpszClassName = m_className;
        wcex.hIconSm = iconSmall;

        RegisterClassEx(&wcex);

        m_hwnd = CreateWindowEx(
            dwExStyle, m_className, lpWindowName, dwStyle, x, y,
            nWidth, nHeight, hWndParent, hMenu, m_hinst, this
        );

        return (m_hwnd ? TRUE : FALSE);
    }

    HWND Window() const { return m_hwnd; }

    BOOL SetClassName(PCWSTR className) {
        HRESULT hr = StringCchCopyW(m_className, sizeof(m_className) / sizeof(*m_className), className);

        if (FAILED(hr)) {
            return FALSE;
        }

        return TRUE;
    }

    PCWSTR ClassName() const {
        return m_className;
    }


protected:

    virtual void Initialize(int nCmdShow) = 0;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
    virtual int RunMessageLoop() = 0;
    
    HWND m_hwnd = nullptr;
    HINSTANCE m_hinst = nullptr;
    wchar_t m_className[256] = { 0 };
};