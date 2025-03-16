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
    currentCheckpoint = 0; // Inicializa el índice del punto de control actual
    isAnimating = false; // Inicializa la variable de animación
    bossCam = false;
    animationProgress = 0.0f; // Inicializa el progreso de la animación
}

Scene::~Scene()
{
    if (mapWalls != NULL)
        delete mapWalls;
    if (mapBackground != NULL)
        delete mapBackground;
    if (mapPlatforms != NULL)
        delete mapPlatforms;
    if (player != NULL)
        delete player;

    // Liberar la memoria de las plataformas móviles
    for (auto platform : movingPlatforms) {
        delete platform;
    }
    movingPlatforms.clear();
}

void Scene::init()
{
    initShaders();
    mapWalls = TileMap::createTileMap("levels/sacredwoods_walls.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
    mapPlatforms = TileMap::createTileMap("levels/sacredwoods_platforms.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
    mapBackground = TileMap::createTileMap("levels/sacredwoods_nocollisions.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
    player = new Player();
    player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
    player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * mapWalls->getTileSize(), INIT_PLAYER_Y_TILES * mapWalls->getTileSize()));
    player->setTileMap(mapWalls, mapPlatforms);

    // Inicializar las plataformas móviles
    // Plataforma 1
    MovingPlatform* platform1 = new MovingPlatform();
    platform1->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
    platform1->setPosition(glm::vec2(3175, 736));
    platform1->setMovementLimits(656, 768);
    platform1->setSpeed(5.5f);
    movingPlatforms.push_back(platform1);

    // Plataforma 2
    MovingPlatform* platform2 = new MovingPlatform();
    platform2->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
    platform2->setPosition(glm::vec2(3143, 784));
    platform2->setMovementLimits(736, 816);
    platform2->setSpeed(5.f);
    movingPlatforms.push_back(platform2);

    // Plataforma 3
    MovingPlatform* platform3 = new MovingPlatform();
    platform3->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
    platform3->setPosition(glm::vec2(3207, 592));
    platform3->setMovementLimits(576, 688);
    platform3->setSpeed(4.5f);
    movingPlatforms.push_back(platform3);

	// Plataforma 4
	MovingPlatform* platform4 = new MovingPlatform();
	platform4->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
	platform4->setPosition(glm::vec2(3232, 816));
	platform4->setMovementLimits(720, 816);
	platform4->setSpeed(4.5f);
	movingPlatforms.push_back(platform4);

	// Plataforma 5
	MovingPlatform* platform5 = new MovingPlatform();
	platform5->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram, "images/platform.png", glm::ivec2(25, 16));
	platform5->setPosition(glm::vec2(3143, 576));
	platform5->setMovementLimits(576, 672);
	platform5->setSpeed(5.0f);
	movingPlatforms.push_back(platform5);

    player->setMovingPlatforms(&movingPlatforms);
    projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
    currentTime = 0.0f;
}


void Scene::update(int deltaTime)
{
    currentTime += deltaTime;
    player->update(deltaTime);
    glm::ivec2 posPlayer = player->getPosition();

    // Actualizar las plataformas móviles
    for (auto platform : movingPlatforms) {
        platform->update(deltaTime);
    }

    // Verificar si el jugador ha alcanzado el punto de control actual
    if (!isAnimating && currentCheckpoint < checkpoints.size() && posPlayer.x >= checkpoints[currentCheckpoint])
    {
        // Iniciar la animación de desplazamiento a la derecha
        isAnimating = true;
        animationProgress = 0.0f;
        initialCamX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f; // Guardar la posición inicial de la cámara
        player->setLeftLimit(checkpoints[currentCheckpoint]);
        currentCheckpoint++;
        followHorizontal = !followHorizontal; // Invertir la dirección de seguimiento
        if (currentCheckpoint == 5) bossCam = true;
    }

    // Ejecutar la animación de desplazamiento a la derecha
    if (isAnimating)
    {
        animationProgress += deltaTime * 0.002f; // Ajusta la velocidad de la animación
        if (animationProgress >= 1.0f)
        {
            animationProgress = 1.0f;
            isAnimating = false;
        }
    }

    // Calcular las coordenadas de la cámara
    float camX = posPlayer.x + 32.f - CAMERA_WIDTH / 2.0f;
    float camY = posPlayer.y + 32.f - CAMERA_HEIGHT / 2.0f;

    if (!bossCam) {
        // Ajustar la lógica de la cámara según la variable followHorizontal
        if (followHorizontal)
        {
            if (currentCheckpoint == 0) camY = 16.f; // Mantener la posición de la cámara en el eje y constante
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

    // Ajustar la posición de la cámara durante la animación
    if (isAnimating)
    {
        camX = glm::mix(initialCamX, checkpoints[currentCheckpoint - 1] + 32.f, animationProgress);
    }

    // Ajustar la matriz de proyección para centrar la cámara en posPlayer
    projection = glm::ortho(camX, camX + CAMERA_WIDTH, camY + CAMERA_HEIGHT, camY);
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
    mapBackground->render();
    mapWalls->render();
    mapPlatforms->render();

    // Renderizar las plataformas móviles
    for (auto platform : movingPlatforms) {
        platform->render();
    }

    player->render();
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

int Scene::getCurrentCheckpoint()
{
	return currentCheckpoint;
}

float Scene::getCameraLimit()
{
	return cameraLimits[currentCheckpoint].first;
}
