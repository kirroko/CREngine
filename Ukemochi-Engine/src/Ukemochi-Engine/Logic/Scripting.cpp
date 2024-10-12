/* Start Header
*****************************************************************/
/*!
\file	Scripting.cpp
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	11/10/24
\brief	This file handles everything related to Mono (i.e., initialization, script execution, shutdown)

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "Scripting.h"

namespace Ukemochi
{
	void Scripting::InitMono()
	{
		// ================== Initialize Mono Runtime ==================
		mono_set_assemblies_path("../Ukemochi-Engine/vendor/Mono/lib/4.5");

		MonoDomain* rootDomain = mono_jit_init("MonoRuntime");
		if (rootDomain == nullptr)
		{
			UME_ENGINE_WARN("Mono failed to initialize!");
			return;
		}
		// Store the root domain pointer
		m_pRootDomain = rootDomain;

		// Create an App Domain
		m_pAppDomain = mono_domain_create_appdomain("UkemochiDomain", nullptr);
		mono_domain_set(m_pAppDomain, true);
	}
}