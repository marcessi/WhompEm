﻿#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 16


#define INIT_PLAYER_X_TILES 1 /*1 123 131 188 205*/
#define INIT_PLAYER_Y_TILES 10 /*10 3 99 99 33*/

#define NUM_STICKS 20

#define STICK_RANGE1_MIN 1020.0f
#define STICK_RANGE1_MAX 1500.0f
#define STICK_RANGE2_MIN 1850.0f
#define STICK_RANGE2_MAX 2000.0f



Scene::Scene()
{
    mapWalls = NULL;
    mapBackground = NULL;
    mapPlatforms = NULL;

    player = NULL;
    followHorizontal = true; // Inicializa la variable para seguir horizontalmente
    currentCheckpoint = 0; // Inicializa el �ndice del punto de control actual
    isAnimating = false; // Inicializa la variable de animaci�n
    bossCam = false;
    hud = NULL;
    animationProgress = 0.0f; // Inicializa el progreso de la animaci�n
    snakeSpawnTimer = 0.0f;

    mapFrontal = NULL;
    player = NULL;
    followHorizontal = true; // Inicializa la variable para seguir horizontalmente
    currentCheckpoint = 0;   // Inicializa el �ndice del punto de control actual
    isAnimating = false;     // Inicializa la variable de animaci�n
    bossCam = false;
    animationProgress = 0.0f; // Inicializa el progreso de la animaci�n

    // Inicializar variables del men�
    gameState = MENU_MAIN;
    currentOption = OPTION_START_GAME;
    menuTime = 0.0f;
    keyPressed = false;
    keyPressedTimer = 0;

    gameOverSprite = NULL;
    victorySprite = NULL;
    bossDefeated = false;

    for (int i = 0; i < 3; i++) {
        mainMenuSprites[i] = NULL;
    }
    instructionsSprite = NULL;
    creditsSprite = NULL;
    sticksCreated = false;

}

Scene::~Scene()
{
    if (mapWalls != NULL)
        delete mapWalls;
    if (mapBackground != NULL)
        delete mapBackground;
    if (mapPlatforms != NULL)
        delete mapPlatforms;
    if (mapFrontal != NULL)
        delete mapFrontal;
    if (mapSpikes != NULL)
        delete mapSpikes;
    if (player != NULL)
        delete player;
    if (hud != NULL)
        delete hud;

    // Liberar la memoria de las plataformas m�viles
    for (int i = 0; i < 3; i++) {
        if (mainMenuSprites[i] != NULL) {
            mainMenuSprites[i]->free();
            delete mainMenuSprites[i];
        }
    }

    if (gameOverSprite != NULL) {
        gameOverSprite->free();
        delete gameOverSprite;
    }

    if (victorySprite != NULL) {
        victorySprite->free();
        delete victorySprite;
    }

    if (instructionsSprite != NULL) {
        instructionsSprite->free();
        delete instructionsSprite;
    }

    if (creditsSprite != NULL) {
        creditsSprite->free();
        delete creditsSprite;
    }

    // Liberar la memoria de las plataformas m�viles
    for (auto platform : movingPlatforms) {
        delete platform;
    }

    for (auto snake : snakes) {
        delete snake;
    }
    snakes.clear();

	for (auto stick : fallingSticks) {
		delete stick;
	}
	fallingSticks.clear();

    for (auto orco : orcos) {
        delete orco;
    }
    orcos.clear();
    spawnPointToOrco.clear();

    for (auto powerUp : powerUps) {
        delete powerUp;
    }
    powerUps.clear();

    if (boss != nullptr) {
        delete boss;
    }

	movingPlatforms.clear();
}

