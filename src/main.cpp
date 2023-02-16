#include <gtk/gtk.h>
#include "shapefilereader.h"
#include "viewport.h"

using namespace router;

static void start_simulation(gpointer user_data) {
    g_print("Starting simulation!\n");
}

static gboolean viewport_on_realize(GtkGLArea* area, gpointer user_data) {
    auto* viewport = static_cast<Viewport*>(user_data);
    return viewport->gtk_on_realize(area);
}

static gboolean viewport_on_unrealize(GtkGLArea* area, gpointer user_data) {
    auto* viewport = static_cast<Viewport*>(user_data);
    return viewport->gtk_on_unrealize(area);
}

static void viewport_on_resize(GtkGLArea* self, gint width, gint height, gpointer user_data) {
    auto* viewport = static_cast<Viewport*>(user_data);
    viewport->gtk_on_resize(self, width, height);
}

static gboolean viwport_render(GtkGLArea* area, GdkGLContext* context, gpointer user_data) {
    auto* viewport = static_cast<Viewport*>(user_data);
    return viewport->gtk_render(area, context);
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

    // Load the chart
    auto* reader = new ShapeFileReader();
    reader->load("data/ne_10m_ocean.shp");

    // Create the viewport and bind it to the gl area
    auto* viewport = new Viewport();
    viewport->setChart(reader->getChart());
    GtkWidget* viewport_gl_area = gtk_gl_area_new();
    gtk_widget_set_hexpand(viewport_gl_area, true);
    gtk_widget_set_vexpand(viewport_gl_area, true);
    gtk_widget_set_visible(viewport_gl_area, true);
    gtk_gl_area_set_auto_render(GTK_GL_AREA(viewport_gl_area), true);
    g_signal_connect(viewport_gl_area, "render", G_CALLBACK(viwport_render), viewport);
    g_signal_connect(viewport_gl_area, "realize", G_CALLBACK(viewport_on_realize), viewport);
    g_signal_connect(viewport_gl_area, "unrealize", G_CALLBACK(viewport_on_unrealize), viewport);
    g_signal_connect(viewport_gl_area, "resize", G_CALLBACK(viewport_on_resize), viewport);
    gtk_box_append(GTK_BOX(box), viewport_gl_area);

    gtk_widget_show(window);
}

int main(int argc, char** argv) {
    // Create a new application
    GtkApplication* app = gtk_application_new("com.example.GtkApplication",
                                              G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}