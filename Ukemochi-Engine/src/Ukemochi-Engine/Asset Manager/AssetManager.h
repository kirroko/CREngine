#pragma once

#include <filesystem>
#include "PreCompile.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/Sprite.h"
#include "../Graphics/Texture.h"
#include "../ECS/ECS.h"


namespace Ukemochi
{
	class AssetManager : public Ukemochi::System
	{
	private:
		std::map<std::string, std::shared_ptr<Sprite>> sprite_list;
		std::map<std::string, std::shared_ptr<Texture>> texture_list;
		std::map<std::string, std::shared_ptr<Shader>> shader_list;
		//std::map<std::string, Audio> audio_list;

	public:
		void addSprite(std::string file_name, std::string file_path /* can add more if need any other arguements */);
		Sprite& getSprite(std::string key_name);

		void addTexture(std::string file_name, std::string file_path /* can add more if need any other arguements */);
		Texture& getTexture(std::string key_name);

		/*void addShader(std::string file_name, std::string frag_path, std::string);
		Shader& getShader(std::string key_name);*/

		//bool addAudio(std::string file_name, std::string file_path /* can add more if need any other arguements */);
		//Audio getAudio(std::string kwey_name);

		//void loadAssetsFromFolder(std::string folder_path);
	};

	
}