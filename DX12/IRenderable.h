#pragma once

#include <d3d12.h>

class Shader;

class IRenderable
{
public:
    IRenderable()
    {
        m_shader = nullptr;
    }

    virtual ~IRenderable() {}

    Shader* GetShader()
    {
        return m_shader;
    }

    void SetShader(Shader* _shader)
    {
        m_shader = _shader;
    }

    virtual void Render(
        ID3D12GraphicsCommandList* _commandList) = 0;

private:
    Shader* m_shader;
};