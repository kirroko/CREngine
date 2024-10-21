/* Start Header
*****************************************************************/
/*!
\file       Core.h
\author     
\date       Sept 15, 2024
\brief      This file contains macro definitions and configurations for the
			Ukemochi Engine, including DLL export settings,
			and assertion handling.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#ifdef UME_PLATFORM_WINDOWS
	#ifdef UME_BUILD_DLL
		#define UME_API __declspec(dllexport)
	#else 
		#define UME_API __declspec(dllimport)
	#endif
#else
	#error UME ONLY SUPPORTS WINDOWS!
#endif

#ifdef UME_ENABLE_ASSERTS
		#define UME_ASSERT(x, ...) { if(!(x)) { UME_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
		#define UME_ENGINE_ASSERT(x, ...) { if(!(x)) { UME_ENGINE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
		#define UME_ASSERT(x, ...)
		#define UME_ENGINE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define UME_BIND_EVENT(fn) std::bind(&fn, this, std::placeholders::_1)
