//
// Created by eput on 2/27/23.
//
#include "controls.h"

using namespace router;

Controls::Controls(Application* application) : application(application) {

}

GtkWidget* Controls::getControlsBar() {
    // The grid is the container for the top bar
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);

    latN  = gtk_toggle_button_new_with_label("N");
    latS  = gtk_toggle_button_new_with_label("S");
    gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(latN), GTK_TOGGLE_BUTTON(latS));
    latDeg = gtk_spin_button_new_with_range(0, 90, 1);
    latMin = gtk_spin_button_new_with_range(0, 59, 1);
    latSec = gtk_spin_button_new_with_range(0, 59, 1);

    gtk_box_append(GTK_BOX(box), latN);
    gtk_box_append(GTK_BOX(box), latS);
    gtk_box_append(GTK_BOX(box), latDeg);
    gtk_box_append(GTK_BOX(box), latMin);
    gtk_box_append(GTK_BOX(box), latSec);

    lonE  = gtk_toggle_button_new_with_label("E");
    lonW  = gtk_toggle_button_new_with_label("W");
    gtk_toggle_button_set_group(GTK_TOGGLE_BUTTON(lonE), GTK_TOGGLE_BUTTON(lonW));
    lonDeg = gtk_spin_button_new_with_range(0, 180, 1);
    lonMin = gtk_spin_button_new_with_range(0, 59, 1);
    lonSec = gtk_spin_button_new_with_range(0, 59, 1);

    gtk_box_append(GTK_BOX(box), lonE);
    gtk_box_append(GTK_BOX(box), lonW);
    gtk_box_append(GTK_BOX(box), lonDeg);
    gtk_box_append(GTK_BOX(box), lonMin);
    gtk_box_append(GTK_BOX(box), lonSec);

    // Set the input position as start
    GtkWidget* buttonSetStart = gtk_button_new_with_label("Set start");
    // When the button is clicked, close the window passed as an argument
    g_signal_connect_swapped(buttonSetStart, "clicked", G_CALLBACK(set_start), this);
    gtk_box_append(GTK_BOX(box), buttonSetStart);

    // Set the input position as start
    GtkWidget* buttonSetDestination = gtk_button_new_with_label("Set destination");
    // When the button is clicked, close the window passed as an argument
    g_signal_connect_swapped(buttonSetDestination, "clicked", G_CALLBACK(set_destination), this);
    gtk_box_append(GTK_BOX(box), buttonSetDestination);

    return box;
}

GtkWidget* Controls::getPopoverMenu() {
    // The grid is the container for the top bar
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);

    // Set the input position as start
    GtkWidget* buttonSetStart = gtk_button_new_with_label("Set start");
    // When the button is clicked, close the window passed as an argument
    g_signal_connect_swapped(buttonSetStart, "clicked", G_CALLBACK(set_start), this);
    gtk_box_append(GTK_BOX(box), buttonSetStart);

    // Set the input position as start
    GtkWidget* buttonSetDestination = gtk_button_new_with_label("Set destination");
    // When the button is clicked, close the window passed as an argument
    g_signal_connect_swapped(buttonSetDestination, "clicked", G_CALLBACK(set_destination), this);
    gtk_box_append(GTK_BOX(box), buttonSetDestination);

    m_popover = gtk_popover_new();
    gtk_popover_set_child(GTK_POPOVER(m_popover), box);
    gtk_popover_set_autohide(GTK_POPOVER(m_popover), false);

    return m_popover;
}

GtkGesture* Controls::getRightClickGesture() {
    GtkGesture* click_gesture = gtk_gesture_click_new();
    gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click_gesture), 3); // 3 = Right
    g_signal_connect(click_gesture, "pressed", G_CALLBACK(on_right_mouse_button_pressed), this); 
    g_signal_connect(click_gesture, "released", G_CALLBACK(on_right_mouse_button_released), this); 

    return click_gesture;
}

Coordinate Controls::getCoordinate() {
    double deg, min, sec;
    deg = gtk_spin_button_get_value(GTK_SPIN_BUTTON(latDeg));
    min = gtk_spin_button_get_value(GTK_SPIN_BUTTON(latMin));
    sec = gtk_spin_button_get_value(GTK_SPIN_BUTTON(latSec));

    double lat = deg + ((min + sec/60.0)/60.0);

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(latS))) {
        lat = -lat;
    }

    deg = gtk_spin_button_get_value(GTK_SPIN_BUTTON(lonDeg));
    min = gtk_spin_button_get_value(GTK_SPIN_BUTTON(lonMin));
    sec = gtk_spin_button_get_value(GTK_SPIN_BUTTON(lonSec));

    double lon = deg + ((min + sec/60.0)/60.0);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lonW))) {
        lon = -lon;
    }

    return {lat, lon};
}

void Controls::setStart() {
    g_print("Setting start position\n");
    application->setRouteStart(m_popover_x, m_popover_y);
}

void Controls::set_start(gpointer user_data) {
    auto* controls = static_cast<Controls*>(user_data);
    controls->setStart();
}

void Controls::setDestination() {
    g_print("Setting destination position\n");
    application->setRouteDestination(m_popover_x, m_popover_y);
}

void Controls::set_destination(gpointer user_data) {
    auto* controls = static_cast<Controls*>(user_data);
    controls->setDestination();
}

void Controls::on_right_mouse_button_pressed(GtkGestureClick* gesture, int n_press, gdouble x, gdouble y, gpointer user_data) {
    auto* controls = static_cast<Controls*>(user_data);
    controls->onRightMouseButtonPressed(x, y);
}

void Controls::onRightMouseButtonPressed(double x, double y) {
    g_print("Pressed RMB\n");
    m_popover_x = x;
    m_popover_y = y;
    GdkRectangle popover_position = GdkRectangle { (int)x, (int)y, 1, 1};
    gtk_popover_set_pointing_to(GTK_POPOVER(m_popover), &popover_position);
    // TODO, popdown on other actions.
    gtk_popover_popup(GTK_POPOVER(m_popover));
    // TODO, set position in the controls.
}

void Controls::on_right_mouse_button_released(GtkGestureClick* gesture, int n_press, gdouble x, gdouble y, gpointer user_data) {
    auto* controls = static_cast<Controls*>(user_data);
    controls->onRightMouseButtonReleased(x, y);
}

void Controls::onRightMouseButtonReleased(double x, double y) {
    g_print("Released RMB\n");
}
