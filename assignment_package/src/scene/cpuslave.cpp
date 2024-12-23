#include "cpuslave.h"

struct Structure {
    bool valid;
    std::vector<std::vector<std::vector<BlockType>>> blocks;
    glm::ivec3 offset;

    Structure(int width, int height, int depth, const glm::ivec3& offset = glm::ivec3(0))
        : blocks(width, std::vector<std::vector<BlockType>>(height, std::vector<BlockType>(depth, BlockType::EMPTY))),
        offset(offset) {}

};

const Structure& getStructreTree1() {
    static Structure structure(5,7,5);
    static bool initialized = false;
    structure.offset = glm::ivec3(-2,1,-2);

    if (!initialized) {

        //leaves
        for (int y = 3; y <=4; y++) {
            for (int x = 0; x <= 4; x++) {
                for (int z = 0; z <= 4; z++) {
                    structure.blocks[x][y][z] = OAKLEAVES;
                }
            }
            structure.blocks[0][y][0] = EMPTY;
            structure.blocks[4][y][0] = EMPTY;
            structure.blocks[0][y][4] = EMPTY;
            structure.blocks[4][y][4] = EMPTY;
        }
        for (int y = 5; y <=6; y++) {
            for (int x = 1; x <= 3; x++) {
                for (int z = 1; z <= 3; z++) {
                    structure.blocks[x][y][z] = OAKLEAVES;
                }
            }
            structure.blocks[1][y][1] = EMPTY;
            structure.blocks[3][y][1] = EMPTY;
            structure.blocks[1][y][3] = EMPTY;
            structure.blocks[3][y][3] = EMPTY;
        }

        //trunk
        for (int i = 0; i <= 4; i++) {
            structure.blocks[2][i][2] = OAKLOG;
        }


        initialized = true;

    }

    structure.valid = true;
    return structure;
}

const Structure& getStructreTree2() {
    static Structure structure(8,20,8);
    static bool initialized = false;
    structure.offset = glm::ivec3(-4,-1,-4);

    if (!initialized) {

        //leaves
        for (int y = 17; y <=18; y++) {
            for (int x = 0; x <= 7; x++) {
                for (int z = 0; z <= 7; z++) {
                    structure.blocks[x][y][z] = OAKLEAVES;
                }
            }
            structure.blocks[0][y][0] = EMPTY;
            structure.blocks[7][y][0] = EMPTY;
            structure.blocks[7][y][7] = EMPTY;
            structure.blocks[0][y][7] = EMPTY;
        }
        for (int y = 13; y <=14; y++) {
            for (int x = 1; x <= 6; x++) {
                for (int z = 1; z <= 6; z++) {
                    structure.blocks[x][y][z] = OAKLEAVES;
                }
            }
            structure.blocks[1][y][1] = EMPTY;
            structure.blocks[6][y][1] = EMPTY;
            structure.blocks[6][y][6] = EMPTY;
            structure.blocks[1][y][6] = EMPTY;
        }
        for (int y = 19; y <= 19; y++) {
            for (int x = 1; x <= 6; x++) {
                for (int z = 1; z <= 6; z++) {
                    structure.blocks[x][y][z] = OAKLEAVES;
                }
            }
            structure.blocks[1][y][1] = EMPTY;
            structure.blocks[6][y][1] = EMPTY;
            structure.blocks[1][y][6] = EMPTY;
            structure.blocks[6][y][6] = EMPTY;
        }

        //trunk
        for (int i = 0; i <= 17; i++) {
            structure.blocks[3][i][3] = OAKLOG;
            structure.blocks[3][i][4] = OAKLOG;
            structure.blocks[4][i][4] = OAKLOG;
            structure.blocks[4][i][3] = OAKLOG;
        }


        initialized = true;

    }

    structure.valid = true;
    return structure;
}

