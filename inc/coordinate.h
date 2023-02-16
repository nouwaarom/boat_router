//
// Created by eput on 2/10/23.
//

#ifndef WEATHER_ROUTER_INC_COORDINATE_H
#define WEATHER_ROUTER_INC_COORDINATE_H

namespace router {
   class Coordinate {
   public:
       Coordinate(double lat, double lon);

       double lat;
       double lon;
   };
}

#endif //WEATHER_ROUTER_INC_COORDINATE_H
