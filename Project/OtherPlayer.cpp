#include "stdafx.h"
#include "OtherPlayer.h"

OtherPlayer::OtherPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
	CLoadedModelInfo* pPlayerModel = pModel;
	if (!pPlayerModel) pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Player.bin", NULL);

	SetChild(pPlayerModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 5, pPlayerModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0); // idle
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1); // walk
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2); // run
	m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3); // jump
	m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4); // swing
	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetTrackEnable(4, false);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//if (pPlayerModel) delete pPlayerModel;
}

OtherPlayer::~OtherPlayer()
{
}

//void OtherPlayer::SetPosition(const XMFLOAT3& position) {
//	m_xmf3Position = position;
//	UpdateTransform();
//}

void OtherPlayer::Update(float fTimeElapsed)
{
	// 위치 및 회전 업데이트
	//XMMATRIX xmTranslate = XMMatrixTranslation(
	//	m_xmf3Position.x,
	//	m_xmf3Position.y,
	//	m_xmf3Position.z
	//);

	// 애니메이션
}
