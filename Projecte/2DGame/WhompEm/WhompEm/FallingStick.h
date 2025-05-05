#ifndef _FALLING_STICK_INCLUDE
#define _FALLING_STICK_INCLUDE

#include "Sprite.h"
#include "TileMap.h"

class FallingStick
{
public:
    // Enumeraciones para estados
    enum State {
        WAITING,     // Esperando para caer
        FALLING,     // Cayendo
        STOPPED,     // Detenido despu�s de colisi�n
        DISAPPEARING, // Desvaneci�ndose antes de ser eliminado
        KILLED
    };

    FallingStick();
    ~FallingStick();

    void init(const glm::ivec2 &tileMapPos, ShaderProgram &shaderProgram);
    void update(int deltaTime);
    void render();
    void setWaiting(float time);
    
    void setTileMap(TileMap *tileMapWalls, TileMap *tileMapPlatforms);
    void setPosition(const glm::vec2 &pos);
    void startFalling(); // Inicia la ca�da desde la posici�n actual
    void stopFalling();  // Detiene la ca�da (por ejemplo, tras colisi�n)
    
    bool isDisappearing() const; // Comprueba si est� desapareciendo (para eliminarlo)
    bool isActive() const; // Comprueba si est� activo (cayendo o detenido)
    
    glm::ivec2 getPosition() const;
    glm::ivec2 getSize() const;
    
    void killStick(float directionX);
    bool isKilled() const;
    bool collisionWithPlayer(const glm::ivec2 &playerPos, const glm::ivec2 &playerSize) const;

private:
    glm::ivec2 tileMapDispl;  // Desplazamiento del tilemap
    glm::ivec2 position;      // Posici�n del palo
    glm::ivec2 size;          // Tama�o del palo (colisi�n)
    
    Texture spritesheet;      // Textura del palo
    Sprite *sprite;           // Sprite del palo
    
    TileMap *mapWalls;        // Referencia al mapa de paredes
    TileMap *mapPlatforms;    // Referencia al mapa de plataformas
    
    // Variables de estado
    State currentState;       // Estado actual
    float fallSpeed;          // Velocidad de ca�da
    float waitTime;           // Tiempo de espera antes de caer
    float disappearTimer;     // Temporizador para desaparecer
    float disappearDuration;  // Duraci�n de la desaparici�n
    float alpha;              // Opacidad para desvanecer
    
    // Enumeraci�n para animaciones
    enum StickAnims {
        STICK_IDLE,   // Esperando
        STICK_FALLING // Cayendo
    };

    // Variables para el mini-salto cuando es bloqueado
    float killJumpAngle;
    float killStartY;
    float killDir; // Direcci�n del mini-salto (-1 izquierda, 1 derecha)

};

#endif // _FALLING_STICK_INCLUDE
