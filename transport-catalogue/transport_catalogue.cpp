#include <iostream> //delete
#include <vector>

#include "transport_catalogue.h"

void TransportCatalogue::AddStop(std::string_view name, detail::Coordinates coordinates){
    auto& last_element = all_stops_.emplace_back(Stop{std::string{name},coordinates});
    stopname_to_stop_[last_element.name] = &last_element;
}

void TransportCatalogue::AddBus(std::string_view name, std::vector<std::string_view>& parsed_stops, bool circle){
    Bus bus;
    bus.name = std::string{name};
    bus.circle = circle;
    for (const auto stop : parsed_stops){
        Stop* stop_ptr = stopname_to_stop_[stop];
        bus.stops.push_back(stop_ptr);
        stop_to_busname_[stop_ptr].insert(bus.name);  
    }
    auto& last_element = buses_.emplace_back(bus);
   busname_to_bus_[last_element.name] = &last_element;  
}

Bus* TransportCatalogue::FindBus(std::string& name){
    if(busname_to_bus_.count(name) != 0){
        return busname_to_bus_.at(name);
    }
    return nullptr;
}

Stop* TransportCatalogue::FindStop(std::string& name){
    if(stopname_to_stop_.count(name) != 0){
        return stopname_to_stop_.at(name);
    }
    return nullptr;
}

BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const{
    using namespace detail;
    BusInfo bus_info;
    if(busname_to_bus_.count(name) ==  0){
        bus_info.found = false;
        return bus_info;
    }
    std::vector<Stop*> stops = busname_to_bus_.at(name)->stops;
    std::set<std::string> unique_stops;
    for (const auto& stop : stops){
        unique_stops.insert(stop->name);
    }
    
    if(busname_to_bus_.at(name)->circle == true){
        bus_info.stops_on_route = stops.size();
        bus_info.unique_stops = unique_stops.size();   
    } else {
        bus_info.stops_on_route = stops.size();
        bus_info.unique_stops = unique_stops.size();
    }
    
    //calculate distance
    for(int i = 0; i < stops.size() - 1; ++i){
        bus_info.route_length += ComputeDistance(stops[i]->coordinates, stops[i + 1]->coordinates);
    }
    return bus_info;
}

StopInfo TransportCatalogue::GetStopInfo(std::string_view name) const{
    StopInfo stop_info;
    if (stopname_to_stop_.count(name) == 0){
        return stop_info;
    }
    stop_info.found = true;
    if (stop_to_busname_.count(stopname_to_stop_.at(name)) == 0){
        stop_info.buses = {};
    } else {
        stop_info.buses = stop_to_busname_.at(stopname_to_stop_.at(name));
    }
    return stop_info;
}

void TransportCatalogue::PrintStops(){
 
    std::cout << "PRINT STOPS" << std::endl;
    //print stops
    for (const auto& stop : all_stops_){
        std::cout << stop.name << " " << stop.name.size() << std::endl;
    }
    std::cout << std::endl;
  
    
    }

    void TransportCatalogue::PrintBuses(){
   
    std::cout << "PRINT BUSES" << std::endl;
    for (const auto bus : buses_){
        std::cout << bus.name << ":";
        for (const auto stop : bus.stops){
            std::cout << stop->name << " ";
        }
        std::cout << std::endl;
    }
    }

    void TransportCatalogue::PrintStopsMap(){
        std::cout << "PRINT STOPS MAP" << std::endl;
    for (auto& [name, stop] : stopname_to_stop_){
        std::cout << std::string{name} << "-" << stop->name << std::endl;
    }
    std::cout << std::endl;
  
    }

    





