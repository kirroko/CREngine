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

#include "imgui.h"
#include "Ukemochi-Engine/Events/ApplicationEvent.h"
#include "Ukemochi-Engine/Events/KeyEvent.h"
#include "Ukemochi-Engine/Events/MouseEvent.h"


namespace Ukemochi
{
	class GameObject;
	class UME_API UseImGui
	{
	public:
		/*!***********************************************************************
		\brief
		Initializes the ImGui context and OpenGL setup.
		\param[in] window
		Pointer to the GLFW window used for setting up the OpenGL context for ImGui.
		\return
		Void.
		*************************************************************************/
		static void ImGuiInit(GLFWwindow* window);

		/*!***********************************************************************
		\brief
		Prepares a new ImGui frame by setting up necessary resources.
		\return
		Void.
		*************************************************************************/
		static void NewFrame();
		
		/*!***********************************************************************
		\brief
		Displays the sprite editor window, allowing for manipulation of sprite-related assets.
		\return
		Void.
		*************************************************************************/
		static void SpriteEditorWindow();

		/*!***********************************************************************
		\brief
		Begins a new ImGui frame, setting up necessary environment for rendering.
		\return
		Void.
		*************************************************************************/
		static void Begin();

		/*!***********************************************************************
		\brief
		Renders the current ImGui frame, displaying the UI elements and any updated components.
		\return
		Void.
		*************************************************************************/
		static void ImGuiUpdate();

		/*!***********************************************************************
		\brief
		Cleans up ImGui resources, releasing any memory or resources used by ImGui.
		\return
		Void.
		*************************************************************************/
		static void ImGuiClean();

		/*!***********************************************************************
		\brief
		Handles drag-and-drop functionality for files within the ImGui interface.
		\return
		Void.
		*************************************************************************/
		static void DragAndDropHandler();

		/*!***********************************************************************
		\brief
		Displays the control panel, showing the current frame rate and other relevant controls.
		\param[in] fps
		The current frames per second value to display.
		\return
		Void.
		*************************************************************************/
		static void ControlPanel(float fps);
		
		/*!***********************************************************************
		\brief
		Renders 2D gizmos in the ImGui interface for debugging and scene visualization.
		\return
		Void.
		*************************************************************************/
		static void RenderGizmo2d();

		/*!***********************************************************************
		\brief
		Displays an error popup with the provided error message.
		\param[in] errorMessage
		The error message to display in the popup.
		\return
		Void.
		*************************************************************************/
		static void ShowErrorPopup(const std::string& errorMessage);

		/*!***********************************************************************
		\brief
		Opens and loads the scene based on user input or file selection.
		\return
		Void.
		*************************************************************************/
		static void LoadScene();

		/*!***********************************************************************
		\brief
		Displays the properties of the given game object in the UI.
		\param[in] obj
		The game object whose details are to be displayed.
		\return
		Void.
		*************************************************************************/
		static void DisplayEntityDetails(GameObject& obj);

		/*!***********************************************************************
		\brief
		Checks for and handles any file drop events, updating the UI accordingly.
		\return
		Void.
		*************************************************************************/
		static void CheckAndHandleFileDrop();
		
		/*!***********************************************************************
		\brief
		Displays a debug window with various system information and status indicators.
		\return
		Void.
		*************************************************************************/
		static void DebugWindow();

		/*!***********************************************************************
		\brief
		Displays a combo box allowing the user to select an entity from the list.
		\param[in/out] selectedEntityIndex
		The index of the currently selected entity, which will be updated if the selection changes.
		\return
		Void.
		*************************************************************************/
		static void DisplayEntitySelectionCombo(int& selectedEntityIndex);

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
		static void ShowInvalidAudioFileTypePopup();

		/*!***********************************************************************
		\brief
		Displays the UI for adding a component to the selected game object.
		\param[in] selectedObject
		Pointer to the game object to which a component is to be added.
		\param[in/out] modified
		A flag indicating whether any modifications were made.
		\return
		Void.
		*************************************************************************/
		static void AddComponentUI(GameObject* selectedObject, bool& modified);

		/*!***********************************************************************
		\brief
		Displays the UI for removing a component from the selected game object.
		\param[in] selectedObject
		Pointer to the game object from which a component is to be removed.
		\param[in/out] modified
		A flag indicating whether any modifications were made.
		\return
		Void.
		*************************************************************************/
		static void RemoveComponentUI(GameObject* selectedObject, bool& modified);

		/*!***********************************************************************
		\brief
		Removes the selected entity from the scene.
		\param[in/out] selectedEntityIndex
		The index of the selected entity, which will be removed from the scene.
		\return
		Void.
		*************************************************************************/
		static void RemoveSelectedEntity(int& selectedEntityIndex);

		/*!***********************************************************************
		\brief
		Displays the UI for editing the properties of a selected game object.
		\param[in] selectedObject
		Pointer to the selected game object whose properties are to be edited.
		\param[in/out] modified
		A flag indicating whether any modifications were made to the game object.
		\return
		Void.
		*************************************************************************/
		static void EditEntityProperties(GameObject* selectedObject, bool& modified);

		/*!***********************************************************************
		\brief
		Displays the entity management UI, allowing for manipulation of game objects and components.
		\return
		Void.
		*************************************************************************/
		static void ShowEntityManagementUI();
		
