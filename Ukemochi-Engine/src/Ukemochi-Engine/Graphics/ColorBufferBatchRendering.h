/*!***********************************************************************
\file       ColorBufferBatchRendering.h
\author     Tan Shun Zhi Tomy, t.shunzhitomy@digipen.edu
\date       26/11/2024
\brief
This file contains the declaration of the `ColorBufferBatchRenderer2D`
class, which handles efficient batch rendering of colored debug boxes
using OpenGL. The renderer uses vertex and index buffers to minimize
draw calls, and it supports rotation, position, and color customization
for each box.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*************************************************************************/

#ifndef COLOR_BUFFER_BATCH_CLASS_H
#define COLOR_BUFFER_BATCH_CLASS_H

#include "PreCompile.h" 
#include "../vendor/glm/glm/glm.hpp"
#include "GLAD/glad.h"

// Forward declaration
class VAO;
class VBO;
class EBO;
class Shader;

/*!***********************************************************************
\struct     ColorBufferVertex
\brief      Represents a vertex in the color buffer, with position and color
			attributes.
*************************************************************************/
struct ColorBufferVertex {
	glm::vec3 position;
	glm::vec3 color;
};

/*!***********************************************************************
\class      ColorBufferBatchRenderer2D
\brief      Handles batch rendering of colored debug boxes using OpenGL.

\details
The `ColorBufferBatchRenderer2D` class batches multiple debug boxes and
renders them efficiently in a single draw call. It supports customizable
positions, sizes, rotations, and colors for each box.

*************************************************************************/
class ColorBufferBatchRenderer2D {
public:
	/*!***********************************************************************
	\brief      Constructs a new `ColorBufferBatchRenderer2D` object.
	*************************************************************************/
	ColorBufferBatchRenderer2D();

	/*!***********************************************************************
   \brief      Destructor for the `ColorBufferBatchRenderer2D` object. Cleans
			   up GPU resources such as VAO, VBO, and EBO.
   *************************************************************************/
	~ColorBufferBatchRenderer2D();

	/*!***********************************************************************
	\brief      Initializes the batch renderer with a shader and sets up the
				necessary GPU resources (VAO, VBO, and EBO).

	\param[in]  colorBufferShader
				A shared pointer to the shader used for rendering the colored
				debug boxes.
	*************************************************************************/
	void init(std::shared_ptr<Shader> colorBufferShader);

	/*!***********************************************************************
	\brief      Clears the vertex and index buffers, preparing for a new batch
				of debug boxes.
	*************************************************************************/
	void beginBatch();

	/*!***********************************************************************
	\brief      Adds a debug box to the current batch. The box is defined by
				its position, size, color, and rotation.

	\param[in]  position
				The position of the box center in world coordinates.

	\param[in]  size
				The size (width and height) of the box.

	\param[in]  color
				The color of the box in RGB format.

	\param[in]  rotation
				The rotation angle of the box, in radians.
	*************************************************************************/
	void drawDebugBox(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color, float rotation);

	/*!***********************************************************************
    \brief      Ends the current batch by rendering all the vertices and
			    indices in the buffers.
    *************************************************************************/
	void endBatch();

	/*!***********************************************************************
	\brief      Flushes the current batch, rendering all the debug boxes in the
				buffers to the screen using the bound shader.
	*************************************************************************/
	void flush();

private:
	std::vector<ColorBufferVertex> vertices;
	std::vector<GLuint> indices;
	std::unique_ptr<VAO> color_buffer_vao;
	std::unique_ptr<VBO> color_buffer_vbo;
	std::unique_ptr<EBO> color_buffer_ebo;

	std::shared_ptr<Shader> shader;
	const int maxBufferSprites = 1000;

};
#endif