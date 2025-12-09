#pragma once
#include "../controllers/AuthController.h"
#include "../controllers/SendMessageController.h"
#include "../controllers/GetChatsController.h"
#include "../controllers/ChatHistoryController.h"

class TgClientFacade {
private:
    AuthController auth_controller_;
    MessageController message_controller_;
    ChatHistoryController history_controller_;
    ChatsController chats_controller_;
    ITgClient& client_;

    static std::vector<std::string> collect_args(int argc, char** argv);
    static void print_usage();
    
public:
    explicit TgClientFacade(ITgClient& client);
    ~TgClientFacade() = default;

    int run(int argc, char** argv);
};