#pragma once

#include "Shader.h"
#include <cstddef>

class ShaderPhong : public Shader
{
public:
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
    };

    struct alignas(256) CB_Phong
    {
        XMFLOAT4X4 WVP;
        XMFLOAT4X4 World;
        XMFLOAT4X4 NormalMatrix;

        Material material;
        Light light;

        XMFLOAT4 CameraPosition;

        UINT IsLightCube;
        UINT UseSpecularMap;
        XMFLOAT2 ScenePadding;
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

    void SetAmbientLight(XMFLOAT4 value)
    {
        m_parameters.light.AmbientColor = value;
    }

    void SetLightPosition(XMFLOAT3 value)
    {
        m_parameters.light.Position = value;
    }

    void SetLightColor(XMFLOAT4 value)
    {
        m_parameters.light.Color = value;
    }

    void SetAttenuation(
        float constant,
        float linear,
        float quadratic)
    {
        m_parameters.light.ConstLinQuad = XMFLOAT3(
            (std::max)(constant, 1.0f),
            (std::max)(linear, 0.0f),
            (std::max)(quadratic, 0.0f));
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
    sizeof(ShaderPhong::Light) == 64,
    "Light layout must match HLSL.");

static_assert(
    offsetof(ShaderPhong::Light, ConstLinQuad) == 48,
    "Attenuation offset must match HLSL.");

static_assert(
    sizeof(ShaderPhong::CB_Phong) == 512,
    "CB_Phong must have a 512-byte stride.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, material) == 192,
    "Material offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, light) == 240,
    "Light offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, CameraPosition) == 304,
    "CameraPosition offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, IsLightCube) == 320,
    "IsLightCube offset must match HLSL.");

static_assert(
    offsetof(ShaderPhong::CB_Phong, UseSpecularMap) == 324,
    "UseSpecularMap offset must match HLSL.");