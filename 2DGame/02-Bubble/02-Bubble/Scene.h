#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE

#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "MovingPlatform.h"
#include "AudioManager.h"


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 960
#define CAMERA_WIDTH 256
#define CAMERA_HEIGHT 240

// Enumeración para los estados del juego
enum GameState {
	MENU_MAIN,
	MENU_INSTRUCTIONS,
	MENU_CREDITS,
	GAMEPLAY
};

// Enumeración para las opciones del menú principal
enum MainMenuOption {
	OPTION_START_GAME,
	OPTION_INSTRUCTIONS,
	OPTION_CREDITS
};

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
	void initMenus();
	void initSounds();
	void updateMenu(int deltaTime);
	void updateGameplay(int deltaTime);
	void renderMenu();
	void renderGameplay();
	void handleMenuInput();

private:

	AudioManager audioManager;

	// Estado actual del juego
	GameState gameState;

	// Variables del menú con Sprite
	MainMenuOption currentOption;

	Sprite* mainMenuSprites[3]; // Un sprite para cada opción del menú principal
	Sprite* instructionsSprite; // Sprite para la pantalla de instrucciones
	Sprite* creditsSprite;      // Sprite para la pantalla de cr

	float menuTime;
	bool keyPressed;
	int keyPressedTimer;

	// Variables del juego
	TileMap* mapWalls;
	TileMap* mapBackground;
	TileMap* mapPlatforms;
	TileMap* mapFrontal;
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