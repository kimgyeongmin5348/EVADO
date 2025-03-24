#include "stdafx.h"
#include "Map.h"

Map::Map(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	LoadMapObjectsFromFile(pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature);
	LoadGeometryFromFile();
	SetMapObjects();

	//cout << "[ m_vLoadedModelInfo ]" << endl;
	//for (auto& a : m_vLoadedModelInfo) {
	//    cout << a->GetFrameName() << " (" << a->GetPosition().x << ", " << a->GetPosition().z << ")" << endl;
	//}
	//cout << "[ m_vObjectInstances ]" << endl;
	//for (auto& a : m_vObjectInstances) {
	//    cout << "[" << a.modelIndex << "] " << a.objectName << "(" << a.position.x << ", " << a.position.z << ")" << endl;
	//}
	//cout << "[ m_vpMapObjects ]" << endl;
	//for (const auto& a : m_vMapObjects) {
	//	cout << a->GetFrameName() << " | " << a->GetPosition().x << ", " << a->GetPosition().z << " | " << endl;
	//}
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
	std::ifstream inFile("Model/Map/Setter/Map_objects_instances_setter.bin", std::ios::binary);
	if (!inFile)
	{
		std::cerr << "Error: Could not open Map_objects_instances_setter.bin" << std::endl;
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

	XMFLOAT3 rootPosition, rootRotation, rootScale;
	XMFLOAT4 rootQuaternion;
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
		inFile.read(reinterpret_cast<char*>(&position), sizeof(DirectX::XMFLOAT3));
		inFile.read(reinterpret_cast<char*>(&rotation), sizeof(DirectX::XMFLOAT3));
		inFile.read(reinterpret_cast<char*>(&scale), sizeof(DirectX::XMFLOAT3));
		inFile.read(reinterpret_cast<char*>(&quaternion), sizeof(DirectX::XMFLOAT4));

		std::string childMatrixTag = ReadString(inFile);

		float matrix[16];
		inFile.read(reinterpret_cast<char*>(matrix), sizeof(float) * 16);

		std::string endFrameTag = ReadString(inFile);
		if (endFrameTag != "</Frame>")
		{
			std::cerr << "Error: Invalid format (expected </Frame>)" << std::endl;
			return;
		}

		int index = -1;
		for (int i = 0; i < m_vLoadedModelInfo.size(); i++)
		{
			string modelName = m_vLoadedModelInfo[i]->GetFrameName();
			if (objectName.find(modelName) != string::npos)
			{
				index = static_cast<int>(i);
				break;
			}
		}

		//cout << std::left
		//	<< std::setw(30) << objectName << " [ "
		//	<< std::setw(4) << index << "] ( "
		//	<< std::setw(10) << position.x << ", "
		//	<< std::setw(10) << position.y << ", "
		//	<< std::setw(10) << position.z << " )  |"
		//	<< std::setw(3) << scale.x << " | " << endl;

		m_vObjectInstances.emplace_back(index, objectName, position, rotation, scale, quaternion, matrix);
	}

	inFile.close();
}

// Model/Map 안의 모든 .bin 파일을 불러와 m_vLoadedModelInfo에 저장
void Map::LoadMapObjectsFromFile(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, ID3D12RootSignature* pd3dGraphicsRootSignature)
{
	std::filesystem::path path{ "Model/Map" };

	if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path)) {
		std::cerr << "Error: Directory not found -> " << path << endl;
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

// m_vLoadedModelInfo과 m_vObjectInstances를 참조하여 m_vMapObjects에 오브젝트 리스트를 저장 
void Map::SetMapObjects()
{
	m_vMapObjects.clear();

	for (const auto& instance : m_vObjectInstances)
	{
		if (instance.modelIndex < 0 || instance.modelIndex >= m_vLoadedModelInfo.size())
		{
			std::cerr << "Warning: Invalid modelIndex (" << instance.modelIndex << ") for object " << instance.objectName << std::endl;
			cout << instance.position.x << instance.position.z << endl;
			continue;
		}

		CGameObject* pModel = m_vLoadedModelInfo[instance.modelIndex];
		if (!pModel) continue;

		CGameObject* pNewObject = pModel->Clone();

		pNewObject->SetPosition(instance.position);
		pNewObject->SetScale(instance.scale);
		pNewObject->Rotate(instance.rotation);

		//std::cout << "Object: " << instance.objectName
		//	<< " | Model Index: " << instance.modelIndex
		//	<< " | Mesh Pointer: " << pNewObject->m_pMesh << std::endl;

		m_vMapObjects.push_back(pNewObject);
	}
}

// m_vMapObjects를 참조하여 렌더링
void Map::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera)
{
	for (const auto& instance : m_vMapObjects)
	{
		instance->Render(pd3dCommandList, pCamera);
	}
}
