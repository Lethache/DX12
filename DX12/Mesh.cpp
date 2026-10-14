#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "CommandController.h"

Mesh::Mesh()
{
    m_vertexBufferView = {};
}

Mesh::~Mesh()
{
    delete GetTexDiffuse();
    SetTexDiffuse(nullptr);

    delete GetTexture1();
    SetTexture1(nullptr);
}

void Mesh::Initialize(Shader* shader)
{
    SetShader(shader);
    CreateVertexBuffer();

    Texture* texture0 = new Texture();
    texture0->Initialize(
        L"..\\Assets\\Textures\\Wood.dds",
        shader);

    SetTexDiffuse(texture0);

    Texture* texture1 = new Texture();
    texture1->Initialize(
        L"..\\Assets\\Textures\\Emoji.dds",
        shader);

    SetTexture1(texture1);
}

void Mesh::CreateVertexBuffer()
{
    // Position XYZ, normal XYZ, texture UV.
    const float vertices[] =
    {
        // Front
        -0.5f, -0.5f, -0.5f,  0, 0, -1,  0, 1,
        -0.5f,  0.5f, -0.5f,  0, 0, -1,  0, 0,
         0.5f,  0.5f, -0.5f,  0, 0, -1,  1, 0,
        -0.5f, -0.5f, -0.5f,  0, 0, -1,  0, 1,
         0.5f,  0.5f, -0.5f,  0, 0, -1,  1, 0,
         0.5f, -0.5f, -0.5f,  0, 0, -1,  1, 1,

         // Back
         -0.5f, -0.5f,  0.5f,  0, 0, 1,  1, 1,
          0.5f, -0.5f,  0.5f,  0, 0, 1,  0, 1,
          0.5f,  0.5f,  0.5f,  0, 0, 1,  0, 0,
         -0.5f, -0.5f,  0.5f,  0, 0, 1,  1, 1,
          0.5f,  0.5f,  0.5f,  0, 0, 1,  0, 0,
         -0.5f,  0.5f,  0.5f,  0, 0, 1,  1, 0,

         // Top
         -0.5f,  0.5f, -0.5f,  0, 1, 0,  0, 1,
         -0.5f,  0.5f,  0.5f,  0, 1, 0,  0, 0,
          0.5f,  0.5f,  0.5f,  0, 1, 0,  1, 0,
         -0.5f,  0.5f, -0.5f,  0, 1, 0,  0, 1,
          0.5f,  0.5f,  0.5f,  0, 1, 0,  1, 0,
          0.5f,  0.5f, -0.5f,  0, 1, 0,  1, 1,

          // Bottom
          -0.5f, -0.5f, -0.5f,  0, -1, 0,  1, 1,
           0.5f, -0.5f, -0.5f,  0, -1, 0,  0, 1,
           0.5f, -0.5f,  0.5f,  0, -1, 0,  0, 0,
          -0.5f, -0.5f, -0.5f,  0, -1, 0,  1, 1,
           0.5f, -0.5f,  0.5f,  0, -1, 0,  0, 0,
          -0.5f, -0.5f,  0.5f,  0, -1, 0,  1, 0,

          // Right
           0.5f, -0.5f, -0.5f,  1, 0, 0,  0, 1,
           0.5f,  0.5f, -0.5f,  1, 0, 0,  0, 0,
           0.5f,  0.5f,  0.5f,  1, 0, 0,  1, 0,
           0.5f, -0.5f, -0.5f,  1, 0, 0,  0, 1,
           0.5f,  0.5f,  0.5f,  1, 0, 0,  1, 0,
           0.5f, -0.5f,  0.5f,  1, 0, 0,  1, 1,

           // Left
           -0.5f, -0.5f, -0.5f, -1, 0, 0,  1, 1,
           -0.5f, -0.5f,  0.5f, -1, 0, 0,  0, 1,
           -0.5f,  0.5f,  0.5f, -1, 0, 0,  0, 0,
           -0.5f, -0.5f, -0.5f, -1, 0, 0,  1, 1,
           -0.5f,  0.5f,  0.5f, -1, 0, 0,  0, 0,
           -0.5f,  0.5f, -0.5f, -1, 0, 0,  1, 0
    };

    auto device = Renderer::GetInstance().GetDevice();

    const UINT bufferSize =
        static_cast<UINT>(sizeof(vertices));

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

    m_vertexBufferView.StrideInBytes = 8 * sizeof(float);
    m_vertexBufferView.SizeInBytes = bufferSize;
}

void Mesh::Render(XMMATRIX viewProj)
{
    GetShader()->SetPipelineState();

    auto renderer = &Renderer::GetInstance();

    auto commandList =
        renderer->GetCommands()->GetCommandList();

    unsigned char* base =
        reinterpret_cast<unsigned char*>(
            GetShader()->GetConstantData());

    ConstantBuffer* constantData =
        reinterpret_cast<ConstantBuffer*>(
            base +
            renderer->GetFrameIndex() *
            GetShader()->GetConstantBufferSize());

    XMStoreFloat4x4(
        &constantData->WVP,
        XMMatrixTranspose(viewProj));

    commandList->IASetPrimitiveTopology(
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandList->IASetVertexBuffers(
        0,
        1,
        &m_vertexBufferView);

    commandList->DrawInstanced(36, 1, 0, 0);
}