#include "PreCompile.h"
#include "Serialization.h"

std::string Serialization::LoadJSON(const std::string& file_name)
{
	using namespace rapidjson;

	std::ifstream input(file_name);
	IStreamWrapper isw(input);
	Document storage;
	storage.ParseStream(isw);

	std::stringstream sample{};
	
	sample << "Game Objects:\n";

	const Value& game_objects = storage["Game Objects"];
	if (game_objects.IsArray())
	{
		for (SizeType i = 0; i < game_objects.Size(); i++)
		{
			sample << "Character " << i << " :\n";
			sample << "Name : " << game_objects[i]["Name"].GetString() << std::endl;
			sample << "X_Scale : " << game_objects[i]["x_scale"].GetFloat() << std::endl;
			sample << "Y_Scale : " << game_objects[i]["y_scale"].GetFloat() << std::endl;
			sample << " : " << game_objects[i]["Weapon"].GetString() << std::endl;

		}
	}

	return sample.str();
}

void Serialization::PushJSON(const std::string& file_name, rapidjson::Document& pushing )
{
	using namespace rapidjson;
	
	std::ofstream output(file_name);
	OStreamWrapper osw(output);

	Writer<OStreamWrapper> writer(osw);
	pushing.Accept(writer);

	//Testing Comment
}

	

	