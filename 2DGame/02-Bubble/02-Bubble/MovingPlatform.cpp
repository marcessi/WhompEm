#include <GL/glew.h>
#include "MovingPlatform.h"

MovingPlatform::MovingPlatform()
{
    sprite = nullptr;
    speed = 1.0f;
    movingUp = true;
}

MovingPlatform::~MovingPlatform()
{
    if (sprite != nullptr)
        delete sprite;
}

void MovingPlatform::init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, const string &texturePath, const glm::ivec2 &size)
{
    this->size = size;
    spritesheet.loadFromFile(texturePath, TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(size, glm::vec2(1.0f, 1.0f), &spritesheet, &shaderProgram);
    
    tileMapDispl = tileMapPos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void MovingPlatform::update(int deltaTime)
{
    // Calcular el nuevo desplazamiento vertical
    float displacement = speed * (deltaTime / 100.0f);
    
    // Actualizar la posici�n seg�n la direcci�n de movimiento
    if (movingUp)
    {
        position.y -= displacement;
        if (position.y <= minY)
        {
            position.y = minY;
            movingUp = false;
        }
    }
    else
    {
        position.y += displacement;
        if (position.y >= maxY)
        {
            position.y = maxY;
            movingUp = true;
        }
    }
    
    // Actualizar la posici�n del sprite
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void MovingPlatform::render() const
{
    sprite->render();
}

bool MovingPlatform::collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& actorSize, int* posY) const
{
    // Verificar si el actor est� directamente sobre la plataforma
    int actorLeft = pos.x;
    int actorRight = pos.x + actorSize.x - 1;
    int actorBottom = pos.y + actorSize.y - 1;

    int platformLeft = position.x;
    int platformRight = position.x + size.x - 1;
    int platformTop = position.y;

    // Verificar colisi�n en coordenadas X
    if (actorRight >= platformLeft && actorLeft <= platformRight)
    {
        // Verificar si el actor est� "aterrizando" en la plataforma
        // Aumentamos el rango de detecci�n para evitar que el jugador atraviese la plataforma
        if (actorBottom >= platformTop - 8 && actorBottom <= platformTop + 4)
        {
            *posY = platformTop - actorSize.y;
            return true;
        }
    }

    return false;
}


void MovingPlatform::setPosition(const glm::vec2 &pos)
{
    position = pos;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void MovingPlatform::setMovementLimits(float minY, float maxY)
{
    this->minY = minY;
    this->maxY = maxY;
    
    // Asegurarse de que la posici�n inicial est� dentro de los l�mites
    position.y = glm::clamp(position.y, minY, maxY);
}

void MovingPlatform::setSpeed(float speed)
{
    this->speed = speed;
}

glm::ivec2 MovingPlatform::getPosition() const
{
    return glm::ivec2(position);
}

glm::ivec2 MovingPlatform::getSize() const
{
    return size;
}
