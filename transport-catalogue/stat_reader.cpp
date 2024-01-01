#include <iomanip>
#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"



void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output) {
    std::string bus_name;
    std::string stop_name;
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
