
#include "pch.h"

#include "SceneRenderer.h"

void SceneRenderer::InitializeDeviceDependentResources(DeviceResources & deviceResources)
{
    m_pDeviceResources = &deviceResources;
}

void SceneRenderer::Render()
{
    if (m_pDeviceResources == nullptr)
    {
        return;
    }

    auto context = m_pDeviceResources->m_deviceDependentResources.context;

    auto renderTargetView = m_pDeviceResources->m_viewportDependentResources.renderTargetView;
    auto depthStencilView = m_pDeviceResources->m_viewportDependentResources.depthStencilView;

    // Reset the viewport to target the bounds of the camera.
    context->RSSetViewports(1, &m_pDeviceResources->m_viewportDependentResources.viewport);

    // Reset render targets to the screen.
    ID3D11RenderTargetView *const targets[1] = { renderTargetView.Get() };
    context->OMSetRenderTargets(1, targets, depthStencilView.Get());

    // Clear the back buffer and depth stencil view.
    context->ClearRenderTargetView(renderTargetView.Get(), DirectX::Colors::White);
    context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void SceneRenderer::DrawRectangle(int x, int y, int w, int h, int r, int g, int b, float a)
{
    if (m_pDeviceResources == nullptr)
    {
        return;
    }

    // TODO
}
