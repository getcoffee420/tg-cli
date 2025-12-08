#include <string>
#include <vector>

class ITgClient {
protected:
    int is_authorized = 0;

public:
    struct Chat {
        std::string title;
        std::string chatId;
    };


    enum class AuthState {
        WaitingPhone,
        WaitingCode,
        Ready,
        LoggingOut,
        Error,
    };

    struct Message {
        std::string messageID;
        std::string text;
        std::string chatId;
        std::string sender;
    };

    ITgClient() = default;

    virtual ~ITgClient() = default;

    virtual AuthState check_status() = 0;

    virtual void enter_phone_number(std::string) = 0;

    virtual void enter_message_code(std::string) = 0;

    virtual void log_out() = 0;

    virtual std::vector<Chat> get_chats(int limit) = 0;

    virtual std::vector<Message> get_chat_history(std::string chatID, int limit) = 0;

    virtual void send_message(std::string chatID, std::string message) = 0;
};
