/* Start Header ************************************************************************/
/*!
\file       GameObject.cpp
\author     Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (50%)
\co-authors Wong Jun Yu, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (50%)
\date       Sept 29, 2024
\brief      This file contains the declaration of the GameObject class. This class is responsible for
        creating and managing game objects. This applies to the C# side as well. It reflects the gameobject instance
        on the C# side of things.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "GameObject.h"
using namespace Ukemochi;

GameObject::GameObject(EntityID id, std::string name, std::string tag): m_Name(std::move(name)),
                                                                        m_Tag(std::move(tag)), m_InstanceID(id)
{
    // m_ManagedInstance = ScriptingEngine::GetInstance().InstantiateClass("GameObject");
    //
    // m_ManagedInstanceHandle = mono_gchandle_new_v2(m_ManagedInstance, true);
    // ScriptingEngine::SetMonoFieldValueULL(m_ManagedInstance, "_id", &m_InstanceID);
    // ScriptingEngine::SetMonoFieldValueString(m_ManagedInstance, "name", m_Name);
}

GameObject& GameObject::operator=(const GameObject& other)
{
    m_Name = other.m_Name;
    m_Tag = other.m_Tag;
    m_InstanceID = other.m_InstanceID;
    
    return *this;
}

EntityID GameObject::GetInstanceID() const
{
    return m_InstanceID;
}

bool GameObject::GetActive() const
{
    return m_isActive;
}
