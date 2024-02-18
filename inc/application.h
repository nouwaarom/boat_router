//
// Created by eput on 2/27/23.
//

#ifndef WEATHER_ROUTER_INC_APPLICATION_H
#define WEATHER_ROUTER_INC_APPLICATION_H

#include "viewport.h"

namespace router {
    /**
     * The application is a container for all components.
     */
    class Application {
    public:
        Application(Viewport* viewport);

        void setRouteStart(double mouse_x, double mouse_y);
        void setRouteStart(Coordinate coord);

        void setRouteDestination(double mouse_x, double mouse_y);
        void setRouteDestination(Coordinate coord);
    private:
        Viewport* m_viewport;
        // Viewport handle for the start marker.
        int       m_start_marker;
        // Viewport handle for the destination marker.
        int       m_destination_marker;

        Coordinate routeStart;
        Coordinate routeDestination;
    };
}

#endif //WEATHER_ROUTER_INC_APPLICATION_H
