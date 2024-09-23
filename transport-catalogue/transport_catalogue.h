#pragma once
#include <deque>
#include <optional>
#include <unordered_map>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>
#include "domain.h"
#include "geo.h"



class TransportCatalogue {
public:
    //Добавление остановки в базу
    void AddStop(std::string_view name, geo::Coordinates coordinates);
   // добавление маршрута в базу
    void AddBus(std::string_view name, const std::vector<std::string_view>& parsed_stops, bool circle);
    
    void AddDistance (std::string_view from_stop, std::string_view to_stop, int distance);
    // возвращает расстояние между двумя остановками
    std::optional<int> GetDistance (std::string_view from_stop, std::string_view to_stop);
    //поиск маршрута по имени
    Bus* FindBus(std::string_view name) const; 
    //поиск остановки по имени
    Stop* FindStop (std::string_view name) const;
    //получение информации о маршруте.
    BusInfo GetBusInfo(std::string_view name) const;
    StopInfo GetStopInfo(std::string_view name) const;
    const std::set<std::string_view> GetSortedBusesNames() const;
    const std::set<std::string_view> GetSortedStopsNames() const;
    std::vector<geo::Coordinates> GetStopsCoordinatesForBus(std::string_view bus_name) const;
    std::vector<geo::Coordinates> GetAllStopsCoordinatesForBuses() const;
    const std::vector<std::string_view> GetAllStopsNames() const;
    const std::vector<std::string_view>  GetAllBusesNames() const;
     int GetDistance(const Stop* stop_from, const Stop* stop_to) const;
    
private:
    class PairPtrHasher{
        public:
        size_t operator() (const std::pair< const Stop*, const Stop*> stops_pair) const{
            return static_cast<size_t>(reinterpret_cast<std::uintptr_t>(stops_pair.first)*37 + reinterpret_cast<std::uintptr_t>(stops_pair.second));
        }
};
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
    std::unordered_map<std::string_view, Bus*> busname_to_bus_;
    std::unordered_map<Stop*, std::set<std::string>> stop_to_busname_; 
    std::unordered_map<std::pair<const Stop*, const Stop*>, int, PairPtrHasher> distances_;
};