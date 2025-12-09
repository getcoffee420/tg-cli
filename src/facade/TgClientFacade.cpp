
#include "TgClientFacade.h"

#include <iostream>
#include <sstream>


TgClientFacade::TgClientFacade(ITgClient& client) :
    client_(client), 
    auth_controller_(client_),
    message_controller_(client_),
    history_controller_(client_),
    chats_controller_(client_) {}

std::vector<std::string> TgClientFacade::collect_args(int argc, char** argv) {
    std::vector<std::string> args;
    args.reserve(static_cast<std::size_t>(argc));
    for (int i = 0; i < argc; ++i) {
        args.emplace_back(argv[i]);
    }
    return args;
}

void TgClientFacade::print_usage() {
    std::cout
        << "Usage:\n"
        << "  tgcli send <chat_id> <message...>\n"
        << "  tgcli login\n"
        // << "  tgcli login-code <code>\n"
        // << "  tgcli chats\n"
        // << "  tgcli history <chat_id> [limit]\n"
        << std::endl;
}

int TgClientFacade::run(int argc, char** argv) {
    auto args = collect_args(argc, argv);
    if (args.size() < 2) {
        std::cerr << "[tgcli] No command specified\n";
        print_usage();
        return 1;
    }

    const std::string& command = args[1];

    try {
        if (command == "auth-status") {
            if (auth_controller_.is_authorized()) {
                std::cout << "You are authorized\n";
            } else {
                std::cout << "You are NOT authorized\n";
            }
        }
        if (command == "login-phone") {
            auth_controller_.enter_phone(args[2]); //CHECKERS
        }
        if (command == "login-code") {
            auth_controller_.enter_code(args[2]); //CHECKERS
        }
        if (command == "logout") {
            auth_controller_.logout();
        }
    } catch (const std::exception& e) {
        std::cerr << "[tgcli] Unhandled error: " << e.what() << "\n";
        return 1;
    }
}