//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Player.h"
#include "Shader.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

CPlayer::CPlayer()
{
	

	m_pCamera = NULL;

	m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3Velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3Gravity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMaxVelocityXZ = 0.0f;
	m_fMaxVelocityY = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_pPlayerUpdatedContext = NULL;
	m_pCameraUpdatedContext = NULL;

	//server
	m_pCamera = new CCamera();
	m_pCamera->SetPosition({ 0,0,0 }); 
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();

	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList)
{
}

void CPlayer::ReleaseShaderVariables()
{
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		//if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		//if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{

	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		m_pCamera->Move(xmf3Shift);
	}
	CalculateBoundingBox();
}

void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if ((nCurrentCameraMode == FIRST_PERSON_CAMERA) || (nCurrentCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > -42.0f) { x -= (m_fPitch + 42.0f); m_fPitch = -42.0f; }
			if (m_fPitch < -200.0f) { x -= (m_fPitch + 200.0f); m_fPitch = -200.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::CalculateBoundingBox()
{
	std::vector<CGameObject*> nodesToProcess = { this };
	bool isFirst = true;
	BoundingBox mergedBox;

	while (!nodesToProcess.empty())
	{
		CGameObject* current = nodesToProcess.back();
		nodesToProcess.pop_back();

		if (current->m_pMesh)
		{
			BoundingBox localBox = current->m_pMesh->GetBoundingBox();
			BoundingBox transformedBox;

			localBox.Transform(transformedBox, XMLoadFloat4x4(&current->m_xmf4x4World));

			if (isFirst)
			{
				mergedBox = transformedBox;
				isFirst = false;
			}
			else
			{
				BoundingBox::CreateMerged(mergedBox, mergedBox, transformedBox);
			}
		}

		if (current->m_pChild)
		{
			CGameObject* child = current->m_pChild;
			nodesToProcess.push_back(child);

			while (child->m_pSibling)
			{
				child = child->m_pSibling;
				nodesToProcess.push_back(child);
			}
		}
	}

	float diameter = std::max(mergedBox.Extents.x, mergedBox.Extents.z) * 2.0f;
	m_BoundingCylinder.Radius = diameter * 0.5f;
	m_BoundingCylinder.Height = mergedBox.Extents.y * 2.0f;
	m_BoundingCylinder.Center = mergedBox.Center;

	// 3. ¿øÅëÀ» °¨½Î´Â AABB·Î º¯È¯
	ConvertCylinderToAABB(m_BoundingCylinder, m_BoundingBox);
}

void CPlayer::Update(float fTimeElapsed)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, m_xmf3Gravity);

	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ;
	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}

	float fMaxVelocityY = m_fMaxVelocityY;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	XMFLOAT3 xmf3Velocity = Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false);
	Move(xmf3Velocity, false);

	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

	DWORD nCurrentCameraMode = m_pCamera->GetMode();
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) { 
		m_pCamera->Update(m_xmf3Position, fTimeElapsed); 
		m_pCamera->SetLookAt(m_xmf3Position); // ÇÃ·¹ÀÌ¾î°¡ È¸Àü ½Ã Ä«¸Þ¶óµµ È¸Àü
	}
	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);
	if (nCurrentCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);
	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * fTimeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

CCamera *CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera = NULL;
	switch (nNewCameraMode)
	{
		case FIRST_PERSON_CAMERA:
			pNewCamera = new CFirstPersonCamera(m_pCamera);
			break;
		case THIRD_PERSON_CAMERA:
			pNewCamera = new CThirdPersonCamera(m_pCamera);
			break;
		case SPACESHIP_CAMERA:
			pNewCamera = new CSpaceShipCamera(m_pCamera);
			break;
	}
	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}

	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}

	if (m_pCamera) delete m_pCamera;

	return(pNewCamera);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4ToParent._11 = m_xmf3Right.x; m_xmf4x4ToParent._12 = m_xmf3Right.y; m_xmf4x4ToParent._13 = m_xmf3Right.z;
	m_xmf4x4ToParent._21 = m_xmf3Up.x; m_xmf4x4ToParent._22 = m_xmf3Up.y; m_xmf4x4ToParent._23 = m_xmf3Up.z;
	m_xmf4x4ToParent._31 = m_xmf3Look.x; m_xmf4x4ToParent._32 = m_xmf3Look.y; m_xmf4x4ToParent._33 = m_xmf3Look.z;
	m_xmf4x4ToParent._41 = m_xmf3Position.x; m_xmf4x4ToParent._42 = m_xmf3Position.y; m_xmf4x4ToParent._43 = m_xmf3Position.z;

	m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
}

