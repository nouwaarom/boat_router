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
        Application();

        Viewport* viewport;
        void setRouteStart(Coordinate coord);
        void setRouteDestination(Coordinate coord);
    private:
        Coordinate routeStart;
        Coordinate routeDestination;
    };
}

#endif //WEATHER_ROUTER_INC_APPLICATION_H
