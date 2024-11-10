#pragma once
#include <SFML/Network.hpp>
#include <iostream>
#include "robot.h"
#include "udp_protocol.h"

using namespace std;
using namespace sf;
//Launching a Server

const int PIXEL_SIZE = 10;

void runUdpServer(unsigned short port, ECE_SpaceShip& spaceShip, bool& firstPacket)
{
	sf::UdpSocket socket;
	//socket.setBlocking(false);

	if (socket.bind(port) != Socket::Done)
	{
		cout << "Failed to bind socket to port " << port << endl;
	}
	cout << "Server is listening to port " << port << ", waiting for a message... " << endl;
    
    //receiving message to client
	std::vector<char> dataIn(4);
	std::size_t received;
	sf::IpAddress client;
	unsigned short clientPort;
	string messageOut = "New robot location: ";
	int speed = 3;
	while (true) 
	{

        if (socket.receive(dataIn.data(), dataIn.size(), received, client, clientPort) == sf::Socket::Done)
        {
			if (!firstPacket)
			{
				firstPacket = true;
				cout << "Received first packet from client: " << clientPort << endl;
			}
            // Create the input string from the received data
            string input(dataIn.data(), received);

            // Trim any trailing null characters 
            input.erase(std::find(input.begin(), input.end(), '\0'), input.end());

            sf::Vector2f position = spaceShip.getPosition();
            cout << "Positions x: " << position.x << " y: "<<position.y << endl;

            // Movement conditions
            if (input == "d" && position.x + speed < WINDOW_WIDTH - PIXEL_SIZE) {
                cout << "Inside to move right" << endl;
                spaceShip.move(speed, 0);
                messageOut += "right";
            }
            if (input == "a" && position.x - speed > 0) {
                spaceShip.move(-speed, 0);
                messageOut += "left";
            }
            if (input == "w" && position.y - speed > 0 - PIXEL_SIZE) {
                spaceShip.move(0, -speed);
                messageOut += "up";
            }
            if (input == "s" && position.y + speed < WINDOW_HEIGHT - PIXEL_SIZE) {
                spaceShip.move(0, speed);
                messageOut += "down";
            }
            if (input == "q") {
				cout << "Client closing at port:" << clientPort <<endl;
                messageOut = "Server is shutting down...";
                socket.send(messageOut.c_str(), messageOut.size(), client, clientPort);
                break;
            }
            if (input == "g") {
				if (speed < 100) speed = min(speed*2, 100);
                messageOut = "Speed: " + to_string(speed) + " (pps)";
            }
            if (input == "h") {

				if (speed > 3) speed = max(speed/2, 3);
                messageOut = "Speed: " + to_string(speed) + " (pps)";
            }
        }
		// Send a message to the client
		if (socket.send(messageOut.c_str(), messageOut.size(), client, clientPort) != sf::Socket::Done)
		{
			cout << "Message: "<< clientPort << "failed to send" << endl;
		}

		messageOut = "Direction: ";
	}
}

//Creating a client

void runUdpClient(unsigned short ServerPort)
{
    sf::IpAddress serverIp;
    unsigned short ClientPort;
    while (true) {
        cout << "Enter Server IP: ";
        cin >> serverIp;
		if (serverIp == sf::IpAddress::None) continue;
		break;
    }
    while (true) {
        cout << "Enter your port number: ";
        cin >> ClientPort;
        if (cin.fail() || ClientPort <= 0 || ClientPort > 65535) {
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid port. Try again (1 to 65535)." << endl;
            continue;
        }
        break;
    }


    sf::UdpSocket socket;
    if (socket.bind(ClientPort) != sf::Socket::Done)
    {
        cout << "Failed to bind a client socket to port " << ClientPort << endl;
        return;
    }

    // Set non-blocking 
    socket.setBlocking(false);

    // Main loop for real-time input
    cout << "Enter a command (w, a, s, d) or 'q' to quit"<<endl;
    while (true)
    {
        // Check for key presses and send the corresponding command
        std::string command;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            command = "w";
			cout << "W pressed" << endl;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            command = "a";
			cout << "A pressed" << endl;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            command = "s";
			cout << "S pressed" << endl;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            command = "d";
			cout << "D pressed" << endl;    
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
            command = "g";
            cout << "G pressed" << endl;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
            command = "h";
            cout << "H pressed" << endl;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            command = "q";
			cout << "Q pressed" << endl;
        }

        // If a command was detected, send it to the server
        if (!command.empty())
        {
            std::vector<char> dataOut(command.begin(), command.end());
            if (socket.send(dataOut.data(), dataOut.size(), serverIp, ServerPort) != sf::Socket::Done)
            {
                cout << "Failed to send message to server" << endl;
            }

            // Small delay to prevent excessive packet sending
            sf::sleep(sf::milliseconds(100));
        }

        // Receive and print server response
        std::vector<char> dataIn(64);
        std::size_t received;
        sf::IpAddress sender;
        unsigned short senderPort;
        if (socket.receive(dataIn.data(), dataIn.size(), received, sender, senderPort) == sf::Socket::Done) {
            cout << "Received ACK from server: " << std::string(dataIn.data(), received) << endl;
        }

        // Exit condition if 'q' was pressed
        if (command == "q") {
            break;
        }
    }
}
