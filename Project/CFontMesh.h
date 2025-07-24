#pragma once
#include"stdafx.h"
#include "Mesh.h"
class CFontMesh : public CMesh
{
public:
	CFontMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const std::wstring& text, float x, float y);
	virtual ~CFontMesh();

private:
	std::wstring m_text;
};

