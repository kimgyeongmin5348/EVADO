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
	m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5); // 旷农府扁
	m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6); // 旷农府绊 叭扁

	m_pSkinnedAnimationController->SetTrackEnable(1, false);
	m_pSkinnedAnimationController->SetTrackEnable(2, false);
	m_pSkinnedAnimationController->SetTrackEnable(3, false);
	m_pSkinnedAnimationController->SetTrackEnable(4, false);
	m_pSkinnedAnimationController->SetTrackEnable(5, false);
	m_pSkinnedAnimationController->SetTrackEnable(6, false);

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

void OtherPlayer::Animate(int animation, float fTimeElapsed)
{
	for (int i = 0; i < 7; ++i) m_pSkinnedAnimationController->SetTrackEnable(i, false);
	m_pSkinnedAnimationController->SetTrackEnable(animation, true);

	if (animation == 0) {
		m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(4, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(5, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(6, 0.0f);
	}
	else if (animation == 3) {
		m_pSkinnedAnimationController->SetTrackSpeed(animation, 2.0f);
		float currentPos = m_pSkinnedAnimationController->m_pAnimationTracks[3].m_fPosition;
		int animSetIndex = m_pSkinnedAnimationController->m_pAnimationTracks[3].m_nAnimationSet;
		float length = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[animSetIndex]->m_fLength;

		if (currentPos >= 1.5)
		{
			m_pSkinnedAnimationController->SetTrackEnable(animation, false);
			m_pSkinnedAnimationController->SetTrackPosition(animation, 0.0f);
		}
	}
	else if (animation == 4) {
		m_pSkinnedAnimationController->SetTrackSpeed(4, 2.0f);
		float currentPos = m_pSkinnedAnimationController->m_pAnimationTracks[4].m_fPosition;
		int animSetIndex = m_pSkinnedAnimationController->m_pAnimationTracks[4].m_nAnimationSet;
		float length = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[animSetIndex]->m_fLength;

		if (currentPos >= 1.5)
		{
			m_pSkinnedAnimationController->SetTrackEnable(4, false);
			m_pSkinnedAnimationController->SetTrackPosition(4, 0.0f);
		}
	}


	CGameObject::Animate(fTimeElapsed);
}
