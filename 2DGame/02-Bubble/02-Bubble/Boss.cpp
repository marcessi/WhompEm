#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include "Boss.h"

Boss::Boss()
{
    mapWalls = nullptr;
    mapPlatforms = nullptr;
    playerPos = nullptr;
    playerHitCallback = nullptr;
    alive = true;
    lives = 9;
    hitsPerLife = 3;
    invulnerable = false;
    visible = true;
    sprite = nullptr;
    leaves_active = false;
    leaf_animation_timer = 0.0f;
    circle_radius = 10.0f;
    circle_max_radius = 100.0f;
    expand_circle = true;
}

Boss::~Boss()
{
    if (sprite != nullptr)
        delete sprite;

    // Eliminar los sticks que quedan
    for (auto stick : fallingSticks) {
        delete stick;
    }
    fallingSticks.clear();

    for (auto& leaf : leaves) {
        if (leaf.sprite != nullptr) {
            leaf.sprite->free();
            delete leaf.sprite;
        }
    }
    leaves.clear();

}

void Boss::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
    // Inicializar sprite
    spritesheet.loadFromFile("images/boss.png", TEXTURE_PIXEL_FORMAT_RGBA);
    sprite = Sprite::createSprite(glm::ivec2(32, 48), glm::vec2(0.25f, 0.5f), &spritesheet, &shaderProgram);
    std::cout << "Boss sprite loaded: " << (spritesheet.width() > 0) << std::endl;
    // Configurar animaciones
    sprite->setNumberAnimations(5);
    firstTime = true;
    // Animación IDLE
    sprite->setAnimationSpeed(ANIM_IDLE, 8);
    sprite->addKeyframe(ANIM_IDLE, glm::vec2(0.5f, 0.0f));

    // Animación ATTACKING
    sprite->setAnimationSpeed(ANIM_ATTACKING_LEFT, 8);
    sprite->addKeyframe(ANIM_ATTACKING_LEFT, glm::vec2(0.f, 0.0f));

	sprite->setAnimationSpeed(ANIM_ATTACKING_RIGHT, 8);
	sprite->addKeyframe(ANIM_ATTACKING_RIGHT, glm::vec2(0.f, 0.5f));
   

    // Animación CRAZY
    sprite->setAnimationSpeed(ANIM_CRAZY, 12);
    sprite->addKeyframe(ANIM_CRAZY, glm::vec2(0.75f, 0.f));


    // Animación GROUNDED
    sprite->setAnimationSpeed(ANIM_GROUNDED, 8);
    sprite->addKeyframe(ANIM_GROUNDED, glm::vec2(0.25f, 0.f));
  

    // Iniciar en estado IDLE
    sprite->changeAnimation(ANIM_IDLE);
    tileMapDispl = tileMapPos;

    // Inicializar variables de estado
    state = IDLE;
    stateTimer = 0.0f;
    attackTimer = 0.0f;
    stickSpawnTimer = 0.0f;

    // Configurar movimiento
    vx = 0.3f;
    vy = 0.0f;
    amplitude = 10.0f;
    frequency = 0.001f;
    phase = 0.0f;
    visible = true;


    leafSpritesheet.loadFromFile("images/bossShits.png", TEXTURE_PIXEL_FORMAT_RGBA);

    // Crear 8 hojas
    for (int i = 0; i < 8; ++i) {
        Leaf leaf;
        leaf.sprite = Sprite::createSprite(glm::ivec2(16, 16), glm::vec2(0.125f, 1.f), &leafSpritesheet, &shaderProgram);
        leaf.sprite->setNumberAnimations(2);

        // Animación 1: Hoja girando
        leaf.sprite->setAnimationSpeed(0, 8);
        leaf.sprite->addKeyframe(0, glm::vec2(0.5f, 0.0f));

        // Animación 2: Hoja brillando
        leaf.sprite->setAnimationSpeed(1, 8);
        leaf.sprite->addKeyframe(1, glm::vec2(0.625f, 0.f));

        leaf.sprite->changeAnimation(0);

        // Distribuir las hojas alrededor del círculo
        leaf.angle = i * (2.0f * 3.14159 / 8.0f); // Ángulo inicial
        leaf.radius = circle_radius; // Radio inicial
        leaf.rotation_speed = 0.002f + ((rand() % 10) * 0.0001f); // Velocidad de rotación aleatoria
        leaf.animation_timer = 0; // Temporizador para animación

        leaves.push_back(leaf);
    }
}

