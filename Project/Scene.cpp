//-----------------------------------------------------------------------------
// File: CScene.cpp
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "Scene.h"
#include "Network.h"
#include "GameFramework.h"

extern CGameFramework gGameFramework;

ID3D12DescriptorHeap *CScene::m_pd3dCbvSrvDescriptorHeap = NULL;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorStartHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorStartHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorStartHandle;

D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dCbvGPUDescriptorNextHandle;
D3D12_CPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvCPUDescriptorNextHandle;
D3D12_GPU_DESCRIPTOR_HANDLE	CScene::m_d3dSrvGPUDescriptorNextHandle;

CScene::CScene()
{
}

CScene::~CScene()
{
}
void CScene::BuildDefaultLightsAndMaterials(bool toggle)
{
	m_nLights = 5;
	m_pLights = new LIGHT[m_nLights];
	::ZeroMemory(m_pLights, sizeof(LIGHT) * m_nLights);

	//m_xmf4GlobalAmbient = XMFLOAT4(0.08f, 0.08f, 0.08f, 1.0f);
	m_xmf4GlobalAmbient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	m_pLights[0].m_bEnable = true;
	m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights[0].m_fRange = 17.0f;
	m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights[0].m_xmf3Position = XMFLOAT3(3.0f, 5.0f, 30.0f);
	m_pLights[0].m_xmf3Attenuation = XMFLOAT3(0.5f, 0.05f, 0.0001f);

	m_pLights[1].m_bEnable = toggle;
	m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights[1].m_fRange = 300.0f;
	m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[1].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 1.0f);
	m_pLights[1].m_xmf3Attenuation = XMFLOAT3(0.5f, 0.05f, 0.0001f);
	m_pLights[1].m_fFalloff = 8.0f;
	m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights[2].m_bEnable = true;
	m_pLights[2].m_nType = SPOT_LIGHT;
	m_pLights[2].m_fRange = 600.0f;
	m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights[2].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights[2].m_xmf3Position = XMFLOAT3(-43.0f, 10.0f, -45.0f);
	m_pLights[2].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights[2].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[2].m_fFalloff = 8.0f;
	m_pLights[2].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[2].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pLights[3].m_bEnable = true;
	m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights[3].m_fRange = 600.0f;
	m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.3f, 0.7f, 0.0f, 1.0f);
	m_pLights[3].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 0.0f);
	m_pLights[3].m_xmf3Position = XMFLOAT3(45.0f, 10.0f, -28.0f);
	m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[3].m_fFalloff = 8.0f;
	m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pLights[4].m_bEnable = true;
	m_pLights[4].m_nType = SPOT_LIGHT;
	m_pLights[4].m_fRange = 600.0f;
	m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f);
	m_pLights[4].m_xmf4Specular = XMFLOAT4(0.6f, 0.2f, 0.2f, 0.0f);
	m_pLights[4].m_xmf3Position = XMFLOAT3(-63.0f, 10.0f, -14.0f);
	m_pLights[4].m_xmf3Direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	m_pLights[4].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights[4].m_fFalloff = 8.0f;
	m_pLights[4].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights[4].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void CScene::InitializeCollisionSystem()
{
	BoundingBox worldBounds(XMFLOAT3(-20.0f, -10.f, -66.0f), XMFLOAT3(150.0f, 100.0f, 170.0f));
	m_CollisionManager.Build(worldBounds, 35, 4);

	for (int i = 0; i < m_nGameObjects; ++i) {
		m_CollisionManager.InsertObject(m_ppGameObjects[i]);
	}

	for (int i = 0; i < m_nMonster; ++i) {
		m_CollisionManager.InsertObject(m_ppMonsters[i]);
	}

	for (auto obj : m_pMap->m_vMapObjects) {
		std::string strFrameName = obj->GetFrameName();
		if (std::string::npos != strFrameName.find("floor") || std::string::npos != strFrameName.find("ceiling"))
			continue;
		m_CollisionManager.InsertObject(obj);
	}

	//m_CollisionManager.PrintTree();
}

void CScene::GenerateGameObjectsBoundingBox()
{
	m_pPlayer->CalculateBoundingBox();

	for (int i = 0; i < m_nGameObjects; ++i) {
		m_ppGameObjects[i]->CalculateBoundingBox();
	}

	for (int i = 0; i < m_nMonster; ++i) {
		m_ppMonsters[i]->CalculateBoundingBox();
	}

	for (auto obj : m_pMap->m_vMapObjects) {
		obj->CalculateBoundingBox(); 
	}
}

void CScene::ItemToHand(CGameObject* pItem)
{

}

