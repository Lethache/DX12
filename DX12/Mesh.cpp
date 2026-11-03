#include "Mesh.h"
#include "Shader.h"
#include "Renderer.h"
#include "CommandController.h"

Mesh::Mesh()
{
    m_objectIndex = 0;
    m_vertexBufferView = {};
}

Mesh::~Mesh()
{
    // Shared textures are owned by GameController::Run.
}

void Mesh::Initialize(Shader* shader)
{
    M_ASSERT(shader != nullptr, "Mesh requires a shader.");

    SetShader(shader);

    // Unique index among objects using this shader.
    m_objectIndex = shader->RegisterObject();

    CreateVertexBuffer();
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
    auto* shader = GetShader();

    shader->SetCurrentObjectIndex(m_objectIndex);
    shader->SetPipelineState();

    // ShaderPhong has already written material, light and world data.
    // Write WVP into the same object's slot.
    auto* constantData =
        reinterpret_cast<ConstantBuffer*>(
            shader->ConstantBufferAddress());

    XMStoreFloat4x4(
        &constantData->WVP,
        XMMatrixTranspose(viewProj));

    auto commandList =
        Renderer::GetInstance()
        .GetCommands()->GetCommandList();

    commandList->IASetPrimitiveTopology(
        D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    commandList->IASetVertexBuffers(
        0,
        1,
        &m_vertexBufferView);

    commandList->DrawInstanced(36, 1, 0, 0);
}