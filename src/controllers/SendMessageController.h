// controllers/messagecontroller.h
#pragma once
#include <string>
#include <vector>
#include <functional>
#include "../tgClient/ITgClient.hpp"

class MessageController {
private:
    ITgClient& client_;
    
    // Опционально: коллбэк на новые сообщения
    std::function<void(const ITgClient::Message&)> on_new_message_;
    std::function<void(const std::string&)> on_send_error_;
    
public:
    explicit MessageController(ITgClient& client);
    
    // Запрет копирования
    MessageController(const MessageController&) = delete;
    MessageController& operator=(const MessageController&) = delete;
    
    ~MessageController() = default;
    
    // Отправить сообщение в чат
    bool send_message(const std::string& chat_id, const std::string& text);
    
    const std::string& get_last_error() const;
    
private:
    std::string last_error_;
    
    // Внутренние методы
    void handle_error(const std::string& error);
    void clear_error();
};