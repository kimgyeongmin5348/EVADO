#include "CText.h"
#include "CFontMesh.h"
#include "CFontShader.h"
#include "Scene.h"
#include "GameFramework.h"

extern CGameFramework gGameFramework;

CText::CText(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, const std::wstring& text, float x, float y) : CGameObject(1)
{
    CFontMesh* pMesh = new CFontMesh(pd3dDevice, pd3dCommandList, text, x, y);
    SetMesh(pMesh);

    m_text = text;
    m_fX = x;
    m_fY = y;
    device = pd3dDevice;
    commandlist = pd3dCommandList;
    rootsig = pd3dGraphicsRootSignature;

    CreateShaderVariables(pd3dDevice, pd3dCommandList);

    CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
    pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/FontAtlas.dds", RESOURCE_TEXTURE2D, 0);

    CFontShader* pShader = new CFontShader();
    pShader->CreateShader(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
    pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);

    CScene::CreateShaderResourceViews(pd3dDevice, pTexture, 0, 16);

    CMaterial* pMaterial = new CMaterial(1);
    pMaterial->SetTexture(pTexture);
    pMaterial->SetShader(pShader);

    SetMaterial(0, pMaterial);
}

CText::~CText()
{
}

//void CText::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
//{
//    CGameObject::Render(pd3dCommandList, pCamera);
//}

void CText::UpdateText(const std::wstring& text, const std::wstring& fixtext)
{
    m_text = fixtext + text;

    if (!commandlist) return;
    CFontMesh* pMesh = new CFontMesh(device, commandlist, m_text, m_fX, m_fY);
    SetMesh(pMesh);
}
