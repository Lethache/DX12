
#include "Shader.h"

Shader::Shader()
{
    m_rootSignature = nullptr;
    m_pipelineState = nullptr;
    m_vertexShader = nullptr;
    m_pixelShader = nullptr;
}

void Shader::Initialize(
    LPCWSTR _shaderFile,
    ID3D12Device* _device)
{
    CompileShaders(_shaderFile);
    CreatePipelineState(_device);
}

void Shader::CompileShaders(LPCWSTR _shaderFile)
{
#if defined(_DEBUG)
    UINT compileFlags =
        D3DCOMPILE_DEBUG |
        D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif

    ComPtr<ID3DBlob> errorBlob;

    // Compile vertex shader
    HRESULT result = D3DCompileFromFile(
        _shaderFile,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VSMain",
        "vs_5_0",
        compileFlags,
        0,
        &m_vertexShader,
        &errorBlob);

    if (FAILED(result))
    {
        if (errorBlob != nullptr)
        {
            OutputDebugStringA(
                static_cast<const char*>(
                    errorBlob->GetBufferPointer()));
        }

        M_ASSERT(
            false,
            "Failed to compile vertex shader.");

        return;
    }

    errorBlob.Reset();

    // Compile pixel shader
    result = D3DCompileFromFile(
        _shaderFile,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PSMain",
        "ps_5_0",
        compileFlags,
        0,
        &m_pixelShader,
        &errorBlob);

    if (FAILED(result))
    {
        if (errorBlob != nullptr)
        {
            OutputDebugStringA(
                static_cast<const char*>(
                    errorBlob->GetBufferPointer()));
        }

        M_ASSERT(
            false,
            "Failed to compile pixel shader.");

        return;
    }
}

void Shader::CreatePipelineState(ID3D12Device* _device)
{
    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;

    // Create root signature
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;

    rootSignatureDesc.Init(
        0,
        nullptr,
        0,
        nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    HRESULT result = D3D12SerializeRootSignature(
        &rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        &signature,
        &error);

    if (FAILED(result))
    {
        if (error != nullptr)
        {
            OutputDebugStringA(
                static_cast<const char*>(
                    error->GetBufferPointer()));
        }

        M_ASSERT(
            false,
            "Failed to serialize root signature.");

        return;
    }

    result = _device->CreateRootSignature(
        0,
        signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create root signature.");

    if (FAILED(result))
    {
        return;
    }

    // Vertex input layout
    D3D12_INPUT_ELEMENT_DESC inputElementDescriptions[] =
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        }
    };

    // Configure graphics pipeline
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDescription = {};

    pipelineDescription.InputLayout =
    {
        inputElementDescriptions,
        _countof(inputElementDescriptions)
    };

    pipelineDescription.pRootSignature =
        m_rootSignature.Get();

    pipelineDescription.VS =
        CD3DX12_SHADER_BYTECODE(m_vertexShader.Get());

    pipelineDescription.PS =
        CD3DX12_SHADER_BYTECODE(m_pixelShader.Get());

    pipelineDescription.RasterizerState =
        CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    pipelineDescription.BlendState =
        CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    pipelineDescription.DepthStencilState.DepthEnable = FALSE;
    pipelineDescription.DepthStencilState.StencilEnable = FALSE;

    pipelineDescription.SampleMask = UINT_MAX;

    pipelineDescription.PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    pipelineDescription.NumRenderTargets = 1;

    pipelineDescription.RTVFormats[0] =
        DXGI_FORMAT_R8G8B8A8_UNORM;

    pipelineDescription.SampleDesc.Count = 1;

    // Create pipeline state object
    result = _device->CreateGraphicsPipelineState(
        &pipelineDescription,
        IID_PPV_ARGS(&m_pipelineState));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create graphics pipeline state.");
}

void Shader::SetPipelineState(
    ID3D12GraphicsCommandList* _commandList)
{
    // Set root signature
    _commandList->SetGraphicsRootSignature(
        m_rootSignature.Get());

    // Set graphics pipeline state
    _commandList->SetPipelineState(
        m_pipelineState.Get());
}