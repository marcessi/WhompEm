#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "FallingStick.h"
#include "Game.h"

#define FALL_SPEED 2.f
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

        // Eliminamos la comprobación de colisiones para que siempre siga cayendo
        // No hay más código aquí, solo cae sin detenerse
        break;

    case STOPPED:
    case DISAPPEARING:
        // Estos estados ya no los usamos, pero los mantenemos por compatibilidad
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
}

void FallingStick::stopFalling()
{
    currentState = STOPPED;
    disappearTimer = 0.0f;
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
