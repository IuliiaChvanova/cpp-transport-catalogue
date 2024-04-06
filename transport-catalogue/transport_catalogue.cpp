#include <iostream> //delete
#include <vector>

#include "transport_catalogue.h"

void TransportCatalogue::AddStop(std::string_view name, detail::Coordinates coordinates){
    auto& last_element = all_stops_.emplace_back(Stop{std::string{name},coordinates});
    stopname_to_stop_[last_element.name] = &last_element;
}

void TransportCatalogue::AddDistance (std::string_view stop1, std::string_view stop2, int distance){
    Stop* stop1_ptr = stopname_to_stop_[stop1];
    Stop* stop2_ptr = stopname_to_stop_[stop2];
    distances_[std::pair{stop1_ptr,stop2_ptr}] = distance;
}


void TransportCatalogue::AddBus(std::string_view name, const std::vector<std::string_view>& parsed_stops, bool circle){
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

Bus* TransportCatalogue::FindBus(std::string_view name){
    if(busname_to_bus_.count(name) != 0){
        return busname_to_bus_.at(name);
    }
    return nullptr;
}

Stop* TransportCatalogue::FindStop(std::string_view name){
    if(stopname_to_stop_.count(name) != 0){
        return stopname_to_stop_.at(name);
    }
    return nullptr;
}

BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const{
    using namespace detail;
    BusInfo bus_info;
    double geographic_distance = 0;
    if(busname_to_bus_.count(name) ==  0){
        bus_info.found = false;
        return bus_info;
    }
    std::vector<Stop*> stops = busname_to_bus_.at(name)->stops;
    std::set<std::string> unique_stops;
    Stop* prev_stop = nullptr;
    for (const auto& stop : stops){
        unique_stops.insert(stop->name);
        if(prev_stop){
            bus_info.route_length += distances_.at({prev_stop,stop}); 
        }
        prev_stop = stop;
    }
    
    
    if(busname_to_bus_.at(name)->circle == true){
        bus_info.stops_on_route = stops.size();
        bus_info.unique_stops = unique_stops.size();   
    } else {
        bus_info.stops_on_route = stops.size();
        bus_info.unique_stops = unique_stops.size();
    }
    
    //calculate geographic distance 
    for(int i = 0; i < stops.size() - 1; ++i){
        geographic_distance += ComputeDistance(stops[i]->coordinates, stops[i + 1]->coordinates);
    }
   bus_info.curvature = bus_info.route_length/geographic_distance;
    
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