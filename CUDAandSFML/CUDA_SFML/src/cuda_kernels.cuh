// cuda_kernels.cuh
#ifndef CUDA_KERNELS_H
#define CUDA_KERNELS_H

#include <vector>
#include <cstdint>
#include "cli_parser.h"

void updateGrid(
    std::uint8_t* grid,
    std::uint8_t* newGrid,
    uint8_t* d_grid, 
    uint8_t* d_newGrid,
    MemType memory,
    int threadsPerBlock,
    int gridWidth,
    int gridHeight
);

// Device function declaration
__device__ int countNeighbors(uint8_t* grid, int x, int y, int width, int height);

#endif // CUDA_KERNELS_H
