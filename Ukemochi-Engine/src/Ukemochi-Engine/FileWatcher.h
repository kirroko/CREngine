/* Start Header ************************************************************************/
/*!
\file       FileWatcher.h
\author     WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu
\date       Oct 24, 2024
\brief      This files implements file watcher for monitoring files for changes.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "PreCompile.h"
#ifdef _WIN32
#include <Windows.h>
#endif


namespace Ukemochi
{
	enum class FileStatus {
		created,
		modified,
		erased
	};

	class FileWatcher 
	{
		std::unordered_map<std::string, std::filesystem::file_time_type> m_Paths{};
		std::vector<std::string> m_ScriptPaths{};
		
		std::string m_path_to_watch;
		
		std::thread m_thread;
		
		std::chrono::duration<int,std::milli> m_delay;
		std::atomic_bool m_running = true;

		bool contains(const std::string& key);
	public:
		std::atomic_bool m_isDone = false;
		
		FileWatcher(const std::string& path_to_watch, std::chrono::duration<int, std::milli> delay);

		/**
		 * @brief Monitor "m_path_to_watch" for changes and in case of a change execute the user supplied
		 * @param action 
		 */
		void Start(const std::function<void(std::string, FileStatus)>& action);

		/**
		 * @brief Get the Script Paths
		 * @return std::vector<std::string>&  
		 */
		std::vector<std::string>& GetScriptPaths() { return m_ScriptPaths; }
		
		/**
		 * @brief Stop the thread
		 */
		void Stop();
	};
}