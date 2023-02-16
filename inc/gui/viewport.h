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

    private:
        std::vector<std::vector<glm::vec2>> chart_vertices;
        int chart_size;

        glm::mat4 orthoMatrix;
        GLuint mainVAO;
        Shader vertexShader, fragmentShader;
        ShaderProgram mainProgram;
        VertexBufferObject shapesVBO;
    };
};

#endif //WEATHER_ROUTER_INC_GUI_VIEWPORT_H