const Structure& getStructreTree3() {
    static Structure structure(8,15,8);
    static bool initialized = false;
    structure.offset = glm::ivec3(-4,-1,-4);

    if (!initialized) {

        //leaves
        for (int y = 12; y <= 13; y++) {
            for (int x = 0; x <= 7; x++) {
                for (int z = 0; z <= 7; z++) {
                    structure.blocks[x][y][z] = JUNGLELEAVES;
                }
            }
            structure.blocks[0][y][0] = EMPTY;
            structure.blocks[7][y][0] = EMPTY;
            structure.blocks[7][y][7] = EMPTY;
            structure.blocks[0][y][7] = EMPTY;
        }
        for (int y = 14; y <= 14; y++) {
            for (int x = 2; x <= 5; x++) {
                for (int z = 2; z <= 5; z++) {
                    structure.blocks[x][y][z] = JUNGLELEAVES;
                }
            }
            structure.blocks[2][y][2] = EMPTY;
            structure.blocks[5][y][2] = EMPTY;
            structure.blocks[5][y][5] = EMPTY;
            structure.blocks[2][y][5] = EMPTY;
        }

        //trunk
        for (int i = 0; i <= 12; i++) {
            structure.blocks[3][i][3] = JUNGLELOG;
            structure.blocks[3][i][4] = JUNGLELOG;
            structure.blocks[4][i][4] = JUNGLELOG;
            structure.blocks[4][i][3] = JUNGLELOG;
        }


        initialized = true;

    }

    structure.valid = true;
    return structure;
}

const Structure& getStructrePyramid() {
    int sizex = 23;
    int sizey = sizex/2+1;
    static Structure structure(sizex,sizey,sizex);
    static bool initialized = false;
    structure.offset = glm::ivec3(-sizex/2,-3,-sizex/2);

    if (!initialized) {

        //stone
        for (int y = 0; y <= sizey; y++) {
            for (int x = y; x <= sizex-1-y; x++) {
                for (int z = y; z <= sizex-1-y; z++) {
                    structure.blocks[x][y][z] = SANDSTONE;
                }
            }
        }

        initialized = true;

    }

    structure.valid = true;
    return structure;
}

const Structure& getStructreBoat() {
    static Structure structure(6,8,5);
    static bool initialized = false;
    structure.offset = glm::ivec3(-4,0,-2);

    if (!initialized) {

        //stone
        for (int x = 0; x < 6; x++) {
            for (int z = 0; z < 5; z++) {
                structure.blocks[x][0][z] = OAKPLANKS;
            }
        }

        for (int y = 1; y < 8; y++) {
            structure.blocks[5][y][2] = OAKLOG;
        }

        for (int y = 4; y < 8; y++) {
            for (int z = 0; z < 5; z++) {
                structure.blocks[4][y][z] = WOOL;
            }
        }

        initialized = true;

    }

    structure.valid = true;
    return structure;
}

struct VertexData {
    glm::vec4 pos;
    glm::vec4 nor;
    glm::vec2 uv;
};

struct BlockModelFace {
    Direction cullDirection;
    glm::ivec3 offset;
    std::array<VertexData, 4> vertexData;
};

struct BlockModel {
    std::vector<BlockModelFace> faces;
};

const static std::unordered_map<BlockType, BlockShape> blockShapeMap = {
    {EMPTY, SHAPE_NONE},
    {GRASS, SHAPE_BLOCK},
    {DIRT, SHAPE_BLOCK},
    {STONE, SHAPE_BLOCK},
    {WATER, SHAPE_FLUID},
    {SNOW, SHAPE_BLOCK},
    {LAVA, SHAPE_FLUID},
    {BEDROCK, SHAPE_BLOCK},
    {SAND, SHAPE_BLOCK},
    {GRAVEL, SHAPE_BLOCK},
    {GRASSPLANT, SHAPE_CROSS},
    {CACTUS, SHAPE_CACTUS},
    {DEADBUSH, SHAPE_CROSS},
    {OAKLOG, SHAPE_BLOCK},
    {OAKLEAVES, SHAPE_BLOCK},
    {OAKPLANKS, SHAPE_BLOCK},
    {DANDELION, SHAPE_CROSS},
    {POPPY, SHAPE_CROSS},
    {ICE, SHAPE_BLOCK},
    {CORAL, SHAPE_CROSS},
    {MELON, SHAPE_BLOCK},
    {REDMUSHROOM, SHAPE_CROSS},
    {BROWNMUSHROOM, SHAPE_CROSS},
    {SANDSTONE, SHAPE_BLOCK},
    {WOOL, SHAPE_BLOCK},
    {JUNGLELEAVES, SHAPE_BLOCK},
    {JUNGLELOG, SHAPE_BLOCK}
};

