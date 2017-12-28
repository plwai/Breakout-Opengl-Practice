#include "game.h"

using namespace irrklang;

const glm::vec2 PLAYER_SIZE(100, 20);
const GLfloat PLAYER_VELOCITY(500.0f);
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const GLfloat BALL_RADIUS = 12.5f;
GLfloat shakeTime = 0.0f;
ISoundEngine *SoundEngine = createIrrKlangDevice();

GLboolean shouldSpawn(GLuint chance) {
	GLuint random = rand() % chance;
	return random == 0;
}

GLboolean isOtherPowerUpActive(std::vector<PowerUp> &powerUps, std::string type)
{
	for (const PowerUp &powerUp : powerUps)
	{
		if (powerUp.activated)
			if (powerUp.type == type)
				return GL_TRUE;
	}
	return GL_FALSE;
}

void Game::activatePowerUp(PowerUp &powerUp)
{
	if (powerUp.type == "speed")
	{
		ball->velocity *= 1.2;
	}
	else if (powerUp.type == "sticky")
	{
		ball->sticky = GL_TRUE;
		player->color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.type == "pass-through")
	{
		ball->passThrough = GL_TRUE;
		ball->color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.type == "pad-size-increase")
	{
		player->size.x += 50;
	}
	else if (powerUp.type == "confuse")
	{
		if (!effects->chaos)
			effects->confuse = GL_TRUE;
	}
	else if (powerUp.type == "chaos")
	{
		if (!effects->confuse)
			effects->chaos = GL_TRUE;
	}
}

Game::Game(GLuint width, GLuint height)
	: state(GAME_ACTIVE), keys(), width(width), height(height) 
{
	this->path = ROOT_PATH;
}

Game::~Game() {
	delete Renderer;
	delete player;
	delete ball;
	delete particles;
	delete effects;
}

void Game::init() {
	ResourceManager::loadShader((this->path + "/src/shader/sprite.vert").c_str(), (this->path + "/src/shader/sprite.frag").c_str(), nullptr, "sprite");
	ResourceManager::loadShader((this->path + "/src/shader/particle.vert").c_str(), (this->path + "/src/shader/particle.frag").c_str(), nullptr, "particle");
	ResourceManager::loadShader((this->path + "/src/shader/postProcess.vert").c_str(), (this->path + "/src/shader/postProcess.frag").c_str(), nullptr, "effects");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->width), static_cast<GLfloat>(this->height), 0.0f, -1.0f, 1.0f);
	ResourceManager::getShader("sprite").use().setInteger("image", 0);
	ResourceManager::getShader("sprite").setMatrix4("projection", projection);
	ResourceManager::getShader("particle").use().setMatrix4("projection", projection);

	Renderer = new SpriteRenderer(ResourceManager::getShader("sprite"));
	text = new TextRenderer(this->width, this->height);
	text->load((this->path + "/assets/font/OCRAEXT.TTF").c_str(), 24);

	ResourceManager::loadTexture((this->path + "/assets/texture/paddle.png").c_str(), GL_TRUE, "paddle");
	ResourceManager::loadTexture((this->path + "/assets/texture/background.jpg").c_str(), GL_FALSE, "background");
	ResourceManager::loadTexture((this->path + "/assets/texture/tennisball512.png").c_str(), GL_TRUE, "face");
	ResourceManager::loadTexture((this->path + "/assets/texture/block.png").c_str(), GL_FALSE, "block");
	ResourceManager::loadTexture((this->path + "/assets/texture/block_solid.png").c_str(), GL_FALSE, "block_solid");
	ResourceManager::loadTexture((this->path + "/assets/texture/particle.png").c_str(), GL_TRUE, "particle");
	ResourceManager::loadTexture((this->path + "/assets/texture/powerup_speed.png").c_str(), GL_TRUE, "texSpeed");
	ResourceManager::loadTexture((this->path + "/assets/texture/powerup_sticky.png").c_str(), GL_TRUE, "texSticky");
	ResourceManager::loadTexture((this->path + "/assets/texture/powerup_passthrough.png").c_str(), GL_TRUE, "texPass");
	ResourceManager::loadTexture((this->path + "/assets/texture/powerup_increase.png").c_str(), GL_TRUE, "texSize");
	ResourceManager::loadTexture((this->path + "/assets/texture/powerup_confuse.png").c_str(), GL_TRUE, "texConfuse");
	ResourceManager::loadTexture((this->path + "/assets/texture/powerup_chaos.png").c_str(), GL_TRUE, "texChaos");

	GameLevel one; one.load((this->path + "/src/levels/level_1.lvl").c_str(), this->width, this->height * 0.5);
	GameLevel two; two.load((this->path + "/src/levels/level_2.lvl").c_str(), this->width, this->height * 0.5);
	GameLevel three; three.load((this->path + "/src/levels/level_3.lvl").c_str(), this->width, this->height * 0.5);
	GameLevel four; four.load((this->path + "/src/levels/level_4.lvl").c_str(), this->width, this->height * 0.5);

	this->levels.push_back(one);
	this->levels.push_back(two);
	this->levels.push_back(three);
	this->levels.push_back(four);
	this->setLevel(1);

	this->lives = 3;

	glm::vec2 playerPos = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);
	player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::getTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
	ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::getTexture("face"));
	ball->passThrough = GL_FALSE;
	ball->sticky = GL_FALSE;

	particles = new ParticleGenerator(
		ResourceManager::getShader("particle"),
		ResourceManager::getTexture("particle"),
		500
	);

	effects = new PostProcessor(ResourceManager::getShader("effects"), this->width, this->height);

	ResourceManager::loadTexture((this->path + "/assets/texture/powerup_chaos.png").c_str(), GL_TRUE, "texChaos");
	SoundEngine->play2D((this->path + "/assets/audio/bgm/breakout.mp3").c_str(), GL_TRUE);
}

