#pragma once
#include "json_builder.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "transport_router.h"
/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 */

class JsonReader {
public:
    JsonReader(std::istream& input);
    TransportCatalogue BuildTransportCatalogue();
    json::Document MakeJsonDocument(RequestHandler& request_handler);

    RenderSettings GetRenderSettings();
    TransportRoutingSettings GetRouterSettings();
    
private:
    void AddStopsToCatalogueFromJson(TransportCatalogue& transport_catalogue, std::vector<json::Node>& base_requests_vect);
    void AddDistancesToCatalogueFromJson(TransportCatalogue& transport_catalogue, std::vector<json::Node>& base_requests_vect);
    void AddBusesToCatalogueFromJson(TransportCatalogue& transport_catalogue, std::vector<json::Node>& base_requests_vect);
    void AddStopInfoInJSON (RequestHandler& request_handler, std::string_view stop_name, int request_id, json::Array& out_array);
    void AddBusInfoInJSON (RequestHandler& request_handler, std::string_view stop_name, int request_id, json::Array& out_array);
    void AddVisualisationMapInJSON(RequestHandler& request_handler, int request_id, json::Array& out_array);
     //-------Functions for map_render ------------
   void AddColor (std::vector<json::Node> color_vector, json::Node underlayer_color_vector, std::vector<svg::Color>& color_palette, svg::Color& underlayer_color );
  //  void AddRouterSettingsFromJson (TransportRouterBuilder& builder);
  void AddRouteInfoInJSON ( RequestHandler& request_handler, int request_id, std::optional<TransportRouter::RouteInfo>& route, json::Array& out_array);
    
    json::Document requests_;
};