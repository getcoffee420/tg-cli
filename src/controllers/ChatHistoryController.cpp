

#include "ChatHistoryController.h"

ChatHistoryController::ChatHistoryController(ITgClient& client) 
        : tgClient(client), target_chat_id("") {}
    
void ChatHistoryController::set_target_chat_id(const std::string& chatId) {
    target_chat_id = chatId;
}

std::string ChatHistoryController::get_target_chat_id() const {
    return target_chat_id;
}

void ChatHistoryController::clear_target_chat_id() {
    target_chat_id.clear();
}
    
std::vector<ITgClient::Message> ChatHistoryController::get_target_chat_history(int limit = 20) {
    if (target_chat_id.empty()) {
        return {};
    }
    return tgClient.get_chat_history(target_chat_id, limit);
}
    
std::vector<ITgClient::Message> ChatHistoryController::get_chat_history(const std::string& chatId, int limit = 20) {
    return tgClient.get_chat_history(chatId, limit);
}
