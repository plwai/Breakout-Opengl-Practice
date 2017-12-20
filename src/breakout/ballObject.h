#ifndef BALL_OBJECT_H
#define BALL_OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.h"
#include "texture.h"

class BallObject : public GameObject {
	public: 
		GLfloat radius;
		GLboolean stuck;
		GLboolean sticky, passThrough;

		BallObject();
		BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);

		glm::vec2 move(GLfloat dt, GLuint windowWidth);

		void reset(glm::vec2 position, glm::vec2 velocity);
};

#endif // !BALL_OBJECT_H
