#include "stdafx.h"
#include "CollisionManager.h"
#include "Object.h"
#include "Player.h"

CCollisionManager::CCollisionManager()
{
    m_pQuadTree = new CQuadTree();
}

CCollisionManager::~CCollisionManager()
{
    delete m_pQuadTree;
}

void CCollisionManager::Build(const BoundingBox& worldBounds, int maxObjectsPerNode, int maxDepth)
{
    m_pQuadTree->Build(worldBounds, maxObjectsPerNode, maxDepth);
}

void CCollisionManager::InsertObject(CGameObject* object)
{
    m_pQuadTree->Insert(object);
}

void CCollisionManager::PrintTree()
{
    m_pQuadTree->PrintTree();
}

void CCollisionManager::Update(CPlayer* player)
{
    // 플레이어가 속한 노드 탐색
    QuadTreeNode* playerNode = m_pQuadTree->FindNode(m_pQuadTree->root, player->GetBoundingBox());
    if (!playerNode) return;

    if (frameCounter % 60 == 0) // 60 프레임마다 출력
        cout << playerNode->bounds.Center.x << ", " << playerNode->bounds.Center.z << endl;

    // 근처 오브젝트 수집
    m_collisions.clear();
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

bool CCollisionManager::IsColliding(const BoundingBox& box1, const BoundingBox& box2)
{
    // X축 충돌 검사
    if (fabs(box1.Center.x - box2.Center.x) > (box1.Extents.x + box2.Extents.x))
        return false;

    // Z축 충돌 검사
    if (fabs(box1.Center.z - box2.Center.z) > (box1.Extents.z + box2.Extents.z))
        return false;

    return true;
}

void CCollisionManager::CollectNearbyObjects(QuadTreeNode* node, const BoundingBox& aabb, std::vector<CGameObject*>& collisions)
{
    if (!node) return;
    for (CGameObject* obj : node->objects)
    {
        if (obj)
        {
            collisions.push_back(obj);
        }
    }
}

void CCollisionManager::HandleCollision(CPlayer* player, CGameObject* obj)
{
    std::string ObjectFrameName = obj->GetFrameName();

    if (frameCounter % 60 == 0)
        cout << "ObjectFrameName: " << ObjectFrameName << endl;

    if (std::string::npos != ObjectFrameName.find("Map_wall_window")
        || std::string::npos != ObjectFrameName.find("Map_wall_plain")
        || std::string::npos != ObjectFrameName.find("Map_wall_baydoor")
        )
    {
        // 플레이어와 벽의 경계 상자 가져오기
        BoundingBox playerBox = player->GetBoundingBox();
        BoundingBox wallBox = obj->GetBoundingBox();

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
                pushDirection = XMFLOAT3(0, 0, -1);
                pushDistance = playerVertices[2].z - wallVertices[1].z + pushMargin;
            }
            else
            {
                // 아래로 밀어야 함
                pushDirection = XMFLOAT3(0, 0, 1);
                pushDistance = wallVertices[2].z - playerVertices[1].z + pushMargin;
            }
        }
        else
        {
            if (diff.x < 0)
            {
                // 오른쪽으로 밀어야 함
                pushDirection = XMFLOAT3(-1, 0, 0);
                pushDistance = playerVertices[1].x - wallVertices[0].x + pushMargin;
            }
            else
            {
                // 왼쪽으로 밀어야 함
                pushDirection = XMFLOAT3(1, 0, 0);
                pushDistance = wallVertices[1].x - playerVertices[0].x + pushMargin;
            }
        }

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
    }

    if (std::string::npos != ObjectFrameName.find("Map_barrel")
        || std::string::npos != ObjectFrameName.find("Map_pallet_variation_01")
        || std::string::npos != ObjectFrameName.find("Map_pallet_variation_02")
        || std::string::npos != ObjectFrameName.find("Map_pallet_variation_03")
        || std::string::npos != ObjectFrameName.find("Map_pallet_variation_04")
        || std::string::npos != ObjectFrameName.find("Map_pallet_variation_05")
        || std::string::npos != ObjectFrameName.find("Map_pallet_variation_06")
        || std::string::npos != ObjectFrameName.find("Map_pallet_variation_07")
        || std::string::npos != ObjectFrameName.find("Map_pallet_variation_08")
        || std::string::npos != ObjectFrameName.find("Map_pallet_variation_09")
        || std::string::npos != ObjectFrameName.find("Map_pillar")
        || std::string::npos != ObjectFrameName.find("Map_shelf_variation_01")
        || std::string::npos != ObjectFrameName.find("Map_shelf_variation_02")
        || std::string::npos != ObjectFrameName.find("Map_shelf_variation_03")
        || std::string::npos != ObjectFrameName.find("Map_shelf_variation_04")
        || std::string::npos != ObjectFrameName.find("Map_shelf_variation_05")
        || std::string::npos != ObjectFrameName.find("Map_shelf_variation_06")
        || std::string::npos != ObjectFrameName.find("Map_shelves_empty")
        || std::string::npos != ObjectFrameName.find("Map_crate_long")
        || std::string::npos != ObjectFrameName.find("Map_garbage_bin")
        || std::string::npos != ObjectFrameName.find("Map_duct_vent")
        || std::string::npos != ObjectFrameName.find("Map_duct_elbow_01")
        || std::string::npos != ObjectFrameName.find("Map_duct_elbow_02")
        || std::string::npos != ObjectFrameName.find("Map_duct_tee")
        || std::string::npos != ObjectFrameName.find("Map_crate_long")
        || std::string::npos != ObjectFrameName.find("Map_crate_short")
        )
    {
        DirectX::XMFLOAT3 playerPos = player->GetPosition();
        BoundingBox playerBox = player->GetBoundingBox();
        BoundingBox objBox = obj->GetBoundingBox();

        // Min, Max 계산
        DirectX::XMFLOAT3 playerMin, playerMax, objMin, objMax;
        playerMin.x = playerBox.Center.x - playerBox.Extents.x;
        playerMin.y = playerBox.Center.y - playerBox.Extents.y;
        playerMin.z = playerBox.Center.z - playerBox.Extents.z;
        playerMax.x = playerBox.Center.x + playerBox.Extents.x;
        playerMax.y = playerBox.Center.y + playerBox.Extents.y;
        playerMax.z = playerBox.Center.z + playerBox.Extents.z;

        objMin.x = objBox.Center.x - objBox.Extents.x;
        objMin.y = objBox.Center.y - objBox.Extents.y;
        objMin.z = objBox.Center.z - objBox.Extents.z;
        objMax.x = objBox.Center.x + objBox.Extents.x;
        objMax.y = objBox.Center.y + objBox.Extents.y;
        objMax.z = objBox.Center.z + objBox.Extents.z;

        // 겹침 크기 계산 (x, z축)
        DirectX::XMFLOAT3 overlap;
        overlap.x = std::min(playerMax.x, objMax.x) - std::max(playerMin.x, objMin.x);
        overlap.z = std::min(playerMax.z, objMax.z) - std::max(playerMin.z, objMin.z);

        // 겹침이 작은 축을 기준으로 플레이어 위치 조정
        if (overlap.x < overlap.z)
        {
            if (playerPos.x < objBox.Center.x)
                playerPos.x = objMin.x - playerBox.Extents.x; // 왼쪽으로 밀어냄
            else
                playerPos.x = objMax.x + playerBox.Extents.x; // 오른쪽으로 밀어냄
        }
        else
        {
            if (playerPos.z < objBox.Center.z)
                playerPos.z = objMin.z - playerBox.Extents.z; // 아래로 밀어냄
            else
                playerPos.z = objMax.z + playerBox.Extents.z; // 위로 밀어냄
        }
        player->SetPosition(playerPos); // 플레이어 위치 갱신
        player->SetVelocity({ 0.0f, 0.0f, 0.0f }); // 속도 정지
        player->CalculateBoundingBox();
        playerBox = player->GetBoundingBox();
    }

    if (std::string::npos != ObjectFrameName.find("Map_corridor_4way")
        || std::string::npos != ObjectFrameName.find("Map_corridor_corner")
        || std::string::npos != ObjectFrameName.find("Map_wall_corridor")
        || std::string::npos != ObjectFrameName.find("Map_corridor_deadend_window")
        || std::string::npos != ObjectFrameName.find("Map_corridor_passthrough")
        || std::string::npos != ObjectFrameName.find("Map_corridor_tee")
        )
    {
        //cout << obj->GetChild()->GetSibling()->GetBoundingBox().Center.x << ", " <<  
        //    obj->GetChild()->GetSibling()->GetBoundingBox().Center.z << ", " <<
        //    obj->GetChild()->GetSibling()->GetBoundingBox().Extents.x << ", " <<
        //    obj->GetChild()->GetSibling()->GetBoundingBox().Extents.z << endl;
    }
}