void CScene::BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 100, 400);

	CMaterial::PrepareShaders(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature); 

	BuildDefaultLightsAndMaterials(false);

	Device = pd3dDevice;
	Commandlist = pd3dCommandList;

	m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_pMap = new Map(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_pEffect = new CParticle(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);


	//m_nHierarchicalGameObjects = 4; // spider, flashlight, shovel, whistle
	//m_ppHierarchicalGameObjects = new CGameObject * [m_nHierarchicalGameObjects];

	//CLoadedModelInfo* pSpiderModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Spider.bin", NULL);
	//m_ppHierarchicalGameObjects[0] = new CSpider(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pSpiderModel, 3);
	//m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0);
	//m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1);
	//m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2);
	//m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	//m_ppHierarchicalGameObjects[0]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	//m_ppHierarchicalGameObjects[0]->SetPosition(3, 0, 30);
	//m_ppHierarchicalGameObjects[0]->Rotate(0, 180, 0);
	//m_ppHierarchicalGameObjects[0]->SetFrameName("Spider");

	m_nMonster = 4; // spider
	m_ppMonsters = new CGameObject * [m_nMonster];
	int monsterIDs[4] = { 10001,10002,10003,10004 };

	CLoadedModelInfo* pSpiderModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/spider_myOldOne.bin", NULL);
	XMFLOAT3 monsterPos[4] = {
		{27, 0, -2},
		{-54, 0, -90},
		{4, 0, -50},
		{-46, 0,-42}
	};
	for (int i = 0; i < m_nMonster; ++i)
	{
		m_ppMonsters[i] = new CSpider(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pSpiderModel, 5);
		m_ppMonsters[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0); //idle
		m_ppMonsters[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1); //walk
		m_ppMonsters[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2); //run
		m_ppMonsters[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3); //attack
		m_ppMonsters[i]->m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4); //death
		m_ppMonsters[i]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
		m_ppMonsters[i]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
		m_ppMonsters[i]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
		m_ppMonsters[i]->m_pSkinnedAnimationController->SetTrackEnable(4, false);

		m_ppMonsters[i]->SetPosition(monsterPos[i]);
		m_ppMonsters[i]->SetScale(3, 3, 3);

		std::string spiderName = "Spider" + std::to_string(i);
		m_ppMonsters[i]->SetFrameName(spiderName.c_str());

		static_cast<CSpider*>(m_ppMonsters[i])->SetMonsterID(monsterIDs[i]);
		g_monsters[monsterIDs[i]] = static_cast<CSpider*>(m_ppMonsters[i]);

	}


	if (pSpiderModel) delete pSpiderModel;

	m_nGameObjects = 3;
	m_ppGameObjects = new CGameObject * [m_nGameObjects];
	long long itemIDs[3] = { 20000,20001,20002 };

	float itemPrices[3] = { 80.0f, 150.0f, 80.0f };

	XMFLOAT3 positions[3] = {
		{-2.0f, 0.0f, 19.0f},
		{-2.0f, 0.0f, 22.0f},
		{-2.0f, 0.0f, 25.0f}
	};

	CLoadedModelInfo* pFlashlightModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Item/Flashlightgold.bin", NULL);
	m_ppGameObjects[0] = new FlashLight(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pFlashlightModel);
	m_ppGameObjects[0]->SetScale(3, 3, 3);
	m_ppGameObjects[0]->Rotate(90, 0, 0);
	//m_ppHierarchicalGameObjects[1]->SetPosition(3, 2, 10);
	m_ppGameObjects[0]->SetFrameName("FlashLight");
	//m_ppGameObjects[0]->price = 80;
	m_ppGameObjects[0]->SetPosition(positions[0]);

	static_cast<Item*>(m_ppGameObjects[0])->SetUniqueID(itemIDs[0]);
	static_cast<Item*>(m_ppGameObjects[0])->SetPrice(itemPrices[0]);
	g_items[monsterIDs[0]] = static_cast<Item*>(m_ppGameObjects[0]);

	if (pFlashlightModel) delete pFlashlightModel;

	// ============================================================================================================

	CLoadedModelInfo* pShovelModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Item/Shovel.bin", NULL);
	m_ppGameObjects[1] = new Shovel(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pShovelModel);
	m_ppGameObjects[1]->SetScale(1, 1, 1);
	m_ppGameObjects[1]->Rotate(0, 0, 90);
	//m_ppHierarchicalGameObjects[2]->SetPosition(3, 2, 12);
	m_ppGameObjects[1]->SetFrameName("Shovel");
	//m_ppGameObjects[1]->price = 80;
	m_ppGameObjects[1]->SetPosition(positions[1]);

	static_cast<Item*>(m_ppGameObjects[1])->SetUniqueID(itemIDs[1]);
	static_cast<Item*>(m_ppGameObjects[1])->SetPrice(itemPrices[1]);
	g_items[monsterIDs[1]] = static_cast<Item*>(m_ppGameObjects[1]);

	if (pShovelModel) delete pShovelModel;

	// ============================================================================================================


	CLoadedModelInfo* pWhistleModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Item/Whistle.bin", NULL);
	m_ppGameObjects[2] = new Whistle(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pWhistleModel);
	m_ppGameObjects[2]->SetScale(1, 1, 1);
	//m_ppHierarchicalGameObjects[3]->SetPosition(3, 2, 13);
	m_ppGameObjects[2]->SetFrameName("Whistle");
	//m_ppGameObjects[2]->price = 30;
	m_ppGameObjects[2]->SetPosition(positions[2]);

	static_cast<Item*>(m_ppGameObjects[2])->SetUniqueID(itemIDs[2]);
	static_cast<Item*>(m_ppGameObjects[2])->SetPrice(itemPrices[2]);
	g_items[monsterIDs[2]] = static_cast<Item*>(m_ppGameObjects[2]);

	if (pWhistleModel) delete pWhistleModel;

	m_nOtherPlayers = 1;
	m_ppOtherPlayers = new OtherPlayer * [m_nOtherPlayers];
	CLoadedModelInfo* pOtherPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, "Model/Player.bin", NULL);
	m_ppOtherPlayers[0] = new OtherPlayer(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, pOtherPlayerModel);
	m_ppOtherPlayers[0]->SetPosition(-1000, -1000, -1000);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackEnable(0, true);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackEnable(6, false);

	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackPosition(4, 0.0f);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackPosition(5, 0.0f);
	m_ppOtherPlayers[0]->m_pSkinnedAnimationController->SetTrackPosition(6, 0.0f);

	if (pOtherPlayerModel) delete pOtherPlayerModel;

	// 인벤토리 UI 및 상점
	m_nShaders = 10;
	m_ppShaders = new CShader * [m_nShaders];

	CTexture* pTextureinven = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pTextureinven->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/inven.dds", RESOURCE_TEXTURE2D, 0);
	CreateShaderResourceViews(pd3dDevice, pTextureinven, 0, 15);
	m_textureMap["inven"] = pTextureinven;

	CTexture* pTextureItem1 = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pTextureItem1->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Shovel.dds", RESOURCE_TEXTURE2D, 0);
	CreateShaderResourceViews(pd3dDevice, pTextureItem1, 0, 15);
	m_textureMap["Shovel"] = pTextureItem1;

	CTexture* pTextureItem2 = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pTextureItem2->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/FlashLight.dds", RESOURCE_TEXTURE2D, 0);
	CreateShaderResourceViews(pd3dDevice, pTextureItem2, 0, 15);
	m_textureMap["FlashLight"] = pTextureItem2;

	CTexture* pTextureItem3 = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pTextureItem3->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Whistle.dds", RESOURCE_TEXTURE2D, 0);
	CreateShaderResourceViews(pd3dDevice, pTextureItem3, 0, 15);
	m_textureMap["Whistle"] = pTextureItem3;

	CTextureToScreenShader* pTextureItem1Shader = new CTextureToScreenShader(1);
	pTextureItem1Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CScreenRectMeshTextured* pMesh = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, 0.02f, 0.225f * 0.5f, -0.65f, 0.4f * 0.5f);
	pTextureItem1Shader->SetMesh(0, pMesh);
	pTextureItem1Shader->SetTexture(pTextureinven);
	m_ppShaders[0] = pTextureItem1Shader;

	CTextureToScreenShader* pTextureItem2Shader = new CTextureToScreenShader(1);
	pTextureItem2Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CScreenRectMeshTextured* pMesh1 = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, 0.02f + 0.125f, 0.225f * 0.5f, -0.65f, 0.4f * 0.5f);
	pTextureItem2Shader->SetMesh(0, pMesh1);
	pTextureItem2Shader->SetTexture(pTextureinven);
	m_ppShaders[1] = pTextureItem2Shader;

	CTextureToScreenShader* pTextureItem3Shader = new CTextureToScreenShader(1);
	pTextureItem3Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CScreenRectMeshTextured* pMesh2 = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, 0.02f + 0.25f, 0.225f * 0.5f, -0.65f, 0.4f * 0.5f);
	pTextureItem3Shader->SetMesh(0, pMesh2);
	pTextureItem3Shader->SetTexture(pTextureinven);
	m_ppShaders[2] = pTextureItem3Shader;

	CTextureToScreenShader* pTextureItem4Shader = new CTextureToScreenShader(1);
	pTextureItem4Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CScreenRectMeshTextured* pMesh3 = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, 0.02f + 0.375f, 0.225f * 0.5f, -0.65f, 0.4f * 0.5f);
	pTextureItem4Shader->SetMesh(0, pMesh3);
	pTextureItem4Shader->SetTexture(pTextureinven);
	m_ppShaders[3] = pTextureItem4Shader;

	CTextureToScreenShader* pInventoryShader = new CTextureToScreenShader(1);
	pInventoryShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Inventory.dds", RESOURCE_TEXTURE2D, 0);
	CreateShaderResourceViews(pd3dDevice, pTexture, 0, 15);
	CScreenRectMeshTextured* pInventoryMesh = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, -0.5f + 0.5f, 1.0f * 0.5f, -0.6f, 0.5f * 0.5f);
	pInventoryShader->SetMesh(0, pInventoryMesh);
	pInventoryShader->SetTexture(pTexture);
	m_ppShaders[4] = pInventoryShader;

	//상점
	CShopShader* pShopShader = new CShopShader(1);
	pShopShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	pShopShader->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, NULL, NULL);

	CTexture* pShopTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pShopTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/shop.dds", RESOURCE_TEXTURE2D, 0);
	CreateShaderResourceViews(pd3dDevice, pShopTexture, 0, 15);
	CScreenRectMeshTextured* pShopMesh = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, -0.9f, 0.5f, 0.8f, 0.7f);
	pShopShader->SetMesh(0, pShopMesh);
	pShopShader->SetTexture(pShopTexture);
	pShopShader->SetVisible(false);

	m_ppShaders[5] = pShopShader;

	//상점 4칸
	CTextureToScreenShader* pShopSpace1Shader = new CTextureToScreenShader(1);
	pShopSpace1Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CScreenRectMeshTextured* pShopMesh1 = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, -0.825f, 0.08f, 0.625f, 0.09f);
	pShopSpace1Shader->SetMesh(0, pShopMesh1);
	pShopSpace1Shader->SetTexture(pTextureinven);
	pShopSpace1Shader->SetVisible(false);
	m_ppShaders[6] = pShopSpace1Shader;

	CTextureToScreenShader* pShopSpace2Shader = new CTextureToScreenShader(1);
	pShopSpace2Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CScreenRectMeshTextured* pShopMesh2 = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, -0.825f, 0.08f, 0.505f, 0.09f);
	pShopSpace2Shader->SetMesh(0, pShopMesh2);
	pShopSpace2Shader->SetTexture(pTextureinven);
	pShopSpace2Shader->SetVisible(false);
	m_ppShaders[7] = pShopSpace2Shader;

	CTextureToScreenShader* pShopSpace3Shader = new CTextureToScreenShader(1);
	pShopSpace3Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CScreenRectMeshTextured* pShopMesh3 = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, -0.825f, 0.08f, 0.385f, 0.09f);
	pShopSpace3Shader->SetMesh(0, pShopMesh3);
	pShopSpace3Shader->SetTexture(pTextureinven);
	pShopSpace3Shader->SetVisible(false);
	m_ppShaders[8] = pShopSpace3Shader;

	CTextureToScreenShader* pShopSpace4Shader = new CTextureToScreenShader(1);
	pShopSpace4Shader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	CScreenRectMeshTextured* pShopMesh4 = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, -0.825f, 0.08f, 0.265f, 0.09f);
	pShopSpace4Shader->SetMesh(0, pShopMesh4);
	pShopSpace4Shader->SetTexture(pTextureinven);
	pShopSpace4Shader->SetVisible(false);
	m_ppShaders[9] = pShopSpace4Shader;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pd3dCbvSrvDescriptorHeap) m_pd3dCbvSrvDescriptorHeap->Release();

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_pTerrain) delete m_pTerrain;
	if (m_pSkyBox) delete m_pSkyBox;

	if (m_ppMonsters)
	{
		for (int i = 0; i < m_nMonster; i++) if (m_ppMonsters[i]) m_ppMonsters[i]->Release();
		delete[] m_ppMonsters;
	}

	ReleaseShaderVariables();

	if (m_pLights) delete[] m_pLights;
}

