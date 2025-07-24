#pragma once
#include "stdafx.h"
#include "Shader.h"
class CFontShader : public CShader
{
public:
    CFontShader();
    virtual ~CFontShader();

    virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
    virtual D3D12_SHADER_BYTECODE CreateVertexShader() override;
    virtual D3D12_SHADER_BYTECODE CreatePixelShader() override;
};

