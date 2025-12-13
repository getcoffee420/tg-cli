
#include <algorithm>
#include "ChatHistoryController.h"

ChatHistoryController::ChatHistoryController(ITgClient& client) 
        : tgClient(client) {}
    
    
std::vector<ITgClient::Message> ChatHistoryController::get_chat_history(const std::string& chatId, int limit) {
    auto result = tgClient.get_chat_history(chatId, limit);
    std::reverse(result.begin(), result.end());
    return result;
}
