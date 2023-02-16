/**
 * @file shapefilereader.h
 * @brief This file ...
 * @author eput
 */
#ifndef SHAPEFILEREADER_H
#define SHAPEFILEREADER_H

#include <vector>
#include <string>
#include "coordinate.h"
#include "chart.h"

namespace router {
    class ShapeFileReader {
    public:
        ShapeFileReader();

        void load(std::string filename);

        Chart* getChart();
    protected:
    private:
        Chart* chart;
    };
}

#endif //SHAPEFILEREADER_H
