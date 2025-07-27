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
	if (m_pSkinnedAnimationController)
	{
		if (m_animBlend.active)
		{
			m_animBlend.elapsed += fTimeElapsed;
			float t = m_animBlend.elapsed / m_animBlend.duration;
			if (t >= 1.0f)
			{
				t = 1.0f;
				m_animBlend.active = false;
				for (int i = 0; i < 7; ++i)
					m_pSkinnedAnimationController->SetTrackEnable(i, i == m_animBlend.to);

				m_pSkinnedAnimationController->SetTrackWeight(m_animBlend.to, 1.0f);
				m_pSkinnedAnimationController->SetTrackWeight(m_animBlend.from, 0.0f);
			}
			else
			{
				m_pSkinnedAnimationController->SetTrackWeight(m_animBlend.from, 1.0f - t);
				m_pSkinnedAnimationController->SetTrackWeight(m_animBlend.to, t);
			}
		}
		switch (animation)
		{
		case 3:
			PlayAnimationTrack(3, 2.0f);
			if (IsAnimationFinished(3)) {
				m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);
				animation = 0;
			}
			break;
		case 4:
			PlayAnimationTrack(4, 2.0f);
			if (IsAnimationFinished(4)) {
				m_pSkinnedAnimationController->SetTrackPosition(4, 0.0f);
				animation = 0;
			}
			break;
		case 5:
			PlayAnimationTrack(5);
			break;
		case 6:
			PlayAnimationTrack(6);
			break;
		case 2:
			PlayAnimationTrack(2);
			break;
		case 1:
			PlayAnimationTrack(1);
			break;
		case 0:
		default:
			PlayAnimationTrack(0);
			break;
		}
	}
	CGameObject::Animate(fTimeElapsed);
}

void OtherPlayer::PlayAnimationTrack(int trackIndex, float speed)
{
	if (currentAnim == trackIndex) return;

	StartAnimationBlend(currentAnim, trackIndex, 0.3f);
	m_pSkinnedAnimationController->SetTrackSpeed(trackIndex, speed);

	currentAnim = trackIndex;
}

bool OtherPlayer::IsAnimationFinished(int trackIndex)
{
	float current = m_pSkinnedAnimationController->m_pAnimationTracks[trackIndex].m_fPosition;
	float length = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[trackIndex]->m_fLength;
	return current >= length;
}

void OtherPlayer::StartAnimationBlend(int fromTrack, int toTrack, float blendTime)
{
	m_animBlend.from = fromTrack;
	m_animBlend.to = toTrack;
	m_animBlend.duration = blendTime;
	m_animBlend.elapsed = 0.0f;
	m_animBlend.active = true;

	for (int i = 0; i < 7; ++i)
		m_pSkinnedAnimationController->SetTrackEnable(i, i == fromTrack || i == toTrack);

	m_pSkinnedAnimationController->SetTrackWeight(fromTrack, 1.0f);
	m_pSkinnedAnimationController->SetTrackWeight(toTrack, 0.0f);

}
