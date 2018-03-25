
#include "pch.h"

#include "DeviceResources.h"

#define ReturnIfFailed(hr) { if (FAILED(hr)) { __debugbreak(); return; } }

#if defined(WINDOWS_DESKTOP)
#define ReturnIfWin32Failed(err) { if (!err) { __debugbreak(); return; } }
#endif

void DeviceResources::InitializeDeviceResources(
    IDXGIAdapter * adapter)
{
    DeviceDependentResources resources = {};

    HRESULT hr = S_OK;

    UINT creationFlags = 0;

#if defined(_DEBUG)
    hr = D3D11CreateDevice(
        adapter,
        D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
        0,
        D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
        nullptr,                    // Any feature level will do.
        0,
        D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
        nullptr,                    // No need to keep the D3D device reference.
        nullptr,                    // No need to know the feature level.
        nullptr                     // No need to keep the D3D device context reference.
    );

    if (SUCCEEDED(hr))
    {
        // If the project is in a debug build, enable debugging via SDK Layers with this flag.
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }
#endif

    // This array defines the set of DirectX hardware feature levels this app will support.
    // Note the ordering should be preserved.
    // Don't forget to declare your application's minimum required feature level in its
    // description.  All applications are assumed to support 9.1 unless otherwise stated.
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // Create the Direct3D 11 API device object and a corresponding context.
    hr = D3D11CreateDevice(
        adapter,                        // Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,       // Create a device using the hardware graphics driver.
        0,                              // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
        creationFlags,                  // Set debug and Direct2D compatibility flags.
        featureLevels,                  // List of feature levels this app can support.
        ARRAYSIZE(featureLevels),       // Size of the list above.
        D3D11_SDK_VERSION,              // Always set this to D3D11_SDK_VERSION for Windows Store apps.
        &resources.device,              // Returns the Direct3D device created.
        &resources.featureLevel,        // Returns feature level of device created.
        &resources.context              // Returns the device immediate context.
    );

    if (FAILED(hr))
    {
        // If the initialization fails, fall back to the WARP device.
        ReturnIfFailed(D3D11CreateDevice(
            adapter,
            D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
            0,
            creationFlags,
            featureLevels,
            ARRAYSIZE(featureLevels),
            D3D11_SDK_VERSION,
            &resources.device,
            &resources.featureLevel,
            &resources.context
        ));
    }

    m_deviceDependentResources = resources;
}

