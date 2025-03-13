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
	THROW, STAND
};

void Player::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram)
{
	spear_visible = false;
	bJumping = false;
	leftLimit = 0.f;
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


		spritesheet_lanza.loadFromFile("images/lanza.png", TEXTURE_PIXEL_FORMAT_RGBA);
		sprite_lanza = Sprite::createSprite(glm::ivec2(48, 16), glm::vec2(0.5, 0.33), &spritesheet_lanza, &shaderProgram);
		sprite_lanza->setNumberAnimations(2);

		sprite_lanza->setAnimationSpeed(THROW, 30);
		sprite_lanza->addKeyframe(THROW, glm::vec2(0.f, 0.f));
		sprite_lanza->addKeyframe(THROW, glm::vec2(0.5f, 0.33f));
		sprite_lanza->addKeyframe(THROW, glm::vec2(5.f, 0.33f));

		sprite_lanza->setAnimationSpeed(STAND, 8);
		sprite_lanza->addKeyframe(STAND, glm::vec2(0.f, 0.f));


		
	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

	sprite_lanza->changeAnimation(0);
	sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x +32), float(tileMapDispl.y + posPlayer.y + -7)));
	
}

void Player::update(int deltaTime)
{
    sprite_lanza->update(deltaTime);
    sprite->update(deltaTime);

    // Guardar la posición previa para restaurarla en caso de colisión
    glm::ivec2 prevPos = posPlayer;

    // Manejar el movimiento horizontal
    if (Game::instance().getKey(GLFW_KEY_LEFT))
    {
        dir = LEFT;
        if (Game::instance().getKey(GLFW_KEY_X)) {
            if (sprite->animation() != ATTACK_LEFT_MOVING)
                sprite->changeAnimation(ATTACK_LEFT_MOVING);
        }
        else {
            if (sprite->animation() != MOVE_LEFT)
                sprite->changeAnimation(MOVE_LEFT);
        }

        posPlayer.x -= 2;
        // Solo comprobar colisiones con mapWalls, no con mapPlatforms
        if (mapWalls->collisionMoveLeft(posPlayer, glm::ivec2(32, 32)) || posPlayer.x < leftLimit)
        {
            posPlayer.x = prevPos.x;
            sprite->changeAnimation(STAND_LEFT);
        }
    }
    else if (Game::instance().getKey(GLFW_KEY_RIGHT))
    {
        dir = RIGHT;
        if (Game::instance().getKey(GLFW_KEY_X)) {
            if (sprite->animation() != ATTACK_RIGHT_MOVING)
                sprite->changeAnimation(ATTACK_RIGHT_MOVING);
        }
        else {
            if (sprite->animation() != MOVE_RIGHT)
                sprite->changeAnimation(MOVE_RIGHT);
        }
        posPlayer.x += 2;
        // Solo comprobar colisiones con mapWalls, no con mapPlatforms
        if (mapWalls->collisionMoveRight(posPlayer, glm::ivec2(32, 32)) || posPlayer.x > 4064.f)
        {
            posPlayer.x = prevPos.x;
            sprite->changeAnimation(STAND_RIGHT);
        }
    }
    else if (Game::instance().getKey(GLFW_KEY_DOWN))
    {
        if (dir == LEFT) {
            if (Game::instance().getKey(GLFW_KEY_X))
                sprite->changeAnimation(ATTACK_DOWN_LEFT);
            else
                sprite->changeAnimation(DOWN_LEFT);
        }
        else if (dir == RIGHT) {
            if (Game::instance().getKey(GLFW_KEY_X))
                sprite->changeAnimation(ATTACK_DOWN_RIGHT);
            else
                sprite->changeAnimation(DOWN_RIGHT);
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
            sprite->changeAnimation(ATTACK_LEFT);
            sprite_lanza->changeAnimation(THROW);
            sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x + 32), float(tileMapDispl.y + posPlayer.y - 7)));
        }
        else if (dir == RIGHT) {
            sprite->changeAnimation(ATTACK_RIGHT);
            sprite_lanza->changeAnimation(THROW);
            sprite_lanza->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x + 32), float(tileMapDispl.y + posPlayer.y - 7)));
        }
    }
    else
    {
        if (dir == LEFT)
            sprite->changeAnimation(STAND_LEFT);
        else if (dir == RIGHT)
            sprite->changeAnimation(STAND_RIGHT);
    }

    // Manejar el salto y la caída
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

            // En la fase descendente, comprobar colisión abajo
            if (jumpAngle > 90) {
                // Primero comprobar colisiones con paredes sólidas (mapWalls)
                if (mapWalls->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y)) {
                    bJumping = false;
                }
                // Luego comprobar colisiones con plataformas solo si está cayendo
                else if (movingDown && mapPlatforms->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y)) {
                    bJumping = false;
                }
            }
        }

        if (dir == LEFT)
            sprite->changeAnimation(JUMP_LEFT);
        else
            sprite->changeAnimation(JUMP_RIGHT);
    }
    else
    {
        spear_visible = false;
        int prevY = posPlayer.y; // Guardamos la posición Y antes de caer
        posPlayer.y += FALL_STEP;

        // Verificar que el jugador está cayendo
        bool isFalling = posPlayer.y > prevY;

        // Primero comprobar colisiones con paredes sólidas (mapWalls)
        bool collisionWithWalls = mapWalls->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y);

        // Luego comprobar colisiones con plataformas solo si está cayendo
        bool collisionWithPlatforms = false;
        if (isFalling) {
            // Solo verificar colisión con plataformas si está cayendo y si el punto de partida está por encima de la plataforma
            collisionWithPlatforms = mapPlatforms->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y);
        }

        // Comprobar colisiones con el suelo
        if (collisionWithWalls || collisionWithPlatforms)
        {
            if (Game::instance().getKey(GLFW_KEY_Z))
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
	if(spear_visible) sprite_lanza->render();
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

