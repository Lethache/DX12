#include "ShaderPhong.h"
#include "Renderer.h"

ShaderPhong::ShaderPhong()
{
    SetShaderFile(L"phong.hlsl");
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
        reinterpret_cast<unsigned char*>(GetConstantData());

    CB_Phong* constantData =
        reinterpret_cast<CB_Phong*>(
            base +
            renderer->GetFrameIndex() * GetConstantBufferSize());

    constantData->DiffuseColor =
        XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
}