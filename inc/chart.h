//
// Created by eput on 2/16/23.
//

#ifndef WEATHER_ROUTER_INC_CHART_H
#define WEATHER_ROUTER_INC_CHART_H

#include <vector>
#include "coordinate.h"

namespace router {
    class Chart {
    public:
        Chart(int ringCount);
        void addRing(std::vector<Coordinate> ring);

        std::size_t getRingCount();
        std::vector<Coordinate> getRing(int index);
    private:
        std::vector<std::vector<Coordinate>> rings;
    };
}

#endif //WEATHER_ROUTER_INC_CHART_H
