/* Start Header
*****************************************************************/
/*!
\file	Systems.h
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	15/09/24
\brief	To maintain a record of registered systems and their signatures.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <unordered_map> // std::unordered_map
#include <set>           // std::set
#include <memory>        // std::shared_ptr
#include "Entity.h"      // Entity, Signature

namespace Ukemochi
{
	/**
	 * @brief A system is a collection of entities that share the same signature.
	 *		This allows you to iterate over all entities that have the same components inside your systems class via inheritance.
	 */
	class System
	{
	public:
		std::set<EntityID> m_Entities;
	};

	class SystemManager
	{
		std::unordered_map<const char*, SignatureID> m_Signatures{};

		std::unordered_map<const char*, std::shared_ptr<System>> m_Systems{};

	public:
		template <typename T>
		std::shared_ptr<T> RegisterSystem();

		template <typename T>
		void SetSystemSignature(SignatureID signature);

		void EntityDestroyed(EntityID entity);

		void EntitySignatureChanged(EntityID entity, SignatureID entitySignature);
	};
}
#include "Systems.tpp"
// 0x4B45414E
