#pragma once

#include "PreCompile.h"
#include "Asset.h"

namespace Ukemochi
{
	class AssetManager
	{
		static void AddAsset(std::string file_path);

		static void InitAssetsFromFolder(std::string folder_path);

		static Asset GetAsset();
	};

	std::map<std::string, Asset> asset_list;
}