/* Start Header ************************************************************************/
/*!
\file       AssetManager.h
\author     Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu
\date       Nov 13, 2024
\brief      This file contains the declaration of the Asset Manager.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once

#include <filesystem>
#include "PreCompile.h"
#include "../Graphics/Renderer.h"
#include "../ECS/Components.h"
#include "../Graphics/Texture.h"
#include "../ECS/ECS.h"


namespace Ukemochi
{
	class AssetManager : public Ukemochi::System
	{
	private:
		//std::map<std::string, std::shared_ptr<SpriteRender>> sprite_list;
		//std::map<std::string, std::shared_ptr<FMOD::Sound>> audio_list;

		//size of texture list
		size_t texture_list_size;
		//number of sounds
		int sound_count;
		//storage of file paths for textures in the order they are registered
		std::vector<std::string> texture_order;
		//storage of pointers to texture objects
		std::unordered_map<std::string, std::shared_ptr<Texture>> texture_list;
		//storage of pointers to shader objects
		std::unordered_map<std::string, std::shared_ptr<Shader>> shader_list;
		//storage of pointers to sound objects
		std::vector<FMOD::Sound*> sound_list;
		////storage of file paths for sounds in the order they are registered
		//std::vector<std::string> sound_name_list;
		//string storing the directory of the assets folder
		std::string asset_dir{ "../Assets" };

		struct UV {
			GLfloat uMin, vMin;
			GLfloat uMax, vMax;
		};

		GLuint atlasTextureID;
		std::unordered_map<std::string, std::unique_ptr<Texture>> spriteSheets;

		struct SpriteInfo {
			UV uv;
			std::string spriteSheetName;
		};
		std::unordered_map<std::string, SpriteInfo> spriteData;

	public:
		void parseAtlasJSON(const std::string& jsonPath, int atlasWidth, int atlasHeight, const std::string& sheetName);

		void loadSpriteSheet(const std::string& sheetName, const std::string& atlasPath);

		const SpriteInfo& getSpriteData(const std::string& spriteName) 
		{
			return spriteData.at(spriteName);
		}


		void bindSpriteSheet(const std::string& sheetName);

		/*!
		* @brief Constructs the AssetManager Class
		*/
		AssetManager();

		/*!
		* @brief Deconstructs the AssetManager Class
		*/
		~AssetManager();

		/*!
		* @brief Creates Texture object and saves it in the container in the AssetManager
		* @param std::string file_path: file path of the texture
		*/
		void addTexture(std::string file_path);
		/*!
		* @brief Returns shared pointer to the Texture object desired
		* @param std::string key_name: name of desired file
		*/
		std::shared_ptr<Texture> getTexture(std::string key_name);

		/*!
		* @brief Creates Shader object and saves it in the container in the AssetManager
		* @param std::string file_name: name that encompasses the shader program
		* @param std::string vert_path: file path for the vertex shader
		* @param std::string frag_path: file path for the fragment shader
		*/
		void addShader(std::string file_name, std::string vert_path, std::string frag_path);
		/*!
		* @brief Returns shared pointer to the Shader object desired
		* @param std::string key_name: name of desired Shader
		*/
		std::shared_ptr<Shader> getShader(std::string key_name);

		/*!
		* @brief Creates Sound object and saves it in the container in the AssetManager
		* @param std::string file_path: file path of the sound file
		*/
		void addSound(std::string file_path);
		/*!
		* @brief Returns pointer to Sound object desired
		* @param std::string key_name: name of desired Sound
		*/
		FMOD::Sound* getSound(int key);

		/*!
		* @brief Returns a boolean that represents if file name provided in params exists in the storage of the Asset Manager
		* @param std::string key_name: name of desired Texture
		*/
		bool ifTextureExists(std::string key_name);

		/*!
		* @brief Returns size_t value representing the number of unique textures stored in the AssetManager
		*/
		size_t getTextureOrderSize();

		/*!
		* @brief Returns string value that is the file path of entry stored in texture_order
		* @param int index: index of entry
		*/
		const size_t getTextureListSize() const;

		/*!
		* @brief Returns size_t value representing the number of textures stored in the AssetManager
		*/
		std::string& getTextureAtIndex(int index);

		/*!
		* @brief An overarching function that is called at the start to read through the Asset folder
		* and creates the respective objects based on the file extention
		*/
		void loadAssetsFromFolder();

		/*!
		* @brief Initialises ASM
		*/
		static AssetManager& initASM()
		{
			static std::unique_ptr<AssetManager>  manager(new AssetManager());
			return *manager;
		}
	};


}