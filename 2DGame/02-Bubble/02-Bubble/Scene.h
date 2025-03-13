#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE


#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"


// Scene contains all the entities of our game.
// It is responsible for updating and render them.


class Scene
{

public:
	Scene();
	~Scene();

	void init();
	void update(int deltaTime);
	void render();

	int getCurrentCheckpoint();
	float getCameraLimit();

private:
	void initShaders();

private:
	TileMap *mapWalls, *mapBackground, *mapPlatforms;
	Player *player;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	bool animationCompleted;
	bool followHorizontal; // Variable para controlar si la cámara sigue horizontalmente o verticalmente

	int currentCheckpoint; // Índice del punto de control actual
	bool isAnimating; // Variable para controlar si el jugador está en movimiento
	float animationProgress; // Variable para controlar el progreso de la animación
	float initialCamX;

	const std::vector<float> checkpoints = { 2048.f, 2304.f ,3072.f, 3312.f}; // Lista de puntos de control
	const std::vector<std::pair<float, float>> cameraLimits = { {32.0f,2080.f}, {16.f,1680.f}, {2336.f, 3104.f}, {496.f,1680.f} , {3344.f,4128.f} };

};


#endif // _SCENE_INCLUDE