		/*!***********************************************************************
		\brief
		Updates the framebuffer size used by ImGui based on the panel size.
		\param[in] panelSize
		The new size of the panel that will be used to update the framebuffer.
		\return
		Void.
		*************************************************************************/
		static void UpdateFramebufferSize(ImVec2 panelSize);


		/*!***********************************************************************
		\brief
		Updates the framebuffer size used for object picking, based on the panel size.
		\param[in] panelSize
		The new size of the panel that will be used to update the object picking framebuffer.
		\return
		Void.
		*************************************************************************/
		static void UpdateObjectPickingFramebufferSize(ImVec2 panelSize);

		/*!***********************************************************************
		\brief
		Displays the content browser UI, allowing the user to navigate and select assets.
		\param[in/out] filePath
		The file path selected by the user, which will be updated when a new file is selected.
		\return
		Void.
		*************************************************************************/
		static void ContentBrowser(char* filePath);

		/*!***********************************************************************
		\brief
		Displays the scene browser, allowing the user to navigate and manage scenes.
		\return
		Void.
		*************************************************************************/
		static void SceneBrowser();

		/*!***********************************************************************
		\brief
		Returns the current size of the scene.
		\return
		The size of the scene (number of entities or objects).
		*************************************************************************/
		static size_t GetSceneSize();

		/*!***********************************************************************
		\brief
		Renders the current scene to the screen.
		\return
		Void.
		*************************************************************************/
		static void SceneRender();

		/*!***********************************************************************
		\brief
		Stores the time of the last asset update for delta time calculation.
		*************************************************************************/
		static float m_LastAssetUpdateTime;

		/*!***********************************************************************
		\brief
		Stores the time of the last scene update for delta time calculation.
		*************************************************************************/
		static float m_LastSceneUpdateTime;

		/*!***********************************************************************
		\brief
		Returns the start scene name.
		\return
		The name of the start scene.
		*************************************************************************/
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

#pragma warning(push)
#pragma warning(disable : 4251)

		/*!***********************************************************************
		\brief
		List of asset file paths loaded into the engine.
		This vector stores the paths to the asset files used by the engine.
		\type
		std::vector<std::string>
		*************************************************************************/
		static std::vector<std::string> assetFiles;

		/*!***********************************************************************
		\brief
		List of folder names in the current asset directory.
		This vector stores the names of the folders in the current asset directory.
		\type
		std::vector<std::string>
		*************************************************************************/
		static std::vector<std::string> folderNames;

		/*!***********************************************************************
		\brief
		List of texture file paths loaded into the engine.
		This vector stores the paths to the texture files used in the engine.
		\type
		std::vector<std::string>
		*************************************************************************/
		static std::vector<std::string> textureFiles;

		/*!***********************************************************************
		\brief
		List of scene file paths loaded into the engine.
		This vector stores the paths to the scene files used by the engine.
		\type
		std::vector<std::string>
		*************************************************************************/
		static std::vector<std::string> sceneFiles;

		/*!***********************************************************************
		\brief
		Path to the sprite asset.
		This string stores the file path of the current sprite asset used by the engine.
		\type
		std::string
		*************************************************************************/
		static std::string m_SpritePath;

		/*!***********************************************************************
		\brief
		Loads contents from the specified directory.
		This function loads assets from the given directory and populates the relevant vectors with file paths.
		\param[in] directory
		The directory path from which the contents will be loaded.
		\return
		Void.
		*************************************************************************/
		static void LoadContents(const std::string& directory);

		/*!***********************************************************************
		\brief
		Stores the time elapsed since the last frame.
		This variable stores the delta time for calculating frame time differences between updates.
		\type
		float
		*************************************************************************/
		static float m_Time;

		/*!***********************************************************************
		\brief
		Global selection index.
		This variable holds the index of the currently selected item (e.g., an entity or asset) in the UI.
		\type
		int
		*************************************************************************/
		static int m_global_selected;

		/*!***********************************************************************
		\brief
		Current panel width for UI rendering.
		This variable stores the current width of the ImGui panel used for rendering.
		\type
		unsigned int
		*************************************************************************/
		static unsigned int m_currentPanelWidth;

		/*!***********************************************************************
		\brief
		Current panel height for UI rendering.
		This variable stores the current height of the ImGui panel used for rendering.
		\type
		unsigned int
		*************************************************************************/
		static unsigned int m_currentPanelHeight;

		/*!***********************************************************************
		\brief
		Flag indicating whether there is a compile error.
		This boolean flag indicates if there was an error during the compilation process.
		\type
		bool
		*************************************************************************/
		static bool m_CompileError;

		/*!***********************************************************************
		\brief
		Flag indicating whether a sprite is currently being loaded or edited.
		This boolean flag indicates if a sprite is being actively edited or loaded into the engine.
		\type
		bool
		*************************************************************************/
		static bool m_SpriteFlag;

		/*!***********************************************************************
		\brief
		Flag indicating whether the engine is currently compiling.
		This boolean flag indicates if the engine is in the process of compiling a script or asset.
		\type
		bool
		*************************************************************************/
		static bool m_Compiling;
	};
#pragma warning(pop)
}