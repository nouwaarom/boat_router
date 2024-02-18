// GLM
#include "glm/glm.hpp"

// Project
#include "meshes/destination.h"

namespace meshes {

glm::vec2 Destination::vertices[] =
{
    glm::vec2(0.0f, 0.0f), // Flag triangle
    glm::vec2(2.0f, 0.5f),
    glm::vec2(0.0f, 1.0f),

    glm::vec2(0.0f,  0.0f), // Stick left triangle
    glm::vec2(0.0f, -1.5f),
    glm::vec2(0.2f, -1.5f),

    glm::vec2(0.0f,  0.0f), // Stick right triangle
    glm::vec2(0.2f,  0.0f),
    glm::vec2(0.2f, -1.5f),
};

Destination::Destination()
    : StaticMesh2D(true, false)
{
    initializeData();
}

void Destination::render() const
{
    if (!_isInitialized) {
        return;
    }

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(glm::vec2));
}

void Destination::initializeData()
{
    if (_isInitialized) {
        return;
    }

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    _vbo.createVBO(sizeof(vertices));
    _vbo.bindVBO();
    
    _vbo.addRawData(vertices, sizeof(vertices));

    _vbo.uploadDataToGPU(GL_STATIC_DRAW);
    setVertexAttributesPointers(sizeof(vertices)/sizeof(glm::vec2));
    _isInitialized = true;
}

} // namespace meshes
