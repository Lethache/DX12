#pragma once

#include "StandardIncludes.h"

class Renderer : public Singleton<Renderer>
{
public:
    Renderer();
    virtual ~Renderer();

    static UINT GetFrameCount() { return m_frameCount; }
    UINT GetFrameIndex() const { return m_frameIndex; }

    ComPtr<ID3D12Device>& GetDevice() { return m_device; }
    ComPtr<IDXGISwapChain3>& GetSwapChain() { return m_swapChain; }
    ComPtr<ID3D12DescriptorHeap>& GetRTVHeap() { return m_rtvHeap; }

    CommandController* GetCommands() { return m_commands; }
    float* GetBGColor() { return m_bgColor; }

    void ConfigurePipeline(
        bool _useWarpDevice,
        HWND _hwnd,
        int _wWidth,
        int _wHeight);

    void Render(IRenderable* _renderable, XMMATRIX _viewProj);
    void Destroy();
    void WaitForPreviousFrame();

private:
    UINT EnableDebugLayer();
    void CreateDevice(bool _useWarpDevice);

    void GetHardwareAdapter(
        IDXGIFactory1* _factory,
        IDXGIAdapter1** _adapter,
        bool _requestHighPerformanceAdapter = false);

    void CreateSwapChain(HWND _hwnd, int _wWidth, int _wHeight);
    void CreateRenderTargetView();
    void CreateFence();
    void CreateViewport(int _wWidth, int _wHeight);
    void PopulateCommandList(IRenderable* _renderable, XMMATRIX _viewProj);

    static const UINT m_frameCount = 2;

    ComPtr<IDXGIFactory4> m_factory;
    ComPtr<ID3D12Device> m_device;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12Resource> m_renderTargets[m_frameCount];
    ComPtr<ID3D12Fence> m_fence;

    CommandController* m_commands;
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;

    UINT m_frameIndex;
    UINT m_rtvDescriptorSize;
    UINT64 m_fenceValue;
    HANDLE m_fenceEvent;
    bool m_initialized;
    float m_bgColor[4];
};