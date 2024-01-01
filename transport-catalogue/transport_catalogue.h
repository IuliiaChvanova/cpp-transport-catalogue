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
    double route_length = 0;
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
    void AddBus(std::string_view name, std::vector<std::string_view>& parsed_stops, bool circle);
    //поиск маршрута по имени
    Bus* FindBus(std::string& name); 
    //поиск остановки по имени
    Stop* FindStop (std::string& name);
    //получение информации о маршруте.
    BusInfo GetBusInfo(std::string_view name) const;
    StopInfo GetStopInfo(std::string_view name) const;
    void PrintStops();
    void PrintBuses();
    void PrintStopsMap();
    
private:
     class StringViewHasher{
    public:
    size_t operator() (const std::string_view str_v) const{
        // не понимаю почему если я конвертирую адрес по которому лежит строк,то
        //такой хэшер не работает. Этот хэшер должен работать лучше жe, так как у каждой строки свой адрес. Нет?
       // const char* ptr = &(str_v[0]);
       // int l1 = reinterpret_cast<std::uintptr_t>(ptr);
        
       
        int l1 = str_v[0] - 'A';
        int l2 = str_v.size();
        return static_cast<size_t>(l1 + l2*37);
    }
   
};
    std::deque<Stop> all_stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*, StringViewHasher> stopname_to_stop_;
    std::unordered_map<std::string_view, Bus*, StringViewHasher> busname_to_bus_;
    std::unordered_map<Stop*, std::set<std::string>> stop_to_busname_;  
};