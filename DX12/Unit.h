#pragma once

#include "StandardIncludes.h"
#include "Mesh.h"

class Unit : public Mesh
{
public:
    Unit();
    virtual ~Unit();

    XMFLOAT3 GetPosition() const
    {
        return m_position;
    }

    void SetPosition(XMFLOAT3 position)
    {
        m_position = position;
    }

    XMFLOAT3 GetRotation() const
    {
        return m_rotation;
    }

    void SetRotation(XMFLOAT3 rotation)
    {
        m_rotation = rotation;
    }

    XMFLOAT3 GetScale() const
    {
        return m_scale;
    }

    void SetScale(XMFLOAT3 scale)
    {
        m_scale = scale;
    }

    void Render(XMMATRIX viewProj) override;

private:
    XMFLOAT3 m_position;
    XMFLOAT3 m_rotation;
    XMFLOAT3 m_scale;
};