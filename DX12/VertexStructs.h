#pragma once

#include "StandardIncludes.h"

enum VertexTypes
{
    EN_VT_POS = 0,
    EN_VT_POSCOLTEX,
    EN_VT_POSNRMTEX
};

struct VTX_Base
{
    virtual VertexTypes GetVertexType() = 0;
};

struct VTX_Pos : public VTX_Base
{
    VertexTypes GetVertexType() override
    {
        return EN_VT_POS;
    }

    D3D12_INPUT_ELEMENT_DESC InputElementDescs[1] =
    {
        {
            "POSITION", 0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }
    };
};

struct VTX_PosColorTex : public VTX_Base
{
    VertexTypes GetVertexType() override
    {
        return EN_VT_POSCOLTEX;
    }

    D3D12_INPUT_ELEMENT_DESC InputElementDescs[3] =
    {
        {
            "POSITION", 0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        {
            "COLOR", 0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0, 12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0,
            DXGI_FORMAT_R32G32_FLOAT,
            0, 28,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }
    };
};

struct VTX_PosNormTex : public VTX_Base
{
    VertexTypes GetVertexType() override
    {
        return EN_VT_POSNRMTEX;
    }

    D3D12_INPUT_ELEMENT_DESC InputElementDescs[3] =
    {
        {
            "POSITION", 0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0, 0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        {
            "NORMAL", 0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0, 12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0,
            DXGI_FORMAT_R32G32_FLOAT,
            0, 24,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
        }
    };
};