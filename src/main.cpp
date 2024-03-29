#include "application.h"
#include "controls.h"
#include "shapefilereader.h"
#include "viewport.h"
#include <gtk/gtk.h>
#include <iostream>
#include <memory>

using namespace router;

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
    // FIXME, this /2 is a hack for highdpi scaling.
    viewport->gtk_on_resize(self, width, height);
}

static gboolean viwport_render(GtkGLArea* area, GdkGLContext* context, gpointer user_data) {
    auto* viewport = static_cast<Viewport*>(user_data);
    return viewport->gtk_render(area, context);
}

static gboolean on_key_pressed(GtkWidget* widget, guint keyval, guint keycode,
                               GdkModifierType state, GtkEventControllerKey* event_controller) {
    g_print("Key press event!\n");

    return FALSE; // Indicate that we did not handle this event and that it should be handled elsewhere.
}

static gboolean viewport_on_scroll(GtkEventControllerScroll* self, gdouble dx, gdouble dy, gpointer user_data) {
    auto* viewport = static_cast<Viewport*>(user_data);
    if (dy > 0.0) {
        viewport->zoom_in();
    } else {
        viewport->zoom_out();
    }

    return TRUE;
}

static gboolean viewport_on_motion(GtkEventControllerMotion* self, gdouble x, gdouble y, gpointer user_data) {
    auto* viewport = static_cast<Viewport*>(user_data);
    viewport->update_mouse_position((float)x, (float)y);

    return TRUE;
}

static void viewport_on_pressed(GtkGestureClick* gesture, int n_press, gdouble x, gdouble y, gpointer user_data) {
    auto* viewport = static_cast<Viewport*>(user_data);
    viewport->on_mouse_pressed((float)x, (float)y);
}

static void viewport_on_released(GtkGestureClick* gesture, int n_press, gdouble x, gdouble y, gpointer user_data) {
    auto* viewport = static_cast<Viewport*>(user_data);
    viewport->on_mouse_released((float)x, (float)y);
}


static void on_activate(GtkApplication* app, gpointer user_data) {
    // Create container
    auto* viewport = new Viewport();
    auto* application = new Application(viewport);
    // Create viewport manager.
    auto* controls = new Controls(application, viewport);

    // Create a new window
    GtkWidget* window = gtk_application_window_new(app);
    gtk_widget_set_can_focus(GTK_WIDGET(window), true);
    gtk_window_set_title(GTK_WINDOW(window), "Weather Router");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 600);

    // The screen is divided vertically between controls and the chart.
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_can_focus(GTK_WIDGET(box), true);
    gtk_window_set_child(GTK_WINDOW(window), box);

    // The controls are rendered in the top bar
    gtk_box_append(GTK_BOX(box), controls->getControlsBar());

    // Load the chart
    auto* reader = new ShapeFileReader();
    reader->load("data/ne_10m_ocean.shp");

    // Create the viewport and bind it to the gl area
    viewport->setChart(reader->getChart());
    GtkWidget* viewport_gl_area = gtk_gl_area_new();
    gtk_widget_grab_focus(GTK_WIDGET(viewport_gl_area));
    gtk_widget_set_focusable(GTK_WIDGET(viewport_gl_area), true);
    gtk_widget_set_can_focus(GTK_WIDGET(box), true);
    gtk_widget_set_hexpand(viewport_gl_area, true);
    gtk_widget_set_vexpand(viewport_gl_area, true);
    gtk_widget_set_visible(viewport_gl_area, true);
    gtk_gl_area_set_auto_render(GTK_GL_AREA(viewport_gl_area), true);
    g_signal_connect(viewport_gl_area, "render", G_CALLBACK(viwport_render), viewport);
    g_signal_connect(viewport_gl_area, "realize", G_CALLBACK(viewport_on_realize), viewport);
    g_signal_connect(viewport_gl_area, "unrealize", G_CALLBACK(viewport_on_unrealize), viewport);
    g_signal_connect(viewport_gl_area, "resize", G_CALLBACK(viewport_on_resize), viewport);

    // Setup event controllers for the viewport
    GtkEventController* key_event_controller = gtk_event_controller_key_new();
    g_signal_connect(key_event_controller, "key-pressed", G_CALLBACK(on_key_pressed), viewport);
    gtk_widget_add_controller(GTK_WIDGET(window), key_event_controller);

    GtkEventController* scroll_event_controller = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_VERTICAL);
    g_signal_connect(scroll_event_controller, "scroll", G_CALLBACK(viewport_on_scroll), viewport);
    gtk_widget_add_controller(GTK_WIDGET(viewport_gl_area), scroll_event_controller);

    GtkEventController* motion_event_controller = gtk_event_controller_motion_new();
    g_signal_connect(motion_event_controller, "motion", G_CALLBACK(viewport_on_motion), viewport);
    gtk_widget_add_controller(GTK_WIDGET(viewport_gl_area), motion_event_controller);

    // Setup listener for left mouseclick.
    /*GtkGesture* left_click_gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(left_click_gesture), 1); // 1 = Left
    g_signal_connect(left_click_gesture, "pressed", G_CALLBACK(viewport_on_pressed), viewport); 
    g_signal_connect(left_click_gesture, "released", G_CALLBACK(viewport_on_released), viewport);*/

    // Controls class handles right mousclicks.
    gtk_widget_add_controller(GTK_WIDGET(viewport_gl_area), GTK_EVENT_CONTROLLER(controls->getLeftClickGesture()));

    // Create popover (menu) for right mouseclicks on the viewport.
    gtk_widget_set_parent(controls->getPopoverMenu(), GTK_WIDGET(viewport_gl_area));
    // Setup listener for right mouseclick
    gtk_widget_add_controller(GTK_WIDGET(viewport_gl_area), GTK_EVENT_CONTROLLER(controls->getRightClickGesture()));

    gtk_box_append(GTK_BOX(box), viewport_gl_area);

    gtk_widget_set_visible(window, true);
}

int main(int argc, char** argv) {
    // Create a new application
    GtkApplication* app = gtk_application_new("com.example.GtkApplication",
                                              G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    return g_application_run(G_APPLICATION(app), argc, argv);
}
