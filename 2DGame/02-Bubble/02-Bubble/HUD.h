#ifndef _HUD_INCLUDE
#define _HUD_INCLUDE

#include <vector>
#include <glm/glm.hpp>
#include "Sprite.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Player.h" // Incluir para tener acceso a WeaponType


class HUD {
public:
    HUD();
    ~HUD();

    void init(const glm::ivec2& screenPos, ShaderProgram& shaderProgram);
    void update(int deltaTime);
    void render();
    void updatePosition(float cameraX, float cameraY);

    // M�todos para actualizar el estado del HUD
    void setHealth(std::vector<int>& newHearts);
    void setLights(int newLights);
    void syncWithPlayer(const std::vector<int>& playerHearts, int playerLights);

    // Nuevos m�todos para power-ups
    void collectSmallHeart();
    void collectLargeHeart();
    void collectGourd();
    void updatePowerUpIcons(int flintSpearHits, int buffaloHelmetHits, bool hasDeerskinShirt);


    // M�todos de acceso
    bool isGameOver() const;
    int getGourdsCount() const;
    int getMaxHearts() const;

	void collectPotion();

    void setBossHealth(const std::vector<int>& bossHealth);
    void showBossHealthBar(bool show);
    void updateBossHeartAnimations();
    void updateWeaponIcon(WeaponType weaponType);

private:
    // Enumeraciones para las animaciones
    enum HeartAnim { FULL_HEART, CASI_FULL, CASI_EMPTY, EMPTY_HEART };
    enum LightAnim { LIGHT_ON };
    enum WeaponAnim { WEAPON_SPEAR, WEAPON_ICE_TOTEM };


    // M�todos de actualizaci�n interna
    void updateHeartAnimations();
    void updateGourdCounter();

    // Posici�n en pantalla
    glm::ivec2 screenPos;

    // Sprites para el HUD
    std::vector<Sprite*> hearts;
    std::vector<Sprite*> lightsIcons;
    Sprite* weaponIcon;
    Sprite* gourdCounter;
    std::vector<Sprite*> gourdDigits;

    // Texturas
    Texture spritesheet_hearts;
    Texture spritesheet_weapon;
    Texture spritesheet_lights;
    Texture spritesheet_gourds;
    Texture spritesheet_numbers;
    Texture bossIconTexture;

    // Estado l�gico
    std::vector<int> heartsLogic;
    int lights;
    int maxLights;
    int maxHearts;  // N�mero m�ximo de corazones disponibles (inicialmente 4)
    int gourdCount; // Contador de calabazas recolectadas
    std::vector<int> gourdThresholds; // Umbrales para desbloquear corazones

    // Estados
    bool hasWeapon;
    bool gameover;

    Sprite* flintSpearIcon;
    Sprite* buffaloHelmetIcon;
    Sprite* deerskinShirtIcon;

    // Texturas
    Texture spritesheet_powerups;

    // Estado
    int flintSpearHits;
    int buffaloHelmetHits;
    bool hasDeerskinShirt;

    Sprite* bossIcon;              // Icono que representa al boss
    std::vector<Sprite*> bossHearts; // Corazones del boss (m�ximo 9)
    std::vector<int> bossHeartsLogic; // Estado l�gico de los corazones del boss
    bool showBossHealth;
};

#endif // _HUD_INCLUDE
