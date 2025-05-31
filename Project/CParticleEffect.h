#pragma once
#include "stdafx.h"
#include "Object.h"
class CParticleEffect : public CGameObject
{
public:
    float m_fLifeTime = 0.5f;
    float m_fElapsedTime = 0.0f;
    bool m_bActive = false;

public:
    CParticleEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);

    void Activate(XMFLOAT3 position);
    void Animate(float fTimeElapsed) override;
    void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) override;
};