void Boss::update(int deltaTime)
{
    if (!alive)
        return;

    // Control de invulnerabilidad
    if (invulnerable) {
        invulnerableTimer += deltaTime;
        visible = ((int)(invulnerableTimer / 100) % 2) == 0;

        if (invulnerableTimer >= 1000) { // 1 segundo de invulnerabilidad
            invulnerable = false;
            visible = true;
        }
    }

    // Actualizar sprite
    sprite->update(deltaTime);

    // Actualizar según el estado actual
    switch (state) {
    case IDLE:
        updateIdleState(deltaTime);
        break;
    case ATTACKING:
        updateAttackingState(deltaTime);
        break;
    case CRAZY:
        updateCrazyState(deltaTime);
        break;
    case GROUNDED:
        updateGroundedState(deltaTime);
        break;
    }

    // Actualizar los palos que caen
    for (int i = 0; i < fallingSticks.size(); ++i) {
        fallingSticks[i]->update(deltaTime);

        // Eliminar palos que ya no son necesarios
        if (fallingSticks[i]->isDisappearing()) {
            delete fallingSticks[i];
            fallingSticks.erase(fallingSticks.begin() + i);
            --i;
        }
    }

    // Comprobar colisión con el jugador
    if (playerPos != nullptr) {
        glm::ivec2 playerHitbox(16, 32);
        glm::ivec2 bossHitbox = getSize();

        // Comprobar colisión AABB
        bool collisionX = position.x < playerPos->x + playerHitbox.x &&
            position.x + bossHitbox.x > playerPos->x;
        bool collisionY = position.y < playerPos->y + playerHitbox.y &&
            position.y + bossHitbox.y > playerPos->y;

        if (collisionX && collisionY && playerHitCallback) {
            playerHitCallback(); // Aplicar daño al jugador
        }
    }

    // Actualizar la posición del sprite
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));

    if (leaves_active) {
        leaf_animation_timer += deltaTime;

        // Ajustar el radio del círculo
        if (expand_circle) {
            // Expandir el círculo
            circle_radius += 0.15f * deltaTime;
            if (circle_radius >= circle_max_radius) {
                expand_circle = false; // Comenzar a contraer
            }
        }
        else {
            // Contraer el círculo
            circle_radius -= 0.15f * deltaTime;
            if (circle_radius <= 10.0f) { // Usar el mismo radio inicial
                // Al llegar al tamaño inicial, finalizar el efecto
                leaves_active = false;
                visible = true;
                
            }
        }

        // Actualizar cada hoja
        for (int i = 0; i < leaves.size(); ++i) {
            Leaf& leaf = leaves[i];

            // Actualizar posición en el círculo - invertir dirección si está contrayendo
            float direction = expand_circle ? 1.0f : -1.0f;
            leaf.angle += leaf.rotation_speed * deltaTime * direction;

            if (leaf.angle > 2.0f * 3.14159) {
                leaf.angle -= 2.0f * 3.14159;
            }
            else if (leaf.angle < 0) {
                leaf.angle += 2.0f * 3.14159;
            }

            // Actualizar radio
            leaf.radius = circle_radius;

            // Actualizar posición
            float leafX = leaves_center.x + cos(leaf.angle) * leaf.radius;
            float leafY = leaves_center.y + sin(leaf.angle) * leaf.radius;
            leaf.sprite->setPosition(glm::vec2(float(tileMapDispl.x + leafX), float(tileMapDispl.y + leafY)));

            // Actualizar animación
            leaf.animation_timer += deltaTime;
            if (leaf.animation_timer > 1000) { // Cambiar animación cada segundo
                leaf.animation_timer = 0;
                leaf.sprite->changeAnimation(1 - leaf.sprite->animation()); // Alternar entre 0 y 1
            }

            leaf.sprite->update(deltaTime);
        }
    }
}

void Boss::render()
{
    if (!alive)
        return;

    if (visible) {
   
        sprite->render();
    }

    // Renderizar los palos que caen
    for (auto stick : fallingSticks) {
        stick->render();
    }

    if (leaves_active) {
        for (auto& leaf : leaves) {
            leaf.sprite->render();
        }
    }
}

