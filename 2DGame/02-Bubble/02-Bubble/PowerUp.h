#ifndef _POWERUP_INCLUDE
#define _POWERUP_INCLUDE

#include "Sprite.h"
#include "TileMap.h"
#include <glm/glm.hpp>

enum PowerUpType {
    SMALL_HEART,
    LARGE_HEART,
    MAGIC_POTION,
    FLINT_SPEAR,
    BUFFALO_HELMET,
    DEERSKIN_SHIRT,
    GOURD,
    TOTEM_BOSS
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

    
	PowerUpType getType() const { return type; }
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
