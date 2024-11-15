/* Start Header ************************************************************************/
/*!
\file       ImGuiCore.h
\author     Hurng Kai Rui, h.kairui, 2301278, h.kairui\@digipen.edu
\date       Sept 25, 2024
\brief      This file declares the UseImGui class, which manages the 
            initialization, rendering, and event handling for ImGui 
            within the Ukemochi Engine.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include <../vendor/GLFW/include/GLFW/glfw3.h>
#include "Ukemochi-Engine/Events/ApplicationEvent.h"
#include "Ukemochi-Engine/Events/KeyEvent.h"
#include "Ukemochi-Engine/Events/MouseEvent.h"


namespace Ukemochi
{
	class GameObject;
	class UME_API UseImGui
	{
	public:
		/*!
		\brief Initializes the ImGui context and OpenGL setup.
		\param window Pointer to the GLFW window.
		*/
		static void ImGuiInit(GLFWwindow* window);

		/*!
		\brief Prepares a new ImGui frame.
		*/
		static void NewFrame();

		static void Begin();
		/*!
		\brief Renders the current ImGui frame.
		*/
		static void ImGuiUpdate();
		//void ImGuiRender();
		/*!
		\brief Cleans up ImGui resources.
		*/
		static void ImGuiClean();

		static void DragAndDropHandler();

		static void ControlPanel(float fps);
		
		static void ShowErrorPopup(const std::string& errorMessage);

		static void LoadScene();

		static void DisplayEntityDetails(GameObject& obj);

		static void CheckAndHandleFileDrop();
		
		static void DebugWindow();

		static void DisplayEntitySelectionCombo(int& selectedEntityIndex);

		static void RemoveSelectedEntity(int& selectedEntityIndex);

		static void EditEntityProperties(GameObject* selectedObject, bool& modified);

		static void ShowEntityManagementUI();

		static void ContentBrowser(char* filePath);

		static void SceneBrowser();

		static size_t GetSceneSize();

		static void SceneRender();

		static float m_LastAssetUpdateTime;

		static float m_LastSceneUpdateTime;

		static std::string GetStartScene();

	private:
		// Mouse and keyboard event handlers
		/*bool IsMouseButtonPressedEvent(MouseButtonPressedEvent& e);
		bool IsMouseButtonReleasedEvent(MouseButtonReleasedEvent& e);
		bool IsMouseMovedEvent(MouseMovedEvent& e);
		bool IsMouseScrolledEvent(MouseScrolledEvent& e);
		bool IsKeyPressedEvent(KeyPressedEvent& e);
		bool IsKeyReleasedEvent(KeyReleasedEvent& e);
		bool IsKeyTypeEvent(KeyTypedEvent& e);
		bool IsWindowResizeEvent(WindowResizeEvent& e);*/

		static std::vector<std::string> assetFiles;
		static std::vector<std::string> folderNames;

		static std::vector<std::string> sceneFiles;
		static void LoadContents(const std::string& directory); // Function to load assets
		static float m_Time; //!< Time since the last frame for delta time calculation.
		static bool m_CompileError;
		static bool m_Compiling;
	};
}