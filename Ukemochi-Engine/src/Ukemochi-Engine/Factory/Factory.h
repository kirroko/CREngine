/* Start Header
*****************************************************************/
/*!
\file		Factory.h
\par		Ukemochi
\author		Pek Jun Kai Gerald, p.junkaigerald, 2301334
\co-authors
\par		p.junkaigerald\@digipen.edu
\par		Course: CSD2400/CSD2401
\date		29/09/24
\brief		This file contains the declaration of the GameObjectFactory class. This class is responsible for creating and cloning game objects.
		You can create a game object with a default constructor or with a file path to load the game object from a JSON file

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "PreCompile.h"
#include "GameObject.h"
#include "../Serialization/Serialization.h"

namespace Ukemochi
{
	class GameObjectFactory
	{
	public:
		/**
		 * @brief Create a new GameObject with default components (Transform)
		 *
		 * @return GameObject&  newly created GameObject
		 */
		static GameObject CreateObject();
		/**
		 * @brief Create a new GameObject with components from a JSON file
		 *
		 * @param filePath the path to the JSON file
		 * @return GameObject& a reference to the newly created GameObject
		 */
		static GameObject CreateObject(const std::string& filePath);
		/**
		 * @brief Clone a GameObject
		 *
		 * @param targetObject the GameObject to clone
		 * @return GameObject& a reference to the cloned GameObject
		 */
		static GameObject CloneObject(GameObject& targetObject);
	};
};