void Boss::updateIdleState(int deltaTime)
{
    stateTimer += deltaTime;

    // En estado IDLE, comprobar colisión con el suelo para "aterrizar"

    
    // Después de 2 segundos, cambiar a estado de ataque
    if (stateTimer >= 1500) {
        

        // y elevarse para empezar a volar
		position.y -= 0.1f * deltaTime;
        
		if (position.y <= initialY - 60.f) {

            stateTimer = 0.0f;
            firstTime = true;
            state = ATTACKING;
            sprite->changeAnimation(ANIM_ATTACKING_LEFT);
            initialY = position.y;
			
		}

        // Iniciar con dirección aleatoria
        vx  = 0.5f;
    }
   
}


void Boss::updateAttackingState(int deltaTime)
{
    stateTimer += deltaTime;
    attackTimer += deltaTime;
    stickSpawnTimer += deltaTime;

    // Movimiento horizontal mejorado
    position.x += vx * deltaTime * 0.4;
    // Límites de movimiento horizontal más amplios
    float baseMinX = 3850.0f;
    float baseMaxX = 4050.0f;

	  
	if (firstTime) {
        float totalRange = baseMaxX - baseMinX;

        // Asegurarnos que hay suficiente espacio para una diferencia de al menos 100
        if (totalRange < 50.0f) {
            // Si no hay suficiente espacio, usar valores por defecto
            minX = baseMinX;
            maxX = baseMaxX;
            return;
        }

        // Generar un punto de inicio aleatorio dentro del rango disponible
        float rangeStart = baseMinX + (rand() % static_cast<int>(totalRange - 50.0f));

        // Calcular el espacio disponible después del punto de inicio
        float availableSpace = baseMaxX - rangeStart;

        // Generar un tamaño de rango aleatorio entre 100 y el espacio disponible
        float rangeSize = 50.0f + (rand() % static_cast<int>(availableSpace - 50.0f + 1));

        // Establecer los valores finales
        minX = rangeStart;
        maxX = rangeStart + rangeSize;
		firstTime = false;
		position.x = minX + (rand() % static_cast<int>(maxX - minX)); // Posición inicial aleatoria

		// Inicializar la posición Y
        amplitude +=  (rand() % 31);
            
	}

    // Si llega a los bordes, cambiar dirección
    if (position.x <= minX || position.x >= maxX) {
        vx = -vx;
    }

    // Movimiento ondulatorio en Y
    phase += frequency * deltaTime * 2;
    position.y = initialY + amplitude * sin(phase);

    // Generar palos cayendo cada cierto tiempo
    if (stickSpawnTimer >= 250) { 
        stickSpawnTimer = 0.0f;
		if (sprite->animation() == ANIM_ATTACKING_LEFT)
			sprite->changeAnimation(ANIM_ATTACKING_RIGHT);
		else
			sprite->changeAnimation(ANIM_ATTACKING_LEFT);
        createFallingStick();
    }

    // Después de 8 segundos, cambiar a estado crazy
    if (stateTimer >= 8000) {
        stateTimer = 0.0f;
        state = CRAZY;
        amplitude = 10.0f;
        sprite->changeAnimation(ANIM_CRAZY);
         firstTime = true;

    }
}

void Boss::updateCrazyState(int deltaTime)
{
    stateTimer += deltaTime;

    // Movimiento en parábola con velocidad adecuada
    position.x += vx * deltaTime * 0.4;

    // Límites de movimiento horizontal más amplios
    float baseMinX = 3850.0f;
    float baseMaxX = 4050.0f;

   

    // Si llega a los bordes, cambiar dirección
    if (position.x <= minX || position.x >= maxX) {
        vx = -vx;
    }


    // La altura Y varía según la posición X (parábola)
    float centerX = 3550.0f; // Centro del área
    float distFromCenter = abs(position.x - centerX);
    float maxHeight = 20.0f; // Altura máxima de la parábola

    // Calcular posición Y usando una parábola invertida
    float normalizedDist = distFromCenter / (maxX - centerX); // Normalizar a [0,1]
    position.y = initialY + maxHeight * (normalizedDist * normalizedDist);

    // Generar sticks más frecuentemente en estado crazy
    


    // Después de 4 segundos, cambiar a estado grounded
    if (stateTimer >= 2000) {
        stateTimer = 0.0f;
        state = GROUNDED;
        firstTime = true;
		amplitude = 10.0f;
        sprite->changeAnimation(ANIM_GROUNDED);

        // Iniciar caída para buscar el suelo
        vy = 2.0f;
    }
}

