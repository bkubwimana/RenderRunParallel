#pragma once

#include<iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "laser.h"

using namespace sf;

class Utilities
{
public:

	Utilities();

	Font font;

	Texture textureStartWindow;
	Texture textureBackground;
	Texture textureLaserBlast;
	Texture textureSpaceShip;
	Texture centipedeHeadTexture;
	Texture centipedeBodyTexture;
	Texture textureSpider;

	Text ScoreMessageText;
	Text scoreText;
};