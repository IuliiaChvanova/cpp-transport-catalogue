#include <iostream>
#include <cassert>
#include <string>


#include "map_renderer.h"
#include "request_handler.h"
#include "json_reader.h"


using namespace std;

int main() {
 /*
    json::Print(
        json::Document{
            json::Builder{}
            .StartDict()
                .Key("key1"s).Value(123)
                .Key("key2"s).Value("value2"s)
                .Key("key3"s).StartArray()
                    .Value(456)
                    .StartDict().EndDict()
                    .StartDict()
                        .Key(""s)
                        .Value(nullptr)
                    .EndDict()
                    .Value(""s)
                .EndArray()
            .EndDict()
            .Build()
        },
        cout
    );
    cout << endl;
 
    json::Print(
        json::Document{
            json::Builder{}
            .Value("just a string"s)
            .Build()
        },
        cout
    );
    cout << endl;
    
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