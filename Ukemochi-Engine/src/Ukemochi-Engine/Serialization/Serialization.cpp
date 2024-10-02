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
		std::cout << "Invalid File Path" << std::endl;
		input.close();
		return false;
	}
	IStreamWrapper isw(input);
	storage.ParseStream(isw);

	//test if Player.json loaded in correctly
	std::size_t is_player_json = file_name.find("Player.json");
	if(is_player_json != std::string::npos)
	{
		const Value& values = storage["GameObject"];
		std::cout << values["Name"].GetString() << std::endl;
		std::cout << values["Tag"].GetString() << std::endl;

		for (auto& comps : values["Components"].GetArray())
		{
			std::string component = comps["Name"].GetString();
			if (component.compare("Transform") == 0)
			{
				std::cout << "Transformation:\n";
				std::cout << "Position: " << comps["Position"][0].GetFloat() << ", " << comps["Position"][1].GetFloat() << std::endl;
				std::cout << "Rotation: " << comps["Rotation"][0].GetFloat() << ", " << comps["Rotation"][1].GetFloat() << std::endl;
				std::cout << "Scale: " << comps["Scale"][0].GetFloat() << ", " << comps["Scale"][1].GetFloat() << std::endl;

			}
			else if (component.compare("Rigidbody2D") == 0)
			{
				std::cout << "Rigidbody2D:\n";
				std::cout << "Velocity: " << comps["Velocity"][0].GetFloat() << ", " << comps["Velocity"][1].GetFloat() << std::endl;
				std::cout << "Acceleration: " << comps["Acceleration"][0].GetFloat() << ", " << comps["Acceleration"][1].GetFloat() << std::endl;
				std::cout << "Kinematic: " << comps["is_kinematic"].GetBool() << std::endl;
			}
			else if (component.compare("BoxCollider2D") == 0)
			{
				std::cout << "BoxCollider2D is present\n";

			}
			else if (component.compare("CircleCollider2D") == 0)
			{
				std::cout << "CircleCollider2D is present\n";
			}
			else if (component.compare("SpriteRender") == 0)
			{
				std::cout << "SpriteRender is present\n";
			}
		}
	}

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



