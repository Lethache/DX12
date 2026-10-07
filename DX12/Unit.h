#pragma once

#include "StandardIncludes.h"
#include "Mesh.h"

class Unit : public Mesh
{
public:
    Unit();
    virtual ~Unit();

    XMFLOAT3 GetPosition() const { return m_position; }
    void SetPosition(XMFLOAT3 _position) { m_position = _position; }

    XMFLOAT3 GetRotation() const { return m_rotation; }
    void SetRotation(XMFLOAT3 _rotation) { m_rotation = _rotation; }

    XMFLOAT3 GetScale() const { return m_scale; }
    void SetScale(XMFLOAT3 _scale) { m_scale = _scale; }

    void Render(XMMATRIX _viewProj) override;

private:
    XMFLOAT3 m_position;
    XMFLOAT3 m_rotation;
    XMFLOAT3 m_scale;
};