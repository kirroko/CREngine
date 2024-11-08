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

void DataSyncSystem::SyncData() const
{
    for(auto& entity : m_Entities)
    {
        auto& transform = ECS::GetInstance().GetComponent<Transform>(entity);
        auto& rb = ECS::GetInstance().GetComponent<Rigidbody2D>(entity);

        auto& go = GameObjectManager::GetInstance().GetGO(entity);
        MonoObject* TransPoint = go.GetManagedInstance("Transform");
        MonoObject* rbPoint = go.GetManagedInstance("Rigidbody2D");
        
        ScriptingEngine::GetInstance().SetVector2Property(TransPoint, "position", transform.position.x, transform.position.y);
        
        ScriptingEngine::GetInstance().SetVector2Property(TransPoint, "scale", transform.scale.x, transform.scale.y);
        
        ScriptingEngine::GetInstance().SetVector2Property(rbPoint, "Velocity", rb.velocity.x, rb.velocity.y);
        
        ScriptingEngine::GetInstance().SetVector2Property(rbPoint, "Acceleration", rb.acceleration.x, rb.acceleration.y);
        
        ScriptingEngine::GetInstance().SetVector2Property(rbPoint, "Force", rb.force.x, rb.force.y);
    }
}
