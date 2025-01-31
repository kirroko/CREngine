/* Start Header ************************************************************************/
/*!
\file       ImGuiCore.cpp
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu (80%)
\co-authors Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu (10%)
\co-authors WONG JUN YU, Kean, junyukean.wong, 2301234, junyukean.wong\@digipen.edu (10%)
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
#include "ImGuizmo.h"

#include "../Application.h"

#include "../Logic/Logic.h"
#include "../Factory/Factory.h"
#include "../ECS/ECS.h"

#include "../Graphics/Renderer.h"
#include "../SceneManager.h"
#include "../Math/Transformation.h"
#include "Ukemochi-Engine/FrameController.h"
#include "Ukemochi-Engine/Collision/Collision.h"
#include "Ukemochi-Engine/Factory/GameObjectManager.h"
#include "../Game/EnemyManager.h"
#include "../Game/DungeonManager.h"
#include <../vendor/glm/glm/gtx/matrix_decompose.hpp>

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
    std::string UseImGui::m_TexturePath;
    int UseImGui::m_global_selected = -1;
    unsigned int UseImGui::m_currentPanelWidth = 1920;
    unsigned int UseImGui::m_currentPanelHeight = 1080;
    ImGuizmo::OPERATION currentGizmoOperation = ImGuizmo::TRANSLATE;

    ImVec2 playerLoaderTopLeft;
    /*!
    \brief Initializes the ImGui context and sets up OpenGL.
    \param window Pointer to the GLFW window (unused in this implementation).
    */
    void UseImGui::ImGuiInit(GLFWwindow * /*window*/)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        io.FontGlobalScale = 1.5f; // Increase font size by 50% (adjust as needed)

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
        // io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // ImGui::LoadIniSettingsFromDisk("imgui_layout.ini");

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        ImGuiStyle &style = ImGui::GetStyle();
        style.Alpha = 1.0f;                        // Set the overall alpha of the UI
        style.WindowRounding = 5.0f;               // Round corners of windows
        style.FrameRounding = 3.0f;                // Round corners of frames
        style.ItemSpacing = ImVec2(10, 10);        // Spacing between items
        style.WindowBorderSize = 0.0f;             // No border for windows
        style.WindowPadding = ImVec2(0.0f, -1.5f); // Padding for windows

        // ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 5.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        Application &app = Application::Get();
        GLFWwindow *window = static_cast<GLFWwindow *>(app.GetWindow().GetNativeWindow());

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 410");
        // ImGui::SaveIniSettingsToDisk("imgui_layout.ini");
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
        float deltaTime = currentTime - m_Time;                // Calculate time since last frame
        m_Time = currentTime;                                  // Update the time for the next frame

        float fps = 1.0f / deltaTime;

        ImGuiIO &io = ImGui::GetIO();
        Application &app = Application::Get();
        io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()),
                                static_cast<float>(app.GetWindow().GetHeight()));

        ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Toggle to enable or disable docking
        static bool enableDocking = true;

        // if (io.KeyCtrl && Input::IsKeyTriggered(UME_KEY_D))
        //{
        //     enableDocking = !enableDocking;
        // }
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("Options"))
        {
            ImGui::MenuItem("Enable Docking", NULL, &enableDocking);

            static float fontSize = 1.5f; // Default font scale (1.0 = 100%)

            ImGui::Text("Font Size:");
            if (ImGui::SliderFloat("##FontSize", &fontSize, 1.0f, 4.0f, "%.1fx"))
            {
                ImGui::GetIO().FontGlobalScale = fontSize; // Apply the new font scale
            }

            if (ImGui::MenuItem("Quit"))
            {
                es_current = ENGINE_STATES::ES_QUIT;
            }
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

        // ImGui::SaveIniSettingsToDisk("imgui_layout.ini");
    }

    // void UseImGui::CheckAndHandleFileDrop()
    //{
    //     Application& app = Application::Get();
    //     WindowsWindow& win = (WindowsWindow&)app.GetWindow();

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

    void UseImGui::RenderGizmo2d()
    {
        // Ensure a valid entity is selected
        auto &selectedEntityID = ECS::GetInstance().GetSystem<Renderer>()->selectedEntityID;

        if (selectedEntityID == -1)
        {
            return;
        }
        if (selectedEntityID >= 0)
        {
            std::cout << "Entity " << selectedEntityID << " selected for Gizmo." << std::endl;
        }

        Transform &transform = ECS::GetInstance().GetComponent<Transform>(selectedEntityID);

        const auto &camera = ECS::GetInstance().GetSystem<Camera>();

        // Use the camera's view and projection matrices
        glm::mat4 viewMatrix = camera->getCameraViewMatrix();
        glm::mat4 projectionMatrix = camera->getCameraProjectionMatrix();

        // Create the entity's transformation matrix
        glm::mat4 objectMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(transform.position.x, transform.position.y, 0));
        objectMatrix = glm::rotate(objectMatrix, glm::radians(transform.rotation), glm::vec3(0, 0, 1));
        objectMatrix = glm::scale(objectMatrix, glm::vec3(transform.scale.x, transform.scale.y, 1));

        // Set the Gizmo to the main scene's viewport
        ImGuizmo::SetOrthographic(true); // Use perspective if your camera is perspective
        ImGuizmo::SetDrawlist();
        // Set Gizmo to the rendering viewport or fallback to window size
        ImVec2 panelSize = ImGui::GetContentRegionAvail();
        ImGuizmo::SetRect(playerLoaderTopLeft.x, playerLoaderTopLeft.y, panelSize.x, panelSize.y);

        // Check for user input to change the mode
        if (ImGui::IsKeyPressed(ImGuiKey_8))
        {
            currentGizmoOperation = ImGuizmo::TRANSLATE;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_9))
        {
            currentGizmoOperation = ImGuizmo::ROTATE;
        }
        if (ImGui::IsKeyPressed(ImGuiKey_0))
        {
            currentGizmoOperation = ImGuizmo::SCALE;
        }
        if (ImGuizmo::IsUsing())
            std::cout << "Gizmo is being interacted with." << std::endl;

        // Manipulate the Gizmo and update the entity transform
        if (ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix),
                                 currentGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(objectMatrix)))
        {
            glm::vec3 translation, scale, skew;
            glm::quat rotation;
            glm::vec4 perspective;

            if (glm::decompose(objectMatrix, scale, rotation, translation, skew, perspective))
            {
                transform.position.x = translation.x;
                transform.position.y = translation.y;
                transform.scale.x = scale.x;
                transform.scale.y = scale.y;
                transform.rotation = glm::degrees(glm::eulerAngles(rotation).z);
            }
        }
    }

    void UseImGui::SpriteEditorWindow()
    {
        ImGui::Begin("Sprite Editor");
        ImGui::Text("Sprite Editor Contents");

        // Static animation variables
        static GLuint texture = 0;
        static char clipName[128] = "";
        static int textureWidth = 0;
        static int textureHeight = 0;
        static int totalFrames = 1;
        static int pixelSize[2] = {64, 64};
        static float pivot[2] = {0.5f, 0.5f}; // center pivot by default
        static int pixelPerUnit = 100;
        static float frameTime = 0.05f;
        static bool looping = true;

        // Editor variables
        static bool showTools = false;
        static bool showGrid = false;
        static bool showPreview = true;
        static bool isAnimation = false;
        static std::string spriteName;

        std::shared_ptr<AssetManager> amRef = ECS::GetInstance().GetSystem<AssetManager>();

        // Back to selection
        if (!m_SpriteFlag)
        {
            if (ImGui::Button("Back"))
            {
                m_SpriteFlag = true;
                texture = 0;
                textureWidth = 0;
                textureHeight = 0;
            }

            ImGui::SameLine();
            
            if (!isAnimation)
            {
                if (ImGui::Button("Replace object's texture"))
                {
                    auto GOs = GameObjectManager::GetInstance().GetAllGOs();
                    if (GOs[m_global_selected]->HasComponent<SpriteRender>())
                    {
                        auto &sr = GOs[m_global_selected]->GetComponent<SpriteRender>();
                        sr.texturePath = m_TexturePath;
                    }
                }
            }
            
            ImGui::Checkbox("Animation",&isAnimation);
        }
        
        if (m_SpriteFlag) // When the user click onto a texture file, it'll display in editor window
        {
            ImGui::Separator();
            
            // Check if atlas texture or standalone
            if (amRef->isAtlasTexture(m_TexturePath))
            {
                // Display individual sprite inside the atlas
                // The name of all individual sprite are inside the json file, 1:1 to texture file name
                static int selectedSpriteIndex = -1;
                int i = 0;
                std::vector<std::string> payload = amRef->getAtlasJSONData(m_TexturePath);
                for (const auto& basic_string : payload)
                {
                    bool isSelected = selectedSpriteIndex == i;
                    if (ImGui::Selectable(basic_string.c_str(), isSelected))
                    {
                        m_SpriteFlag = false; // disable the list
                        selectedSpriteIndex = i;
                        spriteName = basic_string;
                        showPreview = true;
                        texture = amRef->getTexture(m_TexturePath)->ID;
                        textureWidth = static_cast<int>(amRef->spriteData[basic_string].spriteSheetDimension.x);
                        textureHeight = static_cast<int>(amRef->spriteData[basic_string].spriteSheetDimension.y);
                    }

                    // Enable dragging of sprite to sprite renderer
                    if (ImGui::BeginDragDropSource())
                    {
                        ImGui::SetDragDropPayload("ASSET_PATH", spriteName.c_str(),spriteName.size() + 1);
                        ImGui::Text("Drag: %s", spriteName.c_str());
                        ImGui::EndDragDropSource();
                    }

                    i++;
                }
            }
            else if (amRef->ifTextureExists(m_TexturePath))
            {
                
            }
        }

        // Preview Display Image
        // Calculate the aspect ratio of the image
        if (showPreview && !isAnimation)
        {
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

            ImVec2 UV0 = ImVec2(amRef->spriteData[spriteName].uv.uMin,amRef->spriteData[spriteName].uv.vMax);
            ImVec2 UV1 = ImVec2(amRef->spriteData[spriteName].uv.uMax,amRef->spriteData[spriteName].uv.vMin);
            
            ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(displayWidth, displayHeight), UV0, UV1);
        }
        
        if (!isAnimation)
        {
            ImGui::End();
            return;
        }

        ImGui::Separator();

        // Display the texture details
        ImGui::Text("File = %s", spriteName.c_str());
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

        // Input for PPU
        ImGui::InputInt("Pixel Per Unit", &pixelPerUnit);
        pixelPerUnit = std::max(10, pixelPerUnit);

        // Input for pivot
        ImGui::InputFloat2("Pivot", pivot); // Pivot point normalized
        pivot[0] = std::max(0.0f, std::min(1.0f, pivot[0]));
        pivot[1] = std::max(0.0f, std::min(1.0f, pivot[1]));

        // Input for frame time
        ImGui::InputFloat("Frame Time", &frameTime, 0, 0, "%.3f");
        frameTime = std::max(0.001f, frameTime); // max 0.001s

        // Input for looping
        ImGui::Checkbox("Looping", &looping);

        if (ImGui::Button("Export Clip"))
        {
            std::string sClipName = clipName;
            if (sClipName.empty())
            {
                ImGui::OpenPopup("Missing Clip Name");
            }
            else
            {
                // Save the metadata to a file
                // Get file name to save
                rapidjson::Document document;
                document.SetObject();
                rapidjson::Document::AllocatorType &allocator = document.GetAllocator();

                rapidjson::Value textureMetaData(rapidjson::kObjectType);
                std::string tempStr = clipName;
                textureMetaData.AddMember("KeyPath", rapidjson::Value(m_TexturePath.c_str(), allocator), allocator);
                textureMetaData.AddMember("ClipName", rapidjson::Value(tempStr.c_str(), allocator), allocator);
                textureMetaData.AddMember("PivotX", pivot[0], allocator);
                textureMetaData.AddMember("PivotY", pivot[1], allocator);
                textureMetaData.AddMember("PixelsPerUnit", pixelPerUnit, allocator);
                textureMetaData.AddMember("TotalFrames", totalFrames, allocator);
                textureMetaData.AddMember("PixelWidth", pixelSize[0], allocator);
                textureMetaData.AddMember("PixelHeight", pixelSize[1], allocator);
                textureMetaData.AddMember("TextureWidth", textureWidth, allocator);
                textureMetaData.AddMember("TextureHeight", textureHeight, allocator);
                textureMetaData.AddMember("FrameTime", frameTime, allocator);
                textureMetaData.AddMember("Looping", looping, allocator);
                // textureMetaData.AddMember("PixelPerUnit", pixelPerUnit, allocator);

                document.AddMember("TextureMeta", textureMetaData, allocator);
                // Serialize the texture
                std::filesystem::path path = m_TexturePath;
                path.replace_extension(".json");
                // std::string path = "../Assets/Textures/" + fileName + ".json";
                if (!Serialization::PushJSON(path.string(), document))
                {
                    UME_ENGINE_ERROR("Failed to save metadata to file: {0}", path.string());
                }
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Add Clip to GO"))
        {
            std::string sClipName = clipName;
            if (sClipName.empty())
            {
                ImGui::OpenPopup("Missing Clip Name");
            }
            else if (m_global_selected < 0)
            {
                ImGui::OpenPopup("NeverSelectObject");
            }
            else
            {
                auto GOs = GameObjectManager::GetInstance().GetAllGOs();
                if (GOs[m_global_selected]->HasComponent<Animation>())
                {
                    auto &anim = GOs[m_global_selected]->GetComponent<Animation>();
                    anim.clips[sClipName] = AnimationClip{
                        m_TexturePath, sClipName, Vec2(pivot[0], pivot[1]), pixelPerUnit, totalFrames, pixelSize[0], pixelSize[1], textureWidth, textureHeight, frameTime,
                        looping};
                    anim.SetAnimation(clipName);
                }
            }
        }

        if (ImGui::BeginPopup("NeverSelectObject"))
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Select an object first!");
            if (ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup(); // Close the popup when the button is pressed
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopup("Missing Clip Name"))
        {
            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Missing Clip Name, please enter a name");
            if (ImGui::Button("OK"))
            {
                ImGui::CloseCurrentPopup(); // Close the popup when the button is pressed
            }
            ImGui::EndPopup();
        }

        static bool isToggled = true; // This holds the state of the toggle
        static bool isPlaying = false;
        static float timeSinceLastFrame = 0.0f;
        static int currentFrame = 0;
        static ImVec2 uv0 = ImVec2(amRef->spriteData[spriteName].uv.uMin, amRef->spriteData[spriteName].uv.vMax);
        static ImVec2 uv1 = ImVec2(amRef->spriteData[spriteName].uv.uMax,amRef->spriteData[spriteName].uv.vMin);
        if (isToggled)
        {
            if (ImGui::Button("PLAY"))
            {
                isToggled = false; // Toggle OFF
                showGrid = false;
                isPlaying = true;
            }
        }
        else
        {
            if (ImGui::Button("PAUSE"))
            {
                isToggled = true; // Toggle ON
                isPlaying = false;
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("RESET"))
        {
            timeSinceLastFrame = 0.0f;
            currentFrame = 0;
            uv0 = ImVec2(amRef->spriteData[spriteName].uv.uMin, amRef->spriteData[spriteName].uv.vMax);
            uv1 = ImVec2(amRef->spriteData[spriteName].uv.uMax,amRef->spriteData[spriteName].uv.vMin);
            showGrid = true;
            isToggled = true;
            isPlaying = false;
        }

        // Animation
        if (isPlaying)
        {
            timeSinceLastFrame += static_cast<float>(g_FrameRateController.GetDeltaTime());
            if (timeSinceLastFrame >= frameTime)
            {
                currentFrame++;
                if (currentFrame >= totalFrames)
                    currentFrame = looping ? 0 : totalFrames - 1;
                timeSinceLastFrame = 0.0f;
            }

            // Handle the UV here
            UME_ENGINE_ASSERT(pixelSize[0] < textureWidth && pixelSize[1] < textureHeight,
                              "Pixel size is larger than texture size")
            int col = currentFrame % (textureWidth / pixelSize[0]);
            int row = currentFrame / (textureWidth / pixelSize[0]);

            float uvX = static_cast<float>(col) * static_cast<float>(pixelSize[0]) / static_cast<float>(textureWidth);
            float uvY = 1.0f - static_cast<float>(1 + row) * static_cast<float>(pixelSize[1]) / static_cast<float>(textureHeight);
            float uvWidth = static_cast<float>(pixelSize[0]) / static_cast<float>(textureWidth);
            float uvHeight = static_cast<float>(pixelSize[1]) / static_cast<float>(textureHeight);

            uv0.x = uvX;
            uv0.y = uvY + uvHeight;
            uv1.x = uvX + uvWidth;
            uv1.y = uvY;
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
        
        ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(displayWidth, displayHeight), uv0, uv1);

        // Draw the grid
        if (showGrid)
        {
            ImDrawList *drawList = ImGui::GetWindowDrawList();
        
            // scale the grid to match the resized image
            float scaledCellWidth = static_cast<float>(pixelSize[0]) / static_cast<float>(textureWidth) * displayWidth;
            float scaledCellHeight = static_cast<float>(pixelSize[1]) / static_cast<float>(textureHeight) *
                                     displayHeight;
        
            // UME_ENGINE_ASSERT(pixelSize[0] < textureWidth && pixelSize[1] < textureHeight,
            //                   "Pixel size is larger than texture size");
            pixelSize[0] = std::min(pixelSize[0], textureWidth);
            pixelSize[1] = std::min(pixelSize[1], textureHeight);
        
            int colums = textureWidth / pixelSize[0];
            //            int rows = textureHeight / pixelSize[1];
            int maxRows = (totalFrames + colums - 1) / colums;
        
            for (int i = 0; i < totalFrames; i++)
            {
                int col = i % colums;
                int row = i / colums;
                if (row >= maxRows)
                    break;
        
                drawList->AddRect(ImVec2(canvasPos.x + static_cast<float>(col) * scaledCellWidth, canvasPos.y +
                                                                                                      static_cast<float>(row) * scaledCellHeight),
                                  ImVec2(canvasPos.x + static_cast<float>(col + 1) * scaledCellWidth, canvasPos.y +
                                                                                                          static_cast<float>(row + 1) * scaledCellHeight),
                                  IM_COL32(255, 255, 255, 255));
        
                ImVec2 drawPivot = ImVec2{(canvasPos.x + static_cast<float>(col) * scaledCellWidth) + scaledCellWidth * pivot[0],
                                          (canvasPos.y + static_cast<float>(row) * scaledCellHeight) + scaledCellHeight * (1.0f - pivot[1])};
        
                float pivotMarkerSize = 4.0f;
                drawList->AddCircleFilled(drawPivot, pivotMarkerSize, IM_COL32(58, 143, 248, 255));
            }
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
        static const int maxHistorySize = 144;
        fpsHistory.push_back(fps);
        if (fpsHistory.size() > maxHistorySize)
        {
            fpsHistory.erase(fpsHistory.begin());
        }

        ImGui::Begin("Control Panel");
        ImGui::Text("FPS: %.2f", fps);
        ImGui::PlotLines("FPS History", fpsHistory.data(), static_cast<int>(fpsHistory.size()), 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
        ImGui::Text("Control Panel Contents");

        // Only show Play button when not in play mode
        if (es_current != ENGINE_STATES::ES_PLAY)
        {
            if (ImGui::Button("Play"))
            {
                UME_ENGINE_TRACE("Initializing Collision...");
                ECS::GetInstance().GetSystem<Collision>()->Init();
                UME_ENGINE_TRACE("Initializing dungeon manager...");
                ECS::GetInstance().GetSystem<DungeonManager>()->Init();
                // enemy
                ECS::GetInstance().GetSystem<EnemyManager>()->UpdateEnemyList();
                //audio
                if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
                {
                    if (GameObjectManager::GetInstance().GetGOByTag("AudioManager")->HasComponent<AudioManager>())
                    {
                        auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                        if (audioM.GetMusicIndex("BGM") != -1)
                        {
                            if (!ECS::GetInstance().GetSystem<Audio>()->GetInstance().IsMusicPlaying(audioM.GetMusicIndex("BGM")))
                            {
                                audioM.PlayMusic(audioM.GetMusicIndex("BGM"));
                            }
                        }
                    }
                }

                // Recompile scripts and display popup that its compiling. Remove popup when done
                if (ScriptingEngine::GetInstance().compile_flag)
                {
                    UME_ENGINE_INFO("Begin Script reloading");
                    ScriptingEngine::GetInstance().compile_flag = false;
                    ScriptingEngine::GetInstance().Reload();
                    // TODO: Compile runs on the main thread, hence imGUI cannot draw pop-up here...
                }

                if (!ScriptingEngine::ScriptHasError)
                {
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

                // Disable main menu screen
                ECS::GetInstance().GetSystem<InGameGUI>()->RemoveElement("main_menu");
                ECS::GetInstance().GetSystem<InGameGUI>()->RemoveElement("start_button");
            }
            ImGui::SameLine();
        }

        if (ImGui::Button("Stop"))
        {
            if (GameObjectManager::GetInstance().GetGOByTag("AudioManager"))
            {
                if (GameObjectManager::GetInstance().GetGOByTag("AudioManager")->HasComponent<AudioManager>())
                {
                    auto& audioM = GameObjectManager::GetInstance().GetGOByTag("AudioManager")->GetComponent<AudioManager>();
                    audioM.StopMusic(audioM.GetMusicIndex("BGM"));
                }
            }

            SceneManager::GetInstance().LoadSaveFile(SceneManager::GetInstance().GetCurrScene() + ".json");
            es_current = ENGINE_STATES::ES_ENGINE;
            UME_ENGINE_INFO("Simulation (Game is stopping) stopped");
        }

        ImGui::End();
    }

    void UseImGui::ShowErrorPopup(const std::string &errorMessage)
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
    void UseImGui::LoadContents(const std::string &directory)
    {
        // Clear the previous asset list
        assetFiles.clear();
        folderNames.clear();

        // Load assets from the specified directory
        for (const auto &entry : std::filesystem::directory_iterator(directory))
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

    void UseImGui::
    ContentBrowser(char *filePath)
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
                    m_TexturePath = fullPath;
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
        for (const auto &entry : std::filesystem::directory_iterator(scenesDir))
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

        // find the next sceneName
        for (const auto &scene : sceneFiles)
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
                // newSceneName[0] = '\0'; // Clear the input field
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
    void UseImGui::DisplayEntityDetails(GameObject &obj)
    {
        ImGui::Separator();
        ImGui::Text("Entity Details");
        ImGui::BulletText("ID: %d", obj.GetInstanceID());
        ImGui::BulletText("Name: %s", obj.GetName().c_str());
        ImGui::BulletText("Tag: %s", obj.GetTag().c_str());
        ImGui::Separator();

        // Display Transform Component (Green)
        if (obj.HasComponent<Transform>())
        {
            // Set background color for the tree node
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 1.0f, 0.0f, 0.2f));        // Light green background
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 1.0f, 0.0f, 0.4f)); // Darker green when hovered
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 1.0f, 0.0f, 0.6f));  // Even darker green when active

            // TreeNode with custom background color and padding
            if (ImGui::TreeNodeEx("Transform Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                // Display transform component details with some padding
                ImGui::Text("Position: (%.2f, %.2f)", obj.GetComponent<Transform>().position.x, obj.GetComponent<Transform>().position.y);
                ImGui::Text("Rotation: %.2f", obj.GetComponent<Transform>().rotation);
                ImGui::Text("Scale: (%.2f, %.2f)", obj.GetComponent<Transform>().scale.x, obj.GetComponent<Transform>().scale.y);

                ImGui::TreePop();
            }

            ImGui::PopStyleColor(3);
        }

        // Display Rigidbody2D Component (Blue)
        if (obj.HasComponent<Rigidbody2D>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.5f, 1.0f, 0.2f));        // Light blue background
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.5f, 1.0f, 0.4f)); // Darker blue when hovered
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.5f, 1.0f, 0.6f));  // Even darker blue when active

            if (ImGui::TreeNodeEx("Rigidbody2D Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                Rigidbody2D &rb = obj.GetComponent<Rigidbody2D>();
                ImGui::Text("Position: (%.2f, %.2f)", rb.position.x, rb.position.y);
                ImGui::Text("Velocity: (%.2f, %.2f)", rb.velocity.x, rb.velocity.y);
                ImGui::Text("Acceleration: (%.2f, %.2f)", rb.acceleration.x, rb.acceleration.y);

                ImGui::TreePop();
            }

            ImGui::PopStyleColor(3);
        }

        // Display BoxCollider2D Component (Orange)
        if (obj.HasComponent<BoxCollider2D>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 0.5f, 0.0f, 0.2f));        // Light orange background
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 0.5f, 0.0f, 0.4f)); // Darker orange when hovered
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 0.5f, 0.0f, 0.6f));  // Even darker orange when active

            if (ImGui::TreeNodeEx("BoxCollider2D Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                BoxCollider2D &collider = obj.GetComponent<BoxCollider2D>();
                ImGui::Text("Is Trigger: %s", collider.is_trigger ? "True" : "False");

                ImGui::TreePop();
            }

            ImGui::PopStyleColor(3);
        }

        // Display SpriteRender Component (Purple)
        if (obj.HasComponent<SpriteRender>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.6f, 0.4f, 1.0f, 0.2f));        // Light purple background
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.6f, 0.4f, 1.0f, 0.4f)); // Darker purple when hovered
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.6f, 0.4f, 1.0f, 0.6f));  // Even darker purple when active

            if (ImGui::TreeNodeEx("SpriteRender Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                SpriteRender &sprite = obj.GetComponent<SpriteRender>();
                ImGui::Text("Sprite Path: %s", sprite.texturePath.c_str());
                ImGui::Text("Shape: %d", sprite.shape);

                ImGui::TreePop();
            }

            ImGui::PopStyleColor(3);
        }

        if (obj.HasComponent<Animation>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f,0.4f,0.4f,0.2f));        // Light Red background
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered,ImVec4(0.6f,0.0f,0.0f,0.4f));  // Darker red when hovered
            ImGui::PushStyleColor(ImGuiCol_HeaderActive,ImVec4(0.4f,0.0f,0.0f,0.6f));    // Even darker red when active

            if (ImGui::TreeNodeEx("Animation Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                Animation &anim = obj.GetComponent<Animation>();
                for (auto& element : anim.clips)
                {
                    ImGui::Text("Clip name: %s", element.second.name.c_str());
                    ImGui::Text("PPU: %d", element.second.pixelsPerUnit);
                    ImGui::Text("Pivot: %f , %f", element.second.pivot.x, element.second.pivot.y);
                    ImGui::Text("Speed: %f", element.second.frame_time);
                    
                    ImGui::Separator();
                }

                ImGui::TreePop();
            }

            ImGui::PopStyleColor(3);
        }

        // Display Script Component (Yellow)
        if (obj.HasComponent<Script>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(1.0f, 1.0f, 0.0f, 0.2f));        // Light yellow background
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 1.0f, 0.0f, 0.4f)); // Darker yellow when hovered
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(1.0f, 1.0f, 0.0f, 0.6f));  // Even darker yellow when active

            if (ImGui::TreeNodeEx("Script Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                Script &script = obj.GetComponent<Script>();
                ImGui::Text("Script Path: %s", script.scriptPath.c_str());
                ImGui::Text("Class Name: %s", script.scriptName.c_str());

                ImGui::TreePop();
            }

            ImGui::PopStyleColor(3);
        }

        // Display Audio Component
        if (obj.HasComponent<AudioManager>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.0f, 0.5f, 0.5f, 0.2f));  
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.0f, 0.5f, 0.5f, 0.4f)); 
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.0f, 0.5f, 0.5f, 0.6f));

            if (ImGui::TreeNodeEx("Audio Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                AudioManager& audio = obj.GetComponent<AudioManager>();
                for (auto& music : audio.music)
                {
                    ImGui::Text("Music Name: %s", music.audioName.c_str());
                    ImGui::Text("Music Path: %s", music.audioPath.c_str());
                }
                for (auto& sfx : audio.sfx)
                {
                    ImGui::Text("SFX Name: %s", sfx.audioName.c_str());
                    ImGui::Text("SFX Path: %s", sfx.audioPath.c_str());
                }
                //ImGui::Text("Audio Path: %s", audio.audioPath.c_str());
                //ImGui::Text("Audio Name: %s", audio.audioName.c_str());

                ImGui::TreePop();
            }

            ImGui::PopStyleColor(3);
        }

        if (obj.HasComponent<Enemy>())
        {
            ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 1.0f, 0.3f, 0.2f));        // Light green background
            ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 1.0f, 0.3f, 0.4f)); // Darker green when hovered
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.3f, 1.0f, 0.3f, 0.6f));  // Even darker green when active

            if (ImGui::TreeNodeEx("Enemy Component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth))
            {
                const auto& enemy = obj.GetComponent<Enemy>();

                ImGui::Text("Enemy Details:");
                ImGui::Text("Health: %.2f", enemy.health);
                ImGui::Text("Attack Power: %.2f", enemy.attackPower);
                ImGui::Text("Attack Range: %.2f", enemy.attackRange);
                ImGui::Text("Speed: %.2f", enemy.speed);

                ImGui::Text("Position:");
                ImGui::Text("X: %.2f", enemy.posX);
                ImGui::Text("Y: %.2f", enemy.posY);

                ImGui::Text("Target:");
                ImGui::Text("Target X: %.2f", enemy.targetX);
                ImGui::Text("Target Y: %.2f", enemy.targetY);

                ImGui::Text("State: %s",
                    enemy.state == Enemy::ROAM ? "ROAM" :
                    enemy.state == Enemy::CHASE ? "CHASE" :
                    enemy.state == Enemy::ATTACK ? "ATTACK" : "DEAD");

                ImGui::Text("Type: %s",
                    enemy.type == Enemy::FISH ? "FISH" :
                    enemy.type == Enemy::WORM ? "WORM" : "DEFAULT");

                ImGui::Text("Is Dead: %s", enemy.isDead ? "Yes" : "No");
                ImGui::Text("Is Collide: %s", enemy.isCollide ? "Yes" : "No");

                ImGui::TreePop();
            }

            ImGui::PopStyleColor(3);
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
    void UseImGui::DisplayEntitySelectionCombo(int &selectedEntityIndex)
    {
        // auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();
        auto gameObjects = GameObjectManager::GetInstance().GetAllGOs();

        // Store the names of entities in a vector of strings to keep them in memory
        std::vector<std::string> entityNames;
        for (const auto &obj : gameObjects)
        {
            entityNames.push_back(std::to_string(obj->GetInstanceID()) + ": " + obj->GetName());
        }

        // Check if there are any entities and set the selected index
        if (!entityNames.empty() && selectedEntityIndex == -1)
        {
            selectedEntityIndex = 0; // Default to the first item if nothing is selected
        }

        // Create a vector of const char* pointers to each name in entityNames
        std::vector<const char *> entityNamePointers;
        for (const auto &name : entityNames)
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
    void UseImGui::RemoveSelectedEntity(int &selectedEntityIndex)
    {
        // auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();
        auto gameObjects = GameObjectManager::GetInstance().GetAllGOs();
        if (selectedEntityIndex >= 0 && static_cast<size_t>(selectedEntityIndex) < gameObjects.size())
        {
            auto &entityToDelete = gameObjects[selectedEntityIndex];
            GameObjectManager::GetInstance().DestroyObject(entityToDelete->GetInstanceID());
            selectedEntityIndex = -1;
        }
    }

    void UseImGui::AddComponentUI(GameObject *selectedObject, bool &modified)
    {
        ImGui::Text("Component Management");

        static int selectedComponentIndex = 0;
        const char *availableComponents[] = {
            "Rigidbody2D",
            "BoxCollider2D",
            "SpriteRender",
            "Script",
            "Animation",
            "PlayerController",
            "Audio",
            "EnemyController"
        };

        ImGui::Text("Add Component");
        ImGui::Combo("##ComponentCombo", &selectedComponentIndex, availableComponents,
                     IM_ARRAYSIZE(availableComponents));

        bool isAnyPopupOpen = ImGui::IsPopupOpen("Invalid Player Tag") || ImGui::IsPopupOpen("Invalid Enemy Tag");

        // Disable object picking when any modal is open
        if (isAnyPopupOpen)
        {
            ECS::GetInstance().GetSystem<Renderer>()->currentMode = Renderer::InteractionMode::NO_STATE;
        }
        else
        {
        }

        if (ImGui::Button("Add Selected Component"))
        {
            switch (selectedComponentIndex)
            {
            case 0: // Rigidbody2D
                if (!selectedObject->HasComponent<Rigidbody2D>())
                {
                    selectedObject->AddComponent<Rigidbody2D>(Rigidbody2D{});
                    modified = true;
                }
                break;
            case 1: // BoxCollider2D
                if (!selectedObject->HasComponent<BoxCollider2D>())
                {
                    selectedObject->AddComponent<BoxCollider2D>(BoxCollider2D{});
                    modified = true;
                }
                break;
            case 2: // SpriteRender
                if (!selectedObject->HasComponent<SpriteRender>())
                {
                    selectedObject->AddComponent<SpriteRender>(SpriteRender{});
                    modified = true;
                }
                break;
            case 3: // Script
                if (!selectedObject->HasComponent<Script>())
                {
                    selectedObject->AddComponent<Script>(Script{});
                    modified = true;
                }
                break;
            case 4: // Animation
                if (!selectedObject->HasComponent<Animation>())
                {
                    selectedObject->AddComponent<Animation>(Animation{});
                    modified = true;
                }
                break;
            case 5: // Player
                if (selectedObject->GetTag() == "Player" && !selectedObject->HasComponent<Player>())
                {
                    selectedObject->AddComponent<Player>(Player{});
                    modified = true;
                }
                else if (selectedObject->GetTag() != "Player")
                {
                    ImGui::OpenPopup("Invalid Player Tag");
                }
                break;
            case 6: // Audio
                if (!selectedObject->HasComponent<AudioManager>())
                {
                    selectedObject->AddComponent<AudioManager>(AudioManager{});
                    modified = true;
                }
                break;
            case 7:
                if (selectedObject->GetTag() == "Enemy" && !selectedObject->HasComponent<Enemy>())
                {
                    selectedObject->AddComponent<Enemy>(Enemy(0.0f, 0.0f, Enemy::DEFAULT, selectedObject->GetInstanceID()));
                    modified = true;
                }
                else if (selectedObject->GetTag() != "Enemy")
                {
                    // Show popup instead of console message
                    ImGui::OpenPopup("Invalid Enemy Tag");
                }
                break;
            default:
                break;
            }
        }

        // Modal for invalid tag
        if (ImGui::BeginPopupModal("Invalid Player Tag", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("The selected object is not tagged as 'Player'. Cannot add the Player component.");

            // Change text color and font weight for emphasis
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
            ImGui::Text("YOU CAN ONLY HAVE 1 PLAYER");
            ImGui::PopStyleColor();

            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // Modal for invalid tag
        if (ImGui::BeginPopupModal("Invalid Enemy Tag", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("The selected object is not tagged as 'Enemy'. Cannot add the Enemy component.");
            if (ImGui::Button("Close"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    /*!***********************************************************************
    \brief
     Removes components from a selected game object. Provides a UI for removing specific components
     that exist in the selected game object.

    \param[in] selectedObject
     A pointer to the selected GameObject from which components may be removed.

    \param[in/out] modified
     A boolean reference indicating if the GameObject was modified.
     It is set to true if a component was removed, otherwise remains unchanged.

    *************************************************************************/
    void UseImGui::RemoveComponentUI(GameObject *selectedObject, bool &modified)
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

            if (selectedObject->HasComponent<Animation>())
            {
                if (ImGui::Button("Remove Animation Component"))
                {
                    selectedObject->RemoveComponent<Animation>();
                    modified = true;
                }
                ImGui::Spacing();
            }

            if (selectedObject->HasComponent<Enemy>())
            {
                if (ImGui::Button("Remove Enemy Component"))
                {
                    selectedObject->RemoveComponent<Enemy>();
                    modified = true;
                }
                ImGui::Spacing();
            }

            if (selectedObject->HasComponent<Player>())
            {
                if (ImGui::Button("Remove Player Component"))
                {
                    selectedObject->RemoveComponent<Player>();
                    modified = true;
                }
                ImGui::Spacing();
            }

            if (selectedObject->HasComponent<AudioManager>())
            {
                if (ImGui::Button("Remove Audio Component"))
                {
                    selectedObject->RemoveComponent<AudioManager>();
                    modified = true;
                }
                ImGui::Spacing();
            }
        }

        // ImGui::Separator(); // Optional separator after the collapsible section
    }

    /*!***********************************************************************
    \brief
        Displays a modal popup informing the user that an invalid audio file type was selected.
        The popup shows an error message and prevents object picking while it's open.

    \param[in] None
        This function does not require any input parameters.

    \param[in/out] None
        This function does not modify any input/output parameters.

    \return
        None
        This function does not return a value.
    *************************************************************************/
    void UseImGui::ShowInvalidAudioFileTypePopup()
    {
        if (ImGui::BeginPopupModal("InvalidAudioFileType", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Invalid file type selected!");
            ImGui::Spacing();

            // Emphasized error message in red
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red color
            ImGui::Text("Only .wav / .mp3 / .ogg files are allowed!");
            ImGui::PopStyleColor();
            ImGui::Spacing();

            if (ImGui::Button("OK", ImVec2(120, 0))) // Add a fixed width for consistency
            {
                ImGui::CloseCurrentPopup(); // Close the modal when the button is pressed
            }

            ImGui::SetItemDefaultFocus(); // Focus the "OK" button
            ImGui::EndPopup();

            // Disable object picking while the error modal is open
            ECS::GetInstance().GetSystem<Renderer>()->currentMode = Renderer::InteractionMode::NO_STATE;
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
    void UseImGui::EditEntityProperties(GameObject *selectedObject, bool &modified)
    {
        if (!selectedObject)
            return;

        // Store whether the rename mode is enabled
        static bool isRenaming = false;
        static bool isTagRenaming = false;

        // Button to enable/disable renaming
        if (ImGui::Button(isRenaming ? "Confirm Rename" : "Rename Entity Name") && !isTagRenaming)
        {
            isRenaming = !isRenaming; // Toggle renaming mode
        }

        ImGui::SameLine();

        if (ImGui::Button(isTagRenaming ? "Confirm Rename Tag" : "Rename Entity Tag") && !isRenaming)
        {
            isTagRenaming = !isTagRenaming; // Toggle renaming mode
        }

        // Editable name input field, only enabled if renaming is allowed
        char nameBuffer[256];
        std::strncpy(nameBuffer, selectedObject->GetName().c_str(), sizeof(nameBuffer));
        nameBuffer[sizeof(nameBuffer) - 1] = '\0'; // Ensure null termination

        char tagBuffer[256];
        std::strncpy(tagBuffer, selectedObject->GetTag().c_str(), sizeof(tagBuffer));
        tagBuffer[sizeof(tagBuffer) - 1] = '\0'; // Ensure null termination

        // Disable input filed when not tag renaming
        if (isTagRenaming && !isRenaming)
        {
            if (ImGui::InputText("Tag name", tagBuffer, sizeof(tagBuffer)))
            {
                selectedObject->SetTag(std::string(tagBuffer));
                modified = true; // Flag as modified
            }
        }

        // Disable input field when not renaming
        if (isRenaming && !isTagRenaming)
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
            // ImGui::Text("Object Name: %s", selectedObject->GetName().c_str());
        }

        ImGui::Text("Editing properties of: ");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", selectedObject->GetName().c_str());

        // Checkbox to toggle between sliders and input fields
        static bool useSliders = true;
        ImGui::Checkbox("Use Sliders", &useSliders);

        ImGui::Separator();

        // Allow adding components if not in Play mode
        if (es_current != ENGINE_STATES::ES_PLAY)
        {
            AddComponentUI(selectedObject, modified);
        }

        ImGui::Separator();

        // Allow removing components if not in Play mode
        if (es_current != ENGINE_STATES::ES_PLAY)
        {
            RemoveComponentUI(selectedObject, modified);
        }

        ImGui::Separator();

        if (es_current == ENGINE_STATES::ES_PLAY)
        {
            return;
        }

        // Transform Component (always editable)
        if (selectedObject->HasComponent<Transform>())
        {
            if (ImGui::CollapsingHeader("Transform"))
            {
                Transform &transform = selectedObject->GetComponent<Transform>();

                // Position
                ImGui::Text("Position");
                if (useSliders)
                {
                    //if (ImGui::SliderFloat2("##PositionSlider", &transform.position.x, -800.0f, 1500.0f))
                    if(ImGui::SliderFloat3("##PositionSlider", &transform.position.x,-800.0f,1500.f))
                        modified = true;
                }
                else
                {
                    if (ImGui::InputFloat3("##PositionInput", &transform.position.x))
                        modified = true;
                }

                // Rotation
                ImGui::Text("Rotation");
                if (useSliders)
                {
                    if (ImGui::SliderFloat("##RotationSlider", &transform.rotation, -180.0f, 180.0f))
                        modified = true;
                }
                else
                {
                    if (ImGui::InputFloat("##RotationInput", &transform.rotation))
                        modified = true;
                }

                // Scale
                ImGui::Text("Scale");
                if (useSliders)
                {
                    if (ImGui::SliderFloat2("##ScaleSlider", &transform.scale.x, 70.f, 250.0f))
                        modified = true;
                }
                else
                {
                    if (ImGui::InputFloat2("##ScaleInput", &transform.scale.x))
                        modified = true;
                }
            }
        }

        // Rigidbody2D Component (always editable)
        if (selectedObject->HasComponent<Rigidbody2D>())
        {
            if (ImGui::CollapsingHeader("Rigidbody2D"))
            {
                Rigidbody2D &rb = selectedObject->GetComponent<Rigidbody2D>();

                // Velocity
                ImGui::Text("Velocity");
                if (useSliders)
                {
                    if (ImGui::SliderFloat2("##VelocitySlider", &rb.velocity.x, -50.0f, 50.0f))
                        modified = true;
                }
                else
                {
                    if (ImGui::InputFloat2("##VelocityInput", &rb.velocity.x))
                        modified = true;
                }

                // Mass
                ImGui::Text("Mass");
                if (useSliders)
                {
                    if (ImGui::SliderFloat("##MassSlider", &rb.mass, 0.1f, 100.0f))
                        modified = true;
                }
                else
                {
                    if (ImGui::InputFloat("##MassInput", &rb.mass))
                        modified = true;
                }

                // Force
                ImGui::Text("Force");
                if (useSliders)
                {
                    if (ImGui::SliderFloat("##ForceSlider", &rb.force.x, 0.1f, 100.0f))
                        modified = true;
                }
                else
                {
                    if (ImGui::InputFloat("##ForceInput", &rb.force.x))
                        modified = true;
                }

                // Use Gravity Checkbox
                ImGui::Text("Use Gravity");
                if (ImGui::Checkbox("##UseGravityCheckbox", &rb.use_gravity))
                    modified = true;

                // Is Kinematic Checkbox
                ImGui::Text("Is Kinematic");
                if (ImGui::Checkbox("##IsKinematicCheckbox", &rb.is_kinematic))
                    modified = true;
            }
        }

        // BoxCollider2D Component (always editable)
        if (selectedObject->HasComponent<BoxCollider2D>())
        {
            if (ImGui::CollapsingHeader("BoxCollider2D"))
            {
                BoxCollider2D &collider = selectedObject->GetComponent<BoxCollider2D>();

                ImGui::Text("Is Trigger Collider Box");
                if (ImGui::Checkbox("##UseColliderCheckbox", &collider.is_trigger))
                    modified = true;
            }
        }

        // SpriteRender Component (always editable)
        if (selectedObject->HasComponent<SpriteRender>())
        {
            if (ImGui::CollapsingHeader("SpriteRender"))
            {
                SpriteRender &sprite = selectedObject->GetComponent<SpriteRender>();

                ImGui::Text("Texture Path");

                // Extract the filename from the full path
                std::string filename = std::filesystem::path(sprite.texturePath).filename().string();

                char texturePathBuffer[256];
                strncpy(texturePathBuffer, filename.c_str(), sizeof(texturePathBuffer));
                texturePathBuffer[sizeof(texturePathBuffer) - 1] = '\0'; // Ensure null termination

                ImGui::BeginDisabled(true); // Disable the input box while we use drag-and-drop
                ImGui::InputText("##TexturePathInput", texturePathBuffer, sizeof(texturePathBuffer));
                ImGui::EndDisabled();

                // Check for drag-and-drop
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                    {
                        std::string spriteName = std::string(static_cast<const char*>(payload->Data));
                        sprite.texturePath = spriteName; // update the component
                        filename = spriteName; // update displayed information
                        strncpy(texturePathBuffer,spriteName.c_str(), sizeof(texturePathBuffer));
                        texturePathBuffer[sizeof(texturePathBuffer) - 1] = '\0'; // Ensure null termination
                        modified = true;
                        
                        // std::string draggedPath = std::string(static_cast<const char*>(payload->Data));
                        // std::filesystem::path filePath(draggedPath);
                        // std::string extension = filePath.extension().string();

                        // if (extension == ".png")
                        // {
                        //     // Valid file type, update texture path
                        //     sprite.texturePath = draggedPath; // Store full path
                        //     filename = filePath.filename().string(); // Update displayed filename
                        //     strncpy(texturePathBuffer, filename.c_str(), sizeof(texturePathBuffer));
                        //     texturePathBuffer[sizeof(texturePathBuffer) - 1] = '\0'; // Ensure null termination
                        //     modified = true;
                        // }
                        // else
                        // {
                        //     // Invalid file type, show error feedback
                        //     ImGui::OpenPopup("InvalidTextureFileType");
                        // }
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::Text("Sortting Layer");
                ImGui::InputInt("##Sorting Layer", &sprite.layer);

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

        // comments
        //  Script Component, drag-and-drop for script path (always editable)
        if (selectedObject->HasComponent<Script>())
        {
            if (ImGui::CollapsingHeader("Script"))
            {
                Script &script = selectedObject->GetComponent<Script>();
                ImGui::Text("Script Path");

                char scriptPathBuffer[256];
                strncpy(scriptPathBuffer, script.scriptPath.c_str(), sizeof(scriptPathBuffer));
                scriptPathBuffer[sizeof(scriptPathBuffer) - 1] = '\0'; // Ensure null termination

                ImGui::BeginDisabled(true); // Disable input field while using drag-and-drop
                ImGui::InputText("##ScriptPathInput", scriptPathBuffer, sizeof(scriptPathBuffer));
                ImGui::EndDisabled();

                if (es_current != ENGINE_STATES::ES_PLAY)
                {
                    // Check for drag-and-drop
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                        {
                            std::string draggedScript = std::string((const char *)payload->Data);
                            std::filesystem::path filePath(draggedScript);
                            std::string extension = filePath.extension().string();

                            // Validate the file type (assuming only .cs scripts are allowed)
                            if (extension == ".cs")
                            {
                                // Valid file type, update script path
                                strncpy(scriptPathBuffer, draggedScript.c_str(), sizeof(scriptPathBuffer));
                                scriptPathBuffer[sizeof(scriptPathBuffer) - 1] = '\0'; // Ensure null termination
                                script.scriptPath = draggedScript;
                                script.scriptName = filePath.stem().string();
                                MonoObject *newScript = ScriptingEngine::GetInstance().InstantiateClientClass(
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
        }

        // Animation Component
        if (selectedObject->HasComponent<Animation>())
        {
            auto &animation = selectedObject->GetComponent<Animation>();
            if (animation.clips.find("") != animation.clips.end())
                animation.clips.clear();
            // TODO: Drag and drop texture to animation should pull the metadata
            if (ImGui::CollapsingHeader("Animation"))
            {
                // Quick fix empty animation clip

                
                if (!animation.clips.empty())
                {
                    if (ImGui::TreeNode("Clips"))
                    {
                        for (auto &clip : animation.clips)
                        {
                            if (ImGui::Selectable(clip.second.name.c_str()))
                            {
                                animation.currentClip = clip.second.name;
                            }
                        }
                        ImGui::TreePop();
                    }
                }
                static char currentClipBuffer[256] = "";
                strncpy(currentClipBuffer, animation.currentClip.c_str(), sizeof(currentClipBuffer));
                ImGui::BeginDisabled(true);
                ImGui::InputText("CurrentClip", currentClipBuffer, IM_ARRAYSIZE(currentClipBuffer));
                ImGui::EndDisabled();

                if (ImGui::Button("Clear all Clips"))
                {
                    animation.clips.clear();
                    animation.currentClip = "";
                }
                ImGui::Checkbox("Looping", &animation.clips[animation.currentClip].looping);
            }
        }

        if (selectedObject->HasComponent<Player>())
        {
            if (ImGui::CollapsingHeader("Player"))
            {
                auto &player = selectedObject->GetComponent<Player>();
                ImGui::Text("Player Component");
                ImGui::InputInt("Current Health", &player.currentHealth);
                ImGui::InputInt("Combo Damage", &player.comboDamage);
            }
        }

        if (selectedObject->HasComponent<Enemy>())
        {
            if (ImGui::CollapsingHeader("Enemy"))
            {
                auto& enemy = selectedObject->GetComponent<Enemy>();
                ImGui::Text("Enemy Component");

                ImGui::Text("Type:");
                int type = static_cast<int>(enemy.type);
                if (ImGui::Combo("Type", &type, "FISH\0WORM\0DEFAULT\0"))
                {
                    enemy.type = static_cast<Enemy::EnemyTypes>(type);
                }
            }
        }

        if (selectedObject->HasComponent<AudioManager>())
        {
            if (ImGui::CollapsingHeader("Audio"))
            {
                AudioManager& audio = selectedObject->GetComponent<AudioManager>();
                ImGui::Text("Audio");

                // Music Section as TreeNode
                if (ImGui::TreeNode("Music")) {
                    static char musicPath[256] = "";

                    // Add Music Button
                    if (ImGui::Button("Add Music")) {
                        // Add a new music entry with default values (empty name and path)
                        audio.music.push_back({ "", "" });  // Add a new entry with empty name and path
                    }

                    // List existing music
                    for (size_t i = 0; i < audio.music.size(); ++i) {
                        ImGui::PushID(static_cast<int>(i));
                        if (ImGui::TreeNode(("Music " + std::to_string(i)).c_str())) {
                            char newName[128] = "";
                            char newPath[256] = "";
                            strcpy(newName, audio.music[i].audioName.c_str());

                            // Display current music path with drag-and-drop for existing music
                            std::string filename = std::filesystem::path(audio.music[i].audioPath).filename().string();
                            strncpy(newPath, filename.c_str(), sizeof(newPath));
                            newPath[sizeof(newPath) - 1] = '\0';

                            // Editable name
                            if (ImGui::InputText("Music Name", newName, sizeof(newName))) {
                                // Update name in the audio manager
                                audio.music[i].audioName = newName;
                            }

                            ImGui::BeginDisabled(true);
                            ImGui::InputText("Music Path", newPath, sizeof(newPath), ImGuiInputTextFlags_ReadOnly);
                            ImGui::EndDisabled();

                            // Drag and Drop for Music Path (Existing Entry)
                            if (es_current != ENGINE_STATES::ES_PLAY) {
                                if (ImGui::BeginDragDropTarget()) {
                                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
                                        std::string draggedAudio = std::string((const char*)payload->Data);
                                        std::filesystem::path filePath(draggedAudio);
                                        std::string extension = filePath.extension().string();

                                        if (extension == ".wav" || extension == ".mp3" || extension == ".ogg") {
                                            // Update the path of the current music entry
                                            audio.music[i].audioPath = draggedAudio;
                                            ECS::GetInstance().GetSystem<Audio>()->GetInstance().LoadSound(static_cast<int>(i), audio.music[i].audioPath.c_str(), "Music");
                                        }
                                        else {
                                            ImGui::OpenPopup("InvalidAudioFileType");
                                        }
                                    }
                                    ImGui::EndDragDropTarget();
                                }

                                ShowInvalidAudioFileTypePopup();
                            }

                            // Add Play and Stop buttons for Music
                            if (ImGui::Button("Play")) {
                                audio.PlayMusic(static_cast<int>(i)); // Assuming PlayMusic takes an index or path as argument
                            }

                            ImGui::SameLine();

                            if (ImGui::Button("Stop")) {
                                audio.StopMusic(static_cast<int>(i)); // Assuming StopMusic takes an index or path as argument
                            }

                            ImGui::SameLine();

                            // Delete Music Button
                            if (ImGui::Button("Delete Music")) {
                                // Remove music from the list
                                audio.RemoveSoundFromMusic(static_cast<int>(i));
                            }

                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }

                    ImGui::TreePop();  // Close Music section
                }

                if (ImGui::TreeNode("SFX")) {
                    static char sfxPath[256] = "";

                    // Add SFX Button
                    if (ImGui::Button("Add SFX")) {
                        // Add a new SFX entry with default values (empty name and path)
                        audio.sfx.push_back({ "", "" });  // Add a new entry with empty name and path
                    }

                    // List existing SFX
                    for (size_t i = 0; i < audio.sfx.size(); ++i) {
                        ImGui::PushID(static_cast<int>(i));
                        if (ImGui::TreeNode(("SFX " + std::to_string(i)).c_str())) {
                            char newName[128] = "";
                            char newPath[256] = "";
                            strcpy(newName, audio.sfx[i].audioName.c_str());

                            // Display current SFX path with drag-and-drop for existing SFX
                            std::string filename = std::filesystem::path(audio.sfx[i].audioPath).filename().string();
                            strncpy(newPath, filename.c_str(), sizeof(newPath));
                            newPath[sizeof(newPath) - 1] = '\0';

                            // Editable name
                            if (ImGui::InputText("SFX Name", newName, sizeof(newName))) {
                                // Update name in the audio manager
                                audio.sfx[i].audioName = newName;
                            }

                            ImGui::BeginDisabled(true);
                            ImGui::InputText("SFX Path", newPath, sizeof(newPath), ImGuiInputTextFlags_ReadOnly);
                            ImGui::EndDisabled();

                            // Drag and Drop for SFX Path (Existing Entry)
                            if (es_current != ENGINE_STATES::ES_PLAY) {
                                if (ImGui::BeginDragDropTarget())
                                {
                                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
                                        std::string draggedAudio = std::string((const char*)payload->Data);
                                        std::filesystem::path filePath(draggedAudio);
                                        std::string extension = filePath.extension().string();

                                        if (extension == ".wav" || extension == ".mp3" || extension == ".ogg") {
                                            // Update the path of the current SFX entry
                                            audio.sfx[i].audioPath = draggedAudio;
                                            ECS::GetInstance().GetSystem<Audio>()->GetInstance().LoadSound(static_cast<int>(i), audio.sfx[i].audioPath.c_str(), "SFX");
                                        }
                                        else {
                                            ImGui::OpenPopup("InvalidAudioFileType");
                                        }
                                    }
                                    ImGui::EndDragDropTarget();
                                }

                                ShowInvalidAudioFileTypePopup();
                            }

                            // Add Play and Stop buttons for SFX
                            if (ImGui::Button("Play")) {
                                audio.PlaySFX(static_cast<int>(i));
                                // Call the audio manager to start playing the SFX
                                //audio.PlaySFX(i);  // Assuming PlaySFX takes an index or path as argument
                            }

                            ImGui::SameLine();

                            if (ImGui::Button("Stop")) {
                                audio.StopSFX(static_cast<int>(i));
                                // Call the audio manager to stop playing the SFX
                                // Assuming StopSFX takes an index or path as argument
                                //audio.StopSFX(i);
                            }

                            ImGui::SameLine();

                            // Delete SFX Button
                            if (ImGui::Button("Delete SFX")) {
                                
                                // Remove SFX from the list
                                audio.RemoveSoundFromSfx(i);
                            }

                            ImGui::TreePop();
                        }
                        ImGui::PopID();
                    }

                    ImGui::TreePop();  // Close SFX section
                }

        

                
                //std::string filename = std::filesystem::path(audio.audioPath).filename().string();

                //char audioPathBuffer[256];
                //strncpy(audioPathBuffer, filename.c_str(), sizeof(audioPathBuffer));
                //audioPathBuffer[sizeof(audioPathBuffer) - 1] = '\0'; // Ensure null termination

                //ImGui::BeginDisabled(true); // Disable input field while using drag-and-drop
                //ImGui::InputText("##AudioPathInput", audioPathBuffer, sizeof(audioPathBuffer));
                //ImGui::EndDisabled();

                //if (es_current != ENGINE_STATES::ES_PLAY)
                //{

                //    // Check for drag-and-drop
                //    if (ImGui::BeginDragDropTarget())
                //    {
                //        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH"))
                //        {
                //            std::string draggedAudio = std::string((const char*)payload->Data);
                //            std::filesystem::path filePath(draggedAudio);
                //            std::string extension = filePath.extension().string();

                //            // Validate the file type (assuming only .wav, .mp3 , .ogg are allowed)
                //            if (extension == ".wav" || extension == ".mp3" || extension == ".ogg")
                //            {
                //                // Valid file type, update script path
                //                //strncpy(audioPathBuffer, draggedAudio.c_str(), sizeof(audioPathBuffer));
                //                //audioPathBuffer[sizeof(audioPathBuffer) - 1] = '\0'; // Ensure null termination
                //                audio.audioPath = draggedAudio;
                //                filename = filePath.filename().string(); // Update displayed filename
                //                strncpy(audioPathBuffer, filename.c_str(), sizeof(audioPathBuffer));
                //                audioPathBuffer[sizeof(audioPathBuffer) - 1] = '\0'; // Ensure null termination
                //                modified = true;
                //            }
                //            else
                //            {
                //                // Invalid file type, show error feedback
                //                ImGui::OpenPopup("InvalidAudioFileType");
                //            }
                //        }
                //        ImGui::EndDragDropTarget();
                //    }
                //    // Play and Stop buttons
                //    if (!audio.audioPath.empty())
                //    {
                //        if (ImGui::Button("Play Audio"))
                //        {
                //            //PlayAudio(audio.audioPath); // Function to handle playback
                //        }

                //        ImGui::SameLine();

                //        if (ImGui::Button("Stop Audio"))
                //        {
                //            //StopAudio(); // Function to stop playback
                //        }
                //    }



                //    // Add input for channelGroups
                //    ImGui::InputInt("Channel Groups", &audio.pChannelGroups); // Allow the user to specify the number of channel groups

                //    // Optionally, validate that the number of channel groups is a positive number
                //    if (audio.pChannelGroups < 1)
                //    {
                //        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Channel Groups must be at least 1.");
                //    }
                //}
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
    bool IsJsonFile(const std::string &filePath)
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

        static char filePath[256] = "";
        // Sync the selectedEntityID with the Renderer's picked entity
        static int selectedEntityID = -1;
        auto &renderer = *ECS::GetInstance().GetSystem<Renderer>();

        // Update the selected entity from object picking
        size_t pickedEntity = renderer.getSelectedEntityID();
        if (pickedEntity != static_cast<size_t>(-1))
        {
            selectedEntityID = static_cast<int>(pickedEntity);
        }

        m_global_selected = selectedEntityID;
        static bool showError = false;
        static double errorDisplayTime = 0.0f;
        static bool modified = false;

        // Display the Asset Browser
        ContentBrowser(filePath);

        if (ImGui::TreeNode("Current GameObjects"))
        {
            auto gameObjects = GameObjectManager::GetInstance().GetAllGOs();

            for (size_t i = 0; i < gameObjects.size(); ++i)
            {
                auto &obj = gameObjects[i];
                Ukemochi::EntityID instanceID = obj->GetInstanceID();

                // Highlight the selected entity (picked or clicked in UI)
                bool isSelected = selectedEntityID == instanceID;

                // Use different colors for picked vs regular selection
                ImVec4 normalColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
                ImVec4 selectedColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow for picked objects

                if (isSelected)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, selectedColor);
                }

                // Display each object in the TreeNode
                if (ImGui::Selectable((std::to_string(instanceID) + ": " + obj->GetName()).c_str(), isSelected))
                {
                    selectedEntityID = static_cast<int>(instanceID);
                    modified = false;
                }

                if (isSelected)
                {
                    ImGui::PopStyleColor();

                    // Auto-expand details for picked objects
                    //ImGui::SetNextItemOpen(true);
                    if (ImGui::TreeNode(("Details##" + std::to_string(instanceID)).c_str()))
                    {
                        DisplayEntityDetails(*obj);
                        ImGui::TreePop();
                    }
                }
            }
            ImGui::TreePop();
        }

        // Rest of your existing code for entity creation and management...
        ImGui::Text("Entity Management");
        ImGui::BeginDisabled(true); // Start disabled state
        ImGui::InputText("Object Data File", filePath, IM_ARRAYSIZE(filePath));
        ImGui::EndDisabled(); // End disabled state

        if (ImGui::Button("Create Entity"))
        {
            const std::string prefabDirectory = "../Assets/Prefabs/";
            std::string filePathStr = filePath;

            if (filePathStr.rfind(prefabDirectory, 0) == 0 && IsJsonFile(filePath))
            {
                if (ECS::GetInstance().GetLivingEntityCount() == 0)
                    ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(-1);

                auto &go = GameObjectManager::GetInstance().CreatePrefabObject(filePath);
                if (go.GetTag() == "Player")
                {
                    ECS::GetInstance().GetSystem<Renderer>()->SetPlayer(static_cast<int>(go.GetInstanceID()));
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

        if (ImGui::Button("Create Empty Entity"))
        {
            auto &emptyObject = GameObjectManager::GetInstance().CreateEmptyObject();
            selectedEntityID = static_cast<int>(emptyObject.GetInstanceID());
            emptyObject.AddComponent<Transform>(Transform{});
            modified = true;
        }

        if (showError && ImGui::GetTime() - errorDisplayTime < 2.0f)
        {
            ImGui::TextColored(ImVec4(1, 0, 0, 1),
                               "Invalid file type. Only .json files in ../Assets/Prefabs/ can be used to create an object.");
        }

        ImGui::Separator();

        // Handle editing of the selected entity
        if (selectedEntityID >= 0)
        {
            auto gameObjects = GameObjectManager::GetInstance().GetAllGOs();
            GameObject *selectedObject = nullptr;

            // Find the selected object by ID
            for (auto &obj : gameObjects)
            {
                if (obj->GetInstanceID() == selectedEntityID)
                {
                    selectedObject = obj;
                    break;
                }
            }

            if (selectedObject)
            {
                // Add visual feedback for picked objects
                if (selectedEntityID == static_cast<int>(renderer.getSelectedEntityID()))
                {
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Selected via Object Picking");
                }
                else
                {
                    ImGui::Dummy(ImVec2(0.0f, ImGui::GetTextLineHeight()));
                }

                // Edit the properties of the selected object
                EditEntityProperties(selectedObject, modified);

                ImGui::Separator();

                if (modified)
                {
                    if (ImGui::Button("Save Entity"))
                    {
                        std::cout << "Entity is Saved";
                        SceneManager::GetInstance().SavePrefab(selectedObject, selectedObject->GetName());
                        modified = false;
                    }
                }

                if (Input::IsKeyTriggered(UME_KEY_DELETE) && selectedEntityID >= 0 && !(es_current == ENGINE_STATES::ES_PLAY))
                {
                    // Check if the entity exists
                    if (selectedObject)
                    {
                        std::cout << "Deleting Entity with ID: " << selectedEntityID << std::endl;
                        GameObjectManager::GetInstance().DestroyObject(selectedEntityID);
                        selectedEntityID = -1; // Reset the selected entity ID after deletion
                        modified = false;      // Reset any modification flags
                    }
                    else
                    {
                        std::cout << "Invalid Entity ID: " << selectedEntityID << " - Cannot delete" << std::endl;
                    }
                }

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));

                if (es_current != ES_PLAY)
                {
                    if (ImGui::Button("Remove Entity"))
                    {
                        if (selectedEntityID != -1)
                        {
                            GameObjectManager::GetInstance().DestroyObject(selectedEntityID);
                            selectedEntityID = -1;
                            modified = false;
                        }
                    }
                }
                ImGui::PopStyleColor(3);
            }
        }

        ImGui::End();
    }

    /*!***********************************************************************
    \brief
     Updates the framebuffer size based on the provided ImGui panel size. If the panel
     size changes, the framebuffer is resized accordingly.

    \param[in] panelSize
     The new size of the ImGui panel as an ImVec2 structure containing width (x) and height (y).

    *************************************************************************/
    void UseImGui::UpdateFramebufferSize(ImVec2 panelSize)
    {
        unsigned int newWidth = static_cast<unsigned int>(panelSize.x);
        unsigned int newHeight = static_cast<unsigned int>(panelSize.y);

        if (newWidth != m_currentPanelWidth || newHeight != m_currentPanelHeight)
        {
            m_currentPanelWidth = newWidth;
            m_currentPanelHeight = newHeight;

            ECS::GetInstance().GetSystem<Renderer>()->resizeFramebuffer(m_currentPanelWidth, m_currentPanelHeight);
        }
    }

    /*!***********************************************************************
    \brief
     Updates the object picking framebuffer size based on the provided ImGui panel size.
     If the panel size changes, the object picking framebuffer is resized accordingly.

    \param[in] panelSize
     The new size of the ImGui panel as an ImVec2 structure containing width (x) and height (y).

    *************************************************************************/
    void UseImGui::UpdateObjectPickingFramebufferSize(ImVec2 panelSize)
    {
        unsigned int newWidth = static_cast<unsigned int>(panelSize.x);
        unsigned int newHeight = static_cast<unsigned int>(panelSize.y);

        if (newWidth != m_currentPanelWidth || newHeight != m_currentPanelHeight)
        {
            m_currentPanelWidth = newWidth;
            m_currentPanelHeight = newHeight;

            ECS::GetInstance().GetSystem<Renderer>()->resizeObjectPickingFramebuffer(m_currentPanelWidth, m_currentPanelHeight);
        }
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
        // Application& app = Application::Get();
        // GLuint texture = renderer.getTextureColorBuffer();
        GLuint texture = ECS::GetInstance().GetSystem<Renderer>()->getTextureColorBuffer();
        // GLuint texture = ECS::GetInstance().GetSystem<Renderer>()->getObjectPickingColorBuffer();

        if (showGameView)
        {
            ImGui::Begin("Scene Loader", &showGameView); // Create a window called "Another Window"

            ImVec2 panelSize = ImGui::GetContentRegionAvail();
            // UpdateFramebufferSize(panelSize);
            // UpdateObjectPickingFramebufferSize(panelSize);

            float targetAspect = 16.0f / 9.0f;
            float panelAspect = panelSize.x / panelSize.y;
            float displayWidth, displayHeight;
            if (panelAspect > targetAspect)
            {
                displayWidth = panelSize.y * targetAspect;
                displayHeight = panelSize.y;
            }
            else
            {
                displayWidth = panelSize.x;
                displayHeight = panelSize.x / targetAspect;
            }
            float offsetX = (panelSize.x - displayWidth) * 0.5f;
            float offsetY = (panelSize.y - displayHeight) * 0.5f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offsetX);
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);

            ImGui::Image((ImTextureID)(intptr_t)texture, {displayWidth, displayHeight},
                         {0, 1}, {1, 0});
            // ImGui::Image((ImTextureID)(intptr_t)texture,
            //              ImVec2(static_cast<float>(app.GetWindow().GetWidth()),
            //                     static_cast<float>(app.GetWindow().GetHeight())), {0, 1}, {1, 0});

            // Get the position of the ImGui window
            ImVec2 windowPos = ImGui::GetWindowPos();   // Top-left position of the window
            ImVec2 windowSize = ImGui::GetWindowSize(); // Size of the window
            playerLoaderTopLeft = windowPos;
            // Get the mouse position in screen coordinates
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();

            ImVec2 panelSizehere = ImGui::GetContentRegionAvail();

            // static_cast<float>(Application::Get().GetWindow().GetWidth());

            // Calculate mouse position relative to the "Player Loader" window
            float relativeX = (mousePos.x - cursorPos.x) * 1920 / panelSizehere.x; // mousePos.x - windowPos.x;// * static_cast<float>(Application::Get().GetWindow().GetWidth())/windowSize.x;

            //const GLFWvidmode *videomode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            // float relativeY = (windowSize.y - (mousePos.y - windowPos.y));
            //  Get mouse position relative to the play window
            float relativeY = -1 * (mousePos.y - cursorPos.y + 5) * 1080 / displayHeight;

            // Check if the mouse is within the bounds of the window
            // if (relativeX >= 0 && relativeX <= windowSize.x && relativeY >= 0 && relativeY <= windowSize.y)
            {
                // Optional: Handle the mouse position within the window here
                // std::cout << "Mouse relative position in 'Player Loader' window: (" << relativeX << ", " << relativeY << ")\n";
            }
            SceneManager::GetInstance().SetPlayScreen(Vec2(relativeX, relativeY));
            ImGui::End();

            // Add the object picking debug window
            ImGui::Begin("Object Picking Debug View");
            ImGui::Image((ImTextureID)(intptr_t)ECS::GetInstance().GetSystem<Renderer>()->getObjectPickingColorBuffer(),
                         ImVec2(300, 300), ImVec2(0, 1), ImVec2(1, 0));
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
        ImGuiIO &io = ImGui::GetIO();
        Application &app = Application::Get();
        io.DisplaySize = ImVec2(static_cast<float>(app.GetWindow().GetWidth()),
                                static_cast<float>(app.GetWindow().GetHeight()));

        // Call the combined function to check and handle file drops
        // CheckAndHandleFileDrop();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
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
