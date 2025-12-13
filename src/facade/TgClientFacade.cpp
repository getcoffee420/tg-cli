#include "TgClientFacade.h"

#include <memory>
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <iomanip>

TgClientFacade::TgClientFacade(ITgClient& client) : client_(client) {
    client_.send_tdlib_parameters();
    while (client_.check_status() == ITgClient::AuthState::Error) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    auth_controller_ = std::make_unique<AuthController>(client_);
    chats_controller_ = std::make_unique<ChatsController>(client_);
    history_controller_ = std::make_unique<ChatHistoryController>(client_);
    message_controller_ = std::make_unique<MessageController>(client_);
    label_controller_ = std::make_unique<LabelController>("labels.txt");
}

std::string TgClientFacade::auth_state_to_string(ITgClient::AuthState state) {
    using AuthState = ITgClient::AuthState;
    switch (state) {
        case AuthState::WaitingPhone:
            return "WaitingPhone";
        case AuthState::WaitingCode:
            return "WaitingCode";
        case AuthState::Ready:
            return "Ready";
        case AuthState::LoggingOut:
            return "LoggingOut";
        case AuthState::Error:
        default:
            return "Error";
    }
}

void TgClientFacade::print_usage() {
    std::cout
        << "Usage:\n"
        << "  tgcli auth-status\n"
        << "  tgcli login-phone <phone>\n"
        << "  tgcli login-code <code>\n"
        << "  tgcli logout\n"
        << "  tgcli chats\n"
        << "  tgcli search-chats <query>\n"
        << "  tgcli chat-info <chat_label_or_id>\n"
        << "  tgcli history <chat_label_or_id> [limit]\n"
        << "  tgcli send <chat_label_or_id> <message...>\n"
        << std::endl;
}

int TgClientFacade::run(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "[tgcli] No command specified\n";
        print_usage();
        return 1;
    }

    client_.check_status();

    const std::string command = argv[1];

    try {
        if (command == "auth-status") {
            return handle_auth_status();
        } else if (command == "login-phone") {
            if (argc < 3) {
                std::cerr << "[tgcli] login-phone: phone is required\n";
                return 1;
            }
            client_.enter_phone_number(argv[2]);
            return 0;
        } else if (command == "login-code") {
            if (argc < 3) {
                std::cerr << "[tgcli] login-code: code is required\n";
                return 1;
            }
            return handle_login_code(argv[2]);
        } else if (command == "logout") {
            return handle_logout();
        } else if (command == "chats") {
            int limit = 20;
            try {
                if (argc < 3) {
                    limit = std::stoi(argv[2]);
                } 
            } catch (...) {
                std::cerr << "[tgcli] Invalid limit, using default 20\n";
            }
            return handle_get_chats(limit); 
        } else if (command == "search-chats") {
            if (argc < 3) {
                std::cerr << "[tgcli] search-chats: query is required\n";
                return 1;
            }
            return handle_search_chats(argv[2]);
        } else if (command == "chat-info") {
            if (argc < 3) {
                std::cerr << "[tgcli] chat-info: chat_id is required\n";
                return 1;
            }
            return handle_chat_info(argv[2]);
        } else if (command == "history") {
            if (argc < 3) {
                std::cerr << "[tgcli] history: chat_id is required\n";
                return 1;
            }
            int limit = 20;
            if (argc >= 4) {
                try {
                    limit = std::stoi(argv[3]);
                } catch (...) {
                    std::cerr << "[tgcli] Invalid limit, using default 20\n";
                }
            }
            auto x = client_.get_chat_history(argv[2], limit);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // you saw nothing...

            return handle_history(argv[2], limit);

        } else if (command == "send") {
            if (argc < 4) {
                std::cerr << "[tgcli] send: not enough arguments\n";
                std::cerr << "Usage: tgcli send <chat_id> <message...>\n";
                return 1;
            }

            const std::string chat_id = argv[2];

            std::ostringstream oss;
            for (int i = 3; i < argc; ++i) {
                if (i > 3) {
                    oss << ' ';
                }
                oss << argv[i];
            }
            const std::string message = oss.str();

            return handle_send(chat_id, message);
        } else {
            std::cerr << "[tgcli] Unknown command: " << command << "\n";
            print_usage();
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] Unhandled error: " << e.what() << "\n";
        return 1;
    }
}

// =====================
//   AUTH команды
// =====================

int TgClientFacade::handle_auth_status() {
    auto state = auth_controller_->get_auth_state();
    bool authorized = auth_controller_->is_authorized();

    std::cout << "[tgcli] Auth state: " << auth_state_to_string(state)
              << " (authorized=" << (authorized ? "true" : "false") << ")\n";

    return 0;
}

int TgClientFacade::handle_login_phone(const std::string& phone) {
    try {
        auth_controller_->enter_phone(phone);
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] login-phone error: " << e.what() << "\n";
        return 1;
    }

    auto state = auth_controller_->get_auth_state();
    std::cout << "[tgcli] login-phone: new state = "
              << auth_state_to_string(state) << "\n";

    return 0;
}

int TgClientFacade::handle_login_code(const std::string& code) {
    try {
        auth_controller_->enter_code(code);
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] login-code error: " << e.what() << "\n";
        return 1;
    }

    auto state = auth_controller_->get_auth_state();
    std::cout << "[tgcli] login-code: new state = "
              << auth_state_to_string(state) << "\n";

    if (auth_controller_->is_authorized()) {
        std::cout << "[tgcli] Authorization completed\n";
    }

    return 0;
}

