//
// Created by eput on 2/15/23.
//

#ifndef WEATHER_ROUTER_INC_GUI_VIEWPORT_H
#define WEATHER_ROUTER_INC_GUI_VIEWPORT_H

#include <gtk/gtk.h>
#include "shader.h"
#include "shaderProgram.h"
#include "vertexBufferObject.h"
#include "chart.h"

namespace router {
    class Viewport {
    public:
        Viewport();
        void setChart(Chart* chart);

        // GTK callbacks
        gboolean gtk_on_realize(GtkGLArea* area);
        gboolean gtk_on_unrealize(GtkGLArea* area);
        void gtk_on_resize(GtkGLArea* self, gint width, gint height);
        gboolean gtk_render(GtkGLArea* area, GdkGLContext* context);
        // Generic callbacks
        void update_mouse_position(float x, float y);
        void zoom_in();
        void zoom_out();

        void scheduleRender();

    private:
        // Camera state
        float horizontal_aspect_scale, vertical_aspect_scale;
        unsigned int width, height;
        float mouse_x, mouse_y;
        float horizontal_center, vertical_center;
        float horizontal_size, vertical_size;
        float zoom_factor; // Between 0 and 1

        GtkGLArea* gl_area;

        std::vector<std::vector<glm::vec2>> chart_vertices;
        int chart_size;

        glm::mat4 orthoMatrix;
        GLuint mainVAO;
        Shader vertexShader, fragmentShader;
        ShaderProgram mainProgram;
        VertexBufferObject shapesVBO;

        void processZoom(float factor);
        void recalculateProjectionMatrix();
    };
};

#endif //WEATHER_ROUTER_INC_GUI_VIEWPORT_H
