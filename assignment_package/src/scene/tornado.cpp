#include "tornado.h"
#include <iostream>

TornadoSystem::TornadoSystem(OpenGLContext* context, Terrain* terrain)
    : Drawable(context), m_terrain(terrain)
{}


void TornadoSystem::loadMoonMesh(const QString& objFilePath) {
    QFile file(objFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        std::cout << "Failed to open file" << std::endl;
        return;
    }

    m_moonMesh.positions.clear();
    m_moonMesh.indices.clear();

    QTextStream in(&file);
    std::vector<glm::vec4> tempVertices;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList tokens = line.split(" ", Qt::SkipEmptyParts);

        if (tokens.isEmpty()) continue;

        if (tokens[0] == "v") {
            if (tokens.size() >= 4) {
                glm::vec4 pos(
                    tokens[1].toFloat(),
                    tokens[2].toFloat(),
                    tokens[3].toFloat(),
                    1.0f
                    );
                tempVertices.push_back(pos);
            }
        }
        else if (tokens[0] == "f") {
            std::vector<int> faceIndices;

            for (int i = 1; i < tokens.size(); i++) {
                QString vertexData = tokens[i];
                QStringList vertexIndices = vertexData.split("/");
                int vertexIndex = vertexIndices[0].toInt() - 1;
                faceIndices.push_back(vertexIndex);
            }

            for (size_t i = 0; i < faceIndices.size() - 2; i++) {
                m_moonMesh.positions.push_back(tempVertices[faceIndices[0]]);
                m_moonMesh.positions.push_back(tempVertices[faceIndices[i + 1]]);
                m_moonMesh.positions.push_back(tempVertices[faceIndices[i + 2]]);

                int baseIndex = m_moonMesh.indices.size();
                m_moonMesh.indices.push_back(baseIndex);
                m_moonMesh.indices.push_back(baseIndex + 1);
                m_moonMesh.indices.push_back(baseIndex + 2);
            }
        }
    }

    file.close();


}

void TornadoSystem::createVBOdata() {

    m_positions.clear();
    m_indices.clear();
    m_ids.clear();

    const float MESH_SCALE = 3.0f;

    for(int level = 0; level < NUM_LEVELS; level++) {
        float heightPercent = float(level) / float(NUM_LEVELS);
        float radius = TORNADO_BASE_RADIUS * (0.2f + heightPercent * 0.8f);

        for(int moon = 0; moon < MOONS_PER_LEVEL; moon++) {
            float angle = (float(moon) / float(MOONS_PER_LEVEL)) * 2.0f * M_PI;

            glm::vec3 pos(
                m_center.x + radius * cos(angle),
                m_center.y + (heightPercent * TORNADO_HEIGHT - TORNADO_HEIGHT/2),
                m_center.z + radius * sin(angle)
                );

            glm::vec3 toCenter = glm::normalize(glm::vec3(m_center.x, pos.y, m_center.z) - pos);
            glm::vec3 up = glm::vec3(0, 1, 0);
            glm::vec3 right = glm::normalize(glm::cross(toCenter, up));
            glm::mat4 rotation = glm::mat4(
                glm::vec4(right, 0),
                glm::vec4(up, 0),
                glm::vec4(toCenter, 0),
                glm::vec4(0, 0, 0, 1)
                );

            float levelRotation = heightPercent * 4.0f * M_PI;
            rotation = glm::rotate(rotation, levelRotation, up);

            glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) *
                                  rotation *
                                  glm::scale(glm::mat4(1.0f), glm::vec3(MESH_SCALE));

            int baseIndex = m_positions.size();
            for(const auto& meshPos : m_moonMesh.positions) {
                glm::vec4 transformedPos = transform * meshPos;
                m_positions.push_back(transformedPos);
                m_ids.push_back(level * MOONS_PER_LEVEL + moon);
            }

            for(const auto& idx : m_moonMesh.indices) {
                m_indices.push_back(baseIndex + idx);
            }
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

void TornadoSystem::setCenter(const glm::vec3& center) {
    m_center = center;
}
