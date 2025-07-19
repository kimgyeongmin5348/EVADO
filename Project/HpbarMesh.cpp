#include "HpbarMesh.h"

CHpbarMesh::CHpbarMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float width, float height) : CMesh(pd3dDevice, pd3dCommandList)
{
    m_nVertices = 6;
    m_nStride = sizeof(CTexturedVertex);
    m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    float hw = width * 0.5f;
    float hh = height * 0.5f;

    m_vertices[0] = CTexturedVertex({ -hw, +hh, 0.0f }, { 0.0f, 0.0f });
    m_vertices[1] = CTexturedVertex({ +hw, +hh, 0.0f }, { 1.0f, 0.0f });
    m_vertices[2] = CTexturedVertex({ +hw, -hh, 0.0f }, { 1.0f, 1.0f });

    m_vertices[3] = CTexturedVertex({ -hw, +hh, 0.0f }, { 0.0f, 0.0f });
    m_vertices[4] = CTexturedVertex({ +hw, -hh, 0.0f }, { 1.0f, 1.0f });
    m_vertices[5] = CTexturedVertex({ -hw, -hh, 0.0f }, { 0.0f, 1.0f });

    m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_vertices, sizeof(m_vertices),
        D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

    m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
    m_d3dPositionBufferView.StrideInBytes = sizeof(CTexturedVertex);
    m_d3dPositionBufferView.SizeInBytes = sizeof(m_vertices);
}

CHpbarMesh::~CHpbarMesh()
{
}

void CHpbarMesh::UpdateHpRatio(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, float ratio)
{
    float clamped = std::clamp(ratio, 0.0f, 1.0f);

    // 오른쪽 정점의 x 좌표와 UV만  조정
    for (int i = 0; i < 6; ++i)
    {
        if (m_vertices[i].m_xmf3Position.x > 0.0f) {
            m_vertices[i].m_xmf3Position.x = clamped * 3.0f; // 3.0f = 절반 길이
        }
        if (m_vertices[i].m_xmf2TexCoord.x > 0.0f) {
            m_vertices[i].m_xmf2TexCoord.x = clamped;
        }
    }

    // 버퍼 다시 생성 (간단히 처리)
    m_pd3dPositionBuffer->Release();
    m_pd3dPositionUploadBuffer->Release();

    m_pd3dPositionBuffer = CreateBufferResource(pd3dDevice, pd3dCommandList, m_vertices, sizeof(m_vertices),
        D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dPositionUploadBuffer);

    m_d3dPositionBufferView.BufferLocation = m_pd3dPositionBuffer->GetGPUVirtualAddress();
    m_d3dPositionBufferView.StrideInBytes = sizeof(CTexturedVertex);
    m_d3dPositionBufferView.SizeInBytes = sizeof(m_vertices);
}
