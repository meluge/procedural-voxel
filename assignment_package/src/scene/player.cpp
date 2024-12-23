#include "player.h"
#include <QString>
#include <vector>
#include <iostream>

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0.0f,0.0f,0.0f), m_acceleration(0.0f,0.0f,0.0f),
    m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
    mcr_camera(m_camera), jumpCoolDown(0)
{

}

Player::~Player()
{}

void Player::tick(float dT, InputBundle &input) {
    processInputs(input);
    computePhysics(dT, mcr_terrain);
}

void Player::toggleFlightMode() {
    flightMode = !flightMode;
}


Collision Player::getLookCollision(float length)
{
    glm::vec3 eye_pos = m_position + glm::vec3(0.0f,1.5f,0.0f);
    glm::vec3 ray_vec = glm::normalize(m_forward) * length;
    return testRayCollision(eye_pos, ray_vec, false);

}

void Player::jump() {

    isOnGround = false;
    m_acceleration[1] += 0.25f;
    m_velocity[1] += 0.1f;
}

void Player::processInputs(InputBundle &inputs) {

    jumpCoolDown--;

    // Update the Player's velocity and acceleration based on the
    // state of the inputs.

    rotateBufferX -= inputs.mouseX * 0.4;
    rotateBufferY -= inputs.mouseY * 0.4;

    //Mouse inputs
    if (rotateBufferY != 0.0) {
        rotateOnRightLocal(rotateBufferY);
        rotateBufferY *= 0.5;
    }
    if (rotateBufferX != 0.0) {
        rotateOnUpGlobal(rotateBufferX);
        rotateBufferX *= 0.5;
    }

    //WSADQE inputs
    if (this->flightMode) {

        //Update Acceleration based on direction
        glm::vec3 moveDirection = glm::vec3(0);

        if (inputs.wPressed) {
            moveDirection += m_forward;
        }
        if (inputs.sPressed) {
            moveDirection -= m_forward;
        }
        if (inputs.dPressed) {
            moveDirection += m_right;
        }
        if (inputs.aPressed) {
            moveDirection -= m_right;
        }
        if (inputs.ePressed) {
            moveDirection += m_up;
        }
        if (inputs.qPressed) {
            moveDirection -= m_up;
        }


        //Normalize Move Direction
        if (glm::length(moveDirection) > 0) {

            moveDirection = glm::normalize(moveDirection);

            //Acceleration Constant
            float m_acceleration_speed = 0.01f;
            m_acceleration += moveDirection * m_acceleration_speed;

        }

    } else {

        //When not in flight mode, run this:

        //Update Acceleration based on direction
        glm::vec3 moveDirection = glm::vec3(0);

        if (inputs.wPressed) {
            moveDirection += m_forward;
        }
        if (inputs.sPressed) {
            moveDirection -= m_forward;
        }
        if (inputs.dPressed) {
            moveDirection += m_right;
        }
        if (inputs.aPressed) {
            moveDirection -= m_right;
        }

        moveDirection[1] = 0.0f;

        glm::vec3 playerPos = m_position + glm::vec3(0, 1.5f, 0);
        BlockType currentBlock = mcr_terrain.getGlobalBlockAt(playerPos);
        bool isInLiquid = currentBlock == WATER || currentBlock == LAVA;

        if (isOnGround && inputs.spacePressed && jumpCoolDown <= 0 && !(isInLiquid)) {
            jump();
        }

        if (inputs.spacePressed && isInLiquid) {
            m_velocity.y += 0.5;
        }

        //Normalize Move Direction
        if (glm::length(moveDirection) > 0) {

            moveDirection = glm::normalize(moveDirection);

            //Acceleration Constant
            float m_acceleration_speed = 0.01f;
            m_acceleration += moveDirection * m_acceleration_speed;

        }

    }

}

