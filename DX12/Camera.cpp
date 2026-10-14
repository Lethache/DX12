#include "Camera.h"

Camera::Camera()
{
    XMStoreFloat4x4(&m_projection, XMMatrixIdentity());
    XMStoreFloat4x4(&m_view, XMMatrixIdentity());
}

Camera::Camera(int width, int height)
{
    XMMATRIX projection = XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        100.0f);

    XMStoreFloat4x4(&m_projection, projection);

    XMVECTOR camPos = XMVectorSet(0.0f, 1.0f, -3.0f, 1.0f);
    XMVECTOR lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    XMVECTOR camUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(camPos, lookAt, camUp);

    XMStoreFloat4x4(&m_view, view);
}

Camera::~Camera()
{
}