void CPlayer::Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA) CGameObject::Render(pd3dCommandList, pCamera);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
#define _WITH_DEBUG_CALLBACK_DATA

//void CSoundCallbackHandler::HandleCallback(void *pCallbackData, float fTrackPosition)
//{
//   _TCHAR *pWavName = (_TCHAR *)pCallbackData; 
//#ifdef _WITH_DEBUG_CALLBACK_DATA
//	TCHAR pstrDebug[256] = { 0 };
//	_stprintf_s(pstrDebug, 256, _T("%s(%f)\n"), pWavName, fTrackPosition);
//	OutputDebugString(pstrDebug);
//#endif
//#ifdef _WITH_SOUND_RESOURCE
//   PlaySound(pWavName, ::ghAppInstance, SND_RESOURCE | SND_ASYNC);
//#else
//   PlaySound(pWavName, NULL, SND_FILENAME | SND_ASYNC);
//#endif
//}

CTerrainPlayer::CTerrainPlayer(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, void *pContext)
{
	m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);

	CLoadedModelInfo *pPlayerModel = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, "Model/Player.bin", NULL);
	SetChild(pPlayerModel->m_pModelRootObject, true);

	m_pSkinnedAnimationController = new CAnimationController(pd3dDevice, pd3dCommandList, 7, pPlayerModel);
	m_pSkinnedAnimationController->SetTrackAnimationSet(0, 0); // ±âº»
	m_pSkinnedAnimationController->SetTrackAnimationSet(1, 1); // °È±â
	m_pSkinnedAnimationController->SetTrackAnimationSet(2, 2); // ¶Ù±â
	m_pSkinnedAnimationController->SetTrackAnimationSet(3, 3); // ÈÖµÎ¸£±â
	m_pSkinnedAnimationController->SetTrackAnimationSet(4, 4); // Á¡ÇÁ
	m_pSkinnedAnimationController->SetTrackAnimationSet(5, 5); // ¿õÅ©¸®±â
	m_pSkinnedAnimationController->SetTrackAnimationSet(6, 6); // ¿õÅ©¸®°í °È±â
	m_pSkinnedAnimationController->SetTrackEnable(1, false); 
	m_pSkinnedAnimationController->SetTrackEnable(2, false); 
	m_pSkinnedAnimationController->SetTrackEnable(3, false); 
	m_pSkinnedAnimationController->SetTrackEnable(4, false); 
	m_pSkinnedAnimationController->SetTrackEnable(5, false); 
	m_pSkinnedAnimationController->SetTrackEnable(6, false); 

	m_pSkinnedAnimationController->SetCallbackKeys(1, 2);
#ifdef _WITH_SOUND_RESOURCE
	m_pSkinnedAnimationController->SetCallbackKey(0, 0.1f, _T("Footstep01"));
	m_pSkinnedAnimationController->SetCallbackKey(1, 0.5f, _T("Footstep02"));
	m_pSkinnedAnimationController->SetCallbackKey(2, 0.9f, _T("Footstep03"));
#else
	//m_pSkinnedAnimationController->SetCallbackKey(1, 0, 0.2f, _T("Sound/Footstep01.wav"));
	//m_pSkinnedAnimationController->SetCallbackKey(1, 1, 0.5f, _T("Sound/Footstep02.wav"));
