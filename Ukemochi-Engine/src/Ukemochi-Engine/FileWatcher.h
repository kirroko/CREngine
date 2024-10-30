/* Start Header
*****************************************************************/
/*!
\file	FileWatcher.h
\par	Ukemochi
\author WONG JUN YU, Kean, junyukean.wong, 2301234
\par	junyukean.wong\@digipen.edu
\par	Course: CSD2400/CSD2401
\date	24/10/24
\brief	This files implements file watcher for monitoring files for changes

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
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
		std::string m_path_to_watch;

		std::atomic_bool m_running = true;
		
		std::chrono::duration<int,std::milli> m_delay;
		std::thread m_thread;

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
		 * @brief Stop the thread
		 */
		void Stop();
	};
}