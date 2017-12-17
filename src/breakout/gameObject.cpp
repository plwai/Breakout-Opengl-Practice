#include "gameObject.h"

GameObject::GameObject()
	: position(0, 0), size(1, 1), velocity(0.0f), color(1.0f), rotation(0.0f), sprite(), isSolid(false), destroyed(false) {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
	: position(pos), size(size), velocity(velocity), color(color), rotation(0.0f), sprite(sprite), isSolid(false), destroyed(false) {}

void GameObject::Draw(SpriteRenderer &renderer) {
	renderer.DrawSprite(this->sprite, this->position, this->size, this->rotation, this->color);
}