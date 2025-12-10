#include "../src/tgClient/ITgClient.hpp"
#include "../src/tgClient/TgClientTdlib.hpp"
#include <iostream>

int main() {
    TgClientTdlib a = TgClientTdlib();
    auto temp = a.check_status();

    std::cout << "waitingPhone " << (temp == TgClientTdlib::AuthState::WaitingPhone) << std::endl;
    std::cout << "WaitingCode " << (temp == TgClientTdlib::AuthState::WaitingCode) << std::endl;
    std::cout << "WaitingPassword " << (temp == TgClientTdlib::AuthState::WaitingPassword) << std::endl;
    std::cout << "WaitingParameters " << (temp == TgClientTdlib::AuthState::WaitingTdlibParameters) << std::endl;
    std::cout << "Ready " << (temp == TgClientTdlib::AuthState::Ready) << std::endl;
    std::cout << "LogginOut " << (temp == TgClientTdlib::AuthState::LoggingOut) << std::endl;
    std::cout << "error " << (temp == TgClientTdlib::AuthState::Error) << std::endl;
    enum class AuthState {
        WaitingPhone,
        WaitingCode,
        WaitingPassword,
        WaitingTdlibParameters,
        Ready,
        LoggingOut,
        Error,
    };
}
