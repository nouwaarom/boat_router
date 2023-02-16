//
// Created by eput on 2/16/23.
//
#include "chart.h"

using namespace router;

Chart::Chart(int ringCount) {
    rings.reserve(ringCount);
}

void Chart::addRing(std::vector<Coordinate> ring) {
    rings.push_back(ring);
}

std::size_t Chart::getRingCount() {
    return rings.size();
}

std::vector<Coordinate> Chart::getRing(int index) {
    return rings[index];
}
