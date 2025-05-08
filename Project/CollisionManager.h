#pragma once

#include "Object.h"
#include "QuadTree.h"

class CCollisionManager
{
private:
    CQuadTree* m_pQuadTree;

public:
    CCollisionManager() { m_pQuadTree = new CQuadTree(); }
    ~CCollisionManager() { delete m_pQuadTree; }

    void Build(const BoundingBox& worldBounds, int maxDepth, int maxObjectsPerNode)
    {   
        m_pQuadTree->Build(worldBounds, maxDepth, maxObjectsPerNode);
    }

    void InsertObject(CGameObject* object) { m_pQuadTree->Insert(object); }

    void Update(CGameObject* player, std::vector<CGameObject*>& collisions)
    {
        //QuadTreeNode* playerNode = m_pQuadTree->FindNode(m_pQuadTree->root, player->boundingBox);
        //CollectNearbyObjects(playerNode, player->boundingBox, collisions);
        //for (CGameObject* obj : collisions)
        //    if (obj != player && player->boundingBox.Intersects(obj->boundingBox))
        //        HandleCollision(player, obj);
    }

private:
    void CollectNearbyObjects(QuadTreeNode* node, const BoundingBox& aabb, std::vector<CGameObject*>& result)
    {

    }
    void HandleCollision(CGameObject* a, CGameObject* b) 
    { 
         //面倒 贸府 肺流  
    }
};

