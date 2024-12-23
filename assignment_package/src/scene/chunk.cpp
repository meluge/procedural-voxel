#include "chunk.h"
#include <iostream>
#include "noise.h"
#include "cpuslave.h"

BiomeType Chunk::getBiomeFromTemperatureAndHumidity(float t, float h) {

    if (t < tthreshlow) {
        if (h < hthreshlow) {
            return SNOWY;
        } else if (h < hthreshhigh) {
            return PLAIN;
        } else {
            return COLDOCEAN;
        }
    } else if (t < tthreshhigh) {
        if (h < hthreshlow) {
            return RIVERS;
        } else if (h < hthreshhigh) {
            return FOREST;
        } else {
            return OCEAN;
        }
    } else {
        if (h < hthreshlow) {
            return DESERT;
        } else if (h < hthreshhigh) {
            return RAINFOREST;
        } else {
            return WARMOCEAN;
        }
    }

    /*
    if (t < tthreshlow) {
        if (h < hthreshlow) {
            return SNOWY;
        } else if (h < hthreshhigh) {
            return FOREST;
        } else {
            return COLDOCEAN;
        }
    } else if (t < tthreshhigh) {
        if (h < hthreshlow) {
            return PLAIN;
        } else if (h < hthreshhigh) {
            return RIVERS;
        } else {
            return OCEAN;
        }
    } else {
        if (h < hthreshlow) {
            return DESERT;
        } else if (h < hthreshhigh) {
            return FLOWERS;
        } else {
            return WARMOCEAN;
        }
    }*/

}
std::vector<SplineControlPoint> Chunk::getSplineFromBiome(BiomeType b) {
    std::vector<SplineControlPoint> spline;

    switch (b) {
    case PLAIN:
        spline.push_back({0.0f, 70.f});
        spline.push_back({0.3f, 75.f});
        spline.push_back({0.7f, 80.f});
        spline.push_back({1.0f, 82.f});
        break;

    case FOREST:
        spline.push_back({0.0f, 70.f});
        spline.push_back({0.3f, 75.f});
        spline.push_back({0.5f, 75.f});
        spline.push_back({0.7f, 80.f});
        spline.push_back({1.0f, 82.f});
        break;

    case RAINFOREST:
        spline.push_back({0.0f, 65.f});
        spline.push_back({0.1f, 65.f});
        spline.push_back({0.2f, 70.f});
        spline.push_back({0.5f, 72.f});
        spline.push_back({0.7f, 75.f});
        spline.push_back({0.8f, 80.f});
        spline.push_back({1.0f, 82.f});
        break;

    case RIVERS:
        spline.push_back({0.0f, 72.f});
        spline.push_back({0.2f, 73.f});
        spline.push_back({0.5f, 70.f});
        spline.push_back({0.6f, 65.f});
        spline.push_back({0.7f, 70.f});
        spline.push_back({0.85f, 73.f});
        spline.push_back({1.0f, 72.f});
        break;

    case DESERT:
        spline.push_back({0.0f, 70.f});
        spline.push_back({0.5f, 75.f});
        spline.push_back({0.8f, 78.f});
        spline.push_back({0.9f, 90.f});
        spline.push_back({1.0f, 94.f});
        break;

    case SNOWY:
        spline.push_back({0.0f, 70.f});
        spline.push_back({0.3f, 80.f});
        spline.push_back({0.7f, 90.f});
        spline.push_back({0.8f, 110.f});
        spline.push_back({1.0f, 140.f});
        break;

    case OCEAN:
        spline.push_back({0.0f, 45.f});
        spline.push_back({0.6f, 50.f});
        spline.push_back({0.8f, 62.f});
        spline.push_back({1.0f, 65.f});
        break;

    case COLDOCEAN:
        spline.push_back({0.0f, 45.f});
        spline.push_back({0.3f, 50.f});
        spline.push_back({0.6f, 59.f});
        spline.push_back({0.9f, 64.f});
        spline.push_back({1.0f, 65.f});
        break;

    case WARMOCEAN:
        spline.push_back({0.0f, 56.f});
        spline.push_back({0.6f, 58.f});
        spline.push_back({0.7f, 64.f});
        spline.push_back({0.8f, 65.f});
        spline.push_back({1.0f, 80.f});
        break;

    default:
        // Default spline for unknown biomes
        spline.push_back({0.0f, 70.f});
        spline.push_back({0.5f, 80.f});
        spline.push_back({1.0f, 90.f});
        break;
    }

    return spline;
}