void Player::computePhysics(float dT, const Terrain &terrain) {
    // TODO: Update the Player's position based on its acceleration
    // and velocity, and also perform collision detection.

    glm::vec3 playerPos = m_position + glm::vec3(0, 1.5f, 0);
    BlockType currentBlock = mcr_terrain.getGlobalBlockAt(playerPos);

    bool isInWater = (currentBlock == WATER);
    bool isInLava = (currentBlock == LAVA);

    //Temp. On Ground detection:
    if (isOnGround && !flightMode) {

        //check for all 4 corners
        bool stillOnGround = false;
        if (!Chunk::isPassable(mcr_terrain.getGlobalBlockAt(m_position + glm::vec3(0.3,-0.1,0.3)))) stillOnGround = true;
        if (!Chunk::isPassable(mcr_terrain.getGlobalBlockAt(m_position + glm::vec3(-.3,-0.1,-.3)))) stillOnGround = true;
        if (!Chunk::isPassable(mcr_terrain.getGlobalBlockAt(m_position + glm::vec3(-.3,-0.1,0.3)))) stillOnGround = true;
        if (!Chunk::isPassable(mcr_terrain.getGlobalBlockAt(m_position + glm::vec3(0.3,-0.1,-.3)))) stillOnGround = true;
        isOnGround = stillOnGround;
    }

    if (!flightMode && !isOnGround) {
        if (isInWater || isInLava) {
            m_acceleration[1] -= 0.02f * (2.f / 3.f);
        } else {
            m_acceleration[1] -= 0.02f;
        }
    }

    //Basic Physics
    m_velocity += m_acceleration;

    float m_acceleration_decrease_factor = 0.15f;
    float m_velocity_decrease_factor = 0.3f;
    if (flightMode) m_acceleration_decrease_factor = 0.1f;
    if (flightMode) m_velocity_decrease_factor = 0.2f;

    m_acceleration = (1-m_acceleration_decrease_factor) * m_acceleration;
    m_velocity = (1-m_velocity_decrease_factor) * m_velocity;

    //Actually Move
    std::vector<glm::vec3> hitboxCornerOffsets = {
        glm::vec3(-.3,0,-.3),
        glm::vec3(0.3,0,0.3),
        glm::vec3(-.3,0,0.3),
        glm::vec3(0.3,0,-.3),
        glm::vec3(-.3,0.9,-.3),
        glm::vec3(0.3,0.9,0.3),
        glm::vec3(-.3,0.9,0.3),
        glm::vec3(0.3,0.9,-.3),
        glm::vec3(-.3,1.8,-.3),
        glm::vec3(0.3,1.8,0.3),
        glm::vec3(-.3,1.8,0.3),
        glm::vec3(0.3,1.8,-.3)
    };

    for(int i = 0; i < 3; ++i) {
        if (abs(m_velocity[i]) > 0.0001) {

            float axis_velocity = m_velocity[i];
            glm::vec3 ray = glm::vec3(0, 0, 0);
            ray[i] = axis_velocity;

            //determine the minimum distance
            float min_dist = glm::length(ray);

            for (glm::vec3 offset : hitboxCornerOffsets) {

                if ((offset[i] * axis_velocity) < 0) continue;

                glm::vec3 start_pos = m_position + offset;
                Collision c = testRayCollision(start_pos, ray, true);

                if (c.happened) {


                    float col_dist = glm::length(c.position - start_pos);
                    if (col_dist < min_dist) {
                        min_dist = col_dist;

                        //Check if hit ground
                        if (!isOnGround && i == 1 && axis_velocity < 0) {
                            isOnGround = true;
                            jumpCoolDown = 10;
                        }

                    }
                }

            }

            float final_dist = axis_velocity;
            final_dist = std::copysignf(min_dist, final_dist);
            if (final_dist > 0.01) {
                final_dist -= 0.01;
            } else if (final_dist < -0.01) {
                final_dist += 0.01;
            } else {
                final_dist = 0;
            }

            if (isInLava || isInWater) {
                final_dist *= (2.f/3.f);
            }


            if (i == 0) moveRightGlobal(final_dist);
            if (i == 1) moveUpGlobal(final_dist);
            if (i == 2) moveForwardGlobal(final_dist);

        }
    }
}

