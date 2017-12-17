#ifndef GAME_LEVEL_H
#define GAME_LEVEL_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "gameObject.h"
#include "resourceManager.h"

class GameLevel {
	public:
		std::vector<GameObject> bricks;
		GameLevel() {}
		void load(const GLchar *file, GLuint levelWidth, GLuint levelHeight);
		void draw(SpriteRenderer &renderer);
		GLboolean isCompleted();

	private:
		void init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

#endif // !GAME_LEVEL_H
