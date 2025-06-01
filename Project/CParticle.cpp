#include "CParticle.h"
#include "CCubeMesh.h"
#include "CCubeShader.h"
#include "Scene.h"
CParticle::CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(1)
{
    CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, pd3dCommandList, 0.2f, 0.2f, 0.2f);
    SetMesh(pMesh);

    CreateShaderVariables(pd3dDevice, pd3dCommandList);

    CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
    pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/effect.dds", RESOURCE_TEXTURE2D, 0);

    CCubeShader *pShader = new CCubeShader();
    pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
    pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

    CScene::CreateShaderResourceViews(pd3dDevice, pTexture, 0, 15);

    CMaterial *pMaterial = new CMaterial(1);
    pMaterial->SetTexture(pTexture);
    pMaterial->SetShader(pShader);

    SetMaterial(0, pMaterial);
}

void CParticle::Activate()
{
    m_bActive = true;
    m_fElapsed = 0.0f;
}

void CParticle::Animate(float fTimeElapsed, XMFLOAT3 position)
{
    SetPosition(position);

    if (m_bActive) {
        m_fElapsed += fTimeElapsed;
        if (m_fElapsed > m_fLifeTime) m_bActive = false;
    }

    CGameObject::Animate(fTimeElapsed);
}

void CParticle::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* camera)
{
    if (m_bActive) CGameObject::Render(pd3dCommandList, camera);
}
