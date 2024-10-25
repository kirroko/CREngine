#pragma once

#include "PreCompile.h"
#include "Asset.h"

namespace Ukemochi
{
	class AssetManager
	{
		static bool AddAsset(std::string file_path);

		static bool InitAssetsFromFolder(std::string folder_path);

	};

	std::vector<Asset> asset_list;
}