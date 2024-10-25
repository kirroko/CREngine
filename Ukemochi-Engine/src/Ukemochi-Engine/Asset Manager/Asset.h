#pragma once

#include "PreCompile.h"
#include <filesystem>

enum ASSET_TYPE { Text, JSON, Sprite, Audio, Invalid };

namespace Ukemochi
{
	class Asset
	{
		size_t m_asset_id;
		std::string m_path;
		ASSET_TYPE m_type;

	public:
		Asset() = default;
		Asset(std::string file_path, size_t id);
		~Asset() = default;

		ASSET_TYPE GetType() const;
	};

	Asset::Asset(std::string file_path, size_t id) : m_asset_id(id), m_path(file_path), m_type(Invalid)
	{
		using namespace std;
		filesystem::path current = file_path;
		filesystem::path extention = current.extension();
		string temp = extention.generic_string();
		if (temp.compare(".txt") == 0)
		{
			m_type = Text;
		}
		else if (temp.compare(".json") == 0)
		{
			m_type = JSON;
		}
		else if (temp.compare(".png") == 0 || temp.compare(".jpg") == 0)
		{
			m_type = Sprite;
		}
		else if (temp.compare(".wav") == 0)
		{
			m_type = Audio;
		}
		else
		{
			m_type = Invalid;
		}
	}

	ASSET_TYPE Asset::GetType() const
	{
		return m_type;
	}
}