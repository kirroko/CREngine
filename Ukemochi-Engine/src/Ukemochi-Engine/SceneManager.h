#pragma once
#include "PreCompile.h"
#include "ECS/ECS.h"

namespace Ukemochi
{
	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		void LoadScene();
		void SaveScene();
		void Update();
		void ClearScene();

	private:

	};

}