//	m_pSkinnedAnimationController->SetCallbackKey(1, 2, 0.39f, _T("Sound/Footstep03.wav"));
#endif
	//CAnimationCallbackHandler *pAnimationCallbackHandler = new CSoundCallbackHandler();
	//m_pSkinnedAnimationController->SetAnimationCallbackHandler(1, pAnimationCallbackHandler);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
	
	SetPlayerUpdatedContext(pContext);
	SetCameraUpdatedContext(pContext);

	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)pContext;
	SetPosition(XMFLOAT3(0, 0, 0));
	//SetScale(XMFLOAT3(10.0f, 10.0f, 10.0f));

	if (pPlayerModel) delete pPlayerModel;
}

CTerrainPlayer::~CTerrainPlayer()
{
}

CCamera *CTerrainPlayer::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
	switch (nNewCameraMode)
	{
		case FIRST_PERSON_CAMERA:
			SetFriction(500.0f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(300.0f);
			SetMaxVelocityY(800.0f);
			m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.0f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 1.5f, -0.15f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		case SPACESHIP_CAMERA:
			SetFriction(125.0f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(300.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.0f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		case THIRD_PERSON_CAMERA:
			SetFriction(250.0f);
			SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
			SetMaxVelocityXZ(300.0f);
			SetMaxVelocityY(400.0f);
			m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
			m_pCamera->SetTimeLag(0.25f);
			m_pCamera->SetOffset(XMFLOAT3(0.0f, 5.0f, -2.5f));
			m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
			m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
			m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
			break;
		default:
			break;
	}
	m_pCamera->SetPosition(Vector3::Add(m_xmf3Position, m_pCamera->GetOffset()));
	Update(fTimeElapsed);

	if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA) ((CThirdPersonCamera*)m_pCamera)->Rotate(-90.0f, 0.0f, 0.0f);
	
	return(m_pCamera);
}

void CTerrainPlayer::OnPlayerUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pPlayerUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3PlayerPosition = GetPosition();
	int z = (int)(xmf3PlayerPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3PlayerPosition.x, xmf3PlayerPosition.z, bReverseQuad) + 0.0f;
	if (xmf3PlayerPosition.y < fHeight)
	{
		XMFLOAT3 xmf3PlayerVelocity = GetVelocity();
		xmf3PlayerVelocity.y = 0.0f;
		SetVelocity(xmf3PlayerVelocity);
		xmf3PlayerPosition.y = fHeight;
		SetPosition(xmf3PlayerPosition);
	}
}

void CTerrainPlayer::OnCameraUpdateCallback(float fTimeElapsed)
{
	CHeightMapTerrain *pTerrain = (CHeightMapTerrain *)m_pCameraUpdatedContext;
	XMFLOAT3 xmf3Scale = pTerrain->GetScale();
	XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
	int z = (int)(xmf3CameraPosition.z / xmf3Scale.z);
	bool bReverseQuad = ((z % 2) != 0);
	float fHeight = pTerrain->GetHeight(xmf3CameraPosition.x, xmf3CameraPosition.z, bReverseQuad);
	if (xmf3CameraPosition.y <= fHeight)
	{
		xmf3CameraPosition.y = fHeight;
		m_pCamera->SetPosition(xmf3CameraPosition);
		if (m_pCamera->GetMode() == THIRD_PERSON_CAMERA)
		{
			CThirdPersonCamera *p3rdPersonCamera = (CThirdPersonCamera *)m_pCamera;
			p3rdPersonCamera->SetLookAt(GetPosition());
			p3rdPersonCamera->Rotate(-90.0f, 0 , 0);
		}
	}
}

void CTerrainPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection & DIR_DOWN)
	{
		fDistance *= 1.5f;
	}

	if (!isJump) {
		if ((dwDirection & DIR_DOWN) && (dwDirection & (DIR_FORWARD | DIR_BACKWARD | DIR_LEFT | DIR_RIGHT)))
		{
			m_pSkinnedAnimationController->SetTrackEnable(0, false);
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackEnable(2, true);
			m_pSkinnedAnimationController->SetTrackEnable(3, false);
			m_pSkinnedAnimationController->SetTrackEnable(4, false);
			m_pSkinnedAnimationController->SetTrackEnable(5, false);
			m_pSkinnedAnimationController->SetTrackEnable(6, false);
		}
		else if (dwDirection & (DIR_FORWARD | DIR_BACKWARD | DIR_LEFT | DIR_RIGHT))
		{
			m_pSkinnedAnimationController->SetTrackEnable(0, false);
			m_pSkinnedAnimationController->SetTrackEnable(1, true);
			m_pSkinnedAnimationController->SetTrackEnable(2, false);
			m_pSkinnedAnimationController->SetTrackEnable(3, false);
			m_pSkinnedAnimationController->SetTrackEnable(4, false);
			m_pSkinnedAnimationController->SetTrackEnable(5, false);
			m_pSkinnedAnimationController->SetTrackEnable(6, false);
		}
			
		if ((dwDirection & DIR_CROUCH) && (dwDirection & (DIR_FORWARD | DIR_BACKWARD | DIR_LEFT | DIR_RIGHT)))
		{
			isCrouch = false;
			m_pSkinnedAnimationController->SetTrackEnable(0, false);
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackEnable(2, false);
			m_pSkinnedAnimationController->SetTrackEnable(3, false);
			m_pSkinnedAnimationController->SetTrackEnable(4, false);
			m_pSkinnedAnimationController->SetTrackEnable(5, false);
			m_pSkinnedAnimationController->SetTrackEnable(6, true);
		}
		else if (dwDirection & DIR_CROUCH) isCrouch = true;
		else isCrouch = false;
	}

	if (dwDirection & DIR_UP) isJump = true;

	CPlayer::Move(dwDirection, fDistance, bUpdateVelocity);
}


