

#include<iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "utilities.h"

Utilities::Utilities() {
    if (!font.loadFromFile("fonts/KOMIKAP_.ttf")) {
        std::cerr << "Error loading KOMIKAP_.ttf" << std::endl;
    }

    if (!textureStartWindow.loadFromFile("graphics/StartupScreen.png")) {
        std::cerr << "Error loading StartupScreen.png" << std::endl;
    }

    if (!textureBackground.loadFromFile("graphics/background.png")) {
        std::cerr << "Error loading background.png" << std::endl;
    }

    if (!textureLaserBlast.loadFromFile("graphics/laser.png")) {
        std::cerr << "Error loading laser.png" << std::endl;
    }

    if (!textureSpaceShip.loadFromFile("graphics/StarShip.png")) {
        std::cerr << "Error loading StarShip.png" << std::endl;
    }

    if (!textureSpider.loadFromFile("graphics/spider.png")) {
        std::cerr << "Error loading spider.png" << std::endl;
    }

    if (!centipedeHeadTexture.loadFromFile("graphics/CentipedeHead.png")) {
        std::cerr << "Error loading CentipedeHead.png" << std::endl;
    }

    if (!centipedeBodyTexture.loadFromFile("graphics/CentipedeBody.png")) {
        std::cerr << "Error loading CentipedeBody.png" << std::endl;
    }

    ScoreMessageText.setFont(font);
    scoreText.setFont(font);

    ScoreMessageText.setCharacterSize(20);
    scoreText.setCharacterSize(20);

    ScoreMessageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);

    scoreText.setString("0");
    ScoreMessageText.setString("Score : ");

    scoreText.setPosition(960 / 2 + 25, 5);
    ScoreMessageText.setPosition(960 / 2 - 80, 5);
}

