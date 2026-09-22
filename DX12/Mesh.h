
#pragma once

#include "StandardIncludes.h"

class Shader;

class Mesh : public IRenderable
{
public:
    Mesh();
    virtual ~Mesh() {}

    void Initialize(
        ID3D12Device* _device,
        Shader* _shader);

    virtual void Render(
        ID3D12GraphicsCommandList* _commandList);

private:
    void CreateVertexBuffer(ID3D12Device* _device);
    void CreateIndexBuffer(ID3D12Device* _device);

    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    UINT m_indexBufferSize;
    ComPtr<ID3D12Resource> m_indexBuffer;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
};