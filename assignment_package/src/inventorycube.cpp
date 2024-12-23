#include "inventorycube.h"

InventoryCube::InventoryCube(OpenGLContext* context) : Drawable(context) {}

InventoryCube::~InventoryCube() {}

void InventoryCube::createVBOdata() {
    std::vector<glm::vec4> positions {
        glm::vec4(-0.5f, -0.5f, 0.5f, 1.f),
        glm::vec4(0.5f, -0.5f, 0.5f, 1.f),
        glm::vec4(0.5f, 0.5f, 0.5f, 1.f),
        glm::vec4(-0.5f, 0.5f, 0.5f, 1.f),
        // Top face
        glm::vec4(-0.5f, 0.5f, 0.5f, 1.f),
        glm::vec4(0.5f, 0.5f, 0.5f, 1.f),
        glm::vec4(0.5f, 0.5f, -0.5f, 1.f),
        glm::vec4(-0.5f, 0.5f, -0.5f, 1.f),
        // Right face
        glm::vec4(0.5f, -0.5f, 0.5f, 1.f),
        glm::vec4(0.5f, 0.5f, 0.5f, 1.f),
        glm::vec4(0.5f, 0.5f, -0.5f, 1.f),
        glm::vec4(0.5f, -0.5f, -0.5f, 1.f)
    };

    std::vector<glm::vec2> uvs  (12, glm::vec2(0.f, 0.f));

    std::vector<GLuint> glIndex {

        0, 1, 2,
        0, 2, 3,

        4, 5, 6,
        4, 6, 7,

        8, 9, 10,
        8, 10, 11
    };

    generateBuffer(BufferType::POSITION);
    bindBuffer(BufferType::POSITION);
    mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    generateBuffer(BufferType::UV);
    bindBuffer(BufferType::UV);
    mp_context->glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

    generateBuffer(BufferType::INDEX);
    indexCounts[BufferType::INDEX] = 18;
    bindBuffer(BufferType::INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndex.size() * sizeof(GLuint), glIndex.data(), GL_STATIC_DRAW);
}

void InventoryCube::initializeAndBufferGeometryData() {
    createVBOdata();
}

void InventoryCube::updateUVs(const std::vector<glm::vec2>& frontUVs,
                              const std::vector<glm::vec2>& topUVs,
                              const std::vector<glm::vec2>& rightUVs) {
    uvs.clear();
    uvs.insert(uvs.end(), frontUVs.begin(), frontUVs.end());
    uvs.insert(uvs.end(), topUVs.begin(), topUVs.end());
    uvs.insert(uvs.end(), rightUVs.begin(), rightUVs.end());

    bindBuffer(UV);
    mp_context->glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
}
