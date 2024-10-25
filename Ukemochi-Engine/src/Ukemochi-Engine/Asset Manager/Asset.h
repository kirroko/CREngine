#pragma once

#include "PreCompile.h"
#include <filesystem>

enum ASSET_TYPE { Text, JSON, Sprite, Audio, Invalid };

namespace Ukemochi
{
	class Asset
	{
		std::string asset_key;
		std::string asset_path;
		ASSET_TYPE asset_type;

	public:
		Asset() = default;
		Asset(std::string file_path, std::string key);
		~Asset() = default;

		ASSET_TYPE GetType() const;
	};

	Asset::Asset(std::string file_path, std::string key) : asset_key(key), asset_path(file_path), asset_type(Invalid)
	{
		using namespace std;
		filesystem::path current = file_path;
		filesystem::path extention = current.extension();
		string temp = extention.generic_string();
		if (temp.compare(".txt") == 0)
		{
			asset_type = Text;
		}
		else if (temp.compare(".json") == 0)
		{
			asset_type = JSON;
		}
		else if (temp.compare(".png") == 0 || temp.compare(".jpg") == 0)
		{
			asset_type = Sprite;
		}
		else if (temp.compare(".wav") == 0)
		{
			asset_type = Audio;
		}
		else
		{
			asset_type = Invalid;
		}
	}

	ASSET_TYPE Asset::GetType() const
	{
		return asset_type;
	}
}