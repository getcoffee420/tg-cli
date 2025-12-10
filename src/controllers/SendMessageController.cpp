// controllers/messagecontroller.cpp
#include "SendMessageController.h"
#include <iostream>
#include <stdexcept>

MessageController::MessageController(ITgClient& client)
    : client_(client)
    , last_error_("") {}

// Отправить сообщение
bool MessageController::send_message(const std::string& chat_id, 
                                    const std::string& text) {
    clear_error();
    
    // 1. Валидация
    if (chat_id.empty()) {
        handle_error("Chat ID cannot be empty");
        return false;
    }
    
    // 2. Отправка
    try {
        client_.send_message(chat_id, text);
        std::cout << "[MessageController] Message sent to chat " << chat_id << "\n";
        return true;
        
    } catch (const std::exception& e) {
        handle_error(std::string("Failed to send message: ") + e.what());
        return false;
    }
}

// Получить последнюю ошибку
const std::string& MessageController::get_last_error() const {
    return last_error_;
}

void MessageController::handle_error(const std::string& error) {
    last_error_ = error;
    std::cerr << "[MessageController] Error: " << error << "\n";
    
    if (on_send_error_) {
        on_send_error_(error);
    }
}

void MessageController::clear_error() {
    last_error_.clear();
}