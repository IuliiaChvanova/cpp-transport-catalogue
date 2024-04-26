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
        const auto map = node.AsMap();
        if(map.at("type").AsString() == "Stop"){
            std::string_view from_stop = map.at("name").AsString();
            auto stops_distance = map.at("road_distances").AsMap();
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
        const auto map = node.AsMap();
        if(map.at("type").AsString() == "Stop"){
            std::string_view from_stop = map.at("name").AsString();
            auto stops_distance = map.at("road_distances").AsMap();
            for (const auto& [to_stop, distance] : stops_distance){
                transport_catalogue.AddDistance(from_stop, to_stop, distance.AsInt());
            }   
        }
}  

  for (const auto& node : base_requests_vect){
        const auto map = node.AsMap();
        if(map.at("type").AsString() == "Stop"){
            std::string_view from_stop = map.at("name").AsString();
            auto stops_distance = map.at("road_distances").AsMap();
           
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
        const auto map = node.AsMap();
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

TransportCatalogue JsonReader::BuildTransportCatalogue(){
    TransportCatalogue transport_catalogue;
    using namespace json;
    const Node node =  requests_.GetRoot();
    const std::map<std::string, Node> requests_map = node.AsMap();
    std::vector<Node> base_requests_vect = requests_map.at("base_requests").AsArray();
    AddStopsToCatalogueFromJson(transport_catalogue, base_requests_vect);
    AddDistancesToCatalogueFromJson(transport_catalogue, base_requests_vect);
    AddBusesToCatalogueFromJson(transport_catalogue, base_requests_vect);
    return transport_catalogue;
}

void JsonReader::AddStopInfoInJSON (RequestHandler& request_handler, std::string_view stop_name,int request_id, json::Array& out_array){
    using namespace json;
    StopInfo stop_info = request_handler.GetStopInfo(stop_name);
            if (!stop_info.found){
                Node error_str{std::string("not found")};
                Node dict_node{Dict{{"request_id", request_id}, {"error_message", error_str}}};
                out_array.push_back(dict_node);
               
            } else {
                Array buses; 
                for (const auto& bus : stop_info.buses){
                    buses.push_back(bus);
                }
              
               // Node array{Array{buses}};
                Node dict_node{Dict{{"buses", buses}, {"request_id", request_id}}};
                out_array.push_back(dict_node);
            }
}


void JsonReader::AddBusInfoInJSON (RequestHandler& request_handler, std::string_view stop_name,int request_id, json::Array& out_array){
    using namespace json;
    BusInfo bus_info = request_handler.GetBusInfo(stop_name);
            if (!bus_info.found){
                Node error_str{std::string("not found")};
                Node dict_node{Dict{{"request_id", request_id}, {"error_message", error_str}}};
                out_array.push_back(dict_node);
               
            } else {
              
                Node dict_node{Dict{{"curvature", bus_info.curvature}
                                    ,{"request_id", request_id}
                                    ,{"route_length", bus_info.route_length}
                                    ,{"stop_count", bus_info.stops_on_route}
                                    ,{"unique_stop_count", bus_info.unique_stops}}};
                out_array.push_back(dict_node);
               
            }
}

void JsonReader::AddVisualisationMapInJSON (RequestHandler& request_handler, int request_id, json::Array& out_array){
    using namespace json;
    std::ostringstream out;
    svg::Document image = request_handler.RenderMap(out);
    std::string string_image = out.str();       
              
               // Node array{Array{buses}};
                Node dict_node{Dict{{"map", string_image}, {"request_id", request_id}}};
                out_array.push_back(dict_node);
          
}

json::Document JsonReader::MakeJsonDocument(RequestHandler& request_handler){
    using namespace json;
    Array out_array;
    const Node node =  requests_.GetRoot();
    const std::map<std::string, Node> requests_map = node.AsMap();
    std::vector<Node> stat_requests_vect = requests_map.at("stat_requests").AsArray();
    

    for (const auto& node : stat_requests_vect){
       
        const auto request_map = node.AsMap();
        if (request_map.at("type").AsString() == "Stop"){
            std::string_view stop_name = request_map.at("name").AsString();
            int request_id = request_map.at("id").AsInt();
            AddStopInfoInJSON (request_handler, stop_name,request_id, out_array);
        } else if (request_map.at("type").AsString() == "Bus"){
            std::string_view stop_name = request_map.at("name").AsString();
            int request_id = request_map.at("id").AsInt();
            AddBusInfoInJSON (request_handler, stop_name,request_id, out_array);
        } else if (request_map.at("type").AsString() == "Map"){
            int request_id = request_map.at("id").AsInt();
            AddVisualisationMapInJSON(request_handler, request_id, out_array);
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
    const std::map<std::string, Node> requests_map = node.AsMap();
    std::map<std::string, Node> render_settings_map = requests_map.at("render_settings").AsMap(); 


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


