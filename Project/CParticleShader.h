#pragma once
#include "stdafx.h"
#include "Shader.h"
class CParticleShader : public CShader
{
public:
    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
    virtual D3D12_BLEND_DESC CreateBlendState() override;
    virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState() override;
    virtual D3D12_SHADER_BYTECODE CreateVertexShader() override;
    virtual D3D12_SHADER_BYTECODE CreatePixelShader() override;
};

