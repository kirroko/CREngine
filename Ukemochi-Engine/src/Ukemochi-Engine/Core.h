#pragma once

#ifdef UME_PLATFORM_WINDOWS
	#ifdef UME_BUILD_DLL
		#define UME_API _declspec(dllexport)
	#else 
		#define UME_API _declspec(dllimport)
	#endif
#else
	#error UME ONLY SUPPORTS WINDOWS!
#endif

#ifdef UME_ENABLE_ASSERTS
		#define UME_ASSERT(x, ...) { if(!(x)) { UME_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
		#define UME_ENGINE_ASSERT(x, ...) { if(!(x)) { UME_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
		#define UME_ASSERT(x, ...)
		#define UME_ENGINE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define UME_BIND_EVENT(fn) std::bind(&fn, this, std::placeholders::_1)
