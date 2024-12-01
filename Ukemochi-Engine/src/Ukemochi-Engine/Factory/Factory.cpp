/* Start Header ************************************************************************/
/*!
\file       Factory.cpp
\author     Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (50%)
\co-authors Wong Jun Yu, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (50%)
\date       Nov 17, 2024
\brief      This file is responsible for creating, cloning, destroy game objects.
			Also, it is responsible for creating prefabs.
		
			Usage:
			1. CreateObject: Create a new game object with a name and tag.
			2. CreatePrefabObject: Create a new game object from a prefab file.
			3. CloneObject: Clone an existing game object with a new name and tag.
			4. DestroyObject: Destroy an existing game object.
		
			Note: The prefab file must be in the JSON format.
		
			Example:
			\code
			// Create a new game object
			GameObjectFactory::CreateObject("Player", "Player");
		
			// Create a new game object from a prefab file
			GameObjectFactory::CreatePrefabObject("Assets/Prefabs/Player.json");
		
			// Clone an existing game object
			GameObjectFactory::CloneObject(player, "Player(Clone)", "Player");
		
			// Destroy an existing game object
			GameObjectFactory::DestroyObject(player);
			\endcode

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Factory.h"

#include "GameObjectManager.h"
#include "../Logic/Scripting.h"

namespace Ukemochi
{
	using namespace rapidjson;

	GameObject GameObjectFactory::CreateObject(const std::string& name, const std::string& tag)
	{
		EntityID entity = ECS::GetInstance().CreateEntity();

		return {entity, name, tag};
	}

	GameObject& GameObjectFactory::CreatePrefebObject(const std::string& filePath)
	{
		// EntityID entity = ECS::GetInstance().CreateEntity(); // A little bit special here
		// We are going to call the GameObjectManager to create an empty Object for us, and then we will fill it up
		Document storage;
		if (Serialization::LoadJSON(filePath, storage))
		{
			const Value& object = storage["GameObject"];

			std::string name = object["Name"].GetString();
			std::string tag = object["Tag"].GetString();
			// GameObject go = {entity, name, tag};
			GameObject& go = GameObjectManager::GetInstance().CreateObject(name, tag);
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
						comps["is_trigger"].GetBool()
						});
				}
				else if (component == "CircleCollider2D")
				{
					go.AddComponent(CircleCollider2D{
						Vec2(comps["Center"][0].GetFloat(), comps["Center"][1].GetFloat()),
						comps["Radius"].GetFloat()
						});
				}
				else if(component == "ConvexCollider2D")
				{
					go.AddComponent(ConvexCollider2D{});
				}
				else if (component == "SpriteRender")
				{
					std::string TexturePath = std::string(comps["Sprite"].GetString());
			
					go.AddComponent(SpriteRender{
						TexturePath,
						comps["Shape"].GetInt() == 0 ? SPRITE_SHAPE::BOX : SPRITE_SHAPE::CIRCLE
						}); // Default Component
				}
				else if(component == "Script")
				{
					MonoObject* newScript = ScriptingEngine::GetInstance().InstantiateClientClass(comps["ClassName"].GetString());
					EntityID newScriptID = go.GetInstanceID();
					ScriptingEngine::SetMonoFieldValueULL(newScript, "_id", &newScriptID);
					go.AddComponent(Script{
						comps["Path"].GetString(),
						comps["ClassName"].GetString(),
						newScript,
						ScriptingEngine::CreateGCHandle(newScript)
						});
				}
				else if (component == "Animation")
				{
					Animation anim;

					for (auto itr = comps["Clips"].MemberBegin(); itr != comps["Clips"].MemberEnd(); ++itr)
					{
						AnimationClip newClip;
						newClip.keyPath = itr->value[0].GetString();
						newClip.name = itr->value[1].GetString();
						newClip.total_frames = itr->value[2].GetInt();
						newClip.pixel_width = itr->value[3].GetInt();
						newClip.pixel_height = itr->value[4].GetInt();
						newClip.total_width = itr->value[5].GetInt();
						newClip.total_height = itr->value[6].GetInt();
						newClip.frame_time = itr->value[7].GetFloat();
						newClip.looping = itr->value[8].GetBool();
						anim.clips[newClip.name] = newClip;
					}

					anim.currentClip = comps["CurrentClip"].GetString();
					anim.defaultClip = anim.currentClip;

					go.AddComponent(std::move(anim));
				}
				else if (component == "EnemyComponent")
				{
					int type = comps["Type"].GetInt();
					Enemy enemy(comps["Position"][0].GetFloat(),
						comps["Position"][1].GetFloat(), static_cast<Enemy::EnemyTypes>(type), go.GetInstanceID());

					go.AddComponent(std::move(enemy));
				}
				//else if(component == "Button")
				//{
				//	go.AddComponent(Button{});
				//}
				else
				{
					UME_ENGINE_ASSERT(false, "Unknown Component in json file: {1}",component)
				}
			}

			return go;
		}
		UME_ENGINE_ASSERT(false ,"Loading Prefab failed: {1}", filePath)
		return GameObjectManager::GetInstance().CreateObject();
		// Ignore the warning
	}

	GameObject GameObjectFactory::CloneObject(const GameObject& targetObject, const std::string& name, const std::string& tag)
	{
		auto new_entity = ECS::GetInstance().CloneEntity(targetObject.GetInstanceID());
		return {new_entity, name, tag};
	}

	void GameObjectFactory::DestroyObject(const GameObject& targetobject)
	{
		ECS::GetInstance().DestroyEntity(targetobject.GetInstanceID());
	}

}
