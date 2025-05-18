#pragma once

#include <vector>
#include "QuadTree.h"

class CGameObject;
class CPlayer;

class CCollisionManager
{
private:
    CQuadTree* m_pQuadTree = NULL;
    std::vector<CGameObject*> m_collisions;
    int frameCounter = 0;

public:
    CCollisionManager();
    ~CCollisionManager();

    void Build(const BoundingBox& worldBounds, int maxObjectsPerNode, int maxDepth);
    void InsertObject(CGameObject* object);
    void PrintTree();
    void Update(CPlayer* player);
    bool IsColliding(const BoundingBox& box1, const BoundingBox& box2);

private:
    void CollectNearbyObjects(QuadTreeNode* node, const BoundingBox& aabb, std::vector<CGameObject*>& collisions);
    void HandleCollision(CPlayer* player, CGameObject* obj);
};