#pragma once
#include "Object.h"
#include "Player.h"

struct QuadTreeNode
{
    BoundingBox bounds;
    std::vector<CGameObject*> objects;
    QuadTreeNode* children[4];
    bool isLeaf;

    QuadTreeNode(const BoundingBox& bounds) : bounds(bounds), isLeaf(true)
    {
        for (int i = 0; i < 4; i++)
            children[i] = nullptr;
    }
};

class CQuadTree
{
public:
    QuadTreeNode* root;
    int maxObjectsPerNode;
    float minNodeSize; // 최소 노드 크기 (무한 분할 방지)

    CQuadTree(float minSize = 1.0f) : root(nullptr), maxObjectsPerNode(0), minNodeSize(minSize) {}
    ~CQuadTree()
    {
        DeleteNode(root);
    }

    void Build(const BoundingBox& worldBounds, int maxObjects)
    {
        maxObjectsPerNode = maxObjects;
        root = new QuadTreeNode(worldBounds);
    }

    void Insert(CGameObject* object)
    {
        InsertObject(root, object);
    }

    void PrintTree()
    {
        if (!root)
        {
            std::cout << "QuadTree is empty.\n";
            return;
        }
        std::cout << "=== QuadTree Structure ===\n";
        PrintNode(root, 0);
        std::cout << "==========================\n";
    }

    QuadTreeNode* FindNode(QuadTreeNode* node, const BoundingBox& aabb)
    {
        if (!node || !node->bounds.Intersects(aabb))
            return nullptr;

        if (node->isLeaf || !node->children[0])
            return node;

        for (int i = 0; i < 4; i++)
        {
            QuadTreeNode* result = FindNode(node->children[i], aabb);
            if (result)
                return result;
        }
        return node;
    }

private:
    void Subdivide(QuadTreeNode* node)
    {
        // 최소 크기 확인 (무한 분할 방지)
        if (node->bounds.Extents.x < minNodeSize || node->bounds.Extents.y < minNodeSize)
            return;

        XMFLOAT3 center = node->bounds.Center;
        XMFLOAT3 extents = node->bounds.Extents;
        float halfX = extents.x * 0.5f;
        float halfY = extents.y * 0.5f;

        BoundingBox childBounds[4];
        childBounds[0] = BoundingBox(XMFLOAT3(center.x + halfX, center.y + halfY, center.z), XMFLOAT3(halfX, halfY, extents.z));
        childBounds[1] = BoundingBox(XMFLOAT3(center.x - halfX, center.y + halfY, center.z), XMFLOAT3(halfX, halfY, extents.z));
        childBounds[2] = BoundingBox(XMFLOAT3(center.x - halfX, center.y - halfY, center.z), XMFLOAT3(halfX, halfY, extents.z));
        childBounds[3] = BoundingBox(XMFLOAT3(center.x + halfX, center.y - halfY, center.z), XMFLOAT3(halfX, halfY, extents.z));

        for (int i = 0; i < 4; i++)
        {
            node->children[i] = new QuadTreeNode(childBounds[i]);
        }
        node->isLeaf = false;
    }

    void InsertObject(QuadTreeNode* node, CGameObject* object)
    {
        if (node->isLeaf)
        {
            if (node->objects.size() < maxObjectsPerNode ||
                (node->bounds.Extents.x < minNodeSize && node->bounds.Extents.y < minNodeSize))
            {
                node->objects.push_back(object);
                object->m_pNode = node;
            }
            else
            {
                Subdivide(node);
                node->objects.push_back(object);
                for (CGameObject* obj : node->objects)
                    Redistribute(node, obj);
                node->objects.clear();
            }
        }
        else
        {
            bool inserted = false;
            for (int i = 0; i < 4; i++)
            {
                if (node->children[i]->bounds.Intersects(object->GetBoundingBox()))
                {
                    InsertObject(node->children[i], object);
                    inserted = true;
                    // 동일 오브젝트가 여러 자식 노드에 삽입될 수 있음
                }
            }
            if (!inserted)
            {
                node->objects.push_back(object);
                object->m_pNode = node;
            }
        }
    }

    void Redistribute(QuadTreeNode* node, CGameObject* object)
    {
        bool inserted = false;
        for (int i = 0; i < 4; i++)
        {
            if (node->children[i]->bounds.Intersects(object->GetBoundingBox()))
            {
                InsertObject(node->children[i], object);
                inserted = true;
            }
        }
        if (!inserted)
        {
            node->objects.push_back(object);
            object->m_pNode = node;
        }
    }

    void DeleteNode(QuadTreeNode* node)
    {
        if (!node) return;
        for (int i = 0; i < 4; i++)
            DeleteNode(node->children[i]);
        delete node;
    }

    void PrintNode(QuadTreeNode* node, int depth)
    {
        if (!node) return;

        for (int i = 0; i < depth; i++) std::cout << "  ";

        std::cout << "Node (Depth " << depth << "):\n";
        for (int i = 0; i < depth + 1; i++) std::cout << "  ";
        std::cout << "Bounds: Center(" << node->bounds.Center.x << ", " << node->bounds.Center.y << ", " << node->bounds.Center.z << ") "
            << "Extents(" << node->bounds.Extents.x << ", " << node->bounds.Extents.y << ", " << node->bounds.Extents.z << ")\n";
        for (int i = 0; i < depth + 1; i++) std::cout << "  ";
        std::cout << "IsLeaf: " << (node->isLeaf ? "Yes" : "No") << ", Objects: " << node->objects.size() << "\n";

        if (!node->isLeaf)
        {
            for (int i = 0; i < 4; i++)
            {
                if (node->children[i])
                {
                    for (int j = 0; j < depth + 1; j++) std::cout << "  ";
                    std::cout << "Child " << i << ":\n";
                    PrintNode(node->children[i], depth + 1);
                }
            }
        }
    }
};