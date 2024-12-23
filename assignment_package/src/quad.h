#include "drawable.h"
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

class Quad : public Drawable {
public:
    Quad(OpenGLContext* context);
    virtual ~Quad();
    void createVBOdata() override;
    void initializeAndBufferGeometryData();
    void updateUVs(const std::vector<glm::vec2>& newUVs);
private:
    int m_numVertices;
    std::vector<glm::vec4> positions;
    std::vector<glm::vec2> uvs;
};
