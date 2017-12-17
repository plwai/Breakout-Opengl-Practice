#include "gameSetting.h"

const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.0f);

Game::Game(GLuint width, GLuint height)
	: state(GAME_ACTIVE), keys(), width(width), height(height) 
{
	this->path = ROOT_PATH;
}

Game::~Game() {

}

void Game::init() {
	ResourceManager::loadShader((this->path + "/src/shader/sprite.vs").c_str(), (this->path + "/src/shader/sprite.frag").c_str(), nullptr, "sprite");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->width), static_cast<GLfloat>(this->height), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader("sprite").use().setInteger("image", 0);
	ResourceManager::getShader("sprite").setMatrix4("projection", projection);

	Renderer = new SpriteRenderer(ResourceManager::getShader("sprite"));

	ResourceManager::loadTexture((this->path + "/assets/texture/paddle.png").c_str(), GL_TRUE, "paddle");
	ResourceManager::loadTexture((this->path + "/assets/texture/re2_1.jpg").c_str(), GL_FALSE, "background");
	ResourceManager::loadTexture((this->path + "/assets/texture/tennisball512.png").c_str(), GL_TRUE, "face");
	ResourceManager::loadTexture((this->path + "/assets/texture/block.png").c_str(), GL_FALSE, "block");
	ResourceManager::loadTexture((this->path + "/assets/texture/block_solid.png").c_str(), GL_FALSE, "block_solid");

	GameLevel one; one.load((this->path + "/src/levels/level_1.lvl").c_str(), this->width, this->height * 0.5);
	GameLevel two; two.load((this->path + "/src/levels/level_2.lvl").c_str(), this->width, this->height * 0.5);
	GameLevel three; three.load((this->path + "/src/levels/level_3.lvl").c_str(), this->width, this->height * 0.5);
	GameLevel four; four.load((this->path + "/src/levels/level_4.lvl").c_str(), this->width, this->height * 0.5);

	this->levels.push_back(one);
	this->levels.push_back(two);
	this->levels.push_back(three);
	this->levels.push_back(four);
	this->level = 1;

	glm::vec2 playerPos = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);
	player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::getTexture("paddle"));
}

void Game::update(GLfloat dt) {

}


void Game::processInput(GLfloat dt) {
	if (this->state == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;

		if (this->keys[GLFW_KEY_A]) {
			player->position.x - velocity >= 0 ?
				player->position.x -= velocity :
				player->position.x = 0;
		}
		if (this->keys[GLFW_KEY_D]) {
			player->position.x + velocity <= this->width - player->size.x ?
				player->position.x += velocity :
				player->position.x = this->width - player->size.x;
		}
	}
}

void Game::render() {
	if (this->state == GAME_ACTIVE) {
		Renderer->DrawSprite(ResourceManager::getTexture("background"),
				glm::vec2(0, 0), glm::vec2(this->width, this->height), 0.0f
			);

		this->levels[this->level - 1].draw(*Renderer);

		player->Draw(*Renderer);
	}
}