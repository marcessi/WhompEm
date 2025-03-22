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

    // Inicialización
    void init(const glm::ivec2 &screenPos, ShaderProgram &shaderProgram);
    
    // Actualización y renderizado
    void update(int deltaTime);
    void render();

    // Configuración de estado
    void setHealth(int health);
    void setLights(int lights);

	void updatePosition(float cameraX, float cameraY);

private:
    // Posición de la HUD en pantalla
    glm::ivec2 screenPos;

    // Datos del juego
    int health, maxHealth;
    int lights, maxLights;
    int score;
    bool hasWeapon;

    // Texturas y sprites
    Texture spritesheet_hearts;
    std::vector<Sprite*> hearts;

    Texture spritesheet_lights;
    std::vector<Sprite*> lightsIcons;

    Texture spritesheet_weapon;
    Sprite* weaponIcon;

    // Animaciones para los elementos de la HUD
    enum HeartAnimations { FULL_HEART, CASI_FULL, CASI_EMPTY, EMPTY_HEART};
    enum LightAnimations { LIGHT_ON };
    enum WeaponAnimations { WEAPON_ACTIVE };
};

#endif // _HUD_INCLUDE
