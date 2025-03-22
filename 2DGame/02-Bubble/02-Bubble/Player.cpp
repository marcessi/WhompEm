#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Player.h"
#include "Game.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 50
#define FALL_STEP 3


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT, PROTECT_LEFT, PROTECT_RIGHT, DIE_LEFT, DIE_RIGHT, ATTACK_LEFT, ATTACK_RIGHT, DOWN_LEFT, DOWN_RIGHT, ATTACK_DOWN_LEFT, ATTACK_DOWN_RIGHT, ATTACK_RIGHT_MOVING, ATTACK_LEFT_MOVING, ATTACK_JUMPING_RIGHT, ATTACK_JUMPING_LEFT, ATTACK_FALLING_RIGHT,ATTACK_FALLING_LEFT
};

enum LanzaActions {
	THROW_LEFT, STANDS_LEFT, THROW_RIGHT, STANDS_RIGHT, UP, DOWN
};

void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	hearts = glm::vec4(4, 4, 4, 4);
	ligths = 2;
	first_attack = false;
	spear_visible = false;
	movingPlatforms = nullptr;
	bJumping = false;
	leftLimit = 0.f;
	spritesheet.loadFromFile("images/soaring_eagle5.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.125, 0.125), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(22);
	
		sprite->setAnimationSpeed(STAND_LEFT, 8);
		sprite->addKeyframe(STAND_LEFT, glm::vec2(0.500f, 0.375f));
		
		sprite->setAnimationSpeed(STAND_RIGHT, 8);
		sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.f, 0.125f));
		
		sprite->setAnimationSpeed(MOVE_LEFT, 8);
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.500f, 0.250f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.500f, 0.125f));
		sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.500f, 0.f));
		
		sprite->setAnimationSpeed(MOVE_RIGHT, 8);
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.125, 0.125f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.250, 0.125f));
		sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.375f, 0.125f));

		sprite->setAnimationSpeed(JUMP_LEFT, 8);
		sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.500f, 0.500f));

		sprite->setAnimationSpeed(JUMP_RIGHT, 8);
		sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.250f, 0.f));

		sprite->setAnimationSpeed(PROTECT_LEFT, 8);
		sprite->addKeyframe(PROTECT_LEFT, glm::vec2(0.375f, 0.500f));

		sprite->setAnimationSpeed(PROTECT_RIGHT, 8);
		sprite->addKeyframe(PROTECT_RIGHT, glm::vec2(0.f, 0.375f));

		sprite->setAnimationSpeed(DIE_RIGHT, 8);
		sprite->addKeyframe(DIE_RIGHT, glm::vec2(0.250f, 0.375f));
		

		sprite->setAnimationSpeed(DIE_LEFT, 8);
		sprite->addKeyframe(DIE_LEFT, glm::vec2(0.625f, 0.250f));

		sprite->setAnimationSpeed(ATTACK_LEFT, 8);
		sprite->addKeyframe(ATTACK_LEFT, glm::vec2(0.125f, 0.500f));

		sprite->setAnimationSpeed(ATTACK_RIGHT, 8);
		sprite->addKeyframe(ATTACK_RIGHT, glm::vec2(0.f, 0.250f));

		sprite->setAnimationSpeed(DOWN_LEFT, 8);
		sprite->addKeyframe(DOWN_LEFT, glm::vec2(0.500f, 0.500f));

		sprite->setAnimationSpeed(DOWN_RIGHT, 8);
		sprite->addKeyframe(DOWN_RIGHT, glm::vec2(0.250f, 0.f));

		sprite->setAnimationSpeed(ATTACK_DOWN_LEFT, 8);
		sprite->addKeyframe(ATTACK_DOWN_LEFT, glm::vec2(0.125f, 0.375f));

		sprite->setAnimationSpeed(ATTACK_DOWN_RIGHT, 8);
		sprite->addKeyframe(ATTACK_DOWN_RIGHT, glm::vec2(0.375f, 0.f));

		sprite->setAnimationSpeed(ATTACK_RIGHT_MOVING, 8);
		sprite->addKeyframe(ATTACK_RIGHT_MOVING, glm::vec2(0.125f, 0.250f));
		sprite->addKeyframe(ATTACK_RIGHT_MOVING, glm::vec2(0.250f, 0.250f));
		sprite->addKeyframe(ATTACK_RIGHT_MOVING, glm::vec2(0.375f, 0.250f));

		sprite->setAnimationSpeed(ATTACK_LEFT_MOVING, 8);
		sprite->addKeyframe(ATTACK_LEFT_MOVING, glm::vec2(0.f, 0.500f));
		sprite->addKeyframe(ATTACK_LEFT_MOVING, glm::vec2(0.125f, 0.f));
		sprite->addKeyframe(ATTACK_LEFT_MOVING, glm::vec2(0.0f, 0.f));

		sprite->setAnimationSpeed(ATTACK_JUMPING_RIGHT, 8);
		sprite->addKeyframe(ATTACK_JUMPING_RIGHT, glm::vec2(0.250,0.500));

		sprite->setAnimationSpeed(ATTACK_JUMPING_LEFT, 8);
		sprite->addKeyframe(ATTACK_JUMPING_LEFT, glm::vec2(0.375, 0.375));


		sprite->setAnimationSpeed(ATTACK_FALLING_RIGHT, 8);
		sprite->addKeyframe(ATTACK_FALLING_RIGHT, glm::vec2(0.625,0.500));
		 
		sprite->setAnimationSpeed(ATTACK_FALLING_LEFT, 8);
		sprite->addKeyframe(ATTACK_FALLING_LEFT, glm::vec2(0.625,0.375));


		spritesheet_lanza.loadFromFile("images/lanzas1.png", TEXTURE_PIXEL_FORMAT_RGBA);
		sprite_lanza = Sprite::createSprite(glm::ivec2(48, 16), glm::vec2(0.333, 0.25), &spritesheet_lanza, &shaderProgram);
		sprite_lanza->setNumberAnimations(6);

		sprite_lanza->setAnimationSpeed(THROW_LEFT, 30);
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.333f, 0.f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.666f, 0.f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.666f, 0.25f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.666f, 0.5f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.f, 0.75f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.666f, 0.5f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.666f, 0.25f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.666f, 0.f));
		sprite_lanza->addKeyframe(THROW_LEFT, glm::vec2(0.333f, 0.f));

		sprite_lanza->setAnimationSpeed(STANDS_LEFT, 8);
		sprite_lanza->addKeyframe(STANDS_LEFT, glm::vec2(0.333f, 0.f));

		sprite_lanza->setAnimationSpeed(THROW_RIGHT, 30);
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.333f, 0.25f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.25f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.5f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.333f, 0.5f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.5f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.25f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.333f, 0.25f));
		sprite_lanza->addKeyframe(THROW_RIGHT, glm::vec2(0.f, 0.f));

		sprite_lanza->setAnimationSpeed(STANDS_RIGHT, 8);
		sprite_lanza->addKeyframe(STANDS_RIGHT, glm::vec2(0.f, 0.f));
		
		sprite_lanza->setAnimationSpeed(UP, 8);
		sprite_lanza->addKeyframe(UP, glm::vec2(0.333,0.75));

		sprite_lanza->setAnimationSpeed(DOWN, 8);
		sprite_lanza->addKeyframe(DOWN, glm::vec2(0.666,0.75));

	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

	sprite_lanza->changeAnimation(0);
	
}

