#include "ShaderPhong.h"

ShaderPhong::ShaderPhong()
{
    SetShaderFile(L"phong.hlsl");
}

void ShaderPhong::CreatePipelineState()
{
    VTX_PosNormTex vtx;

    SetInputElementDescs(vtx.InputElementDescs);
    SetInputElementDescsSize(_countof(vtx.InputElementDescs));

    Shader::CreatePipelineState();

    SetInputElementDescs(nullptr);
}