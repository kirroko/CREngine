/* Start Header ************************************************************************/
/*!
\file       PreCompile.h
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 14, 2024
\brief      This file contains the declaration of the PreCompile.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <utility>
#include <algorithm>
#include <functional>
#include <bitset>
#include <queue>
#include <array>
#include <set>
#include <random>
#include <filesystem>

#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Ukemochi-Engine/Logs/Log.h"
#include "Ukemochi-Engine/Game/GameStateList.h"
#include "Ukemochi-Engine/Audio/AudioList.h"

#ifdef CR_PLATFORM_WINDOWS
#include "../src/Platform/WindowsWindow.h"
#endif