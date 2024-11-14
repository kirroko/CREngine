/* Start Header ************************************************************************/
/*!
\file       Systems.tpp
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Sep 15, 2024
\brief      To maintain a record of registered systems and their signatures.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "Systems.h"

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
	std::shared_ptr<T> SystemManager::GetSystem()
	{
		const char* typeName = typeid(T).name();
		
		assert(m_Systems.find(typeName) != m_Systems.end() && "System used before registered.");

		return std::static_pointer_cast<T>(m_Systems[typeName]);
	}

	template <typename T>
	void SystemManager::SetSystemSignature(SignatureID signature)
	{
		const char* typeName = typeid(T).name();

		assert(m_Systems.find(typeName) != m_Systems.end() && "System used before registered.");

		m_Signatures.insert({ typeName, signature });
	}
}