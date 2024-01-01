#pragma once
#include <iostream> 
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"

struct CommandDescription {
    // Определяет, задана ли команда (поле command непустое)
    explicit operator bool() const {
        return !command.empty();
    }

    bool operator!() const {
        return !operator bool();
    }

    std::string command;      // Название команды
    std::string id;           // id маршрута или остановки
    std::string description;  // Параметры команды
};

class InputReader {
public:
    /**
     * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
     */ //string: find_first_not_of, find_last_not_of, find, substr.
    void ParseLine(std::string_view line);
    
    // DELETE
   void PrintInput(){
       std::cout << commands_.size() << std::endl;
       for (const auto& command : commands_){
           
           std::cout << command.command << "_" << command.id << "_" << command.description << std::endl;
       }
   }

    /**
     * Наполняет данными транспортный справочник, используя команды из commands_
     */
    void ApplyCommands(TransportCatalogue& catalogue) const;

private:
    std::vector<CommandDescription> commands_;
};