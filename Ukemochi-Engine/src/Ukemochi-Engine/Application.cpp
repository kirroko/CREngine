/* Start Header ************************************************************************/
/*!
\file       Application.cpp
\author     HURNG Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (50%)
\co-authors TAN Si Han, t.sihan , 2301264, t.sihan\@digipen.edu (30%)
\co-authors Wong Jun Yu Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (15%)
\co-authors Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (5%)
\date       Sept 25, 2024
\brief      This file contains the function definitions of application.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "Application.h"
#include "Game/GSM.h"
#include "Ukemochi-Engine/Logs/Log.h"
#include "FrameController.h"

// System Includes
#include "Input/Input.h"				 // for input system
#include "Physics/Physics.h"			 // for physics system
#include "Collision/Collision.h"		 // for collision system
#include "Graphics/Renderer.h"			 // for graphics system
#include "Serialization/Serialization.h" //load json
#include "ECS/ECS.h"
#include <iomanip>

// #include <glad/glad.h>
#include "Audio/Audio.h"
#include "ImGui/ImGuiCore.h"
#include "SceneManager.h"
#include "Logic/Scripting.h"
#include "FileWatcher.h"
#include "Logic/Logic.h"
#include "Game/DungeonManager.h"
#include "Game/EnemyManager.h"
#include "Factory/GameObjectManager.h"
#include <crtdbg.h> // To check for memory leaks

namespace Ukemochi
{
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
    Application* Application::s_Instance = nullptr;

    double accumulator = 0.0;
    int currentNumberOfSteps = 0;
    double lastFPSDisplayTime = 0.0; // To track when we last displayed the FPS
    double fpsDisplayInterval = 1.0; // Display the FPS every 1 second
    double deltaTime = 0.0;

    void EnableMemoryLeakChecking(int breakAlloc = -1)
    {
        int tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
        _CrtSetDbgFlag(tmpDbgFlag);
        if (breakAlloc != -1)
            _CrtSetBreakAlloc(breakAlloc);
    }

    Application::Application()
    {
        s_Instance = this;

        unsigned int win_width, win_height;
        std::string win_title;
        rapidjson::Document config;
        bool success = Serialization::LoadJSON("../Assets/config.json", config);
        if (success)
        {
            const rapidjson::Value& window = config["Window"];
            win_title = window["Title"].GetString();
            win_height = window["Height"].GetUint();
            win_width = window["Width"].GetUint();
        }
        else
        {
            win_title = "Ukemochi";
            win_width = 1920;
            win_height = 1080;
        }
        WindowProps props(win_title, win_width, win_height); // You can customize these properties if needed
        m_Window = std::make_unique<WindowsWindow>(props);
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::EventIsOn));

        SceneManager::GetInstance();

        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
        imguiInstance.ImGuiInit(glfwWindow);

        fwInstance = std::make_shared<FileWatcher>("..\\Assets", std::chrono::milliseconds(3000));

        fwInstance->Start([](const std::string& path_to_watch, FileStatus status)
        {
            switch (status)
            {
                case FileStatus::created:
                {
                    UME_ENGINE_INFO("File created: {0}", path_to_watch);
                    std::filesystem::path filePath(path_to_watch);
                    if (filePath.extension() == ".cs")
                    {
                        ScriptingEngine::GetInstance().compile_flag = true;
                        UME_ENGINE_INFO("Compile flag is open");
                    }
                    break;
                }
                case FileStatus::modified:
                {
                    UME_ENGINE_INFO("File modified: {0}", path_to_watch);
                    std::filesystem::path filePath(path_to_watch);
                    if (filePath.extension() == ".cs")
                    {
                        ScriptingEngine::GetInstance().compile_flag = true;
                        UME_ENGINE_INFO("Compile flag is open");
                    }
                    break;
                }
                case FileStatus::erased:
                {
                    UME_ENGINE_INFO("File deleted: {0}", path_to_watch);
                    std::filesystem::path filePath(path_to_watch);
                    if (filePath.extension() == ".cs")
                    {
                        ScriptingEngine::GetInstance().compile_flag = true;
                        UME_ENGINE_INFO("Compile flag is open");
                    }
                    break;
                }
            }
        });
        //fwInstance->Stop();
    }

    Application::~Application()
    {
        UME_ENGINE_TRACE("Shutting down Application...");
        imguiInstance.ImGuiClean();
        m_running = false;

        // Ensure the thread is joined before exiting to prevent memory leaks
        if (fwInstance)
        {
            fwInstance->Stop(); // Hypothetical method to stop monitoring
            fwInstance.reset(); // Reset shared pointer
        }
        ScriptingEngine::GetInstance().ShutDown();
    }

    void Application::EventIsOn(Event& e)
    {
        // imguiInstance.OnEvent(e);
        EventDispatcher dispatch(e);
        dispatch.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::IsWindowClose));
        std::string eventName(e.GetName());
        if (eventName != "MouseMoved" && eventName != "KeyPressed" && eventName != "KeyTyped" && eventName != "KeyReleased") // NO SPAM MOUSE MOVED EVENT
            UME_ENGINE_TRACE("{0}", e.ToString());
    }

    bool Application::IsWindowClose(WindowCloseEvent& e)
    {
        (void)e; // Suppress the unused parameter warning
        es_current = ENGINE_STATES::ES_QUIT;
        m_running = false;
        return true;
    }

    void Application::StartGame()
    {
        auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
        audioM.PlayMusic(audioM.GetMusicindex("BGM"));
        // enemy
        ECS::GetInstance().GetSystem<EnemyManager>()->UpdateEnemyList();
        // Recompile scripts if needed
        if (ScriptingEngine::GetInstance().compile_flag)
        {
            UME_ENGINE_INFO("Begin Script reloading");
            ScriptingEngine::GetInstance().compile_flag = false;
            ScriptingEngine::GetInstance().Reload();
        }

        // Only start if there are no script errors
        if (!ScriptingEngine::ScriptHasError)
        {
            // Save the current scene state
            //SceneManager::GetInstance().SaveScene(SceneManager::GetInstance().GetCurrScene());
            //m_CompileError = false;
            es_current = ENGINE_STATES::ES_PLAY;
            UME_ENGINE_INFO("Simulation (Game is playing) started");

            // Initialize ECS systems for game mode
            ECS::GetInstance().GetSystem<LogicSystem>()->Init();
        }
        else
        {
            //m_CompileError = true;
        }
    }

    void Application::StopGame()
    {
        // Load the saved scene state to reset to the editor mode
        SceneManager::GetInstance().LoadSaveFile(SceneManager::GetInstance().GetCurrScene() + ".json");

        // Switch back to editor mode
        es_current = ENGINE_STATES::ES_ENGINE;
        UME_ENGINE_INFO("Simulation (Game is stopping) stopped");
    }

    void Application::GameLoop() // run
    {
        // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
        EnableMemoryLeakChecking();

        SceneManager sceneManager = SceneManager::GetInstance();
        sceneManager.SceneMangerInit();
        sceneManager.SceneMangerLoad();

        while (es_current != ENGINE_STATES::ES_QUIT)
        {
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            UpdateFPS();

#ifndef _DEBUG
            StartGame();
            //if (Input::IsKeyPressed(UME_KEY_L))
            //    StartGame();
            //else if (Input::IsKeyPressed(UME_KEY_K))
            //    StopGame();
#endif // !_DEBUG

            // engine
            if (es_current == ENGINE_STATES::ES_ENGINE)
            {
                if (sceneManager.GetOnIMGUI() == false)
                {
                    sceneManager.SceneMangerUpdateCamera(deltaTime);
                }
                //************ Update & Draw ************
                sceneManager.SceneMangerUpdate();
                //************ Update & Draw ************
            }
            // play
            else if (es_current == ENGINE_STATES::ES_PLAY)
            {
                //Run all the systems using ECS
                //************ Update & Draw ************
                sceneManager.SceneMangerRunSystems();
                //************ Update & Draw ************
            }

#ifdef _DEBUG
            UpdateIMGUI();
#endif // _DEBUG

            DrawFPS();

            m_Window->OnUpdate();
        }
        UME_ENGINE_TRACE("Unloading Scene...");
        sceneManager.SceneManagerUnload();
    }

    void Application::UpdateFPS()
    {
        //************ FPS ************
        g_FrameRateController.Update();

        currentNumberOfSteps = 0;
        // Use deltaTime from the FrameRateController
        deltaTime = g_FrameRateController.GetDeltaTime();
        accumulator += deltaTime;

        // Run game logic at a fixed time step (e.g., 60 times per second)
        while (accumulator >= g_FrameRateController.GetFixedDeltaTime())
        {
            // Update game logic with a fixed delta time
            accumulator -= g_FrameRateController.GetFixedDeltaTime();
            currentNumberOfSteps++;
        }
        g_FrameRateController.SetCurrentNumberOfSteps(currentNumberOfSteps);
        //************ FPS ************
    }

    void Application::DrawFPS()
    {
        //************ Display FPS ************
        double currentTime = glfwGetTime();
        // Only log/display the FPS every second (or defined interval)
        if (currentTime - lastFPSDisplayTime >= fpsDisplayInterval)
        {
            double fps = g_FrameRateController.GetFPS();

            // Use std::ostringstream to format the FPS with 2 decimal places
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << fps;
            std::string fpsString = oss.str();

            // Log or display the FPS
            UME_ENGINE_INFO("FPS: {0}", fpsString);

            // Update the last time we displayed the FPS
            lastFPSDisplayTime = currentTime;
        }
        //************ Display FPS ************
    }

    void Application::UpdateIMGUI()
    {
        //************ Render IMGUI ************
        imguiInstance.NewFrame();
        imguiInstance.SpriteEditorWindow();
        imguiInstance.DebugWindow();
        imguiInstance.SceneBrowser();
        //imguiInstance.AssetBrowser();
        imguiInstance.SceneRender();
        imguiInstance.ShowEntityManagementUI();
        // imguiInstance.Begin();
        //imguiInstance.RenderGizmo2d();
        imguiInstance.ImGuiUpdate(); // Render ImGui elements
        //************ Render IMGUI ************
    }
}
