/* Start Header ************************************************************************/
/*!
\file       Quadtree.h
\author     Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu
\date       Jan 20, 2025
\brief      This file contains the declaration of the Quadtree algorithm.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../ECS/ECS.h" // for ECS system and components
#include <vector>

namespace Ukemochi
{
    class QuadTree
    {
    private:
        const int MAX_OBJECTS = 10;       // Maximum objects in a node before splitting
        const int MAX_LEVELS = 5;         // Maximum depth levels of the quadtree

        int level;                        // Current depth level
        std::vector<EntityID> objects;    // Entities in the node
        std::vector<BoxCollider2D> boxes; // Bounding boxes of the entities
        BoxCollider2D bounds;             // Boundaries of this QuadTree node
        std::vector<QuadTree*> nodes;     // Subnodes of the current node

    public:
        /*!***********************************************************************
        \brief
         QuadTree Constructor.
        \param[in] level
         The depth level of the node.
        \param[in] bounds
         The bounding box of the node.
        *************************************************************************/
        QuadTree(int level, const BoxCollider2D& bounds);

        /*!***********************************************************************
        \brief
         QuadTree Destructor.
        *************************************************************************/
        ~QuadTree();

        /*!***********************************************************************
        \brief
         Insert an entity and its bounding box into the QuadTree.
        \param[in] entity
         The ID of the entity to insert.
        \param[in] box
         The bounding box of the entity.
        *************************************************************************/
        void Insert(EntityID entity, const BoxCollider2D& box);

        /*!***********************************************************************
        \brief
         Retrieve all potential colliding entities from the QuadTree.
        \param[out] entities
         A vector to store the retrieved entity IDs.
        \param[in] box
         The bounding box to retrieve potential collisions.
        *************************************************************************/
        void Retrieve(std::vector<EntityID>& entities, const BoxCollider2D& box);

        /*!***********************************************************************
        \brief
         Clear all objects and subnodes from the QuadTree.
        *************************************************************************/
        void Clear();

    private:
        /*!***********************************************************************
        \brief
         Split the node into four subnodes.
        *************************************************************************/
        void Split();

        /*!***********************************************************************
        \brief
         Get the index of the sub node to insert the bounding box.
        \param[in] box
         The bounding box to check.
        \return
         The index of the sub node.
        *************************************************************************/
        int GetIndex(const BoxCollider2D& box) const;
    };
}