#include <SFML/Graphics.hpp>
#include "centipede.h"
#include "constants.h"
#include "mushroom.h"
#include "laser.h"
#include <iostream>
#include <random>
#include <cstdlib>   // For rand() and srand()
#include <ctime> 

using namespace std;
using namespace sf;

// Constructor: Initialize the centipede
ECE_Centipede::ECE_Centipede(int numSegments, float segmentSpeed, float PosX, float PosY) : numSegments(numSegments), segmentSpeed(segmentSpeed), gameWidth(960), gameHeight(560), distanceCovered(0.0f)
{
	elapsedTime = 0;
	MoveUp = false;
	NewRow = true;

	if (!centipedeHeadTexture.loadFromFile("graphics/CentipedeHead.png")) {
		std::cerr << "Error loading CentipedeHead.png" << std::endl;
	}
	if (!centipedeBodyTexture.loadFromFile("graphics/CentipedeBody.png")) {
		std::cerr << "Error loading CentipedeBody.png" << std::endl;
	}
	if (!centipedeHeadleftTexture.loadFromFile("graphics/RotHead.png")) {
		std::cerr << "Error loading CentipedeBody.png" << std::endl;
	}
	if (!centipedeBodyleftTexture.loadFromFile("graphics/RotBody.png")) {
		std::cerr << "Error loading CentipedeBody.png" << std::endl;
	}
	srand(static_cast<unsigned int>(time(0)));

	int randomNum = rand() % 2;
	if (randomNum == 0)
	{
		direction = sf::Vector2f(1.0f, 0.0f); // Move right initially

	}
	else
	{
		direction = sf::Vector2f(-1.0f, 0.0f); // Move left initially
	}


	// Initialize centipede segments
	for (int i = 0; i < numSegments; ++i)
	{
		sf::Sprite segment;
		// Start at the middle of the screen and spread left or right
		segment.setPosition(PosX, PosY); // Start at the top row
		centipede.push_back(segment);
	}

	// Initialize the history table with enough size to handle segment spacing
	for (int i = 0; i < numSegments; ++i)
	{
		historyTable.push_back(centipede[0].getPosition());
	}
}

// Method to move the centipede
void ECE_Centipede::MoveCentipede(std::list<Mushroom>& mushrooms)
{
	// Move the head based on speed
	sf::Sprite& head = centipede[0];
	float distanceToMove = direction.x * segmentSpeed;

	head.move(distanceToMove, 0);

	// Track the distance covered by the head
	distanceCovered += std::abs(distanceToMove);


	// Move each segment to follow the one in front of it immediately
	for (int i = 1; i < numSegments; i++) // Start from the last segment
	{
		// Move the segment to the position of the segment in front
		//sf::Vector2f previousPosition = centipede[i - 1].getPosition();

		if (!centipede[i].getGlobalBounds().intersects(centipede[i - 1].getGlobalBounds()))
		{
			centipede[i].move(distanceToMove, 0); // Set to previous segment's position
		}
	}

	if (distanceCovered >= 23)
	{
		distanceCovered = 0;

		historyTable.push_front(head.getPosition());
		historyTable.pop_back(); // Keep history size fixed

		for (int i = 1; i < numSegments; ++i)
		{
			int delayFactor = 1; // This controls the distance between segments
			int historyIndex = i * delayFactor;

			if (historyIndex < historyTable.size())
			{
				centipede[i].setPosition(historyTable[historyIndex]);
			}
		}
	}
	//cout << "Size" << historyTable.size() << endl;

	// Check for boundary or mushroom collisions and reverse direction if needed
	if (CheckCollisionWithMushrooms(mushrooms, head) || head.getPosition().x < 0 || head.getPosition().x > gameWidth - GRID_CELL_SIZE)
	{
		ReverseDirectionAndMoveDown(head);
	}
}

// Method to reverse direction and move down by one grid cell
void ECE_Centipede::ReverseDirectionAndMoveDown(sf::Sprite& head)
{
	NewRow = true;
	// Reverse direction and move down
	direction.x = -direction.x;
	if (MoveUp)
	{
		head.move(0, -23);
	}
	else
	{
		head.move(0, 23);
	}

	// Wrap to top if it goes beyond the screen height
	if (head.getPosition().y >= gameHeight - GRID_CELL_SIZE - 20 && !MoveUp)
	{
		head.setPosition(head.getPosition().x, head.getPosition().y);
		MoveUp = true;
	}
	if (head.getPosition().y < 0 && MoveUp)
	{
		head.setPosition(head.getPosition().x, 0);
		MoveUp = false;
	}
}

// Method to check for collisions with mushrooms
bool ECE_Centipede::CheckCollisionWithMushrooms(std::list<Mushroom>& mushrooms, sf::Sprite& head)
{
	for (auto& mushroom : mushrooms)
	{
		if (head.getGlobalBounds().intersects(mushroom.SpriteMushroom.getGlobalBounds()))
		{
			return true; // Collision detected
		}
	}
	return false;
}



// Method to draw the centipede
void ECE_Centipede::DrawCentipede(sf::RenderWindow& window, int& currentFrame)
{
	for (size_t i = 0; i < centipede.size(); ++i)
	{
		if (i == 0)
		{
			// This is the head segment, apply head texture
			if (direction.x < 0)
			{
				centipede[i].setTexture(centipedeHeadleftTexture);
				window.draw(centipede[i]);
			}
			else
			{
				centipede[i].setTexture(centipedeHeadTexture);
				centipede[i].move(0, -2);
				window.draw(centipede[i]);
				centipede[i].move(0, 2);
			}
		}
		else
		{

			if (direction.x < 0)
			{
				centipede[i].setTexture(centipedeBodyleftTexture);
			}
			else
			{
				centipede[i].setTexture(centipedeBodyTexture);
			}
			//centipede[i].setScale(1.3f, 1.3f);
			if (!centipede[i].getGlobalBounds().intersects(centipede[i - 1].getGlobalBounds()))
			{
				window.draw(centipede[i]);
			}
		}

	}
}

bool ECE_Centipede::isEmpty() const
{
	return centipede.empty();
}

const std::vector<sf::Sprite>& ECE_Centipede::getSegments() const
{
	return centipede;
}