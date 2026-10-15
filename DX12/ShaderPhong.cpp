#include "ShaderPhong.h"
#include "Renderer.h"

ShaderPhong::ShaderPhong()
{
    SetShaderFile(L"phong.hlsl");

    m_diffuseColor = {};
    m_ambientLight = {};
    m_lightDirection = {};
    m_lightColor = {};
    m_isLightCube = false;

    XMStoreFloat4x4(
        &m_normalMatrix,
        XMMatrixIdentity());
}

void ShaderPhong::SetDiffuseColor(XMFLOAT4 color)
{
    m_diffuseColor = color;
}

void ShaderPhong::SetAmbientLight(XMFLOAT4 color)
{
    m_ambientLight = color;
}

void ShaderPhong::SetLightDirection(XMFLOAT3 direction)
{
    m_lightDirection = direction;
}

void ShaderPhong::SetLightColor(XMFLOAT4 color)
{
    m_lightColor = color;
}

void ShaderPhong::SetIsLightCube(bool isLightCube)
{
    m_isLightCube = isLightCube;
}

void ShaderPhong::SetWorldMatrix(XMMATRIX world)
{
    XMMATRIX normalMatrix =
        XMMatrixTranspose(
            XMMatrixInverse(nullptr, world));

    // Transpose for HLSL's default column-major storage.
    XMStoreFloat4x4(
        &m_normalMatrix,
        XMMatrixTranspose(normalMatrix));
}

void ShaderPhong::CreatePipelineState()
{
    VTX_PosNormTex vtx;

    SetInputElementDescs(vtx.InputElementDescs);

    SetInputElementDescsSize(
        _countof(vtx.InputElementDescs));

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
            renderer->GetFrameIndex() *
            GetConstantBufferSize());

    data->NormalMatrix = m_normalMatrix;

    data->DiffuseColor = m_diffuseColor;
    data->AmbientLight = m_ambientLight;
    data->LightDirection = m_lightDirection;
    data->LightColor = m_lightColor;

    data->pad0 = 0.0f;
    data->IsLightCube = m_isLightCube ? 1U : 0U;
    data->pad1 = XMFLOAT3(0.0f, 0.0f, 0.0f);
}