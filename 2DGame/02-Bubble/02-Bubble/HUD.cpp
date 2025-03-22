#include <iostream>
#include <GL/glew.h>
#include "HUD.h"

HUD::HUD()
{
    health = maxHealth = 4;
    lights = maxLights = 2;
    score = 0;
    hasWeapon = false;
}

HUD::~HUD()
{
    // Liberar memoria de los sprites
    for (auto& heart : hearts)
        if (heart != nullptr) delete heart;

    for (auto& light : lightsIcons)
        if (light != nullptr) delete light;

    if (weaponIcon != nullptr) delete weaponIcon;
}

void HUD::init(const glm::ivec2& screenPos, ShaderProgram& shaderProgram)
{
    this->screenPos = screenPos;

    // Inicializar el arma (arriba a la izquierda)
    spritesheet_weapon.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);
    weaponIcon = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.5f), &spritesheet_weapon, &shaderProgram);

    weaponIcon->setNumberAnimations(1);
    weaponIcon->setAnimationSpeed(WEAPON_ACTIVE, 8);
    weaponIcon->addKeyframe(WEAPON_ACTIVE, glm::vec2(0.75f, 0.5f));
    weaponIcon->changeAnimation(WEAPON_ACTIVE);

    // Posicionar el arma arriba a la izquierda
    weaponIcon->setPosition(glm::vec2(float(screenPos.x), float(screenPos.y)));

    // Inicializar luces (a la derecha del arma)
    spritesheet_lights.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Crear 2 iconos de luz, uno al lado del otro
    for (int i = 0; i < maxLights; ++i)
    {
        Sprite* light = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.5f), &spritesheet_lights, &shaderProgram);

        light->setNumberAnimations(1);
        light->setAnimationSpeed(LIGHT_ON, 8);
        light->addKeyframe(LIGHT_ON, glm::vec2(0.5f, 0.5f));
        light->changeAnimation(LIGHT_ON);

        // Posicionar las luces a la derecha del arma
        light->setPosition(glm::vec2(float(screenPos.x + 18 + i * 18), float(screenPos.y)));

        lightsIcons.push_back(light);
    }

    // Inicializar corazones (debajo del arma, verticalmente)
    spritesheet_hearts.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Crear 4 corazones, uno debajo del otro
    for (int i = 0; i < maxHealth; ++i)
    {
        Sprite* heart = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.5f), &spritesheet_hearts, &shaderProgram);

        heart->setNumberAnimations(4);

        heart->setAnimationSpeed(FULL_HEART, 8);
        heart->addKeyframe(FULL_HEART, glm::vec2(0.0f, 0.0f));

        heart->setAnimationSpeed(CASI_FULL, 8);
        heart->addKeyframe(CASI_FULL, glm::vec2(0.25f, 0.0f));

        heart->setAnimationSpeed(CASI_EMPTY, 8);
        heart->addKeyframe(CASI_EMPTY, glm::vec2(0.5f, 0.0f));

        heart->setAnimationSpeed(EMPTY_HEART, 8);
        heart->addKeyframe(EMPTY_HEART, glm::vec2(0.75f, 0.0f));

        heart->changeAnimation(FULL_HEART);

        // Posicionar los corazones verticalmente bajo el arma, con poca separación
        heart->setPosition(glm::vec2(float(screenPos.x), float(screenPos.y + 10 + i * 10)));

        hearts.push_back(heart);
    }
}

void HUD::update(int deltaTime)
{
    for (auto& heart : hearts)
        heart->update(deltaTime);

    for (auto& light : lightsIcons)
        light->update(deltaTime);

    weaponIcon->update(deltaTime);
}

void HUD::render()
{
    // Renderizar todos los elementos de la HUD
    weaponIcon->render();

    for (auto& light : lightsIcons)
        light->render();

    for (auto& heart : hearts)
        heart->render();
}

void HUD::setHealth(int health)
{
    this->health = health;

    // Actualizar animaciones de los corazones
    for (int i = 0; i < maxHealth; ++i)
    {
        if (i < health)
            hearts[i]->changeAnimation(FULL_HEART);
        else
            hearts[i]->changeAnimation(EMPTY_HEART);
    }
}

void HUD::setLights(int lights)
{
    this->lights = lights;

    // Actualizar animaciones de las luces (puedes implementar esto si necesitas diferentes estados)
}

void HUD::updatePosition(float cameraX, float cameraY)
{
    weaponIcon->setPosition(glm::vec2(cameraX + screenPos.x, cameraY + screenPos.y));

    for (int i = 0; i < maxLights; ++i) {
        lightsIcons[i]->setPosition(glm::vec2(cameraX + screenPos.x + 18 + i * 18, cameraY + screenPos.y));
    }

    for (int i = 0; i < maxHealth; ++i) {
        hearts[i]->setPosition(glm::vec2(cameraX + screenPos.x, cameraY + screenPos.y + 14 + i * 10));
    }
}