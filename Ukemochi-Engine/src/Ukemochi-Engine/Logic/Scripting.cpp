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
#include "../ECS/ECS.h"
#include "InternalCalls.h"

namespace Ukemochi
{
	// ================== PUBLIC FUNCTIONS ==================
	ScriptingEngine::ScriptingEngine()
	{
		// ================== Initialize Mono Runtime ==================
		//mono_set_assemblies_path("../Ukemochi-Engine/vendor/mono/lib/4.5"); // current working dir is Ukemochi-Game
		mono_set_assemblies_path("Mono/lib/4.5");
		
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

	ScriptingEngine::~ScriptingEngine()
	{
		ShutDown(); // If we haven't already unload them
	}

	void ScriptingEngine::Init()
	{
		// Load compiled assemblies into engine (This is our C# code wrapper library for the engine) 
		CoreAssembly = LoadCSharpAssembly("Resources/Scripts/Ukemochi-Scripting.dll");
		UME_ENGINE_ASSERT(CoreAssembly != nullptr, "C# Assembly Null!");
		PrintAssemblyTypes(CoreAssembly); // Print out the types in the assembly for tracing

		RegisterMonoFunctions();

		// TODO - FOR TESTING ONLY, PLEASE REMEMBER TO REMOVE ME 
		// Get a reference to the class we want to instantiate 
		// MonoClass* testingClass = GetClassInAssembly(CoreAssembly, "", "CsharpTest"); 
		//
		// // Allocate an instance of class
		// MonoObject* classInstance = mono_object_new(m_pAppDomain, testingClass);
		//
		// if (classInstance == nullptr)
		// {
		// 	assert(false && "Scripting has an issue, no such class found"); // Engine, Ratatatata ratatatata, pop pop pop, poof Kaput
		// }
		//
		// // Call the parameterless (default) constructor
		// mono_runtime_object_init(classInstance); // without calling this constructor, the instance is oseless
	}

	void ScriptingEngine::Update()
	{
	}

	void ScriptingEngine::ShutDown()
	{
		mono_domain_set(mono_get_root_domain(), false);
		
		mono_jit_cleanup(m_pRootDomain); // Unload the root domain and all the assemblies
	}

	void ScriptingEngine::ReloadScripts()
	{
	}

	MonoObject* ScriptingEngine::InstantiateClass(const std::string& className)
	{
		UME_ENGINE_ASSERT(ClientAssembly != nullptr, "Client Assembly is null!")
		MonoClass* klass = GetClassInAssembly(ClientAssembly, "UkemochiEngine.CoreModule", className.c_str()); // TODO: Client declared Namespace?
		
		MonoObject* instance = mono_object_new(m_pAppDomain, klass);

		if (instance == nullptr)
		{
			UME_ENGINE_ASSERT(false, "Failed to instantiate class: {1}", className);
			return nullptr;
		}

		mono_runtime_object_init(instance);

		return instance;
	}

	MonoObject* ScriptingEngine::InstantiateClass(const std::string& className, const std::string& namespaceName)
	{
		UME_ENGINE_ASSERT(ClientAssembly != nullptr, "Client Assembly is null!")
		MonoClass* klass = GetClassInAssembly(ClientAssembly, namespaceName.c_str(), className.c_str()); // TODO: Client declared Namespace?
		
		MonoObject* instance = mono_object_new(m_pAppDomain, klass);

		if (instance == nullptr)
		{
			UME_ENGINE_ASSERT(false, "Failed to instantiate class: {1}", className);
			return nullptr;
		}

		mono_runtime_object_init(instance);

		return instance;
	}

	MonoMethod* ScriptingEngine::InstatiateMethod(const std::string& methodName, MonoObject* instance)
	{
		MonoClass* klass = mono_object_get_class(instance);
		MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(),0);
		if (!method)
		{
			UME_ENGINE_ERROR("Failed to find method: {0} in class.", methodName);
			return nullptr;
		}

		return method;
	}


	void ScriptingEngine::InvokeMethod(MonoObject* instance, const std::string& methodName, void* args[], int numArgs)
	{
		MonoClass* klass = mono_object_get_class(instance);
		UME_ENGINE_ASSERT(numArgs >= 0, "InvokeMethod has negative number of arguments"); // If you actually give me a negative number, I WILL find you and I will smack you.
		MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), numArgs); // TODO: We would want to save this method for cache purposes
		if (!method)
		{
			UME_ENGINE_ERROR("Failed to find method: {0} in class.", methodName);
			return;
		}

		MonoObject* exception = nullptr; // TODO: Do something with the exception?
		mono_runtime_invoke(method, instance, args, &exception); // TODO: Possible issues would occur here
	}

	// ================== PRIVATE FUNCTIONS ==================
	char* ScriptingEngine::ReadBytes(const std::string& filepath, uint32_t* outSize)
	{
		std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

		if (!stream)
		{
			// Failed to open the file
			UME_ENGINE_WARN("No such file exist!");
			return nullptr;
		}

		std::streampos end = stream.tellg();
		stream.seekg(0, std::ios::beg);
		uint32_t size = end - stream.tellg();

		if (size == 0)
		{
			// File is empty
			return nullptr;
		}

		char* buffer = new char[size];
		stream.read((char*)buffer, size);
		stream.close();

		*outSize = size;
		return buffer;
	}

	MonoAssembly* ScriptingEngine::LoadCSharpAssembly(const std::string& assemblyPath)
	{
		uint32_t fileSize = 0;
		char* fileData = ReadBytes(assemblyPath, &fileSize);

		// NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
		MonoImageOpenStatus status;
		MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

		if (status != MONO_IMAGE_OK)
		{
			const char* errorMessage = mono_image_strerror(status);
			UME_ENGINE_FATAL(errorMessage);
			return nullptr;
		}

		MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
		mono_image_close(image);

		delete[] fileData;

		return assembly;
	}

	void ScriptingEngine::PrintAssemblyTypes(MonoAssembly* assembly)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
		int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

		UME_ENGINE_TRACE("Table of Assembly Types");
		for (int32_t i = 0; i < numTypes; ++i)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
			UME_ENGINE_TRACE("{0}.{1}", nameSpace, name);
		}
	}

	// Use this to bind C++ functions with Mono so they can be invoked from C#
	// Calls from C++ to C# are done through the mono_runtime_invoke function of the MonoObject instance of the class
	void ScriptingEngine::RegisterMonoFunctions() // TODO: Type-Safe Component Registration and Access? Meta-Programming with Reflection??
	{
		// mono_add_internal_call("Ukemochi.CSharpTest::LogMessage",
		// 	(void*)LogMessage);

		//EXTERN_C UME_API void LogMessage(MonoString* message) Example of C# to C++ call
		// {
		// 	UME_ENGINE_INFO("Ukemochi log: {0}", std::string(mono_string_to_utf8(message)));
		// }

		// mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::GetComponentType",
		// 	(void*)GetComponentType);

		mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::AddComponent",
			(void*)InternalCalls::AddComponent);
	}

	MonoClass* ScriptingEngine::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
	{
		MonoImage* image = mono_assembly_get_image(assembly);
		MonoClass* klass = mono_class_from_name(image, namespaceName, className); // klass... Classic name conversion

		if (klass == nullptr) 
		{
			UME_ENGINE_ERROR("No such class name ({0}) in namespace ({1}) inside assembly. Check the class name and namespace, please?",className,namespaceName);
			return nullptr;
		}

		return klass;
	}


}
// 0x4B45414E