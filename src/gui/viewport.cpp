//
// Created by eput on 2/15/23.
//
#include "viewport.h"
#include <glm/gtc/matrix_transform.hpp>
#include "glad/glad.h"

using namespace router;

static const char* vertex_shader_source =
        "#version 440 core\n"
        "\n"
        "uniform struct\n"
        "{\n"
        "    mat4 projectionMatrix;\n"
        "    mat4 modelMatrix;\n"
        "} matrices;\n"
        "\n"
        "layout(location = 0) in vec2 vertexPosition;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    mat4 mvpMatrix = matrices.projectionMatrix * matrices.modelMatrix;\n"
        "    gl_Position = mvpMatrix * vec4(vertexPosition, 0.0, 1.0);\n"
        "}";

static const char* fragment_shader_source =
        "#version 440 core\n"
        "\n"
        "layout(location = 0) out vec4 outputColor;\n"
        "\n"
        "uniform vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    outputColor = color;\n"
        "}";

Viewport::Viewport() {
    width = 0;
    height = 0;
    mouse_x = 0.0f;
    mouse_y = 0.0f;
    zoom_factor = 1.0f;
    horizontal_center = 0.0f;
    vertical_center = 0.0f;
}

void Viewport::update_mouse_position(float x, float y) {
    mouse_x = x;
    mouse_y = y;
}

void Viewport::zoom_in() {
    if (zoom_factor <= 0.005f) {
        return;
    }
    float change = 0.92f;
    processZoom(change);
    zoom_factor *= change;

    recalculateProjectionMatrix();
    scheduleRender();
}

void Viewport::zoom_out() {
    if (zoom_factor >= 0.99f) {
        zoom_factor = 1.0f;
        horizontal_center = 0.0f;
        vertical_center = 0.0f;
     } else {
        float change = 1.0f/0.92f;
        // TODO, zooming out should have different logic and should always return to center.
        processZoom(change);
        zoom_factor *= change;
     }


    recalculateProjectionMatrix();
    scheduleRender();
}

void Viewport::processZoom(float factor) {
    // Update center based on mouse position.
    float relative_x = mouse_x * 4.0f / (float)width - 1.0f; // From -1.0 to 1.0
    float relative_y = -(mouse_y * 4.0f / (float)height) + 1.0f; // From -1.0 to 1.0
    horizontal_center = horizontal_center + (horizontal_size * relative_x * (1.0f - factor));
    vertical_center = vertical_center + (vertical_size * relative_y * (1.0f - factor));
}

void Viewport::scheduleRender() {
    gtk_widget_queue_draw(GTK_WIDGET(gl_area));
}

void Viewport::setChart(Chart* chart) {
    chart_size = 0;
    g_print("Viewport has chart with %d rings\n", chart->getRingCount());
    chart_vertices.reserve(chart->getRingCount());
    for (int i = 0; i < chart->getRingCount(); i++) {
        auto ring = chart->getRing(i);
        std::vector<glm::vec2> ring_vertices;
        ring_vertices.reserve(ring.size());
        chart_size += ring.size()*sizeof(glm::vec2);
        for (auto& v : ring) {
            ring_vertices.emplace_back(v.lat, v.lon);
        }
        chart_vertices.push_back(ring_vertices);
    }
}

