#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <list>
#include <random>
#include <deque>

// Class definition for the mushrooms state and location
class Mushroom
{
public:
    // constructor
    Mushroom(int x, int y);

    // member variables
    int x;
    int y;
    int hits;


    sf::Texture textureMushroom;
    sf::Texture textureHitMushroom;

    sf::Sprite SpriteMushroom;
    sf::Sprite SpriteHitMushroom;

};

    // Function to randomly locate 30 mushrooms in the game play area
    void locateMushrooms(std::list<Mushroom>& mushrooms, float width, float height);

