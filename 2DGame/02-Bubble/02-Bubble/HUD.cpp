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
    showBossHealth = false;
    bossIcon = nullptr;
    bossHeartsLogic.clear();

    // Inicializar los umbrales para obtener m�s corazones con calabazas
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

    // Liberar memoria de los sprites del boss
    if (bossIcon != nullptr) {
        bossIcon->free();
        delete bossIcon;
    }

    for (auto& heart : bossHearts) {
        if (heart != nullptr) {
            heart->free();
            delete heart;
        }
    }

   
}

void HUD::init(const glm::ivec2& screenPos, ShaderProgram& shaderProgram)
{
    this->screenPos = screenPos;

    // Inicializar el arma (arriba a la izquierda)
    spritesheet_weapon.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);
    weaponIcon = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.25f), &spritesheet_weapon, &shaderProgram);

    weaponIcon->setNumberAnimations(2);
    weaponIcon->setAnimationSpeed(WEAPON_SPEAR, 8);
    weaponIcon->addKeyframe(WEAPON_SPEAR, glm::vec2(0.75f, 0.25f));
    weaponIcon->setAnimationSpeed(WEAPON_ICE_TOTEM, 8);
    weaponIcon->addKeyframe(WEAPON_ICE_TOTEM, glm::vec2(0.25f, 0.75f)); // Suponiendo que hay una textura en esta posici�n

    weaponIcon->changeAnimation(WEAPON_SPEAR);

    // Posicionar el arma arriba a la izquierda
    weaponIcon->setPosition(glm::vec2(float(screenPos.x), float(screenPos.y)));

    // Inicializar luces (a la derecha del arma)
    spritesheet_lights.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Crear 4 iconos de luz (m�ximo posible), pero mostrar solo los activos inicialmente
    for (int i = 0; i < 4; ++i)
    {
        Sprite* light = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.25f), &spritesheet_lights, &shaderProgram);

        light->setNumberAnimations(1);
        light->setAnimationSpeed(LIGHT_ON, 8);
        light->addKeyframe(LIGHT_ON, glm::vec2(0.5f, 0.25f));
        light->changeAnimation(LIGHT_ON);

        // Posicionar las luces a la derecha del arma
        light->setPosition(glm::vec2(float(screenPos.x + 18 + i * 18), float(screenPos.y)));

        lightsIcons.push_back(light);
    }

    // Inicializar corazones (hasta 12 potenciales, pero solo se mostrar�n los disponibles)
    spritesheet_hearts.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Crear 12 corazones pero inicialmente solo se mostrar�n 4
    for (int i = 0; i < 12; ++i)
    {
        Sprite* heart = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.25f), &spritesheet_hearts, &shaderProgram);

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
    flintSpearIcon = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.25f), &spritesheet_powerups, &shaderProgram);
    flintSpearIcon->setNumberAnimations(1);
    flintSpearIcon->setAnimationSpeed(0, 8);
    flintSpearIcon->addKeyframe(0, glm::vec2(0.0f, 0.50f)); // Ajusta a coordenadas correctas
    flintSpearIcon->changeAnimation(0);
    flintSpearIcon->setPosition(glm::vec2(float(screenPos.x), float(screenPos.y + 54))); // Posicionar en fila inferior

    // Inicializar Buffalo Helmet
    buffaloHelmetIcon = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.25f), &spritesheet_powerups, &shaderProgram);
    buffaloHelmetIcon->setNumberAnimations(1);
    buffaloHelmetIcon->setAnimationSpeed(0, 8);
    buffaloHelmetIcon->addKeyframe(0, glm::vec2(0.250f, 0.50f)); // Ajusta a coordenadas correctas
    buffaloHelmetIcon->changeAnimation(0);
    buffaloHelmetIcon->setPosition(glm::vec2(float(screenPos.x + 18), float(screenPos.y + 54))); // A la derecha del flint

    // Inicializar Deerskin Shirt
    deerskinShirtIcon = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.25f, 0.25f), &spritesheet_powerups, &shaderProgram);
    deerskinShirtIcon->setNumberAnimations(1);
    deerskinShirtIcon->setAnimationSpeed(0, 8);
    deerskinShirtIcon->addKeyframe(0, glm::vec2(0.5f, 0.50f)); // Ajusta a coordenadas correctas
    deerskinShirtIcon->changeAnimation(0);
    deerskinShirtIcon->setPosition(glm::vec2(float(screenPos.x + 36), float(screenPos.y + 54))); // A la derecha del helmet

    // Inicializar estados
    flintSpearHits = 0;
    buffaloHelmetHits = 0;
    hasDeerskinShirt = false;

    // Inicializar contador de calabazas

    
    bossIconTexture.loadFromFile("images/bossShits.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Inicializar corazones del boss (misma textura que los del jugador)
    for (int i = 0; i < 7; ++i) // 9 corazones m�ximo para el boss
    {
        Sprite* heartBoss = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.125f, 1.f), &bossIconTexture, &shaderProgram);

        heartBoss->setNumberAnimations(4);

        heartBoss->setAnimationSpeed(FULL_HEART, 8);
        heartBoss->addKeyframe(FULL_HEART, glm::vec2(0.0f, 0.0f));

        heartBoss->setAnimationSpeed(CASI_FULL, 8);
        heartBoss->addKeyframe(CASI_FULL, glm::vec2(0.125f, 0.0f));

        heartBoss->setAnimationSpeed(CASI_EMPTY, 8);
        heartBoss->addKeyframe(CASI_EMPTY, glm::vec2(0.250f, 0.0f));

        heartBoss->setAnimationSpeed(EMPTY_HEART, 8);
        heartBoss->addKeyframe(EMPTY_HEART, glm::vec2(0.375f, 0.0f));

        heartBoss->changeAnimation(FULL_HEART);

        // Posicionar en fila horizontal despu�s del icono del boss
        float xPos = screenPos.x + (i % 6) * 10;
        float yPos = screenPos.y + 18 + (i / 6) * 10;
        heartBoss->setPosition(glm::vec2(xPos +35, yPos));
        bossHearts.push_back(heartBoss);
    }

    // Inicialmente no mostramos la salud del boss
    showBossHealth = false;
   

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

    if (showBossHealth) {
        

        for (auto& heart : bossHearts) {
            heart->update(deltaTime);
        }
    }
   

  
}

