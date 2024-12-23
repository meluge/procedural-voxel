#pragma once

#include "drawable.h"


#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class SkyQuad : public Drawable
{
public:
    SkyQuad(OpenGLContext* context);
    void createVBOdata() override;
};
