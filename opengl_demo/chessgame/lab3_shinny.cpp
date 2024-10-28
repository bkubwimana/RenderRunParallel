// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>
#include <control.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

GLuint selectPieceTexture(std::map<std::string, GLuint>& pieceTexturesMap, int row, int col)
{
	if (row == 1) {
		// Pawns for the second row (dark side)
		return pieceTexturesMap.at("wooddar0");  // Texture for pawns
	}
	else if (row == 6) {
		// Pawns for the seventh row (light side)
		return pieceTexturesMap.at("woodlig0");  // Texture for pawns
	}
	else if (row == 0) {
		// Major pieces for dark side (row 0)
		switch (col) {
		case 0: case 7: return pieceTexturesMap.at("wooddar1");  // Rooks
		case 1: case 6: return pieceTexturesMap.at("wooddar2");  // Knights
		case 2: case 5: return pieceTexturesMap.at("wooddar3");  // Bishops
		case 3: return pieceTexturesMap.at("wooddar4");  // Queen
		case 4: return pieceTexturesMap.at("wooddar5");  // King
		}
	}
	else if (row == 7) {
		// Major pieces for light side (row 7)
		switch (col) {
		case 0: case 7: return pieceTexturesMap.at("woodlig1");  // Rooks
		case 1: case 6: return pieceTexturesMap.at("woodlig2");  // Knights
		case 2: case 5: return pieceTexturesMap.at("woodlig3");  // Bishops
		case 3: return pieceTexturesMap.at("woodlig4");  // Queen
		case 4: return pieceTexturesMap.at("woodlig5");  // King
		}
	}
	std::cout << "No texture found for row " << row << " and column " << col << std::endl;
	return 0;  // No texture for empty squares
}

enum PieceType {
	// Dark pieces
	PAWN1,
	ROOK1,
	KNIGHT1,
	BISHOP1,
	QUEEN1,
	KING1,

	// Light pieces
	PAWN2,
	ROOK2,
	KNIGHT2,
	BISHOP2,
	QUEEN2,
	KING2,
	NUM_PIECE_TYPES
};
PieceType stringToPieceType(const std::string& pieceName) {
	if (pieceName == "PAWN1") return PAWN1;
	if (pieceName == "ROOK1") return ROOK1;
	if (pieceName == "KNIGHT1") return KNIGHT1;
	if (pieceName == "BISHOP1") return BISHOP1;
	if (pieceName == "QUEEN1") return QUEEN1;
	if (pieceName == "KING1") return KING1;
	if (pieceName == "PAWN2") return PAWN2;
	if (pieceName == "ROOK2") return ROOK2;
	if (pieceName == "KNIGHT2") return KNIGHT2;
	if (pieceName == "BISHOP2") return BISHOP2;
	if (pieceName == "QUEEN2") return QUEEN2;
	if (pieceName == "KING2") return KING2;
	return NUM_PIECE_TYPES;
}


PieceType selectPieceType(int row, int col) {
	if (row == 1 || row == 6) {
		// Pawns
		return row == 1 ? PAWN1 : PAWN2;
	}
	else if (row == 0 || row == 7) {
		// Major pieces
		switch (col) {
		case 0: case 7: return row == 0 ? ROOK1 : ROOK2;
		case 1: case 6: return row == 0 ? KNIGHT1 : KNIGHT2;
		case 2: case 5: return row == 0 ? BISHOP1 : BISHOP2;
		case 3: return row == 0 ? QUEEN1 : QUEEN2;
		case 4: return row == 0 ? KING1 : KING2;
		}
	}
	std::cout << "No piece found for row " << row << " and column " << col << std::endl;
	return NUM_PIECE_TYPES; // Empty square
}




