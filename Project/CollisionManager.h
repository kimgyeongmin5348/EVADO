#pragma once

#include "QuadTree.h"

class CCollisionManager
{
private:
    CQuadTree* m_pQuadTree;
    std::vector<CGameObject*> m_collisions;
    int frameCounter = 0;

public:
    CCollisionManager() { m_pQuadTree = new CQuadTree(); }
    ~CCollisionManager() { delete m_pQuadTree; }

    void Build(const BoundingBox& worldBounds, int maxObjectsPerNode)
    {   
        m_pQuadTree->Build(worldBounds, maxObjectsPerNode);
    }

    void InsertObject(CGameObject* object) 
    { 
        m_pQuadTree->Insert(object); 
    }

    void PrintTree()
    {
        m_pQuadTree->PrintTree();
    }

    void Update(CPlayer* player)
    {
        // 플레이어가 속한 노드 탐색
        QuadTreeNode* playerNode = m_pQuadTree->FindNode(m_pQuadTree->root, player->GetBoundingBox());
        if (!playerNode) return;
        //if (frameCounter % 60 == 0) // 60 프레임마다 출력
        //    cout << playerNode->bounds.Center.x << endl;

        // 근처 오브젝트 수집
        CollectNearbyObjects(playerNode, player->GetBoundingBox(), m_collisions);

        // 충돌 검사 및 처리
        for (CGameObject* obj : m_collisions)
        {
            if (obj != player && player->GetBoundingBox().Intersects(obj->GetBoundingBox()))
            {
                HandleCollision(player, obj);
            }
        }
    }

private:
    void CollectNearbyObjects(QuadTreeNode* node, const BoundingBox& aabb, std::vector<CGameObject*>& result)
    {
        if (!node || !node->bounds.Intersects(aabb))
            return;

        if (node->isLeaf)
        {
            result.insert(result.end(), node->objects.begin(), node->objects.end());
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                CollectNearbyObjects(node->children[i], aabb, result);
            }
        }
    }

    void HandleCollision(CPlayer* player, CGameObject* b)
    {
        if (!b || !b->GetChild())
            return;

        std::string nameB = b->GetChild()->GetFrameName();
        std::string nameB2 = b->GetFrameName();

        if (frameCounter % 60 == 0)
            cout << "Child(): " << nameB << "Self: " << nameB2 << endl;

        if (std::string::npos != nameB.find("wall") || std::string::npos != nameB2.find("wall"))
        {
            // 플레이어와 벽의 경계 상자 가져오기
            BoundingBox playerBox = player->GetBoundingBox();
            BoundingBox wallBox = b->GetBoundingBox();

            // 플레이어와 벽의 중심 간 차이
            XMFLOAT3 playerCenter = playerBox.Center;
            XMFLOAT3 wallCenter = wallBox.Center;
            XMFLOAT3 diff(playerCenter.x - wallCenter.x, playerCenter.y - wallCenter.y, playerCenter.z - wallCenter.z);

            //// 플레이어 BoundingBox의 4개 꼭짓점 계산
            XMFLOAT3 playerExtents = playerBox.Extents;
            XMFLOAT3 playerVertices[4] = {
                XMFLOAT3(playerCenter.x - playerExtents.x, playerCenter.y, playerCenter.z - playerExtents.z), // 우상단
                XMFLOAT3(playerCenter.x + playerExtents.x, playerCenter.y, playerCenter.z - playerExtents.z), // 좌상단
                XMFLOAT3(playerCenter.x + playerExtents.x, playerCenter.y, playerCenter.z + playerExtents.z), // 좌하단
                XMFLOAT3(playerCenter.x - playerExtents.x, playerCenter.y, playerCenter.z + playerExtents.z), // 우하단
            };

            // 벽 BoundingBox의 4개 꼭짓점 계산
            XMFLOAT3 wallExtents = wallBox.Extents;
            XMFLOAT3 wallVertices[4] = {
                XMFLOAT3(wallCenter.x - wallExtents.x, playerCenter.y, wallCenter.z - wallExtents.z), // 우상단
                XMFLOAT3(wallCenter.x + wallExtents.x, playerCenter.y, wallCenter.z - wallExtents.z), // 좌상단
                XMFLOAT3(wallCenter.x + wallExtents.x, playerCenter.y, wallCenter.z + wallExtents.z), // 좌하단
                XMFLOAT3(wallCenter.x - wallExtents.x, playerCenter.y, wallCenter.z + wallExtents.z), // 우하단
            };

            // 벽이 밀어낼 방향 과 거리 찾기
            XMFLOAT3 pushDirection(0, 0, 0);
            float pushDistance{ 0.0f };
            float pushMargin{ 0.0f };
            float maxExtent = std::max(wallExtents.x, wallExtents.z);
            if (maxExtent == wallExtents.x)
            {
                if (diff.z < 0) 
                {
                    // 위로 밀어야 함
                    cout << "위로 밀어야 함" << endl;
                    pushDirection = XMFLOAT3(0, 0, 1);
                    pushDistance = playerVertices[2].z - wallVertices[1].z + pushMargin;
                }
                else
                {
                    // 아래로 밀어야 함
                    cout << "아래로 밀어야 함" << endl;
                    pushDirection = XMFLOAT3(0, 0, 1);
                    pushDistance = wallVertices[2].z - playerVertices[1].z + pushMargin;
                }
            }
            else
            {
                if (diff.x < 0)
                {
                    // 오른쪽으로 밀어야 함
                    cout << "오른쪽으로 밀어야 함" << endl;
                    pushDirection = XMFLOAT3(-1, 0, 0);
                    pushDistance = playerVertices[1].x - wallVertices[0].x + pushMargin;
                }
                else
                {
                    // 왼쪽으로 밀어야 함
                    cout << "왼쪽으로 밀어야 함" << endl;
                    pushDirection = XMFLOAT3(-1, 0, 0);
                    pushDistance = wallVertices[0].x - playerVertices[1].x + pushMargin;
                }
            }

            // 밀어내기 벡터
            XMFLOAT3 pushVector(
                pushDirection.x * pushDistance,
                0,
                pushDirection.z * pushDistance
            );

            XMFLOAT3 shift = pushVector;
            player->SetVelocity(XMFLOAT3(0, 0, 0));
            player->Move(shift, false);
            player->CalculateBoundingBox();
            playerBox = player->GetBoundingBox();

// 현재 플레이어와 벽의 중심 및 크기
            //XMFLOAT3 playerCenter = playerBox.Center;
            //XMFLOAT3 playerExtents = playerBox.Extents;
            //XMFLOAT3 wallCenter = wallBox.Center;
            //XMFLOAT3 wallExtents = wallBox.Extents;
            //XMFLOAT3 wallMin(wallCenter.x - wallExtents.x, wallCenter.y - wallExtents.y, wallCenter.z - wallExtents.z);
            //XMFLOAT3 wallMax(wallCenter.x + wallExtents.x, wallCenter.y + wallExtents.y, wallCenter.z + wallExtents.z);

            //XMFLOAT3 adjustedCenter = playerCenter;

            //float margin = 0.5;

            //// x축 제한: 플레이어 Center가 벽 안으로 들어오면 벽 외곽으로 조정
            //if (playerCenter.x < wallMin.x)
            //    adjustedCenter.x = wallMin.x + margin; // 왼쪽 외곽으로 이동
            //else if (playerCenter.x > wallMax.x)
            //    adjustedCenter.x = wallMax.x - margin; // 오른쪽 외곽으로 이동
            //// z축 제한: 플레이어 Center가 벽 안으로 들어오면 벽 외곽으로 조정
            //else if (playerCenter.z < wallMin.z)
            //    adjustedCenter.z = wallMin.z + margin; // 앞쪽 외곽으로 이동
            //else if (playerCenter.z > wallMax.z)
            //    adjustedCenter.z = wallMax.z - margin; // 뒤쪽 외곽으로 이동
            //else
            //    return;

            //// y축 유지 (필요 시 조정)
            //adjustedCenter.y = playerCenter.y;

            //if (adjustedCenter.x != playerCenter.x || adjustedCenter.z != playerCenter.z)
            //{
            //    player->SetPosition(adjustedCenter);
            //    player->CalculateBoundingBox();

            //    playerBox = player->GetBoundingBox();
            //}
        }
    }
};

