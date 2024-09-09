#include "BoxCollider.h"

void BoxCollider::drawBox()
{
	VAO BoxColliderVAO;
	BoxColliderVAO.Bind();
	VBO BoxColliderVBO(vertices_box, sizeof(vertices_box));
	EBO BoxColliderEBO(indices_box, sizeof(indices_box));

	// use GL_LINE_LOOP
	
	// Links VBO attributes such as coordinates and colors to VAO
	BoxColliderVAO.LinkAttrib(BoxColliderVBO, 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
	BoxColliderVAO.LinkAttrib(BoxColliderVBO, 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	BoxColliderVAO.LinkAttrib(BoxColliderVBO, 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));

	BoxColliderVAO.Unbind();
	BoxColliderVBO.Unbind();
	BoxColliderEBO.Unbind();

	glDrawArrays(GL_LINE_LOOP, 0, 4);
}
