#include "request_handler.h"

/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */



RequestHandler::RequestHandler(const TransportCatalogue& db, const MapRenderer& renderer, const TransportRouter& router)
    :transport_catalogue_(db), renderer_(renderer), router_(router) { 
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

std::optional<TransportRouter::RouteInfo> RequestHandler::FindRoute(std::string_view from, std::string_view to) const {
    return router_.FindRoute(from, to);
}




[[maybe_unused]] svg::Document RequestHandler::RenderMap(std::ostream& output) const {
    std::vector<std::variant<svg::Polyline,svg::Text, svg::Circle >> drawing;
    svg::Document doc;
    const std::set<std::string_view> sorted_buses_names = GetSortedBusesNamesOnRoute();
    std::vector<Bus*> buses_ptrs;
    std::vector<Stop*> stops_ptrs;

    
    for (const auto& bus_name : sorted_buses_names) {
        buses_ptrs.push_back(transport_catalogue_.FindBus(bus_name));
    }

    std::set<std::string_view> stops_names = transport_catalogue_.GetSortedStopsNames();
    for (const auto& stop_name : stops_names){
        stops_ptrs.push_back(transport_catalogue_.FindStop(stop_name)); 
    }

    renderer_.RenderMap (drawing ,buses_ptrs, stops_ptrs);
    for (const auto& item : drawing){
        if(holds_alternative<svg::Polyline>(item)) {
            doc.Add(get<svg::Polyline>(item));
        }
        if(holds_alternative<svg::Text>(item)) {
            doc.Add(get<svg::Text>(item));
        }
        if(holds_alternative<svg::Circle>(item)) {
            doc.Add(get<svg::Circle>(item));
        }
    }
    doc.Render(output);

    return  doc;
}