void Boss::updateGroundedState(int deltaTime)
{
    stateTimer += deltaTime;

    // En estado GROUNDED, aplicar gravedad hasta tocar el suelo
    if (vy != 0) {
        vy += 0.2f; // Gravedad
        position.y += vy;

        // Comprobar colisión con el suelo
        glm::ivec2 bossSize = getSize();
        int bossBottom = position.y + bossSize.y;
        int auxY = bossBottom;

        if (position.y >= 590.f) {
            position.y = auxY - bossSize.y;
            vy = 0;
            visible = false;
            leaves_active = true;
            leaf_animation_timer = 0.0f;
            circle_radius = 10.0f;
            expand_circle = true;

            // Guardar el centro donde estaba el boss
            leaves_center = glm::vec2(position.x + bossSize.x / 2, position.y + bossSize.y / 2);

            // Posicionar las hojas alrededor del centro
            for (int i = 0; i < leaves.size(); ++i) {
                Leaf& leaf = leaves[i];
                leaf.angle = i * (2.0f * 3.14159 / 8.0f);
                leaf.radius = circle_radius;

                float leafX = leaves_center.x + cos(leaf.angle) * leaf.radius - 8.0f; // Centrar (16x16)
                float leafY = leaves_center.y + sin(leaf.angle) * leaf.radius - 8.0f;
                leaf.sprite->setPosition(glm::vec2(float(tileMapDispl.x + leafX), float(tileMapDispl.y + leafY)));
            }
                for (int i = 0; i < 10; ++i) {
                    float xOffset = 3800.0f + (i * 40.0f);

                    // Crear palos en posiciones fijas a través del área
                    FallingStick* stick = new FallingStick();
                    stick->init(tileMapDispl, *sprite->getShaderProgram());
                    stick->setPosition(glm::vec2(xOffset, position.y - 150.0f));
                    stick->setTileMap(mapWalls, mapPlatforms);
                    stick->startFalling(); // Iniciar la caída inmediatamente
                    fallingSticks.push_back(stick);
                }
            
        }
		stateTimer = 0.0f;
    }

    // Después de 2 segundos en el suelo, volver a estado idle
    if ( vy == 0 && !leaves_active) {

		if (stateTimer >= 3000) {
			state = ATTACKING;
			sprite->changeAnimation(ANIM_ATTACKING_LEFT);
			firstTime = true;
		}

    }
}

void Boss::createFallingStick()
{
    // Crear un palo cayendo desde las manos del jefe
    FallingStick* stick = new FallingStick();

    // Usar el mismo shader del sprite del boss
    stick->init(tileMapDispl, *sprite->getShaderProgram());

    // Posición basada en la posición del jefe
    float stickX = position.x + 8.0f; // Centro del boss
    float stickY = position.y + 25.0f; // Desde las manos

    stick->setPosition(glm::vec2(stickX, stickY));
    stick->setTileMap(mapWalls, mapPlatforms);

    // Iniciar inmediatamente la caída sin esperar
    stick->startFalling();

    fallingSticks.push_back(stick);
}


void Boss::setTileMap(TileMap* tileMapWalls, TileMap* tileMapPlatforms)
{
    mapWalls = tileMapWalls;
    mapPlatforms = tileMapPlatforms;
}

void Boss::setPosition(const glm::vec2& pos)
{
    position = pos;
    initialY = pos.y;
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
	cout << "Boss position set to: " <<  position.x << " " <<  position.y << endl;
}

void Boss::setPlayerPosition(const glm::ivec2* playerPos)
{
    this->playerPos = playerPos;
}

void Boss::setPlayerHitCallback(std::function<void()> callback)
{
    playerHitCallback = callback;
}

void Boss::hit()
{
    if (invulnerable)
        return;

    // Aplicar daño
    hitsPerLife--;

    if (hitsPerLife <= 0) {
        lives--;
        hitsPerLife = 3;

        // Comprobar si ha muerto
        if (lives <= 0) {
            alive = false;
            return;
        }
    }

    // Activar invulnerabilidad
    invulnerable = true;
    invulnerableTimer = 0.0f;
}

bool Boss::isAlive() const
{
    return alive;
}

glm::ivec2 Boss::getPosition() const
{
    return glm::ivec2(position.x, position.y);
}

glm::ivec2 Boss::getSize() const
{
    return glm::ivec2(32, 48);
}

std::vector<FallingStick*>& Boss::getFallingSticks()
{
    return fallingSticks;
}
