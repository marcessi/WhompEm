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

enum WeaponType
{
    SPEAR, ICE_TOTEM
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
    WeaponType getCurrentWeapon() const { return currentWeapon; }

    glm::ivec2 getPosition();
    void setLeftLimit(float leftLimit);
    void decrementFlintSpearHits();
    bool isJumping() const { return bJumping; }
    void isHitted();
    pair<std::vector<int>, int> getplayerLifes();
    bool isProtecting() const;
   

    // Nuevos métodos para powerups
    void collectSmallHeart();
    void collectLargeHeart();
    void collectGourd();
    int getGourds() const { return gourds; }
    int getMaxHearts() const { return maxHearts; }
    bool isGameOver() const { return gameover; }
    void increaseMaxHearts();
    void collectPotion();
    void collectFlintSpear();
    void collectBuffaloHelmet();
    void collectDeerskinShirt();

    int getFlintSpearHits() const;
    int getBuffaloHelmetHits() const;
    bool hasDeerskinShirt() const;


    void activateSpearLarge(bool activate); // Función para activar/desactivar la lanza grande
    bool isSpearLargeActive() const { return spearLargeActive; }

private:
    bool bJumping;
    glm::ivec2 tileMapDispl, posPlayer;
    int jumpAngle, startY;
    Texture spritesheet;
    Sprite* sprite;
    Texture spritesheet_lanza;
    Sprite* sprite_lanza;
    Texture spritesheet_ice_totem;
    Sprite* sprite_ice_totem;
    WeaponType currentWeapon;
    bool weaponSwitchPressed;

    bool spearLargeActive;

    TileMap* mapWalls, * mapPlatforms;
    const std::vector<MovingPlatform*>* movingPlatforms; // Nueva referencia
    Direction dir;
    bool spear_visible;
    float leftLimit;
    bool first_attack;
    bool hitted;
    int ligths;
    bool gameover;
    std::vector<int> hearts;
    bool invulnerable;
    int hitTimer;
    int invulnerableTimer;
    bool player_visible;
    bool jumpingIce;

    // Variables para el salto al recibir daño
    bool knockbackJumping;
    int knockbackAngle;
    int knockbackStartY;
    int knockbackDir; // 1 o -1 según dirección

    AudioManager* audioManager;
    bool jumpSoundPlayed;
    bool spearSoundPlayed;
    bool iceTotemSoundPlaying;

    bool godModeActive;
    bool godModeKeyPressed;
    bool recoverLivesKeyPressed;

    // Nuevas variables para el sistema de power-ups
    int gourds;
    int maxHearts;
    std::vector<int> gourdThresholds;
    int flintSpearHits;       // Número de golpes restantes con flint spear (máx. 4)
    int buffaloHelmetHits;    // Número de golpes restantes con buffalo helmet (máx. 4)
    bool deerskinShirtActive; // Si el deerskin shirt está activo
    float deerskinTimer;
};

#endif // _PLAYER_INCLUDE
