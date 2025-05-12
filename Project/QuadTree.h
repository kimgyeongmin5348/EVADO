#pragma once
#include "Object.h"
#include "Player.h"

struct QuadTreeNode
{
    BoundingBox bounds;
    std::vector<CGameObject*> objects;
    QuadTreeNode* children[4];
    int depth;
    bool isLeaf;

    QuadTreeNode(const BoundingBox& bounds) : bounds(bounds), isLeaf(true), depth(0)
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
    int maxDepth;

    CQuadTree(float minSize = 10.0f) : root(nullptr), maxObjectsPerNode(10), minNodeSize(minSize), maxDepth(0) {}
    ~CQuadTree()
    {
        DeleteNode(root);
    }

    void Build(const BoundingBox& worldBounds, int maxObjects, int maxDepth)
    {
        maxObjectsPerNode = maxObjects > 0 ? maxObjects : 10;
        this->maxDepth = maxDepth > 0 ? maxDepth : 4; // 기본값 4
        if (root) DeleteNode(root);
        root = new QuadTreeNode(worldBounds);
        root->depth = 0;
        PreBuild(root, 0);
    }

    void Insert(CGameObject* object)
    {
        if (!root || !object) return;
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
            if (node->children[i]) {
                QuadTreeNode* result = FindNode(node->children[i], aabb);
                if (result)
                    return result;
            }
        }
        return node;
    }

private:
    void PreBuild(QuadTreeNode* node, int depth)
    {
        if (!node || depth >= maxDepth)
        {
            node->isLeaf = true;
            return;
        }

        XMFLOAT3 center = node->bounds.Center;
        XMFLOAT3 extents = node->bounds.Extents;
        float halfX = extents.x * 0.5f;
        float halfZ = extents.z * 0.5f;

        BoundingBox childBounds[4];
        childBounds[0] = BoundingBox(XMFLOAT3(center.x + halfX, center.y, center.z + halfZ), XMFLOAT3(halfX, extents.y, halfZ));
        childBounds[1] = BoundingBox(XMFLOAT3(center.x - halfX, center.y, center.z + halfZ), XMFLOAT3(halfX, extents.y, halfZ));
        childBounds[2] = BoundingBox(XMFLOAT3(center.x - halfX, center.y, center.z - halfZ), XMFLOAT3(halfX, extents.y, halfZ));
        childBounds[3] = BoundingBox(XMFLOAT3(center.x + halfX, center.y, center.z - halfZ), XMFLOAT3(halfX, extents.y, halfZ));

        for (int i = 0; i < 4; i++)
        {
            node->children[i] = new QuadTreeNode(childBounds[i]);
            node->children[i]->depth = depth + 1;
            node->isLeaf = false;
            PreBuild(node->children[i], depth + 1);
        }
    }

    void InsertObject(QuadTreeNode* node, CGameObject* object)
    {
        if (!node || !object) return;

        const BoundingBox& objBounds = object->GetBoundingBox();

        if (node->isLeaf)
        {
            if (node->bounds.Intersects(objBounds))
            {
                if (std::find(node->objects.begin(), node->objects.end(), object) == node->objects.end())
                {
                    node->objects.push_back(object);
                    object->m_pNode = node;
                }
            }
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                if (node->children[i])
                {
                    InsertObject(node->children[i], object);
                }
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

        if (!node->objects.empty())
        {
            for (int i = 0; i < depth + 1; i++) std::cout << "  ";
            std::cout << "Objects:\n";
            for (size_t i = 0; i < node->objects.size(); ++i)
            {
                for (int j = 0; j < depth + 2; j++) std::cout << "  ";
                std::cout << "Object " << i << ": frameName = " << node->objects[i]->GetFrameName() << "\n";
            }
        }

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


backup_main 은, ForCollsion_Jade 브랜치가 main 브랜치에 컨플릭트를 해결하고 머지를 하기 직전 상태의 main 을 갖고있는 브랜치이다.따라서