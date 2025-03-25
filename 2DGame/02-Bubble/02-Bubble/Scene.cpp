#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 1 /*1 123 131 188 205*/
#define INIT_PLAYER_Y_TILES 10 /*10 3 99 99 33*/

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

    for (int i = 0; i < 3; i++) {
        mainMenuSprites[i] = NULL;
    }
    instructionsSprite = NULL;
    creditsSprite = NULL;
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
    player = new Player();
    player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
    player->setAudioManager(&audioManager);
    player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * mapWalls->getTileSize(), INIT_PLAYER_Y_TILES * mapWalls->getTileSize()));
    player->setTileMap(mapWalls, mapPlatforms);

    hud = new HUD();
    hud->init(glm::ivec2(2, 2), texProgram);
    

    pair<glm::ivec4, int> playerLifes = player->getplayerLifes();
    hud->syncWithPlayer(playerLifes.first, playerLifes.second);

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
}

void Scene::initSounds() {
    audioManager.init();

    // Cargar efectos de sonido
    audioManager.loadSound("jump", "sounds/jump1.mp3");
    audioManager.loadSound("spear", "sounds/spear.mp3");
    audioManager.loadSound("menu_move", "sounds/menu_move.mp3");
    audioManager.loadSound("menu_select", "sounds/menu_select.mp3");
}

void Scene::update(int deltaTime)
{
    audioManager.update();
    switch (gameState)
    {
    case MENU_MAIN:
    case MENU_INSTRUCTIONS:
    case MENU_CREDITS:
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
        // Navegaci�n entre opciones
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

        // Selecci�n de opci�n
        else if (Game::instance().getKey(GLFW_KEY_ENTER) || Game::instance().getKey(GLFW_KEY_SPACE))
        {
            keyPressed = true;
            audioManager.playSound("menu_select", 0.6f);
            switch (currentOption)
            {
            case OPTION_START_GAME:
                gameState = GAMEPLAY;
				audioManager.playMusic("sounds/sacredwoods_music.mp3", true, 0.1f);
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
        // Volver al men� principal con cualquier tecla
        if (Game::instance().getKey(GLFW_KEY_ENTER) ||
            Game::instance().getKey(GLFW_KEY_ESCAPE) ||
            Game::instance().getKey(GLFW_KEY_SPACE))
        {
            gameState = MENU_MAIN;
            keyPressed = true;
        }
        break;
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

void Scene::updateGameplay(int deltaTime)
{
    currentTime += deltaTime;
    pair<glm::ivec4, int> prevLifeState = player->getplayerLifes();
    player->update(deltaTime);
    pair<glm::ivec4, int> currentLifeState = player->getplayerLifes();

    glm::ivec2 posPlayer = player->getPosition();

    
    // Actualizar las plataformas m�viles
    for (auto platform : movingPlatforms) {
        platform->update(deltaTime);
    }

    updateSnakes(deltaTime);

    

    // Si el jugador recibió daño, actualizar el HUD inmediatamente
        hud->syncWithPlayer(currentLifeState.first, currentLifeState.second);
    

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
			audioManager.playMusic("sounds/boss_music.mp3", true, 0.5f);
        }
    }

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
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
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
}

void Scene::updateSnakes(int deltaTime)
{
    glm::ivec2 posPlayer = player->getPosition();
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;

    // Verificar si el jugador está en el rango para generar serpientes
    bool inSnakeZone = (posPlayer.x >= SNAKE_MIN_X && posPlayer.x <= SNAKE_MAX_X);

    // Generar nuevas serpientes solo si el jugador está en la zona
    if (inSnakeZone && snakes.size() < 3) {
        snakeSpawnTimer += deltaTime;

        if (snakeSpawnTimer >= SNAKE_SPAWN_INTERVAL) {
            snakeSpawnTimer = 0.0f;

            // Determinar dirección basada en la posición del jugador
            Snake::Direction snakeDir = Snake::LEFT;
            if (posPlayer.x > (SNAKE_MIN_X + SNAKE_MAX_X) / 2) {
                snakeDir = Snake::RIGHT;
            }

            // Calcular posición de generación (fuera de pantalla pero cerca)
            float spawnX;
            if (snakeDir == Snake::LEFT) {
                spawnX = camX + CAMERA_WIDTH + 32; // A la derecha de la pantalla
            }
            else {
                spawnX = camX - 32; // A la izquierda de la pantalla
            }

            // Ajustar la altura para que esté cerca de la altura del jugador
            // pero no exactamente igual para agregar variedad
            float spawnY = posPlayer.y;

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

        // Comprobar si la lanza del jugador ha golpeado a la serpiente
        if (player->checkSpearCollision(snakePos, snakeSize)) {
            // Eliminar la serpiente al ser golpeada por la lanza
            delete snake;
            snakes.erase(snakes.begin() + i);
            --i; // Ajustar el índice después de eliminar
            continue; // Continuar con la siguiente serpiente
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
    glm::mat4 modelview;

    texProgram.use();
    texProgram.setUniformMatrix4f("projection", projection);
    texProgram.setUniform4f("color", 1.0f, 1.0f, 1.0f, 1.0f);
    modelview = glm::mat4(1.0f);
    texProgram.setUniformMatrix4f("modelview", modelview);
    texProgram.setUniform2f("texCoordDispl", 0.f, 0.f);

    // Elegir qu� renderizar seg�n el estado actual
    if (gameState == GAMEPLAY) {
        renderGameplay();
    }
    else {
        renderMenu();
    }
}

void Scene::renderMenu()
{

	texProgram.use();

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
    }
}

void Scene::renderGameplay()
{

    texProgram.use();

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
    
    player->render();



    //Reiniciar modelview y renderizar parte delantera
    glm::mat4 modelview = glm::mat4(1.0f);
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

void Scene::setPlayerHealth(const glm::ivec4& health)
{
    if (hud != nullptr)
        hud->setHealth(health);
}

void Scene::setPlayerLights(int lights)
{
    if (hud != nullptr)
        hud->setLights(lights);
}

