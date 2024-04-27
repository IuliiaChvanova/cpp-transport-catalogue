#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"



/*
 * Здесь можно было бы разместить код обработчика запросов к базе, содержащего логику, которую не
 * хотелось бы помещать ни в transport_catalogue, ни в json reader.
 *
 * В качестве источника для идей предлагаем взглянуть на нашу версию обработчика запросов.
 * Вы можете реализовать обработку запросов способом, который удобнее вам.
 *
 * Если вы затрудняетесь выбрать, что можно было бы поместить в этот файл,
 * можете оставить его пустым.
 */

// Класс RequestHandler играет роль Фасада, упрощающего взаимодействие JSON reader-а
// с другими подсистемами приложения.
// См. паттерн проектирования Фасад: https://ru.wikipedia.org/wiki/Фасад_(шаблон_проектирования)



class RequestHandler {
public:
    // MapRenderer понадобится в следующей части итогового проекта
   // RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);
     RequestHandler(const TransportCatalogue& db, const MapRenderer& renderer);

    // Возвращает информацию о маршруте (запрос Bus)
    const BusInfo GetBusInfo(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через остановку
   const StopInfo GetStopInfo(const std::string_view& stop_name) const;

  
   
    // Этот метод будет нужен в следующей части итогового проекта
    
   [[maybe_unused]]svg::Document RenderMap (std::ostream& output) const;
   

private:
  const std::set<std::string_view> GetSortedBusesNamesOnRoute() const;
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& transport_catalogue_;
    const MapRenderer& renderer_;
};
