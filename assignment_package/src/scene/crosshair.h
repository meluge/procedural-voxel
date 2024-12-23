#pragma once

#include "drawable.h"
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include "drawable.h"
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Crosshair : public Drawable {
public:
    Crosshair(OpenGLContext* context);
    void createVBOdata() override;
    void setScreenDimensions(int w, int h);
    GLenum drawMode() override;
    void updateColor(OpenGLContext* context);
private:
    int screenWidth, screenHeight;
    glm::vec4 crosshairColor;
};
