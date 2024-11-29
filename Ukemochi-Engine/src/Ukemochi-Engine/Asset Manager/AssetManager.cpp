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
	AssetManager::AssetManager() : texture_list_size(0), sound_count(0)
	{
		texture_order.clear();
		texture_list.clear();
		shader_list.clear();
		sound_list.clear();
		sound_name_list.clear();
	}

	AssetManager::~AssetManager()
	{
		//need to clear properly
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
		std::shared_ptr<Texture> texture(new Texture(file_path.c_str(), GL_TEXTURE_2D, GL_TEXTURE0 + static_cast<int>(texture_list_size), file_render, GL_UNSIGNED_BYTE));
		//textures.push_back(texture);
		//textures_enabled.push_back(true);
		std::string uniformName = "textures[" + std::to_string(texture_list_size) + "]";
		texture->texUnit(shader_list.find("default")->second.get(), uniformName.c_str(), static_cast<GLuint>(texture_list_size));

		//texture_list[file_path] = texture;
		texture_list.emplace(std::make_pair(file_path, texture));
		texture_order.push_back(file_path);
		texture_list_size++;
		UME_ENGINE_INFO("Texture {0} added successfully", file_path);
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
		if (shader_list.find(file_name) != shader_list.end())
		{
			UME_ENGINE_INFO("Shader {0} already exists in list", file_name);
			return;
		}

		std::shared_ptr<Shader> shader(new Shader(vert_path.c_str(), frag_path.c_str()));

		//shader_list[file_name] = shader;
		shader_list.emplace(std::make_pair(file_name, shader));
		UME_ENGINE_INFO("shader {0} added successfully", file_name);
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
		if (std::find(sound_name_list.begin(), sound_name_list.end(), file_path) != sound_name_list.end())
		{
			UME_ENGINE_INFO("Sound {0} already exists in list", file_path);
			return;
		}
		FMOD::Sound* sound{};
		FMOD::System* sys = ECS::GetInstance().GetSystem<Audio>()->accessSys();

		if (sys->createSound(file_path.c_str(), FMOD_DEFAULT, nullptr, &sound) != FMOD_OK)
		{
			std::cerr << "Failed to load sound" << std::endl;
			return;
		}

		sound_list.push_back(std::shared_ptr<FMOD::Sound>(sound));
		sound_name_list.push_back(file_path);
		sound_count++;
		UME_ENGINE_INFO("Sound {0} added successfully", file_path);
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

	bool AssetManager::ifTextureExists(std::string key_name)
	{
		return (texture_list.find(key_name) != texture_list.end());
	}

	size_t AssetManager::getTextureOrderSize()
	{
		return texture_order.size();
	}

	std::string& AssetManager::getTextureAtIndex(int index)
	{
		return texture_order[index];
	}

	const size_t AssetManager::getTextureListSize() const
	{
		return texture_list_size;
	}

	void AssetManager::loadAssetsFromFolder()
	{
		for (auto const& dir : std::filesystem::recursive_directory_iterator(asset_dir))
		{
			if (dir.is_directory())
			{
				//if it is another directory ignore
				continue;
			}
			else
			{
				//file is a file
				std::filesystem::path to_load = dir.path();
				std::string visualize = to_load.generic_string();
				if (to_load.extension() == ".jpeg" || to_load.extension() == ".jpg" || to_load.extension() == ".png")
				{
					std::string file_path = to_load.generic_string();
					std::replace(file_path.begin(), file_path.end(), '\\', '/');
					addTexture(file_path);
				}
				else if (to_load.extension() == ".mp3" || to_load.extension() == ".wav")
				{
					std::string file_path = to_load.generic_string();
					std::replace(file_path.begin(), file_path.end(), '\\', '/');
					addSound(file_path);
				}
				else if (to_load.extension() == ".vert" || to_load.extension() == ".frag")
				{
					std::string holder = to_load.filename().generic_string();
					std::string file_name = holder.substr(0, holder.find_first_of("."));

					if (to_load.extension() == ".vert")
					{
						std::string vertex_shader = to_load.generic_string();
						std::replace(vertex_shader.begin(), vertex_shader.end(), '\\', '/');
						std::string frag_shader{};
						for (auto const& match_shader : std::filesystem::recursive_directory_iterator(asset_dir))
						{
							std::filesystem::path checker = match_shader.path();
							std::string holder2 = checker.filename().generic_string();
							std::string check_name = holder2.substr(0, holder.find_first_of("."));

							check_name;
							if (checker.extension() == ".frag" && file_name.compare(check_name) == 0)
							{
								frag_shader = checker.generic_string();
								std::replace(frag_shader.begin(), frag_shader.end(), '\\', '/');
								break;
							}
						}
						addShader(file_name, vertex_shader, frag_shader);
					}
					else
					{
						std::string frag_shader = to_load.generic_string();
						std::replace(frag_shader.begin(), frag_shader.end(), '\\', '/');
						std::string vertex_shader{};

						for (auto const& match_shader : std::filesystem::recursive_directory_iterator(asset_dir))
						{
							std::filesystem::path checker = match_shader.path();
							std::string holder2 = checker.filename().generic_string();
							std::string check_name = holder2.substr(0, holder.find_first_of("."));

							check_name;
							if (checker.extension() == ".vert" && file_name.compare(check_name) == 0)
							{
								vertex_shader = checker.generic_string();
								std::replace(vertex_shader.begin(), vertex_shader.end(), '\\', '/');
								break;
							}
						}
						addShader(file_name, vertex_shader, frag_shader);
					}
				}
			}
		}
		UME_ENGINE_INFO("Assets completed loading");
	}

}