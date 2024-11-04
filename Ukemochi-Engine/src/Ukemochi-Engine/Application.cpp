/* Start Header ************************************************************************/
/*!
\file       Application.cpp
\par		Ukemochi
\author 	HURNG Kai Rui, h.kairui, 230xxxx, h.kairui\@digipen.edu (50%)
\co-authors x, x, 230xxxx, x\@digipen.edu (x%)
			Wong Jun Yu Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (x%)
			Pek Jun Kai Gerald, p.junkaigerald, 2301334, p.junkaigerald\@digipen.edu (5%)
			x, x, 230xxxx, x\@digipen.edu (x%)
\par        Course: CSD2400/CSD2401
\date   	25-09-2024
\brief      This file contains the function definitions of application.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "PreCompile.h"
#include "Application.h"
#include "Game/GSM.h"
#include "Ukemochi-Engine/Logs/Log.h"
#include "FrameController.h"

// System Includes
#include "Input/Input.h" // for input system
#include "Physics/Physics.h"	   // for physics system
#include "Collision/Collision.h"   // for collision system
#include "Graphics/Renderer.h"     // for graphics system
#include "Serialization/Serialization.h" //load json
#include "ECS/ECS.h"
#include <iomanip>

//#include <glad/glad.h>
#include "Audio/Audio.h"
#include "ImGui/ImGuiCore.h"
#include "SceneManager.h"
#include "Logic/Scripting.h"
#include "FileWatcher.h"

#include <crtdbg.h>				// To check for memory leaks


#include <crtdbg.h>				// To check for memory leaks

namespace Ukemochi
{
#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
    Application* Application::s_Instance = nullptr;

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
            win_title = "DEFAULT";
            win_height = 900;
            win_width = 1600;
        }
        WindowProps props(win_title, win_width, win_height); // You can customize these properties if needed
        m_Window = std::make_unique<WindowsWindow>(props);
        m_Window->SetEventCallback(BIND_EVENT_FN(Application::EventIsOn));

        //GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
        //imguiInstance.ImGuiInit(glfwWindow);

        auto fileWatcher = std::make_shared<FileWatcher>("..\\Assets", std::chrono::milliseconds(3000));
        fileWatcher->Start([fileWatcher](const std::string& path_to_watch, FileStatus status)
        {
            // Logging levels:
            // TRACE (Capture execution of code)
            // DEBUG (Capture relevant detail of event)
            // INFO (Capture an event that occurred)
            // WARN (Indicate unexpected event, disrupt or delay)
            // ERROR (Capture a system interfering with functionalities)
            // FATAL (Capture a system crash)

            // std::filesystem::path path(path_to_watch);
            // path.filename();
            switch (status)
            {
            case Ukemochi::FileStatus::created:
                UME_ENGINE_INFO("File created: {0}", path_to_watch);
            // std::filesystem::path filePath(path_to_watch);
            // if(filePath.extension() == ".cs")
            // {
            //     // recompile clinet assembly
            //     ScriptingEngine::GetInstance().CompileScriptAssembly();
            //     ScriptingEngine::GetInstance().Reload();
            // }
                break;
            case Ukemochi::FileStatus::modified:
                UME_ENGINE_INFO("File modified: {0}", path_to_watch);
                break;
            case Ukemochi::FileStatus::erased:
                UME_ENGINE_INFO("File deleted: {0}", path_to_watch);
                break;
            }
        });
        // ProjectHandler::GenerateSolutionAndProject("..\\Assets");
        fwInstance = fileWatcher; // Keep a reference to the file watch instance
    }

    Application::~Application()
    {
        imguiInstance.ImGuiClean();
        m_running = false;

        // Ensure the thread is joined before exiting to prevent memory leaks
        if (fwInstance)
            fwInstance->Stop();

        ScriptingEngine::GetInstance().ShutDown();
    }

    void Application::EventIsOn(Event& e)
    {
        //imguiInstance.OnEvent(e);
        EventDispatcher dispatch(e);
        dispatch.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::IsWindowClose));
        if (std::string(e.GetName()) != "MouseMoved") // NO SPAM MOUSE MOVED EVENT
            UME_ENGINE_TRACE("{0}", e.ToString());
    }

    bool Application::IsWindowClose(WindowCloseEvent& e)
    {
        (void)e; // Suppress the unused parameter warning
        gsm_current = GS_STATES::GS_QUIT;
        m_running = false;
        return true;
    }

    void Application::GameLoop()
    {
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

        double accumulator = 0.0;
        int currentNumberOfSteps = 0;
        double lastFPSDisplayTime = 0.0; // To track when we last displayed the FPS
        double fpsDisplayInterval = 1.0; // Display the FPS every 1 second

        //Set up SceneManager
        SceneManager sceneManger;
        GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(m_Window->GetNativeWindow());
        imguiInstance.ImGuiInit(glfwWindow);
        //while engine running
        while (gsm_current != GS_STATES::GS_QUIT && m_running)
        {
            if (Input::IsKeyPressed(GLFW_KEY_1))
            {
                gsm_next = GS_LEVEL1;
                gsm_previous = gsm_current = gsm_next;
                // If 'W' key is pressed, move forward


                UME_ENGINE_INFO("1 key is pressed");
            }

            //ENGINE STATES
            if (gsm_current == GS_ENGINE && gsm_current == gsm_next)
            {
                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT);

                //************ FPS ************
                g_FrameRateController.Update();

                //************ FPS ************
                currentNumberOfSteps = 0;
                // Use deltaTime from the FrameRateController
                double deltaTime = g_FrameRateController.GetDeltaTime();
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

                //************ Render IMGUI ************
                imguiInstance.NewFrame();
                //imguiInstance.ShowEntityManagementUI();
                imguiInstance.LoadScene();
                imguiInstance.Begin();
                imguiInstance.ImGuiUpdate(); // Render ImGui elements
                //************ Render IMGUI ************

                m_Window->OnUpdate();
            }
            else if (gsm_current != GS_ENGINE && gsm_current == gsm_next) //in game state
            {
                //current state != restart
                if (gsm_current != GS_STATES::GS_RESTART)
                {
                    sceneManger.LoadScene();
                }
                else
                {
                    gsm_next = gsm_current = gsm_previous;
                }

                //Init Scene
                sceneManger.InitScene();

                while (gsm_current == gsm_next && m_running)
                {
                    glClearColor(0, 0, 0, 1);
                    glClear(GL_COLOR_BUFFER_BIT);

                    //************ FPS ************
                    g_FrameRateController.Update();

                    currentNumberOfSteps = 0;
                    // Use deltaTime from the FrameRateController
                    double deltaTime = g_FrameRateController.GetDeltaTime();
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

                    if (Input::IsKeyPressed(GLFW_KEY_2))
                    {
                        gsm_next = GS_ENGINE;
                        //gsm_previous = gsm_current = gsm_next;

                        UME_ENGINE_INFO("2 key is pressed");
                    }

                    //************ Update & Draw ************
                    sceneManger.Update(deltaTime);
                    //************ Update & Draw ************

                    //************ Render IMGUI ************
                    imguiInstance.NewFrame();
                    imguiInstance.ShowEntityManagementUI();
                    imguiInstance.Begin();
                    imguiInstance.ImGuiUpdate(); // Render ImGui elements
                    //************ Render IMGUI ************

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

                    //Update window
                    m_Window->OnUpdate();
                }
                //Free scene
                gsm_fpFree();
                //If game not restart unload Scene
                if (gsm_next != GS_STATES::GS_RESTART)
                {
                    sceneManger.ClearScene();
                }

                gsm_previous = gsm_current = gsm_next;
            }
        }
    }
}
