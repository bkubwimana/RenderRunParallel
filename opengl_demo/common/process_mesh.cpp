#include <sstream>
#include "process_mesh.h"

// Constructor
ProcessMesh::ProcessMesh() {}

// Process Node
void ProcessMesh::processNode(aiNode *node, const aiScene *scene, std::map<std::string, int> &meshNameToIndex)
{
    // Get the name of the node (group name from OBJ)
    std::string nodeName(node->mName.C_Str());

    // Process all the meshes associated with this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        unsigned int meshIndex = node->mMeshes[i];
        meshNameToIndex[nodeName] = meshIndex;
        std::cout << "Node " << nodeName << " has mesh index " << meshIndex << std::endl;
    }

    // Recursively process each child node
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene, meshNameToIndex);
    }
}