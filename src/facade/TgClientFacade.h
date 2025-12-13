#pragma once
#include "../controllers/AuthController.h"
#include "../controllers/SendMessageController.h"
#include "../controllers/GetChatsController.h"
#include "../controllers/ChatHistoryController.h"
#include "../controllers/LabelsController.h"
#include "../label/Label.h"
#include "../label/LabelsParser.h"
#include <memory>

class TgClientFacade {
private:
    std::unique_ptr<AuthController> auth_controller_;
    std::unique_ptr<MessageController> message_controller_;
    std::unique_ptr<ChatHistoryController> history_controller_;
    std::unique_ptr<ChatsController> chats_controller_;
    std::unique_ptr<LabelsController> label_controller_;
    ITgClient& client_;

    static std::vector<std::string> collect_args(int argc, char** argv);
    static void print_usage();

    int handle_auth_status();
    int handle_login_phone(const std::string& phone);
    int handle_login_code(const std::string& code);
    int handle_logout();

    int handle_send(const std::string& chat_id, const std::string& message);

    int handle_get_chats(int limit);
    int handle_search_chats(const std::string& query);
    int handle_chat_info(const std::string& chat_id);

    int handle_history(const std::string& chat_id, int limit);
    int handle_set_target_chat(const std::string& chat_id);
    int handle_get_target_history(int limit);

    std::map<std::string, std::string> labels;

public:
    explicit TgClientFacade(ITgClient& client);
    ~TgClientFacade() = default;
    static std::string auth_state_to_string(ITgClient::AuthState state);

    int run(int argc, char** argv);
};