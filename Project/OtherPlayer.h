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
	virtual void Update(float fTimeElapsed);

	bool isJump = false;

	bool isConnedted = false;

	XMFLOAT3 m_xmf3Position;
};