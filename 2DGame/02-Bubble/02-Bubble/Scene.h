#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE

#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "MovingPlatform.h" // Nueva inclusión

#define CAMERA_WIDTH 640
#define CAMERA_HEIGHT 480

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
	TileMap* mapWalls;
	TileMap* mapBackground;
	TileMap* mapPlatforms;
	Player* player;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	// Plataformas móviles
	std::vector<MovingPlatform*> movingPlatforms;

	// Variables de cámara
	bool followHorizontal;
	int currentCheckpoint;
	bool isAnimating;
	bool bossCam;
	float animationProgress;
	float initialCamX;

	const std::vector<float> checkpoints = { 2048.f, 2304.f ,3072.f, 3312.f, 3840.f }; // Lista de puntos de control
	const std::vector<std::pair<float, float>> cameraLimits = { {32.0f,2080.f}, {16.f,1680.f}, {2336.f, 3104.f}, {496.f,1680.f} , {3344.f,3872.f} };

};

#endif // _SCENE_INCLUDE