void HUD::render()
{
    // Renderizar el icono del arma
    weaponIcon->render();

    // Renderizar las luces activas
    for (int i = 0; i < lights; ++i) {
        lightsIcons[i]->render();
    }

    // Renderizar solo los corazones disponibles seg�n maxHearts
    for (int i = 0; i < maxHearts; ++i) {
        hearts[i]->render();
    }
    if (flintSpearHits > 0)
        flintSpearIcon->render();

    if (buffaloHelmetHits > 0)
        buffaloHelmetIcon->render();

    if (hasDeerskinShirt)
        deerskinShirtIcon->render();

    if (showBossHealth) {

        // Solo renderizar los corazones necesarios
        for (size_t i = 0; i < bossHeartsLogic.size(); ++i) {
            bossHearts[i]->render();
        }
    }

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
    // Actualizar posici�n del arma
    weaponIcon->setPosition(glm::vec2(cameraX + screenPos.x, cameraY + screenPos.y));

    // Actualizar posici�n de las luces
    for (int i = 0; i < lights; ++i) {
        lightsIcons[i]->setPosition(glm::vec2(cameraX + screenPos.x + 18 + i * 15, cameraY + screenPos.y));
    }

    // Actualizar posici�n de los corazones (2 filas de 6)
    for (int i = 0; i < 12; ++i) {
        int column = i / 6;     // 0 para primera columna, 1 para segunda columna
        int row = i % 6;        // 0-5 para filas en cada columna

        float xPos = cameraX + screenPos.x + column * 10 - 4;  // Columnas separadas por 18 p�xeles
        float yPos = cameraY + screenPos.y + 15 + row * 10; // Filas separadas por 10 p�xeles

        hearts[i]->setPosition(glm::vec2(xPos, yPos));
    }
    flintSpearIcon->setPosition(glm::vec2(cameraX + screenPos.x, cameraY + screenPos.y + 207));
    buffaloHelmetIcon->setPosition(glm::vec2(cameraX + screenPos.x + 18, cameraY + screenPos.y + 207));
    deerskinShirtIcon->setPosition(glm::vec2(cameraX + screenPos.x + 36, cameraY + screenPos.y + 207));

    for (int i = 0; i < 7; ++i) {
        int column = i / 6;     // 0 para primera columna, 1 para segunda columna
        int row = i % 6;        // 0-5 para filas en cada columna

        float xPos = cameraX + screenPos.x + column * 10 + 16;  // Columnas separadas por 18 p�xeles
        float yPos = cameraY + screenPos.y + 15 + row * 10; // Filas separadas por 10 p�xeles

        bossHearts[i]->setPosition(glm::vec2(xPos, yPos));
    }


}

