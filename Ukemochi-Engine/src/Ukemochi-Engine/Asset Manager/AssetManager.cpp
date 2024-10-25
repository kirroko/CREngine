#include "AssetManager.h"

namespace Ukemochi
{

	void AssetManager::AddAsset(std::string file_path)
	{
		size_t index = asset_list.size();
		Asset created(file_path, index);
		if (created.GetType() == Invalid)
		{
			return;
		}
		asset_list.push_back(created);
		return;
	}

	void AssetManager::InitAssetsFromFolder(std::string folder_path)
	{
		for (const auto& entries : std::filesystem::directory_iterator(folder_path))
		{
			if (!entries.is_directory()) //entries is a file
			{
				AddAsset(entries.path().generic_string());
			}
			else //entries is a folder
			{
				InitAssetsFromFolder(entries.path().generic_string()); //recur function
				continue;
			}
		}
		return;
	}
	
}