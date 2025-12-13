//
// Created by Kirtsuha on 13.12.2025.
//

#pragma once

#include "../tgClient/ITgClient.hpp"

#include <map>
#include <string>
#include <vector>


class LabelsController {
public:
    explicit LabelsController(std::string filename);

    void reload();

    void rebuild_from_chats(const std::vector<ITgClient::Chat>& chats);

    std::string resolve_chat_id(const std::string& label_or_id) const;

    std::string label_for_chat_id(const std::string& chat_id) const;

    const std::map<std::string, std::string>& labels() const { return label_to_id_; }

private:
    std::string filename_;
    std::map<std::string, std::string> label_to_id_;
    std::map<std::string, std::string> id_to_label_;

    void rebuild_reverse_index();
};