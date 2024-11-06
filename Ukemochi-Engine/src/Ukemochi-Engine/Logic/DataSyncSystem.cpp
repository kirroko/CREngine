/* Start Header
*****************************************************************/
/*!
\file	DataSyncSystem.cpp
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	4/11/24
\brief	This file is responsible for passing data to C# so that scripts have the latest information.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "DataSyncSystem.h"

#include "Ukemochi-Engine/ECS/ECS.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"

using namespace Ukemochi;

void DataSyncSystem::SyncData()
{
    for(auto& entity : m_Entities)
    {
        auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
        auto& rb = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);

        auto& go = GameObjectManager::GetInstance().GetGO(entity);
        MonoObject* TransPoint = go.GetManagedInstance("Transform");
        MonoObject* rbPoint = go.GetManagedInstance("Rigidbody2D");

        // TODO: Encapsulate this bad bois below (ALL OF IT) into a function at scriptingEngine 
        MonoProperty* position = ScriptingEngine::GetInstance().GetMonoProperty(TransPoint, "Position");
        if(position == nullptr)
            UME_ENGINE_ASSERT(false, "DataSync: Position is null!")
        MonoProperty* scale = ScriptingEngine::GetInstance().GetMonoProperty(TransPoint, "Scale");
        if(scale == nullptr)
            UME_ENGINE_ASSERT(false, "DataSync: Rotation is null!")
        MonoProperty* velocity = ScriptingEngine::GetInstance().GetMonoProperty(rbPoint, "Velocity");
        if(velocity == nullptr)
            UME_ENGINE_ASSERT(false, "DataSync: Velocity is null!")
        MonoProperty* acceleration = ScriptingEngine::GetInstance().GetMonoProperty(rbPoint, "Acceleration");
        if(acceleration == nullptr)
            UME_ENGINE_ASSERT(false, "DataSync: Acceleration is null!")
        MonoProperty* force = ScriptingEngine::GetInstance().GetMonoProperty(rbPoint, "Force");
        if(force == nullptr)
            UME_ENGINE_ASSERT(false, "DataSync: Force is null!")
        
        MonoObject* positionValue = mono_property_get_value(position, TransPoint, nullptr, nullptr);
        MonoObject* scaleValue = mono_property_get_value(scale, TransPoint, nullptr, nullptr);
        MonoObject* velocityValue = mono_property_get_value(velocity, rbPoint, nullptr, nullptr);
        MonoObject* accelerationValue = mono_property_get_value(acceleration, rbPoint, nullptr, nullptr);
        MonoObject* forceValue = mono_property_get_value(force, rbPoint, nullptr, nullptr);

        MonoClass* vector2Position = mono_object_get_class(positionValue);
        MonoClassField* xField = mono_class_get_field_from_name(vector2Position, "x");
        MonoClassField* yField = mono_class_get_field_from_name(vector2Position, "y");

        MonoClass* vector2Scale = mono_object_get_class(scaleValue);
        MonoClassField* xFieldScale = mono_class_get_field_from_name(vector2Scale, "x");
        MonoClassField* yFieldScale = mono_class_get_field_from_name(vector2Scale, "y");

        MonoClass* vector2Velocity = mono_object_get_class(velocityValue);
        MonoClassField* xFieldVelocity = mono_class_get_field_from_name(vector2Velocity, "x");
        MonoClassField* yFieldVelocity = mono_class_get_field_from_name(vector2Velocity, "y");

        MonoClass* vector2Acceleration = mono_object_get_class(accelerationValue);
        MonoClassField* xFieldAcceleration = mono_class_get_field_from_name(vector2Acceleration, "x");
        MonoClassField* yFieldAcceleration = mono_class_get_field_from_name(vector2Acceleration, "y");

        MonoClass* vector2Force = mono_object_get_class(forceValue);
        MonoClassField* xFieldForce = mono_class_get_field_from_name(vector2Force, "x");
        MonoClassField* yFieldForce = mono_class_get_field_from_name(vector2Force, "y");
        
        ScriptingEngine::GetInstance().SetVector2Property(TransPoint, "Position", transform.position.x, transform.position.y);
        
        ScriptingEngine::GetInstance().SetVector2Property(TransPoint, "Scale", transform.scale.x, transform.scale.y);
        
        ScriptingEngine::GetInstance().SetVector2Property(rbPoint, "Velocity", rb.velocity.x, rb.velocity.y);
        
        ScriptingEngine::GetInstance().SetVector2Property(rbPoint, "Acceleration", rb.acceleration.x, rb.acceleration.y);
        
        ScriptingEngine::GetInstance().SetVector2Property(rbPoint, "Force", rb.force.x, rb.force.y);
    }
}
