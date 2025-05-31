#include "CParticleMesh.h"

CParticleMesh::CParticleMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) : CMesh(pd3dDevice, pd3dCommandList)
{
    XMFLOAT3 quadVerts[4] = {
            XMFLOAT3(-0.5f, 0.0f, -0.5f),
            XMFLOAT3(-0.5f, 0.0f, 0.5f),
            XMFLOAT3(0.5f, 0.0f, -0.5f),
            XMFLOAT3(0.5f, 0.0f, 0.5f),
    };

    m_nVertices = 4;
    m_pxmf3Positions = new XMFLOAT3[4];
    memcpy(m_pxmf3Positions, quadVerts, sizeof(quadVerts));

    m_pd3dPositionBuffer = CreateBufferResource(
        pd3dDevice, pd3dCommandList, quadVerts, sizeof(quadVerts),
        D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
        &m_pd3dPositionUploadBuffer
    );

    m_d3dPositionBufferView = {
        m_pd3dPositionBuffer->GetGPUVirtualAddress(),
        sizeof(XMFLOAT3),
        sizeof(XMFLOAT3) * m_nVertices
    };
}

CParticleMesh::~CParticleMesh()
{
}
