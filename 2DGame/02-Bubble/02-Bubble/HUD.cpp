#include <iostream>
#include <GL/glew.h>
#include "HUD.h"

HUD::HUD()
{
    lights = maxLights = 2;
    gourdCount = 0;
    maxHearts = 4; // Inicialmente solo 4 corazones disponibles
    for (int i = 0; i < maxHearts; ++i) {
        heartsLogic.push_back(3); // Todos los corazones comienzan llenos (valor 3)
    }
    hasWeapon = false;
    gameover = false;

    // Inicializar los umbrales para obtener más corazones con calabazas
    gourdThresholds = { 9, 12, 16, 22, 30, 42, 62, 99 };
}

HUD::~HUD()
{
    // Liberar memoria de los sprites
    for (auto& heart : hearts)
        if (heart != nullptr) delete heart;

    for (auto& light : lightsIcons)
        if (light != nullptr) delete light;

    if (weaponIcon != nullptr) delete weaponIcon;

    if (flintSpearIcon != nullptr) {
        flintSpearIcon->free();
        delete flintSpearIcon;
    }

    if (buffaloHelmetIcon != nullptr) {
        buffaloHelmetIcon->free();
        delete buffaloHelmetIcon;
    }

    if (deerskinShirtIcon != nullptr) {
        deerskinShirtIcon->free();
        delete deerskinShirtIcon;
    }

   
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

    // Crear 4 iconos de luz (máximo posible), pero mostrar solo los activos inicialmente
    for (int i = 0; i < 4; ++i)
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

    // Inicializar corazones (hasta 12 potenciales, pero solo se mostrarán los disponibles)
    spritesheet_hearts.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Crear 12 corazones pero inicialmente solo se mostrarán 4
    for (int i = 0; i < 12; ++i)
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

        // Posicionamiento en dos filas (6 corazones por fila)
        float xPos = screenPos.x + (i % 6) * 10;
        float yPos = screenPos.y + 18 + (i / 6) * 10; 
        heart->setPosition(glm::vec2(xPos, yPos));

        hearts.push_back(heart);
    }
    spritesheet_powerups.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Inicializar Flint Spear
    flintSpearIcon = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.5f), &spritesheet_powerups, &shaderProgram);
    flintSpearIcon->setNumberAnimations(1);
    flintSpearIcon->setAnimationSpeed(0, 8);
    flintSpearIcon->addKeyframe(0, glm::vec2(0.25f, 0.0f)); // Ajusta a coordenadas correctas
    flintSpearIcon->changeAnimation(0);
    flintSpearIcon->setPosition(glm::vec2(float(screenPos.x), float(screenPos.y + 54))); // Posicionar en fila inferior

    // Inicializar Buffalo Helmet
    buffaloHelmetIcon = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.5f), &spritesheet_powerups, &shaderProgram);
    buffaloHelmetIcon->setNumberAnimations(1);
    buffaloHelmetIcon->setAnimationSpeed(0, 8);
    buffaloHelmetIcon->addKeyframe(0, glm::vec2(0.50f, 0.0f)); // Ajusta a coordenadas correctas
    buffaloHelmetIcon->changeAnimation(0);
    buffaloHelmetIcon->setPosition(glm::vec2(float(screenPos.x + 18), float(screenPos.y + 54))); // A la derecha del flint

    // Inicializar Deerskin Shirt
    deerskinShirtIcon = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.5f), &spritesheet_powerups, &shaderProgram);
    deerskinShirtIcon->setNumberAnimations(1);
    deerskinShirtIcon->setAnimationSpeed(0, 8);
    deerskinShirtIcon->addKeyframe(0, glm::vec2(0.75f, 0.0f)); // Ajusta a coordenadas correctas
    deerskinShirtIcon->changeAnimation(0);
    deerskinShirtIcon->setPosition(glm::vec2(float(screenPos.x + 36), float(screenPos.y + 54))); // A la derecha del helmet

    // Inicializar estados
    flintSpearHits = 0;
    buffaloHelmetHits = 0;
    hasDeerskinShirt = false;

    // Inicializar contador de calabazas
   

    updateHeartAnimations();
}

void HUD::update(int deltaTime)
{
    // Actualizar todos los sprites
    for (auto& heart : hearts)
        heart->update(deltaTime);

    for (auto& light : lightsIcons)
        light->update(deltaTime);

    weaponIcon->update(deltaTime);
    if (flintSpearHits > 0)
        flintSpearIcon->update(deltaTime);

    if (buffaloHelmetHits > 0)
        buffaloHelmetIcon->update(deltaTime);

    if (hasDeerskinShirt)
        deerskinShirtIcon->update(deltaTime);
   

  
}

void HUD::render()
{
    // Renderizar el icono del arma
    weaponIcon->render();

    // Renderizar las luces activas
    for (int i = 0; i < lights; ++i) {
        lightsIcons[i]->render();
    }

    // Renderizar solo los corazones disponibles según maxHearts
    for (int i = 0; i < maxHearts; ++i) {
        hearts[i]->render();
    }
    if (flintSpearHits > 0)
        flintSpearIcon->render();

    if (buffaloHelmetHits > 0)
        buffaloHelmetIcon->render();

    if (hasDeerskinShirt)
        deerskinShirtIcon->render();

}

void HUD::setHealth( std::vector<int>& newHearts)
{
    heartsLogic = newHearts;
    updateHeartAnimations();
}

void HUD::setLights(int newLights)
{
    lights = newLights;

    // Comprobar si se ha perdido el juego
    if (lights <= 0)
        gameover = true;
}

