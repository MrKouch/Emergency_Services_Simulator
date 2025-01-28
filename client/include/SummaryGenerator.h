#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "event.h"

class SummaryGenerator {
public:
    static void generateSummary(const std::vector<std::string>& args, const std::unordered_map<std::string, std::vector<Event>>& reportedEvents);
};