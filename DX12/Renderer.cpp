#include "Renderer.h"

Renderer::Renderer()
{
    m_factory = nullptr;
    m_device = nullptr;
}

Renderer::~Renderer()
{
}

void Renderer::ConfigurePipeline(bool _userWarpDevice)
{
    UINT dxgiFactoryFlags = EnableDebugLayer();

    M_ASSERT(
        SUCCEEDED(CreateDXGIFactory2(
            dxgiFactoryFlags,
            IID_PPV_ARGS(&m_factory))),
        "Create DXGI factory failed");

    CreateDevice(_userWarpDevice);
}

UINT Renderer::EnableDebugLayer()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    ComPtr<ID3D12Debug> debugController;

    if (SUCCEEDED(D3D12GetDebugInterface(
        IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        // Enable additional debug layers.
    }
#endif

    return dxgiFactoryFlags;
}

void Renderer::CreateDevice(bool _userWarpDevice)
{
    if (_userWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;

        M_ASSERT(
            SUCCEEDED(m_factory->EnumWarpAdapter(
                IID_PPV_ARGS(&warpAdapter))),
            "Enum warp adapter failed");

        M_ASSERT(
            SUCCEEDED(D3D12CreateDevice(
                warpAdapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&m_device))),
            "Create warp device failed");
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;

        GetHardwareAdapter(m_factory.Get(), &hardwareAdapter);

        M_ASSERT(
            SUCCEEDED(D3D12CreateDevice(
                hardwareAdapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&m_device))),
            "Credte hardware device failed");
    }
}

void Renderer::GetHardwareAdapter(
    _In_ IDXGIFactory1* _factory,
    _Outptr_result_maybenull_ IDXGIAdapter1** _adapter,
    bool _reqHighPerfAdapter)
{
    *_adapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIFactory6> factory6;

    if (SUCCEEDED(
        _factory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        DXGI_GPU_PREFERENCE gpuPref =
            DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;

        if (_reqHighPerfAdapter)
            gpuPref = DXGI_GPU_PREFERENCE_UNSPECIFIED;

        for (
            UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                gpuPref,
                IID_PPV_ARGS(&adapter)));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            if (SUCCEEDED(D3D12CreateDevice(
                adapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                __uuidof(ID3D12Device),
                nullptr)))
                break;
        }
    }

    if (adapter.Get() == nullptr)
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(_factory->EnumAdapters1(
                adapterIndex,
                &adapter));
                ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            if (SUCCEEDED(D3D12CreateDevice(
                adapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                __uuidof(ID3D12Device),
                nullptr)))
                break;
        }
    }

    *_adapter = adapter.Detach();
}