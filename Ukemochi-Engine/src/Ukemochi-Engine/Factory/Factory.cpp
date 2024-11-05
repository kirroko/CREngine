/* Start Header
*****************************************************************/
/*!
\file		Factory.cpp
\par		Ukemochi
\author		Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu
\co-authors Wong Jun Yu, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\par		Course: CSD2400/CSD2401
\date		19/10/24
\brief		This file is responsible for creating, cloning, destroy game objects.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "Factory.h"
#include "GameObjectManager.h"
#include "../Graphics/Renderer.h"

#include "GameObjectManager.h"

namespace Ukemochi
{
	using namespace rapidjson;

	GameObject GameObjectFactory::CreateObject(const std::string& name, const std::string& tag)
	{
		EntityID entity = ECS::GetInstance().CreateEntity();

		return {entity, name, tag};
	}

	GameObject GameObjectFactory::CreatePrefebObject(const std::string& filePath)
	{
		EntityID entity = ECS::GetInstance().CreateEntity();
		Document storage;
		if (Serialization::LoadJSON(filePath, storage))
		{
			const Value& object = storage["GameObject"];

			std::string name = object["Name"].GetString();
			std::string tag = object["Tag"].GetString();

			GameObject go = {entity, name, tag};
			for (auto& comps : object["Components"].GetArray()) // TODO: Update whenever new components are added
			{
				std::string component = comps["Name"].GetString();
				if (component == "Transform")
				{
					go.AddComponent(Transform{
						Mtx44{},
						Vec2(comps["Position"][0].GetFloat(),comps["Position"][1].GetFloat()),
						comps["Rotation"].GetFloat(),
						Vec2(comps["Scale"][0].GetFloat(),comps["Scale"][1].GetFloat())
						});
				}
				else if (component == "Rigidbody2D")
				{
					go.AddComponent(Rigidbody2D{
						Vec2(comps["Position"][0].GetFloat(), comps["Position"][1].GetFloat()),
						Vec2(comps["Velocity"][0].GetFloat(), comps["Velocity"][1].GetFloat()),
						Vec2(comps["Acceleration"][0].GetFloat(), comps["Acceleration"][1].GetFloat()),
						Vec2(comps["Force"][0].GetFloat(), comps["Force"][1].GetFloat()),
						comps["Mass"].GetFloat(), comps["Inverse Mass"].GetFloat(), comps["Linear Drag"].GetFloat(),
						comps["Angle"].GetFloat(), comps["Angular Velocity"].GetFloat(), comps["Angular Acceleration"].GetFloat(),
						comps["Torque"].GetFloat(), comps["Inertia Mass"].GetFloat(), comps["Inverse Inertia Mass"].GetFloat(), comps["Angular Drag"].GetFloat(),
						comps["use_gravity"].GetBool(), comps["is_kinematic"].GetBool()
						});
				}
				else if (component == "BoxCollider2D")
				{
					go.AddComponent(BoxCollider2D{
						Vec2(comps["Min"][0].GetFloat(), comps["Min"][1].GetFloat()),
						Vec2(comps["Max"][0].GetFloat(), comps["Max"][1].GetFloat()),
						comps["Collision Flag"].GetInt(),
						comps["is_trigger"].GetBool(),
						comps["Tag"].GetString()
						});
				}
				else if (component == "CircleCollider2D")
				{
					go.AddComponent(CircleCollider2D{
						Vec2(comps["Center"][0].GetFloat(), comps["Center"][1].GetFloat()),
						comps["Radius"].GetFloat()
						});
				}
				else if (component == "SpriteRender")
				{
					std::string TexturePath = "../Assets/Textures/" + std::string(comps["Sprite"].GetString());

					go.AddComponent(SpriteRender{
						TexturePath,
						comps["Shape"].GetInt() == 0 ? SPRITE_SHAPE::BOX : SPRITE_SHAPE::CIRCLE
						}); // Default Component
					//ECS::GetInstance().GetSystem<Renderer>()->setUpTextures(TexturePath);
				}
				else if(component == "Script")
				{
					go.AddComponent(Script{
					comps["Path"].GetString(),
					comps["ClassName"].GetString(),
						ScriptingEngine::GetInstance().InstantiateClientClass(comps["ClassName"].GetString())
					});
				}
				else
				{
					UME_ENGINE_ASSERT(false, "Unknown Component in json file: {1}",component)
				}
			}

			return go;
		}
		return {entity}; // Default GameObject
	}

	GameObject GameObjectFactory::CloneObject(const GameObject& targetObject, const std::string& name, const std::string& tag)
	{
		auto new_entity = ECS::GetInstance().CloneEntity(targetObject.GetInstanceID());
		return {new_entity, name, tag};
	}

	void GameObjectFactory::DestroyObject(GameObject& targetobject)
	{
		ECS::GetInstance().DestroyEntity(targetobject.GetInstanceID());
	}


	// retrieves all active game objects in the current level.
	// 0x4B45414E - No no no, This is not the jedi way
	 //std::vector<GameObject> GameObjectFactory::GetAllObjectsInCurrentLevel() {
	 //	std::vector<GameObject> gameObjects;
	 //	auto entities = ECS::GetInstance().GetAllEntities();
	
	 //	for (const auto& entity : entities) {
	 //		gameObjects.emplace_back(GameObject(entity));
	 //	}
	 //	return gameObjects;
	 //}

}
