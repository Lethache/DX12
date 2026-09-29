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

    void SetCameraMatrices(
        const XMFLOAT4X4& view,
        const XMFLOAT4X4& projection)
    {
        m_cameraData.view = view;
        m_cameraData.projection = projection;
    }

private:
    void CompileShaders(LPCWSTR _shaderFile);
    void CreatePipelineState(ID3D12Device* _device);

    struct CameraData
    {
        XMFLOAT4X4 view;
        XMFLOAT4X4 projection;
    };

    static_assert(
        sizeof(CameraData) == 32 * sizeof(float),
        "Camera data must contain exactly 32 floats.");

    CameraData m_cameraData = {};

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3DBlob> m_vertexShader;
    ComPtr<ID3DBlob> m_pixelShader;
};