//
// Created by ephir on 13.12.2025.
//

#include "LabelsParser.h"
#include <fstream>

std::map<std::string, std::string> LabelsParser::load_labels(const std::string& filename) {
    std::map<std::string, std::string> res;
    auto stream = std::ifstream(filename);
    if (!stream) return res;
    std::string label, id;
    while (stream >> label >> id) res.insert_or_assign(label, id);
    return res;
}

void LabelsParser::save_labels(const std::string& filename, const std::map<std::string, std::string>& labels) {
    auto stream = std::ofstream(filename);
    if (!stream) return;
    for (const auto& [label, id] : labels) stream << label << " " << id << '\n';
}