void HUD::updateHeartAnimations()
{
    // Actualizar todos los corazones disponibles seg�n maxHearts
    for (int i = 0; i < maxHearts && i < hearts.size(); ++i) {
        // Determinar el �ndice correcto para acceder a heartsLogic
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
    // Buscar el primer coraz�n que no est� lleno
    for (int i = 0; i < 4; ++i) {
        if (heartsLogic[i] < 3) {
            heartsLogic[i]++;
            updateHeartAnimations();
            return;
        }
    }

    // Si todos los corazones iniciales est�n llenos, revisar los adicionales
    for (int i = 4; i < maxHearts; ++i) {
        // Asumimos que cada coraz�n adicional tiene 3 unidades de salud
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
        heartsLogic[i] = 3; // Valor m�ximo
    }
    updateHeartAnimations();
}

void HUD::collectGourd()
{
    gourdCount++;

    // Comprobar si hemos alcanzado un umbral para obtener un nuevo coraz�n
    if (maxHearts < 12) { // M�ximo de 12 corazones
        if (gourdCount >= gourdThresholds[maxHearts - 4]) {
            // Aumentar el contador de corazones m�ximos
            maxHearts++;

            // A�adir un nuevo valor l�gico para el coraz�n (lleno - valor 3)
            heartsLogic.push_back(3);

            // No necesitamos crear un nuevo sprite, ya que los sprites para los 12 posibles
            // corazones se crean durante la inicializaci�n del HUD en init()

            // Actualizar la animaci�n de todos los corazones, incluido el nuevo
            updateHeartAnimations();

            // Opcionalmente, puedes reproducir un sonido o mostrar un efecto visual
            // para indicar que se ha ganado un nuevo coraz�n
        }
    }
}


void HUD::updateGourdCounter()
{
    // Actualizar los d�gitos para mostrar la cantidad actual de calabazas
    int count = gourdCount;

    // Dividir el n�mero en d�gitos individuales
    int digitos[3];
    digitos[0] = count / 100;         // Centenas
    digitos[1] = (count / 10) % 10;   // Decenas
    digitos[2] = count % 10;          // Unidades

    // Actualizar las animaciones de los d�gitos
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
        // No necesitamos crear un nuevo sprite, solo actualizar la posici�n
        // en caso de que haya cambiado la c�mara
        updatePosition(screenPos.x, screenPos.y);
    }
    else {
        // Si ya tenemos el m�ximo, restaurar todos los corazones
        collectLargeHeart();
    }
}

void HUD::updatePowerUpIcons(int flintSpearHits, int buffaloHelmetHits, bool hasDeerskinShirt)
{
    this->flintSpearHits = flintSpearHits;
    this->buffaloHelmetHits = buffaloHelmetHits;
    this->hasDeerskinShirt = hasDeerskinShirt;
}

void HUD::setBossHealth(const std::vector<int>& bossHealth)
{
    bossHeartsLogic = bossHealth;
    updateBossHeartAnimations();
}

void HUD::showBossHealthBar(bool show)
{
    showBossHealth = show;
}

void HUD::updateBossHeartAnimations()
{
    // Actualizar la animaci�n de cada coraz�n del boss seg�n su estado l�gico
    for (size_t i = 0; i < bossHeartsLogic.size() && i < bossHearts.size(); ++i) {
        int heartLevel = bossHeartsLogic[i];
        if (heartLevel == 3) {
            bossHearts[i]->changeAnimation(FULL_HEART);
        }
        else if (heartLevel == 2) {
            bossHearts[i]->changeAnimation(CASI_FULL);
        }
        else if (heartLevel == 1) {
            bossHearts[i]->changeAnimation(CASI_EMPTY);
        }
        else {
            bossHearts[i]->changeAnimation(EMPTY_HEART);
        }
    }
}
void HUD::updateWeaponIcon(WeaponType weaponType)
{
    if (weaponType == SPEAR) {
		cout << "Spear" << endl;
        weaponIcon->changeAnimation(WEAPON_SPEAR);
    }
    else if (weaponType == ICE_TOTEM) {
		cout << "Ice Totem" << endl;
        weaponIcon->changeAnimation(WEAPON_ICE_TOTEM);
    }
}

