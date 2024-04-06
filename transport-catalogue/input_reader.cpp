#include "input_reader.h"
#include <algorithm>
#include <cassert>
#include <iterator>
#include <map>    
#include <vector>   
#include <utility>    

/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
namespace detail{
Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}
    }

void InputReader::ParseLine(std::string_view line) {
    auto command_description = detail::ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}
    
void GetDescriptionsWithDistances(std::vector<std::pair<std::string_view, std::string_view>>& description_dist, std::string_view str, std::string_view stop_name){
    
    auto comma = str.find(',');
    
  bool prev_comma = false;
    while(comma != std::string::npos){
  
    auto second_comma = str.find(',', comma + 1);
    if (second_comma != std::string::npos && prev_comma == true){
        auto discription = str.substr(comma + 1, second_comma - comma - 1);
       description_dist.push_back({stop_name,discription});
       
    }
    if(second_comma == std::string::npos){
        if(prev_comma == true){
            auto discription = str.substr(comma + 1);
            description_dist.push_back({stop_name,discription});
        }
        break;
    } else {
       comma = str.find(',', second_comma + 1); 
       if (comma != std::string::npos){
          prev_comma = true; 
       }
       auto discription = str.substr(second_comma+1, comma -second_comma - 1 );
       description_dist.push_back({stop_name,discription});
    }
    }   
}    
    
void ParseDistances(std::map<std::pair<std::string_view, std::string_view>,int>& stop_dist,  std::string_view str, std::string_view stop_1) {
   
    auto number = str.find_first_not_of(' ');
    auto space = str.find(' ');
    int distance = std::stoi(std::string((str.substr(number, space - number))));
    
    auto to_word = str.find_first_not_of(' ', space + 1);
    space = str.find(' ', to_word);
    auto stop = str.find_first_not_of(' ', space + 1);
    auto stop_2 = str.substr(stop);

    std::pair<std::string_view, std::string_view> stops1(stop_1, stop_2);
    std::pair<std::string_view, std::string_view> stops2(stop_2, stop_1);
   stop_dist[stops1] = distance;
    if(stop_dist.count (stops2) == 0){
        stop_dist[stops2] = distance;
    }
    } 

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue) const {
    // first proceed only Stops 
    std::vector<CommandDescription> only_buses;
    std::vector<std::pair<std::string_view,std::string_view>> description_dist;
    std::map<std::pair<std::string_view, std::string_view>, int> stops_distance;
    for (const auto& command : commands_){
        if(command.command == "Stop"){
            std::string_view name = detail::Trim(command.id);
            detail::Coordinates coordinates = detail::ParseCoordinates(command.description);
            catalogue.AddStop(name, coordinates);
        } else if (command.command == "Bus"){
            only_buses.push_back(command);
        }
        }
    //еще раз итерируюсь по остановкам чтобы добавить расстояния
    for (const auto& command: commands_){
        if(command.command == "Stop"){
    std::string_view stop_name = detail::Trim(command.id);
            GetDescriptionsWithDistances( description_dist, command.description, stop_name);
        }
    }
    for (const auto& [stop_name,description]: description_dist){
        std::string_view command = detail::Trim(description);
            ParseDistances(stops_distance, command,stop_name);
         
    }
    for (const auto& [stops, distance] : stops_distance){
        catalogue.AddDistance (stops.first, stops.second, distance);
    }

    for (const auto& command : only_buses){
        std::string_view name = detail::Trim(command.id);
        bool circle;
        if(command.description.find('-') != std::string::npos){
            circle = false;
        }
        std::vector<std::string_view> parsed_stops = detail::ParseRoute(detail::Trim(command.description));
        catalogue.AddBus(name, parsed_stops, circle); 
    }
 
}