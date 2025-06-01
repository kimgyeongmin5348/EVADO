#pragma once
#include "stdafx.h"
#include "object.h"
class CParticle : public CGameObject
{
public:
    bool m_bActive = false;
    float m_fElapsed = 0.0f;
    float m_fLifeTime = 0.5f;

    CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
    void Activate();
    virtual void Animate(float fTimeElapsed, XMFLOAT3 position);
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* camera) override;
};

