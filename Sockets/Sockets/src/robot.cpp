#include <sstream>
#include <vector>
#include <list>
#include "robot.h"
#include <cstdlib> 
#include <ctime>   


using namespace std;


// ECE_SpaceShip class implementation

// Constructor: Initialize the spaceship position and texture
ECE_SpaceShip::ECE_SpaceShip(float x, float y, sf::Texture& texture) {
	setTexture(texture);
	sf::Vector2u textureSize = texture.getSize();
	setScale(0.2f, 0.2f);
	setOrigin(textureSize.x / 2.0f, textureSize.y / 2.0f);
	setPosition(x, y);
}

// Method to move the spaceship
void ECE_SpaceShip::Move(float xOffset, float yOffset) {
	move(xOffset, yOffset);  // Move the spaceship by offset
}

// Draw the spaceship
void ECE_SpaceShip::DrawSpaceShip(sf::RenderWindow& window) {
	window.draw(*this);  // Use the sprite's draw method
}
