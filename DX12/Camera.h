#pragma once

#include <DirectXMath.h>

class Camera
{
public:
    // Select one of the four view/projection combinations.
    void SetSetup(int setup, float aspectRatio)
    {
        using namespace DirectX;

        m_setup = setup;

        // Setups 1 and 2 use the first view.
        // Setups 3 and 4 use the second view.
        XMVECTOR position;

        if (setup <= 2)
        {
            position = XMVectorSet(
                0.0f, 0.0f, -2.0f, 1.0f);
        }
        else
        {
            position = XMVectorSet(
                1.3f, 0.8f, -2.0f, 1.0f);
        }

        XMVECTOR target = XMVectorSet(
            0.0f, 0.2f, 0.0f, 1.0f);

        XMVECTOR up = XMVectorSet(
            0.0f, 1.0f, 0.0f, 0.0f);

        XMStoreFloat4x4(
            &m_view,
            XMMatrixLookAtLH(position, target, up));

        // Setups 1 and 3 use a 45-degree field of view.
        // Setups 2 and 4 use a 75-degree field of view.
        float fovDegrees =
            (setup == 1 || setup == 3) ? 45.0f : 75.0f;

        XMStoreFloat4x4(
            &m_projection,
            XMMatrixPerspectiveFovLH(
                XMConvertToRadians(fovDegrees),
                aspectRatio,
                0.1f,
                100.0f));
    }

    const DirectX::XMFLOAT4X4& GetView() const
    {
        return m_view;
    }

    const DirectX::XMFLOAT4X4& GetProjection() const
    {
        return m_projection;
    }

    int GetSetup() const
    {
        return m_setup;
    }

private:
    DirectX::XMFLOAT4X4 m_view = {};
    DirectX::XMFLOAT4X4 m_projection = {};

    int m_setup = 1;
};