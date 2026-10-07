#include "Shader.h"
#include "Renderer.h"
#include "CommandController.h"

Shader::Shader()
{
    m_constantData = nullptr;
}

Shader::~Shader()
{
    if (m_constantBuffer && m_constantData)
    {
        m_constantBuffer->Unmap(0, nullptr);
        m_constantData = nullptr;
    }
}

void Shader::Initialize(LPCWSTR _shaderFile)
{
    CompileShaders(_shaderFile);
    CreateRootSignature();
    CreatePipelineState();
    CreateConstantBuffer();
}

void Shader::CompileShaders(LPCWSTR _shaderFile)
{
    UINT flags = 0;

#if defined(_DEBUG)
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ComPtr<ID3DBlob> errors;

    HRESULT result = D3DCompileFromFile(
        _shaderFile,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "VSMain",
        "vs_5_0",
        flags,
        0,
        &m_vertexShader,
        &errors);

    if (FAILED(result))
    {
        M_ASSERT(false, errors
            ? static_cast<const char*>(errors->GetBufferPointer())
            : "Cannot open shaders.hlsl or compile vertex shader.");
    }

    errors.Reset();

    result = D3DCompileFromFile(
        _shaderFile,
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "PSMain",
        "ps_5_0",
        flags,
        0,
        &m_pixelShader,
        &errors);

    if (FAILED(result))
    {
        M_ASSERT(false, errors
            ? static_cast<const char*>(errors->GetBufferPointer())
            : "Failed to compile pixel shader.");
    }
}

void Shader::CreateRootSignature()
{
    CD3DX12_ROOT_PARAMETER parameters[2];

    CD3DX12_DESCRIPTOR_RANGE srvRange;
    srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    parameters[0].InitAsConstantBufferView(0);
    parameters[1].InitAsDescriptorTable(
        1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);

    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.MaxAnisotropy = 1;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    CD3DX12_ROOT_SIGNATURE_DESC desc;
    desc.Init(
        _countof(parameters),
        parameters,
        1,
        &sampler,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> errors;

    HRESULT result = D3D12SerializeRootSignature(
        &desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &errors);

    if (FAILED(result))
    {
        M_ASSERT(false, errors
            ? static_cast<const char*>(errors->GetBufferPointer())
            : "Failed to serialize root signature.");
    }

    auto device = Renderer::GetInstance().GetDevice().Get();

    M_ASSERT(SUCCEEDED(device->CreateRootSignature(
        0,
        signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature))),
        "Failed to create root signature.");
}

void Shader::CreatePipelineState()
{
    D3D12_INPUT_ELEMENT_DESC input[] =
    {
        {
            "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
            0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        {
            "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
            0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
            0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }
    };

    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};

    desc.InputLayout = { input, _countof(input) };
    desc.pRootSignature = m_rootSignature.Get();
    desc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader.Get());
    desc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader.Get());

    desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    desc.DepthStencilState.DepthEnable = FALSE;
    desc.DepthStencilState.StencilEnable = FALSE;

    desc.SampleMask = UINT_MAX;
    desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    desc.NumRenderTargets = 1;
    desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;

    auto device = Renderer::GetInstance().GetDevice().Get();

    M_ASSERT(SUCCEEDED(device->CreateGraphicsPipelineState(
        &desc, IID_PPV_ARGS(&m_pipelineState))),
        "Failed to create graphics pipeline state.");
}

void Shader::CreateConstantBuffer()
{
    auto r = &Renderer::GetInstance();
    auto device = r->GetDevice().Get();

    const UINT64 elementSize = sizeof(ConstantBuffer);
    const UINT64 bufferSize = elementSize * r->GetFrameCount();

    auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

    M_ASSERT(SUCCEEDED(device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer))),
        "Failed to create constant buffer.");

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 2;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    M_ASSERT(SUCCEEDED(device->CreateDescriptorHeap(
        &heapDesc, IID_PPV_ARGS(&m_descrHeap))),
        "Failed to create CBV/SRV descriptor heap.");

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = static_cast<UINT>(elementSize);

    device->CreateConstantBufferView(
        &cbvDesc, m_descrHeap->GetCPUDescriptorHandleForHeapStart());

    CD3DX12_RANGE readRange(0, 0);

    M_ASSERT(SUCCEEDED(m_constantBuffer->Map(
        0, &readRange, reinterpret_cast<void**>(&m_constantData))),
        "Failed to map constant buffer.");
}

void Shader::SetPipelineState()
{
    auto r = &Renderer::GetInstance();
    auto commands = r->GetCommands()->GetCommandList().Get();

    commands->SetPipelineState(m_pipelineState.Get());
    commands->SetGraphicsRootSignature(m_rootSignature.Get());

    ID3D12DescriptorHeap* heaps[] = { m_descrHeap.Get() };
    commands->SetDescriptorHeaps(_countof(heaps), heaps);

    commands->SetGraphicsRootConstantBufferView(
        0,
        m_constantBuffer->GetGPUVirtualAddress()
        + static_cast<UINT64>(r->GetFrameIndex()) * sizeof(ConstantBuffer));
}