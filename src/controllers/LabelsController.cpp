//
// Created by Kirtsuha on 13.12.2025.
//

#include "LabelsController.h"

#include "../label/Label.h"
#include "../label/LabelsParser.h"

#include <utility>

LabelController::LabelController(std::string filename)
    : filename_(std::move(filename)) {
    reload();
}

void LabelController::reload() {
    label_to_id_ = LabelsParser::load_labels(filename_);
    rebuild_reverse_index();
}

void LabelController::rebuild_from_chats(const std::vector<ITgClient::Chat>& chats) {
    label_to_id_.clear();
    id_to_label_.clear();

    auto labels = generate_labels(chats.size());
    for (size_t i = 0; i < chats.size(); ++i) {
        if (chats[i].chatId.empty()) continue;
        label_to_id_.insert_or_assign(labels[i], chats[i].chatId);
    }

    rebuild_reverse_index();
    LabelsParser::save_labels(filename_, label_to_id_);
}

std::string LabelController::resolve_chat_id(const std::string& label_or_id) const {
    auto it = label_to_id_.find(label_or_id);
    if (it != label_to_id_.end()) {
        return it->second;
    }
    return label_or_id;
}

std::string LabelController::label_for_chat_id(const std::string& chat_id) const {
    auto it = id_to_label_.find(chat_id);
    if (it != id_to_label_.end()) {
        return it->second;
    }
    return {};
}

void LabelController::rebuild_reverse_index() {
    id_to_label_.clear();
    for (const auto& [label, id] : label_to_id_) {
        if (!id.empty()) id_to_label_.insert_or_assign(id, label);
    }
}