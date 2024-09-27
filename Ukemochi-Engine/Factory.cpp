#include "PreCompile.h"
#include "Factory.h"

static std::string in_file_path("../src/Ukemochi-Engine/Serialization/test-input.json");
static std::string out_file_path("../src/Ukemochi-Engine/Serialization/test-output.json");

void Factory::CloneObject(int count, const std::string& type)
{
	using namespace rapidjson;
	GameObj object;
	Document storage;
	bool success = Serialization::LoadJSON(in_file_path, storage);
	if (success)
	{
		const Value& game_objects = storage["Game Objects"];
		if (game_objects.IsArray())
		{
			for (SizeType i = 0; i < game_objects.Size(); i++)
			{
				std::string obj_type = game_objects[i]["name"].GetString();
				if (obj_type.compare(type) == 0)
				{
					if (type.compare("box") == 0)
					{
						object.type = obj_type;
						object.x_val = game_objects[i]["x_val"].GetInt();
						object.y_val = game_objects[i]["y_val"].GetInt();
						object.height = game_objects[i]["height"].GetInt();
						object.width = game_objects[i]["width"].GetInt();
						break;
					}
					else if (type.compare("circle") == 0)
					{
						object.type = obj_type;
						object.x_val = game_objects[i]["x_val"].GetInt();
						object.y_val = game_objects[i]["y_val"].GetInt();
						object.radius = game_objects[i]["radius"].GetInt();
						object.segment = game_objects[i]["segments"].GetInt();
						break;
					}
				}
			}
		}
		for (int k{}; k < count; k++)
		{
			RandomPosition(object.x_val, object.y_val);
			if (type.compare("box") == 0)
			{
				
			}
		}

	}
	else
	{
		return;
	}
}

//Randomise function for width: 1600 and height: 900
void RandomPosition(int& x_value, int& y_value)
{
	x_value = rand() % 1450 + 150;
	y_value = rand() % 750 + 150;
}