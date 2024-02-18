//
// Created by eput on 2/27/23.
//

#ifndef WEATHER_ROUTER_INC_GUI_CONTROLS_H
#define WEATHER_ROUTER_INC_GUI_CONTROLS_H

#include "application.h"
#include "coordinate.h"
#include <gtk/gtk.h>

namespace router {

class Controls {
public:
    Controls(Application* application, Viewport* viewport);
    // Legacy
    GtkWidget* getControlsBar();

    GtkGesture* getLeftClickGesture();
    GtkGesture* getRightClickGesture();
    GtkWidget* getPopoverMenu();
private:

    static void set_start(gpointer user_data);
    void setStart();

    static void set_destination(gpointer user_data);
    void setDestination();

    static void on_left_mouse_button_pressed(GtkGestureClick* gesture, int n_press, gdouble x, gdouble y, gpointer user_data);
    void onLeftMouseButtonPressed(double x, double y);
    static void on_left_mouse_button_released(GtkGestureClick* gesture, int n_press, gdouble x, gdouble y, gpointer user_data);
    void onLeftMouseButtonReleased(double x, double y);

    static void on_right_mouse_button_pressed(GtkGestureClick* gesture, int n_press, gdouble x, gdouble y, gpointer user_data);
    void onRightMouseButtonPressed(double x, double y);
    static void on_right_mouse_button_released(GtkGestureClick* gesture, int n_press, gdouble x, gdouble y, gpointer user_data);
    void onRightMouseButtonReleased(double x, double y);

    Coordinate getCoordinate();

    Application* m_application;
    Viewport* m_viewport;
    GtkWidget *latN, *latS, *latDeg, *latMin, *latSec;
    GtkWidget *lonE, *lonW, *lonDeg, *lonMin, *lonSec;
    GtkWidget *m_popover;
    double m_popover_x, m_popover_y;
}; // Class

} // Namespace

#endif //WEATHER_ROUTER_INC_GUI_CONTROLS_H
