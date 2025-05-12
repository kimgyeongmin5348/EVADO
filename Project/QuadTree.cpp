#include "stdafx.h"
#include "QuadTree.h"
#include "Object.h"

void CQuadTree::Build(const BoundingBox& worldBounds, int maxObjects, int maxDepth)
{
    maxObjectsPerNode = maxObjects > 0 ? maxObjects : 10;
    this->maxDepth = maxDepth > 0 ? maxDepth : 4; // 기본값 4
    if (root) DeleteNode(root);
    root = new QuadTreeNode(worldBounds);
    root->depth = 0;
    PreBuild(root, 0);
}

void CQuadTree::Insert(CGameObject* object)
{
    if (!root || !object) return;
    InsertObject(root, object);
}

void CQuadTree::PrintTree()
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

QuadTreeNode* CQuadTree::FindNode(QuadTreeNode* node, const BoundingBox& aabb)
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

void CQuadTree::PreBuild(QuadTreeNode* node, int depth)
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

void CQuadTree::InsertObject(QuadTreeNode* node, CGameObject* object)
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

void CQuadTree::DeleteNode(QuadTreeNode* node)
{
    if (!node) return;
    for (int i = 0; i < 4; i++)
        DeleteNode(node->children[i]);
    delete node;
}

void CQuadTree::PrintNode(QuadTreeNode* node, int depth)
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
