#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.

enum Direction
{
	LEFT, RIGHT
};

class Player
{

public:
	void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
	void update(int deltaTime);
	void render();
	
	void setTileMap(TileMap *tileMapWalls, TileMap* tileMapPlatforms);
	void setPosition(const glm::vec2 &pos);
	
	glm::ivec2 getPosition();
	void setLeftLimit(float leftLimit);
	
private:
	bool bJumping;
	glm::ivec2 tileMapDispl, posPlayer;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite *sprite;
	Texture spritesheet_lanza;
	Sprite *sprite_lanza;
	TileMap *mapWalls, *mapPlatforms;
	Direction dir;
	bool spear_visible;
<<<<<<< HEAD
	bool first_attack;

=======
	float leftLimit;
>>>>>>> 02c83530c239855136cf81bf10d5559ac8fc5a1d
};


#endif // _PLAYER_INCLUDE


