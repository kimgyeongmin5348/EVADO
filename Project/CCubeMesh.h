#pragma once
#include "stdafx.h"
#include "Mesh.h"

class CCubeMesh : public CMesh
{
public:
	CCubeMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height, float depth);
	virtual ~CCubeMesh();
};