gboolean Viewport::gtk_on_realize(GtkGLArea* area) {
    // We need to make the context current if we want to
    // call GL API
    gtk_gl_area_make_current(area);
    this->gl_area = area;

    // start OpenGL extension handler
    gladLoadGL();

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION);   // version as a string
    g_print("Renderer: %s\n", renderer);
    g_print("OpenGL version supported %s\n", version);

    // If there were errors during the initialization or
    // when trying to make the context current, this
    // function will return a GError for you to catch
    if (gtk_gl_area_get_error(area) != NULL) {
        g_printerr("Error while realizing chart drawing area\n");
        return FALSE;
    }

    // Compile shaders
    vertexShader.loadShader(vertex_shader_source, GL_VERTEX_SHADER);
    fragmentShader.loadShader(fragment_shader_source, GL_FRAGMENT_SHADER);
    if (!vertexShader.isCompiled() || !fragmentShader.isCompiled()) {
        g_printerr("Could not compile shaders");
        return FALSE;
    }

    mainProgram.createProgram();
    mainProgram.addShaderToProgram(vertexShader);
    mainProgram.addShaderToProgram(fragmentShader);

    if (!mainProgram.linkProgram()) {
        g_printerr("Could not compile shaders");
        return FALSE;
    }

    glGenVertexArrays(1, &mainVAO);
    glBindVertexArray(mainVAO);

    shapesVBO.createVBO(chart_size);
    shapesVBO.bindVBO();
    for (auto& ring_vertices: chart_vertices) {
        shapesVBO.addRawData(ring_vertices.data(), ring_vertices.size()*sizeof(glm::vec2));
    }
    shapesVBO.uploadDataToGPU(GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    return TRUE;
}

gboolean Viewport::gtk_on_unrealize(GtkGLArea* area) {
    mainProgram.deleteProgram();
    vertexShader.deleteShader();
    fragmentShader.deleteShader();

    shapesVBO.deleteVBO();

    glDeleteVertexArrays(1, &mainVAO);

    return TRUE;
}

void Viewport::gtk_on_resize(GtkGLArea* self, gint width, gint height) {
    gtk_gl_area_make_current(self);

    this->width = width;
    this->height = height;

    // Setup the viewport
    //glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    // For a correct perspective the width should be twice the height.
    if (width > 2 * height) { // Width is too large. Reduce height scale to compensate.
        horizontal_aspect_scale = 1.0f;
        vertical_aspect_scale = (2.0f * (float)height) / (float)width;
    } else {
        horizontal_aspect_scale = (float)width / (2.0f * (float)height);
        vertical_aspect_scale = 1.0f;
    }

    // We always project the whole map.
    // TODO change this matrix based on padding and scaling.
    // TODO, make sure perspective stays accurate.
    recalculateProjectionMatrix();
}

gboolean Viewport::gtk_render(GtkGLArea* area, GdkGLContext* context) {
    // inside this function it's safe to use GL; the given
    // GdkGLContext has been made current to the drawable
    // surface used by the `GtkGLArea` and the viewport has
    // already been set to be the size of the allocation
    //gtk_gl_area_make_current(area);

    glBindVertexArray(mainVAO);

    // we can start by clearing the buffer
    glClearColor(0.1, 0.2, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mainProgram.useProgram();
    mainProgram["matrices.projectionMatrix"] = orthoMatrix;

    // Render the shape
    glm::mat4 model = glm::mat4(1.0);
    model = glm::scale(model, glm::vec3(1.0, 1.0, 1));
    mainProgram["matrices.modelMatrix"] = model;
    mainProgram["color"] = glm::vec4(1.0, 1.0, 1.0, 1.0);
    int index = 0;
    for (int i = 0; i < chart_vertices.size(); i++) {
        int size = chart_vertices[i].size();
        // Render part by part.
        glDrawArrays(GL_LINE_STRIP, index, size); // Was GL_TRIANGLE_FAN
        index += size;
    }

    // we completed our drawing; the draw commands will be
    // flushed at the end of the signal emission chain, and
    // the buffers will be drawn on the window
    return TRUE;
}

void Viewport::recalculateProjectionMatrix() {
    horizontal_size = 180.0f * zoom_factor * horizontal_aspect_scale;
    vertical_size = 90.0f * zoom_factor * vertical_aspect_scale;
    orthoMatrix = glm::ortho(horizontal_center - horizontal_size, horizontal_center + horizontal_size, vertical_center - vertical_size, vertical_center + vertical_size);
}
