#include "stdafx.h"
#include "OtherPlayer.h"

OtherPlayer::OtherPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel)
{
	CLoadedModelInfo* pPlayerModel = pModel;
	if (!pPlayerModel) pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Player.bin", NULL);

	SetChild(pPlayerModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 7, pPlayerModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0); // ±âº»
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1); // °È±â
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2); // ¶Ù±â
	m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3); // Á¡ÇÁ
	m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4); // ÈÖµÎ¸£±â
	m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5); // ¿õÅ©¸®±â
	m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6); // ¿õÅ©¸®°í °È±â

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
	for (int i = 0; i < 7; ++i) { 
		m_pSkinnedAnimationController->SetTrackEnable(i, false); 
		m_pSkinnedAnimationController->SetTrackWeight(i, 0.0f);
	}
	m_pSkinnedAnimationController->SetTrackEnable(animation, true);
	m_pSkinnedAnimationController->SetTrackWeight(animation, 1.0f);

	if (animation == 0) {
		m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(4, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(5, 0.0f);
		m_pSkinnedAnimationController->SetTrackPosition(6, 0.0f);
	}
	else if (animation == 3 || animation == 4) {
		m_pSkinnedAnimationController->SetTrackEnable(animation, true);
		m_pSkinnedAnimationController->SetTrackSpeed(animation, 2.0f);
		float currentPos = m_pSkinnedAnimationController->m_pAnimationTracks[animation].m_fPosition;
		int animSetIndex = m_pSkinnedAnimationController->m_pAnimationTracks[animation].m_nAnimationSet;
		float length = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[animSetIndex]->m_fLength;

		if (currentPos >= 1.5)
		{
			m_pSkinnedAnimationController->SetTrackEnable(animation, false);
			m_pSkinnedAnimationController->SetTrackPosition(animation, 0.0f);
		}
	}

	Rotate()

	CGameObject::Animate(fTimeElapsed);
}
