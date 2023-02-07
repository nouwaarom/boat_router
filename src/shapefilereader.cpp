/**
 * @file shapefilereader.cpp
 * @brief This file ...
 * @author eput
 */
#include <iostream>
#include <filesystem>
#include "shapefilereader.h"
#include "ShapefileReader.hpp"

void test_shape_reader(void) {
    shp::ShapefileReader shp {std::filesystem::absolute("points.shp")};
    shp::Bounds b = shp.getBounds();
    std::cout << "Bounds = " << b << "\n";
    int numberOfFeatures = shp.getCount();
    std::cout << "Number of Features = " << numberOfFeatures << "\n";
    for(auto const& feature : shp) {
        std::cout << feature.getGeometry()->wkt() << "\n";
    }
}
