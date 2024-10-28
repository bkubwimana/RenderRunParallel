

#include <sstream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "constants.h"
#include "centipede.h"
#include "mushroom.h"
#include "laser.h"
#include <cstdlib> 
#include <ctime>   


using namespace std;

/*
****************************************
Laser Class Implementation
****************************************
*/

// Constructor for LaserBlast
ECE_LaserBlast::ECE_LaserBlast(float x, float y, sf::Texture& textureLaserBlast) {
	setTexture(textureLaserBlast);  // Set texture
	setPosition(x, y);  // Set initial position
}

// Moves the laser blast upward
void ECE_LaserBlast::MoveLaserBlast() {
	move(0, -LASER_SPEED);  // Move the laser upward
}

// draw the laser blast 
void ECE_LaserBlast::DrawLaserBlast(sf::RenderWindow& window) const {
	window.draw(*this);  // Use the sprite's draw method
}

void ECE_LaserBlast::CheckMushroomCollision(std::list<Mushroom>& mushrooms, std::list<ECE_LaserBlast>& laserBlasts, std::list<ECE_LaserBlast>::iterator& laserIt, int& Score) {
	for (auto it = mushrooms.begin(); it != mushrooms.end();)
	{
		Mushroom& mushroom = *it;

		// Check if the laser intersects with the mushroom
		if (getGlobalBounds().intersects(mushroom.SpriteMushroom.getGlobalBounds()))
		{
			mushroom.hits++;  // Increment the mushroom's hit counter

			if (mushroom.hits == 1)
			{
				mushroom.SpriteMushroom.setTexture(mushroom.textureHitMushroom);
			}
			else if (mushroom.hits == 2)
			{
				Score += 4;
				it = mushrooms.erase(it);
			}
			else
			{
				++it;  // Move to the next mushroom if not removed
			}

			laserIt = laserBlasts.erase(laserIt);  // Erase the laser from the list
			break;  // Exit the loop after one collision
		}
		else
		{
			++it;
		}
	}
}

bool ECE_LaserBlast::CheckCentipedeCollision(std::vector<ECE_Centipede>& centipedes, std::list<ECE_LaserBlast>& laserBlasts, std::list<ECE_LaserBlast>::iterator& laserIt, int& Score, int& Divider) {
	for (auto centipedeIt = centipedes.begin(); centipedeIt != centipedes.end();) {
		ECE_Centipede& centipede = *centipedeIt;
		const std::vector<sf::Sprite>& segments = centipede.getSegments();

		for (auto segmentIt = segments.begin(); segmentIt != segments.end(); ++segmentIt) {
			const sf::Sprite& segment = *segmentIt;
			// Check if laser blast collides with a centipede segment
			if (getGlobalBounds().intersects(segment.getGlobalBounds())) {
				laserIt = laserBlasts.erase(laserIt);  // Erase the laser blast

				// Calculate the segmentID
				int segmentID = std::distance(segments.begin(), segmentIt);

				if (segmentID == 0) {
					centipede.centipede.erase(centipede.centipede.begin());
					Score += 100;
				}
				else if (segmentID == segments.size() - 1) {
					centipede.centipede.pop_back();
					Score += 10;
				}
				else {
					Score += 10;
					Vector2f pos = segments[0].getPosition();
					int mid = segmentID;
					Divider = segmentID;
					int max = centipede.numSegments;

					// Remove the current centipede from the vector
					centipedeIt = centipedes.erase(centipedeIt);  // Erase and get the next valid iterator

					// Create new centipedes
					ECE_Centipede temp1(mid, CENTIPEDE_SPEED, pos.x, pos.y);
					temp1.direction.x = -1.0f;
					centipedes.push_back(temp1);
					ECE_Centipede temp2(max - mid - 1, CENTIPEDE_SPEED, pos.x, pos.y + 32);
					temp2.direction.x = 1.0f;
					centipedes.push_back(temp2);

					return true;  // Exit after handling the collision
				}
				return true;  // Exit after handling the collision
			}
		}
		++centipedeIt;  // Increment the centipede iterator if no collision
	}
	return false;
}

bool ECE_LaserBlast::CheckSpiderCollision(sf::Sprite& head, std::list<ECE_LaserBlast>& laserBlasts, std::list<ECE_LaserBlast>::iterator& laserIt, int& Score)
{
	if (getGlobalBounds().intersects(head.getGlobalBounds()))
	{
		Score += 300;
		laserIt = laserBlasts.erase(laserIt);
		return true;
	}
	return false;
}

/*
****************************************
StarShip Class Implementation
****************************************
*/

// ECE_SpaceShip class implementation

// Constructor: Initialize the spaceship position and texture
ECE_SpaceShip::ECE_SpaceShip(float x, float y, sf::Texture& texture) {
	setTexture(texture);
	setPosition(x, y);
	setScale(1.50f, 1.50f);
}

// Method to move the spaceship
void ECE_SpaceShip::Move(float xOffset, float yOffset) {
	move(xOffset, yOffset);  // Move the spaceship by offset
}

void ECE_SpaceShip::FireLaser(std::list<ECE_LaserBlast>& laserBlasts, sf::Texture& laserTexture) {
	// Adjust the Y position so that the laser appears to fire from the spaceship's top
	float laserX = getPosition().x + (getGlobalBounds().width / 2) - (laserTexture.getSize().x / 2);
	float laserY = getPosition().y - 14;  // Top of spaceship

	ECE_LaserBlast newLaser(laserX, laserY, laserTexture);
	laserBlasts.push_back(newLaser);  // Add the new laser to the list
}

