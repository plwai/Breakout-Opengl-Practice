#ifndef POWER_UP_H
#define POWER_UP_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.h"
#include "texture.h"

const glm::vec2 POWER_SIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.f);

class PowerUp : public GameObject {
	public:
		std::string type;
		GLfloat duration;
		GLboolean activated;

		PowerUp(std::string type, glm::vec3 color, GLfloat duration, glm::vec2 position, Texture2D texture)
			: GameObject(position, POWER_SIZE, texture, color, VELOCITY), type(type), duration(duration), activated() {};
};


#endif // !POWER_UP_H