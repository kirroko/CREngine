#pragma once
#include "PreCompile.h"
#include "GameObject.h"
#include "../Serialization/Serialization.h"

namespace Ukemochi
{
	class GameObjectFactory
	{
		std::vector<std::shared_ptr<GameObject>> m_ActiveGO;

		std::size_t m_LivingGOCount;
	public:
		GameObjectFactory() : m_LivingGOCount(0), m_ActiveGO{} {};

		static GameObject& CreateObject();
		static GameObject& CloneObject(GameObject& targetObject);

		static void CloneObject(int count, const std::string& type);
	};
};