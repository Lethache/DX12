
#include "Mesh.h"
#include "Shader.h"

Mesh::Mesh()
{
    m_vertexBuffer = nullptr;
    m_vertexBufferView = {};

    m_indexBufferSize = 0;
    m_indexBuffer = nullptr;
    m_indexBufferView = {};
}

void Mesh::Initialize(
    ID3D12Device* _device,
    Shader* _shader)
{
    SetShader(_shader);

    CreateVertexBuffer(_device);
    CreateIndexBuffer(_device);
}

void Mesh::CreateVertexBuffer(ID3D12Device* _device)
{
    Vertex vertices[] =
    {
        // Position               // RGBA color
        {{-0.50f,  0.00f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
        {{-0.25f,  0.25f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
        {{-0.25f, -0.25f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
        {{ 0.00f,  0.50f, 0.0f}, {1.0f, 1.0f, 0.0f, 1.0f}},
        {{ 0.50f,  0.00f, 0.0f}, {0.0f, 1.0f, 1.0f, 1.0f}},
        {{ 0.50f,  0.75f, 0.0f}, {1.0f, 0.0f, 1.0f, 1.0f}}
    };

    auto heapProperties =
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    auto resourceDescription =
        CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));

    auto result = _device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDescription,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create vertex buffer resource.");

    UINT8* vertexDataBegin = nullptr;

    CD3DX12_RANGE readRange(0, 0);

    result = m_vertexBuffer->Map(
        0,
        &readRange,
        reinterpret_cast<void**>(&vertexDataBegin));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to map vertex buffer.");

    memcpy(
        vertexDataBegin,
        vertices,
        sizeof(vertices));

    m_vertexBuffer->Unmap(0, nullptr);

    m_vertexBufferView.BufferLocation =
        m_vertexBuffer->GetGPUVirtualAddress();

    m_vertexBufferView.StrideInBytes =
        sizeof(Vertex);

    m_vertexBufferView.SizeInBytes =
        sizeof(vertices);
}

void Mesh::CreateIndexBuffer(ID3D12Device* _device)
{
    unsigned short indices[] =
    {
        0, 1, 2, 3, 4, 5
    };

    m_indexBufferSize = sizeof(indices);

    auto heapProperties =
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    auto resourceDescription =
        CD3DX12_RESOURCE_DESC::Buffer(m_indexBufferSize);

    auto result = _device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDescription,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_indexBuffer));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to create index buffer resource.");

    UINT8* indexDataBegin = nullptr;

    CD3DX12_RANGE readRange(0, 0);

    result = m_indexBuffer->Map(
        0,
        &readRange,
        reinterpret_cast<void**>(&indexDataBegin));

    M_ASSERT(
        SUCCEEDED(result),
        "Failed to map index buffer.");

    memcpy(
        indexDataBegin,
        indices,
        m_indexBufferSize);

    m_indexBuffer->Unmap(0, nullptr);

    m_indexBufferView.BufferLocation =
        m_indexBuffer->GetGPUVirtualAddress();

    m_indexBufferView.SizeInBytes =
        m_indexBufferSize;

    m_indexBufferView.Format =
        DXGI_FORMAT_R16_UINT;
}

void Mesh::Render(
    ID3D12GraphicsCommandList* _commandList)
{
    _commandList->IASetPrimitiveTopology(
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    _commandList->IASetVertexBuffers(
        0,
        1,
        &m_vertexBufferView);

    _commandList->DrawInstanced(
        6,
        1,
        0,
        0);
}