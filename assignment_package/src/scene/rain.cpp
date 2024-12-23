#include "rain.h"
#include "noise.h"
#include <iostream>


RainSystem::RainSystem(OpenGLContext* context)
    : Drawable(context), gen(rd()), dis(0.0f, 1.0f),
    m_playerPos(0.f), m_prevPlayerPos(0.f)
{
    m_positions.clear();
    m_indices.clear();
    m_ids.clear();
}

glm::ivec2 RainSystem::getZone(const glm::vec3& pos) {
    return glm::ivec2(glm::floor(pos.x / ZONE_SIZE),
                      glm::floor(pos.z / ZONE_SIZE));
}

void RainSystem::setPlayerPosition(const glm::vec3& pos) {
    m_prevPlayerPos = m_playerPos;
    m_playerPos = pos;

    glm::ivec2 currentZone = getZone(m_playerPos);
    glm::ivec2 prevZone = getZone(m_prevPlayerPos);

    if (currentZone != prevZone) {
        createVBOdata();
    }
}

void RainSystem::createVBOdata() {
    m_positions.clear();
    m_indices.clear();
    m_ids.clear();

    float halfGrid = (GRID_SIZE * CELL_SIZE) / 2.0f;
    float startX = m_playerPos.x - halfGrid;
    float startZ = m_playerPos.z - halfGrid;

    int rainID = 0;
    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            float x = startX + (i * CELL_SIZE);
            float z = startZ + (j * CELL_SIZE);

            x += dis(gen) * CELL_SIZE;
            z += dis(gen) * CELL_SIZE;

            float y = 200.0f + dis(gen) * 50.0f;

            int baseIndex = m_positions.size();
            float dropWidth = 0.1f;
            float dropHeight = 1.0f;

            m_positions.push_back(glm::vec4(x, y, z, 1.0f));
            m_positions.push_back(glm::vec4(x - dropWidth, y - dropHeight, z, 1.0f));
            m_positions.push_back(glm::vec4(x + dropWidth, y - dropHeight, z, 1.0f));

            m_ids.push_back(rainID);
            m_ids.push_back(rainID);
            m_ids.push_back(rainID);

            m_indices.push_back(baseIndex);
            m_indices.push_back(baseIndex + 1);
            m_indices.push_back(baseIndex + 2);

            rainID++;
        }
    }

    generateBuffer(POSITION);
    bindBuffer(POSITION);
    mp_context->glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(glm::vec4),
                             m_positions.data(), GL_STATIC_DRAW);

    generateBuffer(RAINID);
    bindBuffer(RAINID);
    mp_context->glBufferData(GL_ARRAY_BUFFER, m_ids.size() * sizeof(int),
                             m_ids.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX);
    bindBuffer(INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
                             m_indices.data(), GL_STATIC_DRAW);

    indexCounts[INDEX] = m_indices.size();
}


SnowSystem::SnowSystem(OpenGLContext* context)
    : Drawable(context), gen(rd()),
    dis(0.0f, 1.0f),
    m_playerPos(0.f), m_prevPlayerPos(0.f)
{
    m_positions.clear();
    m_indices.clear();
    m_ids.clear();
}


void SnowSystem::createSnowflake(float centerX, float centerY, float centerZ, int& baseIndex, int snowID) {
    float size = FLAKE_SIZE;

    m_positions.push_back(glm::vec4(centerX, centerY, centerZ, 1.0f));

    for (int i = 0; i < 5; i++) {
        float angle = (i * 72.0f) * 3.14159f / 180.0f;
        float x = centerX + size * cos(angle);
        float z = centerZ + size * sin(angle);

        m_positions.push_back(glm::vec4(x - size/4, centerY - size/4, z, 1.0f));
        m_positions.push_back(glm::vec4(x + size/4, centerY - size/4, z, 1.0f));
        m_positions.push_back(glm::vec4(x + size/4, centerY + size/4, z, 1.0f));
        m_positions.push_back(glm::vec4(x - size/4, centerY + size/4, z, 1.0f));

        int squareBase = baseIndex + 1 + (i * 4);
        m_indices.push_back(baseIndex);
        m_indices.push_back(squareBase);
        m_indices.push_back(squareBase + 1);

        m_indices.push_back(squareBase + 1);
        m_indices.push_back(squareBase + 2);
        m_indices.push_back(squareBase + 3);

        m_indices.push_back(squareBase + 3);
        m_indices.push_back(squareBase);
        m_indices.push_back(baseIndex);
    }

    for (int i = 0; i < 21; i++) {
        m_ids.push_back(snowID);
    }

    baseIndex += 21;
}

void SnowSystem::createVBOdata() {
    m_positions.clear();
    m_indices.clear();
    m_ids.clear();

    float halfGrid = (GRID_SIZE * CELL_SIZE) / 2.0f;
    float startX = m_playerPos.x - halfGrid;
    float startZ = m_playerPos.z - halfGrid;

    int snowID = 0;
    int baseIndex = 0;

    for(int i = 0; i < GRID_SIZE; ++i) {
        for(int j = 0; j < GRID_SIZE; ++j) {
            float x = startX + (i * CELL_SIZE) + dis(gen) * CELL_SIZE;
            float z = startZ + (j * CELL_SIZE) + dis(gen) * CELL_SIZE;
            float y = 200.0f + dis(gen) * 50.0f;

            createSnowflake(x, y, z, baseIndex, snowID);
            snowID++;
        }
    }

    generateBuffer(POSITION);
    bindBuffer(POSITION);
    mp_context->glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(glm::vec4),
                             m_positions.data(), GL_STATIC_DRAW);

    generateBuffer(RAINID);
    bindBuffer(RAINID);
    mp_context->glBufferData(GL_ARRAY_BUFFER, m_ids.size() * sizeof(int),
                             m_ids.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX);
    bindBuffer(INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
                             m_indices.data(), GL_STATIC_DRAW);

    indexCounts[INDEX] = m_indices.size();
}



glm::ivec2 SnowSystem::getZone(const glm::vec3& pos) {
    return glm::ivec2(glm::floor(pos.x / ZONE_SIZE),
                      glm::floor(pos.z / ZONE_SIZE));
}

void SnowSystem::setPlayerPosition(const glm::vec3& pos) {
    m_prevPlayerPos = m_playerPos;
    m_playerPos = pos;

    glm::ivec2 currentZone = getZone(m_playerPos);
    glm::ivec2 prevZone = getZone(m_prevPlayerPos);

    if (currentZone != prevZone) {
        createVBOdata();
    }
}
