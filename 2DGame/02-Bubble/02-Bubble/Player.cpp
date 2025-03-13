#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 50
#define FALL_STEP 4


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT, PROTECT_LEFT, PROTECT_RIGHT, DIE, ATTACK_LEFT, ATTACK_RIGHT, DOWN_LEFT, DOWN_RIGHT, ATTACK_DOWN_LEFT, ATTACK_DOWN_RIGHT, ATTACK_RIGHT_MOVING, ATTACK_LEFT_MOVING
};

enum LanzaActions {
	THROW_LEFT, STANDS_LEFT, THROW_RIGHT, STANDS_RIGHT
};

void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	first_attack = false;
	spear_visible = false;
	bJumping = false;
	spritesheet.loadFromFile("images/soaring_eagle4.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.2, 0.2), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(17);
	
		sprite->setAnimationSpeed(STAND_LEFT, 8);
		sprite->addKeyframe(STAND_LEFT, glm::vec2(0.8f, 0.6f));
		
		sprite->setAnimationSpeed(STAND_RIGHT, 8);
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.2f));
		
		sprite->setAnimationSpeed(MOVE_LEFT, 8);
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.8f, 0.4f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.8f, 0.2f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.8f, 0.f));
		
		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.2, 0.2f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.4, 0.2f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.6f, 0.2f));

		sprite->setAnimationSpeed(JUMP_LEFT, 8);
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.8f, 0.8f));

		sprite->setAnimationSpeed(JUMP_RIGHT, 8);
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.4f, 0.f));

		sprite->setAnimationSpeed(PROTECT_LEFT, 8);
		sprite->addKeyframe(PROTECT_LEFT, glm::vec2(0.6f, 0.8f));

		sprite->setAnimationSpeed(PROTECT_RIGHT, 8);
		sprite->addKeyframe(PROTECT_RIGHT, glm::vec2(0.f, 0.6f));

		sprite->setAnimationSpeed(DIE, 8);
		sprite->addKeyframe(DIE, glm::vec2(0.4f, 0.6f));

		sprite->setAnimationSpeed(ATTACK_LEFT, 8);
		sprite->addKeyframe(ATTACK_LEFT, glm::vec2(0.2f, 0.8f));

		sprite->setAnimationSpeed(ATTACK_RIGHT, 8);
		sprite->addKeyframe(ATTACK_RIGHT, glm::vec2(0.f, 0.4f));

		sprite->setAnimationSpeed(DOWN_LEFT, 8);
		sprite->addKeyframe(DOWN_LEFT, glm::vec2(0.8f, 0.8f));

		sprite->setAnimationSpeed(DOWN_RIGHT, 8);
		sprite->addKeyframe(DOWN_RIGHT, glm::vec2(0.4f, 0.f));

		sprite->setAnimationSpeed(ATTACK_DOWN_LEFT, 8);
		sprite->addKeyframe(ATTACK_DOWN_LEFT, glm::vec2(0.2f, 0.6f));

		sprite->setAnimationSpeed(ATTACK_DOWN_RIGHT, 8);
		sprite->addKeyframe(ATTACK_DOWN_RIGHT, glm::vec2(0.6f, 0.f));

		sprite->setAnimationSpeed(ATTACK_RIGHT_MOVING, 8);
		sprite->addKeyframe(ATTACK_RIGHT_MOVING, glm::vec2(0.2f, 0.4f));
		sprite->addKeyframe(ATTACK_RIGHT_MOVING, glm::vec2(0.4f, 0.4f));
		sprite->addKeyframe(ATTACK_RIGHT_MOVING, glm::vec2(0.6f, 0.4f));

		sprite->setAnimationSpeed(ATTACK_LEFT_MOVING, 8);
		sprite->addKeyframe(ATTACK_LEFT_MOVING, glm::vec2(0.f, 0.8f));
		sprite->addKeyframe(ATTACK_LEFT_MOVING, glm::vec2(0.2f, 0.f));
		sprite->addKeyframe(ATTACK_LEFT_MOVING, glm::vec2(0.0f, 0.f));


		spritesheet_lanza.loadFromFile("images/lanzas.png", TEXTURE_PIXEL_FORMAT_RGBA);
		sprite_lanza = Sprite::createSprite(glm::ivec2(48, 16), glm::vec2(0.33, 0.25), &spritesheet_lanza, &shaderProgram);
		sprite_lanza->setNumberAnimations(4);

		sprite_lanza->setAnimationSpeed(THROW_LEFT, 30);
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.33f, 0.f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.66f, 0.f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.66f, 0.25f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.66f, 0.5f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.f, 0.75f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.66f, 0.5f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.66f, 0.25f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.66f, 0.f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.33f, 0.f));

		sprite_lanza->setAnimationSpeed(STANDS_LEFT, 8);
		sprite_lanza->addKeyframe(STANDS_LEFT, glm::vec2(0.33f, 0.f));

		sprite_lanza->setAnimationSpeed(THROW_RIGHT, 30);
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.33f, 0.25f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.25f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.5f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.33f, 0.5f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.5f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.25f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.33f, 0.25f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.f));

		sprite_lanza->setAnimationSpeed(STANDS_RIGHT, 8);
		sprite_lanza->addKeyframe(STANDS_RIGHT, glm::vec2(0.f, 0.f));
		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

	sprite_lanza->changeAnimation(0);
	
}

