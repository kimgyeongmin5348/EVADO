#include "stdafx.h"
#include "Map.h"

Map::Map(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	LoadMapObjectsFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	LoadGeometryFromFile();
	SetMapObjects();
}

Map::~Map()
{
}

string Map::ReadString(std::ifstream& inFile)
{
    uint8_t length;
    inFile.read(reinterpret_cast<char*>(&length), sizeof(uint8_t));
    std::string str(length, '\0');
    inFile.read(&str[0], length);
    return str;
}

// setter.bin 을 읽어와 m_vObjectInstances에 오브젝트 이름(modelIndex), pos, rot, scl 저장
void Map::LoadGeometryFromFile()
{
    std::ifstream inFile("Model/Map_design_objects_setter.bin", std::ios::binary);
    if (!inFile)
    {
        std::cerr << "Error: Could not open Map_design_objects_setter.bin" << std::endl;
        return;
    }

    m_vObjectInstances.clear();

    // RootObject 정보 읽기
    std::string frameTag = ReadString(inFile);
    if (frameTag != "<Frame>:")
    {
        std::cerr << "Error: Invalid format (expected <Frame>:)" << std::endl;
        return;
    }
    std::string rootObjectName = ReadString(inFile);

    std::string transformTag = ReadString(inFile);

    DirectX::XMFLOAT3 rootPosition, rootRotation, rootScale;
    DirectX::XMFLOAT4 rootQuaternion;
    float rootMatrix[16];

    inFile.read(reinterpret_cast<char*>(&rootPosition), sizeof(DirectX::XMFLOAT3));
    inFile.read(reinterpret_cast<char*>(&rootRotation), sizeof(DirectX::XMFLOAT3));
    inFile.read(reinterpret_cast<char*>(&rootScale), sizeof(DirectX::XMFLOAT3));
    inFile.read(reinterpret_cast<char*>(&rootQuaternion), sizeof(DirectX::XMFLOAT4));

    std::string matrixTag = ReadString(inFile);
    inFile.read(reinterpret_cast<char*>(rootMatrix), sizeof(float) * 16);
    std::string childrenTag = ReadString(inFile);

    int childCount;
    inFile.read(reinterpret_cast<char*>(&childCount), sizeof(int));

    // 자식 오브젝트 읽기 (Level 1)
    for (int i = 0; i < childCount; i++)
    {
        std::string childFrameTag = ReadString(inFile);
        if (childFrameTag != "<Frame>:")
        {
            std::cerr << "Error: Invalid format (expected <Frame>:)" << std::endl;
            return;
        }

        std::string objectName = ReadString(inFile);

        std::string childTransformTag = ReadString(inFile);
        DirectX::XMFLOAT3 position, rotation, scale;
        DirectX::XMFLOAT4 quaternion;
        float matrix[16];

        inFile.read(reinterpret_cast<char*>(&position), sizeof(DirectX::XMFLOAT3));
        inFile.read(reinterpret_cast<char*>(&rotation), sizeof(DirectX::XMFLOAT3));
        inFile.read(reinterpret_cast<char*>(&scale), sizeof(DirectX::XMFLOAT3));
        inFile.read(reinterpret_cast<char*>(&quaternion), sizeof(DirectX::XMFLOAT4));

        std::string childMatrixTag = ReadString(inFile);

        inFile.read(reinterpret_cast<char*>(matrix), sizeof(float) * 16);

        std::string endFrameTag = ReadString(inFile);
        if (endFrameTag != "</Frame>")
        {
            std::cerr << "Error: Invalid format (expected </Frame>)" << std::endl;
            return;
        }

        m_vObjectInstances.emplace_back(objectName, position, rotation, scale, quaternion, matrix);
    }

    inFile.close();
}

// Model/Map 안의 모든 .bin 파일을 불러와 m_vLoadedModelInfo에 저장
void Map::LoadMapObjectsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	std::filesystem::path path{ "Model/Map" };

	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
		std::cerr << "Error: Directory not found -> " << path << std::endl;
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		if (entry.path().extension() == ".bin") {
			CLoadedModelInfo* pModelInfo = CGameObject::LoadGeometryAndAnimationFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature, entry.path(), nullptr);

			if (pModelInfo) {
				m_vLoadedModelInfo.push_back(pModelInfo->m_pModelRootObject);
			}
		}
	}
}

void Map::SetMapObjects()
{

}

void Map::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	//CGameObject::Render(pd3dCommandList, pCamera);

	//for (auto& pObject : m_vHierarchicalMapObjects)
	//{
	//	if (pObject)
	//	{
	//		pObject->Render(pd3dCommandList, pCamera);
	//	}
	//}
}
