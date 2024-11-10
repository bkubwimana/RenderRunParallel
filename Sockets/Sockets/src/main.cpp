// main.cpp
/*
Author: Benjamin Kubwimana
Class: ECE4122 or ECE6122 (section Q)
Last Date Modified: 11/09/2024
Description:
Program that uses SFML and sockets to move drone a remote window.
*/

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <thread>
#include "utilities.h"
#include "udp_protocol.h"

using namespace std;
using namespace sf;

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 640;
const float INPUT_DELAY = 0.05f;



// Main function
int main()
{
	//Client or server

	char type;
	cout << "Server or Client? (s/c): ";
	cin >> type;

	std::thread ServerThreads;
	std::thread clientThreads;

	unsigned short ServerPort = 3000;

	if (type == 's' || type == 'S') {
		Utilities utilities;


		// Create a video mode object
		unsigned int width = WINDOW_WIDTH;

		unsigned int height = WINDOW_HEIGHT;

		// Create a window with a title and dimensions
		sf::RenderWindow window(sf::VideoMode(width, height), "Video Drone Bot");
		// Set FrameRate 
		window.setFramerateLimit(240);
		window.setView(window.getDefaultView());


		// Create a sprite
		Sprite spriteBackground;

		// Attach the texture to the sprite
		spriteBackground.setTexture(utilities.textureBackground);
		// Scale the background to fit the window size
		sf::Vector2u backgroundTextureSize = utilities.textureBackground.getSize();
		float backgroundScaleX = static_cast<float>(WINDOW_WIDTH) / backgroundTextureSize.x;
		float backgroundScaleY = static_cast<float>(WINDOW_HEIGHT) / backgroundTextureSize.y;
		spriteBackground.setScale(backgroundScaleX, backgroundScaleY);


		std::cout << "Window Size: " << window.getSize().x << "x" << window.getSize().y << std::endl;
		cout << "Scaled BG size: " << spriteBackground.getGlobalBounds().width << "x" << spriteBackground.getGlobalBounds().height << endl;


		/*
		****************************************
		Initialize the robot objects
		****************************************
		*/

		// Create the sprite and set its texture
		sf::Sprite spaceShipSprite;
		float spaceshipX = width / 2.0f;
		float spaceshipY = height / 2.0f;

		// Instantiate the spaceship
		ECE_SpaceShip spaceShip(spaceshipX, spaceshipY, utilities.textureSpaceShip);


		bool firstPacket = false;

		ServerThreads = std::thread(runUdpServer, ServerPort, ref(spaceShip), ref(firstPacket));
		int result = system("start sockets_sfml_app.exe");
		if (result != 0) {
			cout << "Error starting the program. Return code: " << result << endl;
		}


		//Display the window and draw the spaceship
		while (window.isOpen())
		{
			Event event;
			while (window.pollEvent(event))
			{

				if (event.type == sf::Event::Closed)
					window.close();

				if (Keyboard::isKeyPressed(Keyboard::Escape))
					window.close();
			}

			window.clear();

			window.draw(spriteBackground);

			if (firstPacket) spaceShip.DrawSpaceShip(window);

			window.display();
		}
	}

	else if (type == 'c' || type == 'C') {
		//char dataOut[] = "Test from client";
		clientThreads = std::thread(runUdpClient, ServerPort);
	}
	else {
		cout << "Invalid input" << endl;
	}


	if (ServerThreads.joinable()) ServerThreads.join();
	if (clientThreads.joinable()) clientThreads.join();
	/*system("taskill /IM sockets_sfml_app.exe /F");*/
	return 0;
}


