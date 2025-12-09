

#include "ChatHistoryController.h"

ChatHistoryController::ChatHistoryController(ITgClient& client) 
        : tgClient(client), target_chat_id("") {}
    
// Установить текущий чат для работы
void ChatHistoryController::set_target_chat_id(const std::string& chatId) {
    target_chat_id = chatId;
}

// Получить ID текущего чата
std::string ChatHistoryController::get_target_chat_id() const {
    return target_chat_id;
}

// Очистить текущий чат
void ChatHistoryController::clear_target_chat_id() {
    target_chat_id.clear();
}
    
// Получить историю текущего чата
std::vector<ITgClient::Message> ChatHistoryController::get_target_chat_history(int limit = 20) {
    if (target_chat_id.empty()) {
        // Можно выбросить исключение или вернуть пустой вектор
        return {};
    }
    return tgClient.get_chat_history(target_chat_id, limit);
}
    
// Получить историю конкретного чата
std::vector<ITgClient::Message> ChatHistoryController::get_chat_history(const std::string& chatId, int limit = 20) {
    return tgClient.get_chat_history(chatId, limit);
}