Collision Player::LinePlaneCollision(glm::vec3 p0, glm::vec3 dir, glm::vec3 p1, glm::vec3 nor) {

    float denom = glm::dot(nor, dir);
    if (fabs(denom) < 1e-6) {
        return Collision(false, EMPTY, p0, dir, nor);
    }
    float t = glm::dot(nor, p1 - p0) / denom;

    glm::vec3 finalP = p0 + t * dir;

    glm::vec3 finalDir = glm::vec3(0);
    if (t < 1.0f) {
        finalDir = (1.0f-t) * dir;
    }

    //check if out of bounds
    glm::vec3 bounds = glm::abs(finalP - p1);

    if (bounds.x <= 0.5 && bounds.y <= 0.5 && bounds.z <= 0.5) {

        return Collision(true, EMPTY, finalP, finalDir, nor);
    } else {

        return Collision(false, EMPTY, finalP, finalDir, nor);
    }

}

Collision Player::testRayCollision(glm::vec3 start, glm::vec3 dir, bool centered) {
    //test collision from a start point along a vector.
    //return: a Collision struct with bool, blocktype, position and direction.
    glm::vec3 curr_pos = glm::vec3(start);
    glm::vec3 curr_dir = glm::vec3(dir);
    glm::vec3 collision_normal = glm::vec3(0);
    glm::vec3 curr_block_center = glm::floor(curr_pos) + glm::vec3(0.5f);

    bool continue_loop = true;
    //check if the current dir vector intersects with any of the block outer planes
    while (continue_loop) {

        //check block of current location
        BlockType currBlock = mcr_terrain.getGlobalBlockAt(curr_block_center);
        if (!Chunk::isPassable(currBlock)) {
            if (centered) {
                return Collision(true, currBlock, curr_pos, dir, collision_normal);
            } else {
                return Collision(true, currBlock, curr_block_center, dir, collision_normal);
            }
        }

        //find the next location
        //check for intersections on x, y, z plane.
        Collision c = Collision(false, EMPTY, glm::vec3(0), glm::vec3(0), glm::vec3(0));
        if (curr_dir.x > 0 && !c.happened) {
            c = LinePlaneCollision(curr_pos, curr_dir, curr_block_center + glm::vec3(0.5,0,0), glm::vec3(-1,0,0));
        }
        if (curr_dir.x < 0 && !c.happened) {
            c = LinePlaneCollision(curr_pos, curr_dir, curr_block_center + glm::vec3(-.5,0,0), glm::vec3(1,0,0));
        }
        if (curr_dir.y > 0 && !c.happened) {
            c = LinePlaneCollision(curr_pos, curr_dir, curr_block_center + glm::vec3(0,.5,0), glm::vec3(0,-1,0));
        }
        if (curr_dir.y < 0 && !c.happened) {
            c = LinePlaneCollision(curr_pos, curr_dir, curr_block_center + glm::vec3(0,-.5,0), glm::vec3(0,1,0));
        }
        if (curr_dir.z > 0 && !c.happened) {
            c = LinePlaneCollision(curr_pos, curr_dir, curr_block_center + glm::vec3(0,0,.5), glm::vec3(0,0,-1));
        }
        if (curr_dir.z < 0 && !c.happened) {
            c = LinePlaneCollision(curr_pos, curr_dir, curr_block_center + glm::vec3(0,0,-.5), glm::vec3(0,0,1));
        }
        //resolve results
        if (c.happened) {

            curr_pos = c.position;
            curr_dir = c.direction;
            collision_normal = c.normal;
            curr_block_center -= c.normal;

        } else {

            continue_loop = false;

        }

    }

    return Collision(false, EMPTY, curr_pos + curr_dir, dir, glm::vec3(0));

}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h) {
    m_camera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir) {
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount) {
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount) {
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount) {
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount) {
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount) {
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount) {
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees) {
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees) {
    Entity::rotateOnRightLocal(degrees);
    m_camera.rotateOnRightLocal(degrees);
}
void Player::rotateOnUpLocal(float degrees) {
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);
}
void Player::rotateOnForwardGlobal(float degrees) {
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees) {
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees) {
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const {
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const {
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const {
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const {
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}


bool Player::isInventorySlotEmpty(int slot) {
    auto it = m_inventorySlots.find(slot);
    if (it != m_inventorySlots.end()) {
        return true;
    }
    return false;
}

bool Player::isCraftingSlotEmpty(int slot) {
    auto it = m_craftingSlots.find(slot);
    if (it != m_craftingSlots.end()) {
        return true;
    }
    return false;
}

BlockType Player::getBlockAtSlot(int slot) {
    if (m_inventorySlots.find(slot) != m_inventorySlots.end() &&
        m_inventorySlots[slot].second > 0) {
        return m_inventorySlots[slot].first;
    }
    return EMPTY;
}

BlockType Player::getBlockAtSlotCrafting(int slot) {
    if (m_craftingSlots.find(slot) != m_craftingSlots.end() &&
        m_craftingSlots[slot].second > 0) {
        return m_craftingSlots[slot].first;
    }
    return EMPTY;
}


void Player::addBlockToInventorySlot(BlockType block) {
    for (auto& pair : m_inventorySlots) {
        if (pair.second.first == block) {
            pair.second.second++;
            return;
        }
    }

    for (int i = 1; i <= 27; i++) {
        if (m_inventorySlots.find(i) == m_inventorySlots.end()) {
            m_inventorySlots[i] = std::make_pair(block, 1);
            return;
        }
    }
}

void Player::addBlockToCraftingSlot(BlockType block) {
    if (m_craftingSlots[8].first != EMPTY) {
        return;
    }

    for (int i = 0; i <= 8; i++) {
        if (m_craftingSlots[i].first == EMPTY) {
            m_craftingSlots[i] = std::make_pair(block, 1);
            return;
        }
    }
}

void Player::removeBlockFromInventorySlot(int slot) {
    m_inventorySlots[slot].second--;

    if (m_inventorySlots[slot].second == 1) {
        m_inventorySlots.erase(slot);
    }
}

void Player::removeBlockFromInventoryBlock(BlockType block) {
    for (auto& pair : m_inventorySlots) {
        if (pair.second.first == block) {
            if (m_inventorySlots[pair.first].second > 1) {
                m_inventorySlots[pair.first].second--;
                break;
            } else {
                m_inventorySlots.erase(pair.first);
                decrementSlots(pair.first);
                break;
            }
        }
    }
}

void Player::decrementSlots(int index) {
    std::map<int, std::pair<BlockType, int>> shiftedInventory;

    int newIndex = 0;
    for (const auto& pair : m_inventorySlots) {
        if (pair.first != index) {
            shiftedInventory[newIndex] = pair.second;
            newIndex++;
        }
    }

    m_inventorySlots = shiftedInventory;
}

void Player::eraseBlockFromInventorySlot(BlockType block) {
    for (auto& pair : m_inventorySlots) {
        if (pair.second.first == block) {
            m_inventorySlots.erase(pair.first);
            return;
        }
    }
}

void Player::removeBlockFromCraftingSlot(int slot) {
        m_craftingSlots.erase(slot);
}

void Player::eraseCrafting() {
    m_craftingSlots.clear();
}

void Player::addBlockSlot(BlockType block, int slot) {
    if (m_inventorySlots.count(slot) > 0 && m_inventorySlots[slot].first == block) {
        m_inventorySlots[slot].second++;
    } else {

        m_inventorySlots[slot] = std::make_pair(block, 1);
    }
}

void Player::updateQuantityOfBlock(BlockType block, int quantity) {
    for (auto& pair : m_inventorySlots) {
        if (pair.second.first == block) {
            m_inventorySlots[pair.first].second = quantity;
            return;
        }
    }
}

void Player::addBlockCraftingSlot(BlockType block, int slot) {
    if (m_craftingSlots.count(slot) > 0 && m_craftingSlots[slot].first == block) {
        m_craftingSlots[slot].second++;
    } else {

        m_craftingSlots[slot] = std::make_pair(block, 1);
    }
}

void Player::returnCraftingToInventory() {
    for (auto it = m_craftingSlots.begin(); it != m_craftingSlots.end();) {
        BlockType block = it->second.first;
        int quantity = it->second.second;

        addBlockToInventorySlot(block);
        updateQuantityOfBlock(block, quantity);

        it = m_craftingSlots.erase(it);
    }
}

std::map<int, std::pair<BlockType, int>> Player::getInventory() {
    return m_inventorySlots;
}

std::map<int, std::pair<BlockType, int>> Player::getCraftingInventory() {
    return m_craftingSlots;
}


bool Player::getIsOnGround() {
    return isOnGround;
}

glm::vec3 Player::getVelocity() {
    return m_velocity;
}
