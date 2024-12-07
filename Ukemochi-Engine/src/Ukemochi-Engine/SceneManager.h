/* Start Header ************************************************************************/
/*!
\file       SceneManager.h
\author     Tan Si Han, t.sihan, 2301264, t.sihan\@digipen.edu
\date       Nov 8, 2024
\brief      This file contains the declaration of the SceneManger system.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header **************************************************************************/

#pragma once
#include "PreCompile.h"
#include "ECS/ECS.h"
#include "Game/GSM.h"
#include "Factory/GameObject.h"
#include "InGameGUI/InGameGUI.h"

namespace Ukemochi
{
	class SceneManager
	{
	public:
		/*!***********************************************************************
		\brief
		 Making SceneManger class object as a Singleton object
		*************************************************************************/
		static SceneManager &GetInstance()
		{
			static std::unique_ptr<SceneManager> instance(new SceneManager());
			return *instance;
		}

		/*!***********************************************************************
		\brief
			Constructor for SceneManager class.
		\details
			Initializes the scene manager and any necessary resources.
		*************************************************************************/
		SceneManager();

		/*!***********************************************************************
		\brief
			Destructor for SceneManager class.
		*************************************************************************/
		~SceneManager();

		/*!***********************************************************************
		\brief
			Loads the scene manager's resources and prepares it for use.
		\details
			This method is responsible for loading all necessary resources and
			setting up the environment for scene management.
		*************************************************************************/
		void SceneMangerLoad();

		/*!***********************************************************************
		\brief
			Initializes the scene manager.
		\details
			This method initializes various systems and components required for
			scene management and the game environment.
		*************************************************************************/
		void SceneMangerInit();

		/*!***********************************************************************
		\brief
			Updates the state of the scene manager when in game Engine State.
		\details
			This method is called periodically to update the scene manager and
			ensure the game world remains in sync with the game logic.
		*************************************************************************/
		void SceneMangerUpdate();

		/*!***********************************************************************
		\brief
			Runs the systems for the current scene.
		\details
			This method iterates through all systems and runs them for the current scene.
		*************************************************************************/
		void SceneMangerRunSystems();

		/*!***********************************************************************
		\brief
			Updates the camera settings during each frame.
		\param deltaTime: The time elapsed since the last frame.
		\details
			This method updates the camera's position, rotation, and other
			settings based on the delta time.
		*************************************************************************/
		void SceneMangerUpdateCamera(double deltaTime);

		/*!***********************************************************************
		\brief
			Draws the current scene to the screen.
		\details
			This method handles the rendering of the scene by drawing all objects
			to the screen, including any necessary UI elements.
		*************************************************************************/
		void SceneManagerDraw();

		/*!***********************************************************************
		\brief
			Frees up resources used by the scene manager.
		\details
			This method is responsible for releasing any resources held by the
			SceneManager, preparing it for shutdown or reinitialization.
		*************************************************************************/
		void SceneManagerFree();

		/*!***********************************************************************
		\brief
			Unloads the current scene.
		\details
			This method ensures the proper unloading of the current scene,
			freeing any resources associated with it.
		*************************************************************************/
		void SceneManagerUnload();

		/*!***********************************************************************
		\brief
			Returns a reference to the flag indicating whether IMGUI is on.
		\return
			A reference to the boolean flag that controls IMGUI visibility.
		*************************************************************************/
		bool &GetOnIMGUI();

		/*!***********************************************************************
		\brief
			Returns the name of the current scene.
		\return
			A reference to the string holding the current scene's name.
		*************************************************************************/
		std::string &GetCurrScene();

		/*!***********************************************************************
		\brief
			Loads a save file into the current scene.
		\param file_name: The name of the save file to load.
		\details
			This method reads the save file and restores the scene to its state
			from the saved file.
		*************************************************************************/
		void LoadSaveFile(const std::string &file_name);

		/*!***********************************************************************
		\brief
			Saves the current scene to a file.
		\param file_name: The name of the file to save the scene to.
		\details
			This method writes the current state of the scene to a specified file.
		*************************************************************************/
		void SaveScene(const std::string &file_name);

		/*!***********************************************************************
		\brief
			Creates a new scene and loads it.
		\param file_name: The name of the file to use for the new scene.
		\details
			This method initializes a new scene based on the provided file name.
		*************************************************************************/
		void CreateNewScene(const std::string &file_name);

		/*!***********************************************************************
		\brief
			Saves a prefab GameObject to a file.
		\param prefabObj: Pointer to the GameObject to save as a prefab.
		\param file_name: The name of the file to save the prefab to.
		\details
			This method saves a GameObject as a prefab, which can be reused or instantiated later.
		*************************************************************************/
		void SavePrefab(GameObject *prefabObj, const std::string &file_name);

		/*!***********************************************************************
		\brief
			Gets the play screen size.
		\return
			The size of the play screen as a Vec2 object.
		*************************************************************************/
		Vec2 GetPlayScreen();

		/*!***********************************************************************
		\brief
			Sets the play screen size.
		\param playsize: The new play screen size to set.
		*************************************************************************/
		void SetPlayScreen(Vec2 playsize);

		/*!***********************************************************************
		\brief
			Prints the performance statistics of different subsystems.
		\param loop: Time duration of the loop.
		\param collision: Time duration spent on collision detection.
		\param physics: Time duration spent on physics simulation.
		\param graphics: Time duration spent on rendering graphics.
		\details
			This method logs the performance data for different subsystems, which can be
			useful for optimization and performance monitoring.
		*************************************************************************/
		void print_performance(std::chrono::duration<double> loop, std::chrono::duration<double> collision, std::chrono::duration<double> physics, std::chrono::duration<double> graphics);

		// --- Frame Performance Viewer
		// TODO: Quick Hack needs to be encapsulated
		static std::chrono::duration<double> loop_time;
		static std::chrono::duration<double> collision_time;
		static std::chrono::duration<double> physics_time;
		static std::chrono::duration<double> graphics_time;
		static std::chrono::duration<double> logic_time;

		std::chrono::steady_clock::time_point sys_start;
		std::chrono::steady_clock::time_point sys_end;
		std::chrono::steady_clock::time_point loop_start;
		std::chrono::steady_clock::time_point loop_end;

	private:
		std::string sceneName;
		Vec2 cameraSize;
	};
}