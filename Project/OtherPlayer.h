#pragma once

#include "Object.h"

class OtherPlayer : public CGameObject
{
public:
	//void SetPosition(const XMFLOAT3& position);
	//void SetMovement(bool isMoving);
	OtherPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel);
	virtual ~OtherPlayer();

public:
	virtual void Animate(int animation, float fTimeElapsed);

	bool isJump = false;

	int animation = 0;

	XMFLOAT3 m_xmf3Position;
	XMFLOAT3 m_xmf3Look;
	XMFLOAT3 m_xmf3Right;


	bool isConnedted = false;

};