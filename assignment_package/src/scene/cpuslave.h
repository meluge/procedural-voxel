#pragma once
#include "chunk.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include "noise.h"

glm::vec2 getBlockFaceTextureLocation(BlockType t, Direction dir);

void VBOWorker(std::vector<Chunk*>& dataToVBOWorkerThreads, std::vector<ChunkVBOData>& dataFromVBOWorkerThreads,
               std::mutex& mutex_dataToVBOWorkerThreads, std::mutex& mutex_dataFromVBOWorkerThreads);

void blockTypeWorker(std::vector<Chunk*>& dataToBlockTypeWorkerThreads, std::vector<Chunk*>& dataFromBlockTypeWorkerThreads,
                     std::mutex& mutex_dataToBlockTypeWorkerThreads, std::mutex& mutex_dataFromBlockTypeWorkerThreads) ;

void populateVBO(
    std::vector<glm::vec4>* vboDataTemp,
    std::vector<GLuint>* vboDataIndexTemp,
    std::vector<glm::vec4>* vboDataWaterTemp,
    std::vector<GLuint>* vboDataWaterIndexTemp,
    Chunk* chunkToProcess
    );
