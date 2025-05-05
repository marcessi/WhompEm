#ifndef _SNAKE_INCLUDE
#define _SNAKE_INCLUDE

#include "Sprite.h"
#include "TileMap.h"

class Snake
{
public:
    enum Direction { LEFT, RIGHT };
    
    void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram, Direction initialDir = LEFT);
    void update(int deltaTime);
    void render();
    
    void setTileMap(TileMap *tileMapWalls, TileMap *tileMapPlatforms);
    void setPosition(const glm::vec2 &pos);
    
    glm::ivec2 getPosition();
    bool collisionWithPlayer(const glm::ivec2 &playerPos, const glm::ivec2 &playerSize) const;
    void setMovementLimits(int minXLimit, int maxXLimit);
	int getminX() { return minX; }
    glm::ivec2 getSize() const;
    void freeze();

private:
    Direction dir;
    glm::ivec2 tileMapDispl, posSnake;
    bool bJumping;
    int minX, maxX;

    int jumpAngle, startY;
    Texture spritesheet;
    Sprite *sprite;
    TileMap *mapWalls;
    TileMap *mapPlatforms;
    bool frozen;          // Indica si la serpiente está congelada
    int frozenTimer;      // Contador para el tiempo de congelación
};

#endif // _SNAKE_INCLUDE
