#include "Texture.h"
#include "Shader.h"
#include "Renderer.h"
#include "CommandController.h"

Texture::Texture()
{
    m_ddsLoader = nullptr;
}

Texture::~Texture()
{
    delete m_ddsLoader;
}

void Texture::Initialize(
    const std::wstring& filename,
    Shader* shader)
{
    LoadTextureFromFile(filename);
    CreateTexture();
    CreateShaderResourceView(shader);
    UploadTexture();
}

void Texture::LoadTextureFromFile(
    const std::wstring& filename)
{
    delete m_ddsLoader;

    m_ddsLoader = new DDSLoad();
    m_ddsLoader->Load(filename);
}

void Texture::CreateTexture()
{
    auto device = Renderer::GetInstance().GetDevice();

    const DDS_HEADER* header =
        m_ddsLoader->GetHeader();

    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.Dimension =
        D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    textureDesc.Width = header->width;
    textureDesc.Height = header->height;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.MipLevels = 1;
    textureDesc.Format = m_ddsLoader->GetFormat();
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    CD3DX12_HEAP_PROPERTIES heapProperties(
        D3D12_HEAP_TYPE_DEFAULT);

    HRESULT result = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &textureDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(m_texture.GetAddressOf()));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create DDS texture.");
}

void Texture::CreateShaderResourceView(Shader* shader)
{
    auto device = Renderer::GetInstance().GetDevice();

    UINT descriptorSize =
        device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
        shader->GetDescrHeap()->GetCPUDescriptorHandleForHeapStart(),
        shader->NextSRVIndex(),
        descriptorSize);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Shader4ComponentMapping =
        D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    srvDesc.Format = m_ddsLoader->GetFormat();
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    device->CreateShaderResourceView(
        m_texture.Get(),
        &srvDesc,
        handle);
}

void Texture::UploadTexture()
{
    auto renderer = &Renderer::GetInstance();
    auto device = renderer->GetDevice();
    auto commands = renderer->GetCommands();

    const DDS_HEADER* header =
        m_ddsLoader->GetHeader();

    DXGI_FORMAT format = m_ddsLoader->GetFormat();

    UINT bytesPerBlock =
        (format == DXGI_FORMAT_BC1_UNORM ||
            format == DXGI_FORMAT_BC4_UNORM)
        ? 8
        : 16;

    size_t blocksWide =
        (static_cast<size_t>(header->width) + 3) / 4;

    size_t blocksHigh =
        (static_cast<size_t>(header->height) + 3) / 4;

    size_t rowPitch = blocksWide * bytesPerBlock;
    size_t slicePitch = rowPitch * blocksHigh;

    M_ASSERT(
        m_ddsLoader->GetSize() >= slicePitch,
        "DDS texture data is incomplete.");

    UINT64 uploadSize = GetRequiredIntermediateSize(
        m_texture.Get(),
        0,
        1);

    ComPtr<ID3D12Resource> uploadBuffer;

    CD3DX12_HEAP_PROPERTIES heapProperties(
        D3D12_HEAP_TYPE_UPLOAD);

    auto bufferDesc =
        CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

    HRESULT result = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadBuffer.GetAddressOf()));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create texture upload buffer.");

    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = m_ddsLoader->GetData();

    textureData.RowPitch =
        static_cast<LONG_PTR>(rowPitch);

    textureData.SlicePitch =
        static_cast<LONG_PTR>(slicePitch);

    commands->Reset();

    UpdateSubresources(
        commands->GetCommandList().Get(),
        m_texture.Get(),
        uploadBuffer.Get(),
        0,
        0,
        1,
        &textureData);

    commands->AddBarrier(
        m_texture.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commands->Close();
    commands->Execute();

    renderer->WaitForPreviousFrame();
}