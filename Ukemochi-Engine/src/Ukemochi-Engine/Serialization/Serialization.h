/* Start Header ************************************************************************/
/*!
\file       Serialization.h
\author     Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu
\date       Sept 20, 2024
\brief      This file contains the declaration of the Serialization namespace and its functions.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include "../include/rapidjson/document.h"
#include "../include/rapidjson/prettywriter.h"
#include "../include/rapidjson/filereadstream.h"
#include "../include/rapidjson/istreamwrapper.h"
#include "../include/rapidjson/ostreamwrapper.h"


#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

/*
	@author Gerald
	@brief
	tentatively returns string to debug.When Game Object class is establish can change to void and set values straight to be stored in variables
*/

namespace Serialization
{
	bool LoadJSON(const std::string& file_name, rapidjson::Document& storage);

	bool PushJSON(const std::string& file_name, rapidjson::Document& pushing);
};




