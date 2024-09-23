#include <map>
#include "json_reader.h"
#include <sstream>


/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

JsonReader::JsonReader(std::istream& input)
    :requests_(json::Load(input)) {
        
    }

void JsonReader::AddStopsToCatalogueFromJson(TransportCatalogue& transport_catalogue, std::vector<json::Node>& base_requests_vect) {
    for (const auto& node : base_requests_vect){
        const auto map = node.AsDict();
        if(map.at("type").AsString() == "Stop"){
            std::string_view from_stop = map.at("name").AsString();
            auto stops_distance = map.at("road_distances").AsDict();
            geo::Coordinates coordinates{map.at("latitude").AsDouble(), map.at("longitude").AsDouble()};
            transport_catalogue.AddStop(from_stop, coordinates);
          /*
            for (const auto& [to_stop, distance] : stops_distance){
                transport_catalogue.AddDistance(from_stop, to_stop, distance.AsInt());
            }
           */
        }

}    
}

void JsonReader::AddDistancesToCatalogueFromJson(TransportCatalogue& transport_catalogue, std::vector<json::Node>& base_requests_vect) {
    for (const auto& node : base_requests_vect){
        const auto map = node.AsDict();
        if(map.at("type").AsString() == "Stop"){
            std::string_view from_stop = map.at("name").AsString();
            auto stops_distance = map.at("road_distances").AsDict();
            for (const auto& [to_stop, distance] : stops_distance){
                transport_catalogue.AddDistance(from_stop, to_stop, distance.AsInt());
            }   
        }
}  

  for (const auto& node : base_requests_vect){
        const auto map = node.AsDict();
        if(map.at("type").AsString() == "Stop"){
            std::string_view from_stop = map.at("name").AsString();
            auto stops_distance = map.at("road_distances").AsDict();
           
            for (const auto& [to_stop, distance] : stops_distance){
               if (transport_catalogue.GetDistance(to_stop,from_stop) == std::nullopt){
                    transport_catalogue.AddDistance(to_stop, from_stop, transport_catalogue.GetDistance(from_stop, to_stop).value());
                }
                if (transport_catalogue.GetDistance(from_stop,to_stop) == std::nullopt){
                    transport_catalogue.AddDistance(from_stop, to_stop, transport_catalogue.GetDistance(to_stop,from_stop).value());
                }  
        }
} 

}
}

void JsonReader::AddBusesToCatalogueFromJson(TransportCatalogue& transport_catalogue, std::vector<json::Node>& base_requests_vect) {
    for (const auto& node : base_requests_vect){
        const auto map = node.AsDict();
        if(map.at("type").AsString() == "Bus"){
            std::vector<std::string_view> bus_stops;
            auto stops_vector = map.at("stops").AsArray();
            for (const auto& stop : stops_vector){
                bus_stops.push_back(stop.AsString());
            }
            transport_catalogue.AddBus(map.at("name").AsString(), bus_stops, map.at("is_roundtrip").AsBool());   
        }
}    
}

TransportRoutingSettings JsonReader::GetRouterSettings() {
    using namespace json;
    const Node node =  requests_.GetRoot();
    const std::map<std::string, Node> requests_map = node.AsDict();
    const auto routing_settings = requests_map.at("routing_settings").AsDict();
   
    int bus_waiting_time = routing_settings.at("bus_wait_time").AsInt();
    double bus_velocity = static_cast<int>(routing_settings.at("bus_velocity").AsDouble());
   
    return TransportRoutingSettings{bus_waiting_time, bus_velocity};
}


/*
void JsonReader::AddRouterSettingsFromJson (TransportRouterBuilder& builder) {
    using namespace json;
    const Node node =  requests_.GetRoot();
    const std::map<std::string, Node> requests_map = node.AsDict();
    const auto routing_settings = requests_map.at("routing_settings").AsDict();
   
    builder
            .SetBusVelocity(static_cast<int>(routing_settings.at("bus_velocity").AsDouble()))
            .SetBusWaitingTime(routing_settings.at("bus_wait_time").AsInt());
}
*/

TransportCatalogue JsonReader::BuildTransportCatalogue(){
    TransportCatalogue transport_catalogue;
    using namespace json;
    const Node node =  requests_.GetRoot();
    const std::map<std::string, Node> requests_map = node.AsDict();
    std::vector<Node> base_requests_vect = requests_map.at("base_requests").AsArray();
    AddStopsToCatalogueFromJson(transport_catalogue, base_requests_vect);
    AddDistancesToCatalogueFromJson(transport_catalogue, base_requests_vect);
    AddBusesToCatalogueFromJson(transport_catalogue, base_requests_vect);
    return transport_catalogue;
}

