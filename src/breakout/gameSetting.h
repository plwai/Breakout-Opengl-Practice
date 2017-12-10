#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>

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
};

#endif // !GAME_H
