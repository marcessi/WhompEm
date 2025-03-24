#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Snake.h"
#include "Game.h"

#define CAMERA_WIDTH 256 // Definimos constante para la anchura de la c�mara

enum SnakeAnims
{
    MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT
};

void Snake::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, Direction initialDir)
{
	int minX = -1; // Por defecto, no hay l�mites de movimiento
    dir = initialDir;
    bJumping = true;
    jumpAngle = 0;

    // L�mites de movimiento por defecto
    minX = 0;
    maxX = CAMERA_WIDTH; // Por defecto, el ancho de la c�mara

    spritesheet.loadFromFile("images/serpienteveneno.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(glm::ivec2(32, 16), glm::vec2(0.125f, 1.f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(4);

    // Configuraci�n de animaciones
    sprite->setAnimationSpeed(MOVE_LEFT, 8);
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.0f, 0.0f));
    sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.125f, 0.0f));

    sprite->setAnimationSpeed(MOVE_RIGHT, 8);
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.750f, 0.f));
    sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.875f, 0.f));

    sprite->setAnimationSpeed(JUMP_LEFT, 8);
    sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.250f, 0.0f));
    sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.375f, 0.0f));

    sprite->setAnimationSpeed(JUMP_RIGHT, 8);
    sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.500f, 0.f));
    sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.625f, 0.f));

    // Iniciar con la animaci�n adecuada seg�n direcci�n
    if (dir == LEFT)
        sprite->changeAnimation(JUMP_LEFT);
    else
        sprite->changeAnimation(JUMP_RIGHT);

    tileMapDispl = tileMapPos;
    posSnake.y = 0;
    posSnake.x = 0;
    startY = posSnake.y;

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posSnake.x), float(tileMapDispl.y + posSnake.y)));
}

void Snake::update(int deltaTime)
{
    sprite->update(deltaTime);

    // Guardar la posici�n previa para restaurarla en caso de colisi�n
    glm::ivec2 prevPos = posSnake;

    int hitboxWidth = 28;
    int hitboxHeight = 14; // Ajustado para mejor coincidir con el sprite
    int hitboxOffsetX = (32 - hitboxWidth) / 2;

    // Gestionar movimiento horizontal seg�n direcci�n con velocidad m�s alta
    if (dir == LEFT)
        posSnake.x -= 2; // Aumentamos velocidad
    else
        posSnake.x += 2; // Aumentamos velocidad

    // Comprobar colisiones con paredes o l�mites de pantalla
    if ((dir == LEFT && posSnake.x < minX) ||
        (dir == LEFT && mapWalls->collisionMoveLeft(glm::ivec2(posSnake.x + hitboxOffsetX, posSnake.y), glm::ivec2(hitboxWidth, hitboxHeight)))) {
        posSnake.x = prevPos.x;
        dir = RIGHT;
        if (!bJumping)
            sprite->changeAnimation(MOVE_RIGHT);
        else
            sprite->changeAnimation(JUMP_RIGHT);
    }
    else if ((dir == RIGHT && posSnake.x + hitboxWidth > maxX) ||
        (dir == RIGHT && mapWalls->collisionMoveRight(glm::ivec2(posSnake.x + hitboxOffsetX, posSnake.y), glm::ivec2(hitboxWidth, hitboxHeight)))) {
        posSnake.x = prevPos.x;
        dir = LEFT;
        if (!bJumping)
            sprite->changeAnimation(MOVE_LEFT);
        else
            sprite->changeAnimation(JUMP_LEFT);
    }

    // Gestionar salto inicial con salto m�s peque�o
    if (bJumping) {
        jumpAngle += 4; // Aumentamos velocidad del salto
        if (jumpAngle >= 180) {
            bJumping = false; // Ya no volver� a saltar
            posSnake.y = startY - 10;

            // Cambiar a la animaci�n de movimiento normal
            if (dir == LEFT)
                sprite->changeAnimation(MOVE_LEFT);
            else
                sprite->changeAnimation(MOVE_RIGHT);
        }
        else {
            posSnake.y = int(startY - 20 * sin(3.14159f * jumpAngle / 180.f)); 
        }
    }
    else {
        // Gestionar ca�da
        posSnake.y += 2;

        // Comprobar colisi�n con el suelo
        if (mapWalls->collisionMoveDown(glm::ivec2(posSnake.x + hitboxOffsetX, posSnake.y),
            glm::ivec2(hitboxWidth, hitboxHeight), &posSnake.y)) {
            // La serpiente est� en el suelo
        }
        else if (mapPlatforms != nullptr &&
            mapPlatforms->collisionMoveDown(glm::ivec2(posSnake.x + hitboxOffsetX, posSnake.y),
                glm::ivec2(hitboxWidth, hitboxHeight), &posSnake.y)) {
            // La serpiente est� en una plataforma
        }
    }

    // Actualizar la posici�n del sprite
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posSnake.x), float(tileMapDispl.y + posSnake.y -3)));
}

// Nuevo m�todo para establecer los l�mites de movimiento
void Snake::setMovementLimits(int minXLimit, int maxXLimit) {
    minX = minXLimit;
    maxX = maxXLimit;
}

void Snake::render()
{
    sprite->render();
}

void Snake::setTileMap(TileMap* tileMapWalls, TileMap* tileMapPlatforms)
{
    mapWalls = tileMapWalls;
    mapPlatforms = tileMapPlatforms;
}

void Snake::setPosition(const glm::vec2& pos)
{
    posSnake = pos;
    startY = pos.y;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posSnake.x), float(tileMapDispl.y + posSnake.y)));
}

glm::ivec2 Snake::getPosition()
{
    return posSnake;
}

bool Snake::collisionWithPlayer(const glm::ivec2& playerPos, const glm::ivec2& playerSize) const
{
    // Implementar detecci�n de colisi�n con jugador
    int snakeWidth = 28;
    int snakeHeight = 28;
    int snakeOffsetX = (32 - snakeWidth) / 2;

    glm::ivec2 snakeBoxPos = glm::ivec2(posSnake.x + snakeOffsetX, posSnake.y);

    // Comprobar si los rect�ngulos se solapan
    bool collisionX = playerPos.x + playerSize.x >= snakeBoxPos.x && snakeBoxPos.x + snakeWidth >= playerPos.x;
    bool collisionY = playerPos.y + playerSize.y >= snakeBoxPos.y && snakeBoxPos.y + snakeHeight >= playerPos.y;

    return collisionX && collisionY;
}
