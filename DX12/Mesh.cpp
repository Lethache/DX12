#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "CommandController.h"

Mesh::Mesh()
{
    m_vertexBufferView = {};
    m_indexBufferView = {};
    m_indexBufferSize = 0;
}

Mesh::~Mesh()
{
    delete GetTexture();
    SetTexture(nullptr);
}

void Mesh::Initialize(Shader* shader)
{
    SetShader(shader);

    CreateVertexBuffer();
    CreateIndexBuffer();

    Texture* texture = new Texture();

    texture->Initialize(
        L"..\\Assets\\Textures\\Wood.dds",
        shader->GetDescrHeap().Get());

    SetTexture(texture);
}

void Mesh::CreateVertexBuffer()
{
    auto device = Renderer::GetInstance().GetDevice();

    Vertex vertices[] =
    {
        {
            XMFLOAT3(-0.25f, 0.25f, 0.0f),
            XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
            XMFLOAT2(0.0f, 0.0f)
        },
        {
            XMFLOAT3(0.25f, -0.25f, 0.0f),
            XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
            XMFLOAT2(1.0f, 1.0f)
        },
        {
            XMFLOAT3(-0.25f, -0.25f, 0.0f),
            XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
            XMFLOAT2(0.0f, 1.0f)
        },
        {
            XMFLOAT3(0.25f, 0.25f, 0.0f),
            XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
            XMFLOAT2(1.0f, 0.0f)
        }
    };

    UINT bufferSize = static_cast<UINT>(sizeof(vertices));

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
        IID_PPV_ARGS(m_vertexBuffer.GetAddressOf()));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create vertex buffer.");

    void* mappedData = nullptr;
    CD3DX12_RANGE readRange(0, 0);

    result = m_vertexBuffer->Map(
        0,
        &readRange,
        &mappedData);

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to map vertex buffer.");

    memcpy(mappedData, vertices, bufferSize);

    m_vertexBuffer->Unmap(0, nullptr);

    m_vertexBufferView.BufferLocation =
        m_vertexBuffer->GetGPUVirtualAddress();

    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = bufferSize;
}

void Mesh::CreateIndexBuffer()
{
    auto device = Renderer::GetInstance().GetDevice();

    unsigned short indices[] =
    {
        0, 1, 2,
        0, 3, 1
    };

    m_indexBufferSize =
        static_cast<UINT>(sizeof(indices));

    CD3DX12_HEAP_PROPERTIES heapProperties(
        D3D12_HEAP_TYPE_UPLOAD);

    auto bufferDesc =
        CD3DX12_RESOURCE_DESC::Buffer(m_indexBufferSize);

    HRESULT result = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(m_indexBuffer.GetAddressOf()));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create index buffer.");

    void* mappedData = nullptr;
    CD3DX12_RANGE readRange(0, 0);

    result = m_indexBuffer->Map(
        0,
        &readRange,
        &mappedData);

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to map index buffer.");

    memcpy(mappedData, indices, m_indexBufferSize);

    m_indexBuffer->Unmap(0, nullptr);

    m_indexBufferView.BufferLocation =
        m_indexBuffer->GetGPUVirtualAddress();

    m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
    m_indexBufferView.SizeInBytes = m_indexBufferSize;
}

void Mesh::Render(XMMATRIX matrix)
{
    auto renderer = &Renderer::GetInstance();

    auto commandList =
        renderer->GetCommands()->GetCommandList();

    GetShader()->SetPipelineState();

    GetTexture()->SetPipelineState(
        GetShader()->GetDescrHeap().Get());

    XMStoreFloat4x4(
        &GetShader()->GetConstantData()
        [renderer->GetFrameIndex()].WVP,
        XMMatrixTranspose(matrix));

    commandList->IASetPrimitiveTopology(
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandList->IASetVertexBuffers(
        0,
        1,
        &m_vertexBufferView);

    commandList->IASetIndexBuffer(
        &m_indexBufferView);

    commandList->DrawIndexedInstanced(
        static_cast<UINT>(
            m_indexBufferSize / sizeof(unsigned short)),
        1,
        0,
        0,
        0);
}