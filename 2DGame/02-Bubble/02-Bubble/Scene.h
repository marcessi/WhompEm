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
#include "FallingStick.h"
#include "HUD.h"
#include "Orco.h"
#include <map>
#include "PowerUp.h"
#include "Boss.h"



#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 960
#define CAMERA_WIDTH 256
#define CAMERA_HEIGHT 240

// Enumeraci�n para los estados del juego
enum GameState {
	MENU_MAIN,
	MENU_INSTRUCTIONS,
	MENU_CREDITS,
	GAMEPLAY,
	GAME_OVER,
	VICTORY
};

// Enumeraci�n para las opciones del men� principal
enum MainMenuOption {
	OPTION_START_GAME,
	OPTION_INSTRUCTIONS,
	OPTION_CREDITS
};

enum OrcSpawnState {
	SPAWN_AVAILABLE,   // El punto está disponible para spawneo inicial
	SPAWN_ACTIVE,      // Hay un orco activo asociado a este punto
	SPAWN_DEAD,        // El orco de este punto murió
	SPAWN_LEFT_SCREEN  // El punto salió de la pantalla
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
	void setPlayerHealth( std::vector<int>& health);
	void setPlayerLights(int lights);
	void setBossDefeated();
	void updateOrcos(int deltaTime);
	void updatePowerUps(int deltaTime);
	void spawnPowerUp(const glm::vec2& position, PowerUpType type);

private:
	void initShaders();
	void initMenus();
	void initSounds();
	void resetGame();

	void updateMenu(int deltaTime);
	void updateGameplay(int deltaTime);
	PowerUpType getRandomPowerUpType();

	void renderMenu();
	void renderGameplay();

	void handleMenuInput();

	void updateSnakes(int deltaTime);
	bool checkSpikeCollision();
	void updateFallingSticks(int deltaTime);
	void updateBoss(int deltaTime);
	void checkGameEndConditions();


private:

	AudioManager audioManager;

	// Estado actual del juego
	GameState gameState;

	// Variables del men� con Sprite
	MainMenuOption currentOption;

	Sprite* mainMenuSprites[3]; // Un sprite para cada opci�n del men� principal
	Sprite* instructionsSprite; // Sprite para la pantalla de instrucciones
	Sprite* creditsSprite;      // Sprite para la pantalla de cr
	Sprite* gameOverSprite;
	Sprite* victorySprite;

	float menuTime;
	bool keyPressed;
	int keyPressedTimer;

	bool bossDefeated;

	std::vector<PowerUp*> powerUps;

	std::vector<FallingStick*> fallingSticks;
	std::vector<float> stickPositionsX; // Posiciones X fijas para los palos
	const float STICK_MIN_X = 980.0f;   // Límite inferior del área de palos
	const float STICK_MAX_X = 1950.0f;  // Límite superior del área de palos
	const int NUM_STICKS = 5;           // Número total de palos
	
	Boss* boss;
	bool bossSpawned;

	std::vector<Orco*> orcos;
	std::vector<float> orcoSpawnPositionsX;
	
	// Y cambia la declaración del vector:
	std::vector<OrcSpawnState> orcoSpawnStates;
	std::map<int, Orco*> spawnPointToOrco;
	// Variables del juego
	TileMap* mapWalls;
	TileMap* mapBackground;
	TileMap* mapPlatforms;
	TileMap* mapFrontal;
	TileMap* mapSpikes;
	Player* player;
	std::vector<Snake*> snakes;
	HUD* hud;
	ShaderProgram texProgram;
	float currentTime;
	glm::mat4 projection;

	const float SNAKE_MIN_X = 2450.0f;
	const float SNAKE_MAX_X = 2950.0f;
	float snakeSpawnTimer;
	const float SNAKE_SPAWN_INTERVAL = 2000.0f;

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