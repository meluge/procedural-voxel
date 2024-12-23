#pragma once
#include "drawable.h"
#include <glm_includes.h>
#include <vector>
#include "noise.h"
#include <random>


class RainSystem : public Drawable {
private:
    const int GRID_SIZE = 300;
    const float CELL_SIZE = 1.0f;
    const float DROP_SIZE = 0.5f;
    const float ZONE_SIZE = 64.0f;

    std::vector<glm::vec4> m_positions;
    std::vector<GLuint> m_indices;
    std::vector<int> m_ids;
    glm::vec3 m_playerPos;
    glm::vec3 m_prevPlayerPos;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

    glm::ivec2 getZone(const glm::vec3& pos);

public:
    RainSystem(OpenGLContext* context);
    virtual ~RainSystem() = default;

    void setPlayerPosition(const glm::vec3& pos);
    void createVBOdata() override;
};

class SnowSystem : public Drawable {
private:
    const int GRID_SIZE = 200;
    const float CELL_SIZE = 1.5f;
    const float FLAKE_SIZE = 0.3f;
    const float ZONE_SIZE = 64.0f;

    std::vector<glm::vec4> m_positions;
    std::vector<GLuint> m_indices;
    std::vector<int> m_ids;
    std::vector<float> m_rotationSpeeds;

    glm::vec3 m_playerPos;
    glm::vec3 m_prevPlayerPos;

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;

    glm::ivec2 getZone(const glm::vec3& pos);
    void createSnowflake(float centerX, float centerY, float centerZ, int& baseIndex, int snowID);

public:
    SnowSystem(OpenGLContext* context);
    virtual ~SnowSystem() = default;

    void setPlayerPosition(const glm::vec3& pos);
    void createVBOdata() override;
};
