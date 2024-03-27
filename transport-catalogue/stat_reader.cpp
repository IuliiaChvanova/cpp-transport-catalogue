#include <iomanip>
#include <optional>
#include <utility>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

std::optional<std::pair<std::string, std::string>> Parse(std::string_view request){
    std::string transpost_name;
    auto space = request.find(' ');
    auto name_first = request.find_first_not_of(' ',space);
    transpost_name = std::string{request.substr(name_first)};
    if(std::string{request.substr(0, space)} == "Bus"){
        return std::pair{transpost_name, "Bus"};
    } else {
        return std::pair{transpost_name, "Stop"}; 
    }
}

void PrintBus(const TransportCatalogue& transport_catalogue, std::string_view bus_name, std::ostream& output) {
    const BusInfo bus_info = transport_catalogue.GetBusInfo(std::string(bus_name));
        if (bus_info.found == false){
            output << "Bus " << std::string{bus_name} << ": " << "not found" << std::endl; 
        } else {
            output << "Bus " << std::string{bus_name} << ": " <<  bus_info.stops_on_route << " stops on route, " << bus_info.unique_stops << " unique stops, " <<
                bus_info.route_length << std::setprecision(6) << " route length" << std::endl;
        }
}

void PrintStop(const TransportCatalogue& transport_catalogue, std::string_view stop_name, std::ostream& output) {
        const StopInfo stop_info = transport_catalogue.GetStopInfo(stop_name);
        if (stop_info.found == false){
            output << "Stop " << std::string{stop_name} << ": " << "not found"; 
        } else {
            std::set<std::string> buses = stop_info.buses;
            if (buses.size() == 0){
            output << "Stop " << std::string{stop_name} << ": " << "no buses";
            } else {
                output << "Stop " << std::string{stop_name} << ": buses";
                for (const auto& bus: buses)
                output <<  " " << bus;
            } 
        }
         output << std::endl; 
}

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    const auto transport_info = Parse(request);
    std::string transport_name = transport_info.value().first;
    std::string transport_type = transport_info.value().second;
    
    if(transport_type == "Bus"){
       PrintBus(transport_catalogue,transport_name, output);
    } else {
        PrintStop(transport_catalogue,transport_name, output);
    }  
}


/*
void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    //std::string bus_name;
    //std::string stop_name;
    std::string transport_name;
    std::string transport_type;
    
    auto space = request.find(' ');
    if(std::string{request.substr(0, space)} == "Bus"){
       auto bus_name_first = request.find_first_not_of(' ',space);
       bus_name = std::string{request.substr(bus_name_first)};
        const BusInfo bus_info = transport_catalogue.GetBusInfo(bus_name);
        if (bus_info.found == false){
            output << "Bus " << std::string{bus_name} << ": " << "not found" << std::endl; 
        } else {
            output << "Bus " << std::string{bus_name} << ": " <<  bus_info.stops_on_route << " stops on route, " << bus_info.unique_stops << " unique stops, " <<
                bus_info.route_length << std::setprecision(6) << " route length" << std::endl;
        }
    } else {
        auto stop_name_first = request.find_first_not_of(' ',space);
       stop_name = std::string{request.substr(stop_name_first)};
        const StopInfo stop_info = transport_catalogue.GetStopInfo(stop_name);
        if (stop_info.found == false){
            output << "Stop " << std::string{stop_name} << ": " << "not found"; 
        } else {
            std::set<std::string> buses = stop_info.buses;
            if (buses.size() == 0){
            output << "Stop " << std::string{stop_name} << ": " << "no buses";
            } else {
                output << "Stop " << std::string{stop_name} << ": buses";
                for (const auto& bus: buses)
                output <<  " " << bus;
            } 
        }
         output << std::endl; 
    }  
}
*/