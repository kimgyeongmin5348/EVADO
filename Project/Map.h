#pragma once

#include "Object.h"

struct MapObjectInstance
{
    int modelIndex;           // m_vLoadedModelInfo에서 참조할 모델의 인덱스
    std::string objectName;
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    DirectX::XMFLOAT3 scale;
    DirectX::XMFLOAT4 quaternion;
    float transformMatrix[16];

    MapObjectInstance(int idx, std::string name, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 scl, DirectX::XMFLOAT4 quat, float matrix[16])
        : modelIndex(idx), objectName(name), position(pos), rotation(rot), scale(scl), quaternion(quat)
    {
        std::copy(matrix, matrix + 16, transformMatrix);
    }
};

class Map : public CGameObject
{
public:
	Map(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	virtual ~Map();

	void LoadMapObjectsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature);
	void LoadGeometryFromFile();
	void SetMapObjects();

    string ReadString(ifstream& inFile);

	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera = NULL);

public:
	std::vector<MapObjectInstance> m_vObjectInstances;
	std::vector<CGameObject*> m_vLoadedModelInfo;
    std::vector<CGameObject*> m_vMapObjects;
};