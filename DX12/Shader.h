#pragma once

#include "StandardIncludes.h"

using namespace DirectX;

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT4 color;
};

class Shader
{
public:
    Shader();
    virtual ~Shader() {}

    ComPtr<ID3D12RootSignature>& GetRootSignature()
    {
        return m_rootSignature;
    }

    ComPtr<ID3D12PipelineState>& GetPipelineState()
    {
        return m_pipelineState;
    }

    void Initialize(
        LPCWSTR _shaderFile,
        ID3D12Device* _device);

    void SetPipelineState(
        ID3D12GraphicsCommandList* _commandList);

private:
    void CompileShaders(LPCWSTR _shaderFile);
    void CreatePipelineState(ID3D12Device* _device);

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3DBlob> m_vertexShader;
    ComPtr<ID3DBlob> m_pixelShader;
};