void Game::update(GLfloat dt) {
	ball->move(dt, this->width);

	this->doCollisions();
	particles->update(dt, *ball, 2, glm::vec2(ball->radius / 2));

	if (ball->position.y >= this->height) {
		--this->lives;
		if (this->lives == 0) {
			this->resetLevel();
			this->state = GAME_MENU;
		}
		
		this->resetPlayer();
		this->powerUps.clear();
		effects->clear();
	}

	if (shakeTime > 0.0f) {
		shakeTime -= dt;
		if (shakeTime <= 0.0f)
			effects->shake = false;
	}

	if (this->state == GAME_ACTIVE && this->levels[this->level].isCompleted()) {
		this->resetLevel();
		this->resetPlayer();
		this->powerUps.clear();
		effects->clear();
		effects->chaos = GL_TRUE;
		this->state = GAME_WIN;
	}



	updatePowerUps(dt);
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

	if (this->state == GAME_MENU) {
		if (this->keys[GLFW_KEY_ENTER] && !this->keysProcessed[GLFW_KEY_ENTER]) {
			this->state = GAME_ACTIVE;
			this->keysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
		}
		if (this->keys[GLFW_KEY_W] && !this->keysProcessed[GLFW_KEY_W]) {
			this->level = (this->level + 1) % 4;
			this->keysProcessed[GLFW_KEY_W] = GL_TRUE;
		}
		if (this->keys[GLFW_KEY_S] && !this->keysProcessed[GLFW_KEY_S]) {
			if (this->level > 0) {
				--this->level;
			}
			else {
				this->level = 3;
			}

			this->keysProcessed[GLFW_KEY_S] = GL_TRUE;
		}
	}

	if (this->state == GAME_WIN) {
		if (this->keys[GLFW_KEY_ENTER]) {
			this->keysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			effects->chaos = GL_FALSE;
			this->state = GAME_MENU;
		}
	}
}

void Game::render() {
	if (this->state == GAME_ACTIVE || this->state == GAME_MENU) {
		effects->beginRender();
		Renderer->DrawSprite(ResourceManager::getTexture("background"),
				glm::vec2(0, 0), glm::vec2(this->width, this->height), 0.0f
			);

		this->levels[this->level].draw(*Renderer);

		player->draw(*Renderer);		
		particles->draw();
		ball->draw(*Renderer);
		for (PowerUp &powerUp : this->powerUps) {
			if (!powerUp.destroyed) {
				powerUp.draw(*Renderer);
			}
		}
		effects->endRender();
		effects->render(glfwGetTime());

		std::stringstream ss;
		ss << this->lives;
		text->renderText("Lives: " + ss.str(), 5.0f, 5.0f, 1.0f);
	}

	if (this->state == GAME_MENU) {
		text->renderText("Press ENTER to start", 250.0f, this->height / 2, 1.0f);
		text->renderText("Press W or S to select level", 245.0f, this->height / 2 + 20.0f, 0.75f);
	}

	if (this->state == GAME_WIN) {
		text->renderText( "You WON!!!", 320.0, this->height / 2 - 20.0, 1.0, glm::vec3(0.0, 1.0, 0.0));
		text->renderText("Press ENTER to retry or ESC to quit", 130.0, this->height / 2, 1.0, glm::vec3(1.0, 1.0, 0.0));
	}
}

