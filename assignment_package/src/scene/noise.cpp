#include "noise.h"


float Noise::getTweakedPerlinNoise(glm::vec2 xz, float offset, float scale) {

    return (perlinNoise((xz + glm::vec2(offset))/scale)+1.f)/2.f;

}

float Noise::getHeightMapNoise(glm::vec2 xz) {

    float offset = 0.5;
    float scale = 32.f;
    float fbm_amplitute_modifier = 0.5f;
    float fbm_frequency_modifier = 2.0f;
    int fbm_octaves = 3;
    float height = fbm( ((xz+glm::vec2(offset))/scale), fbm_octaves, fbm_amplitute_modifier, fbm_frequency_modifier);

    return (height+1.f)/2.f;

}


float Noise::random(glm::vec2 xz) {
    return glm::fract(sin(glm::dot(xz,
                                   glm::vec2(12.9898,78.233))) *
                      43758.5453123);
}

float Noise::noiseFunc(glm::vec2 xz) {
    return glm::fract(sin(glm::dot(xz, glm::vec2(12.9898, 78.233))) * 43758.5453);
}

float Noise::fbm(glm::vec2 xz, int oct, float amp_mod, float freq_mod) {

    float amp = 1.0f;
    float freq = 1.0f;
    float value = 0.0f;

    for(int i = 0; i < oct; i++) {
        value += amp * perlinNoise(freq * xz);
        amp *= amp_mod;
        freq *= freq_mod;
    }

    return value;
}

float Noise::fbm2(glm::vec2 xz) {
    int octaves = 4;
    float amplitude = 20.0f;
    float frequency = 6.f;
    float value = 0.0;

    for(int i = 0; i < octaves; i++) {
        value += amplitude * perlinNoise(frequency * xz);
        amplitude *= 0.4;
        frequency *= 2.0;
    }

    value = glm::clamp(value / 2.0f, 0.0f, 3.0f);
    return value;
}

float Noise::perlinNoise(glm::vec2 xz) {

    float surfletSum = 0.0f;

    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = 0; dy <= 1; ++dy) {
            surfletSum += surflet(xz, glm::floor(xz) + glm::vec2(dx, dy));
        }
    }

    return surfletSum * 2.f;
}


float Noise::surflet(glm::vec2 p, glm::vec2 gridPoint) {
    float distX = abs(p.x - gridPoint.x);
    float distY = abs(p.y - gridPoint.y);
    float tX = 1 - 6 * pow(distX, 5.f) + 15 * pow(distX, 4.f) - 10 * pow(distX, 3.f);
    float tY = 1 - 6 * pow(distY, 5.f) + 15 * pow(distY, 4.f) - 10 * pow(distY, 3.f);

    glm::vec2 gradient = 2.f * random(gridPoint) - glm::vec2(1.f);
    glm::vec2 diff = p - gridPoint;
    float height = glm::dot(diff, gradient);

    return height * tX * tY;
}

float Noise::grasslandHeight(glm::vec2 xz) {
    float h = 0;

    float amp = 0.5;
    float scale = 512;

    for (int i = 0; i < 8; ++i) {
        glm::vec2 offset = glm::vec2(fbm2(xz / 256.f), fbm2(xz / 300.f) + 1000.f);
        float h1 = perlinNoise((xz + offset * 75.f) / scale);
        h1 = 0.5 * (h1 + 1.0);
        h += h1 * amp;

        amp *= 0.5;
        scale *= 0.5;

    }

    h = (glm::smoothstep(0.5f, 0.6f, h) * 0.9 + (0.1f * h));
    h = glm::floor(130 + h * 50) / 4.0f * 4.0f;
    return h;
}

float Noise::mountainHeight(glm::vec2 xz) {
    float h = 0;
    float amp = 0.5;
    float scale = 512;

    for (int i = 0; i < 8; ++i) {
        glm::vec2 offset = glm::vec2(fbm2(xz / 128.f), fbm2(xz / 300.f) + 1000.f);
        float h1 = perlinNoise((xz + offset * 75.f) / scale);
        h1 = 0.5 * (h1 + 1.0);
        h += h1 * amp;
        amp *= 0.5;
        scale *= 0.5;
    }

    h = (glm::smoothstep(0.5f, 0.6f, h) * 0.9 + (0.1f * h));
    h = glm::floor(128 + h * 250);
    h = glm::floor(h * 8.0f) / 8.0f;
    float W = 2.f;
    float k = glm::floor(h / W);
    float f = (h - k*W) / W;
    float s = glm::smoothstep(0.2f, 0.8f, f);
    return (k+s) * W;
    return h;
}

float Noise::biomeBlender(glm::vec2 xz) {
    float largeScale = perlinNoise(xz / 256.f);
    float mediumScale = perlinNoise(xz / 128.f) * 0.5f;
    float smallScale = perlinNoise(xz / 64.f) * 0.25f;

    return 0.5f * ((largeScale + mediumScale + smallScale) / 1.75f + 1.f);
}

float Noise::interpBiomes(glm::vec2 xz) {
    float grassH = grasslandHeight(xz);
    float mountainH = mountainHeight(xz);
    float t = biomeBlender(xz);
    t = glm::smoothstep(0.4f, 0.6f, t);

    float h = glm::mix(grassH, mountainH, t);

    return h;
}


float Noise::perlinNoise3D(glm::vec3 xyz) {
    float surfletSum = 0.f;

    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = 0; dy <= 1; ++dy) {
            for (int dz = 0; dz <= 1; ++dz) {
                surfletSum += surflet3D(xyz, glm::floor(xyz) + glm::vec3(dx, dy, dz));
            }
        }
    }

    return surfletSum;
}

float Noise::surflet3D(glm::vec3 p, glm::vec3 gridPoint) {
    glm::vec3 dist = glm::abs(p - gridPoint);
    glm::vec3 t = 1.f - 6.f * vec3Exp(dist, 5.f) + 15.f * vec3Exp(dist, 4.f) - 10.f * vec3Exp(dist, 3.f);

    glm::vec3 gradient = 2.f * random(glm::vec2(gridPoint.x, gridPoint.y)) - glm::vec3(1.f);
    glm::vec3 diff = p - gridPoint;
    float height = glm::dot(diff, gradient);

    return height * t.x * t.y * t.z;
}

glm::vec3 Noise::vec3Exp(glm::vec3 xyz, float exp) {
    float v1 = pow(xyz.x, exp);
    float v2 = pow(xyz.y, exp);
    float v3 = pow(xyz.z, exp);

    return glm::vec3(v1, v2, v3);
}


float Noise::getTerrainHeight(int x, int z) {
    float mountainH = Noise::mountainHeight(glm::vec2(x, z));
    float grassH = Noise::grasslandHeight(glm::vec2(x, z));
    float biomeValue = Noise::biomeBlender(glm::vec2(x, z));
    float t = glm::smoothstep(0.45f, 0.55f, biomeValue);

    float finalHeight;
    if (t < 0.35f) {
        finalHeight = grassH;
    }
    else if (t > 0.75f) {
        finalHeight = mountainH;
    }
    else {
        finalHeight = glm::mix(grassH, mountainH, t);
    }

    return finalHeight;
}
