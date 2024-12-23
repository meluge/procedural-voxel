#include "terrain.h"
#include "cube.h"
#include <stdexcept>
#include <iostream>
#include <thread>
#include "cpuslave.h"
#include <thread>




Terrain::Terrain(OpenGLContext *context)
    : m_chunks(), m_generatedTerrain(), m_geomCube(context),
    m_chunkVBOsNeedUpdating(true), mp_context(context)
{
    for ( int i = 0; i < 8; i++) {
        blockTypeWorkerThreads.push_back(std::thread(blockTypeWorker, std::ref(dataToBlockTypeWorkerThreads),
                                                     std::ref(dataFromBlockTypeWorkerThreads), std::ref(mutex_dataToBlockTypeWorkerThreads),
                                                     std::ref(mutex_dataFromBlockTypeWorkerThreads)));
        vboWorkerThreads.push_back(std::thread(VBOWorker, std::ref(dataToVBOWorkerThreads), std::ref(dataFromVBOWorkerThreads),
                                               std::ref(mutex_dataToVBOWorkerThreads), std::ref(mutex_dataFromVBOWorkerThreads)));
    }

}

Terrain::~Terrain() {
    for (int i = 0; i < (int) m_chunks.size(); i++) {
        m_chunks.at(i)->destroyVBOdata();
    }
}

// Combine two 32-bit ints into one 64-bit int
// where the upper 32 bits are X and the lower 32 bits are Z
int64_t toKey(int x, int z) {
    int64_t xz = 0xffffffffffffffff;
    int64_t x64 = x;
    int64_t z64 = z;

    // Set all lower 32 bits to 1 so we can & with Z later
    xz = (xz & (x64 << 32)) | 0x00000000ffffffff;

    // Set all upper 32 bits to 1 so we can & with XZ
    z64 = z64 | 0xffffffff00000000;

    // Combine
    xz = xz & z64;
    return xz;
}

glm::ivec2 toCoords(int64_t k) {
    // Z is lower 32 bits
    int64_t z = k & 0x00000000ffffffff;
    // If the most significant bit of Z is 1, then it's a negative number
    // so we have to set all the upper 32 bits to 1.
    // Note the 8    V
    if(z & 0x0000000080000000) {
        z = z | 0xffffffff00000000;
    }
    int64_t x = (k >> 32);

    return glm::ivec2(x, z);
}

// Surround calls to this with try-catch if you don't know whether
// the coordinates at x, y, z have a corresponding Chunk
BlockType Terrain::getGlobalBlockAt(int x, int y, int z) const
{
    if(hasChunkAt(x, z)) {
        // Just disallow action below or above min/max height,
        // but don't crash the game over it.
        if(y < 0 || y >= 256) {
            return EMPTY;
        }
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getLocalBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                                  static_cast<unsigned int>(y),
                                  static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

BlockType Terrain::getGlobalBlockAt(glm::vec3 p) const {
    return getGlobalBlockAt(p.x, p.y, p.z);
}

BiomeType Terrain::getGlobalBiomeAt(int x, int z) const
{
    if(hasChunkAt(x, z)) {
        const uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        return c->getLocalBiomeAt(static_cast<unsigned int>(x - chunkOrigin.x),
                                  static_cast<unsigned int>(z - chunkOrigin.y));
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) + " " +
                                std::to_string(z) + " have no Biome!");
    }
}

bool Terrain::hasChunkAt(int x, int z) const {
    // Map x and z to their nearest Chunk corner
    // By flooring x and z, then multiplying by 16,
    // we clamp (x, z) to its nearest Chunk-space corner,
    // then scale back to a world-space location.
    // Note that floor() lets us handle negative numbers
    // correctly, as floor(-1 / 16.f) gives us -1, as
    // opposed to (int)(-1 / 16.f) giving us 0 (incorrect!).
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.find(toKey(16 * xFloor, 16 * zFloor)) != m_chunks.end();
}


uPtr<Chunk>& Terrain::getChunkAt(int x, int z) {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks[toKey(16 * xFloor, 16 * zFloor)];
}


const uPtr<Chunk>& Terrain::getChunkAt(int x, int z) const {
    int xFloor = static_cast<int>(glm::floor(x / 16.f));
    int zFloor = static_cast<int>(glm::floor(z / 16.f));
    return m_chunks.at(toKey(16 * xFloor, 16 * zFloor));
}

void Terrain::setGlobalBlockAt(int x, int y, int z, BlockType t)
{
    if(hasChunkAt(x, z)) {
        uPtr<Chunk> &c = getChunkAt(x, z);
        glm::vec2 chunkOrigin = glm::vec2(floor(x / 16.f) * 16, floor(z / 16.f) * 16);
        c->setLocalBlockAt(static_cast<unsigned int>(x - chunkOrigin.x),
                           static_cast<unsigned int>(y),
                           static_cast<unsigned int>(z - chunkOrigin.y),
                           t);
    }
    else {
        throw std::out_of_range("Coordinates " + std::to_string(x) +
                                " " + std::to_string(y) + " " +
                                std::to_string(z) + " have no Chunk!");
    }
}

