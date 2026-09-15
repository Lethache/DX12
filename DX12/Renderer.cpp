#include "Renderer.h"

Renderer::Renderer()
{
    m_factory = nullptr;
    m_device = nullptr;
    m_commandQueue = nullptr;
    m_swapChain = nullptr;
    m_rtvHeap = nullptr;

    for (int count = 0; count < m_frameCount; count++)
    {
        m_renderTargets[count] = nullptr;
    }

    m_commandAllocator = nullptr;
    m_commandList = nullptr;
    m_fence = nullptr;
    m_fenceEvent = nullptr;
    m_fenceValue = 0;
    m_frameIndex = 0;
    m_rtvDescriptorSize = 0;
    m_initialized = false;

    memset(
        m_bgColor,
        0,
        sizeof(float) * 4);

    m_bgColor[3] = 1.0f;
}

Renderer::~Renderer()
{
    Destroy();
}

void Renderer::ConfigurePipeline(
    bool _userWarpDevice,
    HWND _hwnd,
    int _wWidth,
    int _wHeight)
{
    UINT dxgiFactoryFlags = EnableDebugLayer();

    M_ASSERT(
        SUCCEEDED(CreateDXGIFactory2(
            dxgiFactoryFlags,
            IID_PPV_ARGS(&m_factory))),
        "Create DXGI factory failed");

    CreateDevice(_userWarpDevice);
    CreateCommandQueue();
    CreateSwapChain(
        _hwnd,
        _wWidth,
        _wHeight);

    CreateRenderTargetView();
    CreateCommands();
    CreateFence();

    // Do not support fullscreen transitions yet.
    M_ASSERT(
        SUCCEEDED(m_factory->MakeWindowAssociation(
            _hwnd,
            DXGI_MWA_NO_ALT_ENTER)),
        "Failed to create window association");

    m_initialized = true;
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

        dxgiFactoryFlags |=
            DXGI_CREATE_FACTORY_DEBUG;
    }

#endif

    return dxgiFactoryFlags;
}

void Renderer::CreateDevice(
    bool _userWarpDevice)
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

        GetHardwareAdapter(
            m_factory.Get(),
            &hardwareAdapter);

        M_ASSERT(
            hardwareAdapter != nullptr,
            "Hardware adapter was not found");

        M_ASSERT(
            SUCCEEDED(D3D12CreateDevice(
                hardwareAdapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                IID_PPV_ARGS(&m_device))),
            "Create hardware device failed");
    }
}

void Renderer::GetHardwareAdapter(
    _In_ IDXGIFactory1* _factory,
    _Outptr_result_maybenull_
    IDXGIAdapter1** _adapter,
    bool _reqHighPerfAdapter)
{
    *_adapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIFactory6> factory6;

    if (SUCCEEDED(
        _factory->QueryInterface(
            IID_PPV_ARGS(&factory6))))
    {
        DXGI_GPU_PREFERENCE gpuPreference =
            DXGI_GPU_PREFERENCE_UNSPECIFIED;

        if (_reqHighPerfAdapter)
        {
            gpuPreference =
                DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
        }

        for (
            UINT adapterIndex = 0;
            SUCCEEDED(
                factory6->EnumAdapterByGpuPreference(
                    adapterIndex,
                    gpuPreference,
                    IID_PPV_ARGS(&adapter)));
            ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc = {};
            adapter->GetDesc1(&desc);

            if (desc.Flags &
                DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                adapter.Reset();
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(
                adapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                __uuidof(ID3D12Device),
                nullptr)))
            {
                break;
            }

            adapter.Reset();
        }
    }

    if (adapter == nullptr)
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(
                _factory->EnumAdapters1(
                    adapterIndex,
                    &adapter));
                    ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc = {};
            adapter->GetDesc1(&desc);

            if (desc.Flags &
                DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                adapter.Reset();
                continue;
            }

            if (SUCCEEDED(D3D12CreateDevice(
                adapter.Get(),
                D3D_FEATURE_LEVEL_11_0,
                __uuidof(ID3D12Device),
                nullptr)))
            {
                break;
            }

            adapter.Reset();
        }
    }

    *_adapter = adapter.Detach();
}

void Renderer::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};

    queueDesc.Flags =
        D3D12_COMMAND_QUEUE_FLAG_NONE;

    queueDesc.Type =
        D3D12_COMMAND_LIST_TYPE_DIRECT;

    M_ASSERT(
        SUCCEEDED(m_device->CreateCommandQueue(
            &queueDesc,
            IID_PPV_ARGS(&m_commandQueue))),
        "Failed to create command queue");
}

