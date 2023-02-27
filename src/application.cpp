//
// Created by eput on 2/27/23.
//
#include "application.h"

using namespace router;

Application::Application() : routeStart(Coordinate(0,0)), routeDestination(Coordinate(0,0)) {
}

void Application::setRouteStart(Coordinate coord) {
    routeStart = coord;

    Viewport::Marker marker = Viewport::Marker(Viewport::Shape::Boat, coord, 45.0f);
    viewport->addMarker(marker);
    viewport->scheduleRender();
    g_print("Route start set to (%4.2f, %4.2f)!\n", coord.lat, coord.lon);
}
void Application::setRouteDestination(Coordinate coord) {
    routeDestination = coord;
}