void CTerrainPlayer::Update(float fTimeElapsed)
{
	CPlayer::Update(fTimeElapsed);

	if (m_pSkinnedAnimationController)
	{
		float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);

		if (isJump) {
			m_pSkinnedAnimationController->SetTrackEnable(0, false);
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackEnable(2, false);
			m_pSkinnedAnimationController->SetTrackEnable(3, true);
			m_pSkinnedAnimationController->SetTrackEnable(4, false);
			m_pSkinnedAnimationController->SetTrackEnable(5, false);
			m_pSkinnedAnimationController->SetTrackEnable(6, false);

			m_pSkinnedAnimationController->SetTrackSpeed(3, 2.0f);

			float currentPos = m_pSkinnedAnimationController->m_pAnimationTracks[3].m_fPosition;
			int animSetIndex = m_pSkinnedAnimationController->m_pAnimationTracks[3].m_nAnimationSet;
			float length = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[animSetIndex]->m_fLength;

			if (currentPos >= 1.5)
			{
				isJump = false;
				m_pSkinnedAnimationController->SetTrackEnable(3, false); // ? ë‹ˆë©”ì´???ë‚¬?¼ë‹ˆê¹?êº?
				m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f); // ?¤ìŒ?????¤í–‰?????ˆê²Œ ì´ˆê¸°??
			}

		}
		else if (isSwing) {
			m_pSkinnedAnimationController->SetTrackEnable(0, false);
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackEnable(2, false);
			m_pSkinnedAnimationController->SetTrackEnable(3, false);
			m_pSkinnedAnimationController->SetTrackEnable(4, true);
			m_pSkinnedAnimationController->SetTrackEnable(5, false);
			m_pSkinnedAnimationController->SetTrackEnable(6, false);

			m_pSkinnedAnimationController->SetTrackSpeed(4, 2.0f);

			float currentPos = m_pSkinnedAnimationController->m_pAnimationTracks[4].m_fPosition;
			int animSetIndex = m_pSkinnedAnimationController->m_pAnimationTracks[4].m_nAnimationSet;
			float length = m_pSkinnedAnimationController->m_pAnimationSets->m_pAnimationSets[animSetIndex]->m_fLength;

			if (currentPos >= 1.5)
			{
				isSwing = false;
				m_pSkinnedAnimationController->SetTrackEnable(4, false);
				m_pSkinnedAnimationController->SetTrackPosition(4, 0.0f);
			}

		}
		else if (isCrouch) {
			m_pSkinnedAnimationController->SetTrackEnable(0, false);
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackEnable(2, false);
			m_pSkinnedAnimationController->SetTrackEnable(3, false);
			m_pSkinnedAnimationController->SetTrackEnable(4, false);
			m_pSkinnedAnimationController->SetTrackEnable(5, true);
			m_pSkinnedAnimationController->SetTrackEnable(6, false);

		}
		else if (::IsZero(fLength))
		{
			m_pSkinnedAnimationController->SetTrackEnable(0, true);
			m_pSkinnedAnimationController->SetTrackEnable(1, false);
			m_pSkinnedAnimationController->SetTrackEnable(2, false);
			m_pSkinnedAnimationController->SetTrackEnable(3, false);
			m_pSkinnedAnimationController->SetTrackEnable(4, false);
			m_pSkinnedAnimationController->SetTrackEnable(5, false);
			m_pSkinnedAnimationController->SetTrackEnable(6, false);

			m_pSkinnedAnimationController->SetTrackPosition(1, 0.0f);
			m_pSkinnedAnimationController->SetTrackPosition(2, 0.0f);
			m_pSkinnedAnimationController->SetTrackPosition(3, 0.0f);
			m_pSkinnedAnimationController->SetTrackPosition(4, 0.0f);
			m_pSkinnedAnimationController->SetTrackPosition(5, 0.0f);
			m_pSkinnedAnimationController->SetTrackPosition(6, 0.0f);
		}
	}


	// server

	// position, look, right ------------------------------------


	static XMFLOAT3 prevPosition = GetPosition();
	static XMFLOAT3 prevLook = GetLook();
	static XMFLOAT3 prevRight = GetRight();

	XMFLOAT3 currPosition = GetPosition();
	XMFLOAT3 currLook = GetLook();
	XMFLOAT3 currRight = GetRight();
	// -----------------------------------------------------------

	// animation ------------------------------------------------
	float fLength = 0.0f;
	if (m_pSkinnedAnimationController)
	{
		fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x +
			m_xmf3Velocity.z * m_xmf3Velocity.z);

		uint8_t currentAnimState = static_cast<uint8_t>(AnimationState::IDLE);
		if (isJump)
			currentAnimState = static_cast<uint8_t>(AnimationState::JUMP); // 4
		else if (isSwing)
			currentAnimState = static_cast<uint8_t>(AnimationState::SWING); // 3
		else if (isCrouch)
			currentAnimState = static_cast<uint8_t>(AnimationState::CROUCH); //5
		else if (::IsZero(fLength))
			currentAnimState = static_cast<uint8_t>(AnimationState::IDLE); //0
		else
			currentAnimState = static_cast<uint8_t>(AnimationState::WALK); //1

		static uint8_t prevAnimState = currentAnimState;
		// -----------------------------------------------------------

		if (currPosition.x != prevPosition.x || currPosition.y != prevPosition.y || currPosition.z != prevPosition.z ||
			currLook.x != prevLook.x || currLook.y != prevLook.y || currLook.z != prevLook.z ||
			currRight.x != prevRight.x || currRight.y != prevRight.y || currRight.z != prevRight.z ||
			currentAnimState != prevAnimState)
		{
			send_position_to_server(currPosition, currLook, currRight, currentAnimState);
			prevPosition = currPosition;
			prevLook = currLook;
			prevRight = currRight;
			prevAnimState = currentAnimState;
		}

	}
}

