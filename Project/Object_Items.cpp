#include "stdafx.h"
#include "Object_Items.h"

void Item::ChangeExistState(bool isExist)
{
	is_exist = isExist;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
Shovel::Shovel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
	CLoadedModelInfo* pShovelModel = pModel;
	if (!pShovelModel) pShovelModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Item/Shovel.bin", NULL);

	SetChild(pShovelModel->m_pModelRootObject, true);
}

Shovel::~Shovel()
{
}

void Shovel::GenerateSwingBoundingBox(XMFLOAT3 playerPos, XMFLOAT3 playerLook)
{
	m_bIsSwingActive = true;

	// 바운딩 박스의 중심: 플레이어 위치에서 전방(Look) 방향으로 1.0f 이동
	XMFLOAT3 forwardOffset = Vector3::ScalarProduct(Vector3::Normalize(playerLook), 1.0f);
	m_attackBoundingBox.Center = Vector3::Add(playerPos, forwardOffset);

	// 바운딩 박스의 크기: (0.5f, 0.5f, 1.0f)
	m_attackBoundingBox.Extents = XMFLOAT3(0.5f, 0.5f, 1.0f);

	UpdateSwingBoundingBox();
}

void Shovel::UpdateSwingBoundingBox()
{
	if (m_bIsSwingActive)
	{

	}
}

void Shovel::DeleteSwingBoundingBox()
{
	m_bIsSwingActive = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
Handmap::Handmap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
	CLoadedModelInfo* pHandmapModel = pModel;
	if (!pHandmapModel) pHandmapModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Item/Flashlight.bin", NULL);

	SetChild(pHandmapModel->m_pModelRootObject, true);
}

Handmap::~Handmap()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
FlashLight::FlashLight(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
	CLoadedModelInfo* pFlashlightModel = pModel;
	if (!pFlashlightModel) pFlashlightModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Item/Flashlight.bin", NULL);

	SetChild(pFlashlightModel->m_pModelRootObject, true);
}

FlashLight::~FlashLight()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
Whistle::Whistle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
	CLoadedModelInfo* pWhistleModel = pModel;
	if (!pWhistleModel) pWhistleModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Item/Whistle.bin", NULL);

	SetChild(pWhistleModel->m_pModelRootObject, true);
}

Whistle::~Whistle()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
void Item::Animate(float fTimeElapsed)
{

}
