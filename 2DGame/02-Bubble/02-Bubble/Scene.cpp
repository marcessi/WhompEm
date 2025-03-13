#include <iostream>
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"
#include "Game.h"


#define SCREEN_X 32
#define SCREEN_Y 16

#define INIT_PLAYER_X_TILES 131 /*4 123 131 205*/
#define INIT_PLAYER_Y_TILES 99 /*10 3 99 33*/

Scene::Scene()
{
    map = NULL;
    player = NULL;
    followHorizontal = true; // Inicializa la variable para seguir horizontalmente
    currentCheckpoint = 0; // Inicializa el índice del punto de control actual
    isAnimating = false; // Inicializa la variable de animación
    animationProgress = 0.0f; // Inicializa el progreso de la animación
}

Scene::~Scene()
{
	if(map != NULL)
		delete map;
	if(player != NULL)
		delete player;
}


void Scene::init()
{
	initShaders();
	map = TileMap::createTileMap("levels/sacredwoods_collisions.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	mapBackground = TileMap::createTileMap("levels/sacredwoods_nocollisions.txt", glm::vec2(SCREEN_X, SCREEN_Y), texProgram);
	player = new Player();
	player->init(glm::ivec2(SCREEN_X, SCREEN_Y), texProgram);
	player->setPosition(glm::vec2(INIT_PLAYER_X_TILES * map->getTileSize(), INIT_PLAYER_Y_TILES * map->getTileSize()));
	player->setTileMap(map);
	projection = glm::ortho(0.f, float(SCREEN_WIDTH), float(SCREEN_HEIGHT), 0.f);
	currentTime = 0.0f;
}

void Scene::update(int deltaTime)
{
    currentTime += deltaTime;
    player->update(deltaTime);
    glm::ivec2 posPlayer = player->getPosition();

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
    map->render();
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
