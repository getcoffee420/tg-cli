// controllers/chatscontroller.cpp
#include "GetChatsController.h"
#include <iostream>
#include <algorithm>
#include <ctime>
#include <cstring>
#include <stdexcept>

ChatsController::ChatsController(ITgClient& client)
    : client_(client)
    , last_cache_update_(0)
    , last_error_("") {}

std::vector<ITgClient::Chat> ChatsController::get_chats(int limit) {
    clear_error();
    
    if (limit <= 0) {
        limit = 10;
    }
    
    if (!cached_chats_.empty() && !should_refresh_cache()) {
        int return_count = std::min(static_cast<int>(cached_chats_.size()), limit);
        return std::vector<ITgClient::Chat>(
            cached_chats_.begin(),
            cached_chats_.begin() + return_count
        );
    }
    
    try {
        std::vector<ITgClient::Chat> chats = client_.get_chats(limit);
        
        update_cache(chats);
        
        std::cout << "[ChatsController] Retrieved " << chats.size() 
                  << " chats\n";
        
        return chats;
        
    } catch (const std::exception& e) {
        handle_error(std::string("Failed to get chats: ") + e.what());
        
        if (!cached_chats_.empty()) {
            std::cerr << "[ChatsController] Returning cached data due to error\n";
            int return_count = std::min(static_cast<int>(cached_chats_.size()), limit);
            return std::vector<ITgClient::Chat>(
                cached_chats_.begin(),
                cached_chats_.begin() + return_count
            );
        }
        
        return {};
    }
}

void ChatsController::refresh_chats() {
    clear_error();
    
    try {
        std::vector<ITgClient::Chat> chats = client_.get_chats(100);
        
        update_cache(chats);
        
        std::cout << "[ChatsController] Cache refreshed with " 
                  << chats.size() << " chats\n";
        
    } catch (const std::exception& e) {
        handle_error(std::string("Failed to refresh chats: ") + e.what());
    }
}



void ChatsController::update_cache(const std::vector<ITgClient::Chat>& chats) {
    cached_chats_ = chats;
    
    chat_titles_.clear();
    for (const auto& chat : chats) {
        chat_titles_[chat.chatId] = chat.title;
    }
    
    last_cache_update_ = std::time(nullptr);
}

bool ChatsController::should_refresh_cache() const {
    const int CACHE_TTL_SECONDS = 5 * 60;
    
    if (cached_chats_.empty()) {
        return true;
    }
    
    time_t now = std::time(nullptr);
    return (now - last_cache_update_) > CACHE_TTL_SECONDS;
}

std::vector<ITgClient::Chat> ChatsController::search_chats(
    const std::string& query,
    int limit) {
    
    clear_error();
    
    if (query.empty()) {
        return get_chats(limit);
    }
    
    if (cached_chats_.empty()) {
        refresh_chats();
    }
    
    std::vector<ITgClient::Chat> result;
    std::string query_lower = query;
    
    std::transform(query_lower.begin(), query_lower.end(), 
                   query_lower.begin(), ::tolower);
    
    for (const auto& chat : cached_chats_) {
        if (result.size() >= static_cast<size_t>(limit)) {
            break;
        }
        
        std::string title_lower = chat.title;
        std::transform(title_lower.begin(), title_lower.end(),
                      title_lower.begin(), ::tolower);
        
        if (title_lower.find(query_lower) != std::string::npos) {
            result.push_back(chat);
        }
    }
    
    std::cout << "[ChatsController] Found " << result.size() 
              << " chats matching '" << query << "'\n";
    
    return result;
}

ITgClient::Chat ChatsController::get_chat_info(const std::string& chat_id) {
    clear_error();
    
    if (chat_id.empty()) {
        handle_error("Chat ID cannot be empty");
        return ITgClient::Chat{"", ""};
    }
    
    for (const auto& chat : cached_chats_) {
        if (chat.chatId == chat_id) {
            return chat;
        }
    }
    
    refresh_chats();
    
    for (const auto& chat : cached_chats_) {
        if (chat.chatId == chat_id) {
            return chat;
        }
    }
    
    handle_error("Chat not found: " + chat_id);
    return ITgClient::Chat{"", ""};
}

bool ChatsController::chat_exists(const std::string& chat_id) {
    if (chat_id.empty()) {
        return false;
    }
    
    for (const auto& chat : cached_chats_) {
        if (chat.chatId == chat_id) {
            return true;
        }
    }
    
    return false;
}

void ChatsController::clear_cache() {
    cached_chats_.clear();
    chat_titles_.clear();
    last_cache_update_ = 0;
    std::cout << "[ChatsController] Cache cleared\n";
}

const std::string& ChatsController::get_last_error() const {
    return last_error_;
}


void ChatsController::handle_error(const std::string& error) {
    last_error_ = error;
    std::cerr << "[ChatsController] Error: " << error << "\n";
}

void ChatsController::clear_error() {
    last_error_.clear();
}
