#include "transport_catalogue.h"

#include <iostream> //delete
#include <vector>

void TransportCatalogue::AddStop(std::string_view name, geo::Coordinates coordinates){
    auto& last_element = stops_.emplace_back(Stop{std::string{name},coordinates});
    stopname_to_stop_[last_element.name] = &last_element;
}

void TransportCatalogue::AddDistance (std::string_view from_stop, std::string_view to_stop, int distance){
    Stop* from_stop_ptr = stopname_to_stop_[from_stop];
    Stop* to_stop_ptr = stopname_to_stop_[to_stop];
    distances_[std::pair{from_stop_ptr,to_stop_ptr}] = distance;
}

std::optional<int> TransportCatalogue::GetDistance (std::string_view from_stop, std::string_view to_stop){
    Stop* from_stop_ptr = stopname_to_stop_[from_stop];
    Stop* to_stop_ptr = stopname_to_stop_[to_stop];
    if (distances_.count(std::pair{from_stop_ptr,to_stop_ptr}) != 0) {
        return distances_.at(std::pair{from_stop_ptr,to_stop_ptr});
    } else {
        return std::nullopt;
    }
    
}

/*
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
*/

void TransportCatalogue::AddBus(std::string_view name, const std::vector<std::string_view>& parsed_stops, bool circle){
    Bus bus;
    bus.name = std::string{name};
    bus.circle = circle;
    //Stop* first_stop_ptr = nullptr;
   // bool first = true;
    for (const auto stop : parsed_stops){
        
        Stop* stop_ptr = stopname_to_stop_[stop];
        bus.stops.push_back(stop_ptr);
        stop_to_busname_[stop_ptr].insert(bus.name);  
    }

    //add stops to the end of "stops" vector depending on Bus type
    if (bus.circle == true){
        //bus.stops.push_back(first_stop_ptr);
    } else {
        int i = bus.stops.size() - 2;
        for (; i >= 0; --i){
            bus.stops.push_back(bus.stops[i]);
        }
    }

    auto& last_element = buses_.emplace_back(bus);
   busname_to_bus_[last_element.name] = &last_element;  
}

Bus* TransportCatalogue::FindBus(std::string_view name) const {
    if(busname_to_bus_.count(name) != 0){
        return busname_to_bus_.at(name);
    }
    return nullptr;
}

Stop* TransportCatalogue::FindStop(std::string_view name) const {
    if(stopname_to_stop_.count(name) != 0){
        return stopname_to_stop_.at(name);
    }
    return nullptr;
}

BusInfo TransportCatalogue::GetBusInfo(std::string_view name) const{
    using namespace geo;
    BusInfo bus_info;
    double geographic_distance = 0;
    if(busname_to_bus_.count(name) ==  0){
        bus_info.found = false;
        return bus_info;
    }
    std::vector<Stop*> stops = busname_to_bus_.at(name)->stops;
    std::set<std::string> unique_stops;
    Stop* prev_stop = nullptr;
   

    //------New--------------------------
    //add stops to the end of "stops" vector depending on Bus type
    /*
    if (busname_to_bus_.at(name)->circle == true){
        stops.push_back(stops[0]);
    } else {
        for (int i = stops.size() - 2; i >= 0; --i){
            stops.push_back(stops[i]);
        }
    }
    */

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
    for (size_t i = 0; i < stops.size() - 1; ++i){
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

const std::set<std::string_view>  TransportCatalogue::GetSortedBusesNames() const {
    std::set<std::string_view> buses_set;
        for (const auto& bus : buses_) {
            if (!bus.stops.empty()) {
                buses_set.insert(bus.name);
            }
        }
        return buses_set;
}

const std::set<std::string_view> TransportCatalogue::GetSortedStopsNames() const {
    std::set<std::string_view> names_set;
    
    for (const auto& bus : buses_){
        for (const auto& stop : bus.stops){
            names_set.insert(stop->name);
        }
    }
    return names_set;
}

const std::vector<std::string_view> TransportCatalogue::GetAllStopsNames() const {
    std::vector<std::string_view> all_stops;
    all_stops.reserve(stops_.size());
    for (const auto& stop : stops_) {
        all_stops.push_back(stop.name);
    }
    return all_stops;
}

const std::vector<std::string_view>  TransportCatalogue::GetAllBusesNames() const {
    std::vector<std::string_view> all_buses;
    all_buses.reserve(buses_.size());
    for (const auto& bus : buses_) {
        all_buses.push_back(bus.name);
    }
    return all_buses;
}

std::vector<geo::Coordinates> TransportCatalogue::GetStopsCoordinatesForBus(std::string_view bus_name) const {
    std::vector<geo::Coordinates> geo_coord;
    Bus* bus = FindBus(bus_name);
    for (const auto& stop : bus->stops){
        geo_coord.push_back(stop->coordinates);
    }
    return geo_coord;
}

std::vector<geo::Coordinates> TransportCatalogue::GetAllStopsCoordinatesForBuses () const {
    std::vector<geo::Coordinates> all_coords;
    std::vector<std::string_view> all_buses_names = GetAllBusesNames();
    for (const auto& bus_name : all_buses_names){
        Bus* bus = busname_to_bus_.at(bus_name);
        for (const auto& stop : bus->stops){
            all_coords.push_back(stop->coordinates);
        }
    }
    return all_coords;
}