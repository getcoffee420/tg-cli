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

// Получить список чатов
std::vector<ITgClient::Chat> ChatsController::get_chats(int limit) {
    clear_error();
    
    if (limit <= 0) {
        limit = 10;  // значение по умолчанию
    }
    
    // Можно использовать кэш, если он актуален
    if (!cached_chats_.empty() && !should_refresh_cache()) {
        // Возвращаем ограниченное количество из кэша
        int return_count = std::min(static_cast<int>(cached_chats_.size()), limit);
        return std::vector<ITgClient::Chat>(
            cached_chats_.begin(),
            cached_chats_.begin() + return_count
        );
    }
    
    // Получаем свежие данные
    try {
        std::vector<ITgClient::Chat> chats = client_.get_chats(limit);
        
        // Обновляем кэш
        update_cache(chats);
        
        std::cout << "[ChatsController] Retrieved " << chats.size() 
                  << " chats\n";
        
        return chats;
        
    } catch (const std::exception& e) {
        handle_error(std::string("Failed to get chats: ") + e.what());
        
        // Если есть кэш, вернем его даже если устарел
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

// Обновить кэш чатов
void ChatsController::refresh_chats() {
    clear_error();
    
    try {
        std::vector<ITgClient::Chat> chats = client_.get_chats(100);  // больше для кэша
        
        update_cache(chats);
        
        std::cout << "[ChatsController] Cache refreshed with " 
                  << chats.size() << " chats\n";
        
    } catch (const std::exception& e) {
        handle_error(std::string("Failed to refresh chats: ") + e.what());
    }
}



void ChatsController::update_cache(const std::vector<ITgClient::Chat>& chats) {
    cached_chats_ = chats;
    
    // Обновляем кэш заголовков
    chat_titles_.clear();
    for (const auto& chat : chats) {
        chat_titles_[chat.chatId] = chat.title;
    }
    
    last_cache_update_ = std::time(nullptr);
}

bool ChatsController::should_refresh_cache() const {
    // Обновляем кэш каждые 5 минут
    const int CACHE_TTL_SECONDS = 5 * 60;
    
    if (cached_chats_.empty()) {
        return true;
    }
    
    time_t now = std::time(nullptr);
    return (now - last_cache_update_) > CACHE_TTL_SECONDS;
}

// Поиск чатов по названию
std::vector<ITgClient::Chat> ChatsController::search_chats(
    const std::string& query,
    int limit) {
    
    clear_error();
    
    if (query.empty()) {
        // Если запрос пустой, возвращаем все чаты
        return get_chats(limit);
    }
    
    // Используем кэш для поиска
    if (cached_chats_.empty()) {
        refresh_chats();
    }
    
    std::vector<ITgClient::Chat> result;
    std::string query_lower = query;
    
    // Приводим к нижнему регистру для регистронезависимого поиска
    std::transform(query_lower.begin(), query_lower.end(), 
                   query_lower.begin(), ::tolower);
    
    for (const auto& chat : cached_chats_) {
        if (result.size() >= static_cast<size_t>(limit)) {
            break;
        }
        
        std::string title_lower = chat.title;
        std::transform(title_lower.begin(), title_lower.end(),
                      title_lower.begin(), ::tolower);
        
        // Ищем вхождение подстроки
        if (title_lower.find(query_lower) != std::string::npos) {
            result.push_back(chat);
        }
    }
    
    std::cout << "[ChatsController] Found " << result.size() 
              << " chats matching '" << query << "'\n";
    
    return result;
}

// Получить информацию о конкретном чате
ITgClient::Chat ChatsController::get_chat_info(const std::string& chat_id) {
    clear_error();
    
    if (chat_id.empty()) {
        handle_error("Chat ID cannot be empty");
        return ITgClient::Chat{"", ""};
    }
    
    // Сначала проверяем кэш
    for (const auto& chat : cached_chats_) {
        if (chat.chatId == chat_id) {
            return chat;
        }
    }
    
    // Если нет в кэше, обновляем кэш
    refresh_chats();
    
    // Ищем снова
    for (const auto& chat : cached_chats_) {
        if (chat.chatId == chat_id) {
            return chat;
        }
    }
    
    handle_error("Chat not found: " + chat_id);
    return ITgClient::Chat{"", ""};
}

// Получить название чата по ID
std::string ChatsController::get_chat_title(const std::string& chat_id) {
    if (chat_id.empty()) {
        return "";
    }
    
    // Сначала проверяем кэш заголовков
    auto it = chat_titles_.find(chat_id);
    if (it != chat_titles_.end()) {
        return it->second;
    }
    
    // Если нет в кэше, ищем в основном кэше
    for (const auto& chat : cached_chats_) {
        if (chat.chatId == chat_id) {
            chat_titles_[chat_id] = chat.title;
            return chat.title;
        }
    }
    
    // Если все еще не нашли, обновляем кэш
    refresh_chats();
    
    for (const auto& chat : cached_chats_) {
        if (chat.chatId == chat_id) {
            chat_titles_[chat_id] = chat.title;
            return chat.title;
        }
    }
    
    return "Unknown chat";
}

// Получить ID чата по названию
std::string ChatsController::get_chat_id_by_title(const std::string& title) {
    if (title.empty()) {
        return "";
    }
    
    for (const auto& chat : cached_chats_) {
        if (chat.title == title) {
            return chat.chatId;
        }
    }
    
    // Попробуем обновить кэш
    refresh_chats();
    
    for (const auto& chat : cached_chats_) {
        if (chat.title == title) {
            return chat.chatId;
        }
    }
    
    return "";
}

// Проверить существование чата
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

// Получить количество чатов
size_t ChatsController::get_chats_count() const {
    return cached_chats_.size();
}

// Время последнего обновления
time_t ChatsController::get_last_update_time() const {
    return last_cache_update_;
}

// Очистить кэш
void ChatsController::clear_cache() {
    cached_chats_.clear();
    chat_titles_.clear();
    last_cache_update_ = 0;
    std::cout << "[ChatsController] Cache cleared\n";
}

// Получить последнюю ошибку
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
