/* Start Header ************************************************************************/
/*!
\file       AssetManager.cpp
\author     Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu
\date       Nov 13, 2024
\brief      This file contains the implementation of the Asset Manager.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "AssetManager.h"

namespace Ukemochi
{	
	AssetManager::AssetManager() : texture_index(0), sound_count(0)
	{
		texture_order.clear();
		texture_list.clear();
		shader_list.clear();
	}

	AssetManager::~AssetManager()
	{

	}

	void AssetManager::addTexture(std::string file_path)
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
		if (extention.compare("png") == 0)
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
		std::shared_ptr<Texture> texture(new Texture(file_path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0 + static_cast<int>(texture_index), file_render, GL_UNSIGNED_BYTE));
		//textures.push_back(texture);
		//textures_enabled.push_back(true);
		std::string uniformName = "textures[" + std::to_string(texture_index) + "]";
		texture->texUnit(shader_list.find("default")->second.get(), uniformName.c_str(), static_cast<GLuint>(texture_index));

		texture_list[file_path] = texture;
		texture_order.push_back(file_path);
		texture_index++;
	}

	std::shared_ptr<Texture> AssetManager::getTexture(std::string key_name)
	{
		if (texture_list.find(key_name) != texture_list.end())
		{
			return texture_list.find(key_name)->second;
		}
		else
		{
			return nullptr;
		}
	}

	void AssetManager::addShader(std::string file_name, std::string vert_path, std::string frag_path)
	{
		std::shared_ptr<Shader> shader(new Shader(vert_path.c_str(), frag_path.c_str()));

		shader_list[file_name] = shader;
	}

	std::shared_ptr<Shader> AssetManager::getShader(std::string key_name)
	{
		if (shader_list.find(key_name) != shader_list.end())
		{
			return shader_list.find(key_name)->second;
		}
		else
		{
			return nullptr;
		}
	}

	void AssetManager::addSound(std::string file_path)
	{
		FMOD::Sound* sound{};
		FMOD::System* sys = ECS::GetInstance().GetSystem<Audio>()->accessSys();

		if (sys->createSound(file_path.c_str(), FMOD_DEFAULT, nullptr, &sound) != FMOD_OK)
		{
			std::cerr << "Failed to load sound" << std::endl;
			return;
		}

		sound_list.push_back(std::make_shared<FMOD::Sound>(sound));
		sound_count++;
	}

	std::shared_ptr<FMOD::Sound> AssetManager::getSound(int key)
	{
		if (key > (sound_count - 1))
		{
			return nullptr;
		}
		else
		{
			return sound_list[key];
		}
	}

	void AssetManager::loadAssetsFromFolder(std::string directory)
	{
		std::filesystem::directory_entry dir_to_watch{ directory };
		

	}

}