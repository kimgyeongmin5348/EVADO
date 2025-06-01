#include "CParticle.h"
#include "CCubeMesh.h"
#include "CCubeShader.h"
#include "Scene.h"
#include <random>

CParticle::CParticle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(1)
{
    m_bActive = false;

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

void CParticle::Activate(XMFLOAT3 pos)
{
    //m_bActive = true;
    //m_fElapsed = 0.0f;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    m_activeCount = MAX_PARTICLES;
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        XMFLOAT3 dir = { dist(gen), dist(gen), dist(gen) };
        XMVECTOR v = XMVector3Normalize(XMLoadFloat3(&dir));
        XMStoreFloat3(&dir, v);

        m_positions[i] = pos;
        m_velocities[i] = { dir.x * 2.0f, dir.y * 2.0f, dir.z * 2.0f };
        m_lifetimes[i] = 0.0f;
        m_bActives[i] = true;
    }

    m_bActive = true;
}

void CParticle::Animate(float fTimeElapsed, XMFLOAT3 position)
{
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        if (!m_bActives[i]) continue;

        m_lifetimes[i] += fTimeElapsed;
        if (m_lifetimes[i] >= m_maxLifetime) {
            m_bActives[i] = false;
            continue;
        }

        m_positions[i].x += m_velocities[i].x * fTimeElapsed;
        m_positions[i].y += m_velocities[i].y * fTimeElapsed;
        m_positions[i].z += m_velocities[i].z * fTimeElapsed;
    }
}

void CParticle::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* camera)
{
    for (int i = 0; i < MAX_PARTICLES; ++i) {
        if (!m_bActives[i]) continue;

        SetPosition(m_positions[i]); // 위치 적용
        CGameObject::Render(pd3dCommandList, camera);
    }
}
