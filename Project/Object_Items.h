#pragma once

#include "Object.h"

class Item : public CGameObject
{
//private:
//	long long m_id = -1;  // ���� ID����
//	bool m_bIsHeld = false;

public:
	//Item(int nMaterials = 0) : CGameObject(nMaterials), m_id(-1) {}

	Item() {};
	virtual ~Item() {};

	virtual void Animate(float fTimeElapsed);
	void ChangeExistState(bool isExist);

	bool is_exist = false;

	//// ���� ID ����
	//void SetUniqueID(long long id) { m_id = id; }
	//// ���� ID ��ȯ
	//long long GetUniqueID() const { return m_id; }
	//void SetHeld(bool held) { m_bIsHeld = held; }
	//bool IsHeld() const { return m_bIsHeld; }

protected:
	float m_fFallingSpeed; // ���� �ӵ�
};



class Shovel : public Item
{
public:
	Shovel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel);
	virtual ~Shovel();

	int				m_damage = 1;
	bool			m_bIsSwingActive = false;
	BoundingBox		m_attackBoundingBox;

	BoundingBox GetattackBoundingBox() { return m_attackBoundingBox; };

	void ProccessSwing();
	void GenerateSwingBoundingBox(XMFLOAT3 playerPos, XMFLOAT3 playerLook);  // �ֵθ� �� �ٿ�� �ڽ� ����
	void UpdateSwingBoundingBox(); // �ٿ�� �ڽ� ������Ʈ
	void DeleteSwingBoundingBox(); // �ٿ�� �ڽ� ����
};

class Handmap : public Item
{
public:
	Handmap(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel);
	virtual ~Handmap();

};


class FlashLight : public Item
{
public:
	FlashLight(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel);
	virtual ~FlashLight();

};

class Whistle : public Item
{
public:
	Whistle(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature, CLoadedModelInfo* pModel);
	virtual ~Whistle();

};