float Chunk::splineLerp(float x, SplineControlPoint p1, SplineControlPoint p2) {
    return p1.y + (x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
}
float Chunk::getSplineOutput(float x, const std::vector<SplineControlPoint>& spline) {

    //clamp x
    x = glm::clamp(x, 0.0f, 1.0f);

    for (size_t i = 0; i < spline.size() - 1; ++i) {
        if (x >= spline[i].x && x <= spline[i + 1].x) {
            return splineLerp(x, spline[i], spline[i + 1]);
        }
    }

    //default values
    return spline.back().y;
}

float Chunk::getBlendedHeightFromTemperatureAndHumidity(BiomeType biome, float height, float t, float h)
{

    float tblendthresh = 0.05f;
    float tblendfactor = 0.50f;
    float tblendposition = 0.00f;
    float hblendthresh = 0.05f;
    float hblendfactor = 0.50f;
    float hblendposition = 0.00f;
    //first, check if is with in threasholds
    if (glm::abs(t-tthreshlow) < tblendthresh) {
        tblendfactor = (t-(tthreshlow - tblendthresh)) / (tblendthresh * 2.f);
        tblendposition = tthreshlow;
    }
    if (glm::abs(t-tthreshhigh) < tblendthresh) {
        tblendfactor = (t-(tthreshhigh - tblendthresh)) / (tblendthresh * 2.f);
        tblendposition = tthreshhigh;
    }
    if (glm::abs(h-hthreshlow) < hblendthresh) {
        hblendfactor = (h-(hthreshlow - hblendthresh)) / (hblendthresh * 2.f);
        hblendposition = hthreshlow;
    }
    if (glm::abs(h-hthreshhigh) < hblendthresh) {
        hblendfactor = (h-(hthreshhigh - hblendthresh)) / (hblendthresh * 2.f);
        hblendposition = hthreshhigh;
    }

    if (tblendposition == 0.0f && hblendposition == 0.0f) {
        return Chunk::getSplineOutput(height, Chunk::getSplineFromBiome(biome));
    } else {
        if (tblendposition == 0.0f) tblendposition = t;
        if (hblendposition == 0.0f) hblendposition = h;
        float p1 = Chunk::getSplineOutput(height, Chunk::getSplineFromBiome(getBiomeFromTemperatureAndHumidity(tblendposition - tblendthresh, hblendposition - hblendthresh)));
        float p2 = Chunk::getSplineOutput(height, Chunk::getSplineFromBiome(getBiomeFromTemperatureAndHumidity(tblendposition + tblendthresh, hblendposition - hblendthresh)));
        float p3 = Chunk::getSplineOutput(height, Chunk::getSplineFromBiome(getBiomeFromTemperatureAndHumidity(tblendposition - tblendthresh, hblendposition + hblendthresh)));
        float p4 = Chunk::getSplineOutput(height, Chunk::getSplineFromBiome(getBiomeFromTemperatureAndHumidity(tblendposition + tblendthresh, hblendposition + hblendthresh)));
        float p12 = p2 * tblendfactor + p1 * (1.f - tblendfactor);
        float p34 = p4 * tblendfactor + p3 * (1.f - tblendfactor);
        float pFinal = p34 * hblendfactor + p12 * (1.f - hblendfactor);
        return pFinal;
    }

    //should be unreachable!
    return -1.f;
}

void Chunk::PaintColumn(int x, int maxY, int z, BiomeType biome) {

    int i = 0;
    BlockType block = EMPTY;
    for (int y = maxY; y > 0 && i < blockDepthThreshold; --y) {
        switch(biome) {
        case PLAIN:
            if (i == 0) {block = GRASS; break;}
            if (i < 3) {block = DIRT; break;}
            if (true) {block = STONE; break;}
        case FOREST:
            if (i == 0) {block = GRASS; break;}
            if (i < 3) {block = DIRT; break;}
            if (true) {block = STONE; break;}
        case RAINFOREST:
            if (i == 0) {block = GRASS; break;}
            if (i < 3) {block = DIRT; break;}
            if (true) {block = STONE; break;}
        case RIVERS:
            if (i == 0) {block = GRASS; break;}
            if (i < 3) {block = DIRT; break;}
            if (true) {block = STONE; break;}
        case DESERT:
            if (i == 0) {block = SAND; break;}
            if (i < 3) {block = SAND; break;}
            if (true) {block = STONE; break;}
        case SNOWY:
            if (i == 0) {block = SNOW; break;}
            if (i < (maxY-70)/10) {block = SNOW; break;}
            if (i < 2) {block = DIRT; break;}
            if (true) {block = STONE; break;}
        case OCEAN:
            if (i == 0) {block = GRAVEL; break;}
            if (i < 3) {block = GRAVEL; break;}
            if (true) {block = STONE; break;}
        case COLDOCEAN:
            if (i == 0) {block = GRAVEL; break;}
            if (i < 3) {block = GRAVEL; break;}
            if (true) {block = STONE; break;}
        case WARMOCEAN:
            if (i == 0) {block = SAND; break;}
            if (i < 3) {block = SAND; break;}
            if (true) {block = STONE; break;}
        default:
            if (i == 0) {block = GRASS; break;}
            if (i < 3) {block = DIRT; break;}
            if (true) {block = STONE; break;}
        }

        setLocalBlockAt(x, y, z, block);

        ++i;
    }

    //paint water
    if (maxY < oceanLevel) {
        if (getLocalBlockAt(x, maxY, z) == GRASS) setLocalBlockAt(x, maxY, z, DIRT);
        for (int y = maxY+1; y <= oceanLevel; ++y) {
            setLocalBlockAt(x, y, z, WATER);
        }
    }

    //paint ice
    if (biome == COLDOCEAN) {
        //float iceNoise = Noise::getTweakedPerlinNoise(glm::vec2(x,z),20.5f,40.f);
        if (maxY < 56) {

            int iceLevel = (56 - maxY);

            if (iceLevel < 5) {
                iceLevel = 1;
            } else {
                iceLevel /= 2;
            }

            for (int y = oceanLevel; y < oceanLevel + iceLevel; y++) {
                setLocalBlockAt(x, y, z, ICE);
            }
        }
    }

    //paint vegetation
    float plantNoise = Noise::getTweakedPerlinNoise(glm::vec2(x,z),0.5f,5.45f);
    if (getLocalBlockAt(x, maxY, z) == GRASS && maxY > oceanLevel && (biome == PLAIN || biome == FOREST)) {
        if (plantNoise < 0.8f && plantNoise >= 0.58f) {
            setLocalBlockAt(x, maxY+1, z, GRASSPLANT);
        } else if (plantNoise < 0.09f) {
            setLocalBlockAt(x, maxY+1, z, DANDELION);
        } else if (plantNoise < 0.5f && plantNoise >= 0.48f) {
            setLocalBlockAt(x, maxY+1, z, POPPY);
        }
    }
    if (getLocalBlockAt(x, maxY, z) == GRASS && maxY > oceanLevel && biome == RIVERS) {
        if (plantNoise < 0.8f && plantNoise >= 0.68f) {
            setLocalBlockAt(x, maxY+1, z, GRASSPLANT);
        } else if (plantNoise < 0.14f) {
            setLocalBlockAt(x, maxY+1, z, BROWNMUSHROOM);
        } else if (plantNoise < 0.5f && plantNoise >= 0.49f) {
            setLocalBlockAt(x, maxY+1, z, REDMUSHROOM);
        }
    }
    if (getLocalBlockAt(x, maxY, z) == GRASS && maxY > oceanLevel && biome == RAINFOREST) {
        if (plantNoise < 0.8f && plantNoise >= 0.62f) {
            setLocalBlockAt(x, maxY+1, z, GRASSPLANT);
        } else if (plantNoise < 0.09f) {
            setLocalBlockAt(x, maxY+1, z, POPPY);
        } else if (plantNoise < 0.51f && plantNoise >= 0.49f) {
            setLocalBlockAt(x, maxY+1, z, MELON);
        }
    }
    if (getLocalBlockAt(x, maxY, z) == SAND && maxY > oceanLevel && biome == DESERT) {
        if (plantNoise < 0.75f && plantNoise >= 0.65f) {
            setLocalBlockAt(x, maxY+1, z, DEADBUSH);
        } else if (static_cast<int>(std::floor(plantNoise * 120.f)) % 40 == 7) {
            setLocalBlockAt(x, maxY+1, z, CACTUS);
            setLocalBlockAt(x, maxY+2, z, CACTUS);
            if (plantNoise > 0.2f) setLocalBlockAt(x, maxY+3, z, CACTUS);
            if (plantNoise > 0.6f) setLocalBlockAt(x, maxY+4, z, CACTUS);
        }
    }
    if (getLocalBlockAt(x, maxY, z) == SAND && maxY < oceanLevel-1 && biome == WARMOCEAN) {
        if (plantNoise < 0.75f && plantNoise >= 0.55f) {
            setLocalBlockAt(x, maxY+1, z, CORAL);
        }
    }

    return;
}

glm::vec2 getBlockFaceTextureLocation(BlockType t, Direction dir) {
    glm::vec2 location;
    switch (t) {
    case GRASS:
        switch (dir) {
        case YPOS:
            location = glm::vec2(8, 13);
            break;
        case YNEG:
            location = glm::vec2(2, 15);
            break;
        default:
            location = glm::vec2(3, 15);
            break;
        }
        break;
    case STONE:
        location = glm::vec2(1, 15);
        break;
    case DIRT:
        location = glm::vec2(2, 15);
        break;
    case SNOW:
        location = glm::vec2(2, 11);
        break;
    case BEDROCK:
        location = glm::vec2(1, 14);
        break;
    case WATER:
        location = glm::vec2(13, 3);
        break;
    case LAVA:
        location = glm::vec2(13, 1);
        break;
    case SAND:
        location = glm::vec2(2, 14);
        break;
    case GRAVEL:
        location = glm::vec2(3, 14);
        break;
    case GRASSPLANT:
        location = glm::vec2(7, 13);
        break;
    case POPPY:
        location = glm::vec2(12, 15);
        break;
    case DANDELION:
        location = glm::vec2(13, 15);
        break;
    case CACTUS:
        switch (dir) {
        case YPOS:
            location = glm::vec2(5, 11);
            break;
        case YNEG:
            location = glm::vec2(5, 11);
            break;
        default:
            location = glm::vec2(6, 11);
            break;
        }
        break;
    case DEADBUSH:
        location = glm::vec2(7, 12);
        break;
    case OAKLOG:
        switch (dir) {
        case YPOS:
            location = glm::vec2(5, 14);
            break;
        case YNEG:
            location = glm::vec2(5, 14);
            break;
        default:
            location = glm::vec2(4, 14);
            break;
        }
        break;
    case OAKLEAVES:
        location = glm::vec2(5, 12);
        break;
    case OAKPLANKS:
        location = glm::vec2(4, 15);
        break;
    case ICE:
        location = glm::vec2(3, 11);
        break;
    case CORAL:
        location = glm::vec2(4, 1);
        break;
    case MELON:
        switch (dir) {
        case YPOS:
            location = glm::vec2(9, 7);
            break;
        case YNEG:
            location = glm::vec2(9, 7);
            break;
        default:
            location = glm::vec2(8, 7);
            break;
        }
        break;
    case REDMUSHROOM:
        location = glm::vec2(12, 14);
        break;
    case BROWNMUSHROOM:
        location = glm::vec2(13, 14);
        break;
    case SANDSTONE:
        switch (dir) {
        case YPOS:
            location = glm::vec2(0, 3);
            break;
        case YNEG:
            location = glm::vec2(0, 2);
            break;
        default:
            location = glm::vec2(0, 4);
            break;
        }
        break;
    case WOOL:
        location = glm::vec2(0, 11);
        break;
    case JUNGLELOG:
        switch (dir) {
        case YPOS:
            location = glm::vec2(5, 14);
            break;
        case YNEG:
            location = glm::vec2(5, 14);
            break;
        default:
            location = glm::vec2(9, 6);
            break;
        }
        break;
    case JUNGLELEAVES:
        location = glm::vec2(5, 12);
        break;
    default:
        location = glm::vec2(0, 0);
        break;
    }
    return location;
}

bool Chunk::isTransparent(BlockType t) {
    if (t == EMPTY || t == WATER || t == LAVA || t == OAKLEAVES || t == DEADBUSH || t == CACTUS || t == GRASSPLANT || t == DANDELION || t == POPPY || t == CORAL || t == REDMUSHROOM || t == BROWNMUSHROOM) {
        return true;
    }
    else {
        return false;
    }
}
bool Chunk::isEmpty(BlockType t) {
    if (t == EMPTY) {
        return true;
    }
    else {
        return false;
    }
}
bool Chunk::isFluid(BlockType t) {
    if (t == WATER || t == LAVA) {
        return true;
    }
    else {
        return false;
    }
}
bool Chunk::isPassable(BlockType t) {
    if (t == EMPTY || t == WATER || t == LAVA || t == GRASSPLANT || t == DEADBUSH || t == DANDELION || t == POPPY || t == BROWNMUSHROOM || t == REDMUSHROOM || t == CORAL) {
        return true;
    }
    else {
        return false;
    }
}
bool Chunk::isSelectable(BlockType t) {
    if (t == EMPTY || t == WATER || t == LAVA) {
        return false;
    }
    else {
        return true;
    }
}


Chunk::Chunk(OpenGLContext* context, int x, int z) : Drawable(context), m_blocks(), minX(x), minZ(z),
    m_neighbors{{XPOS, nullptr}, {XNEG, nullptr}, {ZPOS, nullptr}, {ZNEG, nullptr}}
{
    std::fill_n(m_blocks.begin(), 65536, EMPTY);

}


// Does bounds checking with at()
BlockType Chunk::getLocalBlockAt(unsigned int x, unsigned int y, unsigned int z) const {
    return m_blocks.at(x + 16 * y + 16 * 256 * z);
}

// Exists to get rid of compiler warnings about int -> unsigned int implicit conversion
BlockType Chunk::getLocalBlockAt(int x, int y, int z) const {
    return getLocalBlockAt(static_cast<unsigned int>(x), static_cast<unsigned int>(y), static_cast<unsigned int>(z));
}

// Does bounds checking with at()
void Chunk::setLocalBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t) {
    m_blocks.at(x + 16 * y + 16 * 256 * z) = t;
}

