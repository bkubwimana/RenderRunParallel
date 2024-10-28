#ifndef CONTROLS_HPP
#define CONTROLS_HPP
extern bool lightOn;
void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

#endif