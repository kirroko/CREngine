/* Start Header ************************************************************************/
/*!
\file       Core.h
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 15, 2024
\brief      This file contains macro definitions and configurations for the
			Ukemochi Engine, including DLL export settings,
			and assertion handling.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

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

namespace Ukemochi {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

}

// Explicitly export std::vector<std::string> in case of using it in DLL boundary
#ifdef UME_BUILD_DLL
    template class UME_API std::vector<std::string>;
#endif