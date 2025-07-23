#pragma once

#include "Object.h"

class OtherPlayer : public CGameObject
{
public:
	//void SetPosition(const XMFLOAT3& position);
	//void SetMovement(bool isMoving);
	OtherPlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel);
	virtual ~OtherPlayer();

    bool isJump = false;

    int animation = 0;

    XMFLOAT3					m_xmf3Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    XMFLOAT3					m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
    XMFLOAT3					m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
    XMFLOAT3					m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
    
    XMFLOAT3                    m_xmf3Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

    float           			m_fPitch = 0.0f;
    float           			m_fYaw = 0.0f;
    float           			m_fRoll = 0.0f;

    bool isConnedted = false;

public:
	virtual void Animate(int animation, float fTimeElapsed);

    void Rotate(XMFLOAT3 look, XMFLOAT3 right)
    {
        XMVECTOR lookVector = XMLoadFloat3(&look);
        XMVECTOR rightVector = XMLoadFloat3(&right);
        XMVECTOR upVector = XMVector3Cross(lookVector, rightVector);

        lookVector = XMVector3Normalize(lookVector);
        rightVector = XMVector3Normalize(rightVector);
        upVector = XMVector3Normalize(upVector);

        XMStoreFloat3(&m_xmf3Look, lookVector);
        XMStoreFloat3(&m_xmf3Right, rightVector);
        XMStoreFloat3(&m_xmf3Up, upVector);
    }

    virtual void OnPrepareRender()
    {
        m_xmf4x4ToParent._11 = m_xmf3Right.x; m_xmf4x4ToParent._12 = m_xmf3Right.y; m_xmf4x4ToParent._13 = m_xmf3Right.z;
        m_xmf4x4ToParent._21 = m_xmf3Up.x; m_xmf4x4ToParent._22 = m_xmf3Up.y; m_xmf4x4ToParent._23 = m_xmf3Up.z;
        m_xmf4x4ToParent._31 = m_xmf3Look.x; m_xmf4x4ToParent._32 = m_xmf3Look.y; m_xmf4x4ToParent._33 = m_xmf3Look.z;
        //m_xmf4x4ToParent._41 = m_xmf3Position.x; m_xmf4x4ToParent._42 = m_xmf3Position.y; m_xmf4x4ToParent._43 = m_xmf3Position.z;

        m_xmf4x4ToParent = Matrix4x4::Multiply(XMMatrixScaling(m_xmf3Scale.x, m_xmf3Scale.y, m_xmf3Scale.z), m_xmf4x4ToParent);
    }
};