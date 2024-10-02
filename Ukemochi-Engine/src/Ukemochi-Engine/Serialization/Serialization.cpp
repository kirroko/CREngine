/* Start Header
*****************************************************************/
/*!
\file		Serialization.cpp
\par		Ukemochi
\author		Pek Jun Kai Gerald, p.junkaigerald, 2301334
\co-authors
\par		p.junkaigerald\@digipen.edu
\par		Course: CSD2400/CSD2401
\date		20/09/24
\brief		This file contains the definition of the functions in the Serialization namespace.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "PreCompile.h"
#include "Serialization.h"

bool Serialization::LoadJSON(const std::string& file_name, rapidjson::Document& storage)
{
	using namespace rapidjson;

	std::ifstream input(file_name);
	if (!input.is_open())
	{
		input.close();
		return false;
	}
	IStreamWrapper isw(input);
	storage.ParseStream(isw);

	input.close();
	return true;
}

bool Serialization::PushJSON(const std::string& file_name, rapidjson::Document& storage)
{
	using namespace rapidjson;

	std::ofstream output(file_name);
	if (!output.is_open())
	{
		output.close();
		return false;
	}
	OStreamWrapper osw(output);

	Writer<OStreamWrapper> writer(osw);
	storage.Accept(writer);
	
	output.close();
	return true;
	//Testing Comment
}



