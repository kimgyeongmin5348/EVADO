#pragma once
#include "stdafx.h"
#include "Object.h"
#include "CFontMesh.h"
class CText : public CGameObject
{
public:
    CText(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const std::wstring& text, float x, float y);
    virtual ~CText();

    //virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);
    void UpdateText(const std::wstring& text, const std::wstring& fixtext);

private:
    float m_fX = 0.0f, m_fY = 0.0f;

    std::wstring m_text;

    ID3D12Device* device = nullptr;
    ID3D12GraphicsCommandList* commandlist = nullptr;
    ID3D12RootSignature* rootsig = nullptr;
};

