/*
Author: Benjamin Kubwimana
Class: ECE4122 or ECE6122 (section Q)
Last Date Modified: 10/3/2024
Description:
A header file for the command line parser class for input arguemtns
*/

#pragma once

#include <clipp.h>
#include <iostream>
#include <string>

enum class Mode
{
    Sequential,
    Threads,
    OpenMp
};

class CommandLineParser {
public:

    //member variables
    int numThreads;
    int size;
    int width;
    int height;
    bool showHelp;
    // Constructor to initialize defaults
    CommandLineParser();

    // Method to parse command line arguments
    bool parseArguments(int argc, char** argv);

    // Accessor methods to retrieve parsed values
    Mode getMode() const { return mode; }
    int getCount() const { return numThreads; }
    int getSize() const { return size; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    Mode mode;
};
