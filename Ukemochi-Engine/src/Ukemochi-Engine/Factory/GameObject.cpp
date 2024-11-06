/* Start Header
*****************************************************************/
/*!
\file		GameObject.cpp
\par		Ukemochi
\author		Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu
\co-authors Wong Jun Yu, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\par		Course: CSD2400/CSD2401
\date		29/09/24
\brief		This file contains the definition of the GameObject class. This class is responsible for
		creating and managing game objects.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "GameObject.h"
using namespace Ukemochi;

GameObject::GameObject(EntityID id, std::string name, std::string tag): m_Name(name),
                                                                        m_Tag(tag), m_InstanceID(id)
{
    m_ManagedInstance = ScriptingEngine::GetInstance().InstantiateClass("GameObject");

    m_ManagedInstanceHandle = mono_gchandle_new_v2(m_ManagedInstance,true);
    ScriptingEngine::GetInstance().SetMonoFieldValueULL(m_ManagedInstance, "_id", &m_InstanceID);
    // void* params[] = {&m_InstanceID};
    // ScriptingEngine::GetInstance().InvokeMethod(m_ManagedInstance, "SetID", params, 1);
    // AddComponent(Transform{Vec2(),0.0f,Vec2(1.0f,1.0f)}); // We don't set a default transform here, Other programmer will have to remember to add  instaed...
}

GameObject& GameObject::operator=(const GameObject& other)
{
    m_Name = other.m_Name;
    m_Tag = other.m_Tag;
    m_InstanceID = other.m_InstanceID;
    return *this;
}

GameObject::~GameObject()
{
    mono_gchandle_free_v2(m_ManagedInstanceHandle);
    for (auto& component : m_ManagedComponentsHandle)
    {
        mono_gchandle_free_v2(component.second);
    }
}

MonoObject* GameObject::GetManagedInstance(const std::string& objectName) const
{
    if (strcmp(objectName.c_str(), "GameObject") == 0) // Well... Just in case yeah
        return m_ManagedInstance;
    
    return m_ManagedComponents.at(objectName);
}

void GameObject::SetManagedComponentInstance(MonoObject* instance, const std::string& typeName)
{
    m_ManagedComponentsHandle[typeName] = mono_gchandle_new_v2(instance,true);
    m_ManagedComponents[typeName] = instance;
}

MonoObject* GameObject::GetManagedInstance() const
{
    return m_ManagedInstance;
}

EntityID GameObject::GetInstanceID() const
{
    return m_InstanceID;
}

void GameObject::ReleaseHandle() const
{
    for(auto& pair : m_ManagedComponentsHandle)
    {
        mono_gchandle_free_v2(pair.second);
    }
}
