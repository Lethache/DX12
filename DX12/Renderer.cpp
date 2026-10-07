#include "Renderer.h"
#include "CommandController.h"
#include "IRenderable.h"
#include "Shader.h"

Renderer::Renderer()
{
    m_commands = nullptr;
    m_viewport = {};
    m_scissorRect = {};
    m_frameIndex = 0;
    m_rtvDescriptorSize = 0;
    m_fenceValue = 0;
    m_fenceEvent = nullptr;
    m_initialized = false;

    m_bgColor[0] = 1.0f;
    m_bgColor[1] = 0.0f;
    m_bgColor[2] = 0.0f;
    m_bgColor[3] = 1.0f;
}

Renderer::~Renderer()
{
    Destroy();
}

UINT Renderer::EnableDebugLayer()
{
    UINT flags = 0;

#if defined(_DEBUG)
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(
        IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
        flags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    return flags;
}

void Renderer::GetHardwareAdapter(
    IDXGIFactory1* _factory,
    IDXGIAdapter1** _adapter,
    bool _requestHighPerformanceAdapter)
{
    *_adapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIFactory6> factory6;

    if (SUCCEEDED(_factory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        DXGI_GPU_PREFERENCE preference =
            _requestHighPerformanceAdapter
            ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
            : DXGI_GPU_PREFERENCE_UNSPECIFIED;

        for (UINT index = 0;
            factory6->EnumAdapterByGpuPreference(
                index, preference, IID_PPV_ARGS(&adapter))
            != DXGI_ERROR_NOT_FOUND;
            ++index)
        {
            DXGI_ADAPTER_DESC1 desc = {};
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            if (SUCCEEDED(D3D12CreateDevice(
                adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                __uuidof(ID3D12Device), nullptr)))
            {
                *_adapter = adapter.Detach();
                return;
            }
        }
    }

    for (UINT index = 0;
        _factory->EnumAdapters1(index, &adapter)
        != DXGI_ERROR_NOT_FOUND;
        ++index)
    {
        DXGI_ADAPTER_DESC1 desc = {};
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        if (SUCCEEDED(D3D12CreateDevice(
            adapter.Get(), D3D_FEATURE_LEVEL_11_0,
            __uuidof(ID3D12Device), nullptr)))
        {
            *_adapter = adapter.Detach();
            return;
        }
    }
}

void Renderer::CreateDevice(bool _useWarpDevice)
{
    if (_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> adapter;

        M_ASSERT(SUCCEEDED(m_factory->EnumWarpAdapter(
            IID_PPV_ARGS(&adapter))),
            "Failed to find WARP adapter.");

        M_ASSERT(SUCCEEDED(D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device))),
            "Failed to create WARP device.");
    }
    else
    {
        ComPtr<IDXGIAdapter1> adapter;
        GetHardwareAdapter(m_factory.Get(), &adapter, true);

        M_ASSERT(adapter != nullptr,
            "No compatible DirectX 12 adapter.");

        M_ASSERT(SUCCEEDED(D3D12CreateDevice(
            adapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device))),
            "Failed to create DirectX 12 device.");
    }
}

void Renderer::ConfigurePipeline(
    bool _useWarpDevice,
    HWND _hwnd,
    int _wWidth,
    int _wHeight)
{
    UINT flags = EnableDebugLayer();

    M_ASSERT(SUCCEEDED(CreateDXGIFactory2(
        flags, IID_PPV_ARGS(&m_factory))),
        "Failed to create DXGI factory.");

    CreateDevice(_useWarpDevice);

    m_commands = new CommandController();
    m_commands->Create(m_device.Get());

    CreateSwapChain(_hwnd, _wWidth, _wHeight);
    CreateRenderTargetView();
    CreateFence();
    CreateViewport(_wWidth, _wHeight);

    m_factory->MakeWindowAssociation(_hwnd, DXGI_MWA_NO_ALT_ENTER);
    m_initialized = true;
}

