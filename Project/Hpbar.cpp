#include "Hpbar.h"
#include "CRectMesh.h"
#include "CCubeShader.h"
#include "Scene.h"

Hpbar::Hpbar(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(1)
{
    /*
	CRectMesh* pMesh = new CRectMesh(pd3dDevice, pd3dCommandList, 6.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f);
    SetMesh(pMesh);

    CreateShaderVariables(pd3dDevice, pd3dCommandList);

    CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
    pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/hp.dds", RESOURCE_TEXTURE2D, 0);

    CCubeShader* pShader = new CCubeShader();
    pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
    pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

    CScene::CreateShaderResourceViews(pd3dDevice, pTexture, 0, 15);

    CMaterial* pMaterial = new CMaterial(1);
    pMaterial->SetTexture(pTexture);
    pMaterial->SetShader(pShader);

    SetMaterial(0, pMaterial);
    */
    float totalWidth = 6.0f;
    float height = 0.5f;
    float segmentWidth = totalWidth / m_nSegments;

    for (int i = 0; i < m_nSegments; ++i)
    {
        float xOffset = -totalWidth / 2.0f + segmentWidth * i + segmentWidth / 2.0f;
        CRectMesh* pMesh = new CRectMesh(pd3dDevice, pd3dCommandList, segmentWidth, height, 0.0f, xOffset, 0.0f, 0.0f);

        CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
        pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/hp.dds", RESOURCE_TEXTURE2D, 0);

        CCubeShader* pShader = new CCubeShader();
        pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
        pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

        CScene::CreateShaderResourceViews(pd3dDevice, pTexture, 0, 15);

        CMaterial* pMaterial = new CMaterial(1);
        pMaterial->SetTexture(pTexture);
        pMaterial->SetShader(pShader);

        CGameObject* pSegment = new CGameObject(1);
        pSegment->SetMesh(pMesh);
        pSegment->SetMaterial(0, pMaterial);
        pSegment->SetPosition(XMFLOAT3(xOffset, 0.0f, 0.0f));

        m_hpSegments.push_back(pSegment);
    }
}

void Hpbar::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* camera)
{
    for (auto& segment : m_hpSegments)
    {
        if (segment && segment->GetVisible())
        {
            segment->LookAt(camera->GetPosition(), XMFLOAT3(0, 1, 0));
            segment->Render(pd3dCommandList, camera);
        }
    }
    CGameObject::Render(pd3dCommandList, camera);
}

void Hpbar::SetHpbar(float ratio)
{
    int visibleCount = static_cast<int>(ratio * m_nSegments);

    for (int i = 0; i < m_nSegments; ++i)
    {
        if (i < visibleCount) m_hpSegments[i]->SetVisible(true);
        else m_hpSegments[i]->SetVisible(false);
    }
}