void Scene::init()
{
    initShaders();
	initMenus();
    initSounds();
    mapWalls = TileMap::createTileMap("levels/sacredwoods_walls.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
    mapPlatforms = TileMap::createTileMap("levels/sacredwoods_platforms.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
    mapBackground = TileMap::createTileMap("levels/sacredwoods_nocollisions.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	mapFrontal = TileMap::createTileMap("levels/sacredwoods_frontal.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
    mapSpikes = TileMap::createTileMap("levels/sacredwoods_spikes.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
    player = new Player();
    player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
    player->setAudioManager(&audioManager);
    player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * mapWalls->getTileSize(), INIT_PLAYER_Y_TILES * mapWalls->getTileSize()));
    player->setTileMap(mapWalls, mapPlatforms);

    hud = new HUD();
    hud->init(glm::ivec2(2, 2), texProgram);
    
    boss = nullptr;
    bossSpawned = false;

    pair<std::vector<int>, int> playerLifes = player->getplayerLifes();
    hud->syncWithPlayer(playerLifes.first, playerLifes.second);
    sticksCreated = false;

    hud = new HUD();
    hud->init(glm::ivec2(2, 2), texProgram);

    fallingSticks.clear();
    stickPositionsX.clear();

    orcoSpawnPositionsX = { 275.0f, 475.0f, 750.0f };
    orcos.clear();
    spawnPointToOrco.clear();
    // En Scene::init()
    orcoSpawnStates.clear();
    orcoSpawnStates.resize(orcoSpawnPositionsX.size(), SPAWN_AVAILABLE);


    float range1Size = STICK_RANGE1_MAX - STICK_RANGE1_MIN;
    float range2Size = STICK_RANGE2_MAX - STICK_RANGE2_MIN;
    float totalRangeSize = range1Size + range2Size;

    int sticksInRange1 = static_cast<int>(NUM_STICKS * (range1Size / totalRangeSize));
    int sticksInRange2 = NUM_STICKS - sticksInRange1;

    // Generar posiciones X para el primer rango
    if (sticksInRange1 > 0) {
        float interval1 = range1Size / (sticksInRange1 - 1);
        for (int i = 0; i < sticksInRange1; ++i) {
            float randomOffset = (rand() % 40) - 20.0f; // Variación de ±20 píxeles
            float xPos = STICK_RANGE1_MIN + i * interval1 + randomOffset;
            stickPositionsX.push_back(xPos);
        }
    }

    // Generar posiciones X para el segundo rango
    if (sticksInRange2 > 0) {
        float interval2 = range2Size / (sticksInRange2 - 1);
        for (int i = 0; i < sticksInRange2; ++i) {
            float randomOffset = (rand() % 40) - 20.0f; // Variación de ±20 píxeles
            float xPos = STICK_RANGE2_MIN + i * interval2 + randomOffset;
            stickPositionsX.push_back(xPos);
        }
    }

    // Inicializar los falling sticks con posiciones iniciales aleatorias en Y
    for (int i = 0; i < stickPositionsX.size(); ++i) {
        FallingStick* stick = new FallingStick();
        stick->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);

        // Distribuir verticalmente para que no todos empiecen al mismo tiempo
        float initialY = -300.0f + rand() % 600; // Más distribución vertical
        stick->setPosition(glm::vec2(stickPositionsX[i], initialY));
        stick->setTileMap(mapWalls, mapPlatforms);

        // Iniciar directamente en modo falling (para evitar esperas)
        stick->startFalling();

        fallingSticks.push_back(stick);
    }


    // Inicializar las plataformas m�viles
    // Plataforma 1
    MovingPlatform* platform1 = new MovingPlatform();
    platform1->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
    platform1->setPosition(glm::vec2(3175, 736));
    platform1->setMovementLimits(656, 768);
    platform1->setSpeed(6.f);
    movingPlatforms.push_back(platform1);

    // Plataforma 2
    MovingPlatform* platform2 = new MovingPlatform();
    platform2->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
    platform2->setPosition(glm::vec2(3143, 784));
    platform2->setMovementLimits(736, 816);
    platform2->setSpeed(5.5f);
    movingPlatforms.push_back(platform2);

    // Plataforma 3
    MovingPlatform* platform3 = new MovingPlatform();
    platform3->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
    platform3->setPosition(glm::vec2(3207, 592));
    platform3->setMovementLimits(576, 688);
    platform3->setSpeed(5.f);
    movingPlatforms.push_back(platform3);

    // Plataforma 4
    MovingPlatform* platform4 = new MovingPlatform();
    platform4->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
    platform4->setPosition(glm::vec2(3232, 816));
    platform4->setMovementLimits(720, 816);
    platform4->setSpeed(5.f);
    movingPlatforms.push_back(platform4);

    // Plataforma 5
    MovingPlatform* platform5 = new MovingPlatform();
    platform5->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
    platform5->setPosition(glm::vec2(3143, 576));
    platform5->setMovementLimits(576, 672);
    platform5->setSpeed(5.5f);
    movingPlatforms.push_back(platform5);

    player->setMovingPlatforms(&movingPlatforms);
    projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
    currentTime = 0.0f;

}

void Scene::initShaders()
{
    Shader vShader, fShader;

    vShader.initFromFile(VERTEX_SHADER, "shaders/texture.vert");
    if (!vShader.isCompiled())
    {
        cout << "Vertex Shader Error" << endl;
        cout << "" << vShader.log() << endl << endl;
    }
    fShader.initFromFile(FRAGMENT_SHADER, "shaders/texture.frag");
    if (!fShader.isCompiled())
    {
        cout << "Fragment Shader Error" << endl;
        cout << "" << fShader.log() << endl << endl;
    }
    texProgram.init();
    texProgram.addShader(vShader);
    texProgram.addShader(fShader);
    texProgram.link();
    if (!texProgram.isLinked())
    {
        cout << "Shader Linking Error" << endl;
        cout << "" << texProgram.log() << endl << endl;
    }
    texProgram.bindFragmentOutput("outColor");
    vShader.free();
    fShader.free();
}

void Scene::initMenus()
{
    // Crear los sprites para cada men�
    glm::vec2 quadSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glm::vec2 sizeInSpritesheet(1.0f, 1.0f);

    // Texturas para cargar las im�genes (se crean din�micamente)
    Texture* texStart = new Texture();
    Texture* texSelControls = new Texture();
    Texture* texSelCredits = new Texture();
    Texture* texControls = new Texture();
    Texture* texCredits = new Texture();
    Texture* texGameOver = new Texture();
    Texture* texVictory = new Texture();

    // Cargar las texturas del men� principal
    texStart->loadFromFile("images/start.png", TEXTURE_PIXEL_FORMAT_RGBA);
    texSelControls->loadFromFile("images/selected_controls.png", TEXTURE_PIXEL_FORMAT_RGBA);
    texSelCredits->loadFromFile("images/selected_credits.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Cargar las texturas del men� principal
    texStart->loadFromFile("images/main_start.png", TEXTURE_PIXEL_FORMAT_RGBA);
    texSelControls->loadFromFile("images/main_controls.png", TEXTURE_PIXEL_FORMAT_RGBA);
    texSelCredits->loadFromFile("images/main_credits.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Cargar las texturas de instrucciones y cr�ditos
    texControls->loadFromFile("images/controls.png", TEXTURE_PIXEL_FORMAT_RGBA);
    texCredits->loadFromFile("images/credits.png", TEXTURE_PIXEL_FORMAT_RGBA);

    texGameOver->loadFromFile("images/gameover_screen.png", TEXTURE_PIXEL_FORMAT_RGBA);
    texVictory->loadFromFile("images/win_screen.png", TEXTURE_PIXEL_FORMAT_RGBA);


    // Crear los sprites del men� principal con las texturas
    mainMenuSprites[OPTION_START_GAME] = Sprite::createSprite(quadSize, sizeInSpritesheet, texStart, &texProgram);
    mainMenuSprites[OPTION_START_GAME]->setPosition(glm::vec2(0.f, 0.f));

    mainMenuSprites[OPTION_INSTRUCTIONS] = Sprite::createSprite(quadSize, sizeInSpritesheet, texSelControls, &texProgram);
    mainMenuSprites[OPTION_INSTRUCTIONS]->setPosition(glm::vec2(0.f, 0.f));

    mainMenuSprites[OPTION_CREDITS] = Sprite::createSprite(quadSize, sizeInSpritesheet, texSelCredits, &texProgram);
    mainMenuSprites[OPTION_CREDITS]->setPosition(glm::vec2(0.f, 0.f));

    // Crear los sprites para las pantallas de instrucciones y cr�ditos
    instructionsSprite = Sprite::createSprite(quadSize, sizeInSpritesheet, texControls, &texProgram);
    instructionsSprite->setPosition(glm::vec2(0.f, 0.f));

    creditsSprite = Sprite::createSprite(quadSize, sizeInSpritesheet, texCredits, &texProgram);
    creditsSprite->setPosition(glm::vec2(0.f, 0.f));

    // Crear los sprites para game over y victoria
    gameOverSprite = Sprite::createSprite(quadSize, sizeInSpritesheet, texGameOver, &texProgram);
    gameOverSprite->setPosition(glm::vec2(0.f, 0.f));

    victorySprite = Sprite::createSprite(quadSize, sizeInSpritesheet, texVictory, &texProgram);
    victorySprite->setPosition(glm::vec2(0.f, 0.f));
}

void Scene::initSounds() {
    audioManager.init();

    // Cargar efectos de sonido
    audioManager.loadSound("jump", "sounds/jump1.mp3");
	audioManager.loadSound("player_hit", "sounds/player_hit.mp3");
	audioManager.loadSound("power-up", "sounds/collect_power-up.mp3");
    audioManager.loadSound("spear", "sounds/spear.mp3");
    audioManager.loadSound("ice_totem", "sounds/ice_totem.mp3");
    audioManager.loadSound("menu_move", "sounds/menu_move.mp3");
    audioManager.loadSound("menu_select", "sounds/menu_select.mp3");

    // Nuevos sonidos para game over y victoria
    audioManager.loadSound("game_over", "sounds/game_over.mp3");
    audioManager.loadSound("victory", "sounds/victory.mp3");
	audioManager.loadSound("boss_death", "sounds/boss_death.mp3");
}

void Scene::resetGame()
{
    // Reiniciar estado del juego
    gameState = MENU_MAIN;
    currentOption = OPTION_START_GAME;
    delete boss;
	boss = nullptr;
    if (hud != nullptr) {
        hud->showBossHealthBar(false);
    }
    bossDefeated = false;
    bossCam = false;
	bossSpawned = false;
    menuTime = 0.0f;
    currentTime = 0.0f;

	audioManager.stopAllSounds();

    // Limpiar entidades del juego
    // Limpiar orcos
    for (auto orco : orcos) {
        delete orco;
    }
    orcos.clear();
    spawnPointToOrco.clear();
    orcoSpawnStates.clear();
    orcoSpawnStates.resize(orcoSpawnPositionsX.size(), SPAWN_AVAILABLE);

    // Limpiar serpientes
    for (auto snake : snakes) {
        delete snake;
    }
    snakes.clear();
    snakeSpawnTimer = 0.0f;

    // Limpiar power-ups
    for (auto powerUp : powerUps) {
        delete powerUp;
    }
    powerUps.clear();

    // Reiniciar sticks
    for (auto stick : fallingSticks) {
        delete stick;
    }
    fallingSticks.clear();
    stickPositionsX.clear();

    sticksCreated = false;
    // Generar nuevas posiciones X para los palos
    float range1Size = STICK_RANGE1_MAX - STICK_RANGE1_MIN;
    float range2Size = STICK_RANGE2_MAX - STICK_RANGE2_MIN;
    float totalRangeSize = range1Size + range2Size;

    int sticksInRange1 = static_cast<int>(NUM_STICKS * (range1Size / totalRangeSize));
    int sticksInRange2 = NUM_STICKS - sticksInRange1;

    // Generar posiciones X para el primer rango
    if (sticksInRange1 > 0) {
        float interval1 = range1Size / (sticksInRange1 - 1);
        for (int i = 0; i < sticksInRange1; ++i) {
            float randomOffset = (rand() % 40) - 20.0f; // Variación de ±20 píxeles
            float xPos = STICK_RANGE1_MIN + i * interval1 + randomOffset;
            stickPositionsX.push_back(xPos);
        }
    }

    // Generar posiciones X para el segundo rango
    if (sticksInRange2 > 0) {
        float interval2 = range2Size / (sticksInRange2 - 1);
        for (int i = 0; i < sticksInRange2; ++i) {
            float randomOffset = (rand() % 40) - 20.0f; // Variación de ±20 píxeles
            float xPos = STICK_RANGE2_MIN + i * interval2 + randomOffset;
            stickPositionsX.push_back(xPos);
        }
    }

    // Inicializar los falling sticks con posiciones iniciales aleatorias en Y
    for (int i = 0; i < stickPositionsX.size(); ++i) {
        FallingStick* stick = new FallingStick();
        stick->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);

        // Distribuir verticalmente para que no todos empiecen al mismo tiempo
        float initialY = -300.0f + rand() % 600; // Más distribución vertical
        stick->setPosition(glm::vec2(stickPositionsX[i], initialY));
        stick->setTileMap(mapWalls, mapPlatforms);

        // Iniciar directamente en modo falling (para evitar esperas)
        stick->startFalling();

        fallingSticks.push_back(stick);
    }

    player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);

    player->setAudioManager(&audioManager);
    player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * mapWalls->getTileSize(), INIT_PLAYER_Y_TILES * mapWalls->getTileSize()));
    player->setTileMap(mapWalls, mapPlatforms);
    player->setMovingPlatforms(&movingPlatforms);

    // Reiniciar variables de cámara
    followHorizontal = true;
    currentCheckpoint = 0;
    isAnimating = false;
    bossCam = false;
    animationProgress = 0.0f;

    // Reiniciar HUD
    pair<std::vector<int>, int> playerLifes = player->getplayerLifes();
    hud->syncWithPlayer(playerLifes.first, playerLifes.second);

    // Reiniciar proyección a la posición inicial
    projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
}