#if defined(WINDOWS_UWP)
void DeviceResources::InitializeViewportResources(winrt::Windows::UI::Core::CoreWindow const & window, float dpi)
#elif defined(WINDOWS_DESKTOP)
void DeviceResources::InitializeViewportResources(HWND window)
#endif
{
    ViewportDependentResources resources = {};

    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    m_deviceDependentResources.context->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    m_viewportDependentResources.renderTargetView = nullptr;
    m_viewportDependentResources.depthStencilView = nullptr;
    m_deviceDependentResources.context->Flush();

    // Calculate the size of the viewport.

#if defined(WINDOWS_UWP)
    float width = window.Bounds().Width * dpi;
    float height = window.Bounds().Height * dpi;
#elif defined(WINDOWS_DESKTOP)
    RECT windowRect;
    ReturnIfWin32Failed(GetClientRect(window, &windowRect));

    float width = windowRect.right;
    float height = windowRect.bottom;
#endif

    resources.viewport = CD3D11_VIEWPORT(
        0.0f,
        0.0f,
        width,
        height
    );

    if (m_viewportDependentResources.swapChain)
    {
        // If the swap chain already exists, resize it.
        resources.swapChain = m_viewportDependentResources.swapChain;

        HRESULT hr = resources.swapChain->ResizeBuffers(
            2, // Double-buffered swap chain.
            (UINT)(width),
            (UINT)(height),
            DXGI_FORMAT_B8G8R8A8_UNORM,
            0
        );

        if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
        {
            // If the device was removed for any reason, a new device and swap chain will need to be created.
            //HandleDeviceLost();

            // Everything is set up now. Do not continue execution of this method. HandleDeviceLost will reenter this method 
            // and correctly set up the new device.
            return;
        }
        else
        {
            ReturnIfFailed(hr);
        }
    }
    else
    {
        // Otherwise, create a new one using the same adapter as the existing Direct3D device.
        DXGI_SCALING scaling = DXGI_SCALING_NONE;
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };

        swapChainDesc.Width = (UINT)(width);     // Match the size of the window.
        swapChainDesc.Height = (UINT)(height);
        swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;                      // This is the most common swap chain format.
        swapChainDesc.Stereo = false;
        swapChainDesc.SampleDesc.Count = 1;                                     // Don't use multi-sampling.
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 2;                                          // Use double-buffering to minimize latency.
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;            // All Windows Store apps must use this SwapEffect.
        swapChainDesc.Flags = 0;
        swapChainDesc.Scaling = scaling;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

        // This sequence obtains the DXGI factory that was used to create the Direct3D device above.
        Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
        ReturnIfFailed(m_deviceDependentResources.device.As(&dxgiDevice));

        Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
        ReturnIfFailed(dxgiDevice->GetAdapter(&dxgiAdapter));

        Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
        ReturnIfFailed(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

#if defined(WINDOWS_UWP)
        auto windowUnknown = window.as<IUnknown>();
        HRESULT hr = dxgiFactory->CreateSwapChainForCoreWindow(
            m_deviceDependentResources.device.Get(),
            const_cast<IUnknown *>(&*windowUnknown),
            &swapChainDesc,
            nullptr,
            &resources.swapChain);
#elif defined(WINDOWS_DESKTOP)
        HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(
            m_deviceDependentResources.device.Get(),
            window,
            &swapChainDesc,
            nullptr,
            nullptr,
            &resources.swapChain);
#endif

        // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
        // ensures that the application will only render after each VSync, minimizing power consumption.
        ReturnIfFailed(dxgiDevice->SetMaximumFrameLatency(1));
    }

    // Create a render target view of the swap chain back buffer.
    Microsoft::WRL::ComPtr<ID3D11Texture2D1> backBuffer;
    ReturnIfFailed(resources.swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)));

    ReturnIfFailed(m_deviceDependentResources.device->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        &resources.renderTargetView));

    // Create a depth stencil view for use with 3D rendering if needed.
    CD3D11_TEXTURE2D_DESC depthStencilDesc(
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        (UINT)(width),
        (UINT)(height),
        1, // This depth stencil view has only one texture.
        1, // Use a single mipmap level.
        D3D11_BIND_DEPTH_STENCIL
    );

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
    ReturnIfFailed(m_deviceDependentResources.device->CreateTexture2D(
        &depthStencilDesc,
        nullptr,
        &depthStencil));

    CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
    ReturnIfFailed(m_deviceDependentResources.device->CreateDepthStencilView(
        depthStencil.Get(),
        &depthStencilViewDesc,
        &resources.depthStencilView));

    m_deviceDependentResources.context->RSSetViewports(1, &resources.viewport);

    m_viewportDependentResources = resources;
}

void DeviceResources::Present()
{
    Microsoft::WRL::ComPtr<ID3D11DeviceContext3> context;
    ReturnIfFailed(m_deviceDependentResources.context.As(&context));

    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    DXGI_PRESENT_PARAMETERS parameters = { 0 };
    HRESULT hr = m_viewportDependentResources.swapChain->Present1(1, 0, &parameters);

    // Discard the contents of the render target.
    // This is a valid operation only when the existing contents will be entirely
    // overwritten. If dirty or scroll rects are used, this call should be removed.
    context->DiscardView1(m_viewportDependentResources.renderTargetView.Get(), nullptr, 0);

    // Discard the contents of the depth stencil.
    context->DiscardView1(m_viewportDependentResources.depthStencilView.Get(), nullptr, 0);

    // If the device was removed either by a disconnection or a driver upgrade, we 
    // must recreate all device resources.
    if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
    {
        //HandleDeviceLost();
    }
    else
    {
        ReturnIfFailed(hr);
    }
}
