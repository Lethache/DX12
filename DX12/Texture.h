#pragma once

#include "StandardIncludes.h"
#include "DDSLoad.h"

class Texture
{
public:
    Texture();
    ~Texture();

    void Initialize(
        const std::wstring& filename,
        ID3D12DescriptorHeap* descriptorHeap);

    void SetPipelineState(
        ID3D12DescriptorHeap* descriptorHeap);

private:
    void LoadTextureFromFile(const std::wstring& filename);
    void CreateTexture();

    void CreateShaderResourceView(
        ID3D12DescriptorHeap* descriptorHeap);

    void UploadTexture();

    DDSLoad* m_ddsLoader;
    ComPtr<ID3D12Resource> m_texture;
};