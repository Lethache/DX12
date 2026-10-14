#pragma once

#include "StandardIncludes.h"
#include "IRenderable.h"

class Mesh : public IRenderable
{
public:
    Mesh();
    virtual ~Mesh();

    void Initialize(Shader* shader);
    void Render(XMMATRIX viewProj) override;

private:
    void CreateVertexBuffer();

    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
};