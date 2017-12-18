#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"
#include "spriteRenderer.h"

class GameObject {
	public:
		glm::vec2 position, size, velocity;
		glm::vec3 color;
		GLfloat rotation;
		GLboolean isSolid;
		GLboolean destroyed;

		Texture2D sprite;
		GameObject();
		GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

		virtual void draw(SpriteRenderer &renderer);
};

#endif // !GAME_OBJECT_H
