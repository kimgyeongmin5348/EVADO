#pragma once

//#include "Object.h"
class CGameObject;

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

    void Build(const BoundingBox& worldBounds, int maxObjects, int maxDepth);

    void Insert(CGameObject* object);

    void PrintTree();

    QuadTreeNode* FindNode(QuadTreeNode* node, const BoundingBox& aabb);

private:
    void PreBuild(QuadTreeNode* node, int depth);

    void InsertObject(QuadTreeNode* node, CGameObject* object);

    void DeleteNode(QuadTreeNode* node);

    void PrintNode(QuadTreeNode* node, int depth);
};