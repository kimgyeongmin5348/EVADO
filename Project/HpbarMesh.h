#pragma once
#include "stdafx.h"
#include "Mesh.h"
class CHpbarMesh : public CMesh
{
public:
    CHpbarMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height);
    virtual ~CHpbarMesh();
    void UpdateHpRatio(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float ratio);

private:
    CTexturedVertex m_vertices[6];
};

