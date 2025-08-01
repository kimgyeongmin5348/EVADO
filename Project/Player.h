#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "Object.h"
#include "Camera.h"
#include "Network.h"
#include "CText.h"

struct BoundingCylinder
{
	XMFLOAT3 Center;
	float Radius;
	float Height;

	BoundingCylinder() : Center(0.0f,0.0f,0.0f), Radius(0.0f), Height(0.0f) {}
};

class CPlayer : public CGameObject
{
protected:
	XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	XMFLOAT3					m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	float           			m_fPitch = 0.0f;
	float           			m_fYaw = 0.0f;
	float           			m_fRoll = 0.0f;

	XMFLOAT3					m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3     				m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	float           			m_fMaxVelocityXZ = 0.0f;
	float           			m_fMaxVelocityY = 0.0f;
	float           			m_fFriction = 0.0f;

	LPVOID						m_pPlayerUpdatedContext = NULL;
	LPVOID						m_pCameraUpdatedContext = NULL;

	CCamera						*m_pCamera = NULL;

	XMFLOAT3			m_lastPushDirection; // 마지막 충돌 방향 저장
	BoundingCylinder	m_BoundingCylinder;

	BoundingBox			m_shovelAttackBoundingBox;

public:
	//bool	isSwing = false;
	//bool	isCrouch = false;
	//bool	isCrouchWalk = false;
	//bool	isJump = false;
	//bool	isRun = false;
	//bool	isWalk = false;
	//bool	isIdle = true;

	bool	m_isMonsterHit = false;
	bool	alreadyHeld = false;
	float currentHP = 100.f;

	// 아이템
	int m_nSelectedInventoryIndex = -1;  // 기본값은 0번 (1번 슬롯)
	std::vector<CGameObject*> m_pHeldItems;
	bool bflashlight = false;
	void RemoveHeldItem(CGameObject* pItem);
public:
	CPlayer();
	virtual ~CPlayer();

	XMFLOAT3 GetPosition() { return(m_xmf3Position); }
	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3 GetPushDirection() const { return m_lastPushDirection; }
	BoundingBox GetShovelAttackBoundingBox() { return m_shovelAttackBoundingBox; }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(const XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(const XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }
	void SetPosition(const XMFLOAT3& xmf3Position) { Move(XMFLOAT3(xmf3Position.x - m_xmf3Position.x, xmf3Position.y - m_xmf3Position.y, xmf3Position.z - m_xmf3Position.z), false); }
	void SetPushDirection(const XMFLOAT3& direction) { m_lastPushDirection = direction; }
	void SetShovelAttadckBoundingBox(const BoundingBox& bbShovelAttack) { m_shovelAttackBoundingBox = bbShovelAttack; }

	void SetScale(XMFLOAT3& xmf3Scale) { m_xmf3Scale = xmf3Scale; }

	const XMFLOAT3& GetVelocity() const { return(m_xmf3Velocity); }
	float GetYaw() const { return(m_fYaw); }
	float GetPitch() const { return(m_fPitch); }
	float GetRoll() const { return(m_fRoll); }

	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	virtual void Move(DWORD nDirection, float fDistance, bool bVelocity = false);
	void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Move(float fx= 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	void Rotate(float x, float y, float z);

	virtual void CalculateBoundingBox() override;
	void ConvertCylinderToAABB(const BoundingCylinder& cylinder, BoundingBox& outBox)
	{
		outBox.Center = cylinder.Center;
		outBox.Extents = XMFLOAT3(cylinder.Radius, cylinder.Height * 0.5f, cylinder.Radius);
	}
	void GenerateShovelAttackBoundingBox();

	virtual void Update(float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed) { }
	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }

	virtual void OnCameraUpdateCallback(float fTimeElapsed) { }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	CCamera *OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);

	// 물건 줍기
	bool TryPickUpItem(CGameObject* pItem);
	bool DropItem(int index);
};

//class CSoundCallbackHandler : public CAnimationCallbackHandler
//{
//public:
//	CSoundCallbackHandler() { }
//	~CSoundCallbackHandler() { }
//
//public:
//	virtual void HandleCallback(void *pCallbackData, float fTrackPosition); 
//};

class CTerrainPlayer : public CPlayer
{
public:
	CTerrainPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext=NULL);
	virtual ~CTerrainPlayer();

	CText* m_pText = nullptr;

	//server
	AnimationState m_currentAnim = AnimationState::IDLE;

public:
	virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);

	virtual void OnPlayerUpdateCallback(float fTimeElapsed);
	virtual void OnCameraUpdateCallback(float fTimeElapsed);

	virtual void Move(DWORD nDirection, float fDistance, bool bVelocity = false);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

	virtual void Update(float fTimeElapsed);

	int debt = 10000;

	CTextureToScreenShader* m_playerHP = NULL;
	ID3D12Device* device = nullptr; 
	ID3D12GraphicsCommandList* cmdList = nullptr;
	void SetHPWidth(float newWidth);

	void PlayAnimationTrack(int trackIndex, float speed = 1.0f);
	bool IsAnimationFinished(int trackIndex);

	AnimationBlend m_animBlend;
	int m_currentTrack = -1;

	void StartAnimationBlend(int fromTrack, int toTrack, float blendTime);

	bool IsShovel();
};


