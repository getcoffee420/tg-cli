//
// Created by ephir on 13.12.2025.
//

#ifndef TG_CLI_LABELSPARSER_H
#define TG_CLI_LABELSPARSER_H

#include <string>
#include <fstream>
#include <map>

class LabelsParser final {
public:
    static std::map<std::string, std::string> load_labels(const std::string& filename);

    static void save_labels(const std::string& filename, const std::map<std::string, std::string>& labels);
};


#endif //TG_CLI_LABELSPARSER_H
