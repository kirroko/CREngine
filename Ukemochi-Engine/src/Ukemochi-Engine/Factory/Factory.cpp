#include "PreCompile.h"
#include "Factory.h"

static std::string in_file_path("./test-input.json");
static std::string out_file_path("./test-output.json");

namespace Ukemochi
{
	using namespace rapidjson;

	GameObject& GameObjectFactory::CreateObject()
	{
		EntityID entity = ECS::GetInstance().CreateEntity();

		ECS::GetInstance().AddComponent(entity, Transform{}); // Default Component

		return GameObject(entity);
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

	GameObject& GameObjectFactory::CloneObject(GameObject& targetObject)
	{
		auto newEntity = ECS::GetInstance().CloneEntity(targetObject.GetInstanceID());
		return GameObject(newEntity);
	}
}