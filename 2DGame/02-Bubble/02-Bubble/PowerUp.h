#ifndef _POWERUP_INCLUDE
#define _POWERUP_INCLUDE

#include "Sprite.h"
#include "TileMap.h"
#include <glm/glm.hpp>

enum PowerUpType {
    SMALL_HEART,
    LARGE_HEART,
    GOURD,
    MAGIC_POTION,
    FLINT_SPEAR,   // Flint Spear Head - Aumenta poder de ataque por 4 golpes
    BUFFALO_HELMET, // Buffalo Headdress - Aumenta defensa por 4 golpes
    DEERSKIN_SHIRT
};

class PowerUp
{
public:
    PowerUp(PowerUpType type);
    ~PowerUp();
    
    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
    void update(int deltaTime);
    void render();
    
    void setPosition(const glm::vec2& pos);
    void setTileMap(TileMap* tileMapWalls, TileMap* tileMapPlatforms);
    void setFloatingType(bool floating, bool upwards = false);

    
    PowerUpType getType() const;
    glm::ivec2 getPosition() const { return position; }
    glm::ivec2 getSize() const { return size; }
    bool isActive() const;
    bool collisionWithPlayer(const glm::ivec2& playerPos, const glm::ivec2& playerSize) const;
    void deactivate();
    
private:
    PowerUpType type;
    glm::ivec2 tileMapDispl;
    glm::ivec2 position;
    glm::ivec2 size;
    
    bool active;
    bool bFloating;
    float floatAngle;
    float baseY;
    bool bFloatingUpwards; // Nuevo campo para controlar si flota hacia arriba
    float xOffset;
    Texture spritesheet;
    Sprite* sprite;
    
    TileMap* mapWalls;
    TileMap* mapPlatforms;
};

#endif // _POWERUP_INCLUDE
