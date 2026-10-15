#pragma once

#include "Shader.h"

class ShaderPhong : public Shader
{
public:
    ShaderPhong();

    struct alignas(256) CB_Phong
    {
        XMFLOAT4X4 WVP;
        XMFLOAT4X4 NormalMatrix;

        XMFLOAT4 DiffuseColor;
        XMFLOAT4 AmbientLight;

        XMFLOAT3 LightDirection;
        float pad0;

        XMFLOAT4 LightColor;

        UINT IsLightCube;
        XMFLOAT3 pad1;
    };

    void SetDiffuseColor(XMFLOAT4 color);
    void SetAmbientLight(XMFLOAT4 color);
    void SetLightDirection(XMFLOAT3 direction);
    void SetLightColor(XMFLOAT4 color);
    void SetIsLightCube(bool isLightCube);
    void SetWorldMatrix(XMMATRIX world);

    void SetPipelineState() override;

protected:
    void CreatePipelineState() override;
    void CreateConstantBuffer(UINT64 bufferSize) override;

private:
    XMFLOAT4 m_diffuseColor;
    XMFLOAT4 m_ambientLight;
    XMFLOAT3 m_lightDirection;
    XMFLOAT4 m_lightColor;

    XMFLOAT4X4 m_normalMatrix;
    bool m_isLightCube;
};