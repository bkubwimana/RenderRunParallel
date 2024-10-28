/*
Author: Benjamin Kubwimana
Class: ECE4122 or ECE6122 (section Q)
Last Date Modified: 10/3/2024
Description:

A project to apply principles of multithreading using std::thread and OpenMP for parallel
calculations in a computationally-intensive problem related playing John Conway’s Game of Life

Most of logic is implmented in the main.cpp file.
1. We import files from the thirdparty folder to help with command line parsing with clip through the cli_parser.h file
2. We also import the threadpool.h file to help with the threadpooling logic

Average time on server machine with 16 cores:

1. Sequential: 15000us
2. Threads: 5000us
3. OpenMP: 3200us
*/


#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "cli_parser.h"
#include <random>
#include <iomanip>
#include "threadpool.h"

using namespace std;
using namespace clipp;
using namespace sf;

int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;
int PIXEL_SIZE = 5;
int GRID_WIDTH = WINDOW_WIDTH / PIXEL_SIZE;
int GRID_HEIGHT = WINDOW_HEIGHT / PIXEL_SIZE;
int NUM_THREADS = 8;
const int TILE_SIZE = 12;

void seedRandomGrid(vector<vector<bool>> &grid, Mode mode)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, 1);
    srand(static_cast<unsigned>(time(nullptr)));

    auto seedFunction = [&](int xStart, int yStart, int yEnd)
    {
        for (int y = yStart; y < yEnd; ++y)
        {
            grid[xStart][y] = dist(gen); // Randomly seed each pixel
        }
    };

    if (mode == Mode::OpenMp)
    {
#pragma omp parallel for schedule(static) num_threads(NUM_THREADS)
        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            seedFunction(x, 0, GRID_HEIGHT);
        }
    }
    else if (mode == Mode::Threads)
    {
        vector<thread> threads;
        int rowsPerThread = GRID_HEIGHT / NUM_THREADS;
        int extraRows = GRID_HEIGHT % NUM_THREADS;
        int currentRow = 0;

        // spawn threads for seedFunction based on chunks and number of threads
        for (int i = 1; i <= NUM_THREADS; i++)
        {
            int startRow = currentRow;
            int endRow = currentRow + rowsPerThread;

            if (i == NUM_THREADS)
            {
                endRow += extraRows;
            }
            threads.emplace_back(thread([&, startRow, endRow]()
                                        {
                for (int x = 0; x < GRID_WIDTH; ++x)
                {
                    seedFunction(x, startRow, endRow);
                } }));

            currentRow = endRow;
        }
        for (auto &thread : threads)
        {
            thread.join();
        }
        threads.clear();
    }
    if (mode == Mode::Sequential)
    {
        srand(static_cast<unsigned>(time(nullptr)));
        for (int x = 0; x < GRID_WIDTH; ++x)
            for (int y = 0; y < GRID_HEIGHT; ++y)
                grid[x][y] = (rand() % 2 == 0);
    }
}

int countNeighbors(const vector<vector<bool>> &grid, int x, int y)
{
    int countNeighb = 0;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            if (i == 0 && j == 0)
            {
                continue;
            }
            int nx = (x + i + GRID_WIDTH) % GRID_WIDTH;
            int ny = (y + j + GRID_HEIGHT) % GRID_HEIGHT;
            countNeighb += grid[nx][ny];
        }
    }
    return countNeighb;
}

void updateGrid(vector<vector<bool>> &grid, vector<vector<bool>> &newGrid, Mode mode, ThreadPool &pool)
{
    auto updateFunc = [&](int xStart, int xEnd, int yStart, int yEnd)
    {
        for (int x = xStart; x < xEnd; ++x)
        {
            for (int yTile = yStart; yTile < yEnd; yTile += TILE_SIZE)
            {
                for (int y = yTile; y < min(yTile + TILE_SIZE, yEnd); ++y)
                {
                    int neighbors = countNeighbors(grid, x, y);

                    if (grid[x][y])
                    {
                        if (neighbors < 2 || neighbors > 3)
                        {
                            newGrid[x][y] = false; // Cell dies
                        }
                    }
                    else
                    {
                        if (neighbors == 3)
                        {
                            newGrid[x][y] = true; // Cell becomes alive
                        }
                    }
                }
            }
        }
    };

    // If mode is Threads, update the grid in parallel

    if (mode == Mode::Threads)
    {
        int rowsPerThread = GRID_HEIGHT / NUM_THREADS;
        int extraRows = GRID_HEIGHT % NUM_THREADS;
        int currentRow = 0;

        vector<future<void>> futures;

        // spawn threads for updateFunc based on chunks and number of threads
        for (int i = 1; i <= NUM_THREADS; i++)
        {
            int startRow = currentRow;
            int endRow = currentRow + rowsPerThread;

            if (i == NUM_THREADS)
            {
                endRow += extraRows;
            }
            futures.emplace_back(pool.enqueue([=, &grid, &newGrid, &updateFunc]()
                                              {
                for (int x = 0; x < GRID_WIDTH; x += TILE_SIZE)
                {
                    updateFunc(x, min(x + TILE_SIZE, GRID_WIDTH), startRow, endRow);
                } }));

            currentRow = endRow;
        }

        // Wait for all tasks to finish

        for (auto &future : futures)
        {
            future.get();
        }
    }

    // If mode is OpenMp, update the grid in parallel

    else if (mode == Mode::OpenMp)
    {
#pragma omp parallel for schedule(dynamic) num_threads(NUM_THREADS)
        for (int x = 0; x < GRID_WIDTH; x += TILE_SIZE)
        {
            updateFunc(x, min(x + TILE_SIZE, GRID_WIDTH), 0, GRID_HEIGHT);
        }
    }

    // If mode is sequential, update the grid sequentially
    else if (mode == Mode::Sequential)
    {
        for (int x = 0; x < GRID_WIDTH; ++x) 
        {
            for (int y = 0; y < GRID_HEIGHT; ++y) 
            {
                int neighbors = countNeighbors(grid, x, y);

                if (grid[x][y]) 
                {
                    if (neighbors < 2 || neighbors > 3)
                    {
                        newGrid[x][y] = false;  // Cell dies
                    }
                }
                else 
                {
                    if (neighbors == 3)
                    {
                        newGrid[x][y] = true;  // Cell becomes alive
                    }
                }
            }
        }
    }
}

