/* Start Header ************************************************************************/
/*!
\file       AssetManager.cpp
\author     Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (50%)
\co-authors Wong Jun Yu Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (10%)
\co-authors TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu (25%)
\co-authors Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu (15%)
\date       Feb 04, 2025
\brief      This file contains the definition of the Asset Manager.

Copyright (C) 2025 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "AssetManager.h"
#include "../include/rapidjson/istreamwrapper.h"
#include "../include/rapidjson/document.h"

namespace Ukemochi
{
	/*!
	* @brief Constructs the AssetManager Class
	*/
	AssetManager::AssetManager() : texture_list_size(0), sound_count(0)
	{
		texture_order.clear();
		texture_list.clear();
		shader_list.clear();
		sound_list.clear();
		sound_name_list.clear();
	}

	/*!
	* @brief Deconstructs the AssetManager Class
	*/
	AssetManager::~AssetManager()
	{
		for (auto& ptr : texture_list)
		{
			ptr.second.reset();
			ptr.second.~shared_ptr();
		}

		for (auto& ptr : shader_list)
		{
			ptr.second.reset();
			ptr.second.~shared_ptr();
		}

		for (auto& ptr : sound_list)
		{
			ptr->release();
			ptr = nullptr;
		}

		texture_order.clear();
		texture_list.clear();
		shader_list.clear();
		sound_list.clear();
		sound_name_list.clear();
	}

	/*!
	* @brief Copies an asset file to the appropriate asset directory.
	* @param source_path The file path of the source asset.
	* @param asset_name The name of the asset file.
	* @param asset_type The type of asset (e.g., Texture, Shader, Sound).
	* @return True if the asset was copied successfully, false otherwise.
	*/
	bool AssetManager::copyAssetToDirectory(const std::string& source_path, const std::string& asset_name, const std::string& asset_type)
	{
		std::filesystem::path target_path = asset_dir;

		if (asset_type == "Texture")
			target_path /= "Textures";
		else if (asset_type == "Shader")
			target_path /= "Shaders";
		else if (asset_type == "Sound")
			target_path /= "Audio";

		// Create directory if it doesn't exist
		if (!std::filesystem::exists(target_path))
		{
			if (!std::filesystem::create_directory(target_path))
				return false;
		}

		target_path /= asset_name;

		// Check if the source file exists
		if (!std::filesystem::exists(source_path))
			return false;

		// Check if the target file already exists (for overwriting)
		if (std::filesystem::exists(target_path))
		{
			if (!std::filesystem::remove(target_path))
				return false;
		}

		// Perform the copy operation
		std::error_code ec; // to capture errors without throwing exceptions
		std::filesystem::copy(source_path, target_path, std::filesystem::copy_options::overwrite_existing, ec);

		if (ec)
			return false;

		UME_ENGINE_INFO("{0} added to {1} directory.", asset_name, asset_type);
		return true;
	}

	/*!
	* @brief Adds an asset of a specific type to the asset manager.
	* @tparam AssetType The type of the asset (e.g., Texture, Shader, Sound).
	* @param file_path The file path of the asset to be added.
	*/
	template<>
	void AssetManager::addAsset<Texture>(const std::string& file_path)
	{
		std::string asset_name = std::filesystem::path(file_path).filename().string();

		if (copyAssetToDirectory(file_path, asset_name, "Texture"))
		{
			// Define the target directory where the asset will be copied
			std::string target_directory = asset_dir + "/Textures";

			// Build the full target path
			std::string target_path = target_directory + "/" + asset_name;

			// Ensure the target directory exists (if it doesn't, create it)
			if (!std::filesystem::exists(target_directory))
				std::filesystem::create_directories(target_directory);

			addTexture(target_path);
		}
	}

	/*!
	* @brief Adds an asset of a specific type to the asset manager.
	* @tparam AssetType The type of the asset (e.g., Texture, Shader, Sound).
	* @param file_path The file path of the asset to be added.
	*/
	template<>
	void AssetManager::addAsset<Shader>(const std::string& file_path)
	{
		std::string asset_name = std::filesystem::path(file_path).filename().string();
		std::string vert_path = file_path + ".vert";
		std::string frag_path = file_path + ".frag";

		if (copyAssetToDirectory(file_path, asset_name, "Shader") &&
			copyAssetToDirectory(file_path, asset_name, "Shader"))
		{
			// Define the target directory where the asset will be copied
			std::string target_directory = asset_dir + "/Shaders";

			// Build target paths for vertex and fragment shaders
			std::string target_vert_path = target_directory + "/" + asset_name;
			std::string target_frag_path = target_directory + "/" + asset_name;

			// Ensure the target directory exists (if it doesn't, create it)
			if (!std::filesystem::exists(target_directory))
				std::filesystem::create_directories(target_directory);

			addShader(asset_name, target_vert_path, target_frag_path);
		}
	}

	/*!
	* @brief Adds an asset of a specific type to the asset manager.
	* @tparam AssetType The type of the asset (e.g., Texture, Shader, Sound).
	* @param file_path The file path of the asset to be added.
	*/
	template<>
	void AssetManager::addAsset<FMOD::Sound>(const std::string& file_path)
	{
		std::string asset_name = std::filesystem::path(file_path).filename().string();

		if (copyAssetToDirectory(file_path, asset_name, "Sound"))
		{
			// Define the target directory where the asset will be copied
			std::string target_directory = asset_dir + "/Audio";

			// Build the full target path for sound file
			std::string target_path = target_directory + "/" + asset_name;

			// Ensure the target directory exists (if it doesn't, create it)
			if (!std::filesystem::exists(target_directory))
				std::filesystem::create_directories(target_directory);

			addSound(target_path);
		}
	}

	/*!
	* @brief Removes an asset of a specific type from the asset manager.
	* @tparam AssetType The type of the asset (e.g., Texture, Shader, Sound).
	* @param file_path The file path of the asset to be removed.
	*/
	template<>
	void AssetManager::removeAsset<Texture>(const std::string& file_path)
	{
		removeTexture(file_path);
	}

	/*!
	* @brief Removes an asset of a specific type from the asset manager.
	* @tparam AssetType The type of the asset (e.g., Texture, Shader, Sound).
	* @param file_path The file path of the asset to be removed.
	*/
	template<>
	void AssetManager::removeAsset<Shader>(const std::string& file_path)
	{
		removeShader(file_path);
	}

	/*!
	* @brief Removes an asset of a specific type from the asset manager.
	* @tparam AssetType The type of the asset (e.g., Texture, Shader, Sound).
	* @param file_path The file path of the asset to be removed.
	*/
	template<>
	void AssetManager::removeAsset<FMOD::Sound>(const std::string& file_path)
	{
		removeSound(file_path);
	}

	/*!
	* @brief Creates Texture object and saves it in the container in the AssetManager
	* @param std::string file_path: file path of the texture
	*/
	void AssetManager::addTexture(std::string file_path)
	{
		// Skip if the texture is part of an atlas
		/*if (isTextureInAtlas(file_path))
		{
			UME_ENGINE_INFO("Skipping atlas subtexture: {0}", file_path);
			return;
		}*/

		if (texture_list.find(file_path) != texture_list.end())
		{
			// std::cout << "Texture already exists";
			UME_ENGINE_INFO("Texture {0} already exists in list", file_path);
			return;
		}
		else
		{
			//std::cout << "Adding texture: " << file_path << " /from addTexture() line 85" << std::endl;
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

		//std::cout << "Loaded texture: " << file_path << ", Texture ID: " << texture->ID << std::endl;
	}

	/*!
	* @brief Removes a texture from the asset manager.
	* @param file_path The file path of the texture to be removed.
	*/
	void AssetManager::removeTexture(const std::string& file_path)
	{
		auto it = texture_list.find(file_path);
		if (it != texture_list.end())
		{
			// Release texture memory
			it->second.reset();
			texture_list.erase(it);
			texture_order.erase(std::remove(texture_order.begin(), texture_order.end(), file_path), texture_order.end());
			texture_list_size--;

			// Physically delete the file
			if (std::filesystem::exists(file_path))
			{
				std::filesystem::remove(file_path);
				UME_ENGINE_INFO("Texture file {0} deleted from disk", file_path);
			}

			UME_ENGINE_INFO("Texture {0} removed successfully", file_path);
		}
		else
			UME_ENGINE_WARN("Texture {0} not found", file_path);
	}

	/*!
	* @brief Returns shared pointer to the Texture object desired
	* @param std::string key_name: name of desired file
	*/
	std::shared_ptr<Texture> AssetManager::getTexture(std::string key_name)
	{
		/*for (const auto& [key, value] : texture_list)
		{
			std::cout << "Texture Key: " << key << ", Texture ID: " << value->ID << std::endl;
		}*/

		auto it = texture_list.find(key_name);
		if (it != texture_list.end()) 
		{
			return it->second;
		}

		// Try matching by file name
		for (const auto& [path, texture] : texture_list) 
		{
			if (path.substr(path.find_last_of('/') + 1, path.find_last_of('.') - path.find_last_of('/') - 1) == key_name)
			{
				return texture;
			}
		}

		std::cerr << "Texture not found: " << key_name << std::endl;
		return nullptr;
	}

	/*!
	* @brief Creates Shader object and saves it in the container in the AssetManager
	* @param std::string file_name: name that encompasses the shader program
	* @param std::string vert_path: file path for the vertex shader
	* @param std::string frag_path: file path for the fragment shader
	*/
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

	/*!
	* @brief Removes a shader from the asset manager.
	* @param file_name The name of the shader to be removed.
	*/
	void AssetManager::removeShader(const std::string& file_name)
	{
		// Remove file extension and path from the file_name
		std::filesystem::path shaderPath(file_name);
		std::string baseFileName = shaderPath.stem().string();

		auto it = shader_list.find(baseFileName);
		if (it != shader_list.end())
		{
			it->second.reset();
			shader_list.erase(it);

			// Physically delete the shader file
			std::string vert_path = asset_dir + "/Shaders/" + baseFileName + ".vert";
			std::string frag_path = asset_dir + "/Shaders/" + baseFileName + ".frag";

			if (std::filesystem::exists(vert_path))
			{
				std::filesystem::remove(vert_path);
				UME_ENGINE_INFO("Vertex shader {0} deleted from disk", vert_path);
			}
			if (std::filesystem::exists(frag_path))
			{
				std::filesystem::remove(frag_path);
				UME_ENGINE_INFO("Fragment shader {0} deleted from disk", frag_path);
			}

			UME_ENGINE_INFO("Shader {0} removed successfully", file_name);
		}
		else
			UME_ENGINE_WARN("Shader {0} not found", file_name);
	}

	/*!
	* @brief Returns shared pointer to the Shader object desired
	* @param std::string key_name: name of desired Shader
	*/
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

	/*!
	* @brief Creates Sound object and saves it in the container in the AssetManager
	* @param std::string file_path: file path of the sound file
	*/
	void AssetManager::addSound(std::string file_path)
	{
		/*if (std::find(sound_name_list.begin(), sound_name_list.end(), file_path) != sound_name_list.end())
		{
			UME_ENGINE_INFO("Sound {0} already exists in list", file_path);
			return;
		}*/
		FMOD::Sound* sound{};
		FMOD::System* sys = ECS::GetInstance().GetSystem<Audio>()->pSystem;

		if (sys->createSound(file_path.c_str(), FMOD_DEFAULT, nullptr, &sound) != FMOD_OK)
		{
			std::cerr << "Failed to load sound" << std::endl;
			return;
		}

		sound_list.push_back(sound);
		sound_name_list.push_back(file_path);
		sound_count++;
		UME_ENGINE_INFO("Sound {0} added successfully", file_path);
	}

	/*!
	* @brief Removes a sound from the asset manager.
	* @param file_path The file path of the sound to be removed.
	*/
	void AssetManager::removeSound(const std::string& file_path)
	{
		// Find the sound in the sound_name_list
		auto it = std::find(sound_name_list.begin(), sound_name_list.end(), file_path);

		if (it != sound_name_list.end())
		{
			// Get the index of the sound to remove
			int key = static_cast<int>(it - sound_name_list.begin());

			// Release FMOD Sound memory
			sound_list[key]->release();
			sound_list.erase(sound_list.begin() + key);
			sound_name_list.erase(it);

			// Physically delete the sound file
			if (std::filesystem::exists(file_path))
			{
				std::filesystem::remove(file_path);
				UME_ENGINE_INFO("Sound file {0} deleted from disk", file_path);
			}

			UME_ENGINE_INFO("Sound {0} removed successfully", file_path);
		}
		else
			UME_ENGINE_WARN("Sound {0} not found", file_path);
	}

	/*!
	* @brief Returns shared pointer to Sound object desired
	* @param std::string key_name: name of desired Sound
	*/
	FMOD::Sound* AssetManager::getSound(int key)
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

	/*!
	* @brief Returns a boolean that represents if file name provided in params exists in the storage of the Asset Manager
	* @param std::string key_name: name of desired Texture
	*/
	bool AssetManager::ifTextureExists(std::string key_name)
	{
		return (texture_list.find(key_name) != texture_list.end());
	}

	/*!
	* @brief Returns size_t value representing the number of unique textures stored in the AssetManager
	*/
	size_t AssetManager::getTextureOrderSize()
	{
		return texture_order.size();
	}

	/*!
	* @brief Returns string value that is the file path of entry stored in texture_order
	* @param int index: index of entry
	*/
	std::string& AssetManager::getTextureAtIndex(int index)
	{
		return texture_order[index];
	}

	/*!
	* @brief Returns size_t value representing the number of textures stored in the AssetManager
	*/
	const size_t AssetManager::getTextureListSize() const
	{
		return texture_list_size;
	}

	/*!
	* @brief An overarching function that is called at the start to read through the Asset folder
	* and creates the respective objects based on the file extention
	*/
	void AssetManager::loadAssetsFromFolder()
	{
		for (auto const& dir : std::filesystem::recursive_directory_iterator(asset_dir))
		{
			if (dir.is_directory())
			{
				//if it is another directory ignore
				continue;
			}
			
			//file is a file
			const std::filesystem::path& to_load = dir.path();
			std::string visualize = to_load.generic_string();
			if (to_load.extension() == ".jpeg" || to_load.extension() == ".jpg" || to_load.extension() == ".png")
			{
				std::string file_path = to_load.generic_string();
				std::replace(file_path.begin(), file_path.end(), '\\', '/');
				if (isAtlasTexture(file_path))
				{
					// Derive sheetName from file_path
					std::string sheetName = file_path.substr(file_path.find_last_of('/') + 1);
					sheetName = sheetName.substr(0, sheetName.find_last_of('.')); // Remove extension

					// Handle atlas texture
					std::string json_path = file_path.substr(0, file_path.find_last_of('.')) + ".json";
					parseAtlasJSON(json_path, sheetName); 
				}
				else
				{
					// Handle regular texture
					addTexture(file_path);
				}
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
		UME_ENGINE_INFO("Assets completed loading");

	}

	// Update this to be more dynamic
	bool AssetManager::isAtlasTexture(const std::string& file_path)
	{
		// Check if the file path contains "_Part", indicating it's part of an atlas
		return file_path.find("_Part") != std::string::npos;
	}

	std::string AssetManager::getAtlasMetaData(const std::string& atlasPath)
	{
		std::filesystem::path jsonPath = atlasPath;
		jsonPath.replace_extension(".json");

		if (std::filesystem::exists(jsonPath))
		{
			return jsonPath.generic_string();
		}
		return ""; // Return empty string if no JSON metadata is found
	}
	
	void AssetManager::parseAtlasJSON(const std::string& jsonPath, const std::string& sheetName)
	{
		int atlasWidth = 0, atlasHeight = 0, channels = 0;
		// std::string atlasFilePath = "../Assets/Textures/Environment/" + sheetName + ".png";
		std::string atlasFilePath = jsonPath;
		size_t pos = atlasFilePath.find(".json");
		if (pos != std::string::npos)
			atlasFilePath.replace(pos, 5, ".png");
		
		stbi_uc* data = stbi_load(atlasFilePath.c_str(), &atlasWidth, &atlasHeight, &channels, 0);
		if (!data)
		{
			UME_ENGINE_ERROR("Failed to load atlas texture: {}", sheetName);
			return;
		}
		stbi_image_free(data); // Free texture data after getting dimensions

		std::ifstream file(jsonPath);
		if (!file.is_open())
		{
			UME_ENGINE_ERROR("Failed to open JSON file: {0}", jsonPath);
			return;
		}

		rapidjson::IStreamWrapper isw(file);
		rapidjson::Document document;
		document.ParseStream(isw);

		if (document.HasMember("frames"))
		{
			const auto& frames = document["frames"];
			for (auto it = frames.MemberBegin(); it != frames.MemberEnd(); it++)
			{
				// Get the sprite name and remove the file extension
				const std::string spriteName = it->name.GetString();
				std::string standardizedSpriteName = spriteName.substr(0, spriteName.find_last_of('.'));

				const auto& frame = it->value["frame"];
				//bool isRotated = it->value["rotated"].GetBool();
				
				// Extract UV coordinates
				UV uv;

				// Handle non-rotated sprite
				uv.uMin = static_cast<GLfloat>(frame["x"].GetInt()) / atlasWidth;
				uv.uMax = (static_cast<GLfloat>(frame["x"].GetInt()) + static_cast<GLfloat>(frame["w"].GetInt())) / atlasWidth;

				uv.vMax = 1.0f - (static_cast<GLfloat>(frame["y"].GetInt()) / atlasHeight);
				uv.vMin = 1.0f - ((static_cast<GLfloat>(frame["y"].GetInt()) + static_cast<GLfloat>(frame["h"].GetInt())) / atlasHeight);

				Vec2 dimension = Vec2(static_cast<float>(frame["w"].GetInt()), static_cast<float>(frame["h"].GetInt()));
				Vec2 position = Vec2(static_cast<float>(frame["x"].GetInt()),static_cast<float>(frame["y"].GetInt()));
				
				//std::cout << "Atlas Dimensions: " << atlasWidth << "x" << atlasHeight << std::endl;
				//std::cout << "Sprite: " << spriteName
				//	<< ", x: " << frame["x"].GetInt()
				//	<< ", y: " << frame["y"].GetInt()
				//	<< ", w: " << frame["w"].GetInt()
				//	<< ", h: " << frame["h"].GetInt() << std::endl;

				//std::cout << "UV Coordinates for " << spriteName << ": "
				//	<< "uMin=" << uv.uMin << ", vMin=" << uv.vMin
				//	<< ", uMax=" << uv.uMax << ", vMax=" << uv.vMax << std::endl;

				// Store UV
				spriteData[standardizedSpriteName] = { uv, sheetName, dimension, position };
			}
		}

	// Add the atlas texture to the system
	addTexture(atlasFilePath);
}

	std::vector<std::string> AssetManager::getAtlasJSONData(
		const std::string& jsonPath)
	{
		std::string convertedExtension = jsonPath;
		size_t pos = convertedExtension.find(".png");
		if (pos != std::string::npos)
			convertedExtension.replace(pos, 5, ".json");
		
		std::ifstream file(convertedExtension);
		if (!file.is_open())
		{
			UME_ENGINE_ERROR("File cannot be open! {0}", convertedExtension);
			return std::vector<std::string>{}; // return empty
		}

		rapidjson::IStreamWrapper isw(file);
		rapidjson::Document doc;
		doc.ParseStream(isw);

		std::vector<std::string> payload;

		if (doc.HasMember("frames"))
		{
			const auto& frames = doc["frames"];
			for (auto it = frames.MemberBegin(); it != frames.MemberEnd(); it++)
			{
				const std::string spriteName = it->name.GetString();
				std::string standardSpriteName = spriteName.substr(0, spriteName.find_last_of('.'));

				payload.push_back(standardSpriteName);
			}
		}

		return payload;
	}


	void AssetManager::loadSpriteSheet(const std::string& sheetName, const std::string& atlasPath)
	{
		spriteSheets[sheetName] = std::make_unique<Texture>(atlasPath.c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
	}

	void AssetManager::bindSpriteSheet(const std::string& sheetName) 
	{
		if (spriteSheets.find(sheetName) != spriteSheets.end()) 
		{
			spriteSheets[sheetName]->Bind();
		}
	}

	void AssetManager::debugPrintSpriteData() const
	{
		std::cout << "SpriteData contains the following keys:" << std::endl;
		for (const auto& [key, value] : spriteData) {
			std::cout << key << std::endl;
		}
	}

	const AssetManager::SpriteInfo& AssetManager::getSpriteData(const std::string& spriteName)
	{
		auto it = spriteData.find(spriteName);
		if (it == spriteData.end()) {
			std::cerr << "Error: Sprite '" << spriteName << "' not found in spriteData!" << std::endl;
			throw std::out_of_range("Sprite name not found: " + spriteName);
		}
		return it->second;
	}

	bool AssetManager::isTextureInAtlas(const std::string& texturePath) const
	{
		// Extract the file name without the extension
		std::string fileName = texturePath.substr(texturePath.find_last_of('/') + 1);
		fileName = fileName.substr(0, fileName.find_last_of('.')); // Remove the extension

		// Check if the fileName exists as a key in spriteData
		return spriteData.find(fileName) != spriteData.end();
	}
}