void Game::doCollisions() {
	for (GameObject &box : this->levels[this->level].bricks) {
		if (!box.destroyed) {
			Collision collision = this->checkCollision(*ball, box);

			if (std::get<0>(collision)) {
				if (!box.isSolid) {
					box.destroyed = GL_TRUE;
					this->spawnPowerUps(box);
					SoundEngine->play2D((this->path + "/assets/audio/sounds/bleep.mp3").c_str(), GL_FALSE);
				}
				else {
					shakeTime = 0.05f;
					effects->shake = true;
					SoundEngine->play2D((this->path + "/assets/audio/sounds/solid.wav").c_str(), GL_FALSE);
				}

				Direction dir = std::get<1>(collision);
				glm::vec2 diffVector = std::get<2>(collision);

				if (!(ball->passThrough && !box.isSolid)) {
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

		ball->stuck = ball->sticky;
		SoundEngine->play2D((this->path + "/assets/audio/sounds/bleep.wav").c_str(), GL_FALSE);
	}

	for (PowerUp &powerUp : this->powerUps) {
		if (!powerUp.destroyed) {
			if (powerUp.position.y >= this->height) {
				powerUp.destroyed = GL_TRUE;
			}

			if (checkCollision(*player, powerUp)) {
				activatePowerUp(powerUp);
				powerUp.destroyed = GL_TRUE;
				powerUp.activated = GL_TRUE;

				SoundEngine->play2D((this->path + "/assets/audio/sounds/powerup.wav").c_str(), GL_FALSE);
			}
		}
	}
}

void Game::spawnPowerUps(GameObject &block) {
	if (shouldSpawn(75)) {
		this->powerUps.push_back(
			PowerUp("speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, block.position, ResourceManager::getTexture("texSpeed"))
		);
	}
	if (shouldSpawn(75)) {
		this->powerUps.push_back(
			PowerUp("sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f, block.position, ResourceManager::getTexture("texSticky"))
		);
	}
	if (shouldSpawn(75)) {
		this->powerUps.push_back(
			PowerUp("pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f, block.position, ResourceManager::getTexture("texPass"))
		);
	}
	if (shouldSpawn(75)) {
		this->powerUps.push_back(
			PowerUp("pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4), 0.0f, block.position, ResourceManager::getTexture("texSize"))
		);
	}
	if (shouldSpawn(15)) {
		this->powerUps.push_back(
			PowerUp("confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f, block.position, ResourceManager::getTexture("texConfuse"))
		);
	}
	if (shouldSpawn(15)) {
		this->powerUps.push_back(
			PowerUp("chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f, block.position, ResourceManager::getTexture("texChaos"))
		);
	}
}

void Game::updatePowerUps(GLfloat dt) {
	for (PowerUp &powerUp : this->powerUps) {
		powerUp.position += powerUp.velocity * dt;

		if (powerUp.activated) {
			powerUp.duration -= dt;

			if (powerUp.duration <= 0.0f) {
				powerUp.activated = GL_FALSE;

				if (powerUp.type == "sticky") {
					if (!isOtherPowerUpActive(this->powerUps, "sticky")) {
						ball->sticky = GL_FALSE;
						player->color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.type == "pass-through") {
					if (!isOtherPowerUpActive(this->powerUps, "pass-through")) {
						ball->passThrough = GL_FALSE;
						player->color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.type == "confuse") {
					if (!isOtherPowerUpActive(this->powerUps, "confuse")) {
						effects->confuse = GL_FALSE;
					}
				}
				else if (powerUp.type == "chaos") {
					if (!isOtherPowerUpActive(this->powerUps, "chaos")) {
						effects->chaos = GL_FALSE;
					}
				}
			}
		}
	}

	this->powerUps.erase(std::remove_if(this->powerUps.begin(), this->powerUps.end(),
		[](const PowerUp &powerUp) { return powerUp.destroyed && !powerUp.activated; }
	), this->powerUps.end());
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

	this->lives = 3;
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