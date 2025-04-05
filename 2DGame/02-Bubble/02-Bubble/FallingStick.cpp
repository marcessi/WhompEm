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
    size = glm::ivec2(16, 16); // Tama�o predeterminado para colisiones
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

   

    // Animaci�n de ca�da - Agregamos esta animaci�n que faltaba
    sprite->setAnimationSpeed(STICK_FALLING, 8);
    sprite->addKeyframe(STICK_FALLING, glm::vec2(0.0f, 0.0f)); // Usa el mismo frame si no hay diferentes

    // Iniciar con animaci�n de espera
    sprite->changeAnimation(STICK_FALLING);

    // Establecer posici�n inicial fuera de la pantalla
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
        // Actualizar posici�n vertical (caer)
        position.y += fallSpeed;

        // Eliminamos la comprobaci�n de colisiones para que siempre siga cayendo
        // No hay m�s c�digo aqu�, solo cae sin detenerse
        break;

    case STOPPED:
    case DISAPPEARING:
        // Estos estados ya no los usamos, pero los mantenemos por compatibilidad
        // Si por alguna raz�n llega a estos estados, lo cambiamos a FALLING
        currentState = FALLING;
        sprite->changeAnimation(STICK_FALLING);
        break;
    }

    // Actualizar la posici�n del sprite
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
    // Solo comprobar colisiones si est� cayendo o detenido
    if (currentState != FALLING && currentState != STOPPED)
        return false;

    // Comprobar colisi�n de tipo AABB (Axis-Aligned Bounding Box)
    int hitboxOffsetX = 2; // Offset para centrar la hitbox
    int hitboxWidth = size.x - 4; // Hitbox m�s peque�a que el sprite visual

    // Crear cajas de colisi�n
    glm::ivec2 stickMin(position.x + hitboxOffsetX, position.y);
    glm::ivec2 stickMax(position.x + hitboxOffsetX + hitboxWidth, position.y + size.y);

    glm::ivec2 playerMin(playerPos.x, playerPos.y);
    glm::ivec2 playerMax(playerPos.x + playerSize.x, playerPos.y + playerSize.y);

    // Comprobar superposici�n en ambos ejes
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
