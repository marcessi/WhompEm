#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Snake.h"
#include "Game.h"

#define CAMERA_WIDTH 256 // Definimos constante para la anchura de la cámara

enum SnakeAnims
{
    MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT
};

void Snake::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, Direction initialDir)
{
	int minX = -1; // Por defecto, no hay límites de movimiento
    dir = initialDir;
    bJumping = true;
    jumpAngle = 0;
    frozen = false;
    frozenTimer = 0;

    // Límites de movimiento por defecto
    minX = 0;
    maxX = CAMERA_WIDTH; // Por defecto, el ancho de la cámara

    spritesheet.loadFromFile("images/serpienteveneno.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(glm::ivec2(32, 16), glm::vec2(0.125f, 1.f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(4);

    // Configuración de animaciones
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

    // Iniciar con la animación adecuada según dirección
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

    if (frozen) {
        frozenTimer += deltaTime;

        // Efecto de parpadeo: alternar visibilidad cada 100ms
        bool visible = ((frozenTimer / 100) % 2 == 0);
        float alpha = visible ? 0.7f : 0.3f;  // Alternar entre 70% y 30% de opacidad
        sprite->setAlpha(alpha);

        // Descongelar después de 5 segundos (5000 ms)
        if (frozenTimer >= 3000) {
            frozen = false;
            frozenTimer = 0;
            sprite->setAlpha(1.0f);  // Restaurar opacidad normal
        }

        // El sprite se actualiza pero no se mueve cuando está congelado
        //sprite->update(deltaTime);
        return;
    }

    sprite->update(deltaTime);

    // Guardar la posición previa para restaurarla en caso de colisión
    glm::ivec2 prevPos = posSnake;

    int hitboxWidth = 28;
    int hitboxHeight = 14; // Ajustado para mejor coincidir con el sprite
    int hitboxOffsetX = (32 - hitboxWidth) / 2;

    // Gestionar movimiento horizontal según dirección con velocidad más alta
    if (dir == LEFT)
        posSnake.x -= 2; // Aumentamos velocidad
    else
        posSnake.x += 2; // Aumentamos velocidad

    // Comprobar colisiones con paredes o límites de pantalla
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

    // Gestionar salto inicial con salto más pequeño
    if (bJumping) {
        jumpAngle += 4; // Aumentamos velocidad del salto
        if (jumpAngle >= 180) {
            bJumping = false; // Ya no volverá a saltar
            posSnake.y = startY - 10;

            // Cambiar a la animación de movimiento normal
            if (dir == LEFT)
                sprite->changeAnimation(MOVE_LEFT);
            else
                sprite->changeAnimation(MOVE_RIGHT);
        }
        else {
            posSnake.y = int(startY - 10 * sin(3.14159f * jumpAngle / 180.f)); 
        }
    }
    else {
        // Gestionar caída
        posSnake.y += 2;

        // Comprobar colisión con el suelo
        if (mapWalls->collisionMoveDown(glm::ivec2(posSnake.x + hitboxOffsetX, posSnake.y),
            glm::ivec2(hitboxWidth, hitboxHeight), &posSnake.y)) {
            // La serpiente está en el suelo
        }
        else if (mapPlatforms != nullptr &&
            mapPlatforms->collisionMoveDown(glm::ivec2(posSnake.x + hitboxOffsetX, posSnake.y),
                glm::ivec2(hitboxWidth, hitboxHeight), &posSnake.y)) {
            // La serpiente está en una plataforma
        }
    }

    // Actualizar la posición del sprite
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posSnake.x), float(tileMapDispl.y + posSnake.y -3)));
}

// Nuevo método para establecer los límites de movimiento
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

void Snake::freeze() {
    frozen = true;
    frozenTimer = 0;
}

bool Snake::collisionWithPlayer(const glm::ivec2& playerPos, const glm::ivec2& playerSize) const
{
    // Usar un tamaño de hitbox más preciso para la serpiente
    int snakeWidth = 24; // Reducido desde 32
    int snakeHeight = 12; // Reducido desde 16

    // Ajustar offset para centrar la hitbox en el sprite visual
    int snakeOffsetX = (32 - snakeWidth) / 2;
    int snakeOffsetY = 2; // Pequeño ajuste vertical para alinear mejor con el sprite

    glm::ivec2 snakeBoxPos = glm::ivec2(posSnake.x + snakeOffsetX, posSnake.y + snakeOffsetY);

    // Definir la hitbox ajustada para el jugador (solo parte inferior)
    int playerHitboxWidth = playerSize.x;
    int playerHitboxHeight = playerSize.y / 2; // Solo considerar la mitad inferior del jugador
    glm::ivec2 playerHitboxPos = glm::ivec2(playerPos.x, playerPos.y + playerSize.y - playerHitboxHeight);

    // Comprobar si los rectángulos se solapan
    bool collisionX = playerHitboxPos.x + playerHitboxWidth > snakeBoxPos.x &&
        snakeBoxPos.x + snakeWidth > playerHitboxPos.x;

    bool collisionY = playerHitboxPos.y + playerHitboxHeight > snakeBoxPos.y &&
        snakeBoxPos.y + snakeHeight > playerHitboxPos.y;

    return collisionX && collisionY;
}

glm::ivec2 Snake::getSize() const {
    // Devuelve el tamaño real de la hitbox de la serpiente para uso en colisiones
    return glm::ivec2(24, 12);
}
