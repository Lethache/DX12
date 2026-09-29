#pragma once

#include "Mesh.h"

class Unit : public Mesh
{
public:
    DirectX::XMFLOAT3 GetPosition() const
    {
        return m_position;
    }

    void SetPosition(DirectX::XMFLOAT3 position)
    {
        m_position = position;
    }

    DirectX::XMFLOAT3 GetRotation() const
    {
        return m_rotation;
    }

    void SetRotation(DirectX::XMFLOAT3 rotation)
    {
        m_rotation = rotation;
    }

    DirectX::XMFLOAT3 GetScale() const
    {
        return m_scale;
    }

    void SetScale(DirectX::XMFLOAT3 scale)
    {
        m_scale = scale;
    }

    void Render(
        ID3D12GraphicsCommandList* commandList) override;

private:
    DirectX::XMFLOAT3 m_position = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_rotation = { 0.0f, 0.0f, 0.0f };
    DirectX::XMFLOAT3 m_scale = { 1.0f, 1.0f, 1.0f };
};