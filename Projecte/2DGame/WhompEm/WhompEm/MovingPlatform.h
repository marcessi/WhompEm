#ifndef _MOVING_PLATFORM_INCLUDE
#define _MOVING_PLATFORM_INCLUDE

#include "TileMap.h"
#include "Texture.h"
#include "Sprite.h"

class MovingPlatform
{
public:
    // Constructor y destructor
    MovingPlatform();
    ~MovingPlatform();
    
    // Inicialización
    void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, const string &texturePath, const glm::ivec2 &size);
    
    // Actualización y renderizado
    void update(int deltaTime);
    void render() const;
    
    // Colisiones
    bool collisionMoveDown(const glm::ivec2 &pos, const glm::ivec2 &size, float *posY) const;
    
    // Configuración
    void setPosition(const glm::vec2 &pos);
    void setMovementLimits(float minY, float maxY);
    void setSpeed(float speed);
    
    // Información
    glm::ivec2 getPosition() const;
    glm::ivec2 getSize() const;
    
private:
    // Textura y sprite
    Texture spritesheet;
    Sprite *sprite;
    
    // Posición y desplazamiento
    glm::ivec2 tileMapDispl;
    glm::vec2 position;
    glm::ivec2 size;
    
    // Movimiento
    float speed;
    float minY, maxY;
    bool movingUp;
};

#endif // _MOVING_PLATFORM_INCLUDE
