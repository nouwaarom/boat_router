//
// Created by eput on 2/15/23.
//

#ifndef WEATHER_ROUTER_INC_GUI_VIEWPORT_H
#define WEATHER_ROUTER_INC_GUI_VIEWPORT_H

#include "chart.h"
#include "shader.h"
#include "shaderProgram.h"
#include "staticMesh2D.h"
#include "vertexBufferObject.h"
#include <gtk/gtk.h>

namespace router {
    class Viewport {
    public:
        enum class Shape {
            Boat,
            Marker,
        };
        class Marker {
        public:
            Marker(Shape shape, Coordinate position, float rotation) : shape(shape), position(position), rotation(rotation) {}
            Shape shape;
            Coordinate position;
            float rotation; // Between 0 and 360 degrees.
        };

        Viewport();
        void setChart(Chart* chart);
        // Add a marker for drawing. Marker will be copied and be shown until markers are cleared.
        void addMarker(Marker marker);
        void clearMarkers();

        // GTK callbacks
        gboolean gtk_on_realize(GtkGLArea* area);
        gboolean gtk_on_unrealize(GtkGLArea* area);
        void gtk_on_resize(GtkGLArea* self, gint width, gint height);
        gboolean gtk_render(GtkGLArea* area, GdkGLContext* context);
        // Generic callbacks
        void update_mouse_position(float x, float y);
        void on_mouse_pressed(float x, float y);
        void on_mouse_released(float x, float y);
        void zoom_in();
        void zoom_out();

        void scheduleRender();

    private:
        // Camera state
        float horizontal_aspect_scale, vertical_aspect_scale;
        unsigned int width, height;
        float mouse_x, mouse_y;
        // Store the current center of the viewport.
        float horizontal_center, vertical_center;
        // Store the current size of the vieport
        // Viewport is from (center - size) to (center + size).
        // So these are actually half sizes.
        float horizontal_size, vertical_size;
        float zoom_factor; // Between 0 and 1

        GtkGLArea* gl_area;

        std::vector<Marker> markers;
        std::vector<std::vector<glm::vec2>> chart_vertices;
        int chart_size;

        // This is the projection matrix in use, we use a orthogonal projection because we work purely in 2D
        glm::mat4 orthoMatrix;
        Shader vertexShader, fragmentShader;
        ShaderProgram mainProgram;
        GLuint chartVAO;
        VertexBufferObject chartShapesVBO;

        // Meshes
        meshes::StaticMesh2D* boatMesh;

        bool is_dragging;

        void processZoom(float factor);
        void recalculateProjectionMatrix();
    };
};

#endif //WEATHER_ROUTER_INC_GUI_VIEWPORT_H
