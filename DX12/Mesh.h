#pragma once

#include "StandardIncludes.h"
#include "IRenderable.h"

class Mesh : public IRenderable
{
public:
    Mesh();
    virtual ~Mesh();

    void Initialize(
        Shader* shader,
        const wchar_t* texturePath =
        L"..\\Assets\\Textures\\Wood.dds",
        const wchar_t* secondTexturePath =
        L"..\\Assets\\Textures\\Emoji.dds");

    void Render(XMMATRIX viewProj) override;

private:
    void CreateVertexBuffer();

    ComPtr<ID3D12Resource> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
};