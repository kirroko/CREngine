/* Start Header
*****************************************************************/
/*!
\file		GameObject.cpp
\par		Ukemochi
\author		Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu
\co-authors Wong Jun Yu, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\par		Course: CSD2400/CSD2401
\date		29/09/24
\brief		This file contains the declaration of the GameObject class. This class is responsible for
        creating and managing game objects. This applies to the C# side as well. It reflects the gameobject instance
        on the C# side of things

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "GameObject.h"
using namespace Ukemochi;

GameObject::GameObject(EntityID id, std::string name, std::string tag): m_Name(std::move(name)),
                                                                        m_Tag(std::move(tag)), m_InstanceID(id)
{
    m_ManagedInstance = ScriptingEngine::GetInstance().InstantiateClass("GameObject");

    m_ManagedInstanceHandle = mono_gchandle_new_v2(m_ManagedInstance, true);
    ScriptingEngine::SetMonoFieldValueULL(m_ManagedInstance, "_id", &m_InstanceID);
    ScriptingEngine::SetMonoFieldValueString(m_ManagedInstance, "name", m_Name);
}

GameObject& GameObject::operator=(const GameObject& other)
{
    m_Name = other.m_Name;
    m_Tag = other.m_Tag;
    m_InstanceID = other.m_InstanceID;
    // TODO: Copy the managed instance and components 7/11/24
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
    m_ManagedComponentsHandle[typeName] = mono_gchandle_new_v2(instance, true);
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
    for (auto& pair : m_ManagedComponentsHandle)
    {
        mono_gchandle_free_v2(pair.second);
    }
}
