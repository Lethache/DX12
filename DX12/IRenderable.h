#pragma once

#include "StandardIncludes.h"

class IRenderable
{
public:
    IRenderable()
    {
        m_shader = nullptr;
        m_texture = nullptr;
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

    Texture* GetTexture()
    {
        return m_texture;
    }

    void SetTexture(Texture* _texture)
    {
        m_texture = _texture;
    }

    virtual void Render(XMMATRIX _viewProj) = 0;

private:
    Shader* m_shader;
    Texture* m_texture;
};