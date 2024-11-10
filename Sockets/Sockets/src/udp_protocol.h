#pragma once
#include<iostream>
#include "robot.h"

extern const int PIXEL_SIZE;

void runUdpServer(unsigned short port, ECE_SpaceShip& spaceShip, bool& firstPacket);
void runUdpClient(unsigned short port);