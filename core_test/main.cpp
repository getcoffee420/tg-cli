#include "../src/tgClient/ITgClient.hpp"
#include "../src/tgClient/TgClientTdlib.hpp"
#include "../src/controllers/GetChatsController.h"
#include <iostream>
#include <sstream>

static std::string auth_state_to_string(ITgClient::AuthState state) {
    using S = ITgClient::AuthState;
    switch (state) {
        case S::WaitingPhone:            return "WaitingPhone";
        case S::WaitingCode:             return "WaitingCode";
        case S::WaitingPassword:         return "WaitingPassword";
        case S::WaitingTdlibParameters:  return "WaitingTdlibParameters";
        case S::Ready:                   return "Ready";
        case S::LoggingOut:              return "LoggingOut";
        case S::Error:                   return "Error";
        default:                         return "Unknown";
    }
}

static void print_help() {
    std::cout << "Команды:\n"
              << "  help                       - показать это сообщение\n"
              << "  status                     - показать статус авторизации\n"
              << "  send_params                - отправить TDLib параметры (если надо)\n"
              << "  phone <number>             - ввести номер телефона\n"
              << "  code <code>                - ввести код из Telegram/SMS\n"
              << "  chats [limit]              - вывести список чатов\n"
              << "  history <chatId> [limit]   - вывести историю чата\n"
              << "  send <chatId> <text...>    - отправить сообщение в чат\n"
              << "  logout                     - разлогиниться\n"
              << "  exit / quit                - выйти\n";
}

int main() {
    TgClientTdlib client;

    std::cout << "Примитивный tg-cli. Пиши 'help' для списка команд.\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) {
            break; // EOF / ctrl+d
        }

        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "help") {
            print_help();
        } else if (cmd == "exit" || cmd == "quit") {
            std::cout << "Пока.\n";
            break;
        } else if (cmd == "status") {
            auto st = client.check_status();
            std::cout << "Auth state: " << auth_state_to_string(st) << "\n";
        } else if (cmd == "send_params") {
            client.send_tdlib_parameters();
            std::cout << "TDLib параметры отправлены (если TDLib была в нужном состоянии).\n";
        } else if (cmd == "phone") {
            std::string phone;
            iss >> phone;
            if (phone.empty()) {
                std::cout << "Использование: phone <number>\n";
                continue;
            }
            client.enter_phone_number(phone);
            std::cout << "Номер отправлен.\n";
        } else if (cmd == "code") {
            std::string code;
            iss >> code;
            if (code.empty()) {
                std::cout << "Использование: code <code>\n";
                continue;
            }
            client.enter_message_code(code);
            std::cout << "Код отправлен.\n";
        } else if (cmd == "chats") {
            int limit = 20;
            if (!(iss >> limit)) {
                limit = 20;
            }

            auto chats = client.get_chats(limit);
            std::cout << "Чаты (" << chats.size() << "):\n";
            for (const auto &c : chats) {
                std::cout << "  [" << c.chatId << "] " << c.title << "\n";
            }
        } else if (cmd == "history") {
            std::string chatId;
            iss >> chatId;
            if (chatId.empty()) {
                std::cout << "Использование: history <chatId> [limit]\n";
                continue;
            }

            int limit = 20;
            if (!(iss >> limit)) {
                limit = 20;
            }

            auto msgs = client.get_chat_history(chatId, limit);
            std::cout << "Сообщения (" << msgs.size() << ") в чате " << chatId << ":\n";
            for (const auto &m : msgs) {
                std::cout << "  [" << m.messageID << "] "
                          << m.sender << ": "
                          << m.text << "\n";
            }
        } else if (cmd == "send") {
            std::string chatId;
            iss >> chatId;
            if (chatId.empty()) {
                std::cout << "Использование: send <chatId> <text...>\n";
                continue;
            }

            std::string text;
            std::getline(iss, text);
            if (!text.empty() && text[0] == ' ') {
                text.erase(0, 1);
            }

            if (text.empty()) {
                std::cout << "Пустое сообщение, не отправляю.\n";
                continue;
            }

            client.send_message(chatId, text);
            std::cout << "Сообщение отправлено.\n";
        } else if (cmd == "logout") {
            client.log_out();
            std::cout << "Запрошен logout.\n";
        } else {
            std::cout << "Неизвестная команда: " << cmd << "\n";
            std::cout << "Пиши 'help' чтобы увидеть доступные команды.\n";
        }
    }

    return 0;
}