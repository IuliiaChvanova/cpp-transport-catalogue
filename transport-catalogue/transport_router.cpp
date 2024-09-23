#include "transport_router.h"

TransportRouter::TransportRouter(const TransportCatalogue& transport_catalogue, const TransportRoutingSettings& settings)
    :settings_(settings)
    , transport_catalogue_(transport_catalogue){
        graph::DirectedWeightedGraph<double> filled_graph(transport_catalogue_.GetAllStopsNames().size()*2);
        graph::VertexId vertex_id = 0;
        FillGraphWithStops(transport_catalogue_,  vertex_id, filled_graph);
        FillGraphWithBuses(transport_catalogue_, filled_graph);
        graph_ = std::move(filled_graph);
        router_ =  std::make_unique<graph::Router<double>>(graph_);
    }


  const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const{
    return graph_;
}

std::optional<graph::Router<double>::RouteInfo> TransportRouter::FindRoute(std::string_view stop_from, std::string_view stop_to) const {
    return router_->BuildRoute(stop_to_id_.at(std::string(stop_from)), stop_to_id_.at(std::string(stop_to)));
}

void TransportRouter::FillGraphWithStops(const TransportCatalogue& transport_catalogue
                                        ,graph::VertexId& vertex_id
                                        ,graph::DirectedWeightedGraph<double>& graph) {
   std::vector<std::string_view> stops_names = transport_catalogue.GetAllStopsNames();
   for (int i = 0; i < stops_names.size(); ++i){
   
       stop_to_id_[stops_names[i]] = vertex_id;
       graph.AddEdge({std::string(stops_names[i])
                     ,0
                     ,vertex_id
                     ,++vertex_id
                     ,static_cast<double>(settings_.bus_waiting_time)});
       ++vertex_id;              
   }
}


void TransportRouter::FillGraphWithBuses(const TransportCatalogue& transport_catalogue
                                        ,graph::DirectedWeightedGraph<double>& graph) {
            
    std::vector<std::string_view> bus_names = transport_catalogue.GetAllBusesNames();

    for(const auto& bus_name : bus_names) {
    const Bus* bus =  transport_catalogue.FindBus(bus_name);
     const auto &stops = bus->stops;
     const auto& num_of_stops = stops.size();
        for (int i = 0; i < num_of_stops - 1; ++i) {
            double weight = 0.0;
            double back_weight = 0.0;
             for (int j = i + 1; j < num_of_stops; ++j) {
                const Stop* stop_from = stops[i];
                const Stop* stop_to = stops[j];
                weight += transport_catalogue.GetDistance(stops[j - 1], stops[j]) / (static_cast<double>(settings_.bus_velocity) * CONVERT_KMPH_TO_MPMIN);
                
                graph.AddEdge({bus->name
                              ,j-i
                              ,stop_to_id_[stop_from->name] + 1 // 
                              ,stop_to_id_[stop_to->name]
                              ,weight});
               
                if (!bus->circle) {
                        back_weight += transport_catalogue.GetDistance(stops[j], stops[j - 1]) / (static_cast<double>(settings_.bus_velocity) * CONVERT_KMPH_TO_MPMIN);
                       
                         graph.AddEdge({bus->name
                              ,j-i
                              ,stop_to_id_[stop_to->name] + 1 
                              ,stop_to_id_[stop_from->name]
                              ,back_weight});
                    }
                
               
            
              }
              } 
             
             }                 
                   
            }

