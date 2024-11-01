/* Start Header
*****************************************************************/
/*!
\file       ImGuiCore.cpp
\author     Hurng Kai Rui, h.kairui, 2301278
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

#include "../Factory/Factory.h"
#include "../ECS/ECS.h"

#include "../Graphics/Renderer.h"
namespace Ukemochi
{
	float UseImGui::m_Time = 0.0f; //!< Time since last frame for calculating DeltaTime.


	/*!
	\brief Initializes the ImGui context and sets up OpenGL.
	\param window Pointer to the GLFW window (unused in this implementation).
	*/
	void UseImGui::ImGuiInit(GLFWwindow* /*window*/)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}
	/*!
	\brief Prepares a new ImGui frame and handles window dimensions and timing.
	*/
	void UseImGui::NewFrame()
	{

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

	}

	void UseImGui::LoadScene()
	{
		static char filePath[256] = "../Assets/.json";

	}

	void UseImGui::DisplayEntityDetails(GameObject& obj) {
		ImGui::Text("ID: %d", obj.GetInstanceID());
		ImGui::Text("Name: %s", obj.GetName().c_str());
		ImGui::Text("Tag: %s", obj.GetTag().c_str());

		if (obj.HasComponent<Transform>()) {
			Transform& transform = obj.GetComponent<Transform>();
			ImGui::Text("Transform Component");
			ImGui::Text("Position: (%.2f, %.2f)", transform.position.x, transform.position.y);
			ImGui::Text("Rotation: %.2f", transform.rotation);
			ImGui::Text("Scale: (%.2f, %.2f)", transform.scale.x, transform.scale.y);
		}

		if (obj.HasComponent<Rigidbody2D>()) {
			Rigidbody2D& rb = obj.GetComponent<Rigidbody2D>();
			ImGui::Text("Rigidbody2D Component");
			ImGui::Text("Velocity: (%.2f, %.2f)", rb.velocity.x, rb.velocity.y);
			ImGui::Text("Mass: %.2f", rb.mass);
		}
	}

	void UseImGui::DisplayEntitySelectionCombo(int& selectedEntityIndex) {
		auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();

		// Store the names of entities in a vector of strings to keep them in memory
		std::vector<std::string> entityNames;
		for (const auto& obj : gameObjects) {
			entityNames.push_back(std::to_string(obj.GetInstanceID()) + ": " + obj.GetName());
		}

		// Create a vector of const char* pointers to each name in entityNames
		std::vector<const char*> entityNamePointers;
		for (const auto& name : entityNames) {
			entityNamePointers.push_back(name.c_str());
		}

		// Display the ImGui Combo with the populated entityNamePointers
		ImGui::Combo("Select Entity to Remove", &selectedEntityIndex, entityNamePointers.data(), entityNamePointers.size());
	}

	void UseImGui::RemoveSelectedEntity(int& selectedEntityIndex) {
		auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();
		if (selectedEntityIndex >= 0 && selectedEntityIndex < gameObjects.size()) {
			GameObject entityToDelete = gameObjects[selectedEntityIndex];
			GameObjectFactory::DestroyObject(entityToDelete);
			selectedEntityIndex = -1;
		}
	}

	void UseImGui::EditEntityProperties(GameObject& selectedObject) {
		ImGui::Text("Editing properties of: %s", selectedObject.GetName().c_str());

		if (selectedObject.HasComponent<Transform>()) {
			Transform& transform = selectedObject.GetComponent<Transform>();
			ImGui::InputFloat2("Position", &transform.position.x);
			ImGui::InputFloat("Rotation", &transform.rotation);
			ImGui::InputFloat2("Scale", &transform.scale.x);
		}

		if (selectedObject.HasComponent<Rigidbody2D>()) {
			Rigidbody2D& rb = selectedObject.GetComponent<Rigidbody2D>();
			ImGui::InputFloat2("Velocity", &rb.velocity.x);
			ImGui::InputFloat("Mass", &rb.mass);
		}
	}

	void UseImGui::ShowEntityManagementUI()
	{
		static char filePath[256] = "../Assets/Player.json";
		static int selectedEntityIndex = -1;

		if (ImGui::TreeNode("Current GameObjects")) {
			auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();

			for (size_t i = 0; i < gameObjects.size(); ++i) {
				auto& obj = gameObjects[i];
				if (ImGui::TreeNode((std::to_string(obj.GetInstanceID()) + ": " + obj.GetName()).c_str())) {
					DisplayEntityDetails(obj);
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		ImGui::Text("Entity Management");
		ImGui::InputText("Player Data File", filePath, IM_ARRAYSIZE(filePath));

		if (ImGui::Button("Create Player Entity")) {
			if (filePath[0] != '\0') {
				GameObjectFactory::CreateObject(filePath);
			}
		}

		// Pass selectedEntityIndex by reference so it can be updated in the combo function
		DisplayEntitySelectionCombo(selectedEntityIndex);

		if (ImGui::Button("Remove Entity")) {
			RemoveSelectedEntity(selectedEntityIndex);
		}

		if (selectedEntityIndex >= 0) {
			auto gameObjects = GameObjectFactory::GetAllObjectsInCurrentLevel();
			if (selectedEntityIndex < gameObjects.size()) {
				GameObject& selectedObject = gameObjects[selectedEntityIndex];
				EditEntityProperties(selectedObject);
			}
		}
	}

	void UseImGui::Begin()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
		static bool showGameView = true;
		Application& app = Application::Get();
		//GLuint texture = renderer.getTextureColorBuffer();
		GLuint texture = ECS::GetInstance().GetSystem<Renderer>()->getTextureColorBuffer();
		if (showGameView)
		{
			ImGui::Begin("Player Loader", &showGameView);   // Create a window called "Another Window"
			ImGui::Image((ImTextureID)(intptr_t)texture, ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight()), { 0,1 },{1,0});
			ImGui::End();
		}

		/*static bool showHierarchy = true;
		if (showHierarchy)
		{
			ImGui::Begin("Scene Hierarchy", &showHierarchy);
			ImGui::End();
		}*/
	}
	/*!
	\brief Renders the current ImGui frame and draws the UI.
	*/
	void UseImGui::ImGuiUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

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
