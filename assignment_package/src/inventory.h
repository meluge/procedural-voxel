#pragma once
#include <glm/glm.hpp>
#include <vector>

class Inventory {
public:
    Inventory();
    virtual ~Inventory();
    glm::vec2 getInventoryPosition(glm::vec2 screenDimensions);
    int getSlotAt(float x, float y);
    static const int GRID_WIDTH = 9;
    static const int GRID_HEIGHT = 4;
    static const int CRAFTING_GRID_SIZE = 3;
    static const int CELL_SIZE = 64;
    glm::vec2 getCraftingGridPosition(glm::vec2 screenDimensions);
    int getCraftingSlotAt(float x, float y);

};
