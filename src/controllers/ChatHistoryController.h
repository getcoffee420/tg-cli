#pragma once
#include <string>
#include <vector>
#include "../tgClient/ITgClient.hpp"

class ChatHistoryController {
private:
    ITgClient& tgClient;
    
public:
    std::string target_chat_id;

    explicit ChatHistoryController(ITgClient& client);

    ChatHistoryController(const ChatHistoryController&) = delete;
    ChatHistoryController& operator=(const ChatHistoryController) = delete;

    ~ChatHistoryController() = default;
    
    void set_target_chat_id(const std::string& chatId);

    std::string get_target_chat_id() const;

    void clear_target_chat_id();
    
    std::vector<ITgClient::Message> get_target_chat_history(int limit = 20);
    
    std::vector<ITgClient::Message> get_chat_history(const std::string& chatId, int limit = 20);

};