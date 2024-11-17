/* Start Header ************************************************************************/
/*!
\file       ImGuiCore.cpp
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (%)
\co-authors Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu (%)
\date       Sept 25, 2024
\brief      This file contains the implementation of the UseImGui class,
            which manages the initialization, rendering, and event handling 
            for ImGui within the engine.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "PreCompile.h"
#include "ImGuiCore.h"
#include <filesystem>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include "../Application.h"

#include "../Logic/Logic.h"
#include "../Factory/Factory.h"
#include "../ECS/ECS.h"

#include "../Graphics/Renderer.h"
#include "../SceneManager.h"
#include "../Math/Transformation.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"

namespace Ukemochi
{
    float UseImGui::m_Time = 0.0f; //!< Time since last frame for calculating DeltaTime.
    float UseImGui::m_LastAssetUpdateTime = 0.0f;
    float UseImGui::m_LastSceneUpdateTime = 0.0f;
    std::vector<std::string> UseImGui::assetFiles;
    std::vector<std::string> UseImGui::textureFiles;
    std::vector<std::string> UseImGui::sceneFiles;
    std::vector<std::string> UseImGui::folderNames;
    bool UseImGui::m_CompileError = false;
    bool UseImGui::m_Compiling = false;
    bool UseImGui::m_SpriteFlag = false;
    std::string UseImGui::m_SpritePath;
    int UseImGui::m_global_selected = 0;

    /*!
    \brief Initializes the ImGui context and sets up OpenGL.
    \param window Pointer to the GLFW window (unused in this implementation).
    */
    void UseImGui::ImGuiInit(GLFWwindow* /*window*/)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;

        io.FontGlobalScale = 1.5f; // Increase font size by 50% (adjust as needed)

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        //ImGui::LoadIniSettingsFromDisk("imgui_layout.ini");

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.Alpha = 1.0f; // Set the overall alpha of the UI
        style.WindowRounding = 5.0f; // Round corners of windows
        style.FrameRounding = 3.0f; // Round corners of frames
        style.ItemSpacing = ImVec2(10, 10); // Spacing between items
        style.WindowBorderSize = 0.0f; // No border for windows
        style.WindowPadding = ImVec2(0.0f, -1.5f); // Padding for windows

        //ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 5.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application& app = Application::Get();
        GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
        //ImGui::SaveIniSettingsToDisk("imgui_layout.ini");
    }

    /*!
    \brief Prepares a new ImGui frame and handles window dimensions and timing.
    */
    void UseImGui::NewFrame()
    {
        // ImGui frame setup
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Update DeltaTime
        float currentTime = static_cast<float>(glfwGetTime()); // Get the current time in seconds
        float deltaTime = currentTime - m_Time; // Calculate time since last frame
        m_Time = currentTime; // Update the time for the next frame

        float fps = 1.0f / deltaTime;


        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()),
                                static_cast<float>(app.GetWindow().GetHeight()));

        ImGuiViewport* viewport = ImGui::GetMainViewport();

        // Toggle to enable or disable docking
        static bool enableDocking = true;

        //if (io.KeyCtrl && Input::IsKeyTriggered(UME_KEY_D))
        //{
        //    enableDocking = !enableDocking;
        //}
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Enable Docking", NULL, &enableDocking);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        // Set up the window to cover the entire viewport
        ImVec2 window_pos = viewport->Pos;
        window_pos.y += ImGui::GetFrameHeightWithSpacing() - 10.0f;
        ImGui::SetNextWindowPos(window_pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        // Configure window flags for a fullscreen dockspace
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

        // Begin the main dockspace window
        ImGui::Begin("DockSpace Demo", nullptr, window_flags);

        // Set up the dockspace only if docking is enabled
        if (enableDocking)
        {
            ImGuiID dockspace_id = ImGui::GetID("DockSpace");
            ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
            ImGui::DockSpace(dockspace_id, ImVec2(0, 0), dockspace_flags);
        }

        ControlPanel(fps);


        ImGui::End(); // End the dockspace window

        if (m_CompileError)
        {
            ImGui::OpenPopup("Error");
            m_CompileError = false;
        }
        ShowErrorPopup("A compile error has occurred. Please check the console for more information.");

        //ImGui::SaveIniSettingsToDisk("imgui_layout.ini");
    }

    //void UseImGui::CheckAndHandleFileDrop()
    //{
    //    Application& app = Application::Get();
    //    WindowsWindow& win = (WindowsWindow&)app.GetWindow();

    //    if (win.fileDropped) {
    //        std::cout << "File drop detected!" << std::endl;
    //        const std::string& filePath = win.GetFilePath();
    //        std::cout << "File dropped: " << filePath << std::endl;

    //        ImGui::Begin("File Drop Window");
    //        ImGui::Text("Dropped File Path: %s", filePath.c_str());
    //        win.ClearFilePath();
    //        win.fileDropped = false;  // Reset flag
    //        ImGui::End();
    //    }
    //}

    void UseImGui::SpriteEditorWindow()
    {
        ImGui::Begin("Sprite Editor");
        ImGui::Text("Sprite Editor Contents");

        // Static variables
        static GLuint texture = 0;
        static char clipName[128] = "";
        static int textureWidth = 0;
        static int textureHeight = 0;
        static int totalFrames = 1;
        static int pixelSize[2] = {64, 64};
        static float frameTime = 0.05f;
        static bool looping = true;

        // Editor variables
        static bool showTools = false;
        static bool showGrid = false;
        static std::string fileName;

        if (m_SpriteFlag)
        {
            // Set up the texture details
            if (ECS::GetInstance().GetSystem<AssetManager>()->texture_list.find(m_SpritePath) !=
                ECS::GetInstance().GetSystem<AssetManager>()->texture_list.end())
            {
                std::filesystem::path filePath;
                showGrid = true;
                texture = ECS::GetInstance().GetSystem<AssetManager>()->texture_list[m_SpritePath]->ID;
                textureWidth = ECS::GetInstance().GetSystem<AssetManager>()->texture_list[m_SpritePath]->width;
                textureHeight = ECS::GetInstance().GetSystem<AssetManager>()->texture_list[m_SpritePath]->height;
                filePath = m_SpritePath;
                fileName = filePath.stem().string();
                filePath.replace_extension(".json");
                rapidjson::Document document;
                if (Serialization::LoadJSON(filePath.string(), document))
                {
                    const rapidjson::Value& object = document["TextureMeta"];
                    if (object.HasMember("KeyPath") && object.HasMember("ClipName") && object.HasMember("TotalFrames") &&
                        object.HasMember("PixelWidth") && object.HasMember("PixelHeight") &&
                        object.HasMember("FrameTime") && object.HasMember("Looping"))
                    {
                        // std::copy(temp.begin(), temp.end(), clipName);
                        // std::string temp = object["KeyPath"].GetString();
                        std::string temp = object["ClipName"].GetString();
                        std::copy(temp.begin(),temp.end(), clipName);
                        totalFrames = object["TotalFrames"].GetInt();
                        pixelSize[0] = object["PixelWidth"].GetInt();
                        pixelSize[1] = object["PixelHeight"].GetInt();
                        frameTime = object["FrameTime"].GetFloat();
                        looping = object["Looping"].GetBool();
                    }
                    else
                    {
                        UME_ENGINE_ERROR("Missing required fields in TextureMeta");
                    }
                }
                else
                {
                    UME_ENGINE_WARN("No metadata found");
                    std::fill(std::begin(clipName), std::end(clipName), 0);
                    totalFrames = 1;
                    pixelSize[0] = 64;
                    pixelSize[1] = 64;
                    frameTime = 0.05f;
                    looping = true;
                    m_SpriteFlag = false;
                    showTools = false;
                    showGrid = false;
                }
                m_SpriteFlag = false;
                showTools = true;
            }
            else
            {
                UME_ENGINE_WARN("No texture found in AssetManager for path, was it loaded? : {0}", m_SpritePath);
                fileName.clear();
                textureWidth = 0;
                textureHeight = 0;
                texture = 0;
                std::fill(std::begin(clipName), std::end(clipName), 0);
                totalFrames = 1;
                pixelSize[0] = 64;
                pixelSize[1] = 64;
                frameTime = 0.05f;
                looping = true;
                m_SpriteFlag = false;
                showTools = false;
                showGrid = false;
            }
        }

        if (!showTools)
        {
            ImGui::End();
            return;
        }

        ImGui::Separator();

        // Display the texture details
        ImGui::Text("File = %s", fileName.c_str());
        ImGui::Text("Size = %d x %d", textureWidth, textureHeight);
        ImGui::Checkbox("Show Grid", &showGrid);

        // Input for name of clip
        ImGui::InputText("Clip Name", clipName, IM_ARRAYSIZE(clipName));

        // Input for total frames
        ImGui::InputInt("Total Frame", &totalFrames);
        totalFrames = std::max(1, totalFrames);

        // Input for pixel size
        ImGui::InputInt2("Pixel Size", pixelSize);
        pixelSize[0] = std::max(1, pixelSize[0]);
        pixelSize[1] = std::max(1, pixelSize[1]);

        // Input for frame time
        ImGui::InputFloat("Frame Time", &frameTime, 0, 0, "%.2f");
        frameTime = std::max(0.01f, frameTime); // max 0.01s

        // Input for looping
        ImGui::Checkbox("Looping", &looping);

        if (ImGui::Button("Export Clip"))
        {
            // Get file name to save
            rapidjson::Document document;
            document.SetObject();
            rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

            rapidjson::Value textureMetaData(rapidjson::kObjectType);
            std::string tempStr = clipName;
            textureMetaData.AddMember("KeyPath", rapidjson::Value(m_SpritePath.c_str(), allocator), allocator);
            textureMetaData.AddMember("ClipName", rapidjson::Value(tempStr.c_str(), allocator), allocator);
            textureMetaData.AddMember("TotalFrames", totalFrames, allocator);
            textureMetaData.AddMember("PixelWidth", pixelSize[0], allocator);
            textureMetaData.AddMember("PixelHeight", pixelSize[1], allocator);
            textureMetaData.AddMember("TextureWidth", textureWidth, allocator);
            textureMetaData.AddMember("TextureHeight", textureHeight, allocator);
            textureMetaData.AddMember("FrameTime", frameTime, allocator);
            textureMetaData.AddMember("Looping", looping, allocator);

            document.AddMember("TextureMeta", textureMetaData, allocator);
            // Serialize the texture
            std::string path = "../Assets/Textures/" + fileName + ".json";
            if (!Serialization::PushJSON(path, document))
            {
                UME_ENGINE_ERROR("Failed to save metadata to file: {0}", path);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Add Clip to GO"))
        {
            auto GOs = GameObjectManager::GetInstance().GetAllGOs();
            if (GOs[m_global_selected]->HasComponent<Animation>())
            {
                auto& anim = GOs[m_global_selected]->GetComponent<Animation>();
                anim.clips[clipName] = AnimationClip{
                    m_SpritePath, clipName, texture, totalFrames, pixelSize[0], pixelSize[1], textureWidth, textureHeight, frameTime,
                    looping
                };
                anim.SetAnimation(clipName);
            }
            // Read the metadata from the file
            // rapidjson::Document document;
            // if (!Serialization::LoadJSON(filePath.string(), document))
            // {
            //     UME_ENGINE_ERROR("Failed to load metadata from file: {0}", filePath.string());
            //     return;
            // }
            //
            // const rapidjson::Value& object = document["TextureMeta"];
            //
            // auto GOs = GameObjectManager::GetInstance().GetAllGOs();
            // if (GOs[m_global_selected]->HasComponent<Animation>())
            // {
            //     auto& anim = GOs[m_global_selected]->GetComponent<Animation>();
            //     GLuint textureID = ECS::GetInstance().GetSystem<AssetManager>()->texture_list[m_SpritePath]->ID;
            //     std::string name = object["ClipName"].GetString();
            //     anim.clips[name] = AnimationClip{
            //         name, textureID, object["TotalFrames"].GetInt(),
            //         object["PixelWidth"].GetInt(), object["PixelHeight"].GetInt(),
            //         object["TextureWidth"].GetInt(), object["TextureHeight"].GetInt(),
            //         object["FrameTime"].GetFloat(), object["Looping"].GetBool()
            //     };
            //     anim.SetAnimation(name);
            // }
            // else
            // {
            //     // Else popup saying selected GO does not have animation component
            //     UME_ENGINE_WARN("Selected GO does not have Animation component");
            // }
        }

        // Display Image
        // Calculate the aspect ratio of the image
        float aspectRatio = static_cast<float>(textureWidth) / static_cast<float>(textureHeight);
        ImVec2 availSize = ImGui::GetContentRegionAvail();

        // determine the final size to display the image
        float displayWidth = static_cast<float>(textureWidth);
        float displayHeight = static_cast<float>(textureHeight);

        if (static_cast<float>(textureWidth) > availSize.x || static_cast<float>(textureHeight) > availSize.y)
        {
            if (availSize.x / aspectRatio <= availSize.y)
            {
                displayWidth = availSize.x;
                displayHeight = availSize.x / aspectRatio;
            }
            else
            {
                displayWidth = availSize.y * aspectRatio;
                displayHeight = availSize.y;
            }
        }

        float xOffset = (availSize.x - displayWidth) * 0.5f;
        float yOffset = (availSize.y - displayHeight) * 0.5f;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + xOffset);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + yOffset);
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();

        ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(displayWidth, displayHeight), ImVec2(0, 1), ImVec2(1, 0));

        // Draw the grid
        if (showGrid)
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();

            // scale the grid to match the resized image
            float scaledCellWidth = static_cast<float>(pixelSize[0]) / static_cast<float>(textureWidth) * displayWidth;
            float scaledCellHeight = static_cast<float>(pixelSize[1]) / static_cast<float>(textureHeight) *
                displayHeight;

            int colums = textureWidth / pixelSize[0];
            //            int rows = textureHeight / pixelSize[1];
            int maxRows = (totalFrames + colums - 1) / colums;

            for (int i = 0; i < totalFrames; i++)
            {
                int col = i % colums;
                int row = i / colums;
                if (row >= maxRows) break;

                drawList->AddRect(ImVec2(canvasPos.x + static_cast<float>(col) * scaledCellWidth, canvasPos.y +
                                         static_cast<float>(row) * scaledCellHeight),
                                  ImVec2(canvasPos.x + static_cast<float>(col + 1) * scaledCellWidth, canvasPos.y +
                                         static_cast<float>(row + 1) * scaledCellHeight),
                                  IM_COL32(255, 255, 255, 255));
            }

            // for(float x = 0; x <= displayWidth; x += scaledCellWidth)
            // {
            //     drawList->AddLine(
            //         ImVec2(canvasPos.x + x, canvasPos.y),
            //         ImVec2(canvasPos.x + x, canvasPos.y + displayHeight),
            //         IM_COL32(255,255,255,255));
            // }
            //
            // for(float y = 0; y <= displayHeight; y += scaledCellHeight)
            // {
            //     drawList->AddLine(
            //         ImVec2(canvasPos.x, canvasPos.y + y),
            //         ImVec2(canvasPos.x + displayWidth, canvasPos.y + y),
            //         IM_COL32(255,255,255,255));
            // }
        }
        ImGui::End();
    }

    /**
     * @brief Displays the debug window in the ImGui interface, showing the time taken by various systems.
     */
    void UseImGui::DebugWindow()
    {
        static double lastUpdateTime = 0.0;
        static double cachedCollisionTime = 0.0;
        static double cachedPhysicsTime = 0.0;
        static double cachedGraphicsTime = 0.0;
        static double cachedLogicTime = 0.0;
        static double cachedCollisionPercent = 0.0;
        static double cachedPhysicsPercent = 0.0;
        static double cachedGraphicsPercent = 0.0;
        static double cachedLogicPercent = 0.0;
        static std::string cacheMessage = "WAITING FOR SIMULATION";

        double currentTime = ImGui::GetTime();

        std::chrono::duration<double> loop = SceneManager::loop_time;
        std::chrono::duration<double> collision = SceneManager::collision_time;
        std::chrono::duration<double> physics = SceneManager::physics_time;
        std::chrono::duration<double> graphics = SceneManager::graphics_time;
        std::chrono::duration<double> logic = SceneManager::logic_time;

        ImGui::Begin("Debug Window");
        if (currentTime - lastUpdateTime >= 1.0)
        {
            lastUpdateTime = currentTime;

            cachedCollisionTime = SceneManager::collision_time.count() * 1000.f;
            cachedPhysicsTime = SceneManager::physics_time.count() * 1000.f;
            cachedGraphicsTime = SceneManager::graphics_time.count() * 1000.f;
            cachedLogicTime = SceneManager::logic_time.count() * 1000.f;

            cachedCollisionPercent = collision.count() / loop.count() * 100.f;
            cachedPhysicsPercent = physics.count() / loop.count() * 100.f;
            cachedGraphicsPercent = graphics.count() / loop.count() * 100.f;
            cachedLogicPercent = logic.count() / loop.count() * 100.f;
        }
        if (es_current == ES_PLAY)
            ImGui::Text("SIMULATION RUNNING");
        else
            ImGui::Text("WAITING FOR SIMULATION");

        ImGui::Text("Collision Time: %.2f ms (%.2f%%)", cachedCollisionTime, cachedCollisionPercent);
        ImGui::Text("Physics Time: %.2f ms (%.2f%%)", cachedPhysicsTime, cachedPhysicsPercent);
        ImGui::Text("Graphics Time: %.2f ms (%.2f%%)", cachedGraphicsTime, cachedGraphicsPercent);
        ImGui::Text("Logic Time: %.2f ms (%.2f%%)", cachedLogicTime, cachedLogicPercent);
        ImGui::End();
    }

    /**
 * @brief Displays the control panel in the ImGui interface, showing FPS and various control options.
 *
 * This function creates a control panel window in ImGui where information such as FPS is displayed,
 * and controls for interacting with the engine, like starting and stopping the game, are provided.
 * It provides options to play and stop the game, updating the game state accordingly.
 *
 * @param fps The current frames per second (FPS) value, displayed to the user for performance insights.
 *
 * @details
 * The control panel includes:
 * - **FPS Display**: Shows the current FPS with two decimal precision.
 * - **Play Button**: On click, saves the current scene, updates the engine state to `ES_PLAY`, and
 *   initializes the `LogicSystem` to begin game execution.
 * - **Stop Button**: On click, reloads the current scene from a saved file, updates the engine state to `ES_ENGINE`,
 *   and halts the game to allow further editing.
 */
    void UseImGui::ControlPanel(float fps)
    {
        static std::vector<float> fpsHistory;
        static const int maxHistorySize = 144; // Maximum number of FPS values to store


        fpsHistory.push_back(fps);

        // If the history size exceeds the maximum, remove the oldest value
        if (fpsHistory.size() > maxHistorySize)
        {
            fpsHistory.erase(fpsHistory.begin());
        }

        ImGui::Begin("Control Panel"); // Create a new window titled "Control Panel"

        // Add FPS display inside ControlPanel
        ImGui::Text("FPS: %.2f", fps); // Show FPS with 2 decimal places

        ImGui::PlotLines("FPS History", fpsHistory.data(), fpsHistory.size(), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));

        // Add controls such as buttons, sliders, or entity selectors here
        ImGui::Text("Control Panel Contents");
        // Example: Add a button
        if (ImGui::Button("Play"))
        {
            // Recompile scripts and display popup that its compiling. Remove popup when done
            if (ScriptingEngine::GetInstance().compile_flag)
            {
                UME_ENGINE_INFO("Begin Script reloading");
                ScriptingEngine::GetInstance().compile_flag = false;
                ScriptingEngine::GetInstance().Reload();
                // TODO: Compile runs on the main thread, hence imGUI cannot draw pop-up here...
            }

            // Perform some action when button is clicked
            if (!ScriptingEngine::ScriptHasError)
            {
                // save
                SceneManager::GetInstance().SaveScene(SceneManager::GetInstance().GetCurrScene());
                m_CompileError = false;
                es_current = ENGINE_STATES::ES_PLAY;
                UME_ENGINE_INFO("Simulation (Game is playing) started");
                ECS::GetInstance().GetSystem<LogicSystem>()->Init();
            }
            else
            {
                m_CompileError = true;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            // free
            SceneManager::GetInstance().LoadSaveFile(SceneManager::GetInstance().GetCurrScene() + ".json");

            // Implement functionality to stop the game (e.g., switch to editor mode)
            es_current = ENGINE_STATES::ES_ENGINE;
            UME_ENGINE_INFO("Simulation (Game is stopping) stopped");
        }

        ImGui::End(); // End the control panel window
    }

    void UseImGui::ShowErrorPopup(const std::string& errorMessage)
    {
        // ImGui::OpenPopupOnItemClick()

        if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s", errorMessage.c_str());

            if (ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    /**
 * @brief Loads asset filenames from the `../Assets/Prefabs` directory into `assetFiles`.
 *
 * Clears `assetFiles`, then populates it with names of regular files in the `Prefabs` folder.
 * This updates the asset list dynamically for display in the ImGui interface.
 */
    void UseImGui::LoadContents(const std::string& directory)
    {
        // Clear the previous asset list
        assetFiles.clear();
        folderNames.clear();

        // Load assets from the specified directory
        for (const auto& entry : std::filesystem::directory_iterator(directory))
        {
            if (entry.is_directory())
            {
                folderNames.push_back(entry.path().filename().string());
            }
            else if (entry.is_regular_file())
            {
                assetFiles.push_back(entry.path().filename().string());
            }
        }
    }

    void UseImGui::ContentBrowser(char* filePath)
    {
        static std::string currentDirectory = "../Assets";
        ImGui::Begin("Content Browser");

        // Back Button
        if (currentDirectory != "../Assets" && ImGui::Button("Back"))
        {
            // Go to the parent directory
            currentDirectory = std::filesystem::path(currentDirectory).parent_path().string();
            LoadContents(currentDirectory);
        }

        // Refresh Button
        if (ImGui::Button("Refresh Assets"))
        {
            LoadContents(currentDirectory);
        }

        ImGui::Separator();

        // Display folders first
        for (size_t i = 0; i < folderNames.size(); ++i)
        {
            if (ImGui::Selectable(("[Folder] " + folderNames[i]).c_str(), false))
            {
                // Update to the new directory and reload contents
                currentDirectory += "/" + folderNames[i];
                LoadContents(currentDirectory);
            }
        }

        // Display files
        static int selectedAssetIndex = -1;
        for (size_t i = 0; i < assetFiles.size(); ++i)
        {
            bool isSelected = (selectedAssetIndex == static_cast<int>(i));
            if (ImGui::Selectable(assetFiles[i].c_str(), isSelected))
            {
                selectedAssetIndex = static_cast<int>(i);
                std::string fullPath = currentDirectory + "/" + assetFiles[i];
                std::filesystem::path path = fullPath;
                m_SpriteFlag = path.extension() == ".png";
                if (m_SpriteFlag)
                    m_SpritePath = fullPath;
                std::strncpy(filePath, fullPath.c_str(), 256); // Update filePath with the selected asset
            }

            // Enable dragging for the asset file
            if (ImGui::BeginDragDropSource())
            {
                // Set up the payload to drag
                std::string assetPath = currentDirectory + "/" + assetFiles[i];
                ImGui::SetDragDropPayload("ASSET_PATH", assetPath.c_str(), assetPath.size() + 1);
                ImGui::Text("Drag: %s", assetFiles[i].c_str()); // Text for the drag preview
                ImGui::EndDragDropSource();
            }
        }

        ImGui::End();
    }

    /**
 * @brief Displays a content browser window in ImGui for selecting assets.
 *
 * Lists assets loaded in `assetFiles` with selectable options. Allows users to refresh the list
 * and select an asset, updating `filePath` with the full path of the selected item.
 *
 * @param filePath A character array to store the path of the selected asset.
 */
    void UseImGui::LoadScene()
    {
        // Clear the previous asset list
        sceneFiles.clear();

        // Load assets from the Assets directory
        std::string scenesDir = "../Assets/Scenes";
        for (const auto& entry : std::filesystem::directory_iterator(scenesDir))
        {
            if (entry.is_regular_file())
            {
                sceneFiles.push_back(entry.path().filename().string());
            }
        }
    }

    /**
 * @brief Displays a Scene Browser in ImGui for viewing, saving, and creating scenes.
 *
 * - **Automatic Scene Refresh**: Updates the scene list every second.
 * - **Scene Selection**: Lists available scenes. Selecting a scene saves the current scene and loads the selected one.
 * - **Save Scene**: Toggles an input field to specify a name for saving the current scene.
 * - **Create Scene**: Toggles an input field to name and create a new scene, automatically suggesting a unique name.
 *
 * This function enables scene management directly through the ImGui interface.
 */
    void UseImGui::SceneBrowser()
    {
        // Get the current time
        float currentTime = static_cast<float>(ImGui::GetTime());

        // Check if 1 second has passed since the last update
        if (currentTime - m_LastSceneUpdateTime >= 1.0f)
        {
            // Update the asset list
            LoadScene();
            m_LastSceneUpdateTime = currentTime; // Reset the timer
        }

        ImGui::Begin("Scene Browser");

        if (es_current == ENGINE_STATES::ES_PLAY)
        {
            ImGui::End();
            return;
        }

        // Display the list of scenes
        static int selectedSceneIndex = 0;
        // State variable to manage the visibility of the input field for saving a scene
        static bool showSaveInputField = false;
        static char sceneName[128] = "Level"; // Default name for the scene

        for (size_t i = 0; i < sceneFiles.size(); ++i)
        {
            bool isSelected = (selectedSceneIndex == static_cast<int>(i));
            if (ImGui::Selectable(sceneFiles[i].c_str(), isSelected))
            {
                selectedSceneIndex = static_cast<int>(i);
                SceneManager::GetInstance().SaveScene(SceneManager::GetInstance().GetCurrScene());
                SceneManager::GetInstance().LoadSaveFile(sceneFiles[selectedSceneIndex]);
                std::cout << "Selected scene: " << sceneFiles[selectedSceneIndex] << std::endl;
            }
        }


        // Add a button to save the scene
        if (ImGui::Button("Save Scene"))
        {
            showSaveInputField = !showSaveInputField; // Toggle the input field visibility
            SceneManager::GetInstance().GetOnIMGUI() = showSaveInputField;

            // Get the selected scene's name and remove the ".json" extension if it exists
            std::string selectedScene = sceneFiles[selectedSceneIndex];
            size_t extensionPos = selectedScene.find(".json");
            if (extensionPos != std::string::npos)
            {
                selectedScene = selectedScene.substr(0, extensionPos);
            }

            // Copy the modified name into sceneName
            std::strncpy(sceneName, selectedScene.c_str(), sizeof(sceneName) - 1);
            sceneName[sizeof(sceneName) - 1] = '\0'; // Ensure null-termination
        }

        // Display the input field if showSaveInputField is true
        if (showSaveInputField)
        {
            ImGui::InputText("Scene Name", sceneName, IM_ARRAYSIZE(sceneName));

            if (ImGui::Button("Confirm Save"))
            {
                // Call your SaveScene function
                std::cout << "Scene saved as: " << sceneName << std::endl;

                SceneManager::GetInstance().SaveScene(sceneName);
                std::cout << "Scene saved successfully: " << sceneName << std::endl;
                // Hide the input field after saving
                showSaveInputField = false;
                // Optionally, reset the scene name for the next save
                sceneName[0] = '\0'; // Clear the input field
                SceneManager::GetInstance().GetOnIMGUI() = showSaveInputField;
            }
        }

        // State variable to manage the visibility of the input field for creating a scene
        static bool showCreateInputField = false;
        static char newSceneName[128] = "NewScene"; // Default name for the new scene
        int maxIndex = 0;
        std::string newName = "";

        //find the next sceneName
        for (const auto& scene : sceneFiles)
        {
            if (scene.rfind("NewScene", 0) == 0)
            {
                // Checks if scene starts with "NewScene"
                int index = std::atoi(scene.substr(8).c_str()); // Extract the number after "NewScene"
                if (index > maxIndex)
                {
                    maxIndex = index;
                }
            }
        }
        newName = "NewScene" + std::to_string(maxIndex + 1);

        // Add a button to create a new scene
        if (ImGui::Button("Create Scene"))
        {
            showCreateInputField = !showCreateInputField; // Toggle the input field visibility
            SceneManager::GetInstance().GetOnIMGUI() = showCreateInputField;
            std::strncpy(newSceneName, newName.c_str(), sizeof(newSceneName) - 1);
            newSceneName[sizeof(newSceneName) - 1] = '\0'; // Ensure null-termination
        }

        // Display the input field if showCreateInputField is true
        if (showCreateInputField)
        {
            ImGui::InputText("New Scene Name", newSceneName, IM_ARRAYSIZE(newSceneName));

            if (ImGui::Button("Confirm Create"))
            {
                // Call your CreateScene function
                std::cout << "Creating scene: " << newSceneName << std::endl;

                SceneManager::GetInstance().CreateNewScene(newSceneName);
                std::cout << "Scene created successfully: " << newSceneName << std::endl;
                // Hide the input field after creating
                showCreateInputField = false;
                // Optionally, reset the new scene name for the next creation
                //newSceneName[0] = '\0'; // Clear the input field
                SceneManager::GetInstance().GetOnIMGUI() = showCreateInputField;
            }
        }

        ImGui::End();
    }

    /**
 * @brief Returns the number of scenes in the scene list.
 *
 * This function provides the size of the `sceneFiles` list, representing the total number of scenes available.
 *
 * @return The number of scenes in the `sceneFiles` list.
 */
    size_t UseImGui::GetSceneSize()
    {
        return sceneFiles.size();
    }

    /**
 * @brief Displays detailed information of a `GameObject` in the ImGui interface.
 *
 * This function shows the `GameObject`'s ID, name, and tag, and conditionally displays details for any
 * components attached to the object, such as `Transform` and `Rigidbody2D`.
 *
 * @param obj The `GameObject` whose details are displayed.
 */
    void UseImGui::DisplayEntityDetails(GameObject& obj)
    {
        // Basic Entity Info (White)
        ImGui::Separator();
        ImGui::Text("Entity Details");
        ImGui::BulletText("ID: %d", obj.GetInstanceID());
        ImGui::BulletText("Name: %s", obj.GetName().c_str());
        ImGui::BulletText("Tag: %s", obj.GetTag().c_str());
        ImGui::Separator();

        // Display Transform Component (Green)
        if (obj.HasComponent<Transform>())
        {
            if (ImGui::CollapsingHeader("Transform Component"))
            {
                Transform& transform = obj.GetComponent<Transform>();
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Position: (%.2f, %.2f)", transform.position.x,
                                   transform.position.y);
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Rotation: %.2f", transform.rotation);
                ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Scale: (%.2f, %.2f)", transform.scale.x,
                                   transform.scale.y);
            }
        }

        // Display Rigidbody2D Component (Blue)
        if (obj.HasComponent<Rigidbody2D>())
        {
            if (ImGui::CollapsingHeader("Rigidbody2D Component"))
            {
                Rigidbody2D& rb = obj.GetComponent<Rigidbody2D>();
                ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "Position: (%.2f, %.2f)", rb.position.x,
                                   rb.position.y);
                ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "Velocity: (%.2f, %.2f)", rb.velocity.x,
                                   rb.velocity.y);
                ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "Acceleration: (%.2f, %.2f)", rb.acceleration.x,
                                   rb.acceleration.y);
                ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "Force: (%.2f, %.2f)", rb.force.x, rb.force.y);
                ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "Mass: %.2f", rb.mass);
                ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "Use Gravity: %s",
                                   rb.use_gravity ? "True" : "False");
                ImGui::TextColored(ImVec4(0.0f, 0.5f, 1.0f, 1.0f), "Is Kinematic: %s",
                                   rb.is_kinematic ? "True" : "False");
            }
        }

        // Display BoxCollider2D Component (Orange)
        if (obj.HasComponent<BoxCollider2D>())
        {
            if (ImGui::CollapsingHeader("BoxCollider2D Component"))
            {
                BoxCollider2D& collider = obj.GetComponent<BoxCollider2D>();
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Is Trigger: %s",
                                   collider.is_trigger ? "True" : "False");
            }
        }

        // Display SpriteRender Component (Purple)
        if (obj.HasComponent<SpriteRender>())
        {
            if (ImGui::CollapsingHeader("SpriteRender Component"))
            {
                SpriteRender& sprite = obj.GetComponent<SpriteRender>();
                ImGui::TextColored(ImVec4(0.6f, 0.4f, 1.0f, 1.0f), "Sprite Path: %s", sprite.texturePath.c_str());
                ImGui::TextColored(ImVec4(0.6f, 0.4f, 1.0f, 1.0f), "Shape: %d", sprite.shape);
            }
        }

        // Display Script Component (Yellow)
        if (obj.HasComponent<Script>())
        {
            if (ImGui::CollapsingHeader("Script Component"))
            {
                Script& script = obj.GetComponent<Script>();
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Script Path: %s", script.scriptPath.c_str());
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Class Name: %s", script.scriptName.c_str());
            }
        }
    }

    /**
 * @brief Displays a combo box for selecting a `GameObject` from the current level.
 *
 * This function populates a combo box with the names and IDs of all game objects in the current level.
 * It allows the user to select an entity by index, updating `selectedEntityIndex` based on the selection.
 *
 * @param selectedEntityIndex The index of the currently selected entity, which will be updated upon selection.
 */
    void UseImGui::DisplayEntitySelectionCombo(int& selectedEntityIndex)
    {
        // auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();
        auto gameObjects = GameObjectManager::GetInstance().GetAllGOs();

        // Store the names of entities in a vector of strings to keep them in memory
        std::vector<std::string> entityNames;
        for (const auto& obj : gameObjects)
        {
            entityNames.push_back(std::to_string(obj->GetInstanceID()) + ": " + obj->GetName());
        }

        // Check if there are any entities and set the selected index
        if (!entityNames.empty() && selectedEntityIndex == -1)
        {
            selectedEntityIndex = 0; // Default to the first item if nothing is selected
        }

        // Create a vector of const char* pointers to each name in entityNames
        std::vector<const char*> entityNamePointers;
        for (const auto& name : entityNames)
        {
            entityNamePointers.push_back(name.c_str());
        }

        // Display the ImGui Combo with the populated entityNamePointers
        ImGui::Combo("Select Entity to Remove", &selectedEntityIndex, entityNamePointers.data(),
                     static_cast<int>(entityNamePointers.size()));
    }

    /**
 * @brief Removes the selected `GameObject` from the game world.
 *
 * This function destroys the `GameObject` at the index specified by `selectedEntityIndex`
 * and resets the selection index to -1.
 *
 * @param selectedEntityIndex The index of the selected entity to be removed.
 */
    void UseImGui::RemoveSelectedEntity(int& selectedEntityIndex)
    {
        // auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();
        auto gameObjects = GameObjectManager::GetInstance().GetAllGOs();
        if (static_cast<size_t>(selectedEntityIndex >= 0 && selectedEntityIndex) < gameObjects.size())
        {
            auto& entityToDelete = gameObjects[selectedEntityIndex];
            GameObjectManager::GetInstance().DestroyObject(entityToDelete->GetInstanceID());
            selectedEntityIndex = -1;
        }
    }

    void UseImGui::AddComponentUI(GameObject* selectedObject, bool& modified)
    {
        ImGui::Text("Component Management");

        static int selectedComponentIndex = 0;
        const char* availableComponents[] = {
            "Transform",
            "Rigidbody2D",
            "BoxCollider2D",
            "SpriteRender",
            "Script",
            "Animation"
        };

        ImGui::Text("Add Component");
        ImGui::Combo("##ComponentCombo", &selectedComponentIndex, availableComponents,
                     IM_ARRAYSIZE(availableComponents));

        if (ImGui::Button("Add Selected Component"))
        {
            switch (selectedComponentIndex)
            {
            case 0: // Transform
                if (!selectedObject->HasComponent<Transform>())
                {
                    selectedObject->AddComponent<Transform>(Transform{}); // Use default or custom parameters
                    modified = true;
                }
                break;
            case 1: // Rigidbody2D
                if (!selectedObject->HasComponent<Rigidbody2D>())
                {
                    selectedObject->AddComponent<Rigidbody2D>(Rigidbody2D{});
                    modified = true;
                }
                break;
            case 2: // BoxCollider2D
                if (!selectedObject->HasComponent<BoxCollider2D>())
                {
                    selectedObject->AddComponent<BoxCollider2D>(BoxCollider2D{});
                    modified = true;
                }
                break;
            case 3: // SpriteRender
                if (!selectedObject->HasComponent<SpriteRender>())
                {
                    selectedObject->AddComponent<SpriteRender>(SpriteRender{});
                    modified = true;
                }
                break;
            case 4: // Script
                if (!selectedObject->HasComponent<Script>())
                {
                    selectedObject->AddComponent<Script>(Script{});
                    modified = true;
                }
                break;
            case 5: // Animation
                if (!selectedObject->HasComponent<Animation>())
                {
                    selectedObject->AddComponent<Animation>(Animation{});
                    auto& anim = selectedObject->GetComponent<Animation>();
                    modified = true;
                }
            default:
                break;
            }
        }
    }

    void UseImGui::RemoveComponentUI(GameObject* selectedObject, bool& modified)
    {
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Remove Component"); // Change text color

        // Start a collapsible section for remove components
        if (ImGui::CollapsingHeader("Remove Components"))
        {
            // Only show the buttons for components that exist in the selected object
            if (selectedObject->HasComponent<Rigidbody2D>())
            {
                if (ImGui::Button("Remove Rigidbody2D Component"))
                {
                    selectedObject->RemoveComponent<Rigidbody2D>();
                    modified = true;
                }
                ImGui::Spacing(); // Add spacing after button for separation
            }

            if (selectedObject->HasComponent<BoxCollider2D>())
            {
                if (ImGui::Button("Remove BoxCollider2D Component"))
                {
                    selectedObject->RemoveComponent<BoxCollider2D>();
                    modified = true;
                }
                ImGui::Spacing();
            }

            if (selectedObject->HasComponent<SpriteRender>())
            {
                if (ImGui::Button("Remove SpriteRender Component"))
                {
                    selectedObject->RemoveComponent<SpriteRender>();
                    modified = true;
                }
                ImGui::Spacing();
            }

            if (selectedObject->HasComponent<Script>())
            {
                if (ImGui::Button("Remove Script Component"))
                {
                    selectedObject->RemoveComponent<Script>();
                    modified = true;
                }
                ImGui::Spacing();
            }
        }

        ImGui::Separator(); // Optional separator after the collapsible section
    }

    /**
 * @brief Displays and edits the properties of the selected `GameObject`.
 *
 * This function allows editing of the `Transform` and `Rigidbody2D` components
 * of the `GameObject` using either sliders or input fields. The `modified` flag is set
 * to `true` whenever a property is changed.
 *
 * @param selectedObject The `GameObject` whose properties are being edited.
 * @param modified A reference to a boolean that is set to `true` when any property is modified.
 */
    void UseImGui::EditEntityProperties(GameObject* selectedObject, bool& modified)
    {
        if (!selectedObject) return;

        // Store whether the rename mode is enabled
        static bool isRenaming = false;

        // Button to enable/disable renaming
        if (ImGui::Button(isRenaming ? "Confirm Rename" : "Rename Entity Name"))
        {
            isRenaming = !isRenaming; // Toggle renaming mode
        }

        // Editable name input field, only enabled if renaming is allowed
        char nameBuffer[256];
        std::strncpy(nameBuffer, selectedObject->GetName().c_str(), sizeof(nameBuffer));
        nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // Ensure null termination

        // Disable input field when not renaming
        if (isRenaming)
        {
            if (ImGui::InputText("Object Name", nameBuffer, sizeof(nameBuffer)))
            {
                selectedObject->SetName(std::string(nameBuffer));
                modified = true; // Flag as modified
            }
        }
        else
        {
            // Display the name as text if not renaming
            //ImGui::Text("Object Name: %s", selectedObject->GetName().c_str());
        }

        ImGui::Text("Editing properties of: %s", selectedObject->GetName().c_str());

        // Checkbox to toggle between sliders and input fields
        static bool useSliders = true;
        ImGui::Checkbox("Use Sliders", &useSliders);

        ImGui::Separator();

        // Add Components
        AddComponentUI(selectedObject, modified);

        ImGui::Separator();

        // Remove Components
        RemoveComponentUI(selectedObject, modified);

        ImGui::Separator();

        // Transform Component
        if (selectedObject->HasComponent<Transform>())
        {
            if (ImGui::CollapsingHeader("Transform"))
            {
                Transform& transform = selectedObject->GetComponent<Transform>();

                // Position
                ImGui::Text("Position");
                if (useSliders)
                {
                    if (ImGui::SliderFloat2("##PositionSlider", &transform.position.x, -800.0f, 1500.0f)) modified =
                        true;
                }
                else
                {
                    if (ImGui::InputFloat2("##PositionInput", &transform.position.x)) modified = true;
                }

                // Rotation
                ImGui::Text("Rotation");
                if (useSliders)
                {
                    if (ImGui::SliderFloat("##RotationSlider", &transform.rotation, -180.0f, 180.0f)) modified = true;
                }
                else
                {
                    if (ImGui::InputFloat("##RotationInput", &transform.rotation)) modified = true;
                }

                // Scale
                ImGui::Text("Scale");
                if (useSliders)
                {
                    if (ImGui::SliderFloat2("##ScaleSlider", &transform.scale.x, 70.f, 250.0f)) modified = true;
                }
                else
                {
                    if (ImGui::InputFloat2("##ScaleInput", &transform.scale.x)) modified = true;
                }
            }
        }

        // Rigidbody2D Component
        if (selectedObject->HasComponent<Rigidbody2D>())
        {
            if (ImGui::CollapsingHeader("Rigidbody2D"))
            {
                Rigidbody2D& rb = selectedObject->GetComponent<Rigidbody2D>();

                // Velocity
                ImGui::Text("Velocity");
                if (useSliders)
                {
                    if (ImGui::SliderFloat2("##VelocitySlider", &rb.velocity.x, -50.0f, 50.0f)) modified = true;
                }
                else
                {
                    if (ImGui::InputFloat2("##VelocityInput", &rb.velocity.x)) modified = true;
                }

                // Mass
                ImGui::Text("Mass");
                if (useSliders)
                {
                    if (ImGui::SliderFloat("##MassSlider", &rb.mass, 0.1f, 100.0f)) modified = true;
                }
                else
                {
                    if (ImGui::InputFloat("##MassInput", &rb.mass)) modified = true;
                }

                // Force
                ImGui::Text("Force");
                if (useSliders)
                {
                    if (ImGui::SliderFloat("##ForceSlider", &rb.force.x, 0.1f, 100.0f)) modified = true;
                }
                else
                {
                    if (ImGui::InputFloat("##ForceInput", &rb.force.x)) modified = true;
                }

                // Use Gravity Checkbox
                ImGui::Text("Use Gravity");
                if (ImGui::Checkbox("##UseGravityCheckbox", &rb.use_gravity)) modified = true;

                // Is Kinematic Checkbox
                ImGui::Text("Is Kinematic");
                if (ImGui::Checkbox("##IsKinematicCheckbox", &rb.is_kinematic)) modified = true;
            }
        }

        // BoxCollider2D Component
        if (selectedObject->HasComponent<BoxCollider2D>())
        {
            if (ImGui::CollapsingHeader("BoxCollider2D"))
            {
                BoxCollider2D& collider = selectedObject->GetComponent<BoxCollider2D>();

                ImGui::Text("Use Collider Box");
                if (ImGui::Checkbox("##UseColliderCheckbox", &collider.is_trigger)) modified = true;
            }
        }

        // SpriteRender Component
        if (selectedObject->HasComponent<SpriteRender>())
        {
            if (ImGui::CollapsingHeader("SpriteRender"))
            {
                SpriteRender& sprite = selectedObject->GetComponent<SpriteRender>();

                ImGui::Text("Texture Path");

                char texturePathBuffer[256];
                strncpy(texturePathBuffer, sprite.texturePath.c_str(), sizeof(texturePathBuffer));
                texturePathBuffer[sizeof(texturePathBuffer) - 1] = '\0'; // Ensure null termination

                ImGui::BeginDisabled(true); // Disable the input box while we use drag-and-drop
                ImGui::InputText("##TexturePathInput", texturePathBuffer, sizeof(texturePathBuffer));
                ImGui::EndDisabled();

                // Check for drag-and-drop
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                    {
                        std::string draggedPath = std::string((const char*)payload->Data);
                        std::filesystem::path filePath(draggedPath);
                        std::string extension = filePath.extension().string();

                        if (extension == ".png")
                        {
                            // Valid file type, update texture path
                            strncpy(texturePathBuffer, draggedPath.c_str(), sizeof(texturePathBuffer));
                            texturePathBuffer[sizeof(texturePathBuffer) - 1] = '\0'; // Ensure null termination
                            sprite.texturePath = draggedPath;
                            modified = true;
                        }
                        else
                        {
                            // Invalid file type, show error feedback
                            ImGui::OpenPopup("InvalidTextureFileType");
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                // Error Popup for invalid file type
                if (ImGui::BeginPopup("InvalidTextureFileType"))
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Only .png files are allowed!");
                    if (ImGui::Button("OK"))
                    {
                        ImGui::CloseCurrentPopup(); // Close the popup when the button is pressed
                    }
                    ImGui::EndPopup();
                }
            }
        }

        // Script Component, drag-and-drop for script path
        if (selectedObject->HasComponent<Script>())
        {
            if (ImGui::CollapsingHeader("Script"))
            {
                Script& script = selectedObject->GetComponent<Script>();
                ImGui::Text("Script Path");

                char scriptPathBuffer[256];
                strncpy(scriptPathBuffer, script.scriptPath.c_str(), sizeof(scriptPathBuffer));
                scriptPathBuffer[sizeof(scriptPathBuffer) - 1] = '\0'; // Ensure null termination

                ImGui::BeginDisabled(true); // Disable the input box while we use drag-and-drop
                ImGui::InputText("##ScriptPathInput", scriptPathBuffer, sizeof(scriptPathBuffer));
                ImGui::EndDisabled();

                // Check for drag-and-drop
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                    {
                        std::string draggedPath = std::string((const char*)payload->Data);
                        std::filesystem::path filePath(draggedPath);
                        std::string extension = filePath.extension().string();

                        if (extension == ".cs")
                        {
                            // Valid file type, update script path
                            strncpy(scriptPathBuffer, draggedPath.c_str(), sizeof(scriptPathBuffer));
                            scriptPathBuffer[sizeof(scriptPathBuffer) - 1] = '\0'; // Ensure null termination
                            script.scriptPath = draggedPath;
                            script.scriptName = filePath.stem().string();
                            MonoObject* newScript = ScriptingEngine::GetInstance().InstantiateClientClass(
                                script.scriptName);
                            EntityID newScriptID = selectedObject->GetInstanceID();
                            ScriptingEngine::SetMonoFieldValueULL(newScript, "_id", &newScriptID);
                            script.instance = newScript;
                            script.handle = ScriptingEngine::CreateGCHandle(newScript);
                            modified = true;
                        }
                        else
                        {
                            // Invalid file type, show error feedback
                            ImGui::OpenPopup("InvalidScriptFileType");
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                // Error Popup for invalid file type
                if (ImGui::BeginPopup("InvalidScriptFileType"))
                {
                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Only .cs files are allowed!");
                    if (ImGui::Button("OK"))
                    {
                        ImGui::CloseCurrentPopup(); // Close the popup when the button is pressed
                    }
                    ImGui::EndPopup();
                }
            }
        }

        // Animation Component
        if (selectedObject->HasComponent<Animation>())
        {
            // TODO: Drag and drop texture to animation should pull the metadata
            if (ImGui::CollapsingHeader("Animation"))
            {
                auto& animation = selectedObject->GetComponent<Animation>();
                if (ImGui::TreeNode("Clips"))
                {
                    for (auto& clip : animation.clips)
                    {
                        if (ImGui::TreeNode(clip.second.name.c_str()))
                        {
                            ImGui::Text("Total_Frame: %d", clip.second.total_frames);
                            ImGui::Text("Frame_Rate: %f", clip.second.frame_time);
                            ImGui::Text("Loop: %s", clip.second.looping ? "True" : "False");
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
                static char currentClipBuffer[256] = "";
                strncpy(currentClipBuffer, animation.currentClip.c_str(), sizeof(currentClipBuffer));
                ImGui::BeginDisabled(true);
                ImGui::InputText("##CurrentClip", currentClipBuffer, IM_ARRAYSIZE(currentClipBuffer));
                ImGui::EndDisabled();
                
                static char changeClipBuffer[256] = "";
                ImGui::InputTextWithHint("Default Clip", "Enter Clip Name", changeClipBuffer, IM_ARRAYSIZE(changeClipBuffer));
                if (ImGui::Button("Set Current Clip"))
                {
                    animation.currentClip = changeClipBuffer;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear all Clips"))
                {
                    animation.clips.clear();
                    animation.currentClip = "";
                }
            }
        }
    }

    /**
 * @brief Checks if the given file path corresponds to a JSON file.
 *
 * This function verifies if the provided file path ends with the `.json` extension.
 *
 * @param filePath The path of the file to check.
 * @return `true` if the file is a JSON file, otherwise `false`.
 */
    bool IsJsonFile(const std::string& filePath)
    {
        return filePath.size() >= 5 && filePath.substr(filePath.size() - 5) == ".json";
    }


    /**
 * @brief Displays an ImGui interface for managing game entities.
 *
 * This function allows users to create, select, edit, and remove game entities.
 * It validates file paths for entity creation, shows error messages for invalid files,
 * and lets users modify entity properties such as position, rotation, and scale.
 *
 * @return None
 */
    void UseImGui::ShowEntityManagementUI()
    {
        // Begin a dockable window
        ImGui::Begin("Entity Management", nullptr, ImGuiWindowFlags_None);

        static char filePath[256] = "../Assets/Prefabs/Player.json";
        static int selectedEntityIndex = -1;
        m_global_selected = selectedEntityIndex;
        static bool showError = false;
        static double errorDisplayTime = 0.0f;

        // Persistent flag to track if the selected entity was modified
        static bool modified = false;

        // Display the Asset Browser
        ContentBrowser(filePath);

        if (ImGui::TreeNode("Current GameObjects"))
        {
            auto gameObjects = GameObjectManager::GetInstance().GetAllGOs();

            for (size_t i = 0; i < gameObjects.size(); ++i)
            {
                auto& obj = gameObjects[i];
                if (ImGui::TreeNode((std::to_string(obj->GetInstanceID()) + ": " + obj->GetName()).c_str()))
                {
                    DisplayEntityDetails(*obj);
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }

        ImGui::Text("Entity Management");
        ImGui::InputText("Object Data File", filePath, IM_ARRAYSIZE(filePath));

        // Button to create a new entity from a prefab
        if (ImGui::Button("Create Entity"))
        {
            if (filePath[0] != '\0' && IsJsonFile(filePath))
            {
                if (ECS::GetInstance().GetLivingEntityCount() == 0)
                {
                    ECS::GetInstance().GetSystem<Transformation>()->player = -1;
                    ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(-1);
                }

                auto& go = GameObjectManager::GetInstance().CreatePrefabObject(filePath);
                if (go.GetTag() == "Player")
                {
                    ECS::GetInstance().GetSystem<Transformation>()->player = static_cast<int>(go.GetInstanceID());
                    ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(static_cast<int>(go.GetInstanceID()));
                    // ECS::GetInstance().GetSystem<Renderer>()->initAnimationEntities();
                    // go.GetComponent<SpriteRender>().animated = true;
                    //go.GetComponent<SpriteRender>().animationIndex = 1;
                }
                showError = false; // Reset error
            }
            else
            {
                showError = true; // Show error message
                errorDisplayTime = ImGui::GetTime();
            }
        }

        ImGui::SameLine();

        // Button to create an empty entity
        if (ImGui::Button("Create Empty Entity"))
        {
            auto& emptyObject = GameObjectManager::GetInstance().CreateEmptyObject();
            selectedEntityIndex = static_cast<int>(emptyObject.GetInstanceID());
            modified = true;
        }

        if (showError && ImGui::GetTime() - errorDisplayTime < 2.0f)
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1),
                               "Invalid file type. Only .json files can be used to create an object.");
        }

        ImGui::Separator();

        DisplayEntitySelectionCombo(selectedEntityIndex);

        if (ImGui::Button("Remove Entity"))
        {
            RemoveSelectedEntity(selectedEntityIndex);
            modified = false;
        }

        ImGui::SameLine();

        if (selectedEntityIndex >= 0)
        {
            auto gameObjects = GameObjectManager::GetInstance().GetAllGOs();
            if (selectedEntityIndex < gameObjects.size())
            {
                GameObject* selectedObject = gameObjects[selectedEntityIndex];

                EditEntityProperties(selectedObject, modified);

                // Show the Save button if modifications were made
                if (modified)
                {
                    if (ImGui::Button("Save Entity"))
                    {
                        std::cout << "Entity is Saved";
                        SceneManager::GetInstance().SavePrefab(selectedObject, selectedObject->GetName());
                        modified = false; // Reset modified flag after saving
                    }
                }
            }
        }

        // End the dockable window
        ImGui::End();
    }

    /**
 * @brief Renders the game scene within an ImGui window and handles mouse interaction.
 *
 * This function displays the game scene in a "Player Loader" window using a texture from the renderer system.
 * It also calculates the mouse position relative to the window and updates the play screen with the mouse's
 * position for interaction. The scene is displayed using ImGui's `Image` widget, and the mouse position
 * within the window is used to set the play screen coordinates.
 *
 * @return None
 */
    void UseImGui::SceneRender()
    {
        static bool showGameView = true;
        Application& app = Application::Get();
        //GLuint texture = renderer.getTextureColorBuffer();
        GLuint texture = ECS::GetInstance().GetSystem<Renderer>()->getTextureColorBuffer();

        if (showGameView)
        {
            ImGui::Begin("Player Loader", &showGameView); // Create a window called "Another Window"
            ImGui::Image((ImTextureID)(intptr_t)texture,
                         ImVec2(static_cast<float>(app.GetWindow().GetWidth()),
                                static_cast<float>(app.GetWindow().GetHeight())), {0, 1}, {1, 0});


            // Get the position of the ImGui window
            ImVec2 windowPos = ImGui::GetWindowPos(); // Top-left position of the window
            ImVec2 windowSize = ImGui::GetWindowSize(); // Size of the window

            // Get the mouse position in screen coordinates
            ImVec2 mousePos = ImGui::GetMousePos();

            // Calculate mouse position relative to the "Player Loader" window
            float relativeX = mousePos.x - windowPos.x;
            float relativeY = windowSize.y - (mousePos.y - windowPos.y);


            // Check if the mouse is within the bounds of the window
            if (relativeX >= 0 && relativeX <= windowSize.x && relativeY >= 0 && relativeY <= windowSize.y)
            {
                // Optional: Handle the mouse position within the window here
                //std::cout << "Mouse relative position in 'Player Loader' window: (" << relativeX << ", " << relativeY << ")\n";
            }
            SceneManager::GetInstance().SetPlayScreen(Vec2(relativeX, relativeY));
            // Handle the drop target (accept file drops here)
            if (ImGui::BeginDragDropTarget())
            {
                const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH");
                if (payload)
                {
                    // The payload data is the file path
                    const char* droppedFilePath = static_cast<const char*>(payload->Data);
                    std::cout << "Dropped File: " << droppedFilePath << std::endl;

                    // Create an entity based on the dropped file
                    if (droppedFilePath[0] != '\0' && IsJsonFile(droppedFilePath))
                    {
                        if (ECS::GetInstance().GetLivingEntityCount() == 0)
                        {
                            ECS::GetInstance().GetSystem<Transformation>()->player = -1;
                            ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(-1);
                        }

                        auto& go = GameObjectManager::GetInstance().CreatePrefabObject(droppedFilePath);
                        // You can add additional logic for the entity, like setting up textures, animations, etc.
                        if (go.GetTag() == "Player")
                        {
                            ECS::GetInstance().GetSystem<Transformation>()->player = static_cast<int>(go.
                                GetInstanceID());
                            ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(static_cast<int>(go.GetInstanceID()));
                            ECS::GetInstance().GetSystem<Renderer>()->initAnimationEntities();
                            go.GetComponent<SpriteRender>().animated = true;
                        }
                    }
                    else
                    {
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::End();
        }
    }

    std::string UseImGui::GetStartScene()
    {
        return sceneFiles[0];
    }

    void UseImGui::Begin()
    {
        static bool show = true;
        ImGui::ShowDemoWindow(&show);
    }

    /*!
    \brief Renders the current ImGui frame and draws the UI.
    */
    void UseImGui::ImGuiUpdate()
    {
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()),
                                static_cast<float>(app.GetWindow().GetHeight()));


        // Call the combined function to check and handle file drops
        //CheckAndHandleFileDrop();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        /*ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());*/
    }

    /*!
    \brief Cleans up ImGui resources.
    */
    void UseImGui::ImGuiClean()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    /*!
    \brief Dispatches events to ImGui for handling input.
    \param event Reference to the event to be dispatched.
    */
}

template class UME_API std::vector<std::string>;
template class UME_API std::vector<std::string>;
