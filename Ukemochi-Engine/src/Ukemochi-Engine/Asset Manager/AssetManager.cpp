/* Start Header ************************************************************************/
/*!
\file       AssetManager.cpp
\author     Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu
\date       Nov 13, 2024
\brief      This file contains the definition of the Asset Manager.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "AssetManager.h"

namespace Ukemochi
{	
	void AssetManager::addTexture(std::string file_path, size_t& order_index)
	{
		if (texture_list.find(file_path) != texture_list.end())
		{
			// std::cout << "Texture already exists";
			UME_ENGINE_INFO("Texture {0} already exists in list", file_path);
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
		std::shared_ptr<Texture> texture(new Texture(file_path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0 + static_cast<int>(order_index), file_render, GL_UNSIGNED_BYTE));
		//textures.push_back(texture);
		//textures_enabled.push_back(true);
		std::string uniformName = "textures[" + std::to_string(order_index) + "]";
		texture->texUnit(shader_list.find("default")->second.get(), uniformName.c_str(), static_cast<GLuint>(order_index));

		texture_list[file_path] = texture;
		texture_order.push_back(file_path);
		order_index++;
	}

	std::shared_ptr<Texture> AssetManager::getTexture(std::string key_name)
	{
		return texture_list.find(key_name)->second;
	}

	void AssetManager::addShader(std::string file_name, std::string vert_path, std::string frag_path)
	{
		std::shared_ptr<Shader> shader(new Shader(vert_path.c_str(), frag_path.c_str()));

		shader_list[file_name] = shader;
	}

	std::shared_ptr<Shader> AssetManager::getShader(std::string key_name)
	{
		return shader_list.find(key_name)->second;
	}

}