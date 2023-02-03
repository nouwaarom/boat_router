#include <GL/glew.h>
#include <gtk/gtk.h>

static struct {
    GLuint vbo = 0;
    GLuint vao = 0;
    GLuint shader_programme = 0;
} viewport_state;

float points[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f};

const char* vertex_shader =
        "#version 400\n"
        "in vec3 vp;"
        "void main() {"
        "  gl_Position = vec4(vp, 1.0);"
        "}";

const char* fragment_shader =
        "#version 400\n"
        "out vec4 frag_colour;"
        "void main() {"
        "  frag_colour = vec4(0.5, 0.0, 0.5, 1.0);"
        "}";

static void start_simulation(gpointer user_data) {
    g_print("Starting simulation!\n");
}

static gboolean on_realize(GtkGLArea* area) {
    // We need to make the context current if we want to
    // call GL API
    gtk_gl_area_make_current(area);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // get version info
    const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString(GL_VERSION);   // version as a string
    printf("Renderer: %s\n", renderer);
    printf("OpenGL version supported %s\n", version);

    // If there were errors during the initialization or
    // when trying to make the context current, this
    // function will return a GError for you to catch
    if (gtk_gl_area_get_error(area) != NULL) {
        g_printerr("Error while realizing chart drawing area\n");
        return FALSE;
    }

    // Generate buffers
    glGenBuffers(1, &viewport_state.vbo);
    glGenVertexArrays(1, &viewport_state.vao);

    // Compile shaders
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    viewport_state.shader_programme = glCreateProgram();
    glAttachShader(viewport_state.shader_programme, fs);
    glAttachShader(viewport_state.shader_programme, vs);
    glLinkProgram(viewport_state.shader_programme);

    return TRUE;
}

static gboolean on_unrealize(GtkGLArea* area) {

    return TRUE;
}

static void resize(GtkGLArea* self, gint width, gint height, gpointer user_data) {
    gtk_gl_area_make_current(self);

    g_print("GL Area has size (%d, %d)\n", width, height);

    // Setup the viewport
    glViewport(0, 0, (GLsizei) width, (GLsizei) height);
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();
    //gluOrtho2D(0, width, 0, height);
}

static gboolean render_gl(GtkGLArea* area, GdkGLContext* context) {
    // inside this function it's safe to use GL; the given
    // GdkGLContext has been made current to the drawable
    // surface used by the `GtkGLArea` and the viewport has
    // already been set to be the size of the allocation
    gtk_gl_area_make_current(area);

    // we can start by clearing the buffer
    glClearColor(0.1, 0.2, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, viewport_state.vbo);
    glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), points, GL_STATIC_DRAW);

    glBindVertexArray(viewport_state.vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, viewport_state.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glUseProgram(viewport_state.shader_programme);
    glBindVertexArray(viewport_state.vao);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // we completed our drawing; the draw commands will be
    // flushed at the end of the signal emission chain, and
    // the buffers will be drawn on the window
    return TRUE;
}

static void on_activate(GtkApplication* app, gpointer user_data) {
    // Create a new window
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Weather Router");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 600);

    // The screen is divided vertically between controls and the chart.
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_window_set_child(GTK_WINDOW(window), box);

    // The grid is the container for the top bar
    GtkWidget* grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_box_append(GTK_BOX(box), grid);

    // Create a new button
    GtkWidget* button = gtk_button_new_with_label("Close program");
    // When the button is clicked, close the window passed as an argument
    g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_close), window);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 0, 1, 1);

    // Create another button
    GtkWidget* button2 = gtk_button_new_with_label("Start simulation!");
    // When the button is clicked, close the window passed as an argument
    g_signal_connect_swapped(button2, "clicked", G_CALLBACK(start_simulation), nullptr);
    gtk_grid_attach(GTK_GRID(grid), button2, 1, 0, 1, 1);

    // Create the chart area
    GtkWidget* gl_area = gtk_gl_area_new();
    gtk_widget_set_hexpand(gl_area, true);
    gtk_widget_set_vexpand(gl_area, true);
    gtk_widget_set_visible(gl_area, true);
    gtk_gl_area_set_auto_render(GTK_GL_AREA(gl_area), true);
    g_signal_connect(gl_area, "render", G_CALLBACK(render_gl), NULL);
    g_signal_connect(gl_area, "realize", G_CALLBACK(on_realize), NULL);
    g_signal_connect(gl_area, "unrealize", G_CALLBACK(on_unrealize), NULL);
    g_signal_connect(gl_area, "resize", G_CALLBACK(resize), NULL);
    gtk_box_append(GTK_BOX(box), gl_area);

    gtk_widget_show(window);
}

int main(int argc, char** argv) {

    // Create a new application
    GtkApplication* app = gtk_application_new("com.example.GtkApplication",
                                              G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}