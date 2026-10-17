#pragma once

#include "Shader.h"
#include <cstddef>

class ShaderPhong : public Shader
{
public:
    ShaderPhong();

    struct alignas(256) CB_Phong
    {
        XMFLOAT4X4 WVP;
        XMFLOAT4X4 World;
        XMFLOAT4X4 NormalMatrix;

        XMFLOAT4 DiffuseColor;
        XMFLOAT4 AmbientLight;

        XMFLOAT3 LightPosition;
        float pad0;

        XMFLOAT4 LightColor;

        float SpecularPower;
        float SpecularStrength;
        UINT IsLightCube;
        UINT ColorByPosition;

        XMFLOAT4 SpecularColor;
        XMFLOAT4 CameraPosition;
    };

    void SetDiffuseColor(XMFLOAT4 value);
    void SetAmbientLight(XMFLOAT4 value);
    void SetLightPosition(XMFLOAT3 value);
    void SetLightColor(XMFLOAT4 value);

    void SetSpecularPower(float value);
    void SetSpecularStrength(float value);
    void SetSpecularColor(XMFLOAT4 value);
    void SetCameraPosition(XMFLOAT4 value);

    void SetIsLightCube(bool value);
    void SetColorByPosition(bool value);

    void SetWorldMatrix(XMMATRIX world);

    void SetPipelineState() override;

protected:
    void CreatePipelineState() override;
    void CreateConstantBuffer(UINT64 bufferSize) override;

private:
    CB_Phong m_parameters;
};

static_assert(
    sizeof(ShaderPhong::CB_Phong) == 512,
    "CB_Phong must use a 512-byte frame stride.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, DiffuseColor) == 192,
    "DiffuseColor offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, SpecularPower) == 256,
    "SpecularPower offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, ColorByPosition) == 268,
    "ColorByPosition offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, CameraPosition) == 288,
    "CameraPosition offset must match HLSL.");