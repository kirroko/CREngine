#pragma once

#ifdef CR_PLATFORM_WINDOWS
	#ifdef CR_BUILD_DLL
		#define CR_API _declspec(dllexport)
	#else 
		#define CR_API _declspec(dllimport)
	#endif
#else
	#error CR ONLY SUPPORTS WINDOWS!
#endif

#ifdef CR_ENABLE_ASSERTS
		#define CR_ASSERT(x, ...) { if(!(x)) { CR_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
		#define CR_ENGINE_ASSERT(x, ...) { if(!(x)) { CR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
		#define CR_ASSERT(x, ...)
		#define CR_ENGINE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)


