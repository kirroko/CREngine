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

namespace Ukemochi
{
	using namespace rapidjson;

	GameObject GameObjectFactory::CreateObject()
	{
		EntityID entity = ECS::GetInstance().CreateEntity();

		return {entity};
	}

	GameObject GameObjectFactory::CreateObject(const std::string& filePath)
	{
		EntityID entity = ECS::GetInstance().CreateEntity();

		Document storage;
		if (Serialization::LoadJSON(filePath, storage))
		{
			const Value& object = storage["GameObject"];

			std::string name = object["Name"].GetString();
			std::string tag = object["Tag"].GetString();
			for (auto& comps : object["Components"].GetArray()) // TODO: Update whenever new components are added
			{
				std::string component = comps["Name"].GetString();
				if (component == "Transform")
				{
					ECS::GetInstance().AddComponent(entity, Transform{
						Vec2(comps["Position"][0].GetFloat(),comps["Position"][1].GetFloat()),
						comps["Rotation"].GetFloat(),
						Vec2(comps["Scale"][0].GetFloat(),comps["Scale"][1].GetFloat())
						}); // Default Component
				}
				else if (component == "Rigidbody2D")
				{
					ECS::GetInstance().AddComponent(entity, Rigidbody2D{
						Vec2(comps["Position"][0].GetFloat(), comps["Position"][1].GetFloat()),
						Vec2(comps["Velocity"][0].GetFloat(), comps["Velocity"][1].GetFloat()),
						Vec2(comps["Acceleration"][0].GetFloat(), comps["Acceleration"][1].GetFloat()),
						Vec2(comps["Force"][0].GetFloat(), comps["Force"][1].GetFloat()),
						comps["Mass"].GetFloat(), comps["Inverse Mass"].GetFloat(), comps["Linear Drag"].GetFloat(),
						comps["Angle"].GetFloat(), comps["Angular Velocity"].GetFloat(), comps["Angular Acceleration"].GetFloat(),
						comps["Torque"].GetFloat(), comps["Inertia Mass"].GetFloat(), comps["Inverse Inertia Mass"].GetFloat(), comps["Angular Drag"].GetFloat(),
						comps["use_gravity"].GetBool(), comps["is_kinematic"].GetBool()
						}); // Default Component
				}
				else if (component == "BoxCollider2D")
				{
					ECS::GetInstance().AddComponent(entity, BoxCollider2D{
						Vec2(comps["Min"][0].GetFloat(), comps["Min"][1].GetFloat()),
						Vec2(comps["Max"][0].GetFloat(), comps["Max"][1].GetFloat()),
						comps["Collision Flag"].GetInt(),
						comps["is_trigger"].GetBool(),
						comps["Tag"].GetString()
						}); // Default Component
				}
				else if (component == "CircleCollider2D")
				{
					ECS::GetInstance().AddComponent(entity, CircleCollider2D{
						Vec2(comps["Center"][0].GetFloat(), comps["Center"][1].GetFloat()),
						comps["Radius"].GetFloat()
						}); // Default Component
				}
				else if (component == "SpriteRender")
				{
					std::string TexturePath = "../Assets/Textures/" + std::string(comps["Sprite"].GetString());

					ECS::GetInstance().AddComponent(entity, SpriteRender{
						TexturePath,
						comps["Shape"].GetInt() == 0 ? SPRITE_SHAPE::BOX : SPRITE_SHAPE::CIRCLE
						}); // Default Component
				}
				else
				{
					UME_ENGINE_ASSERT(false, "Uknown Component in json file: {1}",component)
				}
			}

			return {entity, name, tag};
		}
		return {entity}; // Default GameObject
	}

	GameObject GameObjectFactory::CloneObject(GameObject& targetObject)
	{
		auto new_entity = ECS::GetInstance().CloneEntity(targetObject.GetInstanceID());
		return {new_entity};
	}

	void GameObjectFactory::DestroyObject(GameObject& targetobject)
	{
		ECS::GetInstance().DestroyEntity(targetobject.GetInstanceID());
	}
}