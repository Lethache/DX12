#include "Mesh.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
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

void Mesh::Initialize(Shader* _shader)
{
    SetShader(_shader);
    CreateVertexBuffer();
    CreateIndexBuffer();

    Texture* texture = new Texture();
    texture->Initialize(_shader->GetDescrHeap().Get());
    SetTexture(texture);
}

void Mesh::CreateVertexBuffer()
{
    Vertex vertices[] =
    {
        {
            { 0.0f, 0.25f, 0.0f },
            { 1.0f, 0.0f, 0.0f, 1.0f },
            { 0.5f, 0.0f }
        },
        {
            { 0.25f, -0.25f, 0.0f },
            { 0.0f, 1.0f, 0.0f, 1.0f },
            { 1.0f, 1.0f }
        },
        {
            { -0.25f, -0.25f, 0.0f },
            { 0.0f, 0.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f }
        }
    };

    auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertices));
    auto device = Renderer::GetInstance().GetDevice().Get();

    M_ASSERT(SUCCEEDED(device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_vertexBuffer))),
        "Failed to create vertex buffer.");

    UINT8* mapped = nullptr;
    CD3DX12_RANGE readRange(0, 0);

    M_ASSERT(SUCCEEDED(m_vertexBuffer->Map(
        0, &readRange, reinterpret_cast<void**>(&mapped))),
        "Failed to map vertex buffer.");

    memcpy(mapped, vertices, sizeof(vertices));
    m_vertexBuffer->Unmap(0, nullptr);

    m_vertexBufferView.BufferLocation =
        m_vertexBuffer->GetGPUVirtualAddress();

    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    m_vertexBufferView.SizeInBytes = sizeof(vertices);
}

void Mesh::CreateIndexBuffer()
{
    unsigned short indices[] = { 0, 1, 2 };
    m_indexBufferSize = sizeof(indices);

    auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto desc = CD3DX12_RESOURCE_DESC::Buffer(m_indexBufferSize);
    auto device = Renderer::GetInstance().GetDevice().Get();

    M_ASSERT(SUCCEEDED(device->CreateCommittedResource(
        &heap,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_indexBuffer))),
        "Failed to create index buffer.");

    UINT8* mapped = nullptr;
    CD3DX12_RANGE readRange(0, 0);

    M_ASSERT(SUCCEEDED(m_indexBuffer->Map(
        0, &readRange, reinterpret_cast<void**>(&mapped))),
        "Failed to map index buffer.");

    memcpy(mapped, indices, sizeof(indices));
    m_indexBuffer->Unmap(0, nullptr);

    m_indexBufferView.BufferLocation =
        m_indexBuffer->GetGPUVirtualAddress();
    m_indexBufferView.SizeInBytes = m_indexBufferSize;
    m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
}

void Mesh::Render(XMMATRIX _viewProj)
{
    GetShader()->SetPipelineState();
    GetTexture()->SetPipelineState(GetShader()->GetDescrHeap().Get());

    auto r = &Renderer::GetInstance();
    auto commands = r->GetCommands()->GetCommandList().Get();

    ConstantBuffer* data =
        &GetShader()->GetConstantData()[r->GetFrameIndex()];

    XMStoreFloat4x4(&data->WVP, XMMatrixTranspose(_viewProj));

    commands->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commands->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    commands->IASetIndexBuffer(&m_indexBufferView);
    commands->DrawIndexedInstanced(3, 1, 0, 0, 0);
}