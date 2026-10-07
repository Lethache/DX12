#pragma once

#include "StandardIncludes.h"
#include <string>
#include <vector>

struct DDS_PIXELFORMAT
{
    UINT size;
    UINT flags;
    UINT fourCC;
    UINT rgbBitCount;
    UINT rBitMask;
    UINT gBitMask;
    UINT bBitMask;
    UINT aBitMask;
};

struct DDS_HEADER
{
    UINT size;
    UINT flags;
    UINT height;
    UINT width;
    UINT pitchOrLinearSize;
    UINT depth;
    UINT mipMapCount;
    UINT reserved1[11];

    DDS_PIXELFORMAT ddsPixelFormat;

    UINT caps;
    UINT caps2;
    UINT caps3;
    UINT caps4;
    UINT reserved2;
};

class DDSLoad
{
public:
    DDSLoad() = default;
    ~DDSLoad() = default;

    const DDS_HEADER* GetHeader() const
    {
        return &m_header;
    }

    const unsigned char* GetData() const
    {
        return m_data.data() + m_offset;
    }

    DXGI_FORMAT GetFormat() const
    {
        return m_format;
    }

    size_t GetSize() const
    {
        return m_data.size() - m_offset;
    }

    void Load(const std::wstring& filename);

private:
    void ReadDataFromFile(const std::wstring& filename);
    DXGI_FORMAT ConvertFourCCToDXGI(UINT fourCC);

    DDS_HEADER m_header = {};
    DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;

    std::vector<unsigned char> m_data;
    size_t m_offset = 128;
};

static_assert(sizeof(DDS_PIXELFORMAT) == 32,
    "Invalid DDS_PIXELFORMAT size");

static_assert(sizeof(DDS_HEADER) == 124,
    "Invalid DDS_HEADER size");