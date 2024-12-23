#pragma once
#include "smartpointerhelp.h"
#include "glm_includes.h"
#include <array>
#include <unordered_map>
#include <cstddef>
#include "drawable.h"
#include <mutex>


//using namespace std;

// C++ 11 allows us to define the size of an enum. This lets us use only one byte
// of memory to store our different block types. By default, the size of a C++ enum
// is that of an int (so, usually four bytes). This *does* limit us to only 256 different
// block types, but in the scope of this project we'll never get anywhere near that many.
enum BlockType : unsigned char
{
    EMPTY = 0,
    GRASS = 1,
    DIRT = 2,
    STONE = 3,
    WATER = 4,
    SNOW = 5,
    LAVA = 6,
    BEDROCK = 7,
    SAND = 8,
    GRAVEL = 9,
    GRASSPLANT = 10,
    CACTUS = 11,
    DEADBUSH = 12,
    OAKLOG = 13,
    OAKLEAVES = 14,
    OAKPLANKS = 15,
    DANDELION = 16,
    POPPY = 17,
    ICE = 18,
    CORAL = 19,
    MELON = 20,
    REDMUSHROOM = 21,
    BROWNMUSHROOM = 22,
    SANDSTONE = 23,
    WOOL = 24,
    JUNGLELEAVES = 25,
    JUNGLELOG = 26

};

//fill threshholds
static int blockDepthThreshold = 256;
static int oceanLevel = 70;
static float bscale = 1.6f;
static float bshift = 39.f;

struct SplineControlPoint {
    float x;
    float y;
};


enum BiomeType : unsigned char
{
    PLAIN, FOREST, RAINFOREST , RIVERS, DESERT, SNOWY, OCEAN, COLDOCEAN, WARMOCEAN
};
enum BlockShape : unsigned char
{
    SHAPE_CACTUS, SHAPE_CROSS, SHAPE_BLOCK, SHAPE_FLUID, SHAPE_NONE
};

//biome thresholds!
static float tthreshlow = 0.4f;
static float tthreshhigh = 0.65f;
static float hthreshlow = 0.4f;
static float hthreshhigh = 0.65f;

// The six cardinal directions in 3D space
enum Direction : unsigned char
{
    XPOS, XNEG, YPOS, YNEG, ZPOS, ZNEG, NONE
};

// Lets us use any enum class as the key of a
// std::unordered_map
struct EnumHash {
    template <typename T>
    size_t operator()(T t) const {
        return static_cast<size_t>(t);
    }
};

// One Chunk is a 16 x 256 x 16 section of the world,
// containing all the Minecraft blocks in that area.
// We divide the world into Chunks in order to make
// recomputing its VBO data faster by not having to
// render all the world at once, while also not having
// to render the world block by block.

// TODO have Chunk inherit from Drawable
class Chunk : public Drawable{
private:
    // All of the blocks contained within this Chunk
    std::array<BlockType, 65536> m_blocks;
    std::array<BiomeType, 256> m_biomes;



    // The coordinates of the chunk's lower-left corner in world space
    int minX, minZ;
    // This Chunk's four neighbors to the north, south, east, and west
    // The third input to this map just lets us use a Direction as
    // a key for this map.
    // These allow us to properly determine
    std::unordered_map<Direction, Chunk*, EnumHash> m_neighbors;



public:

    std::vector<glm::vec4> vboData;
    std::vector<GLuint> vboDataIndex;
    std::vector<glm::vec4> vboDataWater;
    std::vector<GLuint> vboDataWaterIndex;

    std::mutex mutex;

    Chunk(OpenGLContext* context, int x, int z);

    BiomeType getLocalBiomeAt(unsigned int x, unsigned int z) const;
    BiomeType getLocalBiomeAt(int x, int z) const;
    void setLocalBiomeAt(unsigned int x, unsigned int z, BiomeType b);

    BlockType getLocalBlockAt(unsigned int x, unsigned int y, unsigned int z) const;
    BlockType getLocalBlockAt(int x, int y, int z) const;
    void setLocalBlockAt(unsigned int x, unsigned int y, unsigned int z, BlockType t);

    void linkNeighbor(uPtr<Chunk>& neighbor, Direction dir);

    void createVBOdata() override;
    void bufferVBO();

    bool isTransparent(BlockType t);
    bool isEmpty(BlockType t);

    /*void addFaceVertices(int x, int y, int z, Direction dir, BlockType t,
                                const std::unordered_map<BlockType, glm::vec3>& colorMap,
                                std::vector<glm::vec4>& vboData, std::vector<GLuint>& vboDataIndex, int& index) ;*/

    int getX();
    int getZ();

    void bufferVBO2(const std::vector<glm::vec4>& vboData, const std::vector<GLuint>& indexData,
                    const std::vector<glm::vec4>& vboWaterData, const std::vector<GLuint>& indexWaterData);

    static BiomeType getBiomeFromTemperatureAndHumidity(float t, float h);
    static std::vector<SplineControlPoint> getSplineFromBiome(BiomeType b);
    static float splineLerp(float x, SplineControlPoint p1, SplineControlPoint p2);
    static float getSplineOutput(float x, const std::vector<SplineControlPoint> &spline);
    static float getBlendedHeightFromTemperatureAndHumidity(BiomeType biome, float height, float t, float h);
    void PaintColumn(int x, int maxY, int z, BiomeType biome);
    static bool isPassable(BlockType t);
    static bool isFluid(BlockType t);
    static bool isSelectable(BlockType t);
};

#define ZYX_LOOP for(int z=0; z< 16; ++z) { for(int y=0; y<256; ++y) { for (int x=0; x<16; ++x){
#define ZYX_END }}}


struct ChunkVBOData {
    std::vector<glm::vec4> vboData;
    std::vector<GLuint> vboDataIndex;
    std::vector<glm::vec4> vboDataWater;
    std::vector<GLuint> vboDataWaterIndex;
    Chunk* chunk;
};
