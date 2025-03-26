#include "stdafx.h"
#include "Object_Items.h"

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
HandMap::HandMap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
}

HandMap::~HandMap()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
FlashLight::FlashLight(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
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