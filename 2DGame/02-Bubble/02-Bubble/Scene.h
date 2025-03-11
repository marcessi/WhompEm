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

private:
	void initShaders();

private:
	TileMap *map;
	TileMap* mapBackground;
	Player *player;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;
	bool animationCompleted;
	bool followHorizontal; // Variable para controlar si la c�mara sigue horizontalmente o verticalmente
	std::vector<int> checkpoints; // Lista de puntos de control
	int currentCheckpoint; // �ndice del punto de control actual
	bool isAnimating; // Variable para controlar si el jugador est� en movimiento
	float animationProgress; // Variable para controlar el progreso de la animaci�n
};


#endif // _SCENE_INCLUDE

