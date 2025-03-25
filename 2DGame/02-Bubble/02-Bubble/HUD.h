#ifndef _HUD_INCLUDE
#define _HUD_INCLUDE

#include <vector>
#include <glm/glm.hpp>
#include "Sprite.h"
#include "ShaderProgram.h"
#include "Texture.h"

class HUD
{
public:
    // Constructor y destructor
    HUD();
    ~HUD();

    // Inicializaci�n
    void init(const glm::ivec2 &screenPos, ShaderProgram &shaderProgram);
    
    // Actualizaci�n y renderizado
    void update(int deltaTime);
    void render();

    // Configuraci�n de estado
    void setHealth(const glm::ivec4& newHearts);
    void setLights(int newLights);
    void syncWithPlayer(const glm::ivec4& playerHearts, int playerLights);
    bool isGameOver() const;


	void updatePosition(float cameraX, float cameraY);

private:
    // Posici�n de la HUD en pantalla
    glm::ivec2 screenPos;
    void updateHeartAnimations();
    void updateLightAnimations();
    // Datos del juego
    glm::ivec4 heartsLogic;  // 4 corazones con niveles 0-4
    int lights;        // N�mero de luces actuales
    int maxLights;     // N�mero m�ximo de luces
    bool gameover;
    bool hasWeapon;

    // Texturas y sprites
    Texture spritesheet_hearts;
    std::vector<Sprite*> hearts;

    Texture spritesheet_lights;
    std::vector<Sprite*> lightsIcons;

    Texture spritesheet_weapon;
    Sprite* weaponIcon;

    // Animaciones para los elementos de la HUD
    enum HeartAnimations {
		CASI_FULL, CASI_EMPTY, FULL_HEART, EMPTY_HEART
    };
    enum LightAnimations { LIGHT_ON };
    enum WeaponAnimations { WEAPON_ACTIVE };
};

#endif // _HUD_INCLUDE
