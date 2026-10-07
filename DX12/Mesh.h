#pragma once

#include "StandardIncludes.h"
#include "IRenderable.h"

class Mesh : public IRenderable
{
public:
    Mesh();
    virtual ~Mesh();

    void Initialize(Shader* _shader);
    void Render(XMMATRIX _viewProj) override;

private:
    void CreateVertexBuffer();
    void CreateIndexBuffer();

    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    UINT m_indexBufferSize;
    ComPtr<ID3D12Resource> m_indexBuffer;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
};