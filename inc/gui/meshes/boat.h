#pragma once

// GLAD
#include "glad/glad.h"

// GLM
#include "glm/glm.hpp"

// Project
#include "staticMesh2D.h"

namespace meshes {

/**
 * Quad static mesh of unit size.
 */
class Boat : public StaticMesh2D
{
public:
    Boat();

    void render() const override;

    static glm::vec2 vertices[]; // Array of mesh vertices (and texture coordinates as well)

private:
    void initializeData() override;
};

} // namespace static_meshes_2D