void Scene::update(int deltaTime)
{
    audioManager.update();
    switch (gameState)
    {
    case MENU_MAIN:
    case MENU_INSTRUCTIONS:
    case MENU_CREDITS:
    case GAME_OVER:
    case VICTORY:
        updateMenu(deltaTime);
        break;
    case GAMEPLAY:
        updateGameplay(deltaTime);
        break;
    }
}

void Scene::handleMenuInput()
{
    if (keyPressed)
        return;

    switch (gameState)
    {
    case MENU_MAIN:
        // Navegación entre opciones
        if (Game::instance().getKey(GLFW_KEY_DOWN))
        {
            currentOption = static_cast<MainMenuOption>((currentOption + 1) % 3); // 3 opciones en total
            keyPressed = true;
            audioManager.playSound("menu_move", 0.5f);
        }
        else if (Game::instance().getKey(GLFW_KEY_UP))
        {
            currentOption = static_cast<MainMenuOption>((currentOption + 2) % 3); // +2 para dar la vuelta correctamente
            keyPressed = true;
            audioManager.playSound("menu_move", 0.5f);
        }

        // Selección de opción
        else if (Game::instance().getKey(GLFW_KEY_ENTER) || Game::instance().getKey(GLFW_KEY_SPACE))
        {
            keyPressed = true;
            audioManager.playSound("menu_select", 0.6f);
            switch (currentOption)
            {
            case OPTION_START_GAME:
                gameState = GAMEPLAY;
                audioManager.playMusic("sounds/sacredwoods_music.mp3", true, 0.4f);
                break;
            case OPTION_INSTRUCTIONS:
                gameState = MENU_INSTRUCTIONS;
                break;
            case OPTION_CREDITS:
                gameState = MENU_CREDITS;
                break;
            }
        }
        break;

    case MENU_INSTRUCTIONS:
    case MENU_CREDITS:
        // Volver al menú principal con cualquier tecla
        if (Game::instance().getKey(GLFW_KEY_ENTER) ||
            Game::instance().getKey(GLFW_KEY_ESCAPE) ||
            Game::instance().getKey(GLFW_KEY_SPACE))
        {
            gameState = MENU_MAIN;
            keyPressed = true;
        }
        break;

    case GAME_OVER:
    case VICTORY:
        // Manejar la lógica de fin de juego (antes en handleGameEndInput)
        if (Game::instance().getKey(GLFW_KEY_ENTER) ||
            Game::instance().getKey(GLFW_KEY_ESCAPE) ||
            Game::instance().getKey(GLFW_KEY_SPACE))
        {
            gameState = MENU_MAIN;
            keyPressed = true;
            // Reiniciar el juego
            resetGame();
        }
        break;
    }
}

void Scene::checkGameEndConditions()
{
    // Verificar condición de game over
    if (player->isGameOver())
    {
        gameState = GAME_OVER;
        audioManager.stopAllSounds();
        audioManager.stopMusic();
        audioManager.playSound("game_over", 0.6f);
    }

    // Verificar condición de victoria (cuando el boss es derrotado)
    if (bossDefeated)
    {
        gameState = VICTORY;
        audioManager.stopAllSounds();
        audioManager.stopMusic();
        audioManager.playSound("victory", 0.6f);
    }
}

void Scene::updateMenu(int deltaTime)
{
    menuTime += deltaTime;

    // Manejar la entrada de teclado para el men�
    handleMenuInput();

    // Manejar el tiempo de espera entre pulsaciones de teclas
    if (keyPressed)
    {
        keyPressedTimer += deltaTime;
        if (keyPressedTimer > 200) // 200ms de espera
        {
            keyPressed = false;
            keyPressedTimer = 0;
        }
    }
}

// Añade esta función antes de updateGameplay
bool Scene::checkSpikeCollision()
{
    glm::ivec2 posPlayer = player->getPosition();
    int hitboxWidth = 16;
    int hitboxHeight = 32;
    int hitboxOffsetX = (32 - hitboxWidth) / 2;

    // Obtenemos la posición de los pies del jugador
    glm::ivec2 feetPos = glm::ivec2(posPlayer.x + hitboxOffsetX, posPlayer.y + hitboxHeight);

    // Para comprobar si hay pinchos debajo, verificamos una línea muy delgada
    // justo debajo de los pies del jugador
    glm::ivec2 checkSize = glm::ivec2(hitboxWidth, 1);

    // Usamos una variable temporal para la posición Y que no nos importa actualizar
    int tempPosY = feetPos.y;

    // Verificamos si hay una colisión con un tile de pinchos
    return mapSpikes->collisionMoveDown(feetPos, checkSize, &tempPosY);
}

