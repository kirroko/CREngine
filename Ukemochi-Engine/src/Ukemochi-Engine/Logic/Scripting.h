/* Start Header
*****************************************************************/
/*!
\file	Scripting.h
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
        MonoAssembly* ClientAssembly = nullptr;

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

        /**
         * @brief Bind C++ function with Mono so they can be invoked from C#
         */
        void RegisterMonoFunctions();

        /**
         * @brief Extracts an error code and message from the given MonoError
         * @param error The MonoError to extract the error from
         * @return true if there is an error, false otherwise
         */
        static bool CheckMonoError(MonoError& error);

        /**
         * @brief Get a reference to a C# class
         * @param MonoAssembly Our C# Assembly
         * @param namespaceName The C# namespaceName
         * @param className The C# class name
         * @return Reference to the class
         */
        MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);

    public:
        static ScriptingEngine& GetInstance()
        {
            static ScriptingEngine instance;

            return instance;
        }

        ScriptingEngine();
        ~ScriptingEngine();

        /**
         * @brief Initialize Mono by creating an app domain
         */
        void Init();

        /**
         * @brief Clean up mono, THIS MUST BE CALLED WHEN THE APPLICATION IS CLOSED
         */
        void ShutDown(); // TODO: Did you remember to call this function when application is closed?

        /**
         * @brief Compile client's script aseembly during runtime
         */
        void CompileScriptAssembly();

        /**
         * @brief Reload the script assembly
         */
        void Reload();

        /**
         * @brief Instantiate C# struct object
         * @param structName the struct name
         * @return an instance of the struct
         */
        MonoObject* InstantiateStruct(const std::string& structName);

        /**
         * @brief Instantiate C# class object that is internal
         * @param className the class name of the internal class
         * @return an instance of the class
         */
        MonoObject* InstantiateClass(const std::string& className);

        /**
         * @brief Instatiate C# class object from client assembly
         * @param className the script's class name
         * @return an instance of the script class
         */
        MonoObject* InstantiateClientClass(const std::string& className);

        /**
         * @brief Instantiate a method from a class. This is for caching the method for future use
         * @param methodName the method name to be instantiated
         * @param instance the instance of the class
         * @return an instance of the method
         */
        MonoMethod* InstatiateMethod(const std::string& methodName, MonoObject* instance);

        /**
         * @brief Invoke script Methods (Start, Update, etc)
         * @param instance C# script instance
         * @param methodName the method name to be invoked
         * @param args the arguments to be passed to the method, default is nullptr
         * @param numArgs the number of arguments, default is 0
         */
        void InvokeMethod(MonoObject* instance, const std::string& methodName, void* args[] = nullptr, int numArgs = 0);

        /**
         * @brief Set the value of a field in a C# class
         * @param instance the instance of the class
         * @param fieldName the field name
         * @param value the value to be set
         */
        static void SetMonoFieldValue(MonoObject* instance, const std::string& fieldName, void* value);

        /**
         * @brief Set the value of a field in a C# class
         * @param instance the instance of the class
         * @param fieldName the field name
         * @param value the value to be set
         */
        static void SetMonoFieldValueString(MonoObject* instance, const std::string& fieldName, const std::string& value);

        /**
         * @brief Set the value of a field in a C# class. Value is of type unsigned long long
         * @param instance the instance of the class
         * @param fieldName the field name
         * @param value the value to be set
         */
        static void SetMonoFieldValueULL(MonoObject* instance, const std::string& fieldName, void* value);

        /**
         * @brief Set the value of a property in a C# class
         * @param instance the instance of the class
         * @param propertyName the property name
         * @param value the value to be set
         */
        void SetMonoPropertyValue(MonoObject* instance, const std::string& propertyName, void* value);

        /**
         * @brief Get the property of a C# class
         * @param instance the instance of the class
         * @param propertyName the property name
         * @return the property
         */
        MonoProperty* GetMonoProperty(MonoObject* instance, const std::string& propertyName);

        /**
         * @brief Set the value of a Vector2 field in a C# class
         * @param instance the instance of the class
         * @param propertyName the field name
         * @param x 
         * @param y 
         */
        void SetVector2Property(MonoObject* instance, const std::string& propertyName, float x, float y);

        /**
        * @brief Convert a MonoString to UTF8(std::string), This function has error handling and will return an empty string
        * @param monoString The MonoString to convert
        * @return std::string The converted string
        */
        static std::string MonoStringToUTF8(MonoString* monoString);
    };
}

// 0x4E41454B
