#pragma once
#include <sstream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <string>
#include <iostream>

// Create a class that will contain functions for processing the meshes in the scene
class ProcessMesh
{
public:
    // Constructor
    ProcessMesh();

    // Process Node
    void processNode(aiNode *node, const aiScene *scene, std::map<std::string, int> &meshNameToIndex);
};