#pragma once

#include "DeviceResources.h"

struct SceneRenderer
{
    void Render();
    void DrawRectangle(int x, int y, int w, int h, int r, int g, int b, float a);

    struct DeviceDependentResources
    {
        //
    } m_deviceDependentResources;

    DeviceResources * m_pDeviceResources;

    void InitializeDeviceDependentResources(DeviceResources & deviceResources);
};