glm::mat4 getPieceModelMatrix(int row, int col, PieceType pieceType, const glm::vec3& modelOriginOffset) {
	float spacing = 5.5f;  // Adjust as necessary

	// Calculate the center offset
	float boardCenterOffset = (spacing * 7) / 2.0f;

	// Translation to position the piece on the board
	float xPos = (col * spacing) - boardCenterOffset;
	float yPos = (row * spacing) - boardCenterOffset;
	float zPos = 0.0f;

	// Step 1: Translate model origin to (0,0,0)
	glm::mat4 translationToOrigin = glm::translate(glm::mat4(1.0f), -modelOriginOffset);

	// Step 2: Scaling (if needed)
	float scale = 1.1f; // Adjust as necessary
	glm::mat4 scaling = glm::scale(glm::mat4(1.0f), glm::vec3(scale));

	// Step 3: Rotation (rotate dark pieces by 180 degrees around Z-axis)
	glm::mat4 rotation = glm::mat4(1.0f);
	if (pieceType == PAWN1 || pieceType == ROOK1 || pieceType == KNIGHT1 ||
		pieceType == BISHOP1 || pieceType == QUEEN1 || pieceType == KING1) {
		rotation = glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0, 0, 1));
	}

	// Step 4: Translate to board position
	glm::mat4 translationToBoard = glm::translate(glm::mat4(1.0f), glm::vec3(xPos, yPos, zPos));

	// Combine transformations
	glm::mat4 modelMatrix = translationToBoard * rotation * scaling * translationToOrigin;

	return modelMatrix;
}






void processNode(aiNode* node, const aiScene* scene, std::map<std::string, int>& meshNameToIndex) {
	// Get the name of the node (group name from OBJ)
	std::string nodeName(node->mName.C_Str());

	// Process all the meshes associated with this node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		unsigned int meshIndex = node->mMeshes[i];
		meshNameToIndex[nodeName] = meshIndex;
		//std::cout << "Node " << nodeName << " has mesh index " << meshIndex << std::endl;
	}

	// Recursively process each child node
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, meshNameToIndex);
	}
}

int getMeshIndexForPieceType(PieceType pieceType, const std::map<std::string, int>& meshNameToIndex) {
	switch (pieceType) {
	case PAWN1: return meshNameToIndex.at("PAWN1");
	case PAWN2: return meshNameToIndex.at("PAWN2");
	case ROOK1: return meshNameToIndex.at("ROOK1");
	case ROOK2: return meshNameToIndex.at("ROOK2");
	case KNIGHT1: return meshNameToIndex.at("KNIGHT1");
	case KNIGHT2: return meshNameToIndex.at("KNIGHT2");
	case BISHOP1: return meshNameToIndex.at("BISHOP1");
	case BISHOP2: return meshNameToIndex.at("BISHOP2");
	case QUEEN1: return meshNameToIndex.at("QUEEN1");
	case QUEEN2: return meshNameToIndex.at("QUEEN2");
	case KING1: return meshNameToIndex.at("KING1");
	case KING2: return meshNameToIndex.at("KING2");
	default: return -1; // Invalid
	}
}

glm::vec3 calculateMeshBaseCenter(aiMesh* mesh) {
	const float epsilon = 1e-5;
	float minY = FLT_MAX;

	// Find minY
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		float y = mesh->mVertices[i].y;
		if (y < minY) {
			minY = y;
		}
	}

	// Collect base vertices
	std::vector<aiVector3D> baseVertices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		if (fabs(mesh->mVertices[i].y - minY) < epsilon) {
			baseVertices.push_back(mesh->mVertices[i]);
		}
	}

	// Calculate average X and Z
	float sumX = 0.0f;
	float sumZ = 0.0f;
	for (const auto& vertex : baseVertices) {
		sumX += vertex.x;
		sumZ += vertex.z;
	}
	float centerX = sumX / baseVertices.size();
	float centerZ = sumZ / baseVertices.size();

	return glm::vec3(centerX, minY, centerZ);
}




