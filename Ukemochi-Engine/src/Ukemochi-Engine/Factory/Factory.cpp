/* Start Header
*****************************************************************/
/*!
\file		Factory.cpp
\par		Ukemochi
\author		Pek Jun Kai Gerald, p.junkaigerald, 2301334
\co-authors
\par		p.junkaigerald\@digipen.edu
\par		Course: CSD2400/CSD2401
\date		29/09/24
\brief		This file contains the definition of the GameObjectFactory class.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "Factory.h"
//
//static std::string in_file_path("./test-input.json");
//static std::string out_file_path("./test-output.json");

namespace Ukemochi
{
	using namespace rapidjson;

	GameObject GameObjectFactory::CreateObject()
	{
		EntityID entity = ECS::GetInstance().CreateEntity();

		//ECS::GetInstance().AddComponent(entity, Transform{}); // Default Component

		return GameObject(entity);
	}

	GameObject GameObjectFactory::CreateObject(const std::string& filePath)
	{
		EntityID entity = ECS::GetInstance().CreateEntity();

		Document storage;
		bool success = Serialization::LoadJSON(filePath, storage);
		if (success)
		{
			const Value& object = storage["GameObject"];

			std::string name = object["Name"].GetString();
			std::string tag = object["Tag"].GetString();
			for (auto& comps : object["Components"].GetArray()) // TODO: Update whenever new components are added
			{
				std::string component = comps["Name"].GetString();
				if (component.compare("Transform") == 0)
				{
					ECS::GetInstance().AddComponent(entity, Transform{
						Vec2(comps["Position"][0].GetFloat(),comps["Position"][1].GetFloat()),
						comps["Rotation"].GetFloat(),
						Vec2(comps["Scale"][0].GetFloat(),comps["Scale"][1].GetFloat())
						}); // Default Component
				}
				else if (component.compare("Rigidbody2D") == 0)
				{
					ECS::GetInstance().AddComponent(entity, Rigidbody2D{
						Vec2(comps["Position"][0].GetFloat(), comps["Position"][1].GetFloat()),
						Vec2(comps["Velocity"][0].GetFloat(), comps["Velocity"][1].GetFloat()),
						Vec2(comps["Acceleration"][0].GetFloat(), comps["Acceleration"][1].GetFloat()),
						Vec2(comps["Force"][0].GetFloat(), comps["Force"][1].GetFloat()),
						comps["Mass"].GetFloat(), comps["Inverse Mass"].GetFloat(), comps["Linear Drag"].GetFloat(),
						comps["use_gravity"].GetBool(), comps["is_kinematic"].GetBool()
						}); // Default Component
				}
				else if (component.compare("BoxCollider2D") == 0)
				{
					ECS::GetInstance().AddComponent(entity, BoxCollider2D{
						Vec2(comps["Min"][0].GetFloat(), comps["Min"][1].GetFloat()),
						Vec2(comps["Max"][0].GetFloat(), comps["Max"][1].GetFloat()),
						comps["Collision Flag"].GetInt(),
						comps["is_trigger"].GetBool(),
						comps["is_player"].GetBool()
						}); // Default Component
				}
				else if (component.compare("CircleCollider2D") == 0)
				{
					ECS::GetInstance().AddComponent(entity, CircleCollider2D{
						Vec2(comps["Center"][0].GetFloat(), comps["Center"][1].GetFloat()),
						comps["Radius"].GetFloat()
						}); // Default Component
				}
				else if (component.compare("SpriteRender") == 0)
				{
					std::string TexturePath = "../Assets/Textures/" + std::string(comps["Sprite"].GetString());

					ECS::GetInstance().AddComponent(entity, SpriteRender{
						TexturePath,
						comps["Shape"].GetInt() == 0 ? BOX : CIRCLE
						}); // Default Component
				}
			}

			return GameObject(entity, name, tag);
		}
		return GameObject(entity); // Default GameObject
	}

	//void GameObjectFactory::CloneObject(int count, const std::string& type)
	//{
	//	GameObject object;
	//	Document storage;
	//	bool success = Serialization::LoadJSON(in_file_path, storage);
	//	if (success)
	//	{
	//		const Value& game_objects = storage["Game Objects"];
	//		if (game_objects.IsArray())
	//		{
	//			for (SizeType i = 0; i < game_objects.Size(); i++)
	//			{
	//				std::string obj_type = game_objects[i]["name"].GetString();
	//				if (obj_type.compare(type) == 0)
	//				{
	//					if (type.compare("box") == 0)
	//					{
	//						object.type = obj_type;
	//						object.x_val = game_objects[i]["x_val"].GetInt();
	//						object.y_val = game_objects[i]["y_val"].GetInt();
	//						object.height = game_objects[i]["height"].GetInt();
	//						object.width = game_objects[i]["width"].GetInt();
	//						break;
	//					}
	//					else if (type.compare("circle") == 0)
	//					{
	//						object.type = obj_type;
	//						object.x_val = game_objects[i]["x_val"].GetInt();
	//						object.y_val = game_objects[i]["y_val"].GetInt();
	//						object.radius = game_objects[i]["radius"].GetInt();
	//						object.segment = game_objects[i]["segments"].GetInt();
	//						break;
	//					}
	//				}
	//			}
	//		}
	//		for (int k{}; k < count; k++)
	//		{
	//			//RandomPosition(object.x_val, object.y_val);
	//			if (type.compare("box") == 0)
	//			{
	//			}
	//		}
	//	}
	//	else
	//	{
	//		return;
	//	}
	//}

	GameObject GameObjectFactory::CloneObject(GameObject& targetObject)
	{
		auto new_entity = ECS::GetInstance().CloneEntity(targetObject.GetInstanceID());
		return GameObject(new_entity);
	}
}