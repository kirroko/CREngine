/*!***********************************************************************
\file       DebugModeBatchRendering.h
\author     Tan Shun Zhi Tomy, t.shunzhitomy@digipen.edu
\date       26/11/2024
\brief
This header file declares the `DebugBatchRenderer2D` class and the
associated `debugVertex` structure. The class provides functionality
for batch rendering debug wireframe shapes, specifically boxes, using
OpenGL.

\copyright
Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*************************************************************************/
#ifndef DEBUG_BATCH_RENDERING_H
#define DEBUG_BATCH_RENDERING_H

#include "PreCompile.h" 
#include "../vendor/glm/glm/glm.hpp"
#include "GLAD/glad.h"

// Forward declaration
class VAO;
class VBO;
class Shader;
class Texture;

/*!***********************************************************************
\struct     debugVertex
\brief      Represents a vertex in the debug renderer. Contains the position
			of the vertex in 3D space.
*************************************************************************/
struct debugVertex {
	glm::vec3 position;
};

/*!***********************************************************************
\class      DebugBatchRenderer2D
\brief      Handles batch rendering of debug wireframe shapes using OpenGL.

\details
The `DebugBatchRenderer2D` class is designed to efficiently render multiple
debug shapes (such as wireframe boxes) in a single draw call by batching
the vertex data. It utilizes OpenGL vertex buffer objects (VBOs) and vertex
array objects (VAOs) to manage geometry data on the GPU.

Key Features:
- Batch rendering of multiple wireframe boxes.
- Efficiently manages GPU resources to minimize draw calls.
- Supports basic transformations like rotation and scaling.

Usage:
1. Call `init` to initialize the renderer with a shader.
2. Use `beginBatch` to clear the current batch.
3. Add shapes to the batch using `drawDebugBox`.
4. Call `endBatch` or `flush` to render the shapes to the screen.

*************************************************************************/
class DebugBatchRenderer2D {

public:
	/*!***********************************************************************
	\brief      Constructs a new `DebugBatchRenderer2D` object.
	*************************************************************************/
	DebugBatchRenderer2D();

	/*!***********************************************************************
	\brief      Destructor for the `DebugBatchRenderer2D` object. Cleans up
				resources such as VAO, VBO, and clears the vertex buffer.
	*************************************************************************/
	~DebugBatchRenderer2D();

	/*!***********************************************************************
	\brief      Initializes the batch renderer with a shader and sets up
				necessary GPU resources (VAO and VBO).

	\param[in]  debugShader
				A shared pointer to the shader used for rendering the debug
				wireframes.
	*************************************************************************/
	void init(std::shared_ptr<Shader> debugShader);

	/*!***********************************************************************
	\brief      Clears the vertex buffer and prepares for a new batch of
				debug shapes.
	*************************************************************************/
	void beginBatch();

	/*!***********************************************************************
	\brief      Adds a debug wireframe box to the batch. The box is defined by
				its position, size, and rotation.

	\param[in]  position
				The position of the box center in world coordinates.

	\param[in]  size
				The size (width and height) of the box.

	\param[in]  rotation
				The rotation angle of the box in radians.
	*************************************************************************/
	void drawDebugBox(const glm::vec2& position, const glm::vec2& size, float rotation);

	/*!***********************************************************************
	\brief      Ends the current batch by rendering all the shapes in the
				vertex buffer.
	*************************************************************************/
	void endBatch();

	/*!***********************************************************************
    \brief      Flushes the current batch, rendering all the debug shapes in
			    the vertex buffer using the bound shader.
    *************************************************************************/
	void flush();

private:
	std::vector<debugVertex> vertices;
	std::unique_ptr<VAO> debug_vao;
	std::unique_ptr<VBO> debug_vbo;

	std::shared_ptr<Shader> shader;
	const int maxShapes = 1000;

};
#endif // !#DEBUG_BATCH_RENDERING_H