// Draw the spaceship
void ECE_SpaceShip::DrawSpaceShip(sf::RenderWindow& window) {
	window.draw(*this);  // Use the sprite's draw method
}

bool ECE_SpaceShip::CheckCentipedeCollisionwithSpaceShip(std::vector<ECE_Centipede>& centipedes, ECE_SpaceShip& spaceShip, int& lives)
{
	for (auto& centipede : centipedes) {  // Iterate directly over centipedes
		const std::vector<sf::Sprite>& segments = centipede.getSegments();

		for (const auto& segment : segments) {  // Iterate directly over segments
			// Check if the spaceship collides with any centipede segment
			if (spaceShip.getGlobalBounds().intersects(segment.getGlobalBounds())) {
				lives--;  // Decrease lives if a collision occurs
				// Optional: Add code to handle what happens after a collision, e.g., resetting spaceship position, etc.
				return true;  // Exit the function to avoid further processing
			}
		}
	}
	return false;  // Return false if no collision was detected
}

/*
****************************************
Spider Class Implementation
****************************************
*/
ECE_Spider::ECE_Spider(float x, float y, sf::Texture& texture)
{
	setTexture(texture);  // Set texture
	setPosition(x, y);
	Selector = 0;
	distanceTraveled = 0;
	MoveCounter = 0;

	srand(static_cast<unsigned int>(time(0)));

	int randomNum = rand() % 2;
	if (randomNum == 0)
	{
		direction.x = 1.0f;
		direction.y = -1.0f;
	}
	else
	{
		direction.x = -1.0f;
		direction.y = 1.0f;
	}

}

void ECE_Spider::DrawSpider(sf::RenderWindow& window)
{
	window.draw(*this);  // Use the sprite's draw method
}

void ECE_Spider::MoveSpider(std::list<Mushroom>& mushrooms)
{
	float distanceToMoveX = direction.x * SPIDER_SPEED;
	float distanceToMoveY = direction.y * SPIDER_SPEED;
	float temp1 = distanceToMoveX;
	float temp2 = distanceToMoveY;

	if (distanceToMoveX < 0)
		temp1 = -1 * distanceToMoveX;
	if (distanceToMoveY < 0)
		temp2 = -1 * distanceToMoveY;

	distanceTraveled += (temp1 + temp2);

	if (distanceTraveled > GRID_CELL_SIZE * 12)
	{
		distanceTraveled = 0;
		srand(static_cast<unsigned int>(time(0)));
		int r1;
		do {
			r1 = rand() % 3; // Generate a new random value
		} while (r1 == Selector);


		if (r1 == 0)
		{
			direction.x = -(direction.x);
		}
		else if (r1 == 1)
		{
			direction.y = -(direction.y);
		}
		else
		{
			direction.y = -(direction.y);
			direction.x = -(direction.x);
		}

		do {
			r1 = rand() % 3; // Generate a new random value
		} while (r1 == Selector);
		if (r1 == 0)
		{
			Selector = 0;
		}
		else if (r1 == 1)
		{
			Selector = 1;
		}
		else if (r1 == 2)
		{
			Selector = 2;
		}
	}

	if (Selector == 0)
	{
		this->move(distanceToMoveX / 2, distanceToMoveY / 2);
	}
	if (Selector == 1)
	{
		this->move(distanceToMoveX, 0);
	}
	if (Selector == 2)
	{
		this->move(0, distanceToMoveY / 2);
	}

	// Check for boundary or mushroom collisions and reverse direction if needed
	if (CheckCollisionWithMushrooms(mushrooms, this) || this->getPosition().y < 300 || this->getPosition().x < 0 || this->getPosition().x > 960 - GRID_CELL_SIZE || this->getPosition().y > 560 - GRID_CELL_SIZE)
	{
		if (this->getPosition().y < 300)
		{
			direction.y = 1.0f;
		}
		if (this->getPosition().x < 0)
		{
			direction.x = 1.0f;
		}
		if (this->getPosition().x > 960 - GRID_CELL_SIZE)
		{
			direction.x = -1.0f;
		}
		if (this->getPosition().y > 560 - GRID_CELL_SIZE)
		{
			direction.y = -1.0f;
		}
	}
}

bool ECE_Spider::CheckCollisionWithMushrooms(std::list<Mushroom>& mushrooms, sf::Sprite* head)
{
	for (auto mushroomIt = mushrooms.begin(); mushroomIt != mushrooms.end(); /* increment inside loop */)
	{
		if (head->getGlobalBounds().intersects(mushroomIt->SpriteMushroom.getGlobalBounds()))
		{
			mushroomIt = mushrooms.erase(mushroomIt); // Remove the mushroom and get the new iterator
			return true; // Collision detected and mushroom removed
		}
		else
		{
			++mushroomIt; // Increment iterator if no collision
		}
	}
	return false;
}

bool ECE_Spider::CheckSpiderPlayerCollision(sf::Sprite& head)
{
	if (this->getGlobalBounds().intersects(head.getGlobalBounds()))
	{
		return true; // Collision detected
	}

	return false;
}