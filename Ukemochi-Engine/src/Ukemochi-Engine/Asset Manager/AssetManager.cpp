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
		
		std::string extention = file_path.substr(file_path.find_last_of('.') + 1);
		std::transform(extention.begin(), extention.end(), extention.begin(), ::tolower);

		GLenum file_render;
		if (extention.compare("png") == 0)\
		{
			file_render = GL_RGBA;
		}
		else if (extention.compare("jpeg") == 0 || extention.compare("jpg") == 0)
		{
			file_render = GL_RGB;
		}

	}

	Texture& AssetManager::getTexture(std::string key_name)
	{
		return *texture_list.find(key_name)->second;
	}

	
}