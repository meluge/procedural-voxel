#include "crosshair.h"

Crosshair::Crosshair(OpenGLContext* context)
    : Drawable(context), screenWidth(1920), screenHeight(1080), crosshairColor(glm::vec4(1,1,1,1))
{}

void Crosshair::setScreenDimensions(int w, int h) {
    screenWidth = w;
    screenHeight = h;
    createVBOdata();
}

GLenum Crosshair::drawMode()
{
    return GL_TRIANGLES;
}

void Crosshair::createVBOdata() {
    float crosshairSize = 20.0f;
    float thickness = 2.0f;

    float sizeX = (crosshairSize / screenWidth) * 2.0f;
    float sizeY = (crosshairSize / screenHeight) * 2.0f;
    float thickX = (thickness / screenWidth) * 2.0f;
    float thickY = (thickness / screenHeight) * 2.0f;

    std::vector<glm::vec4> vertPos = {
        glm::vec4(-thickX/2, -sizeY/2, 0, 1),
        glm::vec4(thickX/2, -sizeY/2, 0, 1),
        glm::vec4(thickX/2, sizeY/2, 0, 1),
        glm::vec4(-thickX/2, sizeY/2, 0, 1),

        glm::vec4(-sizeX/2, -thickY/2, 0, 1),
        glm::vec4(sizeX/2, -thickY/2, 0, 1),
        glm::vec4(sizeX/2, thickY/2, 0, 1),
        glm::vec4(-sizeX/2, thickY/2, 0, 1)
    };

    std::vector<glm::vec4> vertCol(8, glm::vec4(1, 1, 1, 1));

    std::vector<GLuint> idx = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7
    };


    generateBuffer(POSITION);
    bindBuffer(POSITION);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vertPos.size() * sizeof(glm::vec4), vertPos.data(), GL_STATIC_DRAW);

    generateBuffer(COLOR);
    bindBuffer(COLOR);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vertCol.size() * sizeof(glm::vec4), vertCol.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX);
    bindBuffer(INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    indexCounts[INDEX] = 12;



}

void Crosshair::updateColor(OpenGLContext* context) {
    GLubyte pixel[4];
    context->glReadPixels(screenWidth/2, screenHeight/2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

    crosshairColor = glm::vec4(
        1.0f - (pixel[0] / 255.0f),
        1.0f - (pixel[1] / 255.0f),
        1.0f - (pixel[2] / 255.0f),
        1.0f
        );

    float crosshairSize = 20.0f;
    float thickness = 2.0f;
    float sizeX = (crosshairSize / screenWidth) * 2.0f;
    float sizeY = (crosshairSize / screenHeight) * 2.0f;
    float thickX = (thickness / screenWidth) * 2.0f;
    float thickY = (thickness / screenHeight) * 2.0f;

    std::vector<glm::vec4> vertPos = {
        glm::vec4(-thickX/2, -sizeY/2, 0, 1),
        glm::vec4(thickX/2, -sizeY/2, 0, 1),
        glm::vec4(thickX/2, sizeY/2, 0, 1),
        glm::vec4(-thickX/2, sizeY/2, 0, 1),
        glm::vec4(-sizeX/2, -thickY/2, 0, 1),
        glm::vec4(sizeX/2, -thickY/2, 0, 1),
        glm::vec4(sizeX/2, thickY/2, 0, 1),
        glm::vec4(-sizeX/2, thickY/2, 0, 1)
    };

    std::vector<glm::vec4> vertCol(8, crosshairColor);

    std::vector<GLuint> idx = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7
    };

    generateBuffer(POSITION);
    bindBuffer(POSITION);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vertPos.size() * sizeof(glm::vec4), vertPos.data(), GL_STATIC_DRAW);

    generateBuffer(COLOR);
    bindBuffer(COLOR);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vertCol.size() * sizeof(glm::vec4), vertCol.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX);
    bindBuffer(INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    indexCounts[INDEX] = 12;
}
