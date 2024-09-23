#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"


struct TransportRoutingSettings  {
    int bus_waiting_time;
    double bus_velocity;
};

class TransportRouter {
public:
    constexpr static const double CONVERT_KMPH_TO_MPMIN = 1000.0 / 60.0;
    using RouteInfo = graph::Router<double>::RouteInfo;

    TransportRouter(const TransportCatalogue& transport_catalogue, const TransportRoutingSettings& settings);
    const graph::DirectedWeightedGraph<double>& GetGraph() const;
    std::optional<graph::Router<double>::RouteInfo> FindRoute(std::string_view stop_from, std::string_view stop_to) const;
   
    
    
private:    
    void FillGraphWithStops (const TransportCatalogue& transport_catalogue
                            ,graph::VertexId& vertex_id 
                            ,graph::DirectedWeightedGraph<double>& graph);
    void FillGraphWithBuses (const TransportCatalogue& transport_catalogue
                             ,graph::DirectedWeightedGraph<double>& graph);
    
    const TransportRoutingSettings& settings_;
    const TransportCatalogue& transport_catalogue_;
    graph::DirectedWeightedGraph<double> graph_;
    std::unordered_map<std::string_view, graph::VertexId> stop_to_id_;
    std::unique_ptr<graph::Router<double>> router_{};
};

