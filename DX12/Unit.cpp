#include "Unit.h"
#include "ShaderPhong.h"

Unit::Unit()
{
    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

Unit::~Unit()
{
}

void Unit::Render(XMMATRIX viewProj)
{
    XMMATRIX world = XMMatrixScaling(
        m_scale.x,
        m_scale.y,
        m_scale.z);

    world *= XMMatrixRotationRollPitchYaw(
        m_rotation.x,
        m_rotation.y,
        m_rotation.z);

    world *= XMMatrixTranslation(
        m_position.x,
        m_position.y,
        m_position.z);

    ShaderPhong* shader =
        dynamic_cast<ShaderPhong*>(GetShader());

    if (shader != nullptr)
    {
        shader->SetWorldMatrix(world);
    }

    Mesh::Render(world * viewProj);
}