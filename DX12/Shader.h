#pragma once

#include "StandardIncludes.h"
#include "VertexStructs.h"

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

    void SetInputElementDescs(
        D3D12_INPUT_ELEMENT_DESC* inputElementDescs)
    {
        m_inputElementDescs = inputElementDescs;
    }

    void SetInputElementDescsSize(unsigned int size)
    {
        m_inputElemDescsSize = size;
    }

    void SetShaderFile(LPCWSTR shaderFile)
    {
        m_shaderFile = shaderFile;
    }

    void Initialize();
    void SetPipelineState();

    unsigned int NextSRVIndex();

protected:
    virtual void CreatePipelineState();

private:
    void CompileShaders();
    void CreateRootSignature();
    void CreateConstantBuffer();

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;

    ComPtr<ID3DBlob> m_vertexShader;
    ComPtr<ID3DBlob> m_pixelShader;

    ComPtr<ID3D12DescriptorHeap> m_descrHeap;
    ComPtr<ID3D12Resource> m_constantBuffer;

    ConstantBuffer* m_constantData;
    unsigned int m_srvIndex;

    D3D12_INPUT_ELEMENT_DESC* m_inputElementDescs;
    unsigned int m_inputElemDescsSize;

    LPCWSTR m_shaderFile;
};