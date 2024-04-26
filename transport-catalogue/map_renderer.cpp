#include "map_renderer.h"

/*
 * В этом файле вы можете разместить код, отвечающий за визуализацию карты маршрутов в формате SVG.
 * Визуализация маршртутов вам понадобится во второй части итогового проекта.
 * Пока можете оставить файл пустым.
 */

std::vector<svg::Point>  MapRenderer::ProjectCoordinates(const std::vector<geo::Coordinates> geo_coords, const SphereProjector& sphere_projector) const {
     std::vector<svg::Point> screen_coords;

                              
    //std::vector<Stop*> stops = bus->stops;
    for (const auto& geo_coord : geo_coords){
        const svg::Point screen_coord = sphere_projector(geo_coord);
        screen_coords.push_back(screen_coord);
    }

    return screen_coords;
}


svg::Polyline MapRenderer::DrawBusRoute(const std::vector<geo::Coordinates> geo_coords,int bus_order, const SphereProjector& sphere_projector) const {
   std::vector<svg::Point> screen_coords = ProjectCoordinates(geo_coords, sphere_projector);
    svg::Polyline polyline;
    //отправить в функцию количество автобусов чтобы правильно установить цвета для линий
    for (const auto& point : screen_coords){
        polyline.AddPoint(point);

    }
    polyline.SetFillColor("none").SetStrokeWidth(render_settings_.line_width).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND).SetStrokeColor(render_settings_.color_palette[bus_order%render_settings_.color_palette.size()]);
    return polyline;
}  

std::vector<svg::Text> MapRenderer::DrawBusTextAndUnderlayer(const Bus* bus, int bus_order, int stop_order, const SphereProjector& sphere_projector) const {
    std::vector<svg::Text> text_underlayer;
    svg::Text text;
    svg::Text underlayer;
   geo::Coordinates stop_coords = bus->stops[stop_order]->coordinates; //lng -x, lat -y
   std::vector<geo::Coordinates> stop_coord_vect;
   stop_coord_vect.push_back(stop_coords);
   std::vector<svg::Point> screen_coords = ProjectCoordinates(stop_coord_vect, sphere_projector);
   //спроецировать остановки
     //свойства подложки
    underlayer.SetPosition(screen_coords[0]);
    underlayer.SetOffset(svg::Point{render_settings_.bus_label_offset[0], render_settings_.bus_label_offset[1]});
    underlayer.SetFontSize(render_settings_.bus_label_font_size);
    underlayer.SetFontFamily("Verdana");
    underlayer.SetFontWeight("bold");
    underlayer.SetFillColor(render_settings_.underlayer_color);
    underlayer.SetStrokeColor(render_settings_.underlayer_color);
    underlayer.SetStrokeWidth(render_settings_.underlayer_width);
    underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    underlayer.SetData(bus->name);

    
    
    // общие свойства
    text.SetPosition(screen_coords[0]);
    text.SetOffset(svg::Point{render_settings_.bus_label_offset[0], render_settings_.bus_label_offset[1]});
    text.SetFontSize(render_settings_.bus_label_font_size);
    text.SetFontFamily("Verdana");
    text.SetFontWeight("bold");
    text.SetFillColor(render_settings_.color_palette[bus_order%render_settings_.color_palette.size()]);
    text.SetData(bus->name);

   
    text_underlayer.push_back(underlayer);
    text_underlayer.push_back(text);
    return text_underlayer;
}

std::vector<svg::Text> MapRenderer::DrawBusName(const Bus* bus, int bus_order, const SphereProjector& sphere_projector) const {
    std::vector<svg::Text> result = DrawBusTextAndUnderlayer(bus, bus_order, 0, sphere_projector);
    bool is_roundtrip = bus->circle;
    if (is_roundtrip == false){
        int end_stop = bus->stops.size()/2;
        if (bus->stops[0]->name != bus->stops[end_stop]->name){
            std::vector<svg::Text> end_stop_text = DrawBusTextAndUnderlayer(bus, bus_order, end_stop, sphere_projector);
            for (const auto& text : end_stop_text) {
                result.push_back(text);
            }
        } 
    }
    return result;
}

