#ifndef _SCENE_INCLUDE
#define _SCENE_INCLUDE

#include <glm/glm.hpp>
#include <vector>
#include "ShaderProgram.h"
#include "TileMap.h"
#include "Player.h"
#include "MovingPlatform.h"
#include "AudioManager.h"
#include "Snake.h"
#include "HUD.h"
#include "MovingPlatform.h" // Nueva inclusi�n


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 960
#define CAMERA_WIDTH 256
#define CAMERA_HEIGHT 240

// Enumeraci�n para los estados del juego
enum GameState {
	MENU_MAIN,
	MENU_INSTRUCTIONS,
	MENU_CREDITS,
	GAMEPLAY
};

// Enumeraci�n para las opciones del men� principal
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
	void setPlayerHealth(const glm::ivec4& health);
	void setPlayerLights(int lights);

private:
	void initShaders();
	void initMenus();
	void initSounds();
	void updateMenu(int deltaTime);
	void updateGameplay(int deltaTime);
	void renderMenu();
	void renderGameplay();
	void handleMenuInput();
	void updateSnakes(int deltaTime);

private:

	AudioManager audioManager;

	// Estado actual del juego
	GameState gameState;

	// Variables del men� con Sprite
	MainMenuOption currentOption;

	Sprite* mainMenuSprites[3]; // Un sprite para cada opci�n del men� principal
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
	std::vector<Snake*> snakes;
	HUD* hud;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	const float SNAKE_MIN_X = 2450.0f;
	const float SNAKE_MAX_X = 2950.0f;
	float snakeSpawnTimer;
	const float SNAKE_SPAWN_INTERVAL = 3000.0f;

	// Plataformas m�viles
	std::vector<MovingPlatform*> movingPlatforms;

	// Variables de c�mara
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