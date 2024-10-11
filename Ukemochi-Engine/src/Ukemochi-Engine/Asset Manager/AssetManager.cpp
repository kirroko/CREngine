#include "AssetManager.h"

namespace Ukemochi
{

	bool AssetManager::AddAsset(std::string file_path)
	{
		size_t index = asset_list.size();
		Asset created(file_path, index);
		if (created.GetType() == Invalid)
		{
			return false;
		}
		asset_list.push_back(created);
		return true;
	}

	bool AssetManager::LoadAssetsFromFolder
	
}