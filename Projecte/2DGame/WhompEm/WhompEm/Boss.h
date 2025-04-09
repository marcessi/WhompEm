#ifndef _BOSS_INCLUDE
#define _BOSS_INCLUDE

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include "Sprite.h"
#include "TileMap.h"
#include "ShaderProgram.h"
#include "FallingStick.h"

class Boss
{
public:
    enum BossState {
        IDLE,       // Estado inicial y entre ataques
        ATTACKING,  // Volando y lanzando palos
        CRAZY,      // Movimiento errático
        GROUNDED    // En el suelo
    };

    enum BossAnimations {
        ANIM_IDLE,
        ANIM_ATTACKING_LEFT,
		ANIM_ATTACKING_RIGHT,
        ANIM_CRAZY,
        ANIM_GROUNDED
    };

    Boss();
    ~Boss();

    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
    void update(int deltaTime);
    void render();

    void setTileMap(TileMap* tileMapWalls, TileMap* tileMapPlatforms);
    void setPosition(const glm::vec2& pos);
    void setPlayerPosition(const glm::ivec2* playerPos);
    void setPlayerHitCallback(std::function<void()> callback);
    void hit(); // Cuando el jugador golpea al jefe

    std::pair<std::vector<int>, int> getLives() const;

    bool isAlive() const;
    glm::ivec2 getPosition() const;
    glm::ivec2 getSize() const;
    std::vector<FallingStick*>& getFallingSticks();

    const char* getStateString() const {
        switch (state) {
        case IDLE: return "IDLE";
        case ATTACKING: return "ATTACKING";
        case CRAZY: return "CRAZY";
        case GROUNDED: return "GROUNDED";
        default: return "UNKNOWN";
        }
    }

private:
    void updateIdleState(int deltaTime);
    void updateAttackingState(int deltaTime);
    void updateCrazyState(int deltaTime);
    void updateGroundedState(int deltaTime);
    void createFallingStick();

    // Estados
    BossState state;
    float stateTimer;
    bool alive;

    // Vidas
    std::vector<int> hearts;  // Nuevo: Vector de corazones, cada uno con 3 puntos de vida
    int maxHearts;
    bool invulnerable;
    float invulnerableTimer;

    // Gráficos
    Sprite* sprite;
    Texture spritesheet;
    glm::ivec2 tileMapDispl;
    glm::vec2 position;
    bool visible;

    // Movimiento
    float vx, vy;
    float initialY;
    float amplitude;
    float frequency;
    float phase;
    bool firstTime;
    float minX, maxX;
    struct Leaf {
        Sprite* sprite;
        float angle;
        float radius;
        float rotation_speed;
        int animation_timer;
    };

    std::vector<Leaf> leaves;
    Texture leafSpritesheet;
    bool leaves_active;
    float leaf_animation_timer;
    float circle_radius;
    float circle_max_radius;
    bool expand_circle;
    glm::vec2 leaves_center;

    // Ataques
    std::vector<FallingStick*> fallingSticks;
    float attackTimer;
    float stickSpawnTimer;

    // Referencias
    TileMap* mapWalls;
    TileMap* mapPlatforms;
    const glm::ivec2* playerPos;
    std::function<void()> playerHitCallback;
};

#endif // _BOSS_INCLUDE
