#pragma once

#include "Object.h"

struct MapObjectInstance
{
    std::string objectName;
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    DirectX::XMFLOAT3 scale;
    DirectX::XMFLOAT4 quaternion;
    float transformMatrix[16];

    MapObjectInstance(std::string name, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 scl, DirectX::XMFLOAT4 quat, float matrix[16])
        : objectName(name), position(pos), rotation(rot), scale(scl), quaternion(quat)
    {
        std::copy(matrix, matrix + 16, transformMatrix);
    }
};

class Map : public CGameObject
{
public:
	Map(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Map();

	void AddMapSection(int modelIndex, const XMFLOAT3& position, const XMFLOAT3& rotation, const XMFLOAT3& scale);
	void LoadMapObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void LoadGeometryFromFile();
	void SetMapObjects();

    string ReadString(ifstream& inFile);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

public:
	std::vector<MapObjectInstance> m_vObjectInstances;
	std::vector<CGameObject*> m_vLoadedModelInfo;
};