#pragma once

#include "Object.h"

struct QuadTreeNode
{
    BoundingBox                 bounds;         // 노드의 경계
    std::vector<CGameObject*>   objects;        // 이 노드에 속한 오브젝트
    QuadTreeNode*               children[4];    // 자식 노드 (4개)
    bool                        isLeaf;         // 리프 노드 여부

    QuadTreeNode(const BoundingBox& bounds) : bounds(bounds), isLeaf(true)
    {
        for (int i = 0; i < 4; i++)
            children[i] = nullptr;
    }
};

class CQuadTree
{
public:
    QuadTreeNode*   root;
    int             maxDepth;
    int             maxObjectsPerNode;

    CQuadTree() : root(nullptr), maxDepth(0), maxObjectsPerNode(0) {}
    ~CQuadTree()
    {
        DeleteNode(root);
    }

    void Build(const BoundingBox& worldBounds, int depth, int maxObjects)
    {
        maxDepth = depth;
        maxObjectsPerNode = maxObjects;
        root = new QuadTreeNode(worldBounds);
        Subdivide(root, 0);
    }

    void Insert(CGameObject* object)
    {
        InsertObject(root, object);
    }

private:
    void Subdivide(QuadTreeNode* node, int depth)
    {
        if (depth >= maxDepth) return;

        XMFLOAT3 center = node->bounds.Center;
        XMFLOAT3 extents = node->bounds.Extents;
        float halfX = extents.x * 0.5f;
        float halfY = extents.y * 0.5f;

        BoundingBox childBounds[4];
        childBounds[0] = BoundingBox(XMFLOAT3(center.x - halfX, center.y - halfY, center.z), XMFLOAT3(halfX, halfY, extents.z)); // 왼쪽 아래
        childBounds[1] = BoundingBox(XMFLOAT3(center.x + halfX, center.y - halfY, center.z), XMFLOAT3(halfX, halfY, extents.z)); // 오른쪽 아래
        childBounds[2] = BoundingBox(XMFLOAT3(center.x - halfX, center.y + halfY, center.z), XMFLOAT3(halfX, halfY, extents.z)); // 왼쪽 위
        childBounds[3] = BoundingBox(XMFLOAT3(center.x + halfX, center.y + halfY, center.z), XMFLOAT3(halfX, halfY, extents.z)); // 오른쪽 위

        for (int i = 0; i < 4; i++)
        {
            node->children[i] = new QuadTreeNode(childBounds[i]);
            Subdivide(node->children[i], depth + 1);
        }
        node->isLeaf = false;
    }

    void InsertObject(QuadTreeNode* node, CGameObject* object, int depth)
    {
        if (node->isLeaf)
        {
            if (node->objects.size() < maxObjectsPerNode || depth >= maxDepth)
            {
                node->objects.push_back(object);
                object->m_pNode = node; // CGameObject에 node 포인터 설정
            }
            else
            {
                Subdivide(node, depth + 1);
                node->objects.push_back(object); // 임시 삽입
                for (CGameObject* obj : node->objects)
                    Redistribute(node, obj);
                node->objects.clear();
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                if (node->children[i]->bounds.Intersects(object->GetBoundingBox())) // CGameObject에서 BoundingBox 반환 가정
                    InsertObject(node->children[i], object, maxDepth);
            }
        }
    }

    void Redistribute(QuadTreeNode* node, CGameObject* object)
    {
        for (int i = 0; i < 4; i++)
        {
            if (node->children[i]->bounds.Intersects(object->GetBoundingBox()))
            {
                InsertObject(node->children[i], object, maxDepth); // depth는 maxDepth로 설정
                break;
            }
        }
    }

    void DeleteNode(QuadTreeNode* node)
    {
        if (!node) return;
        for (int i = 0; i < 4; i++)
            DeleteNode(node->children[i]);
        delete node;
    }
};