void Scene::updateGameplay(int deltaTime)
{
    currentTime += deltaTime;
    pair<std::vector<int>, int> prevLifeState = player->getplayerLifes();
    player->update(deltaTime);
    pair<std::vector<int>, int> currentLifeState = player->getplayerLifes();

    glm::ivec2 posPlayer = player->getPosition();

	cout << "Posicion Jugador: " << posPlayer.x << ", " << posPlayer.y << endl;

    // Comprobar si el jugador está sobre pinchos
    if (checkSpikeCollision() && !player->isInvulnerable()) {
        player->isHitted(); // Aplica daño al jugador
    }

    // Actualizar las plataformas m�viles
    for (auto platform : movingPlatforms) {
        platform->update(deltaTime);
    }
    static bool largeHeartSpawned = false; // Bandera para asegurarnos de que solo se genere una vez
    if (!largeHeartSpawned && posPlayer.x >= 3713) {
        glm::vec2 powerUpPosition(posPlayer.x - 8, posPlayer.y -5 ); // Posición del power-up en la misma Y que el jugador
        spawnPowerUp(powerUpPosition, LARGE_HEART); // Generar el power-up
        largeHeartSpawned = true; // Marcar como generado
    }

    static const float specialStickX = 3650.0f;
    static bool specialStickCreated = false;
    static int specialStickIndex = -1;

    // Calcular los límites de la cámara
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camRight = camX + CAMERA_WIDTH;

    // Verificar si la posición x=3650 es visible en la cámara
    bool isSpecialStickVisible = (specialStickX >= camX && specialStickX <= camRight);

    if (isSpecialStickVisible && !specialStickCreated) {
        // Si la posición es visible y el stick aún no se ha creado, crearlo
        FallingStick* stick = new FallingStick();
        stick->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);

        // Posicionar el stick en la parte superior de la cámara
        float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;
        stick->setPosition(glm::vec2(specialStickX, camY - 60)); // Un poco arriba de la cámara

        stick->setTileMap(mapWalls, mapPlatforms);
        stick->startFalling();

        fallingSticks.push_back(stick);
        stickPositionsX.push_back(specialStickX);

        // Guardar el índice de este stick especial
        specialStickIndex = fallingSticks.size() - 1;
        specialStickCreated = true;
    }
    else if (!isSpecialStickVisible && specialStickCreated) {
        // Si la posición ya no es visible y el stick se había creado, eliminarlo
        if (specialStickIndex >= 0 && specialStickIndex < fallingSticks.size()) {
            delete fallingSticks[specialStickIndex];
            fallingSticks.erase(fallingSticks.begin() + specialStickIndex);
            stickPositionsX.erase(stickPositionsX.begin() + specialStickIndex);
        }

        // Resetear las variables
        specialStickCreated = false;
        specialStickIndex = -1;
    }

    // Gestionar los sticks de la zona especial
    if (posPlayer.x >= 3100 && posPlayer.x <= 3300 &&
        posPlayer.y >= 940 && posPlayer.y <= 1440) {

        // Solo crear sticks si la bandera está en false
        if (!sticksCreated) {
            // Crear exactamente dos falling sticks en posiciones X fijas
            float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;

            // Posiciones X fijas para los sticks
            float stick1X = 3150.0f;
            float stick2X = 3230.0f;

            // Stick 1: posición X fija
            FallingStick* stick1 = new FallingStick();
            stick1->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
            stick1->setPosition(glm::vec2(stick1X, camY - 50));
            stick1->setTileMap(mapWalls, mapPlatforms);
            stick1->startFalling();
            fallingSticks.push_back(stick1);
            stickPositionsX.push_back(stick1X);

            // Stick 2: posición X fija
            FallingStick* stick2 = new FallingStick();
            stick2->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
            stick2->setPosition(glm::vec2(stick2X, camY - 80));
            stick2->setTileMap(mapWalls, mapPlatforms);
            stick2->startFalling();
            fallingSticks.push_back(stick2);
            stickPositionsX.push_back(stick2X);

            // Guardar los índices de estos sticks especiales para poder identificarlos después
            specialStickIndices.clear();
            specialStickIndices.push_back(fallingSticks.size() - 2); // Índice del primer stick especial
            specialStickIndices.push_back(fallingSticks.size() - 1); // Índice del segundo stick especial

            // Marcar que los sticks ya han sido creados
            sticksCreated = true;
        }
    }
    else {
        // Si el jugador está fuera de la zona completa y los sticks fueron creados
        if (sticksCreated) {
            // Si el jugador sale de la zona y los sticks fueron creados, eliminarlos
            if (!specialStickIndices.empty()) {
                // Eliminar los sticks especiales en orden inverso para no afectar los índices
                for (int i = specialStickIndices.size() - 1; i >= 0; --i) {
                    int index = specialStickIndices[i];

                    // Verificar que el índice sea válido
                    if (index >= 0 && index < fallingSticks.size()) {
                        // Eliminar el stick
                        delete fallingSticks[index];
                        fallingSticks.erase(fallingSticks.begin() + index);
                        stickPositionsX.erase(stickPositionsX.begin() + index);
                    }
                }

                // Limpiar la lista de índices
                specialStickIndices.clear();
            }

            // Reiniciar la bandera
            sticksCreated = false;
        }
    }


    updateSnakes(deltaTime);
    updateFallingSticks(deltaTime);
    updateSpecialOrcos(deltaTime);
    updateOrcos(deltaTime);
    updatePowerUps(deltaTime);
   

    // Si el jugador recibió daño, actualizar el HUD inmediatamente
        hud->syncWithPlayer(currentLifeState.first, currentLifeState.second);
    
        hud->updatePowerUpIcons(
            player->getFlintSpearHits(),
            player->getBuffaloHelmetHits(),
            player->hasDeerskinShirt()
        );

        hud->updateWeaponIcon(player->getCurrentWeapon());

    // Verificar si el jugador ha alcanzado el punto de control actual
    if (!isAnimating && currentCheckpoint < checkpoints.size() && posPlayer.x >= checkpoints[currentCheckpoint])
    {
        // Iniciar la animaci�n de desplazamiento a la derecha
        isAnimating = true;
        animationProgress = 0.0f;
        initialCamX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f; // Guardar la posici�n inicial de la c�mara
        player->setLeftLimit(checkpoints[currentCheckpoint]);
        currentCheckpoint++;
        followHorizontal = !followHorizontal; // Invertir la direcci�n de seguimiento
        if (currentCheckpoint == 5) {
            bossCam = true;
			audioManager.playMusic("sounds/boss_music.mp3", true, 0.4f);
        }
    }

    updateBoss(deltaTime);
    // Ejecutar la animaci�n de desplazamiento a la derecha
    if (isAnimating)
    {
        animationProgress += deltaTime * 0.002f; // Ajusta la velocidad de la animaci�n
        if (animationProgress >= 1.0f)
        {
            animationProgress = 1.0f;
            isAnimating = false;
        }
    }

    // Calcular las coordenadas de la c�mara
    camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;

    if (!bossCam) {
        // Ajustar la l�gica de la c�mara seg�n la variable followHorizontal
        if (followHorizontal)
        {
            if (currentCheckpoint == 0) camY = 16.f; // Mantener la posici�n de la c�mara en el eje y constante
            else if (currentCheckpoint == 2) camY = 1456.f;
            else if (currentCheckpoint == 4) camY = 496.f;
            if (camX < cameraLimits[currentCheckpoint].first)
            {
                camX = cameraLimits[currentCheckpoint].first;
            }
            else if (camX + CAMERA_WIDTH > cameraLimits[currentCheckpoint].second)
            {
                camX = cameraLimits[currentCheckpoint].second - CAMERA_WIDTH;
            }
        }
        else
        {
            camX = checkpoints[currentCheckpoint - 1] + 32.f;
            if (camY < cameraLimits[currentCheckpoint].first)
            {
                camY = cameraLimits[currentCheckpoint].first;
            }
            else if (camY + CAMERA_HEIGHT > cameraLimits[currentCheckpoint].second)
            {
                camY = cameraLimits[currentCheckpoint].second - CAMERA_HEIGHT;
            }
        }

    }
    else {
        camX = checkpoints[currentCheckpoint - 1] + 32.f;
        camY = 496.f;
    }

    // Ajustar la posici�n de la c�mara durante la animaci�n
    if (isAnimating)
    {
        camX = glm::mix(initialCamX, checkpoints[currentCheckpoint - 1] + 32.f, animationProgress);
    }

    // Ajustar la matriz de proyecci�n para centrar la c�mara en posPlayer
    projection = glm::ortho(camX, camX + CAMERA_WIDTH, camY + CAMERA_HEIGHT, camY);

    if (hud != nullptr) {
        hud->updatePosition(camX, camY);
    }
    

    hud->update(deltaTime);
    checkGameEndConditions();
}

