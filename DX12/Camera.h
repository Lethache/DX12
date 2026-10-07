#pragma once

#include "StandardIncludes.h"

class Camera
{
public:
    Camera();
    Camera(int _width, int _height);
    virtual ~Camera();

    XMMATRIX GetProjection() const
    {
        return XMLoadFloat4x4(&m_projection);
    }

    XMMATRIX GetView() const
    {
        return XMLoadFloat4x4(&m_view);
    }

private:
    XMFLOAT4X4 m_projection;
    XMFLOAT4X4 m_view;
};