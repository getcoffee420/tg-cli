
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
    
    //AuthController(const AuthController&) = default
    //AuthController& operator=(const AuthController&) = default;

    ~AuthController();

    void enter_phone(const std::string& phone_number);
    void enter_code(const std::string& code);

    void logout();
    bool is_authorized() const;
    ITgClient::AuthState get_auth_state() const;

    void update_status();
};