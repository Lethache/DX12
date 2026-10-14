#pragma once

#include "Shader.h"

class ShaderPhong : public Shader
{
public:
    ShaderPhong();

    struct alignas(256) CB_Phong
    {
        XMFLOAT4X4 WVP;
        XMFLOAT4 DiffuseColor;
    };

    void SetPipelineState() override;

protected:
    void CreatePipelineState() override;
    void CreateConstantBuffer(UINT64 bufferSize) override;
};