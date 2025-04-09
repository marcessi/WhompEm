#include "PowerUp.h"

PowerUp::PowerUp(PowerUpType type)
{
    this->type = type;
    active = true;
    sprite = nullptr; // Inicializar sprite a nullptr para evitar problemas de memoria
    bFloating = true;
    bFloatingUpwards = false;
    floatAngle = 0.0f;
    xOffset = 0.0f;
}

PowerUp::~PowerUp()
{
    if (sprite != nullptr) {
        // Es importante liberar el recurso antes de eliminarlo
        sprite->free();
        delete sprite;
        sprite = nullptr; // Evitar accesos posteriores
    }
}

// En PowerUp.cpp - Modificar el método init
void PowerUp::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
    tileMapDispl = tileMapPos;

    // Cargar la textura apropiada según el tipo
    if (type == SMALL_HEART) {
        spritesheet.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);
        size = glm::ivec2(16, 16);
        setFloatingType(true, true);
    }
    else if (type == LARGE_HEART) {
        spritesheet.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);
        size = glm::ivec2(16, 16);
        setFloatingType(true, false);
    }
    else if (type == MAGIC_POTION) {
        spritesheet.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);
        size = glm::ivec2(16, 16);
        setFloatingType(true, false);
    }
    else if (type == FLINT_SPEAR) {
        spritesheet.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);
        size = glm::ivec2(16, 16);
        setFloatingType(true, false);
    }
    else if (type == BUFFALO_HELMET) {
        spritesheet.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);
        size = glm::ivec2(16, 16);
        setFloatingType(true, false);
    }
    else if (type == DEERSKIN_SHIRT) {
        spritesheet.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);
        size = glm::ivec2(16, 16);
        setFloatingType(true, false);
    }
    else { // GOURD
        spritesheet.loadFromFile("images/HUD.png", TEXTURE_PIXEL_FORMAT_RGBA);
        size = glm::ivec2(16, 16);
        setFloatingType(true, false);
    }

    // Crear el sprite con las dimensiones correctas
    sprite = Sprite::createSprite(size, glm::vec2(0.25f, 0.25f), &spritesheet, &shaderProgram);

    // Configurar animación simple
    sprite->setNumberAnimations(1);
    sprite->setAnimationSpeed(0, 8);
    if (type == SMALL_HEART) {
        sprite->addKeyframe(0, glm::vec2(0.0f, 0.25f)); // Corazón pequeño
    }
    else if (type == LARGE_HEART) {
        sprite->addKeyframe(0, glm::vec2(0.25f, 0.25f)); // Corazón grande
    }
    else if (type == MAGIC_POTION) {
        sprite->addKeyframe(0, glm::vec2(0.5f, 0.250f)); // Poción mágica
    }
    else if (type == FLINT_SPEAR) {
        sprite->addKeyframe(0, glm::vec2(0.0f, 0.50f)); // Flint Spear Head (ajusta coordenadas)
    }
    else if (type == BUFFALO_HELMET) {
        sprite->addKeyframe(0, glm::vec2(0.250f, 0.50f)); // Buffalo Helmet (ajusta coordenadas)
    }
    else if (type == DEERSKIN_SHIRT) {
        sprite->addKeyframe(0, glm::vec2(0.5f, 0.50f)); // Deerskin Shirt (ajusta coordenadas)
    }
    else { // GOURD
        sprite->addKeyframe(0, glm::vec2(0.75f, 0.5f)); // Calabaza
    }

    sprite->changeAnimation(0);
}


// PowerUp.cpp - Método update rediseñado
void PowerUp::update(int deltaTime)
{
    if (!active)
        return;

    sprite->update(deltaTime);

    if (bFloating) {
        // Actualizar el ángulo de flotación
        floatAngle += 0.05f;
        if (floatAngle >= 360.0f)
            floatAngle -= 360.0f;

        if (bFloatingUpwards) {
            // Para corazones pequeños: 
            // 1. Movimiento ascendente constante
            position.y -= 0.1f; // Velocidad de ascenso más rápida

            // 2. Calcular explícitamente una nueva posición X con movimiento sinusoidal
            // El tambaleo en X debe ser independiente de la posición original
            float newX = position.x + 8.0f * sin(3.14159f * floatAngle / 30.0f);

            // 3. Posicionar el sprite con la nueva posición X calculada
            sprite->setPosition(glm::vec2(
                float(tileMapDispl.x + newX),
                float(tileMapDispl.y + position.y)
            ));

            // Importante: NO actualizamos position.x para que la posición base se mantenga
            // El efecto visual se produce por el cálculo de newX
        }
        else {
            // Para corazones grandes y calabazas: solo flota arriba y abajo en el sitio
            float yOffset = 4.0f * sin(3.14159f * floatAngle / 180.0f);
            sprite->setPosition(glm::vec2(
                float(tileMapDispl.x + position.x),
                float(tileMapDispl.y + position.y + yOffset)
            ));
        }
    }
    else {
        // Sin flotación, posición estática
        sprite->setPosition(glm::vec2(
            float(tileMapDispl.x + position.x),
            float(tileMapDispl.y + position.y)
        ));
    }
}

void PowerUp::setFloatingType(bool floating, bool upwards)
{
    bFloating = floating;
    bFloatingUpwards = upwards;
}


void PowerUp::render()
{
    if (active)
        sprite->render();
}

void PowerUp::setPosition(const glm::vec2& pos)
{
    position = glm::ivec2(pos.x, pos.y);
    baseY = pos.y; // Guardar Y base para la animación de flotación
    sprite->setPosition(glm::vec2(float(tileMapDispl.x + position.x), float(tileMapDispl.y + position.y)));
}

void PowerUp::setTileMap(TileMap* tileMapWalls, TileMap* tileMapPlatforms)
{
    mapWalls = tileMapWalls;
    mapPlatforms = tileMapPlatforms;
}

bool PowerUp::collisionWithPlayer(const glm::ivec2& playerPos, const glm::ivec2& playerSize) const
{
    if (!active)
        return false;
        
    // Hitbox para el jugador (más pequeña que el sprite para colisión precisa)
    glm::ivec2 playerCenter = playerPos + playerSize / 2;
    
    // Hitbox para el powerup
    glm::ivec2 powerupCenter = position + size / 2;
    
    // Distancia entre centros
    int distX = abs(playerCenter.x - powerupCenter.x);
    int distY = abs(playerCenter.y - powerupCenter.y);
    
    // Suma de mitades
    int halfWidth = (playerSize.x + size.x) / 2;
    int halfHeight = (playerSize.y + size.y) / 2;
    
    // Si la distancia es menor que la suma de mitades, hay colisión
    return (distX < halfWidth && distY < halfHeight);
}
bool PowerUp::isActive() const
{
    return active;
}

void PowerUp::deactivate()
{
    active = false;
}

PowerUpType PowerUp::getType() const
{
    return type;
}