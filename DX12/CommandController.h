#pragma once

#include "StandardIncludes.h"

class CommandController
{
public:
    CommandController();
    virtual ~CommandController();

    ComPtr<ID3D12CommandQueue>& GetCommandQueue()
    {
        return m_commandQueue;
    }

    ComPtr<ID3D12CommandAllocator>& GetCommandAllocator()
    {
        return m_commandAllocator;
    }

    ComPtr<ID3D12GraphicsCommandList>& GetCommandList()
    {
        return m_commandList;
    }

    void Create(ID3D12Device* _device);
    void Reset(ID3D12PipelineState* _pipelineState = nullptr);
    void Close();
    void Execute();

    void AddBarrier(
        ID3D12Resource* _resource,
        D3D12_RESOURCE_STATES _before,
        D3D12_RESOURCE_STATES _after);

private:
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
};