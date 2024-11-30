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
	public:
		//void addSprite(std::string file_name, std::string file_path /* can add more if need any other arguements */);
		//SpriteRender& getSprite(std::string key_name);

		void addTexture(std::string filePath, size_t& orderIndex);
		std::shared_ptr<Texture> getTexture(std::string key_name);

		void addShader(std::string file_name, std::string vert_path, std::string frag_path);
		std::shared_ptr<Shader> getShader(std::string key_name);

		size_t order_index{};
		std::vector<std::string> texture_order;
		std::map<std::string, std::shared_ptr<Texture>> texture_list;
		std::map<std::string, std::shared_ptr<Shader>> shader_list;

	};

	
}