/* Start Header
*****************************************************************/
/*!
\file	ScriptingEngine.cpp
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
	void ScriptingEngine::InitMono()
	{
		// ================== Initialize Mono Runtime ==================
		//mono_set_assemblies_path("../Ukemochi-Engine/vendor/mono/lib/4.5"); // current working dir is Ukemochi-Game
		mono_set_assemblies_path("Mono/lib/4.5");

		// where the fuck i am
		try {
			std::filesystem::path cwd = std::filesystem::current_path();
			std::cout << "Current Working Directory: " << cwd << std::endl;
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			UME_ENGINE_ERROR("Error: {0}", std::string(e.what()));
		}

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

		CoreAssembly = LoadCSharpAssembly("Resources/Scripts/Ukemochi-Scripting.dll");
		assert(CoreAssembly != nullptr && "Assembly Null!");
		PrintAssemblyTypes(CoreAssembly);
	}

	void ScriptingEngine::ShutDown()
	{
		mono_domain_set(mono_get_root_domain(), false);

		mono_domain_unload(m_pAppDomain);
		mono_jit_cleanup(m_pRootDomain);
	}

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
			// Log some error message using the errorMessage data
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

		for (int32_t i = 0; i < numTypes; ++i)
		{
			uint32_t cols[MONO_TYPEDEF_SIZE];
			mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

			const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
			const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

			UME_ENGINE_INFO("{0}.{1}\n", nameSpace, name);
		}
	}
}