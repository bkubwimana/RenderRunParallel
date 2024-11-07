/*
Author: Benjamin Kubwimana
Class: ECE4122 or ECE6122 (section Q)
Last Date Modified: 11/07/2024
Description:
cuda kernel implementations for grid updates and neighbor status computation
*/

// cuda_kernels.cu
#include "cuda_kernels.cuh"
#include <cuda_runtime.h>
#include <iostream>
#include "device_launch_parameters.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>

using namespace std;

// Kernel function to update the grid
__global__ void updateGridKernel(uint8_t* grid, uint8_t* newGrid, int width, int height)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x; 
    int y = blockIdx.y * blockDim.y + threadIdx.y; 
    if (x < width && y < height)
    {
        int idx = y * width + x;
        int neighbors = countNeighbors(grid, x, y, width, height);
        if (grid[idx])
        {
            // Alive cell
            if (neighbors < 2 || neighbors > 3)
            {
                newGrid[idx] = 0; // Cell dies
            }
            else
            {
                //stays alive
                newGrid[idx] = 1; // 
            }
        }
        else
        {
            // Dead cell
            if (neighbors == 3)
            {   //becomes alive
                newGrid[idx] = 1;
            }
            else
            {   //stays dead
                newGrid[idx] = 0;
            }
        }
    }
}

// function to count neighbors on GPU Device
__device__ int countNeighbors(uint8_t* grid, int x, int y, int width, int height) {
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < width && ny >= 0 && ny < height)
            {
                int idx = ny * width + nx;
                count += grid[idx];
            }
        }
    }
    return count;
}


// Function to update the grid 
void updateGrid(uint8_t* grid, uint8_t* newGrid, uint8_t* d_grid, uint8_t* d_newGrid, MemType memory, int threadsPerBlock, int width, int height)
{
    size_t size = width * height * sizeof(uint8_t);

    cudaError_t err;

    if (memory == MemType::NORMAL || memory == MemType::PINNED) {
        // data from host to device
        err = cudaMemcpy(d_grid, grid, size, cudaMemcpyHostToDevice);
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to copy data from host to device for d_grid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(d_grid);
            cudaFree(d_newGrid);
            exit(EXIT_FAILURE);
        }

        //block and grid dimensions
        dim3 blockDim(threadsPerBlock, 1);
        dim3 gridDim(
            (width + blockDim.x - 1) / blockDim.x,
            (height + blockDim.y - 1) / blockDim.y
        );

        // kernel launch
        updateGridKernel << <gridDim, blockDim >> > (d_grid, d_newGrid, width, height);
        err = cudaGetLastError();
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to launch updateGridKernel (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(d_grid);
            cudaFree(d_newGrid);
            exit(EXIT_FAILURE);
        }

        // Wait for GPU to finish
        err = cudaDeviceSynchronize();
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to synchronize device (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(d_grid);
            cudaFree(d_newGrid);
            exit(EXIT_FAILURE);
        }

        // result back to host
        err = cudaMemcpy(newGrid, d_newGrid, size, cudaMemcpyDeviceToHost);
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to copy data from device to host for newGrid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(d_grid);
            cudaFree(d_newGrid);
            exit(EXIT_FAILURE);
        }
    }
    else if (memory == MemType::MANAGED) {

        // block and grid dimensions
        dim3 blockDim(threadsPerBlock, 1);
        dim3 gridDim(
            (width + blockDim.x - 1) / blockDim.x,
            (height + blockDim.y - 1) / blockDim.y
        );

        // kernel launch
        updateGridKernel << <gridDim, blockDim >> > (grid, newGrid, width, height);
        err = cudaGetLastError();
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to launch updateGridKernel (error code %s)!\n", cudaGetErrorString(err));
            exit(EXIT_FAILURE);
        }

        // Wait for GPU to finish
        err = cudaDeviceSynchronize();
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to synchronize device (error code %s)!\n", cudaGetErrorString(err));
            exit(EXIT_FAILURE);
        }
    }
}
