
#include "pch.h"

#include "Imports.h"
#include "Exports.h"

#include "SceneRenderer.h"

BOOL g_running = true;

DeviceResources g_deviceResources;
SceneRenderer g_scene;

void drawRect(int x, int y, int w, int h, int r, int g, int b, float a)
{
    g_scene.DrawRectangle(x, y, w, h, r, g, b, a);
}

LRESULT CALLBACK WndProc(
    _In_ HWND hwnd,
    _In_ UINT msg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam)
{
    switch (msg)
    {
        case WM_POINTERENTER:
        case WM_POINTERDOWN:
        case WM_POINTERUPDATE:
        case WM_POINTERUP:
        case WM_POINTERLEAVE:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            mouseMoved(x, y);

            return 0;
        }
        case WM_SIZE:
        {
            g_deviceResources.InitializeViewportResources(hwnd);

            return 0;
        }
        case WM_CLOSE:
        {
            g_running = false;
            return 0;
        }
        default:
            return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
}

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE /* hPrevInstance */,
    _In_ LPWSTR lpCmdLine,
    _In_ int /* nCmdShow */)
{
    // Enable Mouse-as-Pointer and disable IMEs
    EnableMouseInPointer(TRUE);
    ImmDisableIME(0);

    WNDCLASSW wc = { 0 };

    wc.lpszClassName = L"Gam";
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;

    RegisterClassW(&wc);

    RECT rcWindow = { 0, 0, 1024, 768 };
    AdjustWindowRect(&rcWindow, wc.style, false);

    int x = 0;
    int y = 0;

    int width = rcWindow.right - rcWindow.left;
    int height = rcWindow.bottom - rcWindow.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    x = (screenWidth - width) / 2;
    y = (screenHeight - height) / 2;

    HWND hwnd = CreateWindowW(
        wc.lpszClassName,
        L"Gam",
        WS_OVERLAPPEDWINDOW,
        x,
        y,
        width,
        height,
        0,
        0,
        hInstance,
        0);

    g_deviceResources.InitializeDeviceResources();
    g_scene.InitializeDeviceDependentResources(g_deviceResources);

    g_deviceResources.InitializeViewportResources(hwnd);

    ShowWindow(hwnd, SW_SHOW);

    while (g_running)
    {
        MSG msg = { 0 };

        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        g_scene.Render();
        g_deviceResources.Present();
    }

    DestroyWindow(hwnd);

    UnregisterClassW(L"Gam", hInstance);

    return 0;
}
