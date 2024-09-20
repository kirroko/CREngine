#ifndef OBJECT_CLASS_H
#define OBJECT_CLASS_H

#include <GLFW/glfw3.h>

enum class ObjectType { Box, Circle };

class Object {

public:
	ObjectType type;
	GLfloat x, y;
	GLfloat width, height;
	GLfloat radius;
	GLboolean enable_texture;

	Object(GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLboolean enable_texture)
		: type(ObjectType::Box), x(x), y(y), width(width), height(height), enable_texture(enable_texture) {};

	Object(GLfloat x, GLfloat y, GLfloat radius, GLboolean enable_texture)
		: type(ObjectType::Circle), x(x), y(y), radius(radius), enable_texture(enable_texture) {};

};
#endif // !OBJECT_CLASS_H
