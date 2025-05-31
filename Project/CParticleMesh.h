#pragma once
#include "stdafx.h"
#include "Mesh.h"
class CParticleMesh : public CMesh
{
public:
    CParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
    virtual ~CParticleMesh();
};

