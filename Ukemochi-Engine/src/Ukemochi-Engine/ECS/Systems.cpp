/* Start Header
*****************************************************************/
/*!
\file Systems.cpp
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par junyukean.wong\@digipen.edu
\date 16/09/24
\brief This file contains the definition of the member function of SystemManager

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "systems.h"

namespace Ukemochi
{
	template <typename T>
	std::shared_ptr<T> SystemManager::RegisterSystem()
	{
		const char* typeName = typeid(T).name();

		assert(m_Systems.find(typeName) == m_Systems.end() && "Registering system more than once.");

		auto system = std::make_shared<T>();
		m_Systems.insert({ typeName, system });

		return system;
	}

	template <typename T>
	void SystemManager::SetSystemSignature(Signature signature)
	{
		const char* typeName = typeid(T).name();

		assert(m_Systems.find(typeName) != m_Systems.end() && "System used before registered.");

		m_Signatures.insert({ typeName, signature });
	}

	void SystemManager::EntityDestroyed(Entity entity)
	{
		for (auto const& pair : m_Systems)
		{
			auto const& system = pair.second;

			system->m_Entities.erase(entity);
		}
	}

	void SystemManager::EntitySignatureChanged(Entity entity, Signature entitySignature)
	{
		for (auto const& pair : m_Systems)
		{
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = m_Signatures[type];

			if ((entitySignature & systemSignature) == systemSignature)
			{
				system->m_Entities.insert(entity);
			}
			else
			{
				system->m_Entities.erase(entity);
			}
		}
	}
}