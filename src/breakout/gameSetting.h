#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <BreakoutConfig.h>

#include "spriteRenderer.h"
#include "resourceManager.h"
#include "gameLevel.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
};

class Game {
	public:
		//Game state
		GameState	state;
		GLboolean	keys[1024];
		GLuint		width, height;
		
		Game(GLuint width, GLuint height);
		~Game();

		void init();
		void processInput(GLfloat dt);
		void update(GLfloat dt);
		void render();

	private: 
		SpriteRenderer *Renderer;
		std::vector<GameLevel> levels;
		GLuint level;
		GameObject *player;
		std::string path;
};

#endif // !GAME_H
