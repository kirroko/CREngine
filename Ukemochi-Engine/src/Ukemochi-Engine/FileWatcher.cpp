/* Start Header
*****************************************************************/
/*!
\file	FileWatcher.cpp
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
#include "PreCompile.h"
#include "FileWatcher.h"

#include "Logic/ProjectHandler.h"

namespace Ukemochi
{
    // TODO: Implement windows.h ReadDirectoryChangesW for windows method
    // ================== PUBLIC FUNCTIONS ==================
    FileWatcher::FileWatcher(const std::string& path_to_watch,
                             std::chrono::duration<int, std::milli> delay) : m_path_to_watch(path_to_watch),
                                                                             m_delay(delay)
    {
        ProjectHandler::GenerateSolutionAndProject("..\\Assets");
        for (auto& file : std::filesystem::recursive_directory_iterator(path_to_watch))
        {
            if (file.path().string().find("obj") != std::string::npos || file.path().string().find("Temp") !=
                std::string::npos || file.path().string().find(".vs") != std::string::npos)
                continue;

            // keep a record of files from the base directory and their last modification time
            m_Paths.insert(std::pair(file.path().string(), std::filesystem::last_write_time(file.path())));

            // Keep a record of scripts only, for scripting hot reloading
            if (file.path().extension() == ".cs")
            {
                std::string copyString = file.path().string();
                size_t pos = copyString.find("Scripts");
                copyString = copyString.substr(pos);
                m_ScriptPaths.push_back(copyString);
                // UME_ENGINE_TRACE("Found Script: {0}", copyString);
            }
        }

    }

    void FileWatcher::Start(const std::function<void(std::string, FileStatus)>& action)
    {
        UME_ENGINE_TRACE("Thread: Started at File Watcher");
        m_thread = std::thread([this,action]()
        {
            UME_ENGINE_TRACE("Thread: Running at File Watcher");

            while (m_running)
            {
                // wait for "m_delay" milliseconds
                std::this_thread::sleep_for(m_delay);
                // UME_ENGINE_TRACE("File Watcher: Checking for changes");

                // Update the m_paths to check for any file been deleted.
                auto it = m_Paths.begin();
                while (it != m_Paths.end())
                {
                    if (!std::filesystem::exists(it->first))
                    {
                        m_ScriptPaths.erase(std::remove(m_ScriptPaths.begin(), m_ScriptPaths.end(), it->first),
                                            m_ScriptPaths.end());
                        action(it->first, FileStatus::erased);
                        it = m_Paths.erase(it);
                        // and erase any script paths that are deleted
                    }
                    else
                        ++it;
                }

                // check if a file was created or modified
                for (auto& file : std::filesystem::recursive_directory_iterator(m_path_to_watch))
                {
                    // if(file.path().string().find("\\obj\\")) continue;
                    if (file.path().string().find("obj") != std::string::npos || file.path().string().find("Temp") !=
                        std::string::npos || file.path().string().find(".vs") != std::string::npos)
                        continue;

                    auto current_file_last_write_time = std::filesystem::last_write_time(file);

                    // File creation
                    if (!contains(file.path().string()))
                    {
                        m_Paths[file.path().string()] = current_file_last_write_time;
                        action(file.path().string(), FileStatus::created);
                    } // File modification
                    else
                    {
                        if (m_Paths[file.path().string()] != current_file_last_write_time)
                        {
                            m_Paths[file.path().string()] = current_file_last_write_time;
                            action(file.path().string(), FileStatus::modified);
                        }
                    }
                }
            }
        });

        m_thread.detach();
    }

    void FileWatcher::Stop()
    {
        m_running = false;
        if (m_thread.joinable()) // if running, block thread main thread until this thread is done
            m_thread.join();
        // UME_ENGINE_TRACE("File Watcher: Stopping thread");
    }

    // ================== PRIVATE FUNCTIONS ==================
    bool FileWatcher::contains(const std::string& key)
    {
        auto el = m_Paths.find(key);
        return el != m_Paths.end();
    }
}
