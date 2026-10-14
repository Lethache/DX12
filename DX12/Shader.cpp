#include "Shader.h"
#include "Renderer.h"
#include "CommandController.h"

Shader::Shader()
{
    m_constantData = nullptr;
    m_srvIndex = 0;
    m_shaderFile = L"shaders.hlsl";
    m_inputElementDescs = nullptr;
    m_inputElemDescsSize = 0;
}

Shader::~Shader()
{
    if (m_constantBuffer && m_constantData)
    {
        m_constantBuffer->Unmap(0, nullptr);
        m_constantData = nullptr;
    }
}

void Shader::Initialize()
{
    CompileShaders();
    CreateRootSignature();
    CreatePipelineState();
    CreateConstantBuffer();
}

unsigned int Shader::NextSRVIndex()
{
    M_ASSERT(
        m_srvIndex < 2,
        "This shader supports only two textures.");

    return ++m_srvIndex;
}

void Shader::CompileShaders()
{
    UINT flags = 0;

#if defined(_DEBUG)
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> error;

    HRESULT result = D3DCompileFromFile(
        m_shaderFile,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VSMain",
        "vs_5_0",
        flags,
        0,
        m_vertexShader.GetAddressOf(),
        error.GetAddressOf());

    M_ASSERT(
        SUCCEEDED(result),
        error
        ? static_cast<const char*>(error->GetBufferPointer())
        : "Failed to compile vertex shader.");

    error.Reset();

    result = D3DCompileFromFile(
        m_shaderFile,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PSMain",
        "ps_5_0",
        flags,
        0,
        m_pixelShader.GetAddressOf(),
        error.GetAddressOf());

    M_ASSERT(
        SUCCEEDED(result),
        error
        ? static_cast<const char*>(error->GetBufferPointer())
        : "Failed to compile pixel shader.");
}

void Shader::CreateRootSignature()
{
    auto device = Renderer::GetInstance().GetDevice();

    CD3DX12_DESCRIPTOR_RANGE srvRange;
    srvRange.Init(
        D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
        2,
        0);

    CD3DX12_ROOT_PARAMETER rootParameters[2];

    rootParameters[0].InitAsConstantBufferView(0);

    rootParameters[1].InitAsDescriptorTable(
        1,
        &srvRange,
        D3D12_SHADER_VISIBILITY_PIXEL);

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;

    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

    sampler.MaxAnisotropy = 1;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;

    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;

    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    CD3DX12_ROOT_SIGNATURE_DESC rootDesc;
    rootDesc.Init(
        _countof(rootParameters),
        rootParameters,
        1,
        &sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;

    HRESULT result = D3D12SerializeRootSignature(
        &rootDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        signature.GetAddressOf(),
        error.GetAddressOf());

    M_ASSERT(
        SUCCEEDED(result),
        error
        ? static_cast<const char*>(error->GetBufferPointer())
        : "Failed to serialize root signature.");

    result = device->CreateRootSignature(
        0,
        signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(m_rootSignature.GetAddressOf()));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create root signature.");
}

void Shader::CreatePipelineState()
{
    auto device = Renderer::GetInstance().GetDevice();

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

    desc.InputLayout =
    {
        m_inputElementDescs,
        m_inputElemDescsSize
    };

    desc.pRootSignature = m_rootSignature.Get();

    desc.VS =
    {
        m_vertexShader->GetBufferPointer(),
        m_vertexShader->GetBufferSize()
    };

    desc.PS =
    {
        m_pixelShader->GetBufferPointer(),
        m_pixelShader->GetBufferSize()
    };

    desc.RasterizerState =
        CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    desc.BlendState =
        CD3DX12_BLEND_DESC(D3D12_DEFAULT);

    desc.DepthStencilState =
        CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    desc.DepthStencilState.DepthEnable = FALSE;
    desc.DepthStencilState.StencilEnable = FALSE;

    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType =
        D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;

    HRESULT result = device->CreateGraphicsPipelineState(
        &desc,
        IID_PPV_ARGS(m_pipelineState.GetAddressOf()));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create pipeline state.");
}

void Shader::CreateConstantBuffer()
{
    auto renderer = &Renderer::GetInstance();
    auto device = renderer->GetDevice();

    const UINT64 elementSize = sizeof(ConstantBuffer);
    const UINT64 bufferSize =
        elementSize * renderer->GetFrameCount();

    CD3DX12_HEAP_PROPERTIES heapProperties(
        D3D12_HEAP_TYPE_UPLOAD);

    auto bufferDesc =
        CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    HRESULT result = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_constantBuffer.GetAddressOf()));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create constant buffer.");

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 3;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    result = device->CreateDescriptorHeap(
        &heapDesc,
        IID_PPV_ARGS(m_descrHeap.GetAddressOf()));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create descriptor heap.");

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation =
        m_constantBuffer->GetGPUVirtualAddress();

    cbvDesc.SizeInBytes = static_cast<UINT>(elementSize);

    device->CreateConstantBufferView(
        &cbvDesc,
        m_descrHeap->GetCPUDescriptorHandleForHeapStart());

    CD3DX12_RANGE readRange(0, 0);

    result = m_constantBuffer->Map(
        0,
        &readRange,
        reinterpret_cast<void**>(&m_constantData));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to map constant buffer.");

    memset(m_constantData, 0, static_cast<size_t>(bufferSize));
}

void Shader::SetPipelineState()
{
    auto renderer = &Renderer::GetInstance();
    auto commandList =
        renderer->GetCommands()->GetCommandList();

    commandList->SetPipelineState(m_pipelineState.Get());
    commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* heaps[] =
    {
        m_descrHeap.Get()
    };

    commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    commandList->SetGraphicsRootConstantBufferView(
        0,
        m_constantBuffer->GetGPUVirtualAddress() +
        renderer->GetFrameIndex() * sizeof(ConstantBuffer));

    UINT descriptorSize =
        renderer->GetDevice()->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(
        m_descrHeap->GetGPUDescriptorHandleForHeapStart(),
        1,
        descriptorSize);

    commandList->SetGraphicsRootDescriptorTable(1, srvHandle);
}