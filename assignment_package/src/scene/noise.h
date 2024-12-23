#pragma once

#include "smartpointerhelp.h"
#include "glm_includes.h"
#include "chunk.h"
#include <array>
#include <unordered_map>
#include <unordered_set>
#include "shaderprogram.h"



class Noise
{
public:

    static float perlinNoise(glm::vec2 xz);
    static float surflet(glm::vec2 p, glm::vec2 gridPoint);
    static float random(glm::vec2 xz);
    static float noiseFunc(glm::vec2 xz);
    static float fbm2(glm::vec2 xz);
    static float grasslandHeight(glm::vec2 xz);
    static float mountainHeight(glm::vec2 xz);
    static float biomeBlender(glm::vec2 xz);
    static float interpBiomes(glm::vec2 xz);
    static float perlinNoise3D(glm::vec3 xyz);
    static glm::vec3 vec3Exp(glm::vec3 xyz, float exp);
    static float surflet3D(glm::vec3 p, glm::vec3 gridPoint);

    float getTerrainHeight(int x, int z);

    static float getHeightMapNoise(glm::vec2 xz);
    static float fbm(glm::vec2 xz, int oct, float amp_mod, float freq_mod);
    static float getTweakedPerlinNoise(glm::vec2 xz, float offset, float scale);
};