std::vector<svg::Text> MapRenderer::DrawStopName(const Stop* stop, const SphereProjector& sphere_projector) const {
    std::vector<svg::Text> result;
    svg::Text text;
    svg::Text underlayer;
  
   std::vector<geo::Coordinates> stop_coord_vect;
   stop_coord_vect.push_back(stop->coordinates);
   std::vector<svg::Point> screen_coords = ProjectCoordinates(stop_coord_vect, sphere_projector);
   //спроецировать остановки
     //свойства подложки
    underlayer.SetPosition(screen_coords[0]);
    underlayer.SetOffset(svg::Point{render_settings_.stop_label_offset[0], render_settings_.stop_label_offset[1]});
    underlayer.SetFontSize(render_settings_.stop_label_font_size);
    underlayer.SetFontFamily("Verdana");
    underlayer.SetFillColor(render_settings_.underlayer_color);
    underlayer.SetStrokeColor(render_settings_.underlayer_color);
    underlayer.SetStrokeWidth(render_settings_.underlayer_width);
    underlayer.SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    underlayer.SetData(stop->name);

    // общие свойства
    text.SetPosition(screen_coords[0]);
    text.SetOffset(svg::Point{render_settings_.stop_label_offset[0], render_settings_.stop_label_offset[1]});
    text.SetFontSize(render_settings_.stop_label_font_size);
    text.SetFontFamily("Verdana");
    text.SetFillColor("black");
    text.SetData(stop->name);

    result.push_back(underlayer);
    result.push_back(text);
    return result;
}


svg::Circle MapRenderer::DrawStop(const Stop* stop, const SphereProjector& sphere_projector) const {
    svg::Circle stop_symbol;
   std::vector<geo::Coordinates> stop_coord_vect;
   stop_coord_vect.push_back(stop->coordinates);
   std::vector<svg::Point> screen_coords = ProjectCoordinates(stop_coord_vect, sphere_projector);
   stop_symbol.SetCenter(screen_coords[0]).SetRadius(render_settings_.stop_radius).SetFillColor("white");
    return stop_symbol;
}


const SphereProjector MapRenderer::CreateSphereProjector(const std::vector<geo::Coordinates> all_coords) const {
    
    return  SphereProjector{all_coords.begin()
                                    ,all_coords.end()
                                    ,render_settings_.width
                                    ,render_settings_.height
                                    ,render_settings_.padding};                              
}

void MapRenderer::RenderMap (svg::Document& doc,  const std::vector<std::vector<geo::Coordinates>>& geo_coordinates_for_each_bus,
const std::vector<Bus*>& buses_ptrs, const std::vector<Stop*>& stops_ptrs) const {

    std::vector<geo::Coordinates> all_geo_coordinates;
    for (const auto& stop : stops_ptrs){
        all_geo_coordinates.push_back(stop->coordinates);
       
    }
   
    //svg::Document doc;
    int bus_order = 0;
    const SphereProjector sphere_projector = CreateSphereProjector(all_geo_coordinates);
    
    //----------------- Отрисовываем линиии маршрутов-----------------
    for (const auto& geo_coordinates : geo_coordinates_for_each_bus) {
       
        //const SphereProjector sphere_projector = renderer_.CreateSphereProjector(all_geo_coordinates);
        svg::Polyline polyline = DrawBusRoute(geo_coordinates, bus_order, sphere_projector);
        ++bus_order;
        doc.Add(std::move(polyline));
    }

    //----------------- Названия Маршрутов-----------------
    bus_order = 0;
    for (const auto& bus_ptr : buses_ptrs) {
        std::vector<svg::Text> name = DrawBusName(bus_ptr, bus_order, sphere_projector);
        for (const auto& text : name) {
            doc.Add(std::move(text));
        }
        ++bus_order;
    }

    //----------------- Отрисовываем круги остановок-----------------

    
    for (const auto& stop : stops_ptrs){
        svg::Circle stop_symbol = DrawStop(stop, sphere_projector);
        doc.Add(std::move(stop_symbol));
    }

    //------------------Названия остановок-----------------
    for (const auto& stop : stops_ptrs){
        std::vector<svg::Text> name = DrawStopName(stop, sphere_projector);
        for (const auto& text : name) {
            doc.Add(std::move(text));
        }
    }

   
}