void HUD::updatePosition(float cameraX, float cameraY)
{
    // Actualizar posición del arma
    weaponIcon->setPosition(glm::vec2(cameraX + screenPos.x, cameraY + screenPos.y));

    // Actualizar posición de las luces
    for (int i = 0; i < lights; ++i) {
        lightsIcons[i]->setPosition(glm::vec2(cameraX + screenPos.x + 18 + i * 15, cameraY + screenPos.y));
    }

    // Actualizar posición de los corazones (2 filas de 6)
    for (int i = 0; i < 12; ++i) {
        int column = i / 6;     // 0 para primera columna, 1 para segunda columna
        int row = i % 6;        // 0-5 para filas en cada columna

        float xPos = cameraX + screenPos.x + column * 10;  // Columnas separadas por 18 píxeles
        float yPos = cameraY + screenPos.y + 18 + row * 10; // Filas separadas por 10 píxeles

        hearts[i]->setPosition(glm::vec2(xPos, yPos));
    }
    flintSpearIcon->setPosition(glm::vec2(cameraX + screenPos.x, cameraY + screenPos.y + 120));
    buffaloHelmetIcon->setPosition(glm::vec2(cameraX + screenPos.x + 18, cameraY + screenPos.y + 120));
    deerskinShirtIcon->setPosition(glm::vec2(cameraX + screenPos.x + 36, cameraY + screenPos.y + 120));


}

void HUD::updateHeartAnimations()
{
    // Actualizar todos los corazones disponibles según maxHearts
    for (int i = 0; i < maxHearts && i < hearts.size(); ++i) {
        // Determinar el índice correcto para acceder a heartsLogic
        int logicIndex = i < heartsLogic.size() ? i : heartsLogic.size() - 1;

        int heartLevel = heartsLogic[logicIndex];
        if (heartLevel == 3) {
            hearts[i]->changeAnimation(FULL_HEART);
        }
        else if (heartLevel == 2) {
            hearts[i]->changeAnimation(CASI_FULL);
        }
        else if (heartLevel == 1) {
            hearts[i]->changeAnimation(CASI_EMPTY);
        }
        else {
            hearts[i]->changeAnimation(EMPTY_HEART);
        }
    }
}


void HUD::syncWithPlayer(const std::vector<int>& playerHearts, int playerLights)
{
    heartsLogic = playerHearts;
    lights = playerLights;
	maxHearts = playerHearts.size();
    updateHeartAnimations();
    
}

void HUD::collectSmallHeart()
{
    // Buscar el primer corazón que no esté lleno
    for (int i = 0; i < 4; ++i) {
        if (heartsLogic[i] < 3) {
            heartsLogic[i]++;
            updateHeartAnimations();
            return;
        }
    }

    // Si todos los corazones iniciales están llenos, revisar los adicionales
    for (int i = 4; i < maxHearts; ++i) {
        // Asumimos que cada corazón adicional tiene 3 unidades de salud
        int heartIdx = (i >= 4) ? 3 : i; // Para acceder a heartsLogic[3] para corazones adicionales
        if (heartsLogic[heartIdx] < 3) {
            heartsLogic[heartIdx]++;
            updateHeartAnimations();
            return;
        }
    }
}

void HUD::collectLargeHeart()
{
    // Restaurar todos los corazones disponibles
    for (int i = 0; i < 4; ++i) {
        heartsLogic[i] = 3; // Valor máximo
    }
    updateHeartAnimations();
}

void HUD::collectGourd()
{
    gourdCount++;

    // Comprobar si hemos alcanzado un umbral para obtener un nuevo corazón
    if (maxHearts < 12) { // Máximo de 12 corazones
        if (gourdCount >= gourdThresholds[maxHearts - 4]) {
            // Aumentar el contador de corazones máximos
            maxHearts++;

            // Añadir un nuevo valor lógico para el corazón (lleno - valor 3)
            heartsLogic.push_back(3);

            // No necesitamos crear un nuevo sprite, ya que los sprites para los 12 posibles
            // corazones se crean durante la inicialización del HUD en init()

            // Actualizar la animación de todos los corazones, incluido el nuevo
            updateHeartAnimations();

            // Opcionalmente, puedes reproducir un sonido o mostrar un efecto visual
            // para indicar que se ha ganado un nuevo corazón
        }
    }
}


void HUD::updateGourdCounter()
{
    // Actualizar los dígitos para mostrar la cantidad actual de calabazas
    int count = gourdCount;

    // Dividir el número en dígitos individuales
    int digitos[3];
    digitos[0] = count / 100;         // Centenas
    digitos[1] = (count / 10) % 10;   // Decenas
    digitos[2] = count % 10;          // Unidades

    // Actualizar las animaciones de los dígitos
    for (int i = 0; i < 3; ++i) {
        gourdDigits[i]->changeAnimation(digitos[i]);
    }
}

bool HUD::isGameOver() const
{
    return gameover;
}

int HUD::getGourdsCount() const
{
    return gourdCount;
}

int HUD::getMaxHearts() const
{
    return maxHearts;
}

void HUD::collectPotion()
{
    if (lights < 3) {
        // Incrementar contador de luces
        lights++;
        // No necesitamos crear un nuevo sprite, solo actualizar la posición
        // en caso de que haya cambiado la cámara
        updatePosition(screenPos.x, screenPos.y);
    }
    else {
        // Si ya tenemos el máximo, restaurar todos los corazones
        collectLargeHeart();
    }
}

void HUD::updatePowerUpIcons(int flintSpearHits, int buffaloHelmetHits, bool hasDeerskinShirt)
{
    this->flintSpearHits = flintSpearHits;
    this->buffaloHelmetHits = buffaloHelmetHits;
    this->hasDeerskinShirt = hasDeerskinShirt;
}


