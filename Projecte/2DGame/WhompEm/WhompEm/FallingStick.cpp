#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "FallingStick.h"
#include "Game.h"

#define FALL_SPEED 3.f
#define DISAPPEAR_TIME 1000.0f // 1 segundo para desaparecer

// Definir las animaciones para el stick
enum StickAnims
{
    STICK_IDLE, STICK_FALLING
};

FallingStick::FallingStick()
{
    mapWalls = nullptr;
    mapPlatforms = nullptr;
    sprite = nullptr;
    currentState = WAITING;
    fallSpeed = FALL_SPEED;
    waitTime = 0.0f;
    disappearTimer = 0.0f;
    disappearDuration = DISAPPEAR_TIME;
    alpha = 1.0f;
    size = glm::ivec2(16, 16); // Tamaño predeterminado para colisiones
}

FallingStick::~FallingStick()
{
    if (sprite != nullptr)
        delete sprite;
}

void FallingStick::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
    tileMapDispl = tileMapPos;

    // Cargar textura del palo
    spritesheet.loadFromFile("images/falling_stick.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(1.f, 1.0f), &spritesheet, &shaderProgram);

    // Configurar animaciones
    sprite->setNumberAnimations(2); // Cambiado a 2 para incluir ambas animaciones

   

    // Animación de caída - Agregamos esta animación que faltaba
    sprite->setAnimationSpeed(STICK_FALLING, 8);
    sprite->addKeyframe(STICK_FALLING, glm::vec2(0.0f, 0.0f)); // Usa el mismo frame si no hay diferentes

    // Iniciar con animación de espera
    sprite->changeAnimation(STICK_FALLING);

    // Establecer posición inicial fuera de la pantalla
    position = glm::ivec2(0, -100);
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));

    // Inicializar estado
    currentState = WAITING;
    waitTime = 0.0f;
    fallSpeed = FALL_SPEED;
    alpha = 1.0f;
}

void FallingStick::update(int deltaTime)
{
    sprite->update(deltaTime);

    switch (currentState)
    {
    case WAITING:
        // Esperar un tiempo aleatorio antes de caer
        waitTime -= deltaTime;
        if (waitTime <= 0) {
            currentState = FALLING;
            sprite->changeAnimation(STICK_FALLING);
        }
        break;

    case FALLING:
        // Actualizar posición vertical (caer)
        position.y += fallSpeed;
        break;

    case KILLED:
        // Mini salto lateral seguido de caída
        killJumpAngle += 6.0f; // Velocidad del salto

        if (killJumpAngle <= 90.0f) {
            // Primera mitad del arco: subir y moverse lateralmente
            position.x += killDir * 1.5f;
            position.y = int(killStartY - 30.0f * sin(3.14159f * killJumpAngle / 180.f));
        }
        else {
            // Segunda mitad: solo caída más rápida que normal
            position.y += fallSpeed * 1.5f;

            // Continuar con un poco de movimiento lateral pero más lento
            position.x += killDir * 0.5f;
        }

        // Reducir ligeramente el alpha para dar efecto visual
        if (alpha > 0.7f) {
            alpha = 0.7f;
        }
        sprite->setAlpha(alpha);

        // Cuando ya no sea visible (esté fuera de pantalla por abajo), mantenerlo en estado KILLED
        // La decisión de eliminarlo completamente se tomará en Scene::updateFallingSticks
        break;

    case STOPPED:
    case DISAPPEARING:
        // Si por alguna razón llega a estos estados, lo cambiamos a FALLING
        currentState = FALLING;
        sprite->changeAnimation(STICK_FALLING);
        break;
    }

    // Actualizar la posición del sprite
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}



void FallingStick::render()
{
    sprite->setAlpha(1.0f);
    sprite->render();

}

void FallingStick::setTileMap(TileMap* tileMapWalls, TileMap* tileMapPlatforms)
{
    mapWalls = tileMapWalls;
    mapPlatforms = tileMapPlatforms;
}

void FallingStick::setPosition(const glm::vec2& pos)
{
    position = pos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void FallingStick::startFalling()
{
    currentState = FALLING;
    sprite->changeAnimation(STICK_FALLING);
    fallSpeed = FALL_SPEED;
    waitTime = 0.0f;
}

void FallingStick::stopFalling()
{
    currentState = STOPPED;
    disappearTimer = 0.0f;
    waitTime = 0.0f;
}

bool FallingStick::isDisappearing() const
{
    return currentState == DISAPPEARING && disappearTimer >= disappearDuration;
}

bool FallingStick::isActive() const
{
    return currentState == FALLING || currentState == STOPPED;
}

glm::ivec2 FallingStick::getPosition() const
{
    return position;
}

glm::ivec2 FallingStick::getSize() const
{
    return size;
}

bool FallingStick::collisionWithPlayer(const glm::ivec2& playerPos, const glm::ivec2& playerSize) const
{
    // Solo comprobar colisiones si está cayendo o detenido
    if (currentState != FALLING && currentState != STOPPED)
        return false;

    // Comprobar colisión de tipo AABB (Axis-Aligned Bounding Box)
    int hitboxOffsetX = 2; // Offset para centrar la hitbox
    int hitboxWidth = size.x - 4; // Hitbox más pequeña que el sprite visual

    // Crear cajas de colisión
    glm::ivec2 stickMin(position.x + hitboxOffsetX, position.y);
    glm::ivec2 stickMax(position.x + hitboxOffsetX + hitboxWidth, position.y + size.y);

    glm::ivec2 playerMin(playerPos.x, playerPos.y);
    glm::ivec2 playerMax(playerPos.x + playerSize.x, playerPos.y + playerSize.y);

    // Comprobar superposición en ambos ejes
    bool collisionX = stickMin.x < playerMax.x && stickMax.x > playerMin.x;
    bool collisionY = stickMin.y < playerMax.y && stickMax.y > playerMin.y;

    return collisionX && collisionY;
}

void FallingStick::setWaiting(float time)
{
    currentState = WAITING;
    waitTime = time;
    alpha = 1.0f;
    sprite->setAlpha(alpha);
    sprite->changeAnimation(STICK_FALLING);
}
void FallingStick::killStick(float directionX)
{
    if (currentState != KILLED) {
        currentState = KILLED;
        killJumpAngle = 0.0f;
        killStartY = position.y;
        killDir = directionX; // Dirección basada en la posición relativa al jugador
        sprite->setAlpha(1.0f); // Asegurar que sea visible durante el mini-salto
    }
}

bool FallingStick::isKilled() const
{
    return currentState == KILLED;
}

