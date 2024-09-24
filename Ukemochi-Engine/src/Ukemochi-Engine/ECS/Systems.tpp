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
	void SystemManager::SetSystemSignature(SignatureID signature)
	{
		const char* typeName = typeid(T).name();

		assert(m_Systems.find(typeName) != m_Systems.end() && "System used before registered.");

		m_Signatures.insert({ typeName, signature });
	}
}