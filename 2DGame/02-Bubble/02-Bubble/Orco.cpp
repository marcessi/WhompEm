#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Orco.h"
#include "Game.h"
#include "MovingPlatform.h"

#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 50
#define FALL_STEP 3
#define DAMAGE_FLASH_TIME 500 // 500ms para el parpadeo de daño

enum OrcoAnims
{
    STAND_LEFT, STAND_RIGHT, JUMP_LEFT, JUMP_RIGHT, FLY_LEFT, FLY_RIGHT
};

Orco::Orco()
{
    mapWalls = NULL;
    mapPlatforms = NULL;
    movingPlatforms = NULL;
    bJumping = false;
    alive = true;
    lives = 2;  // El orco tiene 2 vidas
    playerPos = NULL;
    waitingTime = 0;
    inFlightMode = false;
    bFalling = true; // El orco comienza cayendo hasta encontrar suelo
    hitTime = 0; // Tiempo transcurrido desde último golpe
    isHit = false; // Indica si está en modo "golpeado"
    frozen = false;
    frozenTimer = 0;
}

Orco::~Orco()
{
    if (sprite != NULL)
        delete sprite;
}

void Orco::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, Direction initialDir)
{
    dir = initialDir;
    bJumping = false;  // Comienza en el suelo esperando
    bFalling = true;   // Pero necesita caer hasta encontrar suelo
    jumpAngle = 0;
    frozen = false;
    frozenTimer = 0;

    // Cargar spritesheet
    spritesheet.loadFromFile("images/enemie_verde.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.166f, 0.5f), &spritesheet, &shaderProgram);
    sprite->setNumberAnimations(6);

    // Configurar animaciones
    sprite->setAnimationSpeed(STAND_LEFT, 8);
    sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.5f));

    sprite->setAnimationSpeed(STAND_RIGHT, 8);
    sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.833f, 0.5f));

    sprite->setAnimationSpeed(JUMP_LEFT, 8);
    sprite->addKeyframe(JUMP_LEFT, glm::vec2(0.333f, 0.5f));

    sprite->setAnimationSpeed(JUMP_RIGHT, 8);
    sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.666f, 0.5f));

    sprite->setAnimationSpeed(FLY_LEFT, 20);
    sprite->addKeyframe(FLY_LEFT, glm::vec2(0.333f, 0.5f));
    sprite->addKeyframe(FLY_LEFT, glm::vec2(0.0f, 0.f));
    sprite->addKeyframe(FLY_LEFT, glm::vec2(0.166f, 0.f));

    sprite->setAnimationSpeed(FLY_RIGHT, 20);
    sprite->addKeyframe(FLY_RIGHT, glm::vec2(0.5f, 0.5f));
    sprite->addKeyframe(FLY_RIGHT, glm::vec2(0.833f, 0.f));
    sprite->addKeyframe(FLY_RIGHT, glm::vec2(0.666f, 0.f));

    // Inicializar con la animación adecuada
    if (dir == LEFT)
        sprite->changeAnimation(FLY_LEFT); // Empieza cayendo
    else
        sprite->changeAnimation(FLY_RIGHT); // Empieza cayendo

    tileMapDispl = tileMapPos;
    startY = posOrco.y;

    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posOrco.x), float(tileMapDispl.y + posOrco.y)));
}

