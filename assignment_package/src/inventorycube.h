#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "drawable.h"

class InventoryCube : public Drawable {
public:
    InventoryCube(OpenGLContext* context);
    virtual ~InventoryCube();

    virtual void createVBOdata() override;
    void initializeAndBufferGeometryData();
    void updateUVs(const std::vector<glm::vec2>& frontUVs,
                   const std::vector<glm::vec2>& topUVs,
                   const std::vector<glm::vec2>& rightUVs);

private:
    std::vector<glm::vec2> uvs;
};
