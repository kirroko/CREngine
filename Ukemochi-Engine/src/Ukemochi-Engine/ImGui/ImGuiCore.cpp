/* Start Header
*****************************************************************/
/*!
\file       ImGuiCore.cpp
\author     Hurng Kai Rui, h.kairui, 2301278
\co-authors Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu
\par        email: h.kairui\@digipen.edu
\date       Sept 25, 2024
\brief      This file contains the implementation of the UseImGui class,
            which manages the initialization, rendering, and event handling 
            for ImGui within the engine.


Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/

#include "PreCompile.h"
#include "ImGuiCore.h"

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
    std::vector<std::string> UseImGui::sceneFiles;
    bool UseImGui::m_CompileError = false;
    bool UseImGui::m_Compiling = false;

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

        // Create a main menu bar with an option to toggle docking
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Enable Docking", NULL, &enableDocking);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();

        // Set up the window to cover the entire viewport
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        // Configure window flags for a fullscreen dockspace
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        // Optionally make the background transparent
        window_flags |= ImGuiWindowFlags_NoBackground;

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

        if(m_CompileError)
        {
            ImGui::OpenPopup("Error");
            m_CompileError = false;
        }
        ShowErrorPopup("A compile error has occurred. Please check the console for more information.");
        
        //ImGui::SaveIniSettingsToDisk("imgui_layout.ini");
    }

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
        ImGui::Begin("Control Panel"); // Create a new window titled "Control Panel"

        // Add FPS display inside ControlPanel
        ImGui::Text("FPS: %.2f", fps); // Show FPS with 2 decimal places

        // Add controls such as buttons, sliders, or entity selectors here
        ImGui::Text("Control Panel Contents");
        // Example: Add a button
        if (ImGui::Button("Play"))
        {
            // Recompile scripts and display popup that its compiling. Remove popup when done
            if(ScriptingEngine::GetInstance().compile_flag)
            {
                UME_ENGINE_INFO("Begin Script reloading");
                ScriptingEngine::GetInstance().compile_flag = false;
                ScriptingEngine::GetInstance().Reload(); // TODO: Compile runs on the main thread, hence imGUI cannot draw pop-up here...
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

        if(ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("%s",errorMessage.c_str());
            
            if(ImGui::Button("OK"))
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
    void UseImGui::LoadContents()
    {
        // Clear the previous asset list
        assetFiles.clear();

        // Load assets from the Assets directory
        std::string assetsDir = "../Assets/Prefabs";
        for (const auto& entry : std::filesystem::directory_iterator(assetsDir))
        {
            if (entry.is_regular_file())
            {
                assetFiles.push_back(entry.path().filename().string());
            }
        }
    }

    void UseImGui::ContentBrowser(char* filePath)
    {
        ImGui::Begin("Content Browser");
        //LoadAssets();
        if (ImGui::Button("Refresh Assets"))
        {
            LoadContents(); // Manually trigger loading assets
        }

        static int selectedAssetIndex = -1;
        for (size_t i = 0; i < assetFiles.size(); ++i)
        {
            bool isSelected = (selectedAssetIndex == static_cast<int>(i));
            if (ImGui::Selectable(assetFiles[i].c_str(), isSelected))
            {
                selectedAssetIndex = static_cast<int>(i);
                std::string fullPath = "../Assets/Prefabs/" + assetFiles[i];
                std::strncpy(filePath, fullPath.c_str(), 256); // Update filePath with the selected asset
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
        ImGui::Text("ID: %d", obj.GetInstanceID());
        ImGui::Text("Name: %s", obj.GetName().c_str());
        ImGui::Text("Tag: %s", obj.GetTag().c_str());

        if (obj.HasComponent<Transform>())
        {
            Transform& transform = obj.GetComponent<Transform>();
            ImGui::Text("Transform Component");
            ImGui::Text("Position: (%.2f, %.2f)", transform.position.x, transform.position.y);
            ImGui::Text("Rotation: %.2f", transform.rotation);
            ImGui::Text("Scale: (%.2f, %.2f)", transform.scale.x, transform.scale.y);
        }

        if (obj.HasComponent<Rigidbody2D>())
        {
            Rigidbody2D& rb = obj.GetComponent<Rigidbody2D>();
            ImGui::Text("Rigidbody2D Component");
            ImGui::Text("Velocity: (%.2f, %.2f)", rb.velocity.x, rb.velocity.y);
            ImGui::Text("Mass: %.2f", rb.mass);
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

        ImGui::Text("Editing properties of: %s", selectedObject->GetName().c_str());

        // Checkbox to toggle between sliders and input fields
        static bool useSliders = true;
        ImGui::Checkbox("Use Sliders", &useSliders);

        if (selectedObject->HasComponent<Transform>())
        {
            Transform& transform = selectedObject->GetComponent<Transform>();

            // Position
            ImGui::Text("Position");
            if (useSliders)
            {
                if (ImGui::SliderFloat2("##PositionSlider", &transform.position.x, -800.0f, 1500.0f)) modified = true;
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

        if (selectedObject->HasComponent<Rigidbody2D>())
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

        static bool showError = false;
        static double errorDisplayTime = 0.0f;

        // Persistent flag to track if the selected entity was modified
        static bool modified = false;

        // Display the Asset Browser
        ContentBrowser(filePath);

        if (ImGui::TreeNode("Current GameObjects"))
        {
            // auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();
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
                //ECS::GetInstance().GetSystem<Renderer>()->setUpTextures(go.GetComponent<SpriteRender>().texturePath, ECS::GetInstance().GetSystem<Renderer>()->current_texture_index);
                if (go.GetTag() == "Player")
                {
                    ECS::GetInstance().GetSystem<Transformation>()->player = static_cast<int>(go.GetInstanceID());

                    ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(static_cast<int>(go.GetInstanceID()));
                    //ECS::GetInstance().GetSystem<Renderer>()->SetPlayerObject(go);
                    ECS::GetInstance().GetSystem<Renderer>()->initAnimationEntities();
                    go.GetComponent<SpriteRender>().animated = true;
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

        if (showError && ImGui::GetTime() - errorDisplayTime < 2.0f)
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1),
                               "Invalid file type. Only .json files can be used to create an object.");
        }

        DisplayEntitySelectionCombo(selectedEntityIndex);

        if (ImGui::Button("Remove Entity"))
        {
            RemoveSelectedEntity(selectedEntityIndex);
            modified = false;
        }

        if (selectedEntityIndex >= 0)
        {
            // auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();
            auto gameObjects = GameObjectManager::GetInstance().GetAllGOs();
            if (selectedEntityIndex < gameObjects.size())
            {
                // GameObject* selectedObject = gameObjects[selectedEntityIndex];
                // EditEntityProperties(*selectedObject);
                GameObject* selectedObject = gameObjects[selectedEntityIndex];

                EditEntityProperties(selectedObject, modified);

                // Show the Save button if modifications were made
                if (modified)
                {
                    if (ImGui::Button("Save Entity"))
                    {
                        std::cout << "Entity is Saved";
                        SceneManager::GetInstance().SavePrefab(selectedObject, selectedObject->GetName());
                        //SaveEntity(selectedObject, filePath);  // Ensure filePath points to the correct location
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
