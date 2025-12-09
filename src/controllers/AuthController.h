
// controllers/authcontroller.h
#pragma once
#include <string>
#include <functional>
#include "../tgClient/ITgClient.hpp"

class AuthController {
private:
    ITgClient& client_;
    ITgClient::AuthState current_state_;

    // WaitingPhone,
    // WaitingCode,
    // Ready,
    // LoggingOut,
    // Error,
    
public:
    explicit AuthController(ITgClient& client);
    
    // Запрет копирования
    AuthController(const AuthController&) = delete;
    AuthController& operator=(const AuthController&) = delete;

    ~AuthController() = default;

    // Основные методы
    bool login(const std::string& phone_number, const std::string& code = "");
    void logout();
    bool is_authorized() const;
    ITgClient::AuthState get_auth_state() const;

    void update_status();
};