BiomeType Chunk::getLocalBiomeAt(unsigned int x, unsigned int z) const {
    return m_biomes.at(x + 16 * z);
}

BiomeType Chunk::getLocalBiomeAt(int x, int z) const {
    return getLocalBiomeAt(static_cast<unsigned int>(x), static_cast<unsigned int>(z));
}

void Chunk::setLocalBiomeAt(unsigned int x, unsigned int z, BiomeType b) {
    m_biomes.at(x + 16 * z) = b;
}


const static std::unordered_map<Direction, Direction, EnumHash> oppositeDirection {
    {XPOS, XNEG},
    {XNEG, XPOS},
    {YPOS, YNEG},
    {YNEG, YPOS},
    {ZPOS, ZNEG},
    {ZNEG, ZPOS}
};

void Chunk::linkNeighbor(uPtr<Chunk> &neighbor, Direction dir) {
    if(neighbor != nullptr) {
        this->m_neighbors[dir] = neighbor.get();
        neighbor->m_neighbors[oppositeDirection.at(dir)] = this;
    }
}

void Chunk::createVBOdata() {

    vboData.clear();
    vboDataIndex.clear();
    vboDataWater.clear();
    vboDataWaterIndex.clear();

    populateVBO(&vboData, &vboDataIndex, &vboDataWater, &vboDataWaterIndex, this);

}



