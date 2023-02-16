/**
 * @file shapefilereader.cpp
 * @brief This file ...
 * @author eput
 */
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include "shapefilereader.h"
#include "ShapefileReader.hpp"

using namespace router;

ShapeFileReader::ShapeFileReader() {
}

void ShapeFileReader::load(std::string filename) {
    // TODO, use filename.
    shp::ShapefileReader shp {std::filesystem::absolute(filename)};
    if (shp.getGeometryType() != shp::GeometryType::Polygon) {
        throw std::runtime_error("Shapefile does not contain a polygon");
    }
    shp::Bounds b = shp.getBounds();
    std::cout << "Bounds = " << b << "\n";
    int numberOfFeatures = shp.getCount();
    std::cout << "Number of Features = " << numberOfFeatures << "\n";
    for(auto const& feature : shp) {
        auto* polygon = dynamic_cast<shp::Polygon*>(feature.getGeometry());
        // Make sure we allocate enough memory
        std::vector<shp::Ring> polygon_rings = polygon->getRings();
        chart = new Chart(polygon_rings.size());
        for (int r = 0; r < polygon_rings.size(); r++) {
            shp::Ring& ring = polygon_rings[r];
            std::vector<shp::Point> ring_points = ring.getPoints();
            // Make sure we allocate enough memory
            std::vector<Coordinate> coordinates;
            coordinates.reserve(ring_points.size());
            std::cout << "Ring with " << ring_points.size() << " ring_points" << std::endl;
            for (auto const& point : ring_points) {
                Coordinate coord = Coordinate(point.getX(), point.getY());
                coordinates.push_back(coord);
            }
            chart->addRing(coordinates);
        }
    }
}

Chart* ShapeFileReader::getChart() {
    return chart;
}