ID3D12RootSignature *CScene::CreateGraphicsRootSignature(ID3D12Device *pd3dDevice)
{
	ID3D12RootSignature *pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE pd3dDescriptorRanges[12];

	pd3dDescriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[0].NumDescriptors = 1;
	pd3dDescriptorRanges[0].BaseShaderRegister = 6; //t6: gtxtAlbedoTexture
	pd3dDescriptorRanges[0].RegisterSpace = 0;
	pd3dDescriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[1].NumDescriptors = 1;
	pd3dDescriptorRanges[1].BaseShaderRegister = 7; //t7: gtxtSpecularTexture
	pd3dDescriptorRanges[1].RegisterSpace = 0;
	pd3dDescriptorRanges[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[2].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[2].NumDescriptors = 1;
	pd3dDescriptorRanges[2].BaseShaderRegister = 8; //t8: gtxtNormalTexture
	pd3dDescriptorRanges[2].RegisterSpace = 0;
	pd3dDescriptorRanges[2].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[3].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[3].NumDescriptors = 1;
	pd3dDescriptorRanges[3].BaseShaderRegister = 9; //t9: gtxtMetallicTexture
	pd3dDescriptorRanges[3].RegisterSpace = 0;
	pd3dDescriptorRanges[3].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[4].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[4].NumDescriptors = 1;
	pd3dDescriptorRanges[4].BaseShaderRegister = 10; //t10: gtxtEmissionTexture
	pd3dDescriptorRanges[4].RegisterSpace = 0;
	pd3dDescriptorRanges[4].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[5].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[5].NumDescriptors = 1;
	pd3dDescriptorRanges[5].BaseShaderRegister = 11; //t11: gtxtEmissionTexture
	pd3dDescriptorRanges[5].RegisterSpace = 0;
	pd3dDescriptorRanges[5].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[6].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[6].NumDescriptors = 1;
	pd3dDescriptorRanges[6].BaseShaderRegister = 12; //t12: gtxtEmissionTexture
	pd3dDescriptorRanges[6].RegisterSpace = 0;
	pd3dDescriptorRanges[6].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[7].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[7].NumDescriptors = 1;
	pd3dDescriptorRanges[7].BaseShaderRegister = 13; //t13: gtxtSkyBoxTexture
	pd3dDescriptorRanges[7].RegisterSpace = 0;
	pd3dDescriptorRanges[7].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[8].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[8].NumDescriptors = 1;
	pd3dDescriptorRanges[8].BaseShaderRegister = 1; //t1: gtxtTerrainBaseTexture
	pd3dDescriptorRanges[8].RegisterSpace = 0;
	pd3dDescriptorRanges[8].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[9].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[9].NumDescriptors = 1;
	pd3dDescriptorRanges[9].BaseShaderRegister = 2; //t2: gtxtTerrainDetailTexture
	pd3dDescriptorRanges[9].RegisterSpace = 0;
	pd3dDescriptorRanges[9].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[10].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[10].NumDescriptors = 1;
	pd3dDescriptorRanges[10].BaseShaderRegister = 0; //t0: gtxTexture
	pd3dDescriptorRanges[10].RegisterSpace = 0;
	pd3dDescriptorRanges[10].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	pd3dDescriptorRanges[11].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	pd3dDescriptorRanges[11].NumDescriptors = 1;
	pd3dDescriptorRanges[11].BaseShaderRegister = 3; //t3: gFontTexture
	pd3dDescriptorRanges[11].RegisterSpace = 0;
	pd3dDescriptorRanges[11].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER pd3dRootParameters[17];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 33;
	pd3dRootParameters[1].Constants.ShaderRegister = 2; //GameObject
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[0]);
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[4].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[4].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[1]);
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[5].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[5].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[2]);
	pd3dRootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[6].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[6].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[3]);
	pd3dRootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[7].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[7].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[4]);
	pd3dRootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[8].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[8].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[5]);
	pd3dRootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[9].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[9].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[6]);
	pd3dRootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[10].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[10].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[7]);
	pd3dRootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[11].Descriptor.ShaderRegister = 7; //Skinned Bone Offsets
	pd3dRootParameters[11].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[12].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[12].Descriptor.ShaderRegister = 8; //Skinned Bone Transforms
	pd3dRootParameters[12].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[12].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pd3dRootParameters[13].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[13].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[13].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[8]);
	pd3dRootParameters[13].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[14].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[14].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[14].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[9]);
	pd3dRootParameters[14].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[15].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[15].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[15].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[10]);
	pd3dRootParameters[15].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dRootParameters[16].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[16].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[16].DescriptorTable.pDescriptorRanges = &(pd3dDescriptorRanges[11]);
	pd3dRootParameters[16].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC pd3dSamplerDescs[3];

	pd3dSamplerDescs[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	pd3dSamplerDescs[0].MipLODBias = 0;
	pd3dSamplerDescs[0].MaxAnisotropy = 1;
	pd3dSamplerDescs[0].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[0].MinLOD = 0;
	pd3dSamplerDescs[0].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[0].ShaderRegister = 0;
	pd3dSamplerDescs[0].RegisterSpace = 0;
	pd3dSamplerDescs[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[1].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[1].MipLODBias = 0;
	pd3dSamplerDescs[1].MaxAnisotropy = 1;
	pd3dSamplerDescs[1].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[1].MinLOD = 0;
	pd3dSamplerDescs[1].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[1].ShaderRegister = 1;
	pd3dSamplerDescs[1].RegisterSpace = 0;
	pd3dSamplerDescs[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	pd3dSamplerDescs[2].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	pd3dSamplerDescs[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	pd3dSamplerDescs[2].MipLODBias = 0;
	pd3dSamplerDescs[2].MaxAnisotropy = 1;
	pd3dSamplerDescs[2].ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	pd3dSamplerDescs[2].MinLOD = 0;
	pd3dSamplerDescs[2].MaxLOD = D3D12_FLOAT32_MAX;
	pd3dSamplerDescs[2].ShaderRegister = 2;
	pd3dSamplerDescs[2].RegisterSpace = 0;
	pd3dSamplerDescs[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = _countof(pd3dSamplerDescs);
	d3dRootSignatureDesc.pStaticSamplers = pd3dSamplerDescs;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob *pd3dSignatureBlob = NULL;
	ID3DBlob *pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void **)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void CScene::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
	::memcpy(m_pcbMappedLights->m_pLights, m_pLights, sizeof(LIGHT) * m_nLights);
	::memcpy(&m_pcbMappedLights->m_xmf4GlobalAmbient, &m_xmf4GlobalAmbient, sizeof(XMFLOAT4));
	::memcpy(&m_pcbMappedLights->m_nLights, &m_nLights, sizeof(int));
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		m_pd3dcbLights->Release();
	}
}

void CScene::ReleaseUploadBuffers()
{
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->ReleaseUploadBuffers();
	for (int i = 0; i < m_nMonster; i++)if (m_ppMonsters[i]) m_ppMonsters[i]->ReleaseUploadBuffers();
}

void CScene::CreateCbvSrvDescriptorHeaps(ID3D12Device *pd3dDevice, int nConstantBufferViews, int nShaderResourceViews)
{
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	d3dDescriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3dDescriptorHeapDesc.NodeMask = 0;
	pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap), (void **)&m_pd3dCbvSrvDescriptorHeap);

	m_d3dCbvCPUDescriptorNextHandle = m_d3dCbvCPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_d3dCbvGPUDescriptorNextHandle = m_d3dCbvGPUDescriptorStartHandle = m_pd3dCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_d3dSrvCPUDescriptorNextHandle.ptr = m_d3dSrvCPUDescriptorStartHandle.ptr = m_d3dCbvCPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
	m_d3dSrvGPUDescriptorNextHandle.ptr = m_d3dSrvGPUDescriptorStartHandle.ptr = m_d3dCbvGPUDescriptorStartHandle.ptr + (::gnCbvSrvDescriptorIncrementSize * nConstantBufferViews);
}

