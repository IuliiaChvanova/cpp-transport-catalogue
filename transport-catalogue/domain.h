#pragma once
#include <set>
#include <string>
#include <vector>

#include "geo.h"
/*
 * В этом файле вы можете разместить классы/структуры, которые являются частью предметной области (domain)
 * вашего приложения и не зависят от транспортного справочника. Например Автобусные маршруты и Остановки. 
 *
 * Их можно было бы разместить и в transport_catalogue.h, однако вынесение их в отдельный
 * заголовочный файл может оказаться полезным, когда дело дойдёт до визуализации карты маршрутов:
 * визуализатор карты (map_renderer) можно будет сделать независящим от транспортного справочника.
 *
 * Если структура вашего приложения не позволяет так сделать, просто оставьте этот файл пустым.
 *
 */

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