Chunk* Terrain::instantiateChunkAt(int x, int z) {
    uPtr<Chunk> chunk = mkU<Chunk>(mp_context, x, z);
    Chunk *cPtr = chunk.get();
    m_chunks[toKey(x, z)] = std::move(chunk);
    // Set the neighbor pointers of itself and its neighbors
    if(hasChunkAt(x, z + 16)) {
        auto &chunkNorth = m_chunks[toKey(x, z + 16)];
        cPtr->linkNeighbor(chunkNorth, ZPOS);
    }
    if(hasChunkAt(x, z - 16)) {
        auto &chunkSouth = m_chunks[toKey(x, z - 16)];
        cPtr->linkNeighbor(chunkSouth, ZNEG);
    }
    if(hasChunkAt(x + 16, z)) {
        auto &chunkEast = m_chunks[toKey(x + 16, z)];
        cPtr->linkNeighbor(chunkEast, XPOS);
    }
    if(hasChunkAt(x - 16, z)) {
        auto &chunkWest = m_chunks[toKey(x - 16, z)];
        cPtr->linkNeighbor(chunkWest, XNEG);
    }

    cPtr->createVBOdata();
    cPtr->bufferVBO();

    return cPtr;
    return cPtr;
}

void Terrain::updateChunk(int playerX, int playerZ) {
    int playerChunkX = static_cast<int>(glm::floor(playerX / 16.f)) * 16;
    int playerChunkZ = static_cast<int>(glm::floor(playerZ / 16.f)) * 16;

    const int CHUNK_CHECK_RADIUS = 10;

    for(int x = -CHUNK_CHECK_RADIUS; x <= CHUNK_CHECK_RADIUS; x++) {
        for(int z = -CHUNK_CHECK_RADIUS; z <= CHUNK_CHECK_RADIUS; z++) {
            int checkX = playerChunkX + (x * 16);
            int checkZ = playerChunkZ + (z * 16);

            if(!hasChunkAt(checkX, checkZ)) {
                uPtr<Chunk> chunk = mkU<Chunk>(mp_context, checkX, checkZ);
                Chunk* cPtr = chunk.get();

                if(hasChunkAt(checkX, checkZ + 16)) {
                    auto &chunkNorth = m_chunks[toKey(checkX, checkZ + 16)];
                    cPtr->linkNeighbor(chunkNorth, ZPOS);
                }
                if(hasChunkAt(checkX, checkZ - 16)) {
                    auto &chunkSouth = m_chunks[toKey(checkX, checkZ - 16)];
                    cPtr->linkNeighbor(chunkSouth, ZNEG);
                }
                if(hasChunkAt(checkX + 16, checkZ)) {
                    auto &chunkEast = m_chunks[toKey(checkX + 16, checkZ)];
                    cPtr->linkNeighbor(chunkEast, XPOS);
                }
                if(hasChunkAt(checkX - 16, checkZ)) {
                    auto &chunkWest = m_chunks[toKey(checkX - 16, checkZ)];
                    cPtr->linkNeighbor(chunkWest, XNEG);
                }

                m_chunks[toKey(checkX, checkZ)] = std::move(chunk);


                {
                    std::lock_guard<std::mutex> lock(mutex_dataToBlockTypeWorkerThreads);
                    dataToBlockTypeWorkerThreads.push_back(cPtr);

                }


            }
        }
    }
}



// TODO: When you make Chunk inherit from Drawable, change this code so
// it draws each Chunk with the given ShaderProgram
void Terrain::draw(int minX, int maxX, int minZ, int maxZ, ShaderProgram *shaderProgram) {
    for (auto& pair : m_chunks) {
        Chunk* chunk = pair.second.get();
        shaderProgram->setUnifMat4("u_Model", glm::mat4(1.f));
        shaderProgram->drawWeaved(*chunk, INDEX, INTERLEAVED);
    }

    for (auto& pair : m_chunks) {
        Chunk* chunk = pair.second.get();
        shaderProgram->setUnifMat4("u_Model", glm::mat4(1.f));
        shaderProgram->drawWeaved(*chunk, INDEX_TRANSPARENT, INTERLEAVED_TRANSPARENT);
    }
}




void Terrain::checkForTerrainExpansion(glm::vec3 currPos, glm::vec3 prevPos) {
    glm::vec2 currZone = glm::floor(glm::vec2(currPos.x, currPos.z) / 64.0f);
    glm::vec2 prevZone = glm::floor(glm::vec2(prevPos.x, prevPos.z) / 64.0f);


    if(currZone != prevZone) {
        int radius = 8;
        for(int x = -radius; x <= radius; x++) {
            for(int z = -radius; z <= radius; z++) {
                int checkX = (static_cast<int>(glm::floor(currPos.x / 16.0f)) + x) * 16;
                int checkZ = (static_cast<int>(glm::floor(currPos.z / 16.0f)) + z) * 16;

                if(!hasChunkAt(checkX, checkZ)) {
                    Chunk* newChunk = instantiateChunkAt(checkX, checkZ);
                    {
                        std::lock_guard<std::mutex> lock(mutex_dataToBlockTypeWorkerThreads);
                        dataToBlockTypeWorkerThreads.push_back(newChunk);
                    }
                }
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(mutex_dataFromBlockTypeWorkerThreads);
        if(!dataFromBlockTypeWorkerThreads.empty()) {
            for(Chunk* chunk : dataFromBlockTypeWorkerThreads) {
                {
                    std::lock_guard<std::mutex> lock(mutex_dataToVBOWorkerThreads);
                    dataToVBOWorkerThreads.push_back(chunk);
                }
            }
            dataFromBlockTypeWorkerThreads.clear();
        }
    }

    {
        std::lock_guard<std::mutex> lock(mutex_dataFromVBOWorkerThreads);
        if(!dataFromVBOWorkerThreads.empty()) {
            for(const auto& vboData : dataFromVBOWorkerThreads) {
                if(hasChunkAt(vboData.chunk->getX(), vboData.chunk->getZ())) {
                    vboData.chunk->bufferVBO2(vboData.vboData, vboData.vboDataIndex,vboData.vboDataWater, vboData.vboDataWaterIndex);
                }
            }
            dataFromVBOWorkerThreads.clear();
        }
    }
}
