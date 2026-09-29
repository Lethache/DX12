#include "Unit.h"
#include "Shader.h"

void Unit::Render(
    ID3D12GraphicsCommandList* commandList)
{
    using namespace DirectX;

    // Apply scale, rotation, and translation in that order.
    XMMATRIX world =
        XMMatrixScaling(
            m_scale.x,
            m_scale.y,
            m_scale.z)
        * XMMatrixRotationRollPitchYaw(
            m_rotation.x,
            m_rotation.y,
            m_rotation.z)
        * XMMatrixTranslation(
            m_position.x,
            m_position.y,
            m_position.z);

    // Upload the object transformation before drawing.
    GetShader()->SetWorldMatrix(world);
    GetShader()->SetPipelineState(commandList);

    Mesh::Render(commandList);
}