#pragma once

#include <iosfwd>
#include <optional>
#include <string_view>
#include <utility>

#include "transport_catalogue.h"

void PrintStop(const TransportCatalogue& transport_catalogue, std::string_view stop_name, std::ostream& output);

void PrintBus(const TransportCatalogue& transport_catalogue, std::string_view stop_name, std::ostream& output);

std::optional<std::pair<std::string, std::string>> Parse(std::string_view request);

void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                       std::ostream& output);
                    