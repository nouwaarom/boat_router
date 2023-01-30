#include <gtk/gtk.h>
#include <GL/gl.h>

static gboolean on_realize (GtkGLArea *area) {
    // We need to make the context current if we want to
    // call GL API
    gtk_gl_area_make_current (area);

    // If there were errors during the initialization or
    // when trying to make the context current, this
    // function will return a GError for you to catch
    if (gtk_gl_area_get_error (area) != NULL) {
        g_printerr("Error while realizing chart drawing area\n");
        return FALSE;
    }

    return TRUE;
}

static gboolean on_unrealize (GtkGLArea *area) {

    return TRUE;
}

static void start_simulation(gpointer user_data) {
    g_print("Starting simulation!\n");
}

static gboolean render(GtkGLArea* area, GdkGLContext* context) {
    // inside this function it's safe to use GL; the given
    // GdkGLContext has been made current to the drawable
    // surface used by the `GtkGLArea` and the viewport has
    // already been set to be the size of the allocation

    // we can start by clearing the buffer
    glClearColor(0, 0.5, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw your object
    // Drawing is done by specifying a sequence of vertices.  The way these
    // vertices are connected (or not connected) depends on the argument to
    // glBegin.  GL_POLYGON constructs a filled polygon.
    glBegin(GL_POLYGON);
    glColor3f(1, 0, 0); glVertex3f(-0.6, -0.75, 0.5);
    glColor3f(0, 1, 0); glVertex3f(0.6, -0.75, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0.75, 0);
    glEnd();

    // Flush drawing command buffer to make drawing happen as soon as possible.
    glFlush();

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
    gtk_gl_area_set_auto_render(GTK_GL_AREA(gl_area), true);
    g_signal_connect(gl_area, "render", G_CALLBACK(render), NULL);
    g_signal_connect(gl_area, "realize", G_CALLBACK(on_realize), NULL);
    g_signal_connect(gl_area, "unrealize", G_CALLBACK(on_unrealize), NULL);
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