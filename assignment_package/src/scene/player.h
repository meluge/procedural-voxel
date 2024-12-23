#pragma once
#include "entity.h"
#include "camera.h"
#include "terrain.h"
#include <map>
#include <QSoundEffect>

struct Collision {
    bool happened;
    BlockType block;
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 normal;

    // Constructor
    Collision(bool happened, BlockType block, const glm::vec3& position,
              const glm::vec3& direction, const glm::vec3& normal)
        : happened(happened), block(block), position(position), direction(direction), normal(normal) {}

};

class Player : public Entity {
private:
    glm::vec3 m_velocity, m_acceleration;
    Camera m_camera;
    const Terrain &mcr_terrain;
    int jumpCoolDown;

    float rotateBufferX, rotateBufferY;

    void processInputs(InputBundle &inputs);
    void computePhysics(float dT, const Terrain &terrain);

    void jump();

    std::map<int, std::pair<BlockType, int>> m_inventorySlots;
    std::map<int, std::pair<BlockType, int>> m_craftingSlots;


public:
    // Readonly public reference to our camera
    // for easy access from MyGL
    const Camera& mcr_camera;

    Player(glm::vec3 pos, const Terrain &terrain);
    virtual ~Player() override;

    void setCameraWidthHeight(unsigned int w, unsigned int h);

    void tick(float dT, InputBundle &input) override;

    bool getIsOnGround();
    glm::vec3 getVelocity();

    // Player overrides all of Entity's movement
    // functions so that it transforms its camera
    // by the same amount as it transforms itself.
    void moveAlongVector(glm::vec3 dir) override;
    void moveForwardLocal(float amount) override;
    void moveRightLocal(float amount) override;
    void moveUpLocal(float amount) override;
    void moveForwardGlobal(float amount) override;
    void moveRightGlobal(float amount) override;
    void moveUpGlobal(float amount) override;
    void rotateOnForwardLocal(float degrees) override;
    void rotateOnRightLocal(float degrees) override;
    void rotateOnUpLocal(float degrees) override;
    void rotateOnForwardGlobal(float degrees) override;
    void rotateOnRightGlobal(float degrees) override;
    void rotateOnUpGlobal(float degrees) override;

    // For sending the Player's data to the GUI
    // for display
    QString posAsQString() const;
    QString velAsQString() const;
    QString accAsQString() const;
    QString lookAsQString() const;
    void toggleFlightMode();
    Collision getLookCollision(float length);
    Collision LinePlaneCollision(glm::vec3 p0, glm::vec3 dir, glm::vec3 p1, glm::vec3 nor);
    Collision testRayCollision(glm::vec3 start, glm::vec3 dir, bool exact);

    bool isInventorySlotEmpty(int slot);
    BlockType getBlockAtSlot(int slot);
    void addBlockToInventorySlot(BlockType block);
    void addBlockSlot(BlockType block, int slot);
    void removeBlockFromInventorySlot(int slot);
    std::map<int, std::pair<BlockType, int>> getInventory();
    std::map<int, std::pair<BlockType, int>> getCraftingInventory();

    void addBlockCraftingSlot(BlockType block, int slot);
    void removeBlockFromCraftingSlot(int slot);
    void addBlockToCraftingSlot(BlockType block);
    BlockType getBlockAtSlotCrafting(int slot);
    bool isCraftingSlotEmpty(int slot);
    void eraseBlockFromInventorySlot(BlockType block);
    void removeBlockFromInventoryBlock(BlockType block);
    void decrementSlots(int index);
    void returnCraftingToInventory();
    void updateQuantityOfBlock(BlockType block, int quantity);
    void eraseCrafting();
};