void Orco::update(int deltaTime)
{
    if (!alive)
        return;

    if (frozen) {
        frozenTimer += deltaTime;

        // Efecto de parpadeo: alternar visibilidad cada 100ms
        bool visible = ((frozenTimer / 100) % 2 == 0);
        float alpha = visible ? 0.7f : 0.3f;  // Alternar entre 70% y 30% de opacidad
        sprite->setAlpha(alpha);

        // Descongelar después de 5 segundos (5000 ms)
        if (frozenTimer >= 5000) {
            frozen = false;
            frozenTimer = 0;
            sprite->setAlpha(1.0f);  // Restaurar opacidad normal
        }

        // El sprite se actualiza pero no se mueve cuando está congelado
        //sprite->update(deltaTime);
        return;
    }

    sprite->update(deltaTime);

    // Manejar el estado de golpeado
    if (isHit) {
        hitTime += deltaTime;

        // Efecto visual de daño: hacer que el alpha oscile
        float alpha = 0.5f + 0.4f * sin(hitTime * 0.02f);
        sprite->setAlpha(alpha);

        // Después de DAMAGE_FLASH_TIME, volver al estado normal
        if (hitTime >= DAMAGE_FLASH_TIME) {
            isHit = false;
            sprite->setAlpha(1.0f);
        }
    }

    // Guardar la posición previa para restaurarla en caso de colisión
    glm::ivec2 prevPos = posOrco;

    int hitboxWidth = 20;  // Ajustar según el tamaño del sprite del orco
    int hitboxHeight = 30;  // Ajustar según el tamaño del sprite del orco
    int hitboxOffsetX = (32 - hitboxWidth) / 2;

    // Comprobar colisión con el jugador
    if (playerPos != NULL) {
        // Hitbox del jugador (típicamente más pequeña que su sprite)
        int playerHitboxWidth = 16;
        int playerHitboxHeight = 32;
        int playerHitboxOffsetX = (32 - playerHitboxWidth) / 2;

        // Posición real de la hitbox del jugador
        glm::ivec2 playerHitboxPos = glm::ivec2(
            playerPos->x + playerHitboxOffsetX,
            playerPos->y
        );

        // Comprobar colisión AABB (Axis-Aligned Bounding Box)
        bool collisionX = posOrco.x + hitboxOffsetX < playerHitboxPos.x + playerHitboxWidth &&
            posOrco.x + hitboxOffsetX + hitboxWidth > playerHitboxPos.x;
        bool collisionY = posOrco.y < playerHitboxPos.y + playerHitboxHeight &&
            posOrco.y + hitboxHeight > playerHitboxPos.y;

        // Si hay colisión, notificar al jugador
        if (collisionX && collisionY && playerHitted != NULL) {
            playerHitted();; // Llamar a la función de callback para dañar al jugador
        }
    }

    // Si está cayendo para encontrar suelo al inicio
    if (bFalling && !bJumping)
    {
        // Usar animación de vuelo durante la caída
        if (dir == LEFT)
            sprite->changeAnimation(FLY_LEFT);
        else
            sprite->changeAnimation(FLY_RIGHT);

        // Aplicar gravedad
        posOrco.y += FALL_STEP;

        // Comprobar colisiones con el suelo para detener la caída
        bool foundGround = false;

        // Comprobar colisiones con paredes sólidas (mapWalls)
        if (mapWalls->collisionMoveDown(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
            glm::ivec2(hitboxWidth, hitboxHeight), &posOrco.y)) {
            foundGround = true;
        }
        // Comprobar colisiones con plataformas
        else if (mapPlatforms->collisionMoveDown(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
            glm::ivec2(hitboxWidth, hitboxHeight), &posOrco.y)) {
            foundGround = true;
        }
        // Comprobar colisiones con plataformas móviles
        else if (movingPlatforms != nullptr) {
            for (auto platform : *movingPlatforms) {
                float tempPosY = posOrco.y;
                if (platform->collisionMoveDown(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
                    glm::ivec2(hitboxWidth, hitboxHeight), &tempPosY)) {
                    posOrco.y = tempPosY;
                    foundGround = true;
                    break;
                }
            }
        }

        // Si encuentra suelo, dejar de caer y comenzar ciclo normal
        if (foundGround) {
            bFalling = false;
            waitingTime = 0;

            // Cambiar a animación de pie
            if (dir == LEFT)
                sprite->changeAnimation(STAND_LEFT);
            else
                sprite->changeAnimation(STAND_RIGHT);

            std::cout << "Orco ha encontrado suelo en Y=" << posOrco.y << std::endl;
        }

        // Limitar la caída (por si no hay suelo debajo)
        if (posOrco.y > 2000) {
            alive = false; // Si cae demasiado, eliminarlo
            return;
        }
    }
    // Si no está cayendo ni saltando, proceder con la lógica normal
    else if (!bJumping && !inFlightMode) {
        // MEJORA: Verificar siempre si hay suelo bajo el orco
        // Guardar posición previa
        int prevY = posOrco.y;

        // Aplicar una pequeña gravedad para detectar si hay suelo
        posOrco.y += FALL_STEP;

        // Verificar que el orco está cayendo
        bool isFalling = true;

        // Comprobar colisiones con el suelo
        bool collisionWithWalls = mapWalls->collisionMoveDown(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
            glm::ivec2(hitboxWidth, hitboxHeight), &posOrco.y);

        // Comprobar colisiones con plataformas
        bool collisionWithPlatforms = mapPlatforms->collisionMoveDown(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
            glm::ivec2(hitboxWidth, hitboxHeight), &posOrco.y);

        // Comprobar colisiones con plataformas móviles
        bool collisionWithMovingPlatforms = false;
        if (movingPlatforms != nullptr) {
            for (auto platform : *movingPlatforms) {
                float tempPosY = posOrco.y;
                if (platform->collisionMoveDown(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
                    glm::ivec2(hitboxWidth, hitboxHeight), &tempPosY)) {
                    posOrco.y = tempPosY;
                    collisionWithMovingPlatforms = true;
                    break;
                }
            }
        }

        // Si no hay suelo bajo el orco, entra en modo caída
        if (!collisionWithWalls && !collisionWithPlatforms && !collisionWithMovingPlatforms) {
            bFalling = true;

            // Cambiar a animación de vuelo/caída
            if (dir == LEFT)
                sprite->changeAnimation(FLY_LEFT);
            else
                sprite->changeAnimation(FLY_RIGHT);

            std::cout << "Orco ha perdido el suelo en X=" << posOrco.x << ", Y=" << posOrco.y << std::endl;
        }
        else {
            // Si hay suelo, continuar con la espera para saltar
            waitingTime += deltaTime;

            // Actualizar dirección hacia el jugador si hay referencia al jugador
            if (playerPos != NULL) {
                dir = (posOrco.x > playerPos->x) ? LEFT : RIGHT;
            }

            // Cambiar animación según la dirección
            if (dir == LEFT) {
                sprite->changeAnimation(STAND_LEFT);
            }
            else {
                sprite->changeAnimation(STAND_RIGHT);
            }

            // Después de 0.5 segundos, prepararse para saltar
            if (waitingTime >= 500) {
                inFlightMode = true;  // Entrar en modo vuelo/salto

                // Cambiar a animación de preparación para saltar
                if (dir == LEFT) {
                    sprite->changeAnimation(JUMP_LEFT);
                }
                else {
                    sprite->changeAnimation(JUMP_RIGHT);
                }
            }
        }
    }
    // Si estamos en modo de preparación para vuelo/salto
    else if (!bJumping && inFlightMode) {
        // Después de 0.2 segundos adicionales, iniciar el salto
        waitingTime += deltaTime;
        if (waitingTime >= 700) { // 500 + 200 = 700ms total
            bJumping = true;
            jumpAngle = 0;
            startY = posOrco.y;
            waitingTime = 0;
            inFlightMode = false;

            // Cambiar a animación de vuelo
            if (dir == LEFT) {
                sprite->changeAnimation(FLY_LEFT);
            }
            else {
                sprite->changeAnimation(FLY_RIGHT);
            }
        }
    }
    // Si estamos saltando
    else if (bJumping)
    {
        jumpAngle += JUMP_ANGLE_STEP;

        if (jumpAngle >= 180)
        {
            bJumping = false;
            posOrco.y = startY;
            waitingTime = 0;  // Reiniciar tiempo de espera

            // MEJORA: En vez de asumir que hay suelo, verificar
            // Iniciar un ciclo de verificación para ver si hay suelo
            bFalling = true;

            // Cambiar a animación de vuelo para el período de verificación
            if (dir == LEFT)
                sprite->changeAnimation(FLY_LEFT);
            else
                sprite->changeAnimation(FLY_RIGHT);
        }
        else
        {
            // Guardar posición Y antes de saltar
            int prevY = posOrco.y;

            // Calcular nueva posición Y
            posOrco.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));

            // Verificar si el orco está subiendo o bajando
            bool movingDown = posOrco.y > prevY;

            // Asegurarnos de mantener la animación de vuelo durante todo el salto
            if (dir == LEFT) {
                if (sprite->animation() != FLY_LEFT) sprite->changeAnimation(FLY_LEFT);
            }
            else {
                if (sprite->animation() != FLY_RIGHT) sprite->changeAnimation(FLY_RIGHT);
            }

            // Mover horizontalmente según la dirección
            if (dir == LEFT)
                posOrco.x -= 2;  // Velocidad ligeramente aumentada
            else
                posOrco.x += 2;  // Velocidad ligeramente aumentada

            // Comprobar colisiones con paredes
            if ((dir == LEFT && mapWalls->collisionMoveLeft(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
                glm::ivec2(hitboxWidth, hitboxHeight))) ||
                (dir == RIGHT && mapWalls->collisionMoveRight(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
                    glm::ivec2(hitboxWidth, hitboxHeight)))) {
                posOrco.x = prevPos.x;
                dir = (dir == LEFT) ? RIGHT : LEFT;

                // Actualizar animación
                if (dir == LEFT) {
                    if (sprite->animation() != FLY_LEFT) sprite->changeAnimation(FLY_LEFT);
                }
                else {
                    if (sprite->animation() != FLY_RIGHT) sprite->changeAnimation(FLY_RIGHT);
                }
            }

            if (jumpAngle > 90 && movingDown) {
                // Primero comprobar colisiones con paredes sólidas (mapWalls)
                if (mapWalls->collisionMoveDown(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
                    glm::ivec2(hitboxWidth, hitboxHeight), &posOrco.y)) {
                    bJumping = false;
                    bFalling = false; // Ya encontró suelo, no necesita caer
                    waitingTime = 0;  // Reiniciar tiempo de espera

                    // Cambiar a animación de pie
                    if (dir == LEFT)
                        sprite->changeAnimation(STAND_LEFT);
                    else
                        sprite->changeAnimation(STAND_RIGHT);
                }
                // Luego comprobar colisiones con plataformas solo si está cayendo
                else if (movingDown && mapPlatforms->collisionMoveDown(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
                    glm::ivec2(hitboxWidth, hitboxHeight), &posOrco.y)) {
                    bJumping = false;
                    bFalling = false; // Ya encontró suelo, no necesita caer
                    waitingTime = 0;  // Reiniciar tiempo de espera

                    // Cambiar a animación de pie
                    if (dir == LEFT)
                        sprite->changeAnimation(STAND_LEFT);
                    else
                        sprite->changeAnimation(STAND_RIGHT);
                }
                // Finalmente, comprobar colisiones con plataformas móviles
                else if (movingDown && movingPlatforms != nullptr) {
                    for (auto platform : *movingPlatforms) {
                        float tempPosY = posOrco.y;
                        if (platform->collisionMoveDown(glm::ivec2(posOrco.x + hitboxOffsetX, posOrco.y),
                            glm::ivec2(hitboxWidth, hitboxHeight), &tempPosY)) {
                            posOrco.y = tempPosY;
                            bJumping = false;
                            bFalling = false; // Ya encontró suelo, no necesita caer
                            waitingTime = 0;  // Reiniciar tiempo de espera

                            // Cambiar a animación de pie
                            if (dir == LEFT)
                                sprite->changeAnimation(STAND_LEFT);
                            else
                                sprite->changeAnimation(STAND_RIGHT);

                            break;
                        }
                    }
                }
            }
        }
    }

    // Actualizar la posición del sprite
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posOrco.x), float(tileMapDispl.y + posOrco.y)));
}

