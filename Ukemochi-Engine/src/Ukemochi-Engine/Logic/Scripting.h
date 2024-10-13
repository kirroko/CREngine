/* Start Header
*****************************************************************/
/*!
\file	ScriptingEngine.h
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
#pragma once
#include "PreCompile.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

namespace Ukemochi
{
	class ScriptingEngine
	{
		MonoDomain* m_pRootDomain = nullptr;
		MonoDomain* m_pAppDomain = nullptr;

		MonoAssembly* CoreAssembly = nullptr;

		ScriptingEngine() = default;
		~ScriptingEngine() = default;
		ScriptingEngine(const ScriptingEngine&) = delete;
		ScriptingEngine& operator=(const ScriptingEngine&) = delete;

		/**
		 * @brief load a file into an array of bytes
		 *
		 * @param filePath The absolute path or relative pathing
		 * @param outSize The output data size
		 * @return char* pointer to the char buffer with data
		 */
		char* ReadBytes(const std::string& filePath, uint32_t* outSize);

		/**
		 * @brief Load Csharp Assembly
		 *
		 * @param assemblyPath Path to the assembly
		 * @return MonoAssembly* Pointer to the assembly
		 */
		MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);

		/**
		 * @brief Proper testing by iterating over all the class types define in our assembly (i.e., classes, structs and enums)
		 *
		 * @param assembly Our MonoAssembly
		 */
		void PrintAssemblyTypes(MonoAssembly* assembly);

	public:
		static ScriptingEngine& GetInstance()
		{
			static ScriptingEngine instance;
			return instance;
		}

		/**
		 * @brief Initialize Mono by creating an app domain
		 */
		void InitMono();

		/**
		 * @brief Clean up mono
		 */
		void ShutDown();
	};
}