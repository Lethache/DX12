#include "Renderer.h"
#include "Shader.h"
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

    m_viewport = {};
    m_scissorRect = {};
}

Renderer::~Renderer()
{
    Destroy();
}

void Renderer::ConfigurePipeline(
    bool _userWarpDevice,
    HWND _hwnd,
    int _windowWidth,
    int _windowHeight)
{
    UINT dxgiFactoryFlags =
        EnableDebugLayer();

    M_ASSERT(
        SUCCEEDED(
            CreateDXGIFactory2(
                dxgiFactoryFlags,
                IID_PPV_ARGS(&m_factory))),
        "Create DXGI factory failed.");

    CreateDevice(_userWarpDevice);

    CreateCommandQueue();

    CreateSwapChain(
        _hwnd,
        _windowWidth,
        _windowHeight);

    CreateRenderTargetView();

    CreateCommands();

    CreateFence();

    CreateViewport(
        _windowWidth,
        _windowHeight);

    M_ASSERT(
        SUCCEEDED(
            m_factory->MakeWindowAssociation(
                _hwnd,
                DXGI_MWA_NO_ALT_ENTER)),
        "Failed to create window association.");

    m_initialized = true;
}

UINT Renderer::EnableDebugLayer()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)

    ComPtr<ID3D12Debug> debugController;

    if (SUCCEEDED(
        D3D12GetDebugInterface(
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
            SUCCEEDED(
                m_factory->EnumWarpAdapter(
                    IID_PPV_ARGS(&warpAdapter))),
            "Enum WARP adapter failed.");

        M_ASSERT(
            SUCCEEDED(
                D3D12CreateDevice(
                    warpAdapter.Get(),
                    D3D_FEATURE_LEVEL_11_0,
                    IID_PPV_ARGS(&m_device))),
            "Create WARP device failed.");
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;

        GetHardwareAdapter(
            m_factory.Get(),
            &hardwareAdapter);

        M_ASSERT(
            hardwareAdapter != nullptr,
            "Hardware adapter was not found.");

        M_ASSERT(
            SUCCEEDED(
                D3D12CreateDevice(
                    hardwareAdapter.Get(),
                    D3D_FEATURE_LEVEL_11_0,
                    IID_PPV_ARGS(&m_device))),
            "Create hardware device failed.");
    }
}

