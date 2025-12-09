#pragma once
#include "../controllers/AuthController.h"
#include "../controllers/SendMessageController.h"
#include "../controllers/GetChatsController.h"
#include "../controllers/ChatHistoryController.h"

class TgClientFacade {
private:
    std::unique_ptr<AuthController> auth_controller_;
    //MessageController message_controller_;
    //ChatHistoryController history_controller_;
    //ChatsController chats_controller_;
    ITgClient& client_;

    static std::vector<std::string> collect_args(int argc, char** argv);
    static void print_usage();

    int handle_auth_status();
    int handle_login_phone(const std::string& phone);
    int handle_login_code(const std::string& code);
    int handle_logout();
    int handle_send(const std::string& chat_id, const std::string& message);


    
public:
    explicit TgClientFacade(ITgClient& client);
    ~TgClientFacade() = default;
    static std::string auth_state_to_string(ITgClient::AuthState state);

    int run(int argc, char** argv);
};