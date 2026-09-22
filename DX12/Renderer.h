#pragma once

#include "StandardIncludes.h"

class Renderer : public Singleton<Renderer>
{
public:
    // Constructors / Destructors
    Renderer();
    virtual ~Renderer();

    // Accessors
    ComPtr<ID3D12Device>& GetDevice()
    {
        return m_device;
    }

    ComPtr<IDXGISwapChain3>& GetSwapChain()
    {
        return m_swapChain;
    }

    ComPtr<ID3D12DescriptorHeap>& GetRTVHeap()
    {
        return m_rtvHeap;
    }

    float* GetBGColor()
    {
        return m_bgColor;
    }

    // Methods
    void ConfigurePipeline(
        bool _userWarpDevice,
        HWND _hwnd,
        int _windowWidth,
        int _windowHeight);

    void Render(IRenderable* _renderable);

    void Destroy();

private:
    // Methods
    UINT EnableDebugLayer();

    void CreateDevice(bool _userWarpDevice);

    void GetHardwareAdapter(
        _In_ IDXGIFactory1* _factory,
        _Outptr_result_maybenull_ IDXGIAdapter1** _adapter,
        bool _requireHighPerformanceAdapter = false);

    void CreateCommandQueue();

    void CreateSwapChain(
        HWND _hwnd,
        int _windowWidth,
        int _windowHeight);

    void CreateRenderTargetView();

    void CreateCommands();

    void CreateFence();

    void CreateViewport(
        int _windowWidth,
        int _windowHeight);

    void PopulateCommandList(
        IRenderable* _renderable);

    void WaitForPreviousFrame();

    // Members
    static const UINT m_frameCount = 2;

    ComPtr<IDXGIFactory4> m_factory;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;

    ComPtr<ID3D12Resource>
        m_renderTargets[m_frameCount];

    ComPtr<ID3D12CommandAllocator>
        m_commandAllocator;

    ComPtr<ID3D12GraphicsCommandList>
        m_commandList;

    ComPtr<ID3D12Fence> m_fence;

    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;

    UINT m_frameIndex;
    UINT m_rtvDescriptorSize;
    UINT64 m_fenceValue;

    HANDLE m_fenceEvent;

    bool m_initialized;

    float m_bgColor[4];
};