void Renderer::GetHardwareAdapter(
    _In_ IDXGIFactory1* _factory,
    _Outptr_result_maybenull_ IDXGIAdapter1** _adapter,
    bool _requireHighPerformanceAdapter)
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

        if (_requireHighPerformanceAdapter)
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
            DXGI_ADAPTER_DESC1 description = {};

            adapter->GetDesc1(&description);

            if (description.Flags &
                DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                adapter.Reset();
                continue;
            }

            if (SUCCEEDED(
                D3D12CreateDevice(
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
            DXGI_ADAPTER_DESC1 description = {};

            adapter->GetDesc1(&description);

            if (description.Flags &
                DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                adapter.Reset();
                continue;
            }

            if (SUCCEEDED(
                D3D12CreateDevice(
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
    D3D12_COMMAND_QUEUE_DESC queueDescription = {};

    queueDescription.Flags =
        D3D12_COMMAND_QUEUE_FLAG_NONE;

    queueDescription.Type =
        D3D12_COMMAND_LIST_TYPE_DIRECT;

    M_ASSERT(
        SUCCEEDED(
            m_device->CreateCommandQueue(
                &queueDescription,
                IID_PPV_ARGS(&m_commandQueue))),
        "Failed to create command queue.");
}

void Renderer::CreateSwapChain(
    HWND _hwnd,
    int _windowWidth,
    int _windowHeight)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDescription = {};

    swapChainDescription.BufferCount =
        m_frameCount;

    swapChainDescription.Width =
        static_cast<UINT>(_windowWidth);

    swapChainDescription.Height =
        static_cast<UINT>(_windowHeight);

    swapChainDescription.Format =
        DXGI_FORMAT_R8G8B8A8_UNORM;

    swapChainDescription.BufferUsage =
        DXGI_USAGE_RENDER_TARGET_OUTPUT;

    swapChainDescription.SwapEffect =
        DXGI_SWAP_EFFECT_FLIP_DISCARD;

    swapChainDescription.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;

    M_ASSERT(
        SUCCEEDED(
            m_factory->CreateSwapChainForHwnd(
                m_commandQueue.Get(),
                _hwnd,
                &swapChainDescription,
                nullptr,
                nullptr,
                &swapChain)),
        "Failed to create swap chain.");

    M_ASSERT(
        SUCCEEDED(
            swapChain.As(&m_swapChain)),
        "Casting swap chain failed.");

    m_frameIndex =
        m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::CreateRenderTargetView()
{
    D3D12_DESCRIPTOR_HEAP_DESC
        renderTargetHeapDescription = {};

    renderTargetHeapDescription.NumDescriptors =
        m_frameCount;

    renderTargetHeapDescription.Type =
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    renderTargetHeapDescription.Flags =
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    M_ASSERT(
        SUCCEEDED(
            m_device->CreateDescriptorHeap(
                &renderTargetHeapDescription,
                IID_PPV_ARGS(&m_rtvHeap))),
        "Failed to create descriptor heap.");

    m_rtvDescriptorSize =
        m_device->GetDescriptorHandleIncrementSize(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE
        renderTargetView(
            m_rtvHeap->
            GetCPUDescriptorHandleForHeapStart());

    for (UINT index = 0;
        index < m_frameCount;
        index++)
    {
        M_ASSERT(
            SUCCEEDED(
                m_swapChain->GetBuffer(
                    index,
                    IID_PPV_ARGS(
                        &m_renderTargets[index]))),
            "Swap chain get back buffer failed.");

        m_device->CreateRenderTargetView(
            m_renderTargets[index].Get(),
            nullptr,
            renderTargetView);

        renderTargetView.Offset(
            1,
            m_rtvDescriptorSize);
    }
}

void Renderer::CreateCommands()
{
    M_ASSERT(
        SUCCEEDED(
            m_device->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(
                    &m_commandAllocator))),
        "Failed to create command allocator.");

    M_ASSERT(
        SUCCEEDED(
            m_device->CreateCommandList(
                0,
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                m_commandAllocator.Get(),
                nullptr,
                IID_PPV_ARGS(&m_commandList))),
        "Failed to create command list.");

    M_ASSERT(
        SUCCEEDED(
            m_commandList->Close()),
        "Failed to close command list.");
}

void Renderer::CreateFence()
{
    M_ASSERT(
        SUCCEEDED(
            m_device->CreateFence(
                0,
                D3D12_FENCE_FLAG_NONE,
                IID_PPV_ARGS(&m_fence))),
        "Failed to create fence.");

    m_fenceValue = 1;

    m_fenceEvent = CreateEvent(
        nullptr,
        FALSE,
        FALSE,
        nullptr);

    M_ASSERT(
        m_fenceEvent != nullptr,
        "Failed to create fence event.");
}

void Renderer::CreateViewport(
    int _windowWidth,
    int _windowHeight)
{
    m_viewport = CD3DX12_VIEWPORT(
        0.0f,
        0.0f,
        static_cast<float>(_windowWidth),
        static_cast<float>(_windowHeight));

    m_scissorRect = CD3DX12_RECT(
        0,
        0,
        static_cast<LONG>(_windowWidth),
        static_cast<LONG>(_windowHeight));
}

void Renderer::Render(
    IRenderable* _renderable)
{
    if (!m_initialized)
    {
        return;
    }

    PopulateCommandList(_renderable);

    ID3D12CommandList* commandLists[] =
    {
        m_commandList.Get()
    };

    m_commandQueue->ExecuteCommandLists(
        _countof(commandLists),
        commandLists);

    M_ASSERT(
        SUCCEEDED(
            m_swapChain->Present(
                1,
                0)),
        "Failed to present swap chain.");

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

void Renderer::PopulateCommandList(
    IRenderable* _renderable)
{
    M_ASSERT(
        SUCCEEDED(
            m_commandAllocator->Reset()),
        "Failed to reset command allocator.");

    M_ASSERT(
        SUCCEEDED(
            m_commandList->Reset(
                m_commandAllocator.Get(),
                _renderable
                ->GetShader()
                ->GetPipelineState()
                .Get())),
        "Failed to reset command list.");

    auto barrier =
        CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(),
            D3D12_RESOURCE_STATE_PRESENT,
            D3D12_RESOURCE_STATE_RENDER_TARGET);

    m_commandList->ResourceBarrier(
        1,
        &barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE
        renderTargetView(
            m_rtvHeap->
            GetCPUDescriptorHandleForHeapStart(),
            m_frameIndex,
            m_rtvDescriptorSize);

    m_commandList->OMSetRenderTargets(
        1,
        &renderTargetView,
        FALSE,
        nullptr);

    m_commandList->RSSetViewports(
        1,
        &m_viewport);

    m_commandList->RSSetScissorRects(
        1,
        &m_scissorRect);

    _renderable
        ->GetShader()
        ->SetPipelineState(
            m_commandList.Get());

    m_commandList->ClearRenderTargetView(
        renderTargetView,
        m_bgColor,
        0,
        nullptr);

    _renderable->Render(
        m_commandList.Get());

    barrier =
        CD3DX12_RESOURCE_BARRIER::Transition(
            m_renderTargets[m_frameIndex].Get(),
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            D3D12_RESOURCE_STATE_PRESENT);

    m_commandList->ResourceBarrier(
        1,
        &barrier);

    M_ASSERT(
        SUCCEEDED(
            m_commandList->Close()),
        "Failed to close command list.");
}

void Renderer::WaitForPreviousFrame()
{
    const UINT64 fence =
        m_fenceValue;

    M_ASSERT(
        SUCCEEDED(
            m_commandQueue->Signal(
                m_fence.Get(),
                fence)),
        "Failed to signal fence.");

    m_fenceValue++;

    if (m_fence->GetCompletedValue() < fence)
    {
        M_ASSERT(
            SUCCEEDED(
                m_fence->SetEventOnCompletion(
                    fence,
                    m_fenceEvent)),
            "Failed to set fence completion event.");

        WaitForSingleObject(
            m_fenceEvent,
            INFINITE);
    }

    m_frameIndex =
        m_swapChain->
        GetCurrentBackBufferIndex();
}