void Player::update(int deltaTime)
{
	
	sprite_lanza->update(deltaTime);
	sprite->update(deltaTime);
	if (Game::instance().getKey(GLFW_KEY_DOWN)) {
		if (dir == LEFT) {
			if (Game::instance().getKey(GLFW_KEY_X)) {
				if(sprite -> animation() != ATTACK_DOWN_LEFT)sprite->changeAnimation(ATTACK_DOWN_LEFT);
				spear_visible = true;

				if (sprite_lanza->animation() != THROW_LEFT && !first_attack) {
					sprite_lanza->changeAnimation(THROW_LEFT);
				}
				else if (sprite_lanza->animation() == THROW_LEFT && sprite_lanza->isAnimationFinished()) {
					first_attack = true;
					sprite_lanza->changeAnimation(STANDS_LEFT);
				}
				else if (sprite_lanza->animation() != THROW_LEFT && sprite_lanza->animation() != STANDS_LEFT) {
					sprite_lanza->changeAnimation(STANDS_LEFT);
				}
				sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 44), float(tileMapDispl.y + posPlayer.y + 18)));

			}
				
			else
				sprite->changeAnimation(DOWN_LEFT);
		}
		else if (dir == RIGHT) {
			if (Game::instance().getKey(GLFW_KEY_X)) {
				if(sprite -> animation() != ATTACK_DOWN_RIGHT) sprite->changeAnimation(ATTACK_DOWN_RIGHT);
				spear_visible = true;

				if (sprite_lanza->animation() != THROW_RIGHT && !first_attack) {

					sprite_lanza->changeAnimation(THROW_RIGHT);
				}
				else if (sprite_lanza->animation() == THROW_RIGHT && sprite_lanza->isAnimationFinished()) {

					first_attack = true;
					sprite_lanza->changeAnimation(STANDS_RIGHT);
				}
				else if (sprite_lanza->animation() != THROW_RIGHT && sprite_lanza->animation() != STANDS_RIGHT) {
					first_attack = true;
					sprite_lanza->changeAnimation(STANDS_RIGHT);
				}
				sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x + 28), float(tileMapDispl.y + posPlayer.y + 19)));

			}
				
			else
				sprite->changeAnimation(DOWN_RIGHT);
		}
	}
	else if(Game::instance().getKey(GLFW_KEY_LEFT))
	{
		
		dir = LEFT;
		if (Game::instance().getKey(GLFW_KEY_X)) {
			spear_visible = true;
			if (sprite->animation() != ATTACK_LEFT_MOVING)
				sprite->changeAnimation(ATTACK_LEFT_MOVING);
				
			if (sprite_lanza->animation() != THROW_LEFT && !first_attack) {
				sprite_lanza->changeAnimation(THROW_LEFT);
			}
			else if (sprite_lanza->animation() == THROW_LEFT && sprite_lanza->isAnimationFinished()) {
				first_attack = true;
				sprite_lanza->changeAnimation(STANDS_LEFT);
			}
			else if(sprite_lanza -> animation() != THROW_LEFT && sprite_lanza -> animation() != STANDS_LEFT){
				sprite_lanza->changeAnimation(STANDS_LEFT);
			}

			sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 43), float(tileMapDispl.y + posPlayer.y + 10)));

		}
		else {
			if (sprite->animation() != MOVE_LEFT)
				sprite->changeAnimation(MOVE_LEFT);
		}
			
		posPlayer.x -= 2;
		if(map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x += 2;
			sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if(Game::instance().getKey(GLFW_KEY_RIGHT))
	{
		
		dir = RIGHT;
		if (Game::instance().getKey(GLFW_KEY_X)) {
			spear_visible = true;
			if (sprite->animation() != ATTACK_RIGHT_MOVING)
				sprite->changeAnimation(ATTACK_RIGHT_MOVING);
			if (sprite_lanza->animation() != THROW_RIGHT && !first_attack) {
				
				sprite_lanza->changeAnimation(THROW_RIGHT);
			}
			else if (sprite_lanza->animation() == THROW_RIGHT && sprite_lanza->isAnimationFinished()) {
				
				first_attack = true;
				sprite_lanza->changeAnimation(STANDS_RIGHT);
			}
			else if(sprite_lanza->animation() != THROW_RIGHT && sprite_lanza -> animation() != STANDS_RIGHT){
				first_attack = true;
				sprite_lanza->changeAnimation(STANDS_RIGHT);
			}
			sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x + 26), float(tileMapDispl.y + posPlayer.y + 11)));
		}
		else {
			if (sprite->animation() != MOVE_RIGHT)
				sprite->changeAnimation(MOVE_RIGHT);
		}
		posPlayer.x += 2;
		if(map->collisionMoveRight(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x -= 2;
			sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_UP))
	{
		if (dir == LEFT)
			sprite->changeAnimation(PROTECT_LEFT);
		else if (dir == RIGHT)
			sprite->changeAnimation(PROTECT_RIGHT);
	}
	else if (Game::instance().getKey(GLFW_KEY_X))
	{
		spear_visible = true;
		if (dir == LEFT) {
			if (sprite->animation() != ATTACK_LEFT) sprite->changeAnimation(ATTACK_LEFT);

			if (sprite_lanza->animation() != THROW_LEFT && !first_attack) {
				sprite_lanza->changeAnimation(THROW_LEFT);
			}
			else if (sprite_lanza->animation() == THROW_LEFT && sprite_lanza->isAnimationFinished()) {
				first_attack = true;
				sprite_lanza->changeAnimation(STANDS_LEFT);
			}


			sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 43), float(tileMapDispl.y + posPlayer.y + 10)));
		}
		else if (dir == RIGHT) {
			if (sprite->animation() != ATTACK_RIGHT) sprite->changeAnimation(ATTACK_RIGHT);

			if (sprite_lanza->animation() != THROW_RIGHT && !first_attack) {
				sprite_lanza->changeAnimation(THROW_RIGHT);
			}
			else if (sprite_lanza->animation() == THROW_RIGHT && sprite_lanza->isAnimationFinished()) {
				first_attack = true;
				sprite_lanza->changeAnimation(STANDS_RIGHT);
			}

			sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x + 26), float(tileMapDispl.y + posPlayer.y + 11)));
		}
			
	}
	else
	{
		
		if(dir == LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if(dir == RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}

	if (spear_visible)
	{
		
		if (!Game::instance().getKey(GLFW_KEY_X))
		{
			spear_visible = false;
			first_attack = false;
		}
	}
	
	if(bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if(jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			if(jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y);
		}
		if (dir == LEFT)
			sprite->changeAnimation(JUMP_LEFT);
		else
			sprite->changeAnimation(JUMP_RIGHT);

	}
	else
	{
		
		posPlayer.y += FALL_STEP;
		if(map->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y))
		{
			if(Game::instance().getKey(GLFW_KEY_Z))
			{
				bJumping = true;
				jumpAngle = 0;
				startY = posPlayer.y;
			}
		}
	}
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

void Player::render()
{
	sprite->render();
	if(spear_visible)sprite_lanza->render();
}

void Player::setTileMap(TileMap *tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2 &pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

glm::ivec2 Player::getPosition()
{
	return posPlayer;
}



