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

AuthController::~AuthController() = default;

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

void AuthController::enter_phone(const std::string& phone_number) {
    update_status();
    
    if (current_state_ != ITgClient::AuthState::WaitingPhone) {
        throw std::logic_error("[AuthController] Cannot enter phone in current state");
    }

    if (phone_number.empty()) {
        throw std::invalid_argument("Phone number is required");
    }
            
    try {
        std::cout << "[AuthController] Sending phone number...\n";
        client_.enter_phone_number(phone_number);
        update_status();
    } catch (const std::exception& e) {
        std::cerr << "[AuthController] enter_phone error: " << e.what() << "\n";
        current_state_ = ITgClient::AuthState::Error;
        throw;
    }
}

void AuthController::enter_code(const std::string& code) {
    update_status();

    if (current_state_ != ITgClient::AuthState::WaitingCode) {
        throw std::logic_error("[AuthController] Cannot enter code in current state");
    }

    if (code.empty()) {
        throw std::invalid_argument("Auth code is required");
    }

    try {
        std::cout << "[AuthController] Sending auth code...\n";
        client_.enter_message_code(code);
        update_status();
    } catch (const std::exception& e) {
        std::cerr << "[AuthController] enter_code error: " << e.what() << "\n";
        current_state_ = ITgClient::AuthState::Error;
        throw;
    }
}

void AuthController::logout() {
    update_status();
    try {
        if (current_state_ == ITgClient::AuthState::LoggingOut) {
            std::cout << "[AuthController] Already logging out\n";
            return;
        }
        if (!is_authorized()) {
            std::cout << "[AuthController] Not authorized, nothing to logout\n";
            return;
        }

        client_.log_out();
        update_status();
    }  catch (const std::exception& e) {
        std::cerr << "[AuthController] Logout error: " << e.what() << "\n";
        current_state_ = ITgClient::AuthState::Error;
        return;
    }
}