D3D12_GPU_DESCRIPTOR_HANDLE CScene::CreateConstantBufferViews(ID3D12Device *pd3dDevice, int nConstantBufferViews, ID3D12Resource *pd3dConstantBuffers, UINT nStride)
{
	D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle = m_d3dCbvGPUDescriptorNextHandle;
	D3D12_GPU_VIRTUAL_ADDRESS d3dGpuVirtualAddress = pd3dConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC d3dCBVDesc;
	d3dCBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		d3dCBVDesc.BufferLocation = d3dGpuVirtualAddress + (nStride * j);
		m_d3dCbvCPUDescriptorNextHandle.ptr = m_d3dCbvCPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
		pd3dDevice->CreateConstantBufferView(&d3dCBVDesc, m_d3dCbvCPUDescriptorNextHandle);
		m_d3dCbvGPUDescriptorNextHandle.ptr = m_d3dCbvGPUDescriptorNextHandle.ptr + ::gnCbvSrvDescriptorIncrementSize;
	}
	return(d3dCbvGPUDescriptorHandle);
}

void CScene::CreateShaderResourceViews(ID3D12Device* pd3dDevice, CTexture* pTexture, UINT nDescriptorHeapIndex, UINT nRootParameterStartIndex)
{
	m_d3dSrvCPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);
	m_d3dSrvGPUDescriptorNextHandle.ptr += (::gnCbvSrvDescriptorIncrementSize * nDescriptorHeapIndex);

	if (pTexture)
	{
		int nTextures = pTexture->GetTextures();
		for (int i = 0; i < nTextures; i++)
		{
			ID3D12Resource* pShaderResource = pTexture->GetResource(i);
			D3D12_SHADER_RESOURCE_VIEW_DESC d3dShaderResourceViewDesc = pTexture->GetShaderResourceViewDesc(i);
			pd3dDevice->CreateShaderResourceView(pShaderResource, &d3dShaderResourceViewDesc, m_d3dSrvCPUDescriptorNextHandle);
			m_d3dSrvCPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
			pTexture->SetGpuDescriptorHandle(i, m_d3dSrvGPUDescriptorNextHandle);
			m_d3dSrvGPUDescriptorNextHandle.ptr += ::gnCbvSrvDescriptorIncrementSize;
		}
	}
	int nRootParameters = pTexture->GetRootParameters();
	for (int j = 0; j < nRootParameters; j++) pTexture->SetRootParameterIndex(j, nRootParameterStartIndex + j);
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	{
		if (!isShop) {
			int index = m_pPlayer->m_nSelectedInventoryIndex;

			// 손에 든 아이템 인덱스 유효성 검사
			if (index < 0 || index >= 4) break;

			CGameObject* pHeldItem = m_pPlayer->m_pHeldItems[index];
			if (!pHeldItem) break;

			char* frameName = pHeldItem->GetFrameName();

			if (!strcmp(frameName, "FlashLight"))
			{
				m_pPlayer->bflashlight = !m_pPlayer->bflashlight;
				BuildDefaultLightsAndMaterials(m_pPlayer->bflashlight);
			}
			else if (!strcmp(frameName, "Shovel"))
			{
				dynamic_cast<CTerrainPlayer*>(m_pPlayer)->m_currentAnim = AnimationState::SWING;
				uint8_t currentAnimState = static_cast<uint8_t>(dynamic_cast<CTerrainPlayer*>(m_pPlayer)->m_currentAnim);

				cout << "CScene - anim" << currentAnimState << endl;

				if (m_pPlayer->m_isMonsterHit)
				{
					m_pEffect->Activate(m_ppGameObjects[1]->GetPosition());

					CSpider* pSpider = dynamic_cast<CSpider*>(m_ppMonsters[0]);
					if (pSpider)
					{
						//pSpider->MonsterHP -= 25.0f;
					}
				}
				break;
			}
		}
		else {
			// 상점 판매 버튼 클릭
			const RECT rt[4] =
			{
				{450, 240, 550, 265},
				{450, 310, 550, 335},
				{450, 370, 550, 395},
				{450, 440, 550, 465}
			};

			for (int i = 0; i < 4; ++i) {
				if (PtInRect(&rt[i], m_ptPos)) {
					Item* pItem = dynamic_cast<Item*>(m_pPlayer->m_pHeldItems[i]);
					if (pItem && pItem->IsExist() && pItem->GetPrice() > 0) {
						dynamic_cast<CTerrainPlayer*>(m_pPlayer)->debt -= pItem->GetPrice();
						long long itemID = pItem->GetUniqueID();

						auto it = g_items.find(itemID);
						if (it != g_items.end()) {					
							it->second->ChangeExistState(false);						
							m_pPlayer->m_pHeldItems[i] = nullptr;
						
							for (int j = 0; j < m_nGameObjects; ++j) {
								Item* pGameObjectItem = dynamic_cast<Item*>(m_ppGameObjects[j]);
								if (pGameObjectItem && pGameObjectItem->GetUniqueID() == itemID) {
									m_ppGameObjects[j] = nullptr;
									break;
								}
							}
						}
						auto texIt = m_textureMap.find("inven");
						if (texIt != m_textureMap.end()) {
							auto* pShader = dynamic_cast<CTextureToScreenShader*>(m_ppShaders[i]);
							auto* pShader1 = dynamic_cast<CTextureToScreenShader*>(m_ppShaders[i + 6]);
							if (pShader) {
								pShader->SetTexture(texIt->second);
							}
							if (pShader1) {
								pShader1->SetTexture(texIt->second);
								dynamic_cast<CShopShader*>(m_ppShaders[5])->price[i] = L"0";
							}
						}
					}
				}
			}
		}
	}
	break;
	}
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		{
			int slotIndex = wParam - '1';
			m_pPlayer->m_nSelectedInventoryIndex = slotIndex;

			// 슬롯에 따라 visible 갱신
			for (int i = 0; i < m_pPlayer->m_pHeldItems.size(); ++i)
			{
				CGameObject* item = m_pPlayer->m_pHeldItems[i];
				item->SetVisible(i == slotIndex); // 선택된 슬롯만 true
			}
		}
			break;
		case 'F':
		case 'f':
		{
			bool bPickedUp = false;

			for (int i = 0; i < m_nGameObjects; ++i)
			{
				CGameObject* pObj = m_ppGameObjects[i];
				if (!pObj) continue;

				Item* pItem = dynamic_cast<Item*>(pObj);
				if (!pItem) continue;
				

				XMFLOAT3 playerPos = m_pPlayer->GetPosition();
				XMFLOAT3 itemPos = pItem->GetPosition();
				float distance = Vector3::Length(Vector3::Subtract(playerPos, itemPos));
				if (distance > 0.5f) continue;
				if (m_pPlayer->TryPickUpItem(pItem))

				{
					std::string frameName = pItem->m_pstrFrameName;
					auto it = m_textureMap.find(frameName);
					if (it != m_textureMap.end())
					{
						int newIndex = static_cast<int>(m_pPlayer->m_pHeldItems.size()) - 1;
						if (newIndex < 4)
						{
							auto* pShader = dynamic_cast<CTextureToScreenShader*>(m_ppShaders[newIndex]);
							auto* pShader1 = dynamic_cast<CTextureToScreenShader*>(m_ppShaders[newIndex + 6]);
							if (pShader) pShader->SetTexture(it->second);
							if (pShader1)
							{
								pShader1->SetTexture(it->second);
								dynamic_cast<CShopShader*>(m_ppShaders[5])->price[newIndex] = std::to_wstring(pItem->Getprice());
							}
						}
					}
					bPickedUp = true;
					break;
				}
			}

			if (!bPickedUp)
			{
				int index = m_pPlayer->m_nSelectedInventoryIndex;
				if (m_pPlayer->DropItem(index))
				{
					auto it = m_textureMap.find("inven");
					if (it != m_textureMap.end())
					{
						auto* pShader = dynamic_cast<CTextureToScreenShader*>(m_ppShaders[index]);
						auto* pShader1 = dynamic_cast<CTextureToScreenShader*>(m_ppShaders[index + 6]);
						if (pShader) pShader->SetTexture(it->second);
						if (pShader1)
						{
							//pShader1->SetTexture(it->second); 얘 때문에 이상한 텍스처가 나옴 ㅠㅠㅠ
							dynamic_cast<CShopShader*>(m_ppShaders[5])->price[index] = L"0";
						}
					}
				}
			}

			break;
		}
		case VK_TAB:		
			isShop = !isShop;
			for (int i = 5; i < 10; ++i)
				dynamic_cast<CTextureToScreenShader*>(m_ppShaders[i])->visible = isShop;
			break;
		case VK_UP:
			dynamic_cast<CTerrainPlayer*>(m_pPlayer)->currentHP -= 10.f;
			break;
		}
		break;
		
	default:
		break;
	}
}

