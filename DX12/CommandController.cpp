#include "CommandController.h"

CommandController::CommandController()
{
}

CommandController::~CommandController()
{
}

void CommandController::Create(ID3D12Device* _device)
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    M_ASSERT(SUCCEEDED(_device->CreateCommandQueue(
        &queueDesc,
        IID_PPV_ARGS(&m_commandQueue))),
        "Failed to create command queue.");

    M_ASSERT(SUCCEEDED(_device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        IID_PPV_ARGS(&m_commandAllocator))),
        "Failed to create command allocator.");

    M_ASSERT(SUCCEEDED(_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&m_commandList))),
        "Failed to create command list.");

    Close();
}

void CommandController::Reset(ID3D12PipelineState* _pipelineState)
{
    M_ASSERT(SUCCEEDED(m_commandAllocator->Reset()),
        "Failed to reset command allocator.");

    M_ASSERT(SUCCEEDED(m_commandList->Reset(
        m_commandAllocator.Get(), _pipelineState)),
        "Failed to reset command list.");
}

void CommandController::Close()
{
    M_ASSERT(SUCCEEDED(m_commandList->Close()),
        "Failed to close command list.");
}

void CommandController::Execute()
{
    ID3D12CommandList* lists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(lists), lists);
}

void CommandController::AddBarrier(
    ID3D12Resource* _resource,
    D3D12_RESOURCE_STATES _before,
    D3D12_RESOURCE_STATES _after)
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        _resource, _before, _after);

    m_commandList->ResourceBarrier(1, &barrier);
}