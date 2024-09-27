#pragma once

#include "../rapidjson/document.h"
#include "../rapidjson/prettywriter.h"
#include "../rapidjson/filereadstream.h"
#include "../rapidjson/istreamwrapper.h"
#include "../rapidjson/ostreamwrapper.h"

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdio>
constexpr size_t BUFFER_SIZE{ 65536 };

/*
	@author Gerald
	@brief
	tentatively returns string to debug.When Game Object class is establish can change to void and set values straight to be stored in variables
*/

class Serialization
{
	bool LoadJSON(const std::string& file_name, rapidjson::Document& storage);

	bool PushJSON(const std::string& file_name, rapidjson::Document& pushing);
};




