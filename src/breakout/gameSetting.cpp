#include "gameSetting.h"

const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const GLfloat BALL_RADIUS = 12.5f;
GLfloat shakeTime = 0.0f;

Game::Game(GLuint width, GLuint height)
	: state(GAME_ACTIVE), keys(), width(width), height(height) 
{
	this->path = ROOT_PATH;
}

Game::~Game() {

}

void Game::init() {
	ResourceManager::loadShader((this->path + "/src/shader/sprite.vs").c_str(), (this->path + "/src/shader/sprite.frag").c_str(), nullptr, "sprite");
	ResourceManager::loadShader((this->path + "/src/shader/particle.vs").c_str(), (this->path + "/src/shader/particle.frag").c_str(), nullptr, "particle");
	ResourceManager::loadShader((this->path + "/src/shader/postProcess.vs").c_str(), (this->path + "/src/shader/postProcess.frag").c_str(), nullptr, "effects");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->width), static_cast<GLfloat>(this->height), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader("sprite").use().setInteger("image", 0);
	ResourceManager::getShader("sprite").setMatrix4("projection", projection);
	ResourceManager::getShader("particle").use().setMatrix4("projection", projection);

	Renderer = new SpriteRenderer(ResourceManager::getShader("sprite"));

	ResourceManager::loadTexture((this->path + "/assets/texture/paddle.png").c_str(), GL_TRUE, "paddle");
	ResourceManager::loadTexture((this->path + "/assets/texture/re2_1.jpg").c_str(), GL_FALSE, "background");
	ResourceManager::loadTexture((this->path + "/assets/texture/tennisball512.png").c_str(), GL_TRUE, "face");
	ResourceManager::loadTexture((this->path + "/assets/texture/block.png").c_str(), GL_FALSE, "block");
	ResourceManager::loadTexture((this->path + "/assets/texture/block_solid.png").c_str(), GL_FALSE, "block_solid");
	ResourceManager::loadTexture((this->path + "/assets/texture/particle.png").c_str(), GL_TRUE, "particle");

	GameLevel one; one.load((this->path + "/src/levels/level_1.lvl").c_str(), this->width, this->height * 0.5);
	GameLevel two; two.load((this->path + "/src/levels/level_2.lvl").c_str(), this->width, this->height * 0.5);
	GameLevel three; three.load((this->path + "/src/levels/level_3.lvl").c_str(), this->width, this->height * 0.5);
	GameLevel four; four.load((this->path + "/src/levels/level_4.lvl").c_str(), this->width, this->height * 0.5);

	this->levels.push_back(one);
	this->levels.push_back(two);
	this->levels.push_back(three);
	this->levels.push_back(four);
	this->setLevel(1);

	glm::vec2 playerPos = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);
	player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::getTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("face"));

	particles = new ParticleGenerator(
		ResourceManager::getShader("particle"),
		ResourceManager::getTexture("particle"),
		500
	);

	effects = new PostProcessor(ResourceManager::getShader("effects"), width, height);
}

void Game::update(GLfloat dt) {
	ball->move(dt, this->width);

	this->doCollisions();
	particles->update(dt, *ball, 2, glm::vec2(ball->radius / 2));

	if (ball->position.y >= this->height) {
		this->resetLevel();
		this->resetPlayer();
	}

	if (shakeTime > 0.0f) {
		shakeTime -= dt;
		if (shakeTime <= 0.0f)
			effects->shake = false;
	}
}


void Game::processInput(GLfloat dt) {
	if (this->state == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;

		if (this->keys[GLFW_KEY_A]) {
			if (player->position.x - velocity >= 0) {
				player->position.x -= velocity;
				if (ball->stuck)
					ball->position.x -= velocity;
			}
			else {
				player->position.x = 0;
				if (ball->stuck)
					ball->position.x = PLAYER_SIZE.x / 2 - BALL_RADIUS;
			}
		}
		if (this->keys[GLFW_KEY_D]) {
			if (player->position.x + velocity <= this->width - player->size.x)
			{
				player->position.x += velocity;
				if (ball->stuck)
					ball->position.x += velocity;
			}
			else {
				player->position.x = this->width - player->size.x;
				if (ball->stuck)
					ball->position.x = this->width - player->size.x + (PLAYER_SIZE.x / 2 - BALL_RADIUS);
			}
		}
		if (this->keys[GLFW_KEY_SPACE]) {
			ball->stuck = false;
		}
	}
}