void printArguments(const CommandLineParser &parser)
{
    cout << "-x (Width): " << WINDOW_WIDTH << endl;
    cout << "-y (Height): " << WINDOW_HEIGHT << endl;
    cout << "-c (Size): " << PIXEL_SIZE << endl;
    cout << "-n (Threads): " << NUM_THREADS << endl;
}

string modeToString(Mode mode)
{
    switch (mode)
    {
    case Mode::Sequential:
        return "Sequential";
    case Mode::Threads:
        return "Threads";
    case Mode::OpenMp:
        return "OpenMp";
    default:
        return "Unknown mode";
    }
}

int main(int argc, char *argv[])
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

    Mode GameMode = parser.getMode();
    WINDOW_WIDTH = parser.getWidth();
    WINDOW_HEIGHT = parser.getHeight();
    PIXEL_SIZE = parser.getSize();
    NUM_THREADS = parser.getCount();
    GRID_WIDTH = WINDOW_WIDTH / PIXEL_SIZE;
    GRID_HEIGHT = WINDOW_HEIGHT / PIXEL_SIZE;

    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Game of Life");
    window.setFramerateLimit(120); // Set frame rate to control speed

    vector<vector<bool>> grid_current(GRID_WIDTH, vector<bool>(GRID_HEIGHT, false));
    vector<vector<bool>> grid_next(GRID_WIDTH, vector<bool>(GRID_HEIGHT, false));
    vector<vector<bool>> &refCurrent = grid_current, &refNext = grid_next;

    // Seed the grid with random values and initialize the thread pool
    seedRandomGrid(grid_current, GameMode);
    ThreadPool pool((GameMode == Mode::Threads) ? NUM_THREADS : 0);

    // Initialize count and computeTime
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

        // if an Escape key is pressed, close the window
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }
        if (count % 100 == 0 && count != 0)
        {
            computeTime = 0;
            cout << "Reset @ count: " << count << endl;
        }

        if (count++ % 2)
        {
            refCurrent = grid_next;
            refNext = grid_current;
        }
        else
        {
            refCurrent = grid_current;
            refNext = grid_next;
        }
        computeStart = chrono::high_resolution_clock::now();

        updateGrid(refCurrent, refNext, GameMode, pool);

        // measure time taken to compute a single generation and accumulate to computeTime
        auto computEnd = chrono::high_resolution_clock::now();
        auto duration = chrono::duration<long double, micro>(computEnd - computeStart);
        computeTime += duration.count();

        if (count % 100 == 0 && count != 0)
        {
            cout << fixed << setprecision(3);
            cout << "Update @ count: " << count << endl;
            if (GameMode == Mode::Sequential)
                cout << "100 generations took: " << computeTime << "us" << " with " << modeToString(GameMode) << endl;
            else
                cout << "100 generations took: " << computeTime << "us" << " with " << NUM_THREADS << " threads in: " << modeToString(GameMode) << endl;
        }

        window.clear();

        for (int x = 0; x < GRID_WIDTH; ++x)
        {
            for (int y = 0; y < GRID_HEIGHT; ++y)
            {
                if (refNext[x][y])
                {
                    RectangleShape cell(Vector2f(PIXEL_SIZE, PIXEL_SIZE));
                    cell.setPosition(x * PIXEL_SIZE, y * PIXEL_SIZE);
                    cell.setFillColor(Color::White);
                    window.draw(cell);
                }
            }
        }

        window.display();
    }

    return 0;
}
