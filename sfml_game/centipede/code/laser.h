#ifndef LASER_H
#define LASER_H

#include <SFML/Graphics.hpp>
#include <list>
#include "constants.h"
#include "centipede.h"
#include "mushroom.h"


class ECE_Centipede;
// ECE_LaserBlast class definition
class ECE_LaserBlast : public sf::Sprite
{
public:

    ECE_LaserBlast(float x, float y, sf::Texture &texture);

    void MoveLaserBlast();
    void CheckMushroomCollision(std::list<Mushroom> &mushrooms, std::list<ECE_LaserBlast> &laserBlasts, std::list<ECE_LaserBlast>::iterator &laserIt,int &Score);
    bool CheckCentipedeCollision(std::vector<ECE_Centipede> &centipedes, std::list<ECE_LaserBlast> &laserBlasts, std::list<ECE_LaserBlast>::iterator &laserIt, int& Score,int& Divider);
    bool CheckSpiderCollision(sf::Sprite& head, std::list<ECE_LaserBlast>& laserBlasts, std::list<ECE_LaserBlast>::iterator& laserIt, int& Score);
    void DrawLaserBlast(sf::RenderWindow &window) const;

};


// ECE_SpaceShip class definition
class ECE_SpaceShip : public sf::Sprite
{
public:

    ECE_SpaceShip(float x, float y, sf::Texture &texture);

    bool CheckCentipedeCollisionwithSpaceShip(std::vector<ECE_Centipede>& centipedes, ECE_SpaceShip &spaceShip,int &lives);

    void Move(float xOffset, float yOffset);
    void FireLaser(std::list<ECE_LaserBlast> &laserBlasts, sf::Texture &laserTexture);
    void DrawSpaceShip(sf::RenderWindow &window);
};

class ECE_Spider : public sf::Sprite
{
public:

    sf::Vector2f direction;
    float distanceCovered;
    int Selector;
    int MoveCounter;
    float distanceTraveled;

    ECE_Spider(float x, float y, sf::Texture& texture);

    void DrawSpider(sf::RenderWindow& window);

    void MoveSpider(std::list<Mushroom>& mushrooms);

    bool CheckCollisionWithMushrooms(std::list<Mushroom>& mushrooms, sf::Sprite* head);

    bool CheckSpiderPlayerCollision(sf::Sprite& head);

};



#endif // LASER_H
