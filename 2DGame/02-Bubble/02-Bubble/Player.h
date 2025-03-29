#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE

#include "Sprite.h"
#include "TileMap.h"
#include "MovingPlatform.h"
#include "AudioManager.h"
#include <vector>           // Para std::vector

// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.

enum Direction
{
    LEFT, RIGHT
};

class Player
{

public:
    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
    void update(int deltaTime);
    void render();

    void setTileMap(TileMap* tileMapWalls, TileMap* tileMapPlatforms);
    void setMovingPlatforms(const std::vector<MovingPlatform*>* platforms);
    void setAudioManager(AudioManager* audioManager);
    void setPosition(const glm::vec2& pos);
    bool checkSpearCollision(const glm::ivec2& enemyPos, const glm::ivec2& enemySize);
    bool isInvulnerable() const { return invulnerable; }

    glm::ivec2 getPosition();
    void setLeftLimit(float leftLimit);
    bool isJumping() const { return bJumping; }
    void isHitted();
    pair<glm::ivec4, int> getplayerLifes();


private:
    bool bJumping;
    glm::ivec2 tileMapDispl, posPlayer;
    int jumpAngle, startY;
    Texture spritesheet;
    Sprite* sprite;
    Texture spritesheet_lanza;
    Sprite* sprite_lanza;
    TileMap* mapWalls, * mapPlatforms;
    const std::vector<MovingPlatform*>* movingPlatforms; // Nueva referencia
    Direction dir;
    bool spear_visible;
    float leftLimit;
    bool first_attack;
    bool hitted;
    int ligths;
    bool gameover;
    glm::ivec4 hearts;
	bool invulnerable;
	int hitTimer;
	int invulnerableTimer;
	bool player_visible;

    // Variables para el salto al recibir daño
    bool knockbackJumping;
    int knockbackAngle;
    int knockbackStartY;
    int knockbackDir; // 1 o -1 según dirección

    AudioManager* audioManager;
    bool jumpSoundPlayed;
    bool spearSoundPlayed;


};

#endif // _PLAYER_INCLUDE
