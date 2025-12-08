// controllers/authcontroller.cpp
#include "AuthController.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

// Конструктор
AuthController::AuthController(ITgClient& client)
    : client_(client)
    , current_state_(ITgClient::AuthState::Error) {
    
    update_status();
}

void AuthController::update_status() {
    try {
        current_state_ = client_.check_status();
    } catch (const std::exception& e) {
        std::cerr << "[AuthController] Failed to check status: " << e.what() << "\n";
        current_state_ = ITgClient::AuthState::Error;
    }
}

ITgClient::AuthState AuthController::get_auth_state() const {
    return current_state_;
}

bool AuthController::is_authorized() const {
    return current_state_ == ITgClient::AuthState::Ready;
}

bool AuthController::login(const std::string& phone_number, const std::string& code) {
    
    // 1. Проверяем текущее состояние
    update_status();
    
    // 2. Если уже авторизованы - ничего не делаем
    if (is_authorized()) {
        std::cerr << "[AuthController] Already authorized\n";
        return true;
    }
    
    // 3. Обрабатываем в зависимости от состояния
    try {
        switch (current_state_) {
            case ITgClient::AuthState::WaitingPhone:
                // Требуется номер телефона
                if (phone_number.empty()) {
                    throw std::invalid_argument("Phone number is required");
                }
                
                // if (!validatePhoneNumber(phone_number)) {
                //     throw std::invalid_argument("Invalid phone number format");
                // }
                
                std::cout << "[AuthController] Sending phone number...\n";
                client_.enter_phone_number(phone_number);
                break;
                
            case ITgClient::AuthState::WaitingCode:
                // Требуется код подтверждения
                if (code.empty()) {
                    throw std::invalid_argument("Auth code is required");
                }
                
                // if (!validateCode(code)) {
                //     throw std::invalid_argument("Invalid code format");
                // }
                
                std::cout << "[AuthController] Sending auth code...\n";
                client_.enter_message_code(code);
                break;
                
            case ITgClient::AuthState::Ready:
                return true;
                
            case ITgClient::AuthState::LoggingOut:
                throw std::runtime_error("Cannot login while logging out");
                
            case ITgClient::AuthState::Error:
                throw std::runtime_error("In error state, cannot login");
                
            default:
                throw std::runtime_error("Unknown auth state");
        }
        
        // 4. Обновляем состояние после отправки
        update_status();
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[AuthController] Login error: " << e.what() << "\n";
        current_state_ = ITgClient::AuthState::Error;
        return false;
    }
}

void AuthController::logout() {
    update_status();
    try {
        if (!is_authorized() && get_auth_state() != ITgClient::AuthState::LoggingOut) {
            std::cout << "[AuthController] Not authorized, nothing to logout\n";
            return;
        }
        client_.log_out();
        update_status();
    }  catch (const std::exception& e) {
        std::cerr << "[AuthController] Login error: " << e.what() << "\n";
        current_state_ = ITgClient::AuthState::Error;
        return;
    }
}