void Game::render() {
	if (this->state == GAME_ACTIVE) {
		effects->beginRender();
		Renderer->DrawSprite(ResourceManager::getTexture("background"),
				glm::vec2(0, 0), glm::vec2(this->width, this->height), 0.0f
			);

		this->levels[this->level].draw(*Renderer);

		player->draw(*Renderer);		
		ball->draw(*Renderer);
		particles->draw();
		effects->endRender();
		effects->render(glfwGetTime());
	}
}

void Game::doCollisions() {
	for (GameObject &box : this->levels[this->level].bricks) {
		if (!box.destroyed) {
			Collision collision = this->checkCollision(*ball, box);

			if (std::get<0>(collision)) {
				if (!box.isSolid) {
					box.destroyed = GL_TRUE;
				}
				else {
					shakeTime = 0.05f;
					effects->shake = true;
				}

				Direction dir = std::get<1>(collision);
				glm::vec2 diffVector = std::get<2>(collision);

				if (dir == LEFT || dir == RIGHT) {
					ball->velocity.x = -ball->velocity.x;

					GLfloat penetration = ball->radius - std::abs(diffVector.x);
					if (dir == LEFT) {
						ball->position.x += penetration;
					}
					else {
						ball->position.x -= penetration;
					}
				}
				else {
					ball->velocity.y = -ball->velocity.y;

					GLfloat penetration = ball->radius - std::abs(diffVector.y);
					if (dir == UP) {
						ball->position.y -= penetration;
					}
					else {
						ball->position.y += penetration;
					}
				}
			}
		}
	}

	Collision result = checkCollision(*ball, *player);

	if (!ball->stuck && std::get<0>(result)) {
		GLfloat centerBoard = player->position.x + player->size.x / 2;
		GLfloat distance = (ball->position.x + ball->radius) - centerBoard;
		GLfloat percentage = distance / (player->size.x / 2);

		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = ball->velocity;
		ball->velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		ball->velocity.y = -1 * std::abs(ball->velocity.y);
		ball->velocity = glm::normalize(ball->velocity) * glm::length(oldVelocity);
	}
}

void Game::setLevel(GLuint levelNumber) {
	this->level = levelNumber - 1;
}

void Game::resetLevel()
{
	if (this->level == 0)
		this->levels[0].load((this->path + "/src/levels/level_1.lvl").c_str(), this->width, this->height * 0.5f);
	else if (this->level == 1)
		this->levels[1].load((this->path + "/src/levels/level_2.lvl").c_str(), this->width, this->height * 0.5f);
	else if (this->level == 2)
		this->levels[2].load((this->path + "/src/levels/level_3.lvl").c_str(), this->width, this->height * 0.5f);
	else if (this->level == 3)
		this->levels[3].load((this->path + "/src/levels/level_4.lvl").c_str(), this->width, this->height * 0.5f);
}

void Game::resetPlayer()
{
	player->size = PLAYER_SIZE;
	player->position = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);
	ball->reset(player->position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}

GLboolean Game::checkCollision(GameObject &firstObj, GameObject &secondObj) {
	bool collisionX = firstObj.position.x + firstObj.size.x >= secondObj.position.x &&
		secondObj.position.x + secondObj.size.x >= firstObj.position.x;

	bool collisionY = firstObj.position.y + firstObj.size.y >= secondObj.position.y &&
		secondObj.position.y + secondObj.size.y >= firstObj.position.y;

	return collisionX && collisionY;
}

Collision Game::checkCollision(BallObject &firstObj, GameObject &secondObj) {
	glm::vec2 center(firstObj.position + firstObj.radius);
	glm::vec2 aabbHalfExtents(secondObj.size.x / 2, secondObj.size.y / 2);
	glm::vec2 aabbCenter(
		secondObj.position.x + aabbHalfExtents.x,
		secondObj.position.y + aabbHalfExtents.y
	);

	glm::vec2 difference = center - aabbCenter;
	glm::vec2 clamped = glm::clamp(difference, -aabbHalfExtents, aabbHalfExtents);
	glm::vec2 closest = aabbCenter + clamped;

	difference = closest - center;

	if (glm::length(difference) < firstObj.radius) {
		return Collision(GL_TRUE, vectorDirection(difference), difference);
	}
	else {
		return Collision(GL_FALSE, UP, glm::vec2(0, 0));
	}
}

Direction Game::vectorDirection(glm::vec2 target) {
	glm::vec2 compass[] = {
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, -1.0f),
		glm::vec2(-1.0f, 0.0f),
	};

	GLfloat max = 0.0f;
	GLuint bestMatch = -1;
	for (GLuint i = 0; i < 4; i++) {
		GLfloat dotProduct = glm::dot(glm::normalize(target), compass[i]);

		if (dotProduct > max) {
			max = dotProduct;
			bestMatch = i;
		}
	}

	return (Direction)bestMatch;
}