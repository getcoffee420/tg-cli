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
    current_state_ = client_.check_status();
}

ITgClient::AuthState AuthController::get_auth_state() const{
    return current_state_;
}

