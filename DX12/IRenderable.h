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
        m_texDiffuse = nullptr;
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

    Texture* GetTexDiffuse() const
    {
        return m_texDiffuse;
    }

    void SetTexDiffuse(Texture* texture)
    {
        m_texDiffuse = texture;
    }

    Texture* GetTexture0() const
    {
        return m_texDiffuse;
    }

    void SetTexture0(Texture* texture)
    {
        m_texDiffuse = texture;
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
    Texture* m_texDiffuse;
    Texture* m_texture1;
};