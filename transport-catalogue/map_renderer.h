#pragma once
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <vector>

#include "domain.h"
#include "svg.h"


inline const double EPSILON = 1e-6;





class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
        min_lon_ = left_it->lng;
        const double max_lon = right_it->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
        const double min_lat = bottom_it->lat;
        max_lat_ = top_it->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        
       
        if (!IsZeroValue(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }
     
    
    /*
       // instead of above function
       if(!(std::abs(max_lon - min_lon_) < EPSILON)){
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
       }
       */

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
       
       if (!IsZeroValue(max_lat_ - min_lat)) {
         height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
       }
      
       /*
       // instead of above function 
        if(!(std::abs(max_lat_ - min_lat) < EPSILON)){
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
       }
       */

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }


    

private:

    bool IsZeroValue(double value) {
    return std::abs(value) < EPSILON;
}
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};



struct RenderSettings {
    double width;
    double height;

    double padding;

    double line_width;
    double stop_radius;

    int bus_label_font_size; //???????? type ???? int or double
    std::vector<double> bus_label_offset;

    int stop_label_font_size; //???????? type ???? int or double
    std::vector<double> stop_label_offset;

    svg::Color underlayer_color;
    double underlayer_width;

    std::vector<svg::Color> color_palette;
};

class MapRenderer {
public:    
    MapRenderer(const RenderSettings& render_settings)
        :render_settings_(render_settings)
        {
        }
    svg::Polyline DrawBusRoute(const std::vector<geo::Coordinates> geo_coords, int bus_order, const SphereProjector& sphere_projector) const;
    std::vector<svg::Text> DrawBusName(const Bus* bus, int bus_order, const SphereProjector& sphere_projector) const;
    svg::Circle DrawStop(const Stop* stop, const SphereProjector& sphere_projector) const;
    std::vector<svg::Text> DrawStopName(const Stop* stop, const SphereProjector& sphere_projector) const;

    std::vector<svg::Text> DrawBusTextAndUnderlayer(const Bus* bus, int bus_order, int stop_order, const SphereProjector& sphere_projector) const;
    std::vector<svg::Point> ProjectCoordinates (const std::vector<geo::Coordinates> geo_coords, const SphereProjector& sphere_projector) const;
    const SphereProjector CreateSphereProjector (const std::vector<geo::Coordinates> all_coords) const;

  
  void RenderMap (std::vector<std::variant<svg::Polyline,svg::Text, svg::Circle >>& drawing,
const std::vector<Bus*>& buses_ptrs, const std::vector<Stop*>& stops_ptrs) const;
                                            
    
private:

    const RenderSettings render_settings_;  
    
};
