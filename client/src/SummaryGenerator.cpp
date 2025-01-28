#include "SummaryGenerator.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <iostream>
#include "event.h"

// Helper function to convert epoch time to date time string
std::string epochToDateTime(int epoch) {
    std::time_t t = epoch;
    std::tm* tm = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm, "%d/%m/%y %H:%M");
    return oss.str();
}

// Helper function to generate summary of description
std::string generateSummaryHelper(const std::string& description) {
    if (description.length() <= 27) {
        return description;
    }
    return description.substr(0, 27) + "...";
}

void SummaryGenerator::generateSummary(const std::vector<std::string>& args, const std::unordered_map<std::string, std::vector<Event>>& reportedEvents) {
    std::string channel_name = args[1];
    std::string user = args[2];
    std::string file = args[3];

    // Retrieve events for the specified channel
    if (reportedEvents.find(channel_name) == reportedEvents.end()) {
        std::cerr << "Channel not found: " << channel_name << std::endl;
        return;
    }

    std::vector<Event> events = reportedEvents.at(channel_name);
    std::vector<Event> userEvents;

    // Filter events for the specified user
    for (const Event& event : events) {
        if (event.getEventOwnerUser() == user) {
            userEvents.push_back(event);
        }
    }

    std::cout << "[DEBUG]: Total events for user: " << userEvents.size() << std::endl;

    // Generate stats
    int totalReports = userEvents.size();
    int activeCount = 0;
    int forcesArrivalCount = 0;

    for (const Event& event : userEvents) {
        if (event.get_general_information().count("active") && event.get_general_information().at("active") == "true") {
            activeCount++;
        }
        if (event.get_general_information().count("forces arrival at scene") && event.get_general_information().at("forces arrival at scene") == "true") {
            forcesArrivalCount++;
        }
    }

    // Sort events by date_time and then by event_name lexicographically
    std::sort(userEvents.begin(), userEvents.end(), [](const Event& a, const Event& b) {
        if (a.get_date_time() != b.get_date_time()) {
            return a.get_date_time() < b.get_date_time();
        }
        return a.get_name() < b.get_name();
    });

    // Generate report
    std::ofstream outFile(file);
    if (!outFile) {
        std::cerr << "Failed to open file: " << file << std::endl;
        return;
    }

    outFile << "Channel " << channel_name << "\n";
    outFile << "Stats:\n";
    outFile << "Total: " << totalReports << "\n";
    outFile << "active: " << activeCount << "\n";
    outFile << "forces arrival at scene: " << forcesArrivalCount << "\n";
    outFile << "Event Reports:\n";

    for (size_t i = 0; i < userEvents.size(); ++i) {
        const Event& event = userEvents[i];
        outFile << "Report_" << (i + 1) << ":\n";
        outFile << "city: " << event.get_city() << "\n";
        outFile << "date time: " << epochToDateTime(event.get_date_time()) << "\n";
        outFile << "event name: " << event.get_name() << "\n";
        outFile << "summary: " << generateSummaryHelper(event.get_description()) << "\n";
    }

    outFile.close();
    std::cout << "Generating summary for channel: " << channel_name << ", user: " << user << std::endl;
}