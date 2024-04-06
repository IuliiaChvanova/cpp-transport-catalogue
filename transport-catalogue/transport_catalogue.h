#pragma once
#include <deque>
#include <unordered_map>
#include <set>
#include <string>
#include <string_view>

#include "geo.h"

struct Stop {
    std::string name;
    detail::Coordinates coordinates;
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

class TransportCatalogue {
public:
    //Добавление остановки в базу
    void AddStop(std::string_view name, detail::Coordinates coordinates);
   // добавление маршрута в базу
    void AddBus(std::string_view name, const std::vector<std::string_view>& parsed_stops, bool circle);
    
    void AddDistance (std::string_view stop1, std::string_view stop2, int distance);
    //поиск маршрута по имени
    Bus* FindBus(std::string_view name); 
    //поиск остановки по имени
    Stop* FindStop (std::string_view name);
    //получение информации о маршруте.
    BusInfo GetBusInfo(std::string_view name) const;
    StopInfo GetStopInfo(std::string_view name) const;
    
private:
    // Спасибо за комметарии. Хорошего дня:)
    class PairPtrHasher{
        public:
        size_t operator() (const std::pair<Stop*, Stop*> stops_pair) const{
            return static_cast<size_t>(reinterpret_cast<std::uintptr_t>(stops_pair.first)*37 + reinterpret_cast<std::uintptr_t>(stops_pair.second));
        }
};
    std::deque<Stop> all_stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<Stop*, std::set<std::string>> stop_to_busname_; 
    std::unordered_map<std::pair<Stop*, Stop*>, int, PairPtrHasher> distances_;
};