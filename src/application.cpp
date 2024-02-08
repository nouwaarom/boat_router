//
// Created by eput on 2/27/23.
//
#include "application.h"

using namespace router;

Application::Application(Viewport* viewport):
    m_viewport(viewport), routeStart(Coordinate(0,0)), routeDestination(Coordinate(0,0)) {
    m_start_marker = -1;
    m_destination_marker = -1;
}
void Application::setRouteStart(double mouse_x, double mouse_y) {
    Coordinate coord = m_viewport->getCoordinateForPosition(mouse_x, mouse_y);
    setRouteStart(coord);
}

void Application::setRouteStart(Coordinate coord) {
    routeStart = coord;

    if (m_start_marker != -1) {
        m_viewport->removeMarker(m_start_marker);
    }

    Viewport::Marker marker = Viewport::Marker(Viewport::Shape::Boat, coord, 45.0f);
    m_start_marker = m_viewport->addMarker(marker);
    m_viewport->scheduleRender();
    g_print("Route start set to (%4.2f, %4.2f)!\n", coord.lat, coord.lon);
}

void Application::setRouteDestination(double mouse_x, double mouse_y) {
    Coordinate coord = m_viewport->getCoordinateForPosition(mouse_x, mouse_y);
    setRouteDestination(coord);
}
void Application::setRouteDestination(Coordinate coord) {
    routeDestination = coord;

    if (m_destination_marker != -1) {
        m_viewport->removeMarker(m_destination_marker);
    }

    Viewport::Marker marker = Viewport::Marker(Viewport::Shape::Boat, coord, 90.0f);
    m_destination_marker = m_viewport->addMarker(marker);
    m_viewport->scheduleRender();
    g_print("Route destination set to (%4.2f, %4.2f)!\n", coord.lat, coord.lon);
}
