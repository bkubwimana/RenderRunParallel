#pragma once

#include<iostream>
#include <SFML/Graphics.hpp>

using namespace sf;

class Utilities
{
public:

	Utilities();

	Font font;

	Texture textureBackground;
	Texture textureLaserBlast;
	Texture textureSpaceShip;

	Text ScoreMessageText;
	Text scoreText;
};