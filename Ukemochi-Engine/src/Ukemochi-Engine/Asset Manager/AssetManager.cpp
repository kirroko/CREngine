#include "AssetManager.h"

namespace Ukemochi
{
	
	void AssetManager::addTexture(std::string file_name, std::string file_path)
	{
		if (texture_list.find(file_name) != texture_list.end())
		{
			std::cout << "Texture already exists";
			return;
		}

		
	}

	Texture& AssetManager::getTexture(std::string key_name)
	{
		return *texture_list.find(key_name)->second;
	}

	
}