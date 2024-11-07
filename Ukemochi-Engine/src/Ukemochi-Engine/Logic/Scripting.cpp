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
        //ShutDown(); // If we haven't already unload them
    }

    void ScriptingEngine::Init()
    {
        // Load compiled assemblies into engine (This is our C# code wrapper library for the engine)
        UME_ENGINE_INFO("Loading Core Assembly");
        CoreAssembly = LoadCSharpAssembly("Resources/Scripts/Ukemochi-Scripting.dll");
        UME_ENGINE_ASSERT(CoreAssembly != nullptr, "C# Assembly Null!")
        PrintAssemblyTypes(CoreAssembly); // Print out the types in the assembly for tracing

        RegisterMonoFunctions();

        UME_ENGINE_INFO("Compiling Script Assembly");

        CompileScriptAssembly();
        UME_ENGINE_INFO("Loading Client Assembly");
        ClientAssembly = LoadCSharpAssembly("Resources/Scripts/Assembly-CSharp.dll");
        UME_ENGINE_ASSERT(ClientAssembly != nullptr, "Client Assembly is null!")
        PrintAssemblyTypes(ClientAssembly); // Print out the types in the assembly for tracing
    }

    void ScriptingEngine::ShutDown()
    {
        mono_domain_set(mono_get_root_domain(), false);

        mono_jit_cleanup(m_pRootDomain); // Unload the root domain and all the assemblies
    }

    void ScriptingEngine::CompileScriptAssembly()
    {
        // Search for the csproj file
        std::string csprojPath = "../Assets/Assembly-CSharp.csproj"; // TODO: Hardcoded path here
        UME_ENGINE_ASSERT(std::filesystem::exists(csprojPath), "csproj file does not exist!")

        // Set to build in Release configuration, default is also debug
        // TODO: Remember to change the configuration to Release
        std::wstring buildCmd = L"dotnet build -c Debug " + std::wstring(csprojPath.begin(), csprojPath.end());
        UME_ENGINE_INFO("Executing Dotnet build!");
        int result = _wsystem(buildCmd.c_str()); // Execute the build command
        if (result == 0) // Successful
        {
            UME_ENGINE_INFO("Successfully compiled the script assembly!");
            try
            {
                std::filesystem::path sourcePath = "../Assets/bin/Debug/net472/Assembly-CSharp.dll";
                std::filesystem::path destinationPath = "Resources/Scripts/Assembly-CSharp.dll";

                if (std::filesystem::exists(sourcePath))
                {
                    std::filesystem::create_directories(destinationPath.parent_path());
                    std::filesystem::copy_file(sourcePath, destinationPath,
                                               std::filesystem::copy_options::overwrite_existing);
                    UME_ENGINE_INFO("Successfully copied the script assembly!");
                }
                else
                {
                    UME_ENGINE_ERROR("Source file does not exist: {0} | Current Path {1}", sourcePath.string(),
                                     std::filesystem::current_path().string());
                }
            }
            catch (const std::filesystem::filesystem_error& e)
            {
                UME_ENGINE_ERROR("Filesystem error: {0}", e.what());
            }
        }
        else // Failed
            UME_ENGINE_ERROR("Failed to compile the script assembly!");
    }

    void ScriptingEngine::Reload()
    {
        mono_domain_unload(m_pAppDomain); // Unload the app domain and all the assemblies
        m_pAppDomain = mono_domain_create_appdomain("UkemochiDomain", nullptr);
        mono_domain_set(m_pAppDomain, true);

        Init(); // Reinitialize the engine
        // Load client assembly
        ClientAssembly = LoadCSharpAssembly("Resources/Scripts/Assembly-CSharp.dll");
        UME_ENGINE_ASSERT(ClientAssembly != nullptr, "Client Assembly is null!")
        PrintAssemblyTypes(ClientAssembly); // Print out the types in the assembly for tracing

        // TODO: Our MonoObject instance are still pointing to the old assembly, we need to update them
    }

    MonoObject* ScriptingEngine::InstantiateStruct(const std::string& structName)
    {
        UME_ENGINE_ASSERT(CoreAssembly != nullptr, "Core Assembly is null!")
        MonoClass* klass = GetClassInAssembly(CoreAssembly, "UkemochiEngine.CoreModule", structName.c_str());
        if (klass == nullptr)
            UME_ENGINE_ASSERT(false, "Failed to instantiate struct: {1}", structName)

        MonoObject* instance = mono_value_box(m_pAppDomain, klass, nullptr);

        return instance;
    }

    MonoObject* ScriptingEngine::InstantiateClass(const std::string& className)
    {
        UME_ENGINE_ASSERT(CoreAssembly != nullptr, "Core Assembly is null!")
        MonoClass* klass = GetClassInAssembly(CoreAssembly, "UkemochiEngine.CoreModule", className.c_str());
        // TODO: Client declared Namespace?
        if (klass == nullptr)
            UME_ENGINE_ASSERT(false, "Failed to instantiate class: {1}", className)

        MonoObject* instance = mono_object_new(m_pAppDomain, klass);

        if (instance == nullptr)
            UME_ENGINE_ASSERT(false, "Failed to instantiate object: {1}", className)

        mono_runtime_object_init(instance);

        return instance;
    }

    MonoObject* ScriptingEngine::InstantiateClientClass(const std::string& className)
    {
        UME_ENGINE_ASSERT(ClientAssembly != nullptr, "Client Assembly is null!")
        MonoClass* klass = GetClassInAssembly(ClientAssembly, "", className.c_str());
        if (klass == nullptr)
            UME_ENGINE_ASSERT(false, "Failed to instantiate class: {1}", className)

        MonoObject* instance = mono_object_new(m_pAppDomain, klass);

        if (instance == nullptr)
        {
            UME_ENGINE_ASSERT(false, "Failed to instantiate class: {1}", className)
            return nullptr;
        }
        UME_ENGINE_TRACE("ScriptingEngine: Instantiated class {0}", className);

        mono_runtime_object_init(instance);

        return instance;
    }

    MonoMethod* ScriptingEngine::InstatiateMethod(const std::string& methodName, MonoObject* instance)
    {
        MonoClass* klass = mono_object_get_class(instance);
        MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), 0);
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
        UME_ENGINE_ASSERT(numArgs >= 0, "InvokeMethod has negative number of arguments")
        // Don't do this, be better please
        MonoMethod* method = mono_class_get_method_from_name(klass, methodName.c_str(), numArgs);
        // TODO: We would want to save this method for cache purposes
        if (!method)
        {
            // If the method is not found, check if the coreAssembly or ClientAssembly been rebuilt and reloaded
            UME_ENGINE_ERROR("Failed to find method: {0} in class.", methodName);
            return;
        }
        // UME_ENGINE_TRACE("ScriptingEngine: Invoking method {0} in class {1}", methodName, mono_class_get_name(klass));
        MonoObject* exception = nullptr; // TODO: Do something with the exception?
        mono_runtime_invoke(method, instance, args, &exception); // TODO: Possible issues would occur here
        if (exception != nullptr)
        {
            MonoClass* exceptionClass = mono_object_get_class(exception);
            MonoProperty* messageProperty = mono_class_get_property_from_name(exceptionClass, "Message");
            MonoString* message = (MonoString*)mono_property_get_value(messageProperty, exception, nullptr, nullptr);
            char* messageStr = mono_string_to_utf8(message);
            std::string messageString = messageStr;
            UME_ENGINE_WARN("Scripting Log: " + messageString);
            mono_free(messageStr);
        }
    }

    void ScriptingEngine::SetMonoFieldValue(MonoObject* instance, const std::string& fieldName, void* value)
    //TODO : Turn to template, get data type for field value
    {
        MonoClass* klass = mono_object_get_class(instance);
        MonoClassField* field = mono_class_get_field_from_name(klass, fieldName.c_str());

        MonoTypeEnum type = static_cast<MonoTypeEnum>(mono_type_get_type(mono_field_get_type(field))); // DEBUGGING
        // UME_ENGINE_INFO("Scripting Log: Setting type {0} for field {1}", static_cast<int>(type), fieldName);

        mono_field_set_value(instance, field, value);

        float fieldValue;
        void* param = &fieldValue;
        mono_field_get_value(instance, field, param);
        // UME_ENGINE_INFO("Scripting Log: Field {0} has value {1}", fieldName, fieldValue);
    }

    void ScriptingEngine::SetMonoFieldValueULL(MonoObject* instance, const std::string& fieldName, void* value)
    {
        MonoClass* klass = mono_object_get_class(instance);
        MonoClassField* field = mono_class_get_field_from_name(klass, fieldName.c_str());

        mono_field_set_value(instance, field, value);
    }

    void ScriptingEngine::SetMonoPropertyValue(MonoObject* instance, const std::string& propertyName, void* value)
    {
        MonoProperty* property = mono_class_get_property_from_name(mono_object_get_class(instance),
                                                                   propertyName.c_str());
        void* args[] = {value};
        mono_property_set_value(property, instance, args, nullptr);
    }

    MonoProperty* ScriptingEngine::GetMonoProperty(MonoObject* instance, const std::string& propertyName)
    {
        MonoProperty* property = mono_class_get_property_from_name(mono_object_get_class(instance),
                                                                   propertyName.c_str());
        if (property == nullptr)
            UME_ENGINE_ERROR("Failed to get property: {0}", propertyName);

        return property;
    }

    void ScriptingEngine::SetVector2Property(MonoObject* instance, const std::string& propertyName, float x, float y)
    {
        UME_ENGINE_ASSERT(CoreAssembly != nullptr, "Core Assembly is null!")
        MonoProperty* property = mono_class_get_property_from_name(mono_object_get_class(instance),
                                                                   propertyName.c_str());
        if (!property)
            UME_ENGINE_ASSERT(false, "Property not found: {1}", propertyName)

        MonoObject* vector2Struct = mono_property_get_value(property, instance, nullptr, nullptr);
        if (!vector2Struct)
            UME_ENGINE_ASSERT(false, "Failed to get Vector2 struct")

        struct Vector2Data
        {
            float x;
            float y;
        };
        // void* data = mono_object_unbox(vector2Struct);
        // Vector2Data* vector2Data = static_cast<Vector2Data*>(data);
        // auto* vector2Data = static_cast<Vector2Data*>(mono_object_unbox(vector2Struct));

        SetMonoFieldValue(vector2Struct, "x", &x);
        SetMonoFieldValue(vector2Struct, "y", &y);

        // void* args[] = { vector2Struct };
        // MonoObject* exception = nullptr;
        // mono_property_set_value(property, instance, args, &exception);
        // if(exception != nullptr)
        // {
        //     MonoClass* exceptionClass = mono_object_get_class(exception);
        //     MonoProperty* messageProperty = mono_class_get_property_from_name(exceptionClass, "Message");
        //     MonoString* message = (MonoString*)mono_property_get_value(messageProperty, exception, nullptr, nullptr);
        //     char* messageStr = mono_string_to_utf8(message);
        //     std::string messageString = messageStr;
        //     UME_ENGINE_WARN(messageString);
        //     mono_free(messageStr);
        // }

        //         // Create a new Vector2 Obj
        //         UME_ENGINE_ASSERT(CoreAssembly != nullptr, "Core Assembly is null!")
        //         MonoClass* klass = GetClassInAssembly(CoreAssembly, "UkemochiEngine.CoreModule", "Vector2");
        //         if (klass == nullptr)
        //             UME_ENGINE_ASSERT(false, "Failed to instantiate struct: {1}", "Vector2")
        //
        // #pragma pack(push, 1) // Ensure no padding is added
        //         struct
        //         {
        //             float x;
        //             float y;
        //         } vector2Instance = {x, y};
        // #pragma pack(pop)
        //
        //         MonoObject* vector2Obj = mono_value_box(m_pAppDomain, klass, &vector2Instance);
        //         if (!vector2Obj)
        //             UME_ENGINE_ASSERT(false, "Failed to instantiate Vector2 struct")
        //
        //         // Set the x and y fields on the Vector2 Object
        //         // SetMonoFieldValue(vector2Obj, "x", &x);
        //         // SetMonoFieldValue(vector2Obj, "y", &y);
        //         
        //         // Get the property on the Object
        //         MonoProperty* property = mono_class_get_property_from_name(mono_object_get_class(instance),
        //                                                                    propertyName.c_str());
        //         if(!property)
        //             UME_ENGINE_ASSERT(false, "Failed to get property: {1}", propertyName)
        //
        //         // Set the Vector2 property on the object
        //         void* agrs[] = { vector2Obj };
        //         mono_property_set_value(property, instance, agrs, nullptr);
        //
        //         // Debug logging
        //         UME_ENGINE_INFO("SetVector2Property: Set {0} to ({1}, {2})", propertyName, x, y);
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
        uint32_t size = static_cast<uint32_t>(end - stream.tellg());

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
    void ScriptingEngine::RegisterMonoFunctions()
    // TODO: Type-Safe Component Registration and Access? Meta-Programming with Reflection??
    {
        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::AddComponent",
                               (void*)InternalCalls::AddComponent);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::GetMonoObjectByTag",
                               (void*)InternalCalls::GetMonoObjectByTag);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::GetKey",
                               (void*)InternalCalls::GetKey);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::GetKeyDown",
                               (void*)InternalCalls::GetKeyDown);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::LogMessage",
                               (void*)InternalCalls::LogMessage);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetTransformPosition",
                               (void*)InternalCalls::SetTransformPosition);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetTransformRotation",
                               (void*)InternalCalls::SetTransformRotation);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetTransformScale",
                               (void*)InternalCalls::SetTransformScale);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyVelocity",
                               (void*)InternalCalls::SetRigidbodyVelocity);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyAcceleration",
                               (void*)InternalCalls::SetRigidbodyAcceleration);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyForce",
                               (void*)InternalCalls::SetRigidbodyForce);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyMass",
                               (void*)InternalCalls::SetRigidbodyMass);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyInverseMass",
                               (void*)InternalCalls::SetRigidbodyInverseMass);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyDrag",
                               (void*)InternalCalls::SetRigidbodyDrag);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyAngle",
                               (void*)InternalCalls::SetRigidbodyAngle);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyAngularVelocity",
                               (void*)InternalCalls::SetRigidbodyAngularVelocity);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyTorque",
                               (void*)InternalCalls::SetRigidbodyTorque);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyInertiaMass",
                               (void*)InternalCalls::SetRigidbodyInertiaMass);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyInverseInertiaMass",
                               (void*)InternalCalls::SetRigidbodyInverseInertiaMass);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyAngularDrag",
                               (void*)InternalCalls::SetRigidbodyAngularDrag);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyUseGravity",
                               (void*)InternalCalls::SetRigidbodyUseGravity);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetRigidbodyIsKinematic",
                               (void*)InternalCalls::SetRigidbodyIsKinematic);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetSpriteRenderFlipX",
                               (void*)InternalCalls::SetSpriteRenderFlipX);

        mono_add_internal_call("UkemochiEngine.CoreModule.EngineInterop::SetSpriteRenderFlipY",
                               (void*)InternalCalls::SetSpriteRenderFlipY);
    }

    bool ScriptingEngine::CheckMonoError(MonoError& error)
    {
        bool hasError = !mono_error_ok(&error);
        if (hasError)
        {
            unsigned short errorCode = mono_error_get_error_code(&error);
            const char* errorMessage = mono_error_get_message(&error);
            UME_ENGINE_ERROR("Mono Error!");
            UME_ENGINE_ERROR("\tError Code: {0}", errorCode);
            UME_ENGINE_ERROR("\tError Message: {0}", errorMessage);
            mono_error_cleanup(&error);
        }
        return hasError;
    }

    std::string ScriptingEngine::MonoStringToUTF8(MonoString* monoString)
    {
        if (monoString == nullptr || mono_string_length(monoString) == 0)
            return "";

        MonoError error;
        char* utf8 = mono_string_to_utf8_checked(monoString, &error);
        if (CheckMonoError(error))
            return "";
        std::string result(utf8);
        mono_free(utf8);
        return result;
    }

    MonoClass* ScriptingEngine::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName,
                                                   const char* className)
    {
        MonoImage* image = mono_assembly_get_image(assembly);
        MonoClass* klass = mono_class_from_name(image, namespaceName, className); // klass... Classic name conversion

        if (klass == nullptr)
        {
            UME_ENGINE_ERROR(
                "No such class name ({0}) in namespace ({1}) inside assembly. Check the class name and namespace, please?",
                className, namespaceName);
            return nullptr;
        }

        return klass;
    }
}

// 0x4B45414E
