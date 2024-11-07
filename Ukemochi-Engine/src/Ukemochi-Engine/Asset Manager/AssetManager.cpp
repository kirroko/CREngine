#include "AssetManager.h"

namespace Ukemochi
{
	
	void AssetManager::addTexture(std::string file_name, std::string file_path, std::string shader_name)
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
		else
		{
			file_render = GL_RGB;
		}

		// Load and store the texture with the determined format
		std::shared_ptr<Texture> texture(new Texture(file_path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, file_render, GL_UNSIGNED_BYTE));
		//textures.push_back(texture);
		//textures_enabled.push_back(true);
		texture->texUnit(*(shader_list.find(shader_name)->second), "tex0", 0);

		texture_list[file_name] = texture;
	}

	Texture& AssetManager::getTexture(std::string key_name)
	{
		return *texture_list.find(key_name)->second;
	}

	void AssetManager::addShader(std::string file_name, std::string vert_path, std::string frag_path)
	{
		std::shared_ptr<Shader> shader(new Shader(vert_path.c_str(), frag_path.c_str()));

		shader_list[file_name] = shader;
	}

	Shader& AssetManager::getShader(std::string key_name)
	{
		return *shader_list.find(key_name)->second;
	}
}