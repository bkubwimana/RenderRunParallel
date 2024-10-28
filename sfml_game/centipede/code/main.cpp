/*
Author: Benjamin Kubwimana
Class: ECE4122 or ECE6122 (section Q)
Last Date Modified: 09/30/2024
Description:
A C++ program to create a simple version of the classic arcade game called Centipede using the SFML libary.
The game supports Level 1 of the game only and will restart over if all player lives are used. 
*/


// C++ libraries here
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <list>
#include <random>
#include "centipede.h"
#include "constants.h"
#include "mushroom.h"
#include "laser.h"
#include "utilities.h"

using namespace sf;
using namespace std;

void DirectionSetter(int windowWidth, int windowHeight, ECE_SpaceShip& spaceShip, Clock& movementClock);

int main()
{

	Utilities utilities;


	// Create a video mode object
	unsigned int height = 560;
	unsigned int width = 960;
	int currentFrame = 0;

	// Create a window with a title and dimensions
	sf::RenderWindow window(sf::VideoMode(width, height), "Retro Centipede Arcade Game!");
	// Set FrameRate 
	window.setFramerateLimit(240);

	// Create a sprite
	Sprite spriteStartWindow;
	Sprite spriteBackground;

	// Attach the texture to the sprite
	spriteStartWindow.setTexture(utilities.textureStartWindow);
	spriteBackground.setTexture(utilities.textureBackground);

	/*
	****************************************
	Initialize the game objects
	****************************************
	*/

	list<Mushroom> mushrooms;

	vector<ECE_Centipede> centipedes;
	ECE_Centipede temp1(NUM_SEGMENTS, CENTIPEDE_SPEED, width / 2 - (utilities.centipedeHeadTexture.getSize().x / 2), 32);
	centipedes.push_back(temp1);


	// Place spaceship at the bottom center of the game window
	float spaceshipX = (width / 2) - (utilities.textureSpaceShip.getSize().x / 2);
	float spaceshipY = height - utilities.textureSpaceShip.getSize().y - 10;

	// Instantiate the spaceship
	ECE_SpaceShip spaceShip(spaceshipX, spaceshipY, utilities.textureSpaceShip);

	//	Instantiate the Spider
	ECE_Spider Spider(100, 500, utilities.textureSpider);

	// Create a list of laser blasts
	std::list<ECE_LaserBlast> laserBlasts;
	laserBlasts.emplace_back(spaceshipX, spaceshipY, utilities.textureLaserBlast);

	// Variables to control time
	sf::Clock clock;
	sf::Clock movementClock;
	sf::Clock firingClock;  
	sf::Clock AnimateClock;

	bool paused = true;

	int Score = 0;
	int Lives = 3;
	Sprite LivesShip;

	/*
	bool MainMenuState = true;
	bool GameActiveState = false;
	bool GamePauseState = false;
	*/

	bool InStartMode = true;
	bool acceptInput = false;
	bool spaceBarPressed = false;

	bool SpiderAlive = true;

	// Check window limits and apply movement based on the grid
	float windowWidth = window.getSize().x;  // Window width
	float windowHeight = window.getSize().y; // Window height


	/*
	****************************************
	Start the game loop
	****************************************
	*/


	while (window.isOpen() && Lives > 0 && centipedes.size() > 0)
	{
		float deltaTimeAnimate = AnimateClock.restart().asSeconds(); // Get time since last frame
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyReleased && !paused)
				acceptInput = true; //enable Input

			if (event.type == sf::Event::Closed)
				window.close();

			if (Keyboard::isKeyPressed(Keyboard::Escape))
				window.close();
		}

		// Handle pressing Enter to start/reset the game
		if (Keyboard::isKeyPressed(Keyboard::Return))
		{

			if (!acceptInput && clock.getElapsedTime().asSeconds() > INPUT_DELAY)
			{
				acceptInput = true; // Re-enable input after the delay
				clock.restart();    // Reset the clock
			}

			if (InStartMode)
			{
				InStartMode = false;
				paused = false;
				acceptInput = true; // Enable input
				if (mushrooms.empty())
				{
					locateMushrooms(mushrooms, width, height - 60); // Only call once at the start
				}
			}
			else if (!InStartMode && !paused)
			{
				paused = true;
				acceptInput = false;
				// cout << "Game Paused" << endl;
			}
			else if (paused) // If paused and Enter is pressed, reset the game
			{
				paused = false;
				//Score = 0;
				acceptInput = true; // Enable input
				cout << "Game Resumed" << endl;
			}
		}

		// Then when you handle input, disable it after one movement and reset the movement clock
		if (movementClock.getElapsedTime().asSeconds() > INPUT_DELAY)
		{
			DirectionSetter(windowWidth, windowHeight, spaceShip, movementClock);
		}

		// Handle spacebar press to fire a laser (independent clock)
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && firingClock.getElapsedTime().asSeconds() > 0.23f)
		{
			if (!spaceBarPressed)
			{
				spaceShip.FireLaser(laserBlasts, utilities.textureLaserBlast);
				firingClock.restart(); // Restart the firing clock after shooting a laser
				spaceBarPressed = true; // Set the flag to true to indicate the spacebar is pressed
			}
		}
		else
		{
			spaceBarPressed = false; // Reset the flag when the spacebar is not pressed
		}

		// Unload lasers upward
		for (auto& laser : laserBlasts)
		{
			laser.MoveLaserBlast();
		}

		// Check for collisions with mushrooms and Centipede
		for (auto iterLaser = laserBlasts.begin(); iterLaser != laserBlasts.end();)
		{
			bool laserErased = false;

			// Check collision with mushrooms
			iterLaser->CheckMushroomCollision(mushrooms, laserBlasts, iterLaser, Score);

			if (iterLaser->CheckSpiderCollision(Spider, laserBlasts, iterLaser, Score))
			{
				cout << "Spider Dead";
				srand(static_cast<unsigned int>(time(0)));

				if (rand() % 2 == 1)
				{
					Spider.setPosition(100, 500);

				}
				else
				{
					Spider.setPosition(860, 500);

				}

			}
			if (iterLaser == laserBlasts.end()) {
				laserErased = true; // The laser was erased, exit early
			}

			if (!laserErased)
			{
				// Check collision with centipedes only if the laser was not erased
				int divider;

				if (iterLaser->CheckCentipedeCollision(centipedes, laserBlasts, iterLaser, Score, divider))
				{
					cout << "Point of Impact: " << divider << endl;
				}

				if (iterLaser == laserBlasts.end()) {
					laserErased = true;
				}
			}

			// Increment the iterator only if the laser was not erased
			if (!laserErased) {
				++iterLaser;
			}
		}

		if (spaceShip.CheckCentipedeCollisionwithSpaceShip(centipedes, spaceShip, Lives))
		{
			spaceShip.setPosition(spaceshipX, spaceshipY);
			centipedes.clear();
			centipedes.push_back(temp1);
		}

		// Remove lasers that have moved off-screen
		laserBlasts.remove_if([](const ECE_LaserBlast& laser)
			{ return laser.getPosition().y < 0; });
		// std::cout << "Number of centipedes before removal: " << centipedes.size() << std::endl;
		// Remove empty centipedes after splitting
		centipedes.erase(
			std::remove_if(centipedes.begin(), centipedes.end(), [](ECE_Centipede& c) {
				return c.isEmpty();
			}),
			centipedes.end()
		);


		/*
		****************************************
		Draw the scene
		****************************************
		*/
		window.clear();

		if (InStartMode)
		{
			window.draw(spriteStartWindow);
		}
		else
		{
			window.draw(spriteBackground);
			utilities.scoreText.setString(to_string(Score));
			window.draw(utilities.ScoreMessageText);
			window.draw(utilities.scoreText);

			for (int n = 0; n < Lives; n++)
			{
				LivesShip.setTexture(utilities.textureSpaceShip);
				LivesShip.setPosition(n * 30 + 960 / 4 + 960 / 2, 10);
				window.draw(LivesShip);
			}

			// Draw mushrooms
			for (const auto& mushroom : mushrooms)
			{
				window.draw(mushroom.SpriteMushroom);
			}

			// Draw the Centipede
			for (auto& centipede : centipedes)
			{
				centipede.MoveCentipede(mushrooms);
				centipede.DrawCentipede(window, currentFrame);
			}



			// Draw Spider
			Spider.DrawSpider(window);
			Spider.MoveSpider(mushrooms);
			if (Spider.CheckSpiderPlayerCollision(spaceShip))
			{
				Lives--;
				Spider.setPosition(100, 500);
				spaceShip.setPosition(spaceshipX, spaceshipY);

				centipedes.clear();
				ECE_Centipede temp2(NUM_SEGMENTS, CENTIPEDE_SPEED, width / 2 - (utilities.centipedeHeadTexture.getSize().x / 2), 32);
				centipedes.push_back(temp2);
			}



			// Draw the spaceship
			spaceShip.DrawSpaceShip(window);

			// Draw the laser blasts
			for (const auto& laser : laserBlasts)
			{
				laser.DrawLaserBlast(window);
			}

			if (Lives == 0)
			{
				Lives = 3;
				Score = 0;
				InStartMode = true;
				centipedes.clear();
				mushrooms.clear();

				ECE_Centipede temp2(NUM_SEGMENTS, CENTIPEDE_SPEED, width / 2 - (utilities.centipedeHeadTexture.getSize().x / 2), 32);
				centipedes.push_back(temp2);
			}
			if (centipedes.size() == 0)
			{
				centipedes.clear();
				// mushrooms.clear();

				ECE_Centipede temp2(NUM_SEGMENTS, CENTIPEDE_SPEED, width / 2 - (utilities.centipedeHeadTexture.getSize().x / 2), 32);
				centipedes.push_back(temp2);
				// locateMushrooms(mushrooms, width, height - 60);

			}
		}

		window.display();
	}

	return 0;
}



void DirectionSetter(int windowWidth, int windowHeight, ECE_SpaceShip& spaceShip, Clock& movementClock)
{
	sf::Vector2f position = spaceShip.getPosition();
	int xDirection = 0, yDirection = 0;

	// Horizontal movement
	if (Keyboard::isKeyPressed(Keyboard::Right) && position.x < 960 - GRID_CELL_SIZE)
	{
		xDirection = 1;
		spaceShip.move(2, 0); // Move right
	}
	if (Keyboard::isKeyPressed(Keyboard::Left) && position.x > 0)
	{
		xDirection = -1;
		spaceShip.move(-2, 0); // Move left
	}

	// Vertical movement
	if (Keyboard::isKeyPressed(Keyboard::Up) && position.y > 300)
	{
		yDirection = -1;
		spaceShip.move(0, -2); // Move up
	}
	if (Keyboard::isKeyPressed(Keyboard::Down) && position.y < 560 - GRID_CELL_SIZE)
	{
		yDirection = 1;
		spaceShip.move(0, 2); // Move down
	}

}
