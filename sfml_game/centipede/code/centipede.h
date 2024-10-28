#ifndef CENTIPEDE_H
#define CENTIPEDE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <deque>
#include "mushroom.h"  // Assuming you have a Mushroom class or struct
#include "constants.h" // For GRID_CELL_SIZE, etc.
#include "laser.h"

using namespace sf;
// Forward declare ECE_LaserBlast instead of including the whole header
class ECE_LaserBlast;

class ECE_Centipede 
{
public:
    ECE_Centipede(int numSegments, float segmentSpeed, float gameWidth, float gameHeight);

    void MoveCentipede(std::list<Mushroom>& mushrooms);

    void DrawCentipede(sf::RenderWindow& window,int &currentFrame);
  
    bool CheckCollisionWithMushrooms(std::list<Mushroom>& mushrooms, sf::Sprite& head);

    bool isEmpty() const; 

    void ReverseDirectionAndMoveDown(sf::Sprite& head);


    // Method to get the segments of the centipede
    const std::vector<sf::Sprite>& getSegments() const;

    float elapsedTime;
    std::vector<sf::Sprite> centipede;   // Store each segment as a sprite
    int numSegments;

    bool NewRow;

    Texture centipedeHeadTexture;
    Texture centipedeHeadleftTexture;
    Texture centipedeBodyTexture;
    Texture centipedeBodyleftTexture;
    sf::Vector2f direction;  // Moving direction (-1, 0) for left, (1, 0) for right, etc.

private:

    float segmentSpeed;
    float distanceCovered;
    float gameWidth, gameHeight;

    bool MoveUp;

    std::deque<sf::Vector2f> historyTable; // History table to store previous positions of the head


};

#endif // CENTIPEDE_H