void JsonReader::AddStopInfoInJSON (RequestHandler& request_handler, std::string_view stop_name,int request_id, json::Array& out_array){
    using namespace json;
    Node result;
    StopInfo stop_info = request_handler.GetStopInfo(stop_name);
            if (!stop_info.found){
           
                result = Builder{}.StartDict()
                                        .Key("request_id").Value(request_id)
                                        .Key("error_message").Value("not found")
                                    .EndDict()    
                                    .Build();
              
            } else {
                Array buses; 
                for (const auto& bus : stop_info.buses){
                    buses.push_back(bus);
                }
            
                result = Builder{}.StartDict()
                                        .Key("buses").Value(buses)
                                        .Key("request_id").Value(request_id)
                                    .EndDict()
                                    .Build();  
               
            }
            out_array.push_back(result);
}


void JsonReader::AddBusInfoInJSON (RequestHandler& request_handler, std::string_view stop_name,int request_id, json::Array& out_array){
    using namespace json;
    Node result;
    BusInfo bus_info = request_handler.GetBusInfo(stop_name);
            if (!bus_info.found){
             
                result = Builder{}.StartDict()
                                        .Key("request_id").Value(request_id)
                                        .Key("error_message").Value("not found")
                                    .EndDict()    
                                    .Build();
             
               
            } else {
                             
                result = Builder{}.StartDict()
                                        .Key("curvature").Value(bus_info.curvature)
                                        .Key("request_id").Value(request_id)
                                        .Key("route_length").Value(bus_info.route_length)
                                        .Key("stop_count").Value(bus_info.stops_on_route)
                                        .Key("unique_stop_count").Value(bus_info.unique_stops)
                                    .EndDict()    
                                    .Build();                    
               // out_array.push_back(dict_node);
               
            }
            out_array.push_back(result);  
}

void JsonReader::AddVisualisationMapInJSON (RequestHandler& request_handler, int request_id, json::Array& out_array){
    using namespace json;
    std::ostringstream out;
    svg::Document image = request_handler.RenderMap(out);
    std::string string_image = out.str();       
    Node result;
              
    result = Builder{}.StartDict()
                            .Key("map").Value(string_image)
                            .Key("request_id").Value(request_id)
                        .EndDict()    
                        .Build();
    out_array.push_back(result);                    
 
          
}

 void JsonReader::AddRouteInfoInJSON ( RequestHandler& request_handler, int request_id, std::optional<TransportRouter::RouteInfo>& route, json::Array& out_array) {
    using namespace json;
    Node result;
     
    if (!route.has_value()) {
                Node result;
                result = Builder{}.StartDict()
                                        .Key("request_id").Value(request_id)
                                        .Key("error_message").Value("not found")
                                   .EndDict()
                                   .Build();
                out_array.push_back(result);
    } else { 
     

   Array items;
   double total_time = 0.0;
   items.reserve(route.value().edges.size());
    for (auto &edge_id: route.value().edges) {
        const graph::Edge<double> edge = request_handler.GetGraph().GetEdge(edge_id);
        if (edge.span_count == 0) {
            items.emplace_back(
                Builder{}.StartDict()
                                .Key("stop_name").Value(edge.name)
                                .Key("time").Value(edge.weight)
                                .Key("type").Value("Wait")
                         .EndDict()
                         .Build()
            );

            total_time += edge.weight;
        } else {
            items.emplace_back(
                Builder{}.StartDict()
                                .Key("bus").Value(edge.name)
                                .Key("span_count").Value(static_cast<int>(edge.span_count))
                                .Key("time").Value(edge.weight)
                                .Key("type").Value("Bus")
                          .EndDict()
                          .Build()
            );

            total_time += edge.weight;
        }
    }
     
   
    result = Builder{}.StartDict()
                              .Key("request_id").Value(request_id)
                              .Key("total_time").Value(total_time)
                              .Key("items").Value(items)
                       .EndDict()
                       .Build();    
    out_array.push_back(result);
    }
}


json::Document JsonReader::MakeJsonDocument(RequestHandler& request_handler){
    using namespace json;
    Array out_array;
    const Node node =  requests_.GetRoot();
    const std::map<std::string, Node> requests_map = node.AsDict();
    std::vector<Node> stat_requests_vect = requests_map.at("stat_requests").AsArray();
    

    for (const auto& node : stat_requests_vect){
        const auto request_map = node.AsDict();
        int request_id = request_map.at("id").AsInt();
        if (request_map.at("type").AsString() == "Stop"){
            std::string_view stop_name = request_map.at("name").AsString();
            AddStopInfoInJSON (request_handler, stop_name,request_id, out_array);
        } else if (request_map.at("type").AsString() == "Bus"){
            std::string_view stop_name = request_map.at("name").AsString(); 
            AddBusInfoInJSON (request_handler, stop_name,request_id, out_array);
        } else if (request_map.at("type").AsString() == "Map"){
            AddVisualisationMapInJSON(request_handler, request_id, out_array);
        } 
        
        else if (request_map.at("type").AsString() == "Route") {
            std::string_view from = request_map.at("from").AsString();
            std::string_view to = request_map.at("to").AsString();
            auto route = request_handler.FindRoute(from, to);
            AddRouteInfoInJSON (request_handler, request_id, route, out_array);
            }  
    }
        Document output(out_array);
        return output;
    
}


    //-------------Functions for Map_renderer

