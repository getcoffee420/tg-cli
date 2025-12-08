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
    
    // Кэшированные чаты (опционально)
    std::vector<ITgClient::Chat> cached_chats_;
    std::map<std::string, std::string> chat_titles_;  // chat_id -> title
    time_t last_cache_update_;
    
public:
    explicit ChatsController(ITgClient& client);
    
    // Запрет копирования
    ChatsController(const ChatsController&) = delete;
    ChatsController& operator=(const ChatsController&) = delete;
    
    ~ChatsController() = default;

    // Получить список чатов
    std::vector<ITgClient::Chat> get_chats(int limit = 10);
    
    // Обновить кэш чатов
    void refresh_chats();
    
    // Поиск чатов по названию
    std::vector<ITgClient::Chat> search_chats(
        const std::string& query,
        int limit = 10
    );
    
    // Получить информацию о конкретном чате
    ITgClient::Chat get_chat_info(const std::string& chat_id);
    
    // Получить название чата по ID (из кэша)
    std::string get_chat_title(const std::string& chat_id);
    
    // Получить ID чата по названию (из кэша)
    std::string get_chat_id_by_title(const std::string& title);
    
    // === Вспомогательные методы ===
    
    // Проверить, существует ли чат
    bool chat_exists(const std::string& chat_id);
    
    // Получить количество чатов
    size_t get_chats_count() const;
    
    // Время последнего обновления
    time_t get_last_update_time() const;
    
    // Очистить кэш
    void clear_cache();
    
    const std::string& get_last_error() const;
    
private:
    std::string last_error_;
    
    // Внутренние методы
    void handle_error(const std::string& error);
    void clear_error();
    void update_cache(const std::vector<ITgClient::Chat>& chats);
    bool should_refresh_cache() const;
};