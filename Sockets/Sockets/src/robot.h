#ifndef ROBOT_H
#define ROBOT_H

#include <SFML/Graphics.hpp>
#include <list>
#include "utilities.h"

extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;
extern const float INPUT_DELAY;

// ECE_SpaceShip class declaration  
class ECE_SpaceShip : public sf::Sprite
{
public:

    ECE_SpaceShip(float x, float y, sf::Texture &texture);

    void Move(float xOffset, float yOffset);
    void DrawSpaceShip(sf::RenderWindow &window);
};


#endif // ROBOT_H
