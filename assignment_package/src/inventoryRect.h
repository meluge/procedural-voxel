#pragma once

#include "drawable.h"

class InventoryRectangle : public Drawable {
public:
    InventoryRectangle(OpenGLContext* context);
    virtual ~InventoryRectangle();
    void createVBOdata() override;
    void initializeAndBufferGeometryData();
    void updateUVs(const std::vector<glm::vec2>& newUVs);
private:
    int m_numVertices;
    std::vector<glm::vec4> positions;
    std::vector<glm::vec2> uvs;
};
