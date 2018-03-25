#pragma once

#include <winapifamily.h>

#if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    #define WINDOWS_DESKTOP
#elif WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
    #define WINDOWS_UWP
#endif

struct DeviceResources
{
    void Present();

    struct DeviceDependentResources
    {
        Microsoft::WRL::ComPtr<ID3D11Device> device;
        D3D_FEATURE_LEVEL featureLevel;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
    } m_deviceDependentResources;

    void InitializeDeviceResources(IDXGIAdapter * adapter = nullptr);

    struct ViewportDependentResources
    {
        D3D11_VIEWPORT viewport;
        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
    } m_viewportDependentResources;

#if defined(WINDOWS_UWP)
    void InitializeViewportResources(winrt::Windows::UI::Core::CoreWindow const & window, float dpi = 1.0f);
#elif defined(WINDOWS_DESKTOP)
    void InitializeViewportResources(HWND window);
#endif
};
