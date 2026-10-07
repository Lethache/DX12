#include "DDSLoad.h"

namespace
{
    constexpr UINT MakeFourCC(char a, char b, char c, char d)
    {
        return static_cast<UINT>(a)
            | (static_cast<UINT>(b) << 8)
            | (static_cast<UINT>(c) << 16)
            | (static_cast<UINT>(d) << 24);
    }
}

void DDSLoad::ReadDataFromFile(const std::wstring& filename)
{
    HANDLE file = CreateFileW(
        filename.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);

    M_ASSERT(
        file != INVALID_HANDLE_VALUE,
        "Failed to open DDS file. Check the path.");

    LARGE_INTEGER fileSize = {};

    if (!GetFileSizeEx(file, &fileSize))
    {
        CloseHandle(file);
        M_ASSERT(false, "Failed to get DDS file size.");
        return;
    }

    if (fileSize.QuadPart < 128 ||
        fileSize.QuadPart > MAXDWORD)
    {
        CloseHandle(file);
        M_ASSERT(false, "Invalid DDS file size.");
        return;
    }

    m_data.resize(static_cast<size_t>(fileSize.QuadPart));

    DWORD bytesRead = 0;

    BOOL result = ReadFile(
        file,
        m_data.data(),
        static_cast<DWORD>(m_data.size()),
        &bytesRead,
        nullptr);

    CloseHandle(file);

    M_ASSERT(
        result && bytesRead == m_data.size(),
        "Failed to read DDS file.");
}

DXGI_FORMAT DDSLoad::ConvertFourCCToDXGI(UINT fourCC)
{
    switch (fourCC)
    {
    case MakeFourCC('D', 'X', 'T', '1'):
        return DXGI_FORMAT_BC1_UNORM;

    case MakeFourCC('D', 'X', 'T', '3'):
        return DXGI_FORMAT_BC2_UNORM;

    case MakeFourCC('D', 'X', 'T', '5'):
        return DXGI_FORMAT_BC3_UNORM;

    case MakeFourCC('A', 'T', 'I', '1'):
    case MakeFourCC('B', 'C', '4', 'U'):
        return DXGI_FORMAT_BC4_UNORM;

    case MakeFourCC('A', 'T', 'I', '2'):
    case MakeFourCC('B', 'C', '5', 'U'):
        return DXGI_FORMAT_BC5_UNORM;

    default:
        return DXGI_FORMAT_UNKNOWN;
    }
}

void DDSLoad::Load(const std::wstring& filename)
{
    ReadDataFromFile(filename);

    UINT magic = 0;
    memcpy(&magic, m_data.data(), sizeof(magic));

    M_ASSERT(
        magic == 0x20534444,
        "Invalid DDS signature.");

    memcpy(
        &m_header,
        m_data.data() + sizeof(UINT),
        sizeof(m_header));

    M_ASSERT(
        m_header.size == sizeof(DDS_HEADER) &&
        m_header.ddsPixelFormat.size == sizeof(DDS_PIXELFORMAT),
        "Invalid DDS header.");

    M_ASSERT(
        m_header.width > 0 && m_header.height > 0,
        "Invalid DDS dimensions.");

    // This loader handles compressed 2D textures.
    M_ASSERT(
        (m_header.ddsPixelFormat.flags & 0x4) != 0,
        "DDS must contain a FourCC format.");

    M_ASSERT(
        (m_header.caps2 & (0x200 | 0x200000)) == 0 &&
        m_header.depth <= 1,
        "Cubemap and volume DDS textures are not supported.");

    m_format = ConvertFourCCToDXGI(
        m_header.ddsPixelFormat.fourCC);

    M_ASSERT(
        m_format != DXGI_FORMAT_UNKNOWN,
        "Unsupported DDS format.");

    m_offset = sizeof(UINT) + sizeof(DDS_HEADER);
}