/* Start Header ************************************************************************/
/*!
\file       AssetManager.h
\author     Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (50%)
\co-authors Wong Jun Yu Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (10%)
\co-authors TAN Shun Zhi Tomy, t.shunzhitomy, 2301341, t.shunzhitomy@digipen.edu (25%)
\co-authors Lum Ko Sand, kosand.lum, 2301263, kosand.lum\@digipen.edu (15%)
\date       Feb 04, 2025
\brief      This file contains the declaration of the Asset Manager.

Copyright (C) 2025 DigiPen Institute of Technology.
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
		//storage of file paths for sounds in the order they are registered
		std::vector<std::string> sound_name_list;
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
			Vec2 spriteSheetDimension{};
			Vec2 spriteSheetPosition{};
		};
		
		std::string getAtlasMetaData(const std::string& atlasPath);
	public:
		std::unordered_map<std::string, SpriteInfo> spriteData;
		bool isAtlasTexture(const std::string& file_path);
		
		void parseAtlasJSON(const std::string& jsonPath, const std::string& sheetName);

		std::vector<std::string> getAtlasJSONData(const std::string& jsonPath);

		void loadSpriteSheet(const std::string& sheetName, const std::string& atlasPath);

		void debugPrintSpriteData() const;

		const SpriteInfo& getSpriteData(const std::string& spriteName);

		bool isTextureInAtlas(const std::string& texturePath) const;

		void bindSpriteSheet(const std::string& sheetName);

		std::string getAtlasForSubtexture(const std::string& subtextureName) const;
		/*!
		* @brief Constructs the AssetManager Class
		*/
		AssetManager();

		/*!
		* @brief Deconstructs the AssetManager Class
		*/
		~AssetManager();

		/*!
		* @brief Copies an asset file to the appropriate asset directory.
		* @param source_path The file path of the source asset.
		* @param asset_name The name of the asset file.
		* @param asset_type The type of asset (e.g., Texture, Shader, Sound).
		* @return True if the asset was copied successfully, false otherwise.
		*/
		bool copyAssetToDirectory(const std::string& source_path, const std::string& asset_name, const std::string& asset_type);

		/*!
		* @brief Adds an asset of a specific type to the asset manager.
		* @tparam AssetType The type of the asset (e.g., Texture, Shader, Sound).
		* @param file_path The file path of the asset to be added.
		*/
		template<typename AssetType>
		void addAsset(const std::string& file_path);

		/*!
		* @brief Removes an asset of a specific type from the asset manager.
		* @tparam AssetType The type of the asset (e.g., Texture, Shader, Sound).
		* @param file_path The file path of the asset to be removed.
		*/
		template<typename AssetType>
		void removeAsset(const std::string& file_path);

		/*!
		* @brief Creates Texture object and saves it in the container in the AssetManager
		* @param std::string file_path: file path of the texture
		*/
		void addTexture(std::string file_path);

		/*!
		* @brief Removes a texture from the asset manager.
		* @param file_path The file path of the texture to be removed.
		*/
		void removeTexture(const std::string& file_path);

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
		* @brief Removes a shader from the asset manager.
		* @param file_name The name of the shader to be removed.
		*/
		void removeShader(const std::string& file_name);

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
		* @brief Removes a sound from the asset manager.
		* @param file_path The file path of the sound to be removed.
		*/
		void removeSound(const std::string& file_path);

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
			static std::unique_ptr<AssetManager> manager(new AssetManager());
			return *manager;
		}
	};
}