void Scene::updateOrcos(int deltaTime)
{
    // Calcular los límites de la cámara
    glm::ivec2 posPlayer = player->getPosition();
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;
    float camRight = camX + CAMERA_WIDTH;
    float camBottom = camY + CAMERA_HEIGHT;

    // Actualizar estado de los puntos de spawn basado en visibilidad
    for (int i = 0; i < orcoSpawnPositionsX.size(); ++i) {
        float spawnX = orcoSpawnPositionsX[i];
        bool isSpawnVisible = (spawnX >= camX && spawnX <= camRight);

        // Si el punto es visible
        if (isSpawnVisible) {
            // Si el punto estaba fuera de pantalla y ahora es visible de nuevo
            if (orcoSpawnStates[i] == SPAWN_LEFT_SCREEN) {
                // Verificar si hay un orco asociado a este punto
                auto it = spawnPointToOrco.find(i);

                // Si no hay orco asociado o el orco ya no existe/está muerto
                if (it == spawnPointToOrco.end() || it->second == nullptr || !it->second->isAlive() || orcoSpawnStates[i] == SPAWN_DEAD) {
                    orcoSpawnStates[i] = SPAWN_AVAILABLE; // Punto disponible para nuevo orco
                    spawnPointToOrco.erase(i); // Eliminar la referencia al orco anterior

                }
                else {
                    // Si el orco sigue existiendo, mantener como activo
                    orcoSpawnStates[i] = SPAWN_ACTIVE;
                }
            }

            // Si el punto está disponible, generar un nuevo orco
            if (orcoSpawnStates[i] == SPAWN_AVAILABLE) {
                // Verificar que no haya un orco ya asociado a este punto
                if (spawnPointToOrco.find(i) == spawnPointToOrco.end()) {
                    // Crear nuevo orco
                    Orco* newOrco = new Orco();

                    // Determinar dirección inicial según la posición del jugador
                    Orco::Direction dir = (posPlayer.x < spawnX) ? Orco::LEFT : Orco::RIGHT;
                    newOrco->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, dir);

                    // Posicionar orco
                    newOrco->setPosition(glm::vec2(spawnX, camY - 32));

                    // Configuraciones
                    newOrco->setTileMap(mapWalls, mapPlatforms);
                    newOrco->setMovingPlatforms(&movingPlatforms);
                    newOrco->setPlayerPosition(&posPlayer);

                    // Configurar callback para daño al jugador
                    newOrco->setPlayerHitCallback([this]() {
                        if (!player->isInvulnerable()) {
                            player->isHitted();
                        }
                        });

                    // Asociar orco al punto de spawn en el mapa
                    spawnPointToOrco[i] = newOrco;
                    orcos.push_back(newOrco);
                    orcoSpawnStates[i] = SPAWN_ACTIVE;

                }
            }
        }
        else { // Si el punto no es visible
                orcoSpawnStates[i] = SPAWN_LEFT_SCREEN;

            
        }
    }

    // Actualizar todos los orcos existentes
    for (size_t i = 0; i < orcos.size(); ++i) {
        Orco* orco = orcos[i];

        // Actualizar orco
        orco->update(deltaTime);
        orco->setPlayerPosition(&posPlayer);

        glm::ivec2 orcoPos = orco->getPosition();
        glm::ivec2 orcoSize = orco->getSize();

        // Verificar colisión con la lanza del jugador
        if (player->checkSpearCollision(orcoPos, orcoSize)) {
            // Si es el totem de hielo, congelar al orco en vez de dañarlo
            if (player->getCurrentWeapon() == ICE_TOTEM) {
                orco->freeze();
            }
            else {
                if (player->getFlintSpearHits() > 0) {
                    orco->hitWithFlint();  // Usar el nuevo método
                }
                else {
                    orco->hit();  // Daño normal
                }
                player->decrementFlintSpearHits();
            }
        }

        // Verificar si el orco está dentro del rango visible
        bool isOrcoVisible = (orcoPos.x + orcoSize.x >= camX - 100 &&
            orcoPos.x <= camRight + 100 &&
            orcoPos.y + orcoSize.y >= camY - 200 &&
            orcoPos.y <= camBottom + 200);

        if (!orco->isAlive()) {
            
            // Generar power-up con probabilidad
            int randomVal = rand() % 100;
            if (randomVal < 80) {
                // Seleccionar un tipo de power-up aleatorio
                PowerUpType type = getRandomPowerUpType();
                // Generar el power-up ligeramente por encima del orco para evitar que se quede atascado en el suelo
                spawnPowerUp(glm::vec2(orcoPos.x, orcoPos.y), type);
            }
        }

        // Si el orco está muerto o fuera de pantalla, eliminarlo
        if (!orco->isAlive() || !isOrcoVisible) {
            // Buscar el punto de spawn asociado
            for (auto it = spawnPointToOrco.begin(); it != spawnPointToOrco.end(); ++it) {
                if (it->second == orco) {
                    int spawnIndex = it->first;

                    // Si el orco murió, marcar punto como muerto
                    if (!orco->isAlive()) {
                        orcoSpawnStates[spawnIndex] = SPAWN_DEAD;

                    }

                    // Eliminar la referencia al orco en el mapa
                    // pero no del vector aún (se hará más abajo)
                    it->second = nullptr;
                    break;
                }
            }

            // Eliminar el orco
            delete orco;
            orcos.erase(orcos.begin() + i);
            --i; // Ajustar índice
        }
    }

    // Limpiar referencias a orcos nulos en el mapa
    for (auto it = spawnPointToOrco.begin(); it != spawnPointToOrco.end();) {
        if (it->second == nullptr) {
            it = spawnPointToOrco.erase(it);
        }
        else {
            ++it;
        }
    }
	
}



