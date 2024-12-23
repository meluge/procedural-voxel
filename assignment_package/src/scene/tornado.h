#pragma once
#include "drawable.h"
#include <glm_includes.h>
#include <vector>
#include "terrain.h"
#include "noise.h"
#include <random>
#include "tinyobj/tiny_obj_loader.h"
#include <QString>
#include <QFile>
#include <sstream>


class TornadoSystem : public Drawable {
private:
    struct MoonMesh {
        std::vector<glm::vec4> positions;
        std::vector<GLuint> indices;
    };
    MoonMesh m_moonMesh;

    const int NUM_LEVELS = 40;
    const int MOONS_PER_LEVEL = 12;  // Number of half moons around each circle
    const float TORNADO_HEIGHT = 120.0f;
    const float TORNADO_BASE_RADIUS = 8.0f;

    glm::vec3 m_center;
    Terrain* m_terrain;

    std::vector<glm::vec4> m_positions;
    std::vector<GLuint> m_indices;
    std::vector<int> m_ids;


    // For random variation
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dis;




public:

    TornadoSystem(OpenGLContext* context, Terrain* terrain) ;
    void createVBOdata() override;
    void loadMoonMesh(const QString& objFilePath);
    void createParticlesForChunk(int chunkX, int chunkZ);

    void setCenter(const glm::vec3& center);

};
