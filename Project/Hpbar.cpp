#include "Hpbar.h"
#include "CRectMesh.h"
#include "CCubeShader.h"
#include "Scene.h"

Hpbar::Hpbar(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature) : CGameObject(1)
{
	CRectMesh* pMesh = new CRectMesh(pd3dDevice, pd3dCommandList, 8.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
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
}

void Hpbar::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* camera)
{
    XMFLOAT3 xmf3CameraPosition = camera->GetPosition();
    LookAt(xmf3CameraPosition, XMFLOAT3(0.0f, 1.0f, 0.0f));

    CGameObject::Render(pd3dCommandList, camera);
}