const BlockModel& getModelBlock() {
    static BlockModel blockmodel;
    static bool initialized = false;

    if (!initialized) {

        std::vector<std::pair<Direction, glm::vec3>> dlist = {
            {XPOS, glm::vec3(1, 0, 0)},
            {XNEG, glm::vec3(-1, 0, 0)},
            {YPOS, glm::vec3(0, 1, 0)},
            {YNEG, glm::vec3(0, -1, 0)},
            {ZPOS, glm::vec3(0, 0, 1)},
            {ZNEG, glm::vec3(0, 0, -1)}
        };

        for (const auto& [dir, offset] : dlist ) {

            BlockModelFace face;

            face.cullDirection = dir;

            switch (dir) {
            case XPOS:
                face.vertexData[0].pos = glm::vec4(1, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(1, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(1, 0, 1, 1);
                break;
            case XNEG:
                face.vertexData[0].pos = glm::vec4(0, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(0, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(0, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(0, 0, 1, 1);
                break;
            case YPOS:
                face.vertexData[0].pos = glm::vec4(0, 1, 0, 1);
                face.vertexData[1].pos = glm::vec4(1, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(0, 1, 1, 1);
                break;
            case YNEG:
                face.vertexData[0].pos = glm::vec4(0, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(1, 0, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, 0, 1, 1);
                face.vertexData[3].pos = glm::vec4(0, 0, 1, 1);
                break;
            case ZPOS:
                face.vertexData[0].pos = glm::vec4(0, 0, 1, 1);
                face.vertexData[1].pos = glm::vec4(0, 1, 1, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(1, 0, 1, 1);
                break;
            case ZNEG:
                face.vertexData[0].pos = glm::vec4(0, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(0, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 0, 1);
                face.vertexData[3].pos = glm::vec4(1, 0, 0, 1);
                break;
            }

            // Set normal for all vertices
            for (auto& v : face.vertexData) {
                v.nor = glm::vec4(offset, 0);
            }

            // Set uv for all vertices
            face.vertexData[0].uv = glm::vec2(0, 0);
            face.vertexData[1].uv = glm::vec2(0, 1);
            face.vertexData[2].uv = glm::vec2(1, 1);
            face.vertexData[3].uv = glm::vec2(1, 0);

            blockmodel.faces.push_back(face);
        }
        initialized = true;
    }

    return blockmodel;
}

const BlockModel& getModelFluid() {
    static BlockModel blockmodel;
    static bool initialized = false;

    if (!initialized) {

        std::vector<std::pair<Direction, glm::vec3>> dlist = {
            {XPOS, glm::vec3(1, 0, 0)},
            {XNEG, glm::vec3(-1, 0, 0)},
            {YPOS, glm::vec3(0, 1, 0)},
            {YNEG, glm::vec3(0, -1, 0)},
            {ZPOS, glm::vec3(0, 0, 1)},
            {ZNEG, glm::vec3(0, 0, -1)}
        };

        for (const auto& [dir, offset] : dlist ) {

            BlockModelFace face;

            face.cullDirection = dir;

            switch (dir) {
            case XPOS:
                face.vertexData[0].pos = glm::vec4(1, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(1, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(1, 0, 1, 1);
                break;
            case XNEG:
                face.vertexData[0].pos = glm::vec4(0, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(0, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(0, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(0, 0, 1, 1);
                break;
            case YPOS:
                face.vertexData[0].pos = glm::vec4(0, .875, 0, 1);
                face.vertexData[1].pos = glm::vec4(1, .875, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, .875, 1, 1);
                face.vertexData[3].pos = glm::vec4(0, .875, 1, 1);
                break;
            case YNEG:
                face.vertexData[0].pos = glm::vec4(0, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(1, 0, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, 0, 1, 1);
                face.vertexData[3].pos = glm::vec4(0, 0, 1, 1);
                break;
            case ZPOS:
                face.vertexData[0].pos = glm::vec4(0, 0, 1, 1);
                face.vertexData[1].pos = glm::vec4(0, 1, 1, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(1, 0, 1, 1);
                break;
            case ZNEG:
                face.vertexData[0].pos = glm::vec4(0, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(0, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 0, 1);
                face.vertexData[3].pos = glm::vec4(1, 0, 0, 1);
                break;
            }

            // Set normal for all vertices
            for (auto& v : face.vertexData) {
                v.nor = glm::vec4(offset, 0);
            }

            // Set uv for all vertices
            face.vertexData[0].uv = glm::vec2(0, 0);
            face.vertexData[1].uv = glm::vec2(0, 1);
            face.vertexData[2].uv = glm::vec2(1, 1);
            face.vertexData[3].uv = glm::vec2(1, 0);

            blockmodel.faces.push_back(face);
        }
        initialized = true;
    }

    return blockmodel;
}

const BlockModel& getModelCactus() {
    static BlockModel blockmodel;
    static bool initialized = false;

    if (!initialized) {

        std::vector<std::pair<Direction, glm::vec3>> dlist = {
            {XPOS, glm::vec3(1, 0, 0)},
            {XNEG, glm::vec3(-1, 0, 0)},
            {YPOS, glm::vec3(0, 1, 0)},
            {YNEG, glm::vec3(0, -1, 0)},
            {ZPOS, glm::vec3(0, 0, 1)},
            {ZNEG, glm::vec3(0, 0, -1)}
        };

        for (const auto& [dir, offset] : dlist ) {

            BlockModelFace face;

            face.cullDirection = dir;

            switch (dir) {
            case XPOS:
                face.vertexData[0].pos = glm::vec4(0.9375, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(0.9375, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(0.9375, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(0.9375, 0, 1, 1);
                face.cullDirection = NONE;
                break;
            case XNEG:
                face.vertexData[0].pos = glm::vec4(0.0625, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(0.0625, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(0.0625, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(0.0625, 0, 1, 1);
                face.cullDirection = NONE;
                break;
            case YPOS:
                face.vertexData[0].pos = glm::vec4(0, 1, 0, 1);
                face.vertexData[1].pos = glm::vec4(1, 1, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 1, 1);
                face.vertexData[3].pos = glm::vec4(0, 1, 1, 1);
                break;
            case YNEG:
                face.vertexData[0].pos = glm::vec4(0, 0, 0, 1);
                face.vertexData[1].pos = glm::vec4(1, 0, 0, 1);
                face.vertexData[2].pos = glm::vec4(1, 0, 1, 1);
                face.vertexData[3].pos = glm::vec4(0, 0, 1, 1);
                break;
            case ZPOS:
                face.vertexData[0].pos = glm::vec4(0, 0, 0.9375, 1);
                face.vertexData[1].pos = glm::vec4(0, 1, 0.9375, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 0.9375, 1);
                face.vertexData[3].pos = glm::vec4(1, 0, 0.9375, 1);
                face.cullDirection = NONE;
                break;
            case ZNEG:
                face.vertexData[0].pos = glm::vec4(0, 0, 0.0625, 1);
                face.vertexData[1].pos = glm::vec4(0, 1, 0.0625, 1);
                face.vertexData[2].pos = glm::vec4(1, 1, 0.0625, 1);
                face.vertexData[3].pos = glm::vec4(1, 0, 0.0625, 1);
                face.cullDirection = NONE;
                break;
            }

            // Set normal for all vertices
            for (auto& v : face.vertexData) {
                v.nor = glm::vec4(offset, 0);
            }

            // Set uv for all vertices
            face.vertexData[0].uv = glm::vec2(0, 0);
            face.vertexData[1].uv = glm::vec2(0, 1);
            face.vertexData[2].uv = glm::vec2(1, 1);
            face.vertexData[3].uv = glm::vec2(1, 0);

            blockmodel.faces.push_back(face);
        }
        initialized = true;
    }

    return blockmodel;
}

const BlockModel& getModelCross() {
    static BlockModel blockmodel;
    static bool initialized = false;

    if (!initialized) {

        std::vector<std::pair<Direction, glm::vec3>> dlist = {
                                                              {XPOS, glm::normalize(glm::vec3(0, 1, 0))},
                                                              {XNEG, glm::normalize(glm::vec3(0, 1, 0))},
                                                              {ZPOS, glm::normalize(glm::vec3(0, 1, 0))},
                                                              {ZNEG, glm::normalize(glm::vec3(0, 1, 0))},
                                                              };

        for (const auto& [dir, offset] : dlist ) {

            BlockModelFace face;

            face.cullDirection = dir;

            switch (dir) {
            case ZPOS:
                face.vertexData[0].pos = glm::vec4(.1464, 0, .1464, 1);
                face.vertexData[1].pos = glm::vec4(.1464, 1, .1464, 1);
                face.vertexData[2].pos = glm::vec4(.8536, 1, .8536, 1);
                face.vertexData[3].pos = glm::vec4(.8536, 0, .8536, 1);
                break;
            case ZNEG:
                face.vertexData[3].pos = glm::vec4(.8536, 0, .8536, 1);
                face.vertexData[2].pos = glm::vec4(.8536, 1, .8536, 1);
                face.vertexData[1].pos = glm::vec4(.1464, 1, .1464, 1);
                face.vertexData[0].pos = glm::vec4(.1464, 0, .1464, 1);
                break;
            case XPOS:
                face.vertexData[0].pos = glm::vec4(.1464, 0, .8536, 1);
                face.vertexData[1].pos = glm::vec4(.1464, 1, .8536, 1);
                face.vertexData[2].pos = glm::vec4(.8536, 1, .1464, 1);
                face.vertexData[3].pos = glm::vec4(.8536, 0, .1464, 1);
                break;
            case XNEG:
                face.vertexData[3].pos = glm::vec4(.8536, 0, .1464, 1);
                face.vertexData[2].pos = glm::vec4(.8536, 1, .1464, 1);
                face.vertexData[1].pos = glm::vec4(.1464, 1, .8536, 1);
                face.vertexData[0].pos = glm::vec4(.1464, 0, .8536, 1);
                break;
            }

            // Set normal for all vertices
            for (auto& v : face.vertexData) {
                v.nor = glm::vec4(offset, 0);
            }

            // Set uv for all vertices
            face.vertexData[0].uv = glm::vec2(0, 0);
            face.vertexData[1].uv = glm::vec2(0, 1);
            face.vertexData[2].uv = glm::vec2(1, 1);
            face.vertexData[3].uv = glm::vec2(1, 0);

            blockmodel.faces.push_back(face);
        }
        initialized = true;
    }

    return blockmodel;
}

BlockModel getBlockShapeModel(BlockShape sp) {
    switch(sp) {
    case SHAPE_BLOCK:
        return getModelBlock();
    case SHAPE_FLUID:
        return getModelFluid();
    case SHAPE_CROSS:
        return getModelCross();
    case SHAPE_CACTUS:
        return getModelCactus();
    default:
        return BlockModel();
    }
}

static const std::unordered_map<Direction, glm::vec3> dirToOffsetMap = {
    {XPOS, glm::vec3(1, 0, 0)},
    {XNEG, glm::vec3(-1, 0, 0)},
    {YPOS, glm::vec3(0, 1, 0)},
    {YNEG, glm::vec3(0, -1, 0)},
    {ZPOS, glm::vec3(0, 0, 1)},
    {ZNEG, glm::vec3(0, 0, -1)}
};

void populateVBO(
    std::vector<glm::vec4>* vboDataTemp,
    std::vector<GLuint>* vboDataIndexTemp,
    std::vector<glm::vec4>* vboDataWaterTemp,
    std::vector<GLuint>* vboDataWaterIndexTemp,
    Chunk* chunkToProcess
    ) {

    std::vector<glm::vec4> *this_vboData;
    std::vector<GLuint> *this_vboDataIndex;
    int index = 0;
    int indexWater = 0;
    int *this_index;

    ZYX_LOOP

            BlockType t = chunkToProcess->getLocalBlockAt(x, y, z);
    BlockShape p = blockShapeMap.at(t);

    if (p != SHAPE_NONE) {

        if (Chunk::isFluid(t)) {
            this_vboData = vboDataWaterTemp;
            this_vboDataIndex = vboDataWaterIndexTemp;
            this_index = &indexWater;
        } else {
            this_vboData = vboDataTemp;
            this_vboDataIndex = vboDataIndexTemp;
            this_index = &index;
        }

        BlockModel model = getBlockShapeModel(p);

        for (const auto& face : model.faces) {

            bool inBound;
            bool hasNeighbor;

            if (face.cullDirection != NONE) {
                //offset vec3
                glm::vec3 cullCheckOffset = dirToOffsetMap.at(face.cullDirection);

                int nx = x + cullCheckOffset.x;
                int ny = y + cullCheckOffset.y;
                int nz = z + cullCheckOffset.z;

                inBound = (nx >= 0 && nx < 16 && ny >= 0 && ny < 256 && nz >= 0 && nz < 16);
                hasNeighbor;
                if (Chunk::isFluid(t)) {
                    hasNeighbor = (inBound) &&
                                  !(chunkToProcess->isEmpty(chunkToProcess->getLocalBlockAt(nx, ny, nz)));
                } else {
                    hasNeighbor = (inBound) &&
                                  !(chunkToProcess->isTransparent(chunkToProcess->getLocalBlockAt(nx, ny, nz)));
                }
            } else {
                inBound = true;
                hasNeighbor = false;
            }

            if ((!Chunk::isFluid(t) && !hasNeighbor) || (Chunk::isFluid(t) && inBound && !hasNeighbor)) {

                for (const auto& vd : face.vertexData) {
                    this_vboData->push_back(vd.pos + glm::vec4(x, y, z, 0) +
                                            glm::vec4(chunkToProcess->getX(), 0, chunkToProcess->getZ(), 0));
                    this_vboData->push_back(vd.nor);

                    glm::vec2 uv = (vd.uv + getBlockFaceTextureLocation(t, face.cullDirection)) / 16.0f;
                    float isAnimated = 0.0f;
                    float animationInterval = 0.0f;
                    if (t == LAVA) {
                        isAnimated = 1.0f;
                        animationInterval = 160.0f;
                    }
                    if (t == WATER) {
                        isAnimated = 1.0f;
                        animationInterval = 80.0f;
                    }
                    this_vboData->push_back(glm::vec4(uv.x,uv.y,isAnimated,animationInterval));

                }

                this_vboDataIndex->push_back(*this_index);
                this_vboDataIndex->push_back(*this_index + 1);
                this_vboDataIndex->push_back(*this_index + 2);
                this_vboDataIndex->push_back(*this_index);
                this_vboDataIndex->push_back(*this_index + 2);
                this_vboDataIndex->push_back(*this_index + 3);

                *this_index += 4;

            }
        }
    }
    ZYX_END
}

void VBOWorker(std::vector<Chunk*>& dataToVBOWorkerThreads,
               std::vector<ChunkVBOData>& dataFromVBOWorkerThreads,
               std::mutex& mutex_dataToVBOWorkerThreads,
               std::mutex& mutex_dataFromVBOWorkerThreads) {

    while (true) {
        Chunk* chunkToProcess = nullptr;
        {
            std::lock_guard<std::mutex> lock(mutex_dataToVBOWorkerThreads);
            if (!dataToVBOWorkerThreads.empty()) {
                chunkToProcess = dataToVBOWorkerThreads.back();
                dataToVBOWorkerThreads.pop_back();
            }
        }

        if (chunkToProcess) {

            std::lock_guard<std::mutex> lock(chunkToProcess->mutex);

            std::vector<glm::vec4> vboDataTemp;
            std::vector<GLuint> vboDataIndexTemp;
            std::vector<glm::vec4> vboDataWaterTemp;
            std::vector<GLuint> vboDataWaterIndexTemp;

            populateVBO(&vboDataTemp, &vboDataIndexTemp, &vboDataWaterTemp, &vboDataWaterIndexTemp, chunkToProcess);

            ChunkVBOData vboData;
            vboData.vboData = vboDataTemp;
            vboData.vboDataIndex = vboDataIndexTemp;
            vboData.vboDataWater = vboDataWaterTemp;
            vboData.vboDataWaterIndex = vboDataWaterIndexTemp;
            vboData.chunk = chunkToProcess;

            {
                std::lock_guard<std::mutex> lock(mutex_dataFromVBOWorkerThreads);
                dataFromVBOWorkerThreads.push_back(vboData);
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

float getHumidityFromXZ(int x, int z) {
    return 1.f-Noise::getTweakedPerlinNoise(glm::vec2(x,z), bshift, 311.f * bscale);
}

float getTemperatureFromXZ(int x, int z) {
    return Noise::getTweakedPerlinNoise(glm::vec2(x, z), bshift * 8.f, 196.f * bscale);
}

BiomeType getBiomeFromXZ(int x, int z) {

    float Temperature = getTemperatureFromXZ(x, z);
    float Humidity = getHumidityFromXZ(x, z);

    return Chunk::getBiomeFromTemperatureAndHumidity(Temperature, Humidity);

}
float getHeightFromXZ(int x, int z) {

    float Temperature = getTemperatureFromXZ(x, z);
    float Humidity = getHumidityFromXZ(x, z);
    BiomeType biome = getBiomeFromXZ(x, z);

    float heightFactor = Noise::getHeightMapNoise(glm::vec2(x,z));
    return Chunk::getBlendedHeightFromTemperatureAndHumidity(biome, heightFactor, Temperature, Humidity);


}

void blockTypeWorker(std::vector<Chunk*>& dataToBlockTypeWorkerThreads,
                     std::vector<Chunk*>& dataFromBlockTypeWorkerThreads,
                     std::mutex& mutex_dataToBlockTypeWorkerThreads,
                     std::mutex& mutex_dataFromBlockTypeWorkerThreads) {
    while (true) {
        Chunk* chunkToProcess = nullptr;

        {
            std::lock_guard<std::mutex> lock(mutex_dataToBlockTypeWorkerThreads);
            if (!dataToBlockTypeWorkerThreads.empty()) {
                chunkToProcess = dataToBlockTypeWorkerThreads.back();
                dataToBlockTypeWorkerThreads.pop_back();
            }
        }

        if (chunkToProcess) {

            std::lock_guard<std::mutex> lock(chunkToProcess->mutex);

            int x = chunkToProcess->getX();
            int z = chunkToProcess->getZ();

            std::vector<std::vector<int>> heightMap(16, std::vector<int>(16, 0));

            for (int localX = 0; localX < 16; localX++) {
                for (int localZ = 0; localZ < 16; localZ++) {
                    int worldX = x + localX;
                    int worldZ = z + localZ;

                    BiomeType biome = getBiomeFromXZ(worldX, worldZ);

                    chunkToProcess->setLocalBiomeAt(localX,localZ,biome);

                    float finalHeight = getHeightFromXZ(worldX, worldZ);

                    chunkToProcess->PaintColumn(localX, finalHeight, localZ, biome);

                    heightMap[localX][localZ] = finalHeight;

                    /*
                    BlockType topBlock = GRASS;

                    switch (biome) {

                    }

                    chunkToProcess->setLocalBlockAt(localX, finalHeight, localZ, topBlock);*/

                    /*
                    float mountainH = Noise::mountainHeight(glm::vec2(worldX, worldZ));
                    float grassH = Noise::grasslandHeight(glm::vec2(worldX, worldZ));

                    float biomeValue = Noise::biomeBlender(glm::vec2(worldX, worldZ));
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

                    if (t > 0.55f) {
                        if (finalHeight > 200) {
                            chunkToProcess->setLocalBlockAt(localX, finalHeight, localZ, SNOW);
                        }
                        else {
                            chunkToProcess->setLocalBlockAt(localX, finalHeight, localZ, STONE);
                        }
                        for (int y = finalHeight - 1; y > 0; --y) {
                            chunkToProcess->setLocalBlockAt(localX, y, localZ, STONE);
                        }
                    }
                    else {
                        chunkToProcess->setLocalBlockAt(localX, finalHeight, localZ, GRASS);
                        for (int y = finalHeight - 1; y > 128; --y) {
                            chunkToProcess->setLocalBlockAt(localX, y, localZ, DIRT);
                        }
                        for (int y = 128; y > 0; --y) {
                            chunkToProcess->setLocalBlockAt(localX, y, localZ, STONE);
                        }
                    }*/

                    /*
                    for (int y = 128; y <= 138; y++) {
                        if (chunkToProcess->getLocalBlockAt(localX, y, localZ) == EMPTY) {
                            chunkToProcess->setLocalBlockAt(localX, y, localZ, WATER);
                        }
                    }
                    */


                    for (int y = 0; y <= finalHeight - 5; ++y) {
                        glm::vec3 worldPosCave = glm::vec3(worldX, y, worldZ);
                        float noiseValueCave = Noise::perlinNoise3D(worldPosCave / 32.f);

                        if (y == 0) {
                            chunkToProcess->setLocalBlockAt(localX, y, localZ, BEDROCK);
                        } else if (noiseValueCave < 0) {
                            if (y < 25) {
                                chunkToProcess->setLocalBlockAt(localX, y, localZ, LAVA);
                            } else {
                                 chunkToProcess->setLocalBlockAt(localX, y, localZ, EMPTY);
                            }
                        }
                    }

                }
            }

            //Check for assets
            for (int px = -16; px < 32; px++) {
                for (int pz = -16; pz < 32; pz++) {
                    float assetNoise = Noise::getTweakedPerlinNoise(glm::vec2(x+px,z+pz), 6.75, 0.8f);

                    //getBiome
                    BiomeType biome = getBiomeFromXZ(x+px,z+pz);

                    float sRate = 10000.0f;
                    if (biome == FOREST) {
                        sRate = 40.f;
                    }
                    if (biome == RIVERS) {
                        sRate = 100.f;
                    }
                    if (biome == PLAIN) {
                        sRate = 180.f;
                    }
                    if (biome == RAINFOREST) {
                        sRate = 30.f;
                    }
                    if (biome == OCEAN ) {
                        sRate = 150.f;
                    }
                    if (biome == WARMOCEAN ) {
                        sRate = 600.f;
                    }
                    if (biome == DESERT ) {
                        sRate = 450.f;
                    }

                    if ((static_cast<int>(std::floor(assetNoise * sRate)) % static_cast<int>(sRate) == 3)) {

                        int y = getHeightFromXZ(x+px,z+pz);

                        Structure s (1, 1, 1);
                        if (biome == FOREST || biome == RIVERS || biome == PLAIN) {
                            if (y > oceanLevel) {
                                s = getStructreTree1();
                            }
                        }
                        if (biome == RAINFOREST) {
                            if (y > oceanLevel) {
                                if (y % 3 == 0) {
                                    s = getStructreTree2();

                                } else {
                                    s = getStructreTree3();
                                }
                            }
                        }
                        if (biome == OCEAN || biome == WARMOCEAN) {
                            s = getStructreBoat();
                            y = oceanLevel;
                        }
                        if (biome == DESERT) {
                            if (y > oceanLevel) {
                                s = getStructrePyramid();
                            }
                        }

                        if (s.valid) {

                            int cx = 0;
                            for (const auto& xdata : s.blocks) {
                                int sbx = cx + px + s.offset.x;

                                if (sbx >= 0 && sbx < 16) {

                                    int cy = 0;
                                    for (const auto& ydata : xdata) {
                                        int sby = cy + s.offset.y;

                                        if (sby >= 0 && sby < 256) {

                                            int cz = 0;
                                            for (const auto& block : ydata) {
                                                int sbz = cz + pz + s.offset.z;

                                                if (sbz >= 0 && sbz < 16) {

                                                    if (block != EMPTY) {
                                                        chunkToProcess->setLocalBlockAt(sbx, y + sby, sbz, block);
                                                    }

                                                }
                                                cz++;
                                            }

                                        }
                                        cy++;
                                    }

                                }
                                cx++;
                            }
                        }


                    }
                }
            }

            {
                std::lock_guard<std::mutex> lock(mutex_dataFromBlockTypeWorkerThreads);
                dataFromBlockTypeWorkerThreads.push_back(chunkToProcess);
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}