void Player::update(int deltaTime)
{
	
	

    sprite_lanza->update(deltaTime);
    sprite->update(deltaTime);

    // Guardar la posición previa para restaurarla en caso de colisión
    glm::ivec2 prevPos = posPlayer;

	int hitboxWidth = 16;
	int hitboxHeight = 32;
	int hitboxOffsetX = (32 - hitboxWidth) / 2;

	if (Game::instance().getKey(GLFW_KEY_DOWN))
	{
		if (Game::instance().getKey(GLFW_KEY_LEFT)) {
			dir = LEFT;
		}
		else if (Game::instance().getKey(GLFW_KEY_RIGHT)) {
			dir = RIGHT;
		}
		if (dir == LEFT) {
			if (Game::instance().getKey(GLFW_KEY_X)) {
				if (sprite->animation() != ATTACK_DOWN_LEFT)sprite->changeAnimation(ATTACK_DOWN_LEFT);
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
				if (sprite->animation() != ATTACK_DOWN_RIGHT) sprite->changeAnimation(ATTACK_DOWN_RIGHT);
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
				sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x + 28), float(tileMapDispl.y + posPlayer.y + 18)));

			}

			else
				sprite->changeAnimation(DOWN_RIGHT);
		}
	}
	else if (Game::instance().getKey(GLFW_KEY_UP))
	{
		if (Game::instance().getKey(GLFW_KEY_LEFT)) {
			dir = LEFT;
		}
		else if (Game::instance().getKey(GLFW_KEY_RIGHT)) {
			dir = RIGHT;
		}
		if (dir == LEFT)
			sprite->changeAnimation(PROTECT_LEFT);
		else if (dir == RIGHT)
			sprite->changeAnimation(PROTECT_RIGHT);
	}
    // Manejar el movimiento horizontal
    else if (Game::instance().getKey(GLFW_KEY_LEFT))
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
			else if (sprite_lanza->animation() != THROW_LEFT && sprite_lanza->animation() != STANDS_LEFT) {
				sprite_lanza->changeAnimation(STANDS_LEFT);
			}

			sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 43), float(tileMapDispl.y + posPlayer.y + 10)));

		}
		else {
			if (sprite->animation() != MOVE_LEFT)
				sprite->changeAnimation(MOVE_LEFT);
		}

        posPlayer.x -= 2;

		if (mapWalls->collisionMoveLeft(glm::ivec2(posPlayer.x + hitboxOffsetX, posPlayer.y),
			glm::ivec2(hitboxWidth, hitboxHeight)) || posPlayer.x < leftLimit)
		{
			posPlayer.x = prevPos.x;
			sprite->changeAnimation(STAND_LEFT);
		}
    }
    else if (Game::instance().getKey(GLFW_KEY_RIGHT))
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
			else if (sprite_lanza->animation() != THROW_RIGHT && sprite_lanza->animation() != STANDS_RIGHT) {
				first_attack = true;
				sprite_lanza->changeAnimation(STANDS_RIGHT);
			}
			sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x + 26), float(tileMapDispl.y + posPlayer.y + 10)));
		}
		else {
			if (sprite->animation() != MOVE_RIGHT)
				sprite->changeAnimation(MOVE_RIGHT);
		}
        posPlayer.x += 2;
        // Solo comprobar colisiones con mapWalls, no con mapPlatforms
		if (mapWalls->collisionMoveRight(glm::ivec2(posPlayer.x + hitboxOffsetX, posPlayer.y),
			glm::ivec2(hitboxWidth, hitboxHeight)) || posPlayer.x > 4064.f)
		{
			posPlayer.x = prevPos.x;
			sprite->changeAnimation(STAND_RIGHT);
		}
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

			sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x + 26), float(tileMapDispl.y + posPlayer.y + 10)));
		}
    }
    else
    {
        if (dir == LEFT)
            sprite->changeAnimation(STAND_LEFT);
        else if (dir == RIGHT)
            sprite->changeAnimation(STAND_RIGHT);
    }

	if (spear_visible)
	{

		if (!Game::instance().getKey(GLFW_KEY_X))
		{

			if(!Game::instance().getKey(GLFW_KEY_1) && !Game::instance().getKey(GLFW_KEY_2)) spear_visible = false;
			first_attack = false;
		}
	}

	if (bJumping)
	{
		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			// Guardar posición Y antes de saltar
			int prevY = posPlayer.y;

			// Calcular nueva posición Y
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));

			// Verificar si el jugador está subiendo o bajando
			bool movingDown = posPlayer.y > prevY;

			if (Game::instance().getKey(GLFW_KEY_DOWN)) {
				spear_visible = true;
				if (sprite_lanza->animation() != DOWN) sprite_lanza->changeAnimation(DOWN);
				if (dir == RIGHT) {
					if (sprite->animation() != ATTACK_FALLING_RIGHT) sprite->changeAnimation(ATTACK_FALLING_RIGHT);
					sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 13), float(tileMapDispl.y + posPlayer.y + 32)));

				}
				else {
					if (sprite->animation() != ATTACK_FALLING_LEFT) sprite->changeAnimation(ATTACK_FALLING_LEFT);
					sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 21), float(tileMapDispl.y + posPlayer.y + 32)));

				}

			}
			else if (Game::instance().getKey(GLFW_KEY_UP)) {
				spear_visible = true;

				if (sprite_lanza->animation() != UP) sprite_lanza->changeAnimation(UP);
				if (dir == RIGHT) {
					if (sprite->animation() != ATTACK_JUMPING_RIGHT) sprite->changeAnimation(ATTACK_JUMPING_RIGHT);
					sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 10), float(tileMapDispl.y + posPlayer.y - 16)));

				}
				else {
					if (sprite->animation() != ATTACK_JUMPING_LEFT) sprite->changeAnimation(ATTACK_JUMPING_LEFT);
					sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 16), float(tileMapDispl.y + posPlayer.y - 16)));

				}

			}
			else {
				if (dir == LEFT)
					sprite->changeAnimation(JUMP_LEFT);
				else
					sprite->changeAnimation(JUMP_RIGHT);
			}

			if (jumpAngle > 90) {
				// Primero comprobar colisiones con paredes sólidas (mapWalls)
				if (mapWalls->collisionMoveDown(glm::ivec2(posPlayer.x + hitboxOffsetX, posPlayer.y),
					glm::ivec2(hitboxWidth, hitboxHeight), &posPlayer.y)) {
					bJumping = false;
				}
				// Luego comprobar colisiones con plataformas solo si está cayendo
				else if (movingDown && mapPlatforms->collisionMoveDown(glm::ivec2(posPlayer.x + hitboxOffsetX, posPlayer.y),
					glm::ivec2(hitboxWidth, hitboxHeight), &posPlayer.y)) {
					bJumping = false;
				}
				// Finalmente, comprobar colisiones con plataformas móviles
				else if (movingDown && movingPlatforms != nullptr) {
					for (auto platform : *movingPlatforms) {
						float tempPosY = posPlayer.y;
						if (platform->collisionMoveDown(glm::ivec2(posPlayer.x + hitboxOffsetX, posPlayer.y),
							glm::ivec2(hitboxWidth, hitboxHeight), &tempPosY)) {
							posPlayer.y = tempPosY;
							bJumping = false;
							break;
						}
					}
				}
			}
        }			
    }
    else
    {
        
        int prevY = posPlayer.y; // Guardamos la posición Y antes de caer
        posPlayer.y += FALL_STEP;

		// Verificar que el jugador está cayendo
		bool isFalling = posPlayer.y > prevY;

		// Primero comprobar colisiones con paredes sólidas (mapWalls)
		bool collisionWithWalls = mapWalls->collisionMoveDown(glm::ivec2(posPlayer.x + hitboxOffsetX, posPlayer.y),
			glm::ivec2(hitboxWidth, hitboxHeight), &posPlayer.y);

		// Luego comprobar colisiones con plataformas estáticas solo si está cayendo
		bool collisionWithPlatforms = false;
		if (isFalling) {
			collisionWithPlatforms = mapPlatforms->collisionMoveDown(glm::ivec2(posPlayer.x + hitboxOffsetX, posPlayer.y),
				glm::ivec2(hitboxWidth, hitboxHeight), &posPlayer.y);
		}

		// Comprobar colisiones con plataformas móviles
		bool collisionWithMovingPlatforms = false;
		if (isFalling && movingPlatforms != nullptr) {
			for (auto platform : *movingPlatforms) {
				float tempPosY = posPlayer.y;
				if (platform->collisionMoveDown(glm::ivec2(posPlayer.x + hitboxOffsetX, posPlayer.y),
					glm::ivec2(hitboxWidth, hitboxHeight), &tempPosY)) {
					posPlayer.y = tempPosY;
					collisionWithMovingPlatforms = true;
					break;
				}
			}
		}

        // Comprobar colisiones con el suelo
        if (collisionWithWalls || collisionWithPlatforms || collisionWithMovingPlatforms)
        {
            if (Game::instance().getKey(GLFW_KEY_Z))
            {
                bJumping = true;
                jumpAngle = 0;
                startY = posPlayer.y;
            }
        }
		else {
			if (Game::instance().getKey(GLFW_KEY_DOWN)) {
				spear_visible = true;
				if (sprite_lanza->animation() != DOWN) sprite_lanza->changeAnimation(DOWN);
				if (dir == RIGHT) {
					if (sprite->animation() != ATTACK_FALLING_RIGHT) sprite->changeAnimation(ATTACK_FALLING_RIGHT);
					sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 13), float(tileMapDispl.y + posPlayer.y + 32)));
				}
				else {
					if (sprite->animation() != ATTACK_FALLING_LEFT) sprite->changeAnimation(ATTACK_FALLING_LEFT);
					sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 21), float(tileMapDispl.y + posPlayer.y + 32)));
				}
			}
			else if (Game::instance().getKey(GLFW_KEY_UP)) {
				spear_visible = true;
				if (sprite_lanza->animation() != UP) sprite_lanza->changeAnimation(UP);
				if (dir == RIGHT) {
					if (sprite->animation() != ATTACK_JUMPING_RIGHT) sprite->changeAnimation(ATTACK_JUMPING_RIGHT);
					sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 10), float(tileMapDispl.y + posPlayer.y - 16)));
				}
				else {
					if (sprite->animation() != ATTACK_JUMPING_LEFT) sprite->changeAnimation(ATTACK_JUMPING_LEFT);
					sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x - 16), float(tileMapDispl.y + posPlayer.y - 16)));
				}
			}
			else {
				if (dir == LEFT)
					if (sprite->animation() != JUMP_LEFT) sprite->changeAnimation(JUMP_LEFT);
				else
						if (sprite->animation() != JUMP_RIGHT) sprite->changeAnimation(JUMP_RIGHT);
			}
		}
    }
	if (hitted) {
		if (dir == LEFT) {
			if (sprite->animation() != DIE_LEFT) sprite->changeAnimation(DIE_LEFT);
		}
		else {
			if (sprite->animation() != DIE_RIGHT) sprite->changeAnimation(DIE_RIGHT);
		}
	}


	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}



void Player::render()
{
	sprite->render();
	if(spear_visible)sprite_lanza->render();
}

void Player::setTileMap(TileMap *tileMapWalls, TileMap* tileMapPlatforms)
{
	mapWalls = tileMapWalls;
	mapPlatforms = tileMapPlatforms;
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

void Player::setLeftLimit(float leftLimit)
{
	this->leftLimit = leftLimit;
}

void Player::setMovingPlatforms(const std::vector<MovingPlatform*>* platforms)
{
	movingPlatforms = platforms;
}

pair<glm::ivec4, int> Player::getplayerLifes() {
	 pair< glm::ivec4, int> aux;
	 aux.first = hearts;
	 aux.second = ligths;
	 return aux;

}

void Player::isHitted() {
	hitted = true;
	if (hearts.x > 0) {
		hearts.x--;
	}
	else if (hearts.y > 0) {
		hearts.y--;
	}
	else if (hearts.z > 0) {
		hearts.z--;
	}
	else if (hearts.w > 0) {
		hearts.w--;
	}
	else {
		ligths--;
		if (ligths == 0) {
			gameover = true;
		}
		
	}
}
