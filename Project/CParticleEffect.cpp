#include "CParticleEffect.h"
#include "CparticleMesh.h"
#include "CParticleShader.h"
#include "Scene.h"

CParticleEffect::CParticleEffect(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(1)
{
    CMesh* mesh = new CParticleMesh(pd3dDevice, pd3dCommandList);
    SetMesh(mesh);

    CreateShaderVariables(pd3dDevice, pd3dCommandList);

    CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
    pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/effect.dds", RESOURCE_TEXTURE2D, 0);

    CParticleShader* pParticleShader = new CParticleShader();
    pParticleShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
    pParticleShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

    CScene::CreateShaderResourceViews(pd3dDevice, pTexture, 0, 15);

    CMaterial* pMaterial = new CMaterial(1);
    pMaterial->SetTexture(pTexture);
    pMaterial->SetShader(pParticleShader);
   
    SetMaterial(0, pMaterial);
}

void CParticleEffect::Activate(XMFLOAT3 position)
{
    SetPosition(position);
    m_bActive = true;
    m_fElapsedTime = 0.0f;
}

void CParticleEffect::Animate(float fTimeElapsed)
{
    if (m_bActive) {
        m_fElapsedTime += fTimeElapsed;
        if (m_fElapsedTime >= m_fLifeTime) {
            m_bActive = false;
        }
    }
}

void CParticleEffect::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
    if (m_bActive) {
        cout << "[Particle] Render Called" << endl;
        CGameObject::Render(pd3dCommandList, pCamera);
    }
}
