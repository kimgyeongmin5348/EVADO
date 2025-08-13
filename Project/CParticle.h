#pragma once
#include "stdafx.h"
#include "object.h"

class CParticle : public CGameObject
{
public:
    bool m_bActive = false;
    //float m_fElapsed = 0.0f;
    //float m_fLifeTime = 0.5f;

    static const int MAX_PARTICLES = 30;

    XMFLOAT3 m_positions[MAX_PARTICLES] = {};
    XMFLOAT3 m_velocities[MAX_PARTICLES] = {};
    float m_lifetimes[MAX_PARTICLES] = {};
    float m_maxLifetime = 0.5f;
    bool m_bActives[MAX_PARTICLES] = {};
    float m_alphas[MAX_PARTICLES] = {};

    int m_activeCount = 0;

    CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
    void Activate(XMFLOAT3 pos);
    virtual void Animate(float fTimeElapsed, XMFLOAT3 position);
    virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* camera) override;
};

