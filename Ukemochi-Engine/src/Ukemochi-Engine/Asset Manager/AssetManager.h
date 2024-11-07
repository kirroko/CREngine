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
		std::map<std::string, std::shared_ptr<Texture>> texture_list;
		std::map<std::string, std::shared_ptr<Shader>> shader_list;
		//std::map<std::string, std::shared_ptr<FMOD::Sound>> audio_list;

	public:
		//void addSprite(std::string file_name, std::string file_path /* can add more if need any other arguements */);
		//SpriteRender& getSprite(std::string key_name);

		void addTexture(std::string file_name, std::string file_path , std::string shader_name);
		Texture& getTexture(std::string key_name);

		void addShader(std::string file_name, std::string vert_path, std::string frag_path);
		Shader& getShader(std::string key_name);

		

	};

	
}