void Renderer::CreateSwapChain(
    HWND _hwnd,
    int _wWidth,
    int _wHeight)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

    swapChainDesc.BufferCount =
        m_frameCount;

    swapChainDesc.Width =
        static_cast<UINT>(_wWidth);

    swapChainDesc.Height =
        static_cast<UINT>(_wHeight);

    swapChainDesc.Format =
        DXGI_FORMAT_R8G8B8A8_UNORM;

    swapChainDesc.BufferUsage =
        DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapChainDesc.SwapEffect =
        DXGI_SWAP_EFFECT_FLIP_DISCARD;

    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;

    M_ASSERT(
        SUCCEEDED(m_factory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),
            _hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain)),
        "Failed to create swap chain");

    M_ASSERT(
        SUCCEEDED(swapChain.As(&m_swapChain)),
        "Casting swapchain failed");

    m_frameIndex =
        m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::CreateRenderTargetView()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};

    rtvHeapDesc.NumDescriptors =
        m_frameCount;

    rtvHeapDesc.Type =
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    rtvHeapDesc.Flags =
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    M_ASSERT(
        SUCCEEDED(m_device->CreateDescriptorHeap(
            &rtvHeapDesc,
            IID_PPV_ARGS(&m_rtvHeap))),
        "Failed to create descriptor heap");

    m_rtvDescriptorSize =
        m_device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_rtvHeap->
        GetCPUDescriptorHandleForHeapStart());

    for (UINT n = 0; n < m_frameCount; n++)
    {
        M_ASSERT(
            SUCCEEDED(m_swapChain->GetBuffer(
                n,
                IID_PPV_ARGS(&m_renderTargets[n]))),
            "Swapchain get back buffer failed");

        m_device->CreateRenderTargetView(
            m_renderTargets[n].Get(),
            nullptr,
            rtvHandle);

        rtvHandle.Offset(
            1,
            m_rtvDescriptorSize);
    }
}

void Renderer::CreateCommands()
{
    M_ASSERT(
        SUCCEEDED(m_device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&m_commandAllocator))),
        "Failed to create command allocator");

    M_ASSERT(
        SUCCEEDED(m_device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_commandAllocator.Get(),
            nullptr,
            IID_PPV_ARGS(&m_commandList))),
        "Failed to create command list");

    // Command lists are created in the recording state.
    // The main loop expects the list to be closed.
    M_ASSERT(
        SUCCEEDED(m_commandList->Close()),
        "Failed to close command list");
}

void Renderer::CreateFence()
{
    M_ASSERT(
        SUCCEEDED(m_device->CreateFence(
            0,
            D3D12_FENCE_FLAG_NONE,
            IID_PPV_ARGS(&m_fence))),
        "Failed to create fence");

    m_fenceValue = 1;

    m_fenceEvent = CreateEvent(
        nullptr,
        FALSE,
        FALSE,
        nullptr);

    M_ASSERT(
        m_fenceEvent != nullptr,
        "Failed to create fence event");
}

void Renderer::Render()
{
    if (!m_initialized)
    {
        return;
    }

    PopulateCommandList();

    ID3D12CommandList* commandLists[] =
    {
        m_commandList.Get()
    };

    m_commandQueue->ExecuteCommandLists(
        _countof(commandLists),
        commandLists);

    M_ASSERT(
        SUCCEEDED(m_swapChain->Present(
            1,
            0)),
        "Failed to present swapchain.");

    WaitForPreviousFrame();
}

void Renderer::Destroy()
{
    if (!m_initialized)
    {
        return;
    }

    WaitForPreviousFrame();

    if (m_fenceEvent != nullptr)
    {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }

    m_initialized = false;
}

void Renderer::PopulateCommandList()
{
    M_ASSERT(
        SUCCEEDED(m_commandAllocator->Reset()),
        "Failed to reset command allocator.");

    M_ASSERT(
        SUCCEEDED(m_commandList->Reset(
            m_commandAllocator.Get(),
            nullptr)),
        "Failed to reset command list.");

    CD3DX12_RESOURCE_BARRIER barrier =
        CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);

    m_commandList->ResourceBarrier(
        1,
        &barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_rtvHeap->
        GetCPUDescriptorHandleForHeapStart(),
        m_frameIndex,
        m_rtvDescriptorSize);

    m_commandList->OMSetRenderTargets(
        1,
        &rtvHandle,
        FALSE,
        nullptr);

    // The background colour comes from ToolsForm.
    m_commandList->ClearRenderTargetView(
        rtvHandle,
        m_bgColor,
        0,
        nullptr);

    barrier =
        CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);

    m_commandList->ResourceBarrier(
        1,
        &barrier);

    M_ASSERT(
        SUCCEEDED(m_commandList->Close()),
        "Failed to close command list.");
}

void Renderer::WaitForPreviousFrame()
{
    const UINT64 fence =
        m_fenceValue;

    M_ASSERT(
        SUCCEEDED(m_commandQueue->Signal(
            m_fence.Get(),
            fence)),
        "Failed to signal fence.");

    m_fenceValue++;

    if (m_fence->GetCompletedValue() < fence)
    {
        M_ASSERT(
            SUCCEEDED(m_fence->SetEventOnCompletion(
                fence,
                m_fenceEvent)),
            "Failed to set fence complete.");

        WaitForSingleObject(
            m_fenceEvent,
            INFINITE);
    }

    m_frameIndex =
        m_swapChain->
        GetCurrentBackBufferIndex();
}