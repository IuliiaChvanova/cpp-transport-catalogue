#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */



RequestHandler::RequestHandler(const TransportCatalogue& db, const MapRenderer& renderer)
    :transport_catalogue_(db), renderer_(renderer) { 
}



// Возвращает маршруты, проходящие через остановку
const StopInfo RequestHandler::GetStopInfo(const std::string_view& stop_name) const {
    StopInfo stop_info = transport_catalogue_.GetStopInfo(stop_name);
    return stop_info;
}

const BusInfo RequestHandler::GetBusInfo(const std::string_view& bus_name) const {
    BusInfo bus_info = transport_catalogue_.GetBusInfo(bus_name);
    return bus_info;
}

const std::set<std::string_view>  RequestHandler::GetSortedBusesNamesOnRoute() const {
    return transport_catalogue_.GetSortedBusesNames();
}




[[maybe_unused]] svg::Document RequestHandler::RenderMap(std::ostream& output) const {

    const std::set<std::string_view> sorted_buses_names = GetSortedBusesNamesOnRoute();
    std::vector<Bus*> buses_ptrs;
    std::vector<Stop*> stops_ptrs;
    std::vector<std::vector<geo::Coordinates>> geo_coordinates_for_each_bus;
    std::vector<geo::Coordinates> all_geo_coordinates = transport_catalogue_.GetAllStopsCoordinatesForBuses();
    std::vector<geo::Coordinates> geo_coordinates;
    
    for (const auto& bus_name : sorted_buses_names) {
        geo_coordinates =  transport_catalogue_.GetStopsCoordinatesForBus(bus_name);
        geo_coordinates_for_each_bus.push_back(geo_coordinates);
        buses_ptrs.push_back(transport_catalogue_.FindBus(bus_name));
    }

    std::set<std::string_view> stops_names = transport_catalogue_.GetSortedStopsNames();
    for (const auto& stop_name : stops_names){
        stops_ptrs.push_back(transport_catalogue_.FindStop(stop_name)); 
    }

    svg::Document doc = renderer_.RenderMap (output
                                        ,all_geo_coordinates
                                        ,geo_coordinates_for_each_bus
                                        ,buses_ptrs
                                        ,stops_ptrs);

    return  doc;
}















/* Предыдущая версия 

 [[maybe_unused]] svg::Document RequestHandler::RenderMap(std::ostream& output) const {
    const std::set<std::string_view> buses = GetSortedBusesNamesOnRoute();
    std::vector<geo::Coordinates> geo_coordinates;
    std::vector<geo::Coordinates> all_geo_coordinates = transport_catalogue_.GetAllStopsCoordinatesForBuses();
     svg::Document doc;
     int bus_order = 0;
     const SphereProjector sphere_projector = renderer_.CreateSphereProjector(all_geo_coordinates);
    //----------------- Отрисовываем линиии маршрутов-----------------
    for (const auto& bus_name : buses) {
        geo_coordinates =  transport_catalogue_.GetStopsCoordinatesForBus(bus_name);
        //const SphereProjector sphere_projector = renderer_.CreateSphereProjector(all_geo_coordinates);
        svg::Polyline polyline = renderer_.DrawBusRoute(geo_coordinates, bus_order, sphere_projector);
        ++bus_order;
        doc.Add(std::move(polyline));
    }

    //----------------- Названия Маршрутов-----------------
    bus_order = 0;
    for (const auto& bus_name : buses) {
        
       
        std::vector<svg::Text> name = renderer_.DrawBusName(transport_catalogue_.FindBus(bus_name), bus_order, sphere_projector);
        for (const auto& text : name) {
            doc.Add(std::move(text));
        }
        ++bus_order;
        
    }



    //----------------- Отрисовываем круги остановок-----------------

    std::set<std::string_view> stops_names = transport_catalogue_.GetSortedStopsNames();
    for (const auto& stop_name : stops_names){
        Stop* stop = transport_catalogue_.FindStop(stop_name);
        svg::Circle stop_symbol = renderer_.DrawStop(stop, sphere_projector);
        doc.Add(std::move(stop_symbol));
    }

    //------------------Названия остановок-----------------
    for (const auto& stop_name : stops_names){
        Stop* stop = transport_catalogue_.FindStop(stop_name);
        std::vector<svg::Text> name = renderer_.DrawStopName(stop, sphere_projector);
        for (const auto& text : name) {
            doc.Add(std::move(text));
        }
    }


    doc.Render(output);
    return  doc;
}

*/


