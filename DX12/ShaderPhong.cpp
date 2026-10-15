#include "ShaderPhong.h"
#include "Renderer.h"

#include <algorithm>

ShaderPhong::ShaderPhong()
{
    SetShaderFile(L"phong.hlsl");

    m_parameters = {};

    XMStoreFloat4x4(
        &m_parameters.World,
        XMMatrixIdentity());

    XMStoreFloat4x4(
        &m_parameters.NormalMatrix,
        XMMatrixIdentity());
}

void ShaderPhong::SetDiffuseColor(XMFLOAT4 value)
{
    m_parameters.DiffuseColor = value;
}

void ShaderPhong::SetAmbientLight(XMFLOAT4 value)
{
    m_parameters.AmbientLight = value;
}

void ShaderPhong::SetLightPosition(XMFLOAT3 value)
{
    m_parameters.LightPosition = value;
}

void ShaderPhong::SetLightColor(XMFLOAT4 value)
{
    m_parameters.LightColor = value;
}

void ShaderPhong::SetSpecularPower(float value)
{
    m_parameters.SpecularPower =
        std::max(value, 1.0f);
}

void ShaderPhong::SetSpecularStrength(float value)
{
    m_parameters.SpecularStrength =
        std::max(value, 0.0f);
}

void ShaderPhong::SetSpecularColor(XMFLOAT4 value)
{
    m_parameters.SpecularColor = value;
}

void ShaderPhong::SetCameraPosition(XMFLOAT4 value)
{
    m_parameters.CameraPosition = value;
}

void ShaderPhong::SetIsLightCube(bool value)
{
    m_parameters.IsLightCube = value ? 1U : 0U;
}

void ShaderPhong::SetWorldMatrix(XMMATRIX world)
{
    XMStoreFloat4x4(
        &m_parameters.World,
        XMMatrixTranspose(world));

    XMMATRIX normalMatrix =
        XMMatrixTranspose(
            XMMatrixInverse(nullptr, world));

    XMStoreFloat4x4(
        &m_parameters.NormalMatrix,
        XMMatrixTranspose(normalMatrix));
}

void ShaderPhong::CreatePipelineState()
{
    VTX_PosNormTex vertexLayout;

    SetInputElementDescs(
        vertexLayout.InputElementDescs);

    SetInputElementDescsSize(
        _countof(vertexLayout.InputElementDescs));

    SetConstantBufferSize(sizeof(CB_Phong));

    Shader::CreatePipelineState();

    SetInputElementDescs(nullptr);
}

void ShaderPhong::CreateConstantBuffer(UINT64 bufferSize)
{
    Shader::CreateConstantBuffer(sizeof(CB_Phong));
}

void ShaderPhong::SetPipelineState()
{
    Shader::SetPipelineState();

    auto renderer = &Renderer::GetInstance();

    unsigned char* base =
        reinterpret_cast<unsigned char*>(
            GetConstantData());

    CB_Phong* data =
        reinterpret_cast<CB_Phong*>(
            base +
            renderer->GetFrameIndex()
            * GetConstantBufferSize());

    // Mesh::Render writes WVP after this method.
    data->World = m_parameters.World;
    data->NormalMatrix = m_parameters.NormalMatrix;

    data->DiffuseColor = m_parameters.DiffuseColor;
    data->AmbientLight = m_parameters.AmbientLight;
    data->LightPosition = m_parameters.LightPosition;
    data->LightColor = m_parameters.LightColor;

    data->SpecularPower = m_parameters.SpecularPower;
    data->SpecularStrength = m_parameters.SpecularStrength;
    data->SpecularColor = m_parameters.SpecularColor;
    data->CameraPosition = m_parameters.CameraPosition;

    data->IsLightCube = m_parameters.IsLightCube;

    data->pad0 = 0.0f;
    data->pad1 = 0.0f;
}