void Scene::updatePowerUps(int deltaTime)
{
    glm::ivec2 posPlayer = player->getPosition();
    glm::ivec2 playerSize(16, 32); // Tamaño del hitbox del jugador

    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;
    float camRight = camX + CAMERA_WIDTH;
    float camBottom = camY + CAMERA_HEIGHT;

    // Actualizar cada power-up
    for (int  i = 0; i < powerUps.size(); ++i) {
        PowerUp* powerUp = powerUps[i];

        glm::ivec2 powerUpPos = powerUp->getPosition();
        glm::ivec2 powerUpSize = powerUp->getSize();

        // Comprobar si está fuera de pantalla
        bool isVisible = (powerUpPos.x + powerUpSize.x >= camX &&
            powerUpPos.x <= camRight &&
            powerUpPos.y + powerUpSize.y >= camY &&
            powerUpPos.y <= camBottom);

        // Si no es visible, desactivarlo
        if (!isVisible) {
            // Solo desactivamos los corazones pequeños fuera de pantalla
            // ya que suben y pueden salir rápidamente
            powerUp->deactivate();
        }

        // Actualizar el power-up
        powerUp->update(deltaTime);

		if (powerUp->getType() == TOTEM_BOSS && powerUp->collisionWithPlayer(posPlayer, playerSize)) {
			setBossDefeated();
			powerUp->deactivate();
            return;
		}

        // Comprobar colisión con el jugador
        if (powerUp->isActive() && powerUp->collisionWithPlayer(posPlayer, playerSize)) {
            // Aplicar el efecto del power-up según su tipo
            audioManager.playSound("power-up", 0.3f);
            switch (powerUp->getType()) {
            case SMALL_HEART:
                player->collectSmallHeart();
                break;
            case LARGE_HEART:
                player->collectLargeHeart();
                break;
            case GOURD:
                player->collectGourd();
				hud->collectGourd();
                break;
			case MAGIC_POTION:
				player->collectPotion();
				hud->collectPotion();
				break;
            case FLINT_SPEAR:
                player->collectFlintSpear();
                break;
            case BUFFALO_HELMET:
                player->collectBuffaloHelmet();
                break;
            case DEERSKIN_SHIRT:
                player->collectDeerskinShirt();
                break;
			case LARGE_SPEAR:
				if(!player->isSpearLargeActive())player->activateSpearLarge(true);
				break;
            }

            // Desactivar el power-up recogido
            powerUp->deactivate();

            // Actualizar el HUD con el nuevo estado del jugador
            pair<std::vector<int>, int> playerLifes = player->getplayerLifes();
            hud->syncWithPlayer(playerLifes.first, playerLifes.second);
			
            hud->updatePowerUpIcons(
                player->getFlintSpearHits(),
                player->getBuffaloHelmetHits(),
                player->hasDeerskinShirt()
            );
        }

        // Eliminar power-ups desactivados
        if (!powerUp->isActive()) {
            delete powerUp;
            powerUps.erase(powerUps.begin() + i);
            --i; // Ajustar el índice
        }
    }
}

// Añadir función spawnPowerUp
void Scene::spawnPowerUp(const glm::vec2& position, PowerUpType type)
{
    PowerUp* powerUp = new PowerUp(type);
    powerUp->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
    if (type == SMALL_HEART ) {
        powerUp->setFloatingType(true, true); // Flota hacia arriba
    }
    else {
        powerUp->setFloatingType(false, false); // Flota en el sitio
    }
    powerUp->setPosition(position);
    powerUp->setTileMap(mapWalls, mapPlatforms);
    powerUps.push_back(powerUp);
}


void Scene::updateSnakes(int deltaTime)
{
    glm::ivec2 posPlayer = player->getPosition();
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;

    // Verificar si el jugador está en el rango para generar serpientes
    bool inSnakeZone = (posPlayer.x >= SNAKE_MIN_X && posPlayer.x <= SNAKE_MAX_X);

    static bool lastSnakeWasLeft = false;

    // Generar nuevas serpientes solo si el jugador está en la zona
    if (inSnakeZone && snakes.size() < 2) {
        snakeSpawnTimer += deltaTime;

        if (snakeSpawnTimer >= SNAKE_SPAWN_INTERVAL) {
            snakeSpawnTimer = 0.0f;

            // Determinar dirección basada en la posición del jugador
            Snake::Direction snakeDir;
            float spawnX;

            // Si la última serpiente fue de izquierda, la próxima será de derecha y viceversa
            if (!lastSnakeWasLeft) {
                // Generar serpiente que viene desde la izquierda (dirección RIGHT)
                snakeDir = Snake::RIGHT;
                spawnX = camX - 32; // A la izquierda de la pantalla
                lastSnakeWasLeft = true;
            }
            else {
                // Generar serpiente que viene desde la derecha (dirección LEFT)
                snakeDir = Snake::LEFT;
                spawnX = camX + CAMERA_WIDTH + 32; // A la derecha de la pantalla
                lastSnakeWasLeft = false;
            }

            // Ajustar la altura para que esté cerca de la altura del jugador
            // pero no exactamente igual para agregar variedad
            float spawnY = posPlayer.y + 6;

            // Asegurarse de que no esté demasiado baja
            // Si el jugador está saltando, usar una altura base razonable
            if (spawnY > camY + CAMERA_HEIGHT - 80) {
                spawnY = camY + CAMERA_HEIGHT - 80;
            }

            // Crear la serpiente
            Snake* newSnake = new Snake();
            newSnake->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, snakeDir);
            newSnake->setPosition(glm::vec2(spawnX, spawnY));
            newSnake->setMovementLimits(camX - 20, camX + CAMERA_WIDTH - 10);
            newSnake->setTileMap(mapWalls, mapPlatforms);
            snakes.push_back(newSnake);
        }
    }
    else {
        // Si el jugador no está en la zona, reiniciar el temporizador
        snakeSpawnTimer = 0.0f;
    }

    // Actualizar todas las serpientes existentes
    for (size_t i = 0; i < snakes.size(); ++i) {
        Snake* snake = snakes[i];
        snake->update(deltaTime);

        glm::ivec2 snakePos = snake->getPosition();
        glm::ivec2 snakeSize = snake->getSize(); // Asumimos que la clase Snake tiene un método getSize()

        if (player->checkSpearCollision(snakePos, snakeSize)) {
            // Si es el totem de hielo, congelar la serpiente en vez de matarla
            if (player->getCurrentWeapon() == ICE_TOTEM) {
                snake->freeze();
            }
            else {
                // Eliminar la serpiente al ser golpeada por la lanza
                int randomVal = rand() % 100;
                if (randomVal < 90) {  // 35% de probabilidad (menor que los orcos)
                    PowerUpType type = getRandomPowerUpType();
                    spawnPowerUp(glm::vec2(snakePos.x, snakePos.y - 1), type);
                }
                delete snake;
                snakes.erase(snakes.begin() + i);
                player->decrementFlintSpearHits();
                --i; // Ajustar el índice después de eliminar
            }
        }

        // Comprobar colisiones con el jugador (si no fue eliminada)
        if (snake->collisionWithPlayer(posPlayer, glm::ivec2(16, 32))) {
            player->isHitted(); // El jugador recibe daño
        }

        // Eliminar serpientes que ya no son visibles en la cámara
        if (snakePos.x < camX - 64 || snakePos.x > camX + CAMERA_WIDTH + 64 ||
            snakePos.y < camY - 64 || snakePos.y > camY + CAMERA_HEIGHT + 64) {
            delete snake;
            snakes.erase(snakes.begin() + i);
            --i; // Ajustar el índice después de eliminar
        }
    }
}