void Chunk::bufferVBO() {
    \
        generateBuffer(INTERLEAVED);
    bindBuffer(INTERLEAVED);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(glm::vec4), vboData.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX);
    bindBuffer(INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, vboDataIndex.size() * sizeof(GLuint), vboDataIndex.data(), GL_STATIC_DRAW);

    indexCounts[INDEX] = vboDataIndex.size();

    generateBuffer(INTERLEAVED_TRANSPARENT);
    bindBuffer(INTERLEAVED_TRANSPARENT);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vboDataWater.size() * sizeof(glm::vec4), vboDataWater.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX_TRANSPARENT);
    bindBuffer(INDEX_TRANSPARENT);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, vboDataWaterIndex.size() * sizeof(GLuint), vboDataWaterIndex.data(), GL_STATIC_DRAW);

    indexCounts[INDEX_TRANSPARENT] = vboDataWaterIndex.size();


}




int Chunk::getX() {
    return minX;
}

int Chunk::getZ() {
    return minZ;
}


void Chunk::bufferVBO2(const std::vector<glm::vec4>& vboData, const std::vector<GLuint>& indexData,
                       const std::vector<glm::vec4>& vboDataWater, const std::vector<GLuint>& indexWaterData) {
    generateBuffer(INTERLEAVED);
    bindBuffer(INTERLEAVED);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vboData.size() * sizeof(glm::vec4), vboData.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX);
    bindBuffer(INDEX);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(GLuint), indexData.data(), GL_STATIC_DRAW);

    indexCounts[INDEX] = indexData.size();

    generateBuffer(INTERLEAVED_TRANSPARENT);
    bindBuffer(INTERLEAVED_TRANSPARENT);
    mp_context->glBufferData(GL_ARRAY_BUFFER, vboDataWater.size() * sizeof(glm::vec4), vboDataWater.data(), GL_STATIC_DRAW);

    generateBuffer(INDEX_TRANSPARENT);
    bindBuffer(INDEX_TRANSPARENT);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexWaterData.size() * sizeof(GLuint), indexWaterData.data(), GL_STATIC_DRAW);

    indexCounts[INDEX_TRANSPARENT] = indexWaterData.size();
}
