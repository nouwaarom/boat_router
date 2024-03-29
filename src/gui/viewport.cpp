//
// Created by eput on 2/15/23.
//
#include "viewport.h"
#include "meshes/boat.h"
#include "meshes/destination.h"
#include "glad/glad.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

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
    is_dragging = false;
}

int Viewport::addMarker(Marker marker) {
    int first_free_key = 0;
    for (const auto& elem: m_markers) {
        if (elem.first != first_free_key) {
            break;
        }
        first_free_key++;
    }
    m_markers.insert({first_free_key, marker});
    std::cout << "Marker " << first_free_key << " inserted." << std::endl;

    return first_free_key;
}

void Viewport::removeMarker(int marker_handle) {
    std::cout << "Marker " << marker_handle << " removed." << std::endl;
    if (m_markers.contains(marker_handle)) {
        m_markers.erase(marker_handle);
    }}

void Viewport::clearMarkers() {
    m_markers.clear();
}

Coordinate Viewport::getCoordinateForPosition(double x, double y) {
    float relative_x = ((2.0 * x) - (float)width) / (float)width;
    float relative_y = ((-2.0 * y) + (float)height) / (float)height;
    double coordinate_x = horizontal_center + (horizontal_size * relative_x);
    double coordinate_y = vertical_center + (vertical_size * relative_y);

    return Coordinate {coordinate_y, coordinate_x};
}

void Viewport::update_mouse_position(float x, float y) {
    if (is_dragging) {
        float relative_x = 2.0 * (mouse_x - x) / (float)width;
        float relative_y = -2.0 * (mouse_y - y) / (float)height;
        horizontal_center = horizontal_center + (horizontal_size * relative_x);
        vertical_center = vertical_center + (vertical_size * relative_y);
        recalculateProjectionMatrix();
        scheduleRender();
    }

    mouse_x = x;
    mouse_y = y;
}

void Viewport::on_mouse_pressed(float x, float y) {
    is_dragging = true;
    mouse_x = x;
    mouse_y = y;
}

void Viewport::on_mouse_released(float x, float y) {
    is_dragging = false;
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
        float change = 1.0f / 0.92f;
        // TODO, zooming out should have different logic and should always return to center.
        processZoom(change);
        zoom_factor *= change;
     }


    recalculateProjectionMatrix();
    scheduleRender();
}

void Viewport::processZoom(float factor) {
    // Update center based on mouse position.
    float relative_x = mouse_x * 2.0f / (float)width - 1.0f; // From -1.0 to 1.0
    float relative_y = -(mouse_y * 2.0f / (float)height) + 1.0f; // From -1.0 to 1.0
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

    // Upload chart data to the GPU
    glGenVertexArrays(1, &chartVAO);
    glBindVertexArray(chartVAO);

    chartShapesVBO.createVBO(chart_size);
    chartShapesVBO.bindVBO();
    for (auto& ring_vertices: chart_vertices) {
        chartShapesVBO.addRawData(ring_vertices.data(), ring_vertices.size()*sizeof(glm::vec2));
    }
    chartShapesVBO.uploadDataToGPU(GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    // Upload marker data to the GPU
    m_shape_mesh_map.insert({Shape::Boat, new meshes::Boat()});
    m_shape_mesh_map.insert({Shape::Destination, new meshes::Destination()});

    return TRUE;
}

gboolean Viewport::gtk_on_unrealize(GtkGLArea* area) {
    mainProgram.deleteProgram();
    vertexShader.deleteShader();
    fragmentShader.deleteShader();

    chartShapesVBO.deleteVBO();

    glDeleteVertexArrays(1, &chartVAO);

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

    // Update the projection to make sure the aspect stays correct.
    recalculateProjectionMatrix();
}

gboolean Viewport::gtk_render(GtkGLArea* area, GdkGLContext* context) {
    // inside this function it's safe to use GL; the given
    // GdkGLContext has been made current to the drawable
    // surface used by the `GtkGLArea` and the viewport has
    // already been set to be the size of the allocation
    //gtk_gl_area_make_current(area);

    glBindVertexArray(chartVAO);

    // we can start by clearing the buffer
    glClearColor(0.1, 0.2, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the chart
    mainProgram.useProgram();
    mainProgram["matrices.projectionMatrix"] = orthoMatrix;

    glm::mat4 model = glm::mat4(1.0);
    model = glm::scale(model, glm::vec3(1.0, 1.0, 1.0));
    mainProgram["matrices.modelMatrix"] = model;
    mainProgram["color"] = glm::vec4(1.0, 1.0, 1.0, 1.0);
    int index = 0;
    for (int i = 0; i < chart_vertices.size(); i++) {
        int size = chart_vertices[i].size();
        // Render part by part.
        glDrawArrays(GL_LINE_STRIP, index, size); // Was GL_TRIANGLE_FAN
        index += size;
    }

    // Render the markers
    for (auto const& marker_pair : m_markers) {
        auto marker = marker_pair.second;
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(marker.position.lon, marker.position.lat, 0.0));
        model = glm::rotate(model, glm::radians(marker.rotation), glm::vec3(0,0,-1));
        model = glm::scale(model, glm::vec3(2.0 * sqrt(zoom_factor), 2.0 * sqrt(zoom_factor), 1.0));
        mainProgram["matrices.modelMatrix"] = model;
        mainProgram["color"] = glm::vec4(0.8, 0.3, 0.3, 1.0);

        if (m_shape_mesh_map.contains(marker.shape)) {
            m_shape_mesh_map[marker.shape]->render();
        } else {
            g_printerr("No mesh for shape XXX found\n");
        }
    }

    // we completed our drawing; the draw commands will be
    // flushed at the end of the signal emission chain, and
    // the buffers will be drawn on the window
    return TRUE;
}

void Viewport::recalculateProjectionMatrix() {
    //g_print("Center: x=%4.2f, y=%4.2f\n", horizontal_center, vertical_center); 
    //g_print("Size: x=%4.2f, y=%4.2f\n", horizontal_size, vertical_size);
    horizontal_size = 180.0f * zoom_factor * horizontal_aspect_scale;
    vertical_size = 90.0f * zoom_factor * vertical_aspect_scale;
    orthoMatrix = glm::ortho(horizontal_center - horizontal_size, horizontal_center + horizontal_size, vertical_center - vertical_size, vertical_center + vertical_size);
}
