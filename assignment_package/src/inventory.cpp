#include "inventory.h"

const int Inventory::GRID_WIDTH;
const int Inventory::GRID_HEIGHT;
const int Inventory::CELL_SIZE;

Inventory::Inventory()
{}

Inventory::~Inventory()
{}


glm::vec2 Inventory::getInventoryPosition(glm::vec2 screenDimensions) {
    float totalWidth = GRID_WIDTH * CELL_SIZE;
    float startX = (screenDimensions.x - totalWidth) / 2.0f;
    float startY = screenDimensions.y - CELL_SIZE * 1.5f;
    return glm::vec2(startX, startY);
}

int Inventory::getSlotAt(float x, float y) {
    glm::vec2 inventoryPos = getInventoryPosition(glm::vec2(x, y));

    if (x < inventoryPos.x || x > inventoryPos.x + GRID_WIDTH * CELL_SIZE ||
        y < inventoryPos.y || y > inventoryPos.y + GRID_HEIGHT * CELL_SIZE) {
        return -1;
    }

    int gridX = (x - inventoryPos.x) / CELL_SIZE;
    int gridY = (y - inventoryPos.y) / CELL_SIZE;

    return gridX + gridY * GRID_WIDTH;
}



