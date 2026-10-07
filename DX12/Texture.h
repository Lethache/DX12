#pragma once

#include "StandardIncludes.h"
#include "DDSLoad.h"

class Shader;

class Texture
{
public:
    Texture();
    ~Texture();

    void Initialize(
        const std::wstring& filename,
        Shader* shader);

private:
    void LoadTextureFromFile(const std::wstring& filename);
    void CreateTexture();
    void CreateShaderResourceView(Shader* shader);
    void UploadTexture();

    DDSLoad* m_ddsLoader;
    ComPtr<ID3D12Resource> m_texture;
};