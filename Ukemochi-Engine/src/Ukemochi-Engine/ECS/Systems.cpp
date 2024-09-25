/* Start Header
*****************************************************************/
/*!
\file	Systems.cpp
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	16/09/24
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
	void SystemManager::EntityDestroyed(EntityID entity)
	{
		for (auto const& pair : m_Systems)
		{
			auto const& system = pair.second;

			system->m_Entities.erase(entity);
		}
	}

	void SystemManager::EntitySignatureChanged(EntityID entity, SignatureID entitySignature)
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