#pragma once

#include "Shader.h"
#include <cstddef>

class ShaderPhong : public Shader
{
public:
    static constexpr UINT MAX_LIGHTS = 8;

    struct Material
    {
        XMFLOAT4 DiffuseColor;
        XMFLOAT4 SpecularColor;

        float SpecularPower;
        float SpecularStrength;
        XMFLOAT2 Padding;
    };

    struct Light
    {
        XMFLOAT4 AmbientColor;

        XMFLOAT3 Position;
        float Padding;

        XMFLOAT4 Color;

        XMFLOAT3 ConstLinQuad;
        float Padding1;

        XMFLOAT3 SpotDirection;
        float Padding2;

        XMFLOAT2 SpotConeFalloff;
        XMFLOAT2 Padding3;
    };

    struct alignas(256) CB_Phong
    {
        XMFLOAT4X4 WVP;
        XMFLOAT4X4 World;
        XMFLOAT4X4 NormalMatrix;

        Material material;

        Light Lights[MAX_LIGHTS];

        XMFLOAT4 CameraPosition;

        UINT LightCount;
        UINT IsLightCube;
        UINT UseSpecularMap;
        float ScenePadding;
    };

    ShaderPhong();

    void SetDiffuseColor(XMFLOAT4 value)
    {
        m_parameters.material.DiffuseColor = value;
    }

    void SetSpecularColor(XMFLOAT4 value)
    {
        m_parameters.material.SpecularColor = value;
    }

    void SetSpecularPower(float value)
    {
        m_parameters.material.SpecularPower =
            (std::max)(value, 1.0f);
    }

    void SetSpecularStrength(float value)
    {
        m_parameters.material.SpecularStrength =
            (std::max)(value, 0.0f);
    }

    void SetLightCount(UINT count)
    {
        m_parameters.LightCount =
            (std::min)(count, MAX_LIGHTS);
    }

    void SetLight(UINT index, const Light& value)
    {
        M_ASSERT(
            index < MAX_LIGHTS,
            "Light index exceeds MAX_LIGHTS.");

        m_parameters.Lights[index] = value;
    }

    void SetCameraPosition(XMFLOAT4 value)
    {
        m_parameters.CameraPosition = value;
    }

    void SetIsLightCube(bool value)
    {
        m_parameters.IsLightCube = value ? 1U : 0U;
    }

    void SetUseSpecularMap(bool value)
    {
        m_parameters.UseSpecularMap = value ? 1U : 0U;
    }

    void SetWorldMatrix(XMMATRIX world);

    void SetPipelineState() override;

protected:
    void CreatePipelineState() override;
    void CreateConstantBuffer(UINT64 bufferSize) override;

private:
    CB_Phong m_parameters;
};

static_assert(
    sizeof(ShaderPhong::Material) == 48,
    "Material layout must match HLSL.");

static_assert(
    sizeof(ShaderPhong::Light) == 96,
    "Light layout must match HLSL.");

static_assert(
    offsetof(ShaderPhong::Light, ConstLinQuad) == 48,
    "Attenuation offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::Light, SpotDirection) == 64,
    "SpotDirection offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::Light, SpotConeFalloff) == 80,
    "SpotConeFalloff offset must match HLSL.");

static_assert(
    sizeof(ShaderPhong::CB_Phong) == 1280,
    "CB_Phong must have a 1280-byte stride.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, material) == 192,
    "Material offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, Lights) == 240,
    "Lights offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, CameraPosition) == 1008,
    "CameraPosition offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, LightCount) == 1024,
    "LightCount offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, IsLightCube) == 1028,
    "IsLightCube offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, UseSpecularMap) == 1032,
    "UseSpecularMap offset must match HLSL.");