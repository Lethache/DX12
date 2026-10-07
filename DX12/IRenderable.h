#pragma once

#include "StandardIncludes.h"

class Shader;
class Texture;

class IRenderable
{
public:
    IRenderable()
    {
        m_shader = nullptr;
        m_texture0 = nullptr;
        m_texture1 = nullptr;
    }

    virtual ~IRenderable() {}

    Shader* GetShader() const
    {
        return m_shader;
    }

    void SetShader(Shader* shader)
    {
        m_shader = shader;
    }

    Texture* GetTexture0() const
    {
        return m_texture0;
    }

    void SetTexture0(Texture* texture)
    {
        m_texture0 = texture;
    }

    Texture* GetTexture1() const
    {
        return m_texture1;
    }

    void SetTexture1(Texture* texture)
    {
        m_texture1 = texture;
    }

    virtual void Render(XMMATRIX viewProj) = 0;

private:
    Shader* m_shader;
    Texture* m_texture0;
    Texture* m_texture1;
};