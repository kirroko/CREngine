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

	std::stringstream sample{};

	sample << "Game Objects:\n";

	const Value& game_objects = storage["Game Objects"];
	if (!game_objects.IsArray())
	{
		input.close();
		return false;
	}
	else
	{
		for (SizeType i = 0; i < game_objects.Size(); i++)
		{
			std::string obj_type = game_objects[i]["name"].GetString();
			if (obj_type.compare("box") == 0)
			{
				sample << "Object " << i + 1 << " :\n";
				sample << "Name : " << obj_type << std::endl;
				sample << "X Value : " << game_objects[i]["x_val"].GetInt() << std::endl;
				sample << "Y Value : " << game_objects[i]["y_val"].GetInt() << std::endl;
				sample << "Width : " << game_objects[i]["width"].GetInt() << std::endl;
				sample << "Height : " << game_objects[i]["height"].GetInt() << std::endl;

				std::cout << sample.str();
			}
			else
			{
				sample << "Object " << i + 1 << " :\n";
				sample << "Name : " << obj_type << std::endl;
				sample << "X Value : " << game_objects[i]["x_val"].GetInt() << std::endl;
				sample << "Y Value : " << game_objects[i]["y_val"].GetInt() << std::endl;
				sample << "Radius : " << game_objects[i]["radius"].GetInt() << std::endl;
				sample << "Segments : " << game_objects[i]["segments"].GetInt() << std::endl;

				std::cout << sample.str();
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