int main( void )
{
	// Initialize GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make macOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 09 - Loading with AssImp", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it is closer to the camera than the former one
	glDepthFunc(GL_LESS); 


	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Load the texture
	GLuint Texture = loadBMP_custom("12951_Stone_Chess_Board_diff.bmp");
	
	std::vector<GLuint> woodDarkTextures;
	std::vector<GLuint> woodLightTextures;
	std::map<std::string, GLuint> pieceTexturesMap;

	// Check for anisotropic filtering support
	GLfloat largestSupportedAnisotropy = 16.0f; // Default value if not supported
	if (glewIsExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largestSupportedAnisotropy);
		std::cout << "Anisotropic filtering is supported. Max level: " << largestSupportedAnisotropy << std::endl;
	}
	else {
		std::cout << "Anisotropic filtering is not supported." << std::endl;
	}



	// Assuming you've checked for anisotropic filtering support and stored largestSupportedAnisotropy

	for (int i = 0; i < 6; i++) {
		// Load the textures
		GLuint darkTexture = loadBMP_custom(("chess/wooddar" + std::to_string(i) + ".bmp").c_str());
		GLuint lightTexture = loadBMP_custom(("chess/woodlig" + std::to_string(i) + ".bmp").c_str());

		// Set texture parameters for dark texture
		glBindTexture(GL_TEXTURE_2D, darkTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set anisotropic filtering for dark texture
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largestSupportedAnisotropy);

		// Set texture parameters for light texture
		glBindTexture(GL_TEXTURE_2D, lightTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set anisotropic filtering for light texture
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, largestSupportedAnisotropy);

		// Store textures
		woodDarkTextures.push_back(darkTexture);
		woodLightTextures.push_back(lightTexture);
		pieceTexturesMap.emplace("wooddar" + std::to_string(i), darkTexture);
		pieceTexturesMap.emplace("woodlig" + std::to_string(i), lightTexture);
	}


	std::cout << "finished loading textures" << std::endl;

	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	bool res = loadAssImp("board/12951_Stone_Chess_Board_v1_L3.obj", indices, indexed_vertices, indexed_uvs, indexed_normals);
	std::cout << "finished board loading obj" << std::endl;
	// Load it into a VBO

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

	GLuint vaoBoard;
	glGenVertexArrays(1, &vaoBoard);
	glBindVertexArray(vaoBoard);

	// Bind the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Set vertex attribute pointer for positions (location = 0)
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute location
		3,                  // size (x, y, z)
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Bind the UV buffer
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	// Set vertex attribute pointer for UVs (location = 1)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute location
		2,                  // size (u, v)
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Bind the normal buffer
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	// Set vertex attribute pointer for normals (location = 2)
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                  // attribute location
		3,                  // size (nx, ny, nz)
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Bind the element buffer (indices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

	// Unbind the VAO to prevent accidental modification
	glBindVertexArray(0);

	//===============================================================================================================

	// Load chess pieces obj file

	// Create Assimp importer
	Assimp::Importer importer;

	// Load the chess pieces obj file
	const aiScene* scene = importer.ReadFile("chess/chess.obj", 
		aiProcess_Triangulate			|
		aiProcess_JoinIdenticalVertices	|
		aiProcess_SortByPType);
	
	// Check if the model was successfully loaded
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Error loading chess.obj: " << importer.GetErrorString() << std::endl;
		return -1;
	}

	std::cout << "finished chess piecess loading obj" << std::endl;


	// Store combined VBO for each piece type
	GLuint vboModelData[12]; // Assuming 6 distinct pieces

	// Create VAO for each piece type
	GLuint vaoPieces[12];

	// Store piece indices (using your existing variable)
	std::vector<std::vector<unsigned short>> pieceIndices(12);

	// Map to keep track of piece names and their mesh indices
	std::map<std::string, int> meshNameToIndex;
	processNode(scene->mRootNode, scene, meshNameToIndex);

	// Map to store offset of each piece's base center
	std::map<PieceType, glm::vec3> modelOriginOffsets;

	for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[i];

		// Store mesh data in one buffer (positions, normals, UVs)
		std::vector<float> vertexData;

		// Collect indices for faces
		std::vector<unsigned short> indices;

		for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
			// Push positions
			vertexData.push_back(mesh->mVertices[j].x);
			vertexData.push_back(mesh->mVertices[j].y);
			vertexData.push_back(mesh->mVertices[j].z);

			// Push normals
			vertexData.push_back(mesh->mNormals[j].x);
			vertexData.push_back(mesh->mNormals[j].y);
			vertexData.push_back(mesh->mNormals[j].z);

			// Push UVs (if they exist)
			if (mesh->mTextureCoords[0]) {
				vertexData.push_back(mesh->mTextureCoords[0][j].x);
				vertexData.push_back(mesh->mTextureCoords[0][j].y);
			}
			else {
				vertexData.push_back(0.0f);
				vertexData.push_back(0.0f);
			}
		}

		// Calculate the base center of the mesh
		glm::vec3 baseCenter = calculateMeshBaseCenter(mesh);

		// Find the piece type for this mesh from `meshNameToIndex` by key from index `i`
		PieceType pieceType = NUM_PIECE_TYPES;
		for (auto const& x : meshNameToIndex) {
			if (x.second == i) {
				pieceType = stringToPieceType(x.first);
				break;
			}
		}

		// Store the base center of the mesh
		if (pieceType != NUM_PIECE_TYPES) {
			modelOriginOffsets[pieceType] = baseCenter;
		}
		else {
			std::cerr << "Invalid piece type for mesh index: " << i << std::endl;
		}

		// Store face indices (pieceIndices)
		for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
			aiFace face = mesh->mFaces[j];
			for (unsigned int k = 0; k < face.mNumIndices; k++) {
				pieceIndices[i].push_back(face.mIndices[k]);  // Use `pieceIndices`
			}
		}

		// Generate VAO
		glGenVertexArrays(1, &vaoPieces[i]);
		glBindVertexArray(vaoPieces[i]);

		// Generate and bind VBO for combined vertex data
		glGenBuffers(1, &vboModelData[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vboModelData[i]);
		glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);

		// Set vertex attribute pointers (position=0, normal=1, UV=2)
		glEnableVertexAttribArray(0); // Position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

		glEnableVertexAttribArray(1); // Normal
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

		glEnableVertexAttribArray(2); // UV
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		// Generate and bind EBO for indices (replacing `indices` with `pieceIndices[i]`)
		GLuint ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pieceIndices[i].size() * sizeof(unsigned short), &pieceIndices[i][0], GL_STATIC_DRAW);

		// Unbind VAO for now
		glBindVertexArray(0);
	}



	//===============================================================================================================


	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
	GLuint LightOnID = glGetUniformLocation(programID, "lightOn");

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	std::cout << "Enterring the while loop" << std::endl;

	do{

		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Set the 'lightPos' uniform in the shader
		glm::vec3 lightPos = glm::vec3(15,10,10);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		glUniform1i(LightOnID, lightOn);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glUniform1i(TextureID, 0);

		// Send our transformation to the shader
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		// Bind the chessboard VAO
		glBindVertexArray(vaoBoard);

		// Draw the chessboard
		glDrawElements(
			GL_TRIANGLES,      // mode
			indices.size(),    // count
			GL_UNSIGNED_SHORT, // type
			(void*)0           // element array buffer offset
		);

		// Unbind the VAO (optional, but good practice)
		glBindVertexArray(0);

		// Draw chess pieces
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				// Skip empty rows
				if (row > 1 && row < 6) {
					continue;
				}

				PieceType pieceType = selectPieceType(row, col);
				if (pieceType == NUM_PIECE_TYPES) {
					continue;  // Empty square, skip
				}

				// Get the mesh index for this piece type
				int meshIndex = getMeshIndexForPieceType(pieceType, meshNameToIndex);
				if (meshIndex == -1) {
					std::cerr << "Invalid mesh index for piece type: " << pieceType << std::endl;
					continue;
				}

				// Bind VAO
				glBindVertexArray(vaoPieces[meshIndex]);

				// Bind the texture for this piece
				GLuint pieceTexture = selectPieceTexture(pieceTexturesMap, row, col);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, pieceTexture);
				glUniform1i(TextureID, 0);  // Set the texture sampler

				// Compute the ModelMatrix for this piece using its base center
				glm::mat4 ModelMatrix = getPieceModelMatrix(row, col, pieceType, modelOriginOffsets[pieceType]);
				glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

				// Send our transformation to the shader
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
				glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

				// Draw the piece
				glDrawElements(GL_TRIANGLES, pieceIndices[meshIndex].size(), GL_UNSIGNED_SHORT, 0);

				// Unbind VAO
				glBindVertexArray(0);
			}
		}




		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBOs for the chessboard
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);

	// Cleanup VAO for the chessboard
	glDeleteVertexArrays(1, &vaoBoard);

	// Cleanup VBOs and VAOs for chess pieces
	for (int i = 0; i < 12; i++) {
		glDeleteBuffers(1, &vboModelData[i]);  
		glDeleteVertexArrays(1, &vaoPieces[i]);
	}

	// Delete textures for chess pieces
	for (int i = 0; i < 6; i++) {
		glDeleteTextures(1, &woodDarkTextures[i]);
		glDeleteTextures(1, &woodLightTextures[i]);
	}

	// Delete shader program
	glDeleteProgram(programID);

	// Delete chessboard texture
	glDeleteTextures(1, &Texture);

	// Terminate GLFW
	glfwTerminate();


	return 0;
}

