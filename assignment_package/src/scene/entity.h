#pragma once
#include "glm_includes.h"

struct InputBundle {
    bool wPressed, aPressed, sPressed, dPressed;
    bool ePressed, qPressed;
    bool spacePressed;
    float mouseX, mouseY;

    InputBundle()
        : wPressed(false), aPressed(false), sPressed(false),
        dPressed(false), ePressed(false), qPressed(false), spacePressed(false), mouseX(0.f), mouseY(0.f)
    {}
};

class Entity {
protected:
    // Vectors that define the axes of our local coordinate system
    glm::vec3 m_forward, m_right, m_up;
    // The origin of our local coordinate system
    glm::vec3 m_position;
    //check if entity is in flight mode
    bool flightMode;
    //check if entity is on the ground rn
    bool isOnGround;

public:
    // A readonly reference to position for external use
    const glm::vec3& mcr_position;
    glm::vec3 prev_pos;

    // Various constructors
    Entity();
    Entity(glm::vec3 pos);
    Entity(const Entity &e);


    virtual ~Entity();

    // To be called by MyGL::tick()
    virtual void tick(float dT, InputBundle &input) = 0;

    // Translate along the given vector
    virtual void moveAlongVector(glm::vec3 dir);

    // Translate along one of our local axes
    virtual void moveForwardLocal(float amount);
    virtual void moveRightLocal(float amount);
    virtual void moveUpLocal(float amount);

    // Translate along one of the world axes
    virtual void moveForwardGlobal(float amount);
    virtual void moveRightGlobal(float amount);
    virtual void moveUpGlobal(float amount);

    // Rotate about one of our local axes
    virtual void rotateOnForwardLocal(float degrees);
    virtual void rotateOnRightLocal(float degrees);
    virtual void rotateOnUpLocal(float degrees);

    // Rotate about one of the world axes
    virtual void rotateOnForwardGlobal(float degrees);
    virtual void rotateOnRightGlobal(float degrees);
    virtual void rotateOnUpGlobal(float degrees);

    glm::vec3 getR();
    glm::vec3 getF();
    glm::vec3 getU();

};