void Renderer::CreateSwapChain(HWND _hwnd, int _wWidth, int _wHeight)
{
    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.BufferCount = m_frameCount;
    desc.Width = _wWidth;
    desc.Height = _wHeight;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;

    M_ASSERT(SUCCEEDED(m_factory->CreateSwapChainForHwnd(
        m_commands->GetCommandQueue().Get(),
        _hwnd, &desc, nullptr, nullptr, &swapChain)),
        "Failed to create swap chain.");

    M_ASSERT(SUCCEEDED(swapChain.As(&m_swapChain)),
        "Failed to get IDXGISwapChain3.");

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::CreateRenderTargetView()
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.NumDescriptors = m_frameCount;
    desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

    M_ASSERT(SUCCEEDED(m_device->CreateDescriptorHeap(
        &desc, IID_PPV_ARGS(&m_rtvHeap))),
        "Failed to create RTV heap.");

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (UINT index = 0; index < m_frameCount; ++index)
    {
        M_ASSERT(SUCCEEDED(m_swapChain->GetBuffer(
            index, IID_PPV_ARGS(&m_renderTargets[index]))),
            "Failed to get swap chain buffer.");

        m_device->CreateRenderTargetView(
            m_renderTargets[index].Get(), nullptr, handle);

        handle.Offset(1, m_rtvDescriptorSize);
    }
}

void Renderer::CreateFence()
{
    M_ASSERT(SUCCEEDED(m_device->CreateFence(
        0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))),
        "Failed to create fence.");

    m_fenceValue = 1;
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    M_ASSERT(m_fenceEvent != nullptr,
        "Failed to create fence event.");
}

void Renderer::CreateViewport(int _wWidth, int _wHeight)
{
    m_viewport = CD3DX12_VIEWPORT(
        0.0f, 0.0f,
        static_cast<float>(_wWidth),
        static_cast<float>(_wHeight));

    m_scissorRect = CD3DX12_RECT(0, 0, _wWidth, _wHeight);
}

void Renderer::Render(IRenderable* _renderable, XMMATRIX _viewProj)
{
    if (!m_initialized || !_renderable)
        return;

    PopulateCommandList(_renderable, _viewProj);
    m_commands->Execute();

    M_ASSERT(SUCCEEDED(m_swapChain->Present(1, 0)),
        "Failed to present swap chain.");

    WaitForPreviousFrame();
}

void Renderer::PopulateCommandList(
    IRenderable* _renderable, XMMATRIX _viewProj)
{
    m_commands->Reset(
        _renderable->GetShader()->GetPipelineState().Get());

    m_commands->AddBarrier(
        m_renderTargets[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        m_frameIndex, m_rtvDescriptorSize);

    auto commands = m_commands->GetCommandList().Get();

    commands->OMSetRenderTargets(1, &handle, FALSE, nullptr);
    commands->RSSetViewports(1, &m_viewport);
    commands->RSSetScissorRects(1, &m_scissorRect);
    commands->ClearRenderTargetView(handle, m_bgColor, 0, nullptr);

    _renderable->Render(_viewProj);

    m_commands->AddBarrier(
        m_renderTargets[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);

    m_commands->Close();
}

void Renderer::WaitForPreviousFrame()
{
    if (!m_commands || !m_fence || !m_fenceEvent)
        return;

    const UINT64 fence = m_fenceValue++;

    M_ASSERT(SUCCEEDED(m_commands->GetCommandQueue()->Signal(
        m_fence.Get(), fence)),
        "Failed to signal fence.");

    if (m_fence->GetCompletedValue() < fence)
    {
        M_ASSERT(SUCCEEDED(m_fence->SetEventOnCompletion(
            fence, m_fenceEvent)),
            "Failed to set fence event.");

        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    if (m_swapChain)
        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::Destroy()
{
    WaitForPreviousFrame();
    m_initialized = false;

    if (m_fenceEvent)
    {
        CloseHandle(m_fenceEvent);
        m_fenceEvent = nullptr;
    }

    delete m_commands;
    m_commands = nullptr;
}