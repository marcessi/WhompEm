#ifndef _ORCO_INCLUDE
#define _ORCO_INCLUDE

#include "Sprite.h"
#include "TileMap.h"
#include "MovingPlatform.h"

#include <vector>
#include <functional>

// Clase para el enemigo Orco que se mueve saltando
class Orco
{
public:
    // Enumeración para la dirección del Orco
    enum Direction { LEFT, RIGHT };

    Orco();
    ~Orco();

    void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram, Direction initialDir = LEFT);
    void update(int deltaTime);
    void render();

    void hitWithFlint();
    void setTileMap(TileMap* tileMapWalls, TileMap* tileMapPlatforms);
    void setPosition(const glm::vec2& pos);
    void setMovingPlatforms(const std::vector<MovingPlatform*>* platforms);
    void setPlayerPosition(const glm::ivec2* playerPosition);
    void setPlayerHitCallback(std::function<void()> callback); // Función para dañar al jugador

    glm::ivec2 getPosition() const;
    glm::ivec2 getSize() const;
    bool isAlive() const;
    void hit(); // Función para recibir daño
    void kill();
    void freeze();

private:
    // Sprites
    Texture spritesheet;
    Sprite* sprite;

    // Posición y movimiento
    glm::ivec2 tileMapDispl;
    glm::ivec2 posOrco;
    Direction dir;

    // Salto y caída
    bool bJumping;
    bool bFalling; // Indica si está cayendo para encontrar suelo
    int jumpAngle, startY;

    // Colisiones
    TileMap* mapWalls;
    TileMap* mapPlatforms;
    const std::vector<MovingPlatform*>* movingPlatforms;

    // Estado
    bool alive;
    int lives; // Contador de vidas
    bool isHit; // Indica si está en modo "golpeado"
    int hitTime; // Tiempo transcurrido desde el último golpe
    bool frozen;
    int frozenTimer;

    // Referencias
    const glm::ivec2* playerPos; // Puntero a la posición del jugador
    std::function<void()> playerHitted; // Callback para notificar colisión con jugador

    // Temporizadores
    int waitingTime; // Tiempo de espera antes de saltar
    bool inFlightMode; // Indica si está en modo vuelo/preparando salto
};

#endif // _ORCO_INCLUDE
