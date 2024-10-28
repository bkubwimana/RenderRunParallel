// C++ libraries here
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <list>
#include <random>
#include <ctime>
#include <unordered_set>
#include "centipede.h"
#include "constants.h"
#include "mushroom.h"

using namespace sf;
using namespace std;

Mushroom::Mushroom(int x, int y) : x(x), y(y), hits(0)
{
    if (!textureMushroom.loadFromFile("graphics/Mushroom0.png"))
    {
        cout << "Error loading mushroom texture" << endl;
    }


    if (!textureHitMushroom.loadFromFile("graphics/Mushroom1.png"))
    {
        cout << "Error loading hit mushroom texture" << endl;
    }

    SpriteMushroom.setTexture(textureMushroom);
    SpriteMushroom.setTextureRect(sf::IntRect(0, 0, 32, 32));
    SpriteMushroom.setScale(0.9f, 0.9f);
    SpriteMushroom.setPosition(x+1, y+1);


    SpriteHitMushroom.setTexture(textureHitMushroom);
    SpriteHitMushroom.setPosition(x, y);
}

void locateMushrooms(std::list<Mushroom>& mushrooms, float width, float height)
{
    // Random engine setup
    unsigned seed = static_cast<unsigned>(time(0));
    std::mt19937 randomEngine(seed);

    // Grid calculations
    int fieldWidth = width;
    int fieldHeight = height;

    int numCols = fieldWidth / GRID_CELL_SIZE;  // Grid cell size is 32
    int numRows = fieldHeight / GRID_CELL_SIZE; // Grid cell size is 32

    // Set up random distribution for grid positions
    std::uniform_int_distribution<int> distX(1, numCols - 1); // Random column
    std::uniform_int_distribution<int> distY(1, numRows - 1); // Random row

    // Locate 30 mushrooms
    for (int i = 0; i < 30; i++)
    {
        // Get random grid coordinates
        int gridX = distX(randomEngine);
        int gridY = distY(randomEngine);

        // Convert grid coordinates to pixel positions
        int pixelX = gridX * 32;  // Mushrooms are 32x32, so scale by 32
        int pixelY = gridY * 32;

        mushrooms.emplace_back(pixelX, pixelY); // Add mushroom at grid-aligned position
    }
}
