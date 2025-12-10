// controllers/chatscontroller.h
#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "../tgClient/ITgClient.hpp"

class ChatsController {
private:
    ITgClient& client_;
    
    std::vector<ITgClient::Chat> cached_chats_;
    std::map<std::string, std::string> chat_titles_;  // chat_id -> title
    time_t last_cache_update_;
    
public:
    explicit ChatsController(ITgClient& client);
    
    ChatsController(const ChatsController&) = delete;
    ChatsController& operator=(const ChatsController&) = delete;
    
    ~ChatsController() = default;

    std::vector<ITgClient::Chat> get_chats(int limit = 10);
    
    void refresh_chats();
    
    std::vector<ITgClient::Chat> search_chats(
        const std::string& query,
        int limit = 10
    );
    
    ITgClient::Chat get_chat_info(const std::string& chat_id);

    bool chat_exists(const std::string& chat_id);

    void clear_cache();
    
    const std::string& get_last_error() const;
    
private:
    std::string last_error_;
    
    void handle_error(const std::string& error);
    void clear_error();
    void update_cache(const std::vector<ITgClient::Chat>& chats);
    bool should_refresh_cache() const;
};