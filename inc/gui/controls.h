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
        Controls(Application* application);
        GtkWidget* getControlsBar();
    private:
        Application* application;
        GtkWidget *latN, *latS, *latDeg, *latMin, *latSec;
        GtkWidget *lonE, *lonW, *lonDeg, *lonMin, *lonSec;

        static void set_start(gpointer user_data);
        void setStart();

        static void set_destination(gpointer user_data);
        void setDestination();

        Coordinate getCoordinate();
    };
}

#endif //WEATHER_ROUTER_INC_GUI_CONTROLS_H
