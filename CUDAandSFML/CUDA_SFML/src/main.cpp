// main.cpp
// main.cpp
/*
Author: Benjamin Kubwimana
Last Date Modified: 11/07/2024
Description:
Program that uses CUDA and GPU acceleration to simulate the Game of Life
*/

#include <SFML/Graphics.hpp>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include "cli_parser.h"
#include "cuda_kernels.cuh"

using namespace std;
using namespace sf;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int PIXEL_SIZE = 5;
int GRID_WIDTH = WINDOW_WIDTH / PIXEL_SIZE;
int GRID_HEIGHT = WINDOW_HEIGHT / PIXEL_SIZE;

// Function to seed the grid with random values on the host since it is only called once
void seedRandomGrid(uint8_t* grid, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        grid[i] = rand() % 2;
    }
}


// Function to convert MemType to string
string MemTypeToString(MemType MemType)
{
    switch (MemType)
    {
    case MemType::NORMAL:
        return "NORMAL";
    case MemType::PINNED:
        return "PINNED";
    case MemType::MANAGED:
        return "MANAGED";
    default:
        return "Unknown MemType";
    }
}

// Function to print command-line arguments
void printArguments(CommandLineParser& parser)
{
    cout << "-x (Width): " << WINDOW_WIDTH << endl;
    cout << "-y (Height): " << WINDOW_HEIGHT << endl;
    cout << "-c (Size): " << PIXEL_SIZE << endl;
    cout << "-n (threadsPerBlock): " << parser.getCount() << endl;
    cout << "-t (MemType): " << MemTypeToString(parser.getMemType()) << endl;
}



// Main function
int main(int argc, char* argv[])
{
    CommandLineParser parser;

    if (!parser.parseArguments(argc, argv))
    {
        return EXIT_FAILURE;
    }
    else if (parser.getWidth() <= 0 || parser.getHeight() <= 0 || parser.getSize() <= 0 || parser.getCount() <= 0)
    {
        cout << "Invalid input" << endl;
        return EXIT_FAILURE;
    }

    MemType GameMemType = parser.getMemType();
    WINDOW_WIDTH = parser.getWidth();
    WINDOW_HEIGHT = parser.getHeight();
    PIXEL_SIZE = parser.getSize();
    int threadsPerBlock = parser.getCount();
    GRID_WIDTH = WINDOW_WIDTH / PIXEL_SIZE;
    GRID_HEIGHT = WINDOW_HEIGHT / PIXEL_SIZE;

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of Life");
    window.setFramerateLimit(60); // Set frame rate to control speed

	// Host memory variables initialization
    uint8_t* grid_current = nullptr;
    uint8_t* grid_next = nullptr;
    std::vector<uint8_t> grid_current_normal;
    std::vector<uint8_t> grid_next_normal;
    size_t gridSize = GRID_WIDTH * GRID_HEIGHT * sizeof(uint8_t);


    //device memory
    uint8_t* d_grid = nullptr;
    uint8_t* d_newGrid = nullptr;

    cudaError_t err;


	// Flag to check if memory is normal or pinned
    bool flag_normal_or_pinned = false;

	// Allocate memory for the grid from host to avoid repeated memory allocation, memory address will be passed to the device once
    if (GameMemType == MemType::PINNED) {
        flag_normal_or_pinned = true;
        err = cudaMallocHost(&grid_current, gridSize);

        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for d_newGrid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(grid_current);
            exit(EXIT_FAILURE);
        }

        err = cudaMallocHost(&grid_next, gridSize);
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for d_newGrid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(grid_next);
            exit(EXIT_FAILURE);
        }

        err = cudaMalloc((void**)&d_grid, gridSize);
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for d_newGrid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(d_grid);
            exit(EXIT_FAILURE);
        }
        err = cudaMalloc((void**)&d_newGrid, gridSize);
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for d_newGrid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(d_newGrid);
            exit(EXIT_FAILURE);
        }
    }
    else if (GameMemType == MemType::MANAGED) {
        err = cudaMallocManaged(&grid_current, gridSize);
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for d_newGrid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(grid_current);
            exit(EXIT_FAILURE);
        }

        err = cudaMallocManaged(&grid_next, gridSize);
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for d_newGrid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(grid_next);
            exit(EXIT_FAILURE);
        }
    }
    else {
        flag_normal_or_pinned = true;
        grid_current_normal.resize(GRID_WIDTH * GRID_HEIGHT, 0);
        grid_next_normal.resize(GRID_WIDTH * GRID_HEIGHT, 0);
        grid_current = grid_current_normal.data();
        grid_next = grid_next_normal.data();

        err = cudaMalloc((void**)&d_grid, gridSize);
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for d_newGrid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(d_grid);
            exit(EXIT_FAILURE);
        }

        err = cudaMalloc((void**)&d_newGrid, gridSize);
        if (err != cudaSuccess) {
            fprintf(stderr, "Failed to allocate device memory for d_newGrid (error code %s)!\n", cudaGetErrorString(err));
            cudaFree(d_newGrid);
            exit(EXIT_FAILURE);
        }
    }

    printArguments(parser);

    seedRandomGrid(grid_current, GRID_WIDTH * GRID_HEIGHT);


    unsigned long count = 0;
    long double computeTime = 0.0;
    auto computeStart = chrono::high_resolution_clock::now();

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
        }

        // Close the window if the Escape key is pressed
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        if (count % 100 == 0 && count != 0)
        {
            computeTime = 0;
            cout << "Reset @ count: " << count << endl;
        }

        computeStart = chrono::high_resolution_clock::now();

        // Update the grid
        updateGrid(grid_current, grid_next, d_grid, d_newGrid, GameMemType, threadsPerBlock, GRID_WIDTH, GRID_HEIGHT);

        // Get time taken to compute a single generation
        auto computeEnd = chrono::high_resolution_clock::now();
        auto duration = chrono::duration<long double, micro>(computeEnd - computeStart);
        computeTime += duration.count();

        if (count % 100 == 0 && count != 0)
        {
            cout << fixed << setprecision(3);
            cout << "Update @ count: " << count << endl;
            cout << "100 generations took: " << computeTime << "us"
                << " with " << threadsPerBlock << " threads per block using: " << MemTypeToString(GameMemType) << endl;
        }

        window.clear();

        // Draw the cells
        for (int y = 0; y < GRID_HEIGHT; ++y)
        {
            for (int x = 0; x < GRID_WIDTH; ++x)
            {
                int idx = y * GRID_WIDTH + x;
                if (grid_next[idx])
                {
                    RectangleShape cell(Vector2f(PIXEL_SIZE, PIXEL_SIZE));
                    cell.setPosition(x * PIXEL_SIZE, y * PIXEL_SIZE);
                    cell.setFillColor(Color::White);
                    window.draw(cell);
                }
            }
        }

        window.display();

        // Swap the grids for the next iteration
        std::swap(grid_current, grid_next);

		//Make a second buffer swap if using normal or pinned memory
        if (flag_normal_or_pinned) {
            std::swap(d_grid, d_newGrid);
        }

        count++;
    }

    // Free allocated memory at the end of the program
    cudaFree(d_grid);
    cudaFree(d_newGrid);
	// Free allocated memory at the end of the program
    if (GameMemType == MemType::PINNED) {
        cudaFreeHost(grid_current);
        cudaFreeHost(grid_next);
    }
    else if (GameMemType == MemType::MANAGED) {
        cudaFree(grid_current);
        cudaFree(grid_next);
    }

    return 0;
}
