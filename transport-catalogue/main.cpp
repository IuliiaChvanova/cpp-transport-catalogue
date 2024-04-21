#include <iostream>
#include <cassert>
#include <string>

#include "map_renderer.h"
#include "request_handler.h"
#include "json_reader.h"

using namespace std;

int main() {
   
  /* 
int main() {
   
     * Примерная структура программы:
     *
     * Считать JSON из stdin
     * Построить на его основе JSON базу данных транспортного справочника
     * Выполнить запросы к справочнику, находящиеся в массивe "stat_requests", построив JSON-массив
     * с ответами Вывести в stdout ответы в виде JSON

}
 */
  
    JsonReader json_reader(std::cin);
    const TransportCatalogue& catalogue = json_reader.BuildTransportCatalogue();
    const RenderSettings& render_settings = json_reader.GetRenderSettings();
    const MapRenderer map_renderer(render_settings);
    RequestHandler request_handler(catalogue, map_renderer);
   // svg::Document doc_out = request_handler.RenderMap(std::cout);
    
    json::Document doc_out = json_reader.MakeJsonDocument(request_handler);
    
    json::Print(doc_out, std::cout);
 
}
