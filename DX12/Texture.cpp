#include "Texture.h"
#include "Renderer.h"
#include "CommandController.h"

Texture::Texture()
{
    m_width = 256;
    m_height = 256;
    m_pixelSize = 4;
}

void Texture::Initialize(ID3D12DescriptorHeap* _descrHeap)
{
    CreateTexture();
    CreateShaderResourceView(_descrHeap);
    UploadTexture();
}

void Texture::CreateTexture()
{
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    desc.Width = m_width;
    desc.Height = m_height;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    auto device = Renderer::GetInstance().GetDevice().Get();

    M_ASSERT(SUCCEEDED(device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&m_texture))),
        "Failed to create texture.");
}

void Texture::CreateShaderResourceView(
    ID3D12DescriptorHeap* _descrHeap)
{
    auto device = Renderer::GetInstance().GetDevice().Get();

    UINT size = device->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
        _descrHeap->GetCPUDescriptorHandleForHeapStart(), 1, size);

    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Shader4ComponentMapping =
        D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipLevels = 1;

    device->CreateShaderResourceView(
        m_texture.Get(), &desc, handle);
}

void Texture::UploadTexture()
{
    auto r = &Renderer::GetInstance();
    auto device = r->GetDevice().Get();

    ComPtr<ID3D12Resource> uploadHeap;

    UINT64 size = GetRequiredIntermediateSize(
        m_texture.Get(), 0, 1);

    auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(size);

    M_ASSERT(SUCCEEDED(device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadHeap))),
        "Failed to create texture upload heap.");

    std::vector<UINT8> pixels = GenerateTextureData();

    D3D12_SUBRESOURCE_DATA data = {};
    data.pData = pixels.data();
    data.RowPitch = m_width * m_pixelSize;
    data.SlicePitch = data.RowPitch * m_height;

    auto commands = r->GetCommands();
    commands->Reset();

    UpdateSubresources(
        commands->GetCommandList().Get(),
        m_texture.Get(),
        uploadHeap.Get(),
        0, 0, 1, &data);

    commands->AddBarrier(
        m_texture.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    commands->Close();
    commands->Execute();

    r->WaitForPreviousFrame();
}

std::vector<UINT8> Texture::GenerateTextureData()
{
    const UINT rowPitch = m_width * m_pixelSize;
    const UINT cellWidth = m_width / 8;
    const UINT cellHeight = m_height / 8;

    std::vector<UINT8> data(rowPitch * m_height);

    for (UINT y = 0; y < m_height; ++y)
    {
        for (UINT x = 0; x < m_width; ++x)
        {
            UINT offset = y * rowPitch + x * m_pixelSize;
            bool black = (x / cellWidth) % 2 == (y / cellHeight) % 2;
            UINT8 color = black ? 0 : 255;

            data[offset] = color;
            data[offset + 1] = color;
            data[offset + 2] = color;
            data[offset + 3] = 255;
        }
    }

    return data;
}

void Texture::SetPipelineState(ID3D12DescriptorHeap* _descrHeap)
{
    auto r = &Renderer::GetInstance();

    UINT size = r->GetDevice()->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CD3DX12_GPU_DESCRIPTOR_HANDLE handle(
        _descrHeap->GetGPUDescriptorHandleForHeapStart(), 1, size);

    r->GetCommands()->GetCommandList()
        ->SetGraphicsRootDescriptorTable(1, handle);
}