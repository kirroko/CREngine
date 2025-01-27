/* Start Header ************************************************************************/
/*!
\file       Quadtree.cpp
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Jan 20, 2025
\brief      This file contains the definition of the Quadtree algorithm.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "QuadTree.h" // for forward declaration
#include <algorithm>

namespace Ukemochi
{
    /*!***********************************************************************
    \brief
     QuadTree Constructor.
    \param[in] level
     The depth level of the node.
    \param[in] bounds
     The bounding box of the node.
    *************************************************************************/
    QuadTree::QuadTree(int level, const BoxCollider2D& bounds) : level(level), bounds(bounds)
    {
        nodes.resize(4, nullptr);
    }

    /*!***********************************************************************
    \brief
     QuadTree Destructor.
    *************************************************************************/
    QuadTree::~QuadTree()
    {
        Clear();
    }

    /*!***********************************************************************
    \brief
     Insert an entity and its bounding box into the QuadTree.
    \param[in] entity
     The ID of the entity to insert.
    \param[in] box
     The bounding box of the entity.
    *************************************************************************/
    void QuadTree::Insert(EntityID entity, const BoxCollider2D& box)
    {
        if (!nodes[0]) // If not split, add the entity here
        {
            objects.push_back(entity);
            boxes.push_back(box);

            // Check if need to split the node
            if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS)
            {
                // Split the node if not already split
                if (!nodes[0])
                    Split();

                // Assign the entities to nodes if needed
                for (size_t i = 0; i < objects.size();)
                {
                    int index = GetIndex(boxes[i]);
                    if (index != -1)
                    {
                        nodes[index]->Insert(objects[i], boxes[i]);
                        objects.erase(objects.begin() + i);
                        boxes.erase(boxes.begin() + i);
                    }
                    else
                    {
                        // Go to next entity
                        ++i;
                    }
                }
            }
            return;
        }

        // If the node is already split, set to appropriate sub node
        int index = GetIndex(box);
        if (index != -1)
        {
            nodes[index]->Insert(entity, box);
        }
        else // If it cannot fit in any sub node, store it in this node
        {
            objects.push_back(entity);
            boxes.push_back(box);
        }
    }

    /*!***********************************************************************
    \brief
     Retrieve all potential colliding entities from the QuadTree.
    \param[out] entities
     A vector to store the retrieved entity IDs.
    \param[in] box
     The bounding box to retrieve potential collisions.
    *************************************************************************/
    void QuadTree::Retrieve(std::vector<EntityID>& entities, const BoxCollider2D& box)
    {
        // Check the index of the sub node to fit the box
        int index = GetIndex(box);
        if (index != -1 && nodes[0])
        {
            // Retrieve entities from the sub node
            nodes[index]->Retrieve(entities, box);
        }

        // Insert all entities stored in this node
        entities.insert(entities.end(), objects.begin(), objects.end());
    }

    /*!***********************************************************************
    \brief
     Clear all objects and subnodes from the QuadTree.
    *************************************************************************/
    void QuadTree::Clear()
    {
        // Clear the object and box lists
        objects.clear();
        boxes.clear();

        // Clear the node lists
        for (auto& node : nodes)
        {
            if (node)
            {
                node->Clear();
                delete node;
                node = nullptr;
            }
        }
    }

    /*!***********************************************************************
    \brief
     Split the node into four subnodes.
    *************************************************************************/
    void QuadTree::Split()
    {
        // Calculate the boundaries of the subnodes
        float sub_width = (bounds.max.x - bounds.min.x) / 2.0f;
        float sub_height = (bounds.max.y - bounds.min.y) / 2.0f;
        float x = bounds.min.x;
        float y = bounds.min.y;

        // Create the subnodes with its boundaries
        nodes[0] = new QuadTree(level + 1, { {x + sub_width, y}, {x + 2 * sub_width, y + sub_height} });
        nodes[1] = new QuadTree(level + 1, { {x, y}, {x + sub_width, y + sub_height} });
        nodes[2] = new QuadTree(level + 1, { {x, y + sub_height}, {x + sub_width, y + 2 * sub_height} });
        nodes[3] = new QuadTree(level + 1, { {x + sub_width, y + sub_height}, {x + 2 * sub_width, y + 2 * sub_height} });
    }

    /*!***********************************************************************
    \brief
     Get the index of the sub node to insert the bounding box.
    \param[in] box
     The bounding box to check.
    \return
     The index of the sub node.
    *************************************************************************/
    int QuadTree::GetIndex(const BoxCollider2D& box) const
    {
        // Calculate the midpoints of the current node
        float vertical_midpoint = bounds.min.x + (bounds.max.x - bounds.min.x) / 2.0f;
        float horizontal_midpoint = bounds.min.y + (bounds.max.y - bounds.min.y) / 2.0f;

        // Check if the box fit the top or btm quad
        bool top_quad = (box.min.y > horizontal_midpoint);
        bool btm_quad = (box.max.y < horizontal_midpoint);

        // Check if the box fit the left or right quad
        if (box.max.x < vertical_midpoint)
        {
            if (top_quad) // Fit the top-left quad
                return 2;
            else if (btm_quad) // Fit the btm-left quad
                return 1;
        }
        else if (box.min.x > vertical_midpoint)
        {
            if (top_quad) // Fit the top-right quad
                return 3;
            else if (btm_quad) // Fit the btm-right quad
                return 0;
        }

        // Cannot fit in any quad
        return -1;
    }
}