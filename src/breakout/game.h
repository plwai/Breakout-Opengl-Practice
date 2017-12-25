#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <tuple>
#include <algorithm>
#include <irrKlang.h>

#include <BreakoutConfig.h>

#include "spriteRenderer.h"
#include "resourceManager.h"
#include "gameLevel.h"
#include "ballObject.h"
#include "particleGenerator.h"
#include "postProcessor.h"
#include "powerUp.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT,
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

class Game {
	public:
		//Game state
		GameState	state;
		GLboolean	keys[1024];
		GLuint		width, height;
		std::vector<PowerUp> powerUps;
		
		Game(GLuint width, GLuint height);
		~Game();

		void init();
		void processInput(GLfloat dt);
		void update(GLfloat dt);
		void render();
		void doCollisions();
		void setLevel(GLuint levelNumber);
		void resetLevel();
		void resetPlayer();
		void spawnPowerUps(GameObject &block);
		void updatePowerUps(GLfloat dt);
		void proceedNextLevel();

	private: 
		SpriteRenderer *Renderer;
		std::vector<GameLevel> levels;
		GLuint level;
		GameObject *player;
		BallObject *ball;
		ParticleGenerator *particles;
		PostProcessor *effects;
		std::string path;

		GLboolean checkCollision(GameObject &firstObj, GameObject &secondObj);
		Collision checkCollision(BallObject &firstObj, GameObject &secondObj);
		Direction vectorDirection(glm::vec2 target);
		void activatePowerUp(PowerUp &powerUp);
};

#endif // !GAME_H
