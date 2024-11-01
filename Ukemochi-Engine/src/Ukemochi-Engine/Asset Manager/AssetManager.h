#pragma once

#include <filesystem>
#include "PreCompile.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Sprite.h"
#include "../Graphics/shaderClass.h"
#include "../Graphics/Texture.h"
#include "../Audio/Audio.h"


namespace Ukemochi
{
	class AssetManager
	{
	private:
		std::map<std::string, Sprite> sprite_list;
		std::map<std::string, Texture> texture_list;
		std::map<std::string, Shader> shader_list;
		std::map<std::string, Audio> audio_list;

	public:
		bool addSprite(std::string file_name, std::string file_path /* can add more if need any other arguements */);
		Sprite getSprite(std::string key_name);

		bool addTexture(std::string file_name, std::string file_path /* can add more if need any other arguements */);
		Texture getTexture(std::string key_name);

		bool addShader(std::string file_name, std::string file_path /* can add more if need any other arguements */);
		Shader getShader(std::string key_name);

		bool addAudio(std::string file_name, std::string file_path /* can add more if need any other arguements */);
		Audio getAudio(std::string kwey_name);

		void loadAssetsFromFolder(std::string folder_path);
	};

	
}