void Orco::render()
{
    if (alive)
        sprite->render();
}

void Orco::freeze() {
    frozen = true;
    frozenTimer = 0;
}

void Orco::setTileMap(TileMap* tileMapWalls, TileMap* tileMapPlatforms)
{
    mapWalls = tileMapWalls;
    mapPlatforms = tileMapPlatforms;
}

void Orco::setPosition(const glm::vec2& pos)
{
    posOrco = pos;
    startY = pos.y;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + posOrco.x), float(tileMapDispl.y + posOrco.y)));
}

void Orco::setMovingPlatforms(const std::vector<MovingPlatform*>* platforms)
{
    movingPlatforms = platforms;
}

void Orco::setPlayerPosition(const glm::ivec2* playerPosition)
{
    playerPos = playerPosition;
}

void Orco::setPlayerHitCallback(std::function<void()> callback)
{
    playerHitted = callback;
}

glm::ivec2 Orco::getPosition() const
{
    return posOrco;
}

glm::ivec2 Orco::getSize() const
{
    return glm::ivec2(32, 32); // Ajustar según el tamaño real del sprite
}

bool Orco::isAlive() const
{
    return alive;
}

void Orco::hit()
{
    if (!alive || isHit)
        return;

    lives--;

    // Iniciar el estado de golpeado
    isHit = true;
    hitTime = 0;

    // Establecer alpha inicial para el efecto visual
    sprite->setAlpha(0.5f);

    if (lives <= 0) {
        alive = false;
        std::cout << "Orco muerto" << std::endl;
    }
    else {
        std::cout << "Orco golpeado, vidas restantes: " << lives << std::endl;
    }
}

void Orco::kill()
{
    alive = false;
}