void Scene::render()
{
    // Elegir qué renderizar según el estado actual
    if (gameState == GAMEPLAY) {
        renderGameplay();
    }
    else {
        renderMenu();
    }
}

void Scene::renderMenu()
{
    glm::mat4 modelview;

    texProgram.use();
    projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    modelview = glm::mat4(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    // Renderizar el sprite apropiado seg�n el estado actual
    switch (gameState)
    {
    case MENU_MAIN:
        if (mainMenuSprites[currentOption] != nullptr) {
            mainMenuSprites[currentOption]->render();
        }
        break;
    case MENU_INSTRUCTIONS:
        if (instructionsSprite != nullptr) {
            instructionsSprite->render();
        }
        break;
    case MENU_CREDITS:
        if (creditsSprite != nullptr) {
            creditsSprite->render();
        }
        break;
    case GAME_OVER:
        if (gameOverSprite != nullptr) {
            gameOverSprite->render();
        }
        break;
    case VICTORY:
        if (victorySprite != nullptr) {
            victorySprite->render();
        }
        break;
    }
}

void Scene::renderGameplay()
{

    glm::mat4 modelview;

    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    modelview = glm::mat4(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    mapBackground->render();
    mapWalls->render();
    mapPlatforms->render();


    // Renderizar las plataformas m�viles
    for (auto platform : movingPlatforms) {
        platform->render();
    }

    for (auto snake : snakes) {
        snake->render();
    }

    for (auto orco : orcos) {
        orco->render();
    }

    for (auto powerUp : powerUps) {
        powerUp->render();
    }


    glm::ivec2 posPlayer = player->getPosition();
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camRight = camX + CAMERA_WIDTH;

    for (int i = 0; i < fallingSticks.size(); ++i) {
        FallingStick* stick = fallingSticks[i];
        float stickX = stickPositionsX[i];

        // Solo renderizar si está dentro del rango visible
        if (stickX >= camX && stickX <= camRight) {
            stick->render();
        }
    }
    
    player->render();

    if (boss != nullptr) {
        boss->render();
    }


    //Reiniciar modelview y renderizar parte delantera
    modelview = glm::mat4(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    mapFrontal->render();
    hud->render();
}

int Scene::getCurrentCheckpoint()
{
    return currentCheckpoint;
}

float Scene::getCameraLimit()
{
    return cameraLimits[currentCheckpoint].first;
}

void Scene::setPlayerHealth( std::vector<int>& health)
{
    if (hud != nullptr)
        hud->setHealth(health);
}

void Scene::setPlayerLights(int lights)
{
    if (hud != nullptr)
        hud->setLights(lights);
}

void Scene::setBossDefeated()
{
    bossDefeated = true;
}

void Scene::updateFallingSticks(int deltaTime)
{
    glm::ivec2 posPlayer = player->getPosition();
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;
    float camRight = camX + CAMERA_WIDTH;
    float camBottom = camY + CAMERA_HEIGHT;

    if (fallingSticks.size() != stickPositionsX.size()) {
        return;
    }

    // Actualizar cada falling stick
    for (int i = 0; i < fallingSticks.size(); ++i) {
        FallingStick* stick = fallingSticks[i];
        float stickX = stickPositionsX[i];

        // Comprobar si el stick está dentro del rango visible de la cámara en X (extendido)
        bool isInViewX = (stickX >= camX - 100 && stickX <= camRight + 100);
        glm::ivec2 stickPos = stick->getPosition();

        // Comprobar si es un stick que ha sido bloqueado
        if (stick->isKilled()) {
            // Actualizar siempre los sticks bloqueados
            stick->update(deltaTime);

            // Si el stick bloqueado ha salido completamente de la pantalla por abajo,
            // lo marcamos para eliminación permanente
            if (stickPos.y > camBottom + 200) {
                // En lugar de reposicionarlo, lo dejamos en su estado KILLED
                // pero lo movemos muy lejos para que no interfiera
                stick->setPosition(glm::vec2(-1000, -1000));
            }

            continue; // Pasar al siguiente stick
        }

        // Si está en el rango horizontal visible de la cámara
        if (isInViewX) {
            // Si el stick ha salido completamente por la parte inferior de la pantalla,
            // reposicionarlo en la parte superior y hacerlo caer inmediatamente
            if (stickPos.y > camBottom + 100) {
                // Reposicionar arriba de la cámara con un poco de aleatoriedad
                float randomHeight = rand() % 100; // Variación pequeña
                stick->setPosition(glm::vec2(stickX, camY - stick->getSize().y - 20 - randomHeight));

                // Hacer que caiga inmediatamente (sin esperar)
                stick->startFalling();
            }
            else {
                // Actualizar el stick normalmente
                stick->update(deltaTime);

                // Comprobar colisión con el jugador
                if (stick->collisionWithPlayer(posPlayer, glm::ivec2(16, 32))) {
                    // Comprobar si el jugador está en modo protección
                    if (player->isProtecting()) {
                        // Calcular dirección del rebote basado en la posición relativa al jugador
                        float reboteDir = (stickX > posPlayer.x) ? 1.0f : -1.0f;
                        stick->killStick(reboteDir);
                    }
                    else {
                        // Guardar estado actual de vida antes del golpe
                        pair<std::vector<int>, int> beforeHit = player->getplayerLifes();

                        // El jugador recibe daño
                        player->isHitted();

                        // Obtener nuevo estado de vida después del golpe
                        pair<std::vector<int>, int> afterHit = player->getplayerLifes();

                        // Actualizar el HUD solo si las vidas cambiaron
                        if (beforeHit.first != afterHit.first || beforeHit.second != afterHit.second) {
                            hud->syncWithPlayer(afterHit.first, afterHit.second);
                        }
                    }
                }
            }
        }
        else {
            // Si está fuera de la vista pero cerca verticalmente, seguir actualizando
            if (stickPos.y > camY - 300 && stickPos.y < camBottom + 300) {
                stick->update(deltaTime);
            }
            // Si está muy por encima o debajo, reposicionarlo aleatoriamente
            else {
                // Posición aleatoria arriba de la cámara
                float randomHeight = 20 + rand() % 280; // Entre 20 y 300 píxeles arriba
                stick->setPosition(glm::vec2(stickX, camY - randomHeight));

                // Hacer que caiga inmediatamente
                stick->startFalling(); 
            }
        }
    }
}

void Scene::updateBoss(int deltaTime)
{
    glm::ivec2 posPlayer = player->getPosition();
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;
    float camRight = camX + CAMERA_WIDTH;
    float camBottom = camY + CAMERA_HEIGHT;

    // Si el boss no ha sido creado y estamos en la zona del boss
    if (!bossSpawned && bossCam) {
        boss = new Boss();
        boss->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);

        // Posicionar el boss a una altura similar a la del suelo donde está el jugador
        // pero un poco a la derecha para que sea visible
        float floorY = 770.0f; // Altura aproximada del suelo en esta zona
        boss->setPosition(glm::vec2(4000.f, posPlayer.y + 6.f)); // Restamos el tamaño del boss
        boss->setTileMap(mapWalls, mapPlatforms);
        boss->setPlayerPosition(&posPlayer);

        // Configurar callback para daño al jugador
        boss->setPlayerHitCallback([this]() {
            if (!player->isInvulnerable()) {
                player->isHitted();
            }
            });

        bossSpawned = true;
        if (hud != nullptr) {
            hud->showBossHealthBar(true);
            std::pair<std::vector<int>, int> bossLives = boss->getLives();
            hud->setBossHealth(bossLives.first);
        }
    }

    // Actualizar el boss si existe
    if (boss != nullptr) {
        // Debug info
        if (bossSpawned) {
            glm::ivec2 bossPos = boss->getPosition();
        }

        if (hud != nullptr) {
            std::pair<std::vector<int>, int> bossLives = boss->getLives();
            hud->setBossHealth(bossLives.first);
        }
        boss->update(deltaTime);
        boss->setPlayerPosition(&posPlayer);

        // Obtener los sticks controlados por el Boss y actualizarlos si hay colisión con el jugador
        std::vector<FallingStick*>& bossSticks = boss->getFallingSticks();
        for (FallingStick* stick : bossSticks) {
            // Verificar colisión con el jugador
            if (stick->collisionWithPlayer(posPlayer, glm::ivec2(32, 32))) {
                // Comprobar si el jugador está en modo protección
                if (player->isProtecting()) {
                    // Calcular dirección del rebote basado en la posición relativa al jugador
                    float reboteDir = (stick->getPosition().x > posPlayer.x) ? 1.0f : -1.0f;
                    stick->killStick(reboteDir);
                }
                else {
                    // El jugador recibe daño
                    player->isHitted();

                    // Actualizar el HUD
                    pair<std::vector<int>, int> playerLifes = player->getplayerLifes();
                    hud->syncWithPlayer(playerLifes.first, playerLifes.second);
                }
            }
        }

        // Comprobar si el jugador golpea al boss con la lanza
        glm::ivec2 bossPos = boss->getPosition();
        glm::ivec2 bossSize = boss->getSize();
        if (player->getCurrentWeapon() != ICE_TOTEM && player->checkSpearCollision(bossPos, bossSize)) {
            boss->hit();
            player->decrementFlintSpearHits();
            if (hud != nullptr) {
                std::pair<std::vector<int>, int> bossLives = boss->getLives();
                hud->setBossHealth(bossLives.first);
            }
        }

        // Si el boss muere, generar un power-up importante
        if (!boss->isAlive()) {
            // Generar power-up de tipo grande
            spawnPowerUp(glm::vec2(bossPos.x + 32, bossPos.y + 32), TOTEM_BOSS);

            // Eliminar el boss
            delete boss;
            boss = nullptr;

            // Cambiar la música de vuelta a la normal
            audioManager.stopMusic();
			audioManager.playSound("boss_death", 0.6f);

            if (hud != nullptr) {
                hud->showBossHealthBar(false);
            }
            
        }
       
    }
}

PowerUpType Scene::getRandomPowerUpType()
{
    // Array con todos los tipos posibles de power-ups
    PowerUpType types[] = {
        SMALL_HEART,     // Corazón pequeño - común
        SMALL_HEART,     // Repetido para aumentar su probabilidad
        GOURD,           // Calabaza - común
        GOURD,           // Repetido para aumentar su probabilidad
        MAGIC_POTION,    // Poción mágica - menos común
        FLINT_SPEAR,     // Lanza de pedernal - menos común
        BUFFALO_HELMET,  // Casco de búfalo - menos común
        DEERSKIN_SHIRT,  // Camisa de piel de ciervo - rara
        LARGE_HEART,      // Corazón grande - raro
        LARGE_SPEAR
    };

    // Seleccionar un índice aleatorio
    int index = rand() % (sizeof(types) / sizeof(types[0]));
    return types[index];
}

void Scene::updateSpecialOrcos(int deltaTime)
{
    glm::ivec2 posPlayer = player->getPosition();
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;
    float camRight = camX + CAMERA_WIDTH;
    float camBottom = camY + CAMERA_HEIGHT;

    // Definir las posiciones específicas de los orcos
    static const glm::vec2 orcoPos1(3224.0f, 896.0f);
    static const glm::vec2 orcoPos2(3214.0f, 1232.0f);
    static const glm::vec2 stickPos(3214.0f, camY - 50);  // La posición Y se ajustará en función de la cámara

    // Variables estáticas para rastrear si los orcos ya han sido creados
    static bool orco1Created = false;
    static bool orco2Created = false;
    static Orco* specialOrco1 = nullptr;
    static Orco* specialOrco2 = nullptr;
    static FallingStick* specialStick = nullptr;

    // Comprobar si la posición del orco 1 es visible
    bool isOrco1Visible = (orcoPos1.x >= camX && orcoPos1.x <= camRight &&
        orcoPos1.y >= camY && orcoPos1.y <= camBottom);

    // Comprobar si la posición del orco 2 es visible
    bool isOrco2Visible = (orcoPos2.x >= camX && orcoPos2.x <= camRight &&
        orcoPos2.y >= camY && orcoPos2.y <= camBottom);

    // Si el orco 1 es visible y no ha sido creado todavía
    if (isOrco1Visible && !orco1Created) {
        specialOrco1 = new Orco();
        Orco::Direction dir = (posPlayer.x < orcoPos1.x) ? Orco::LEFT : Orco::RIGHT;
        specialOrco1->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, dir);
        specialOrco1->setPosition(orcoPos1);
        specialOrco1->setTileMap(mapWalls, mapPlatforms);
        specialOrco1->setMovingPlatforms(&movingPlatforms);
        specialOrco1->setPlayerPosition(&posPlayer);

        // Configurar callback para daño al jugador
        specialOrco1->setPlayerHitCallback([this]() {
            if (!player->isInvulnerable()) {
                player->isHitted();
            }
            });

        orcos.push_back(specialOrco1);
        orco1Created = true;
    }

    // Si el orco 2 es visible y no ha sido creado todavía
    if (isOrco2Visible && !orco2Created) {
        specialOrco2 = new Orco();
        Orco::Direction dir = (posPlayer.x < orcoPos2.x) ? Orco::LEFT : Orco::RIGHT;
        specialOrco2->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, dir);
        specialOrco2->setPosition(orcoPos2);
        specialOrco2->setTileMap(mapWalls, mapPlatforms);
        specialOrco2->setMovingPlatforms(&movingPlatforms);
        specialOrco2->setPlayerPosition(&posPlayer);

        // Configurar callback para daño al jugador
        specialOrco2->setPlayerHitCallback([this]() {
            if (!player->isInvulnerable()) {
                player->isHitted();
            }
            });

        orcos.push_back(specialOrco2);
        orco2Created = true;

       
    }

    // Si los orcos ya fueron creados, verificar si siguen vivos
    // Si mueren, debemos resetear las variables para permitir que sean recreados la próxima vez
    if (orco1Created && (specialOrco1 == nullptr || !specialOrco1->isAlive())) {
        orco1Created = false;
    }

    if (orco2Created && (specialOrco2 == nullptr || !specialOrco2->isAlive())) {
        orco2Created = false;
    }
}