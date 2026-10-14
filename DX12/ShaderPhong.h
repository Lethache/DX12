#pragma once

#include "Shader.h"

class ShaderPhong : public Shader
{
public:
    ShaderPhong();

protected:
    void CreatePipelineState() override;
};