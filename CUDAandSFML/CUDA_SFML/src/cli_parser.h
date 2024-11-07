/*
Author: Benjamin Kubwimana
Last Date Modified: 11/07/2024
Description:
A header file for the command line parser class for input arguemtns
*/

#pragma once

#include <iostream>
#include <string>
#include "clipp.h"

enum class MemType
{
    NORMAL,
    PINNED,
    MANAGED,
};

class CommandLineParser {
public:

    //member variables
    int threadsPerBlock;
    int size;
    int width;
    int height;
    bool showHelp;
    // Constructor to initialize defaults
    CommandLineParser();

    // Method to parse command line arguments
    bool parseArguments(int argc, char** argv);

    // Accessor methods to retrieve parsed values
    MemType getMemType() const { return memType; }
    int getCount() const { return threadsPerBlock; }
    int getSize() const { return size; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    MemType memType;
};
