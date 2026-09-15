#pragma once

#include "StandardIncludes.h"

class Renderer : public Singleton<Renderer>
{
public:
    // Constructors / Destructors
    Renderer();
    virtual ~Renderer();

    // Accessors
    ComPtr<ID3D12Device> GetDevice() { return m_device; }

    // Methods
    void ConfigurePipeline(bool _userWarpDevice);

private:
    // Methods
    UINT EnableDebugLayer();
    void CreateDevice(bool _userWarpDevice);
    void GetHardwareAdapter(
        _In_ IDXGIFactory1* _factory,
        _Outptr_result_maybenull_ IDXGIAdapter1** _adapter,
        bool _reqHighPerfAdapter = false);

    // Members
    ComPtr<IDXGIFactory4> m_factory;
    ComPtr<ID3D12Device> m_device;
};