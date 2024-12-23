#include "quad.h"

Quad::Quad(OpenGLContext* context) : Drawable(context) {}

Quad::~Quad() {}

void Quad::createVBOdata() {
    std::vector<glm::vec4> positions {
        glm::vec4(-1.f, -1.f, 0.f, 1.f),
        glm::vec4( 1.f, -1.f, 0.f, 1.f),
        glm::vec4( 1.f,  1.f, 0.f, 1.f),
        glm::vec4(-1.f,  1.f, 0.f, 1.f)
    };

    std::vector<glm::vec2> uvs {
        glm::vec2(0.f, 0.f),
        glm::vec2(1.f, 0.f),
        glm::vec2(1.f, 1.f),
        glm::vec2(0.f, 1.f)
    };

    std::vector<GLuint> glIndex {0,1,2,0,2,3};

    generateBuffer(BufferType::POSITION);
    bindBuffer(BufferType::POSITION);
    mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    generateBuffer(BufferType::UV);
    bindBuffer(BufferType::UV);
    mp_context->glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

    generateBuffer(BufferType::INDEX);
    indexCounts[BufferType::INDEX] = 6;
    bindBuffer(BufferType::INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, glIndex.size() * sizeof(GLuint), glIndex.data(), GL_STATIC_DRAW);
}


void Quad::initializeAndBufferGeometryData() {
    createVBOdata();
}

void Quad::updateUVs(const std::vector<glm::vec2>& newUVs) {
    uvs = newUVs;
    bindBuffer(UV);
    mp_context->glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
}
