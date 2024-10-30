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

GameObject& GameObject::operator=(const GameObject& other)
{
	m_Name = other.m_Name;
	m_Tag = other.m_Tag;
	m_InstanceID = other.m_InstanceID;
	return *this;
}

Ukemochi::EntityID Ukemochi::GameObject::GetInstanceID()
{
	return m_InstanceID;
}