int TgClientFacade::handle_logout() {
    auth_controller_->logout();

    auto state = auth_controller_->get_auth_state();
    std::cout << "[tgcli] logout: new state = "
              << auth_state_to_string(state) << "\n";

    return 0;
}

// =====================
//   CHATS команды
// =====================

int TgClientFacade::handle_get_chats(int limit = 20) {
    if (!auth_controller_->is_authorized()) {
        std::cerr << "[tgcli] Not authorized. Use login-phone/login-code first.\n";
        return 1;
    }

    try {
        auto chats = chats_controller_->get_chats(limit);

        if (label_controller_) {
            label_controller_->rebuild_from_chats(chats);
        }

        if (chats.empty()) {
            std::cout << "[tgcli] No chats found\n";
            return 0;
        }
        
        std::cout << "[tgcli] Found " << chats.size() << " chats:\n";
        std::cout << "========================================\n";
        
        for (size_t i = 0; i < chats.size(); ++i) {
            const auto& chat = chats[i];
            const std::string label = label_controller_ ? label_controller_->label_for_chat_id(chat.chatId)
                                                        : std::string{};

            std::cout << std::setw(3) << (i + 1) << ". "
                      << "Label: " << (label.empty() ? "-" : label)
                      << " | Title: " << chat.title
                      << " | ID: " << chat.chatId
                      << "\n";
        }
        
        std::cout << "========================================\n";
        
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] get-chats error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

int TgClientFacade::handle_search_chats(const std::string& query) {
    if (!auth_controller_->is_authorized()) {
        std::cerr << "[tgcli] Not authorized. Use login-phone/login-code first.\n";
        return 1;
    }

    try {
        auto chats = chats_controller_->search_chats(query, 50);
        
        if (chats.empty()) {
            std::cout << "[tgcli] No chats found for query: " << query << "\n";
            return 0;
        }
        
        std::cout << "[tgcli] Found " << chats.size() 
                  << " chats matching '" << query << "':\n";
        std::cout << "========================================\n";
        
        for (size_t i = 0; i < chats.size(); ++i) {
            const auto& chat = chats[i];

            std::string label;
            if (label_controller_) {
                label = label_controller_->label_for_chat_id(chat.chatId);
            }

            std::cout << std::setw(3) << (i + 1) << ". "
                      << "Label: " << (label.empty() ? "-" : label)
                      << " | Title: " << chat.title
                      << " | ID: " << chat.chatId << "\n";
        }
        
        std::cout << "========================================\n";
        
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] search-chats error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

int TgClientFacade::handle_chat_info(const std::string& chat_id) {
    if (!auth_controller_->is_authorized()) {
        std::cerr << "[tgcli] Not authorized. Use login-phone/login-code first.\n";
        return 1;
    }

    try {
        const std::string resolved_id = label_controller_ ? label_controller_->resolve_chat_id(chat_id)
                                                          : chat_id;
        auto chat = chats_controller_->get_chat_info(resolved_id);
        
        if (chat.chatId.empty()) {
            std::cerr << "[tgcli] Chat not found: " << chat_id << "\n";
            return 1;
        }
        
        std::cout << "[tgcli] Chat info:\n";
        std::cout << "  Label: "
                  << (label_controller_ ? label_controller_->label_for_chat_id(chat.chatId) : std::string{"-"})
                  << "\n";
        std::cout << "  ID: " << chat.chatId << "\n";
        std::cout << "  Title: " << chat.title << "\n";
        
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] chat-info error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

// =====================
//   HISTORY команды
// =====================

int TgClientFacade::handle_history(const std::string& chat_id, int limit) {
    if (!auth_controller_->is_authorized()) {
        std::cerr << "[tgcli] Not authorized. Use login-phone/login-code first.\n";
        return 1;
    }

    try {
        const std::string resolved_id = label_controller_ ? label_controller_->resolve_chat_id(chat_id)
                                                          : chat_id;
        auto messages = history_controller_->get_chat_history(resolved_id, limit);
        
        if (messages.empty()) {
            std::cout << "[tgcli] No messages found in chat: " << chat_id << "\n";
            return 0;
        }
        
        std::cout << "[tgcli] History for chat " << chat_id 
                  << " (" << messages.size() << " messages):\n";
        std::cout << "========================================\n";
        
        for (const auto& msg : messages) {
            std::cout << "[" << chats_controller_->get_chat_title(msg.sender) << "] " << "(" << msg.messageID << "): " << msg.text << "\n";
        }
        
        std::cout << "========================================\n";
        
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] history error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

// =====================
//   SEND команда 
// =====================

int TgClientFacade::handle_send(const std::string& chat_id,
                                const std::string& message) {
    if (!auth_controller_->is_authorized()) {
        std::cerr << "[tgcli] Not authorized. Use login-phone/login-code first.\n";
        return 1;
    }

    try {
        const std::string resolved_id = label_controller_ ? label_controller_->resolve_chat_id(chat_id)
                                                          : chat_id;
        bool success = message_controller_->send_message(resolved_id, message);
        
        if (success) {
            std::cout << "[tgcli] Message sent to chat " << chat_id << "\n";
            return 0;
        } else {
            std::cerr << "[tgcli] Failed to send message: " 
                      << message_controller_->get_last_error() << "\n";
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] send error: " << e.what() << "\n";
        return 1;
    }
}