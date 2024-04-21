#pragma once
#include <set>
#include <string>
#include <vector>

#include "geo.h"

struct Stop {
    std::string name;
    geo::Coordinates coordinates;
};

struct Bus {
    std::string name;
    std::vector<Stop*> stops;
    bool circle = true;
};

struct BusInfo {
    int stops_on_route;
    int unique_stops;
    int route_length = 0;
    double curvature = 0;
    bool found = true;
};

struct StopInfo {
    bool found = false;
    std::set<std::string> buses;
};