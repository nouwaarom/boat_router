// GLM
#include "glm/glm.hpp"

// Project
#include "gui/meshes/boat.h"

namespace meshes {

glm::vec2 Boat::vertices[] =
{
    glm::vec2(0.0f, 1.0f), // Top triangle
    glm::vec2(0.3f, 0.2f),
    glm::vec2(-0.3f, 0.2f),
    glm::vec2(-0.3f, 0.2f), // Bottom left triangle
    glm::vec2(0.3f, 0.2f),
    glm::vec2(-0.2f, -1.0f),
    glm::vec2(0.3f, 0.2f), // Bottom right triangle
    glm::vec2(0.2f, -1.0f),
    glm::vec2(-0.2f, -1.0f),
};

Boat::Boat()
    : StaticMesh2D(true, false)
{
    initializeData();
}

void Boat::render() const
{
    if (!_isInitialized) {
        return;
    }

    glBindVertexArray(_vao);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(glm::vec2));
}

void Boat::initializeData()
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

} // namespace static_meshes_2D