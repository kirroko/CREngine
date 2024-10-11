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

	bool AssetManager::LoadAssetsFromFolder(std::string folder_path)
	{
		for (const auto& entries : std::filesystem::directory_iterator(folder_path))
		{
			if (!entries.is_directory())
			{
				AddAsset(entries.path().generic_string());
			}
		}
	}
	
}