void JsonReader::AddColor (std::vector<json::Node> color_vector, json::Node underlayer_color_node, std::vector<svg::Color>& color_palette, svg::Color& underlayer_color ){
     //-----------underlayer_color-------------------
    
    if (underlayer_color_node.IsString()){
        underlayer_color = underlayer_color_node.AsString();
    } else {
        std::vector<json::Node> underlayer_color_vect = underlayer_color_node.AsArray();
        if (underlayer_color_vect.size() == 1){
                if (underlayer_color_vect[0].IsString()) underlayer_color = underlayer_color_vect[0].AsString();
            } else if (underlayer_color_vect.size() == 3){
                underlayer_color = svg::Rgb{static_cast<uint8_t>(underlayer_color_vect[0].AsInt())
                                            ,static_cast<uint8_t>(underlayer_color_vect[1].AsInt())
                                            ,static_cast<uint8_t>(underlayer_color_vect[2].AsInt())};
            
            } else if (underlayer_color_vect.size() == 4){
                underlayer_color = svg::Rgba{static_cast<uint8_t>(underlayer_color_vect[0].AsInt())
                                            ,static_cast<uint8_t>(underlayer_color_vect[1].AsInt())
                                            ,static_cast<uint8_t>(underlayer_color_vect[2].AsInt())
                                            ,underlayer_color_vect[3].AsDouble()};
            }

    }

    
 
     //-----------color_vector-------------------
       
    for (const auto& color : color_vector){
        if(color.IsString()){
            color_palette.push_back(color.AsString());
        } else if (color.IsArray()) {
            std::vector<json::Node> color_arr = color.AsArray();
            if(color_arr.size() == 3){
                color_palette.emplace_back(svg::Rgb{static_cast<uint8_t>(color_arr[0].AsInt())
                                                ,static_cast<uint8_t>(color_arr[1].AsInt())
                                                ,static_cast<uint8_t>(color_arr[2].AsInt())});
            } else {
                color_palette.emplace_back(svg::Rgba{static_cast<uint8_t>(color_arr[0].AsInt())
                                                  ,static_cast<uint8_t>(color_arr[1].AsInt())
                                                  ,static_cast<uint8_t>(color_arr[2].AsInt())
                                                  ,color_arr[3].AsDouble()});
            }
        
        } 
    }
}


RenderSettings JsonReader::GetRenderSettings(){
    using namespace json;
    Array out_array;
    const Node node =  requests_.GetRoot();
    const std::map<std::string, Node> requests_map = node.AsDict();
    std::map<std::string, Node> render_settings_map = requests_map.at("render_settings").AsDict(); 


    double width = render_settings_map.at("width").AsDouble();
    double height = render_settings_map.at("height").AsDouble();

    double padding = render_settings_map.at("padding").AsDouble();

    double stop_radius = render_settings_map.at("stop_radius").AsDouble();
    double line_width = render_settings_map.at("line_width").AsDouble();
    

    int bus_label_font_size = render_settings_map.at("bus_label_font_size").AsDouble(); 

    auto offset_bus = render_settings_map.at("bus_label_offset").AsArray();
    std::vector<double> bus_label_offset{offset_bus[0].AsDouble(), offset_bus[1].AsDouble()};
    

    int stop_label_font_size = render_settings_map.at("stop_label_font_size").AsDouble(); 

    auto offset_stop = render_settings_map.at("stop_label_offset").AsArray();
    std::vector<double> stop_label_offset{offset_stop[0].AsDouble(), offset_stop[1].AsDouble()};

   
    double underlayer_width = render_settings_map.at("underlayer_width").AsDouble();
    svg::Color underlayer_color;
    std::vector<svg::Color> color_palette;
    
    AddColor (render_settings_map.at("color_palette").AsArray()
              ,render_settings_map.at("underlayer_color")
              ,color_palette
              ,underlayer_color);
   


    return RenderSettings{width, height, padding, line_width, stop_radius, 
bus_label_font_size, bus_label_offset, stop_label_font_size, stop_label_offset,
underlayer_color, underlayer_width, color_palette};
}