bool CScene::ProcessInput(UCHAR *pKeysBuffer)
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) { 
		m_ppGameObjects[i]->Animate(fTimeElapsed); 

		if (m_ppGameObjects[i]->isFalling) {
			XMFLOAT3 pos = m_ppGameObjects[i]->GetPosition();
			if (pos.y > 0.1f)
			{
				pos.y -= 0.1f;
				m_ppGameObjects[i]->SetPosition(pos);
			}
			else m_ppGameObjects[i]->isFalling = false;		
		}
	}
	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->AnimateObjects(fTimeElapsed);

	if (m_pLights)
	{	
		m_pLights[1].m_xmf3Position = m_pPlayer->GetPosition();
		m_pLights[1].m_xmf3Direction = m_pPlayer->GetLookVector();
	}

	if (m_pEffect&& m_ppGameObjects[1]) m_pEffect->Animate(fTimeElapsed, m_ppGameObjects[1]->GetPosition());
}

void CScene::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights

	if (m_pSkyBox) m_pSkyBox->Render(pd3dCommandList, pCamera);
	if (m_pTerrain) m_pTerrain->Render(pd3dCommandList, pCamera);
	if (m_pMap) m_pMap->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) { 
		if (m_ppGameObjects[i]->GetVisible())
		{
			m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
		}
	}
  
	for (int i = 0; i < m_nMonster; i++)
	{
		if (m_ppMonsters[i])
		{
			m_ppMonsters[i]->Animate(m_fElapsedTime);
			//if (!m_ppHierarchicalGameObjects[i]->m_pSkinnedAnimationController) m_ppHierarchicalGameObjects[i]->UpdateTransform(NULL);
			m_ppMonsters[i]->Render(pd3dCommandList, pCamera);
		}
	}

	m_CollisionManager.Update(m_pPlayer);
	
	if (m_pEffect) m_pEffect->Render(pd3dCommandList, pCamera);

	for (int i = 0; i < m_nOtherPlayers; ++i) 
	{
		//if (m_ppOtherPlayers[i]->isConnedted)
		//m_ppOtherPlayers[i]->Animate(m_ppOtherPlayers[i]->animation, m_fElapsedTime);
		m_ppOtherPlayers[i]->Render(pd3dCommandList, pCamera);
	}

	for (int i = 0; i < m_nShaders; i++) if (dynamic_cast<CTextureToScreenShader*>(m_ppShaders[i])->visible) { 
		m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	}
}

// 아이템 생성 server(민상.ver AddItem)

void CScene::AddItem(long long id, ITEM_TYPE type, const XMFLOAT3& position) {
	CLoadedModelInfo* pModel = nullptr;
	Item* pNewItem = nullptr;

	switch (type)
	{
	case ITEM_TYPE_SHOVEL:
		dynamic_cast<Shovel*>(m_ppGameObjects[1])->ChangeExistState(true);
		dynamic_cast<Shovel*>(m_ppGameObjects[1])->SetPosition(position);
		break;
	case ITEM_TYPE_HANDMAP:
		break;
	case ITEM_TYPE_FLASHLIGHT:
		dynamic_cast<FlashLight*>(m_ppGameObjects[0])->ChangeExistState(true);
		dynamic_cast<FlashLight*>(m_ppGameObjects[0])->SetPosition(position);
		break;
	case ITEM_TYPE_WHISTLE:
		dynamic_cast<Whistle*>(m_ppGameObjects[2])->ChangeExistState(true);
		dynamic_cast<Whistle*>(m_ppGameObjects[2])->SetPosition(position);
		break;
	default:
		std::cerr << "[Error] Unknown item type: " << static_cast<int>(type) << std::endl;
		return;
	}

	if (pModel && pNewItem) {
		pNewItem->SetPosition(position);
		pNewItem->SetScale(1.0f, 1.0f, 1.0f); // 기본 스케일 설정

		std::lock_guard<std::mutex> lock(g_item_mutex);
		g_items[id] = pNewItem;
		delete pModel; // 모델 데이터는 복제되었으므로 삭제
	}
}


void CStartScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	CreateCbvSrvDescriptorHeaps(pd3dDevice, 0, 100); 

	m_nShaders = 1;
	m_ppShaders = new CShader * [m_nShaders];

	CTextureToScreenShader* pTextureToScreenShader = new CTextureToScreenShader(1);
	pTextureToScreenShader->CreateShader(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	CTexture* pTexture = new CTexture(1, RESOURCE_TEXTURE2D, 0, 1);
	pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/StartScene.dds", RESOURCE_TEXTURE2D, 0);
	//pTexture->LoadTextureFromDDSFile(pd3dDevice, pd3dCommandList, L"Image/Font.dds", RESOURCE_TEXTURE2D, 1);

	CreateShaderResourceViews(pd3dDevice, pTexture, 0, 15);

	CScreenRectMeshTextured* pMesh = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, -1.0f, 2.0f, 1.0f, 2.0f);
	pTextureToScreenShader->SetMesh(0, pMesh);
	//pMesh = new CScreenRectMeshTextured(pd3dDevice, pd3dCommandList, -1.f, 0.12f * 36, -0.5f, 0.20f, 1);
	//pTextureToScreenShader->SetMesh(1, pMesh);
	pTextureToScreenShader->SetTexture(pTexture);

	m_ppShaders[0] = pTextureToScreenShader;

	m_nGameObjects = 2;
	m_ppGameObjects = new CGameObject * [m_nGameObjects];

	m_pFontID = new CText(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Enter ID : ", -0.5f, -0.25f);
	m_ppGameObjects[0] = m_pFontID;

	m_pFontIP = new CText(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, L"Enter IP : ", -0.5f, -0.55f);
	m_ppGameObjects[1] = m_pFontIP;


	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CStartScene::ReleaseObjects()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}

	if (m_ppGameObjects)
	{
		for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Release();
		delete[] m_ppGameObjects;
	}
}

void CStartScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	if (m_pd3dGraphicsRootSignature) pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	if (m_pd3dCbvSrvDescriptorHeap) pd3dCommandList->SetDescriptorHeaps(1, &m_pd3dCbvSrvDescriptorHeap);

	pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	pCamera->UpdateShaderVariables(pd3dCommandList);

	UpdateShaderVariables(pd3dCommandList);

	for (int i = 0; i < m_nShaders; i++) if (m_ppShaders[i]) m_ppShaders[i]->Render(pd3dCommandList, pCamera);
	for (int i = 0; i < m_nGameObjects; i++) if (m_ppGameObjects[i]) m_ppGameObjects[i]->Render(pd3dCommandList, pCamera);
}

void CStartScene::AnimateObjects(float fTimeElapsed)
{
	if (m_textDirty && m_inputStep == InputStep::EnterID)
	{
		wstring id;
		id.assign(m_inputID.begin(), m_inputID.end());

		m_pFontID->UpdateText(id, L"Enter ID : ");
		m_textDirty = false;
	}
	if (m_textDirty && m_inputStep == InputStep::EnterIP)
	{
		wstring ip;
		ip.assign(m_inputIP.begin(), m_inputIP.end());

		m_pFontIP->UpdateText(ip, L"Enter IP : ");
		m_textDirty = false;
	}
}

void CStartScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_networkInitialized) return;

	switch (nMessageID)
	{
	case WM_KEYDOWN:
		if (m_inputStep == InputStep::EnterID) {
			if (wParam == VK_RETURN) {
				m_inputStep = InputStep::EnterIP;
				::user_name = m_inputID;
			}
			else if (wParam == VK_BACK && !m_inputID.empty()) {
				m_inputID.pop_back();
				m_textDirty = true;
			}
			else if (isprint(wParam) && m_inputID.length() < MAX_ID_LENGTH - 1) {
				m_inputID.push_back((char)wParam);
				m_textDirty = true;
			}
			
			break;
		}
		if (m_inputStep == InputStep::EnterIP) {
			if (isdigit(wParam) && m_inputIP.length() < 15) {
				m_inputIP.push_back((char)wParam);
				m_textDirty = true;
			}
			else if (wParam == 190) {
				m_inputIP.push_back('.');
				m_textDirty = true;
			}

			else if (wParam == VK_RETURN) {
				m_inputStep = InputStep::Done;
				m_networkInitialized = true;

				char serverIP[16];
				strcpy(serverIP, m_inputIP.c_str());
				std::cout << "Connecting to: " << serverIP << std::endl;
				InitializeNetwork(serverIP); // server IP 전달
				gGameFramework.MoveToNextScene();
			}
			else if (wParam == VK_BACK && !m_inputIP.empty()) {
				m_inputIP.pop_back();
			}
		}
		break;
	}
}


void CEndScene::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
}

void CEndScene::ReleaseObjects()
{
}

void CEndScene::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
}

void CEndScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
}
