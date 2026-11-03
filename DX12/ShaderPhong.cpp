#include "ShaderPhong.h"

#include <cstring>

ShaderPhong::ShaderPhong()
{
    SetShaderFile(L"phong.hlsl");

    m_parameters = {};

    XMStoreFloat4x4(
        &m_parameters.WVP,
        XMMatrixIdentity());

    XMStoreFloat4x4(
        &m_parameters.World,
        XMMatrixIdentity());

    XMStoreFloat4x4(
        &m_parameters.NormalMatrix,
        XMMatrixIdentity());

    SetDiffuseColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    SetSpecularColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    SetSpecularPower(32.0f);
    SetSpecularStrength(1.0f);

    SetAmbientLight(
        XMFLOAT4(0.12f, 0.12f, 0.12f, 1.0f));

    SetLightPosition(
        XMFLOAT3(0.0f, 3.0f, -3.0f));

    SetLightColor(
        XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

    SetIsLightCube(false);
    SetUseSpecularMap(true);
}

void ShaderPhong::SetWorldMatrix(XMMATRIX world)
{
    XMStoreFloat4x4(
        &m_parameters.World,
        XMMatrixTranspose(world));

    const XMMATRIX normalMatrix =
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

void ShaderPhong::CreateConstantBuffer(UINT64)
{
    Shader::CreateConstantBuffer(sizeof(CB_Phong));
}

void ShaderPhong::SetPipelineState()
{
    Shader::SetPipelineState();

    std::memcpy(
        ConstantBufferAddress(),
        &m_parameters,
        sizeof(m_parameters));

    // Mesh::Render writes WVP into this same slot afterward.
}