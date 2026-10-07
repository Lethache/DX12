#pragma once

#include "StandardIncludes.h"

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT4 color;
    XMFLOAT2 texcoords;
};

struct alignas(256) ConstantBuffer
{
    XMFLOAT4X4 WVP;
};

class Shader
{
public:
    Shader();
    virtual ~Shader();

    ComPtr<ID3D12RootSignature>& GetRootSignature()
    {
        return m_rootSignature;
    }

    ComPtr<ID3D12PipelineState>& GetPipelineState()
    {
        return m_pipelineState;
    }

    ComPtr<ID3D12DescriptorHeap>& GetDescrHeap()
    {
        return m_descrHeap;
    }

    ConstantBuffer* GetConstantData() const
    {
        return m_constantData;
    }

    void Initialize(LPCWSTR _shaderFile);
    void SetPipelineState();

private:
    void CompileShaders(LPCWSTR _shaderFile);
    void CreateRootSignature();
    void CreatePipelineState();
    void CreateConstantBuffer();

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3DBlob> m_vertexShader;
    ComPtr<ID3DBlob> m_pixelShader;
    ComPtr<ID3D12DescriptorHeap> m_descrHeap;
    ComPtr<ID3D12Resource> m_constantBuffer;
    ConstantBuffer* m_constantData;
};