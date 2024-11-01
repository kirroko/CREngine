#include "AssetManager.h"

namespace Ukemochi
{

	/*void AssetManager::AddAsset(std::string file_path)
	{
		size_t start = file_path.find_last_of('\\');
		size_t end = file_path.find_first_of('.');
		size_t len = end - start;
		std::string key = file_path.substr(start, len);
		Asset created(file_path, key);
		if (created.GetType() == Invalid)
		{
			return;
		}
		asset_list.emplace(key, created);
		return;
	}*/

	void AssetManager::loadAssetsFromFolder(std::string folder_path)
	{
		for (const auto& entries : std::filesystem::directory_iterator(folder_path))
		{
			if (!entries.is_directory()) //entries is a file
			{
				//check extention and add asset accordingly
			}
			else //entries is a folder
			{
				loadAssetsFromFolder(entries.path().generic_string()); //recur function with the subfolder;
				continue;
			}
		}
		return;
	}
	
}