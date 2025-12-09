#include <gtest/gtest.h>

#include "../src/controllers/AuthController.h"
#include "../src/tgClient/ITgClient.hpp"

#include <stdexcept>
#include <string>
#include <vector>

// Фейковый клиент для тестов
class FakeTgClient : public ITgClient {
public:
    AuthState state = AuthState::WaitingPhone;

    int  check_status_calls       = 0;
    bool throw_in_check_status    = false;

    bool enter_phone_called       = false;
    std::string last_phone;
    bool throw_in_enter_phone     = false;

    bool enter_code_called        = false;
    std::string last_code;
    bool throw_in_enter_code      = false;

    bool log_out_called           = false;
    bool throw_in_log_out         = false;

    // ITgClient interface

    AuthState check_status() override {
        ++check_status_calls;
        if (throw_in_check_status) {
            throw std::runtime_error("check_status failed");
        }
        return state;
    }

    void enter_phone_number(std::string phone) override {
        if (throw_in_enter_phone) {
            throw std::runtime_error("enter_phone_number failed");
        }
        enter_phone_called = true;
        last_phone = std::move(phone);
        // Эмулируем переход в ожидание кода
        state = AuthState::WaitingCode;
    }

    void enter_message_code(std::string code) override {
        if (throw_in_enter_code) {
            throw std::runtime_error("enter_message_code failed");
        }
        enter_code_called = true;
        last_code = std::move(code);
        // Эмулируем успешную авторизацию
        state = AuthState::Ready;
    }

    void log_out() override {
        log_out_called = true;
        if (throw_in_log_out) {
            throw std::runtime_error("log_out failed");
        }
        // Эмулируем выход
        state = AuthState::LoggingOut;
    }

    std::vector<Chat> get_chats(int) override {
        return {};
    }

    std::vector<Message> get_chat_history(std::string, int) override {
        return {};
    }

    void send_message(std::string, std::string) override {
        // не нужно для AuthController
    }
};

// ---------------- ТЕСТЫ ----------------

using AuthState = ITgClient::AuthState;

// Конструктор: подтягивает состояние из клиента
TEST(AuthControllerTests, ConstructorInitializesStateFromClient) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingPhone;

    AuthController controller(fake);

    EXPECT_EQ(fake.check_status_calls, 1);
    EXPECT_EQ(controller.get_auth_state(), AuthState::WaitingPhone);
    EXPECT_FALSE(controller.is_authorized());
}

// Если check_status кидает — состояние Error
TEST(AuthControllerTests, UpdateStatusOnExceptionSetsError) {
    FakeTgClient fake;
    fake.throw_in_check_status = true;

    AuthController controller(fake);

    EXPECT_EQ(controller.get_auth_state(), AuthState::Error);
    EXPECT_FALSE(controller.is_authorized());
}

// enter_phone: нормальный поток
TEST(AuthControllerTests, EnterPhoneInWaitingPhoneSendsPhoneAndMovesToWaitingCode) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingPhone;

    AuthController controller(fake);
    ASSERT_EQ(controller.get_auth_state(), AuthState::WaitingPhone);

    controller.enter_phone("+71234567890");

    EXPECT_TRUE(fake.enter_phone_called);
    EXPECT_EQ(fake.last_phone, "+71234567890");
    // FakeTgClient перевёл state в WaitingCode, update_status это подтянул
    EXPECT_EQ(controller.get_auth_state(), AuthState::WaitingCode);
}

// enter_phone: пустой номер -> invalid_argument, состояние не меняется
TEST(AuthControllerTests, EnterPhoneEmptyNumberThrowsInvalidArgumentAndKeepsState) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingPhone;

    AuthController controller(fake);
    ASSERT_EQ(controller.get_auth_state(), AuthState::WaitingPhone);

    EXPECT_THROW(controller.enter_phone(""), std::invalid_argument);
    EXPECT_EQ(controller.get_auth_state(), AuthState::WaitingPhone);
    EXPECT_FALSE(fake.enter_phone_called);
}

// enter_phone: вызов в неправильном состоянии -> logic_error
TEST(AuthControllerTests, EnterPhoneInWrongStateThrowsLogicError) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingCode;

    AuthController controller(fake);
    ASSERT_EQ(controller.get_auth_state(), AuthState::WaitingCode);

    EXPECT_THROW(controller.enter_phone("+7123"), std::logic_error);
    EXPECT_FALSE(fake.enter_phone_called);
    EXPECT_EQ(controller.get_auth_state(), AuthState::WaitingCode);
}

// enter_phone: если клиент кидает — контроллер ставит Error и пробрасывает дальше
TEST(AuthControllerTests, EnterPhoneClientThrowsSetsErrorAndRethrows) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingPhone;
    fake.throw_in_enter_phone = true;

    AuthController controller(fake);
    ASSERT_EQ(controller.get_auth_state(), AuthState::WaitingPhone);

    EXPECT_THROW(controller.enter_phone("+71234567890"), std::runtime_error);
    EXPECT_EQ(controller.get_auth_state(), AuthState::Error);
}

// enter_code: нормальный поток
TEST(AuthControllerTests, EnterCodeInWaitingCodeSendsCodeAndBecomesReady) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingCode;

    AuthController controller(fake);
    ASSERT_EQ(controller.get_auth_state(), AuthState::WaitingCode);

    controller.enter_code("12345");

    EXPECT_TRUE(fake.enter_code_called);
    EXPECT_EQ(fake.last_code, "12345");
    EXPECT_EQ(controller.get_auth_state(), AuthState::Ready);
    EXPECT_TRUE(controller.is_authorized());
}

// enter_code: пустой код -> invalid_argument, состояние не меняется
TEST(AuthControllerTests, EnterCodeEmptyThrowsInvalidArgumentAndKeepsState) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingCode;

    AuthController controller(fake);
    ASSERT_EQ(controller.get_auth_state(), AuthState::WaitingCode);

    EXPECT_THROW(controller.enter_code(""), std::invalid_argument);
    EXPECT_FALSE(fake.enter_code_called);
    EXPECT_EQ(controller.get_auth_state(), AuthState::WaitingCode);
}

// enter_code: вызов в неправильном состоянии -> logic_error
TEST(AuthControllerTests, EnterCodeInWrongStateThrowsLogicError) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingPhone;

    AuthController controller(fake);
    ASSERT_EQ(controller.get_auth_state(), AuthState::WaitingPhone);

    EXPECT_THROW(controller.enter_code("12345"), std::logic_error);
    EXPECT_FALSE(fake.enter_code_called);
    EXPECT_EQ(controller.get_auth_state(), AuthState::WaitingPhone);
}

// enter_code: если клиент кидает — контроллер ставит Error и пробрасывает дальше
TEST(AuthControllerTests, EnterCodeClientThrowsSetsErrorAndRethrows) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingCode;
    fake.throw_in_enter_code = true;

    AuthController controller(fake);
    ASSERT_EQ(controller.get_auth_state(), AuthState::WaitingCode);

    EXPECT_THROW(controller.enter_code("12345"), std::runtime_error);
    EXPECT_EQ(controller.get_auth_state(), AuthState::Error);
}

// logout: уже LoggingOut — ничего не делает
TEST(AuthControllerTests, LogoutWhenAlreadyLoggingOutDoesNothing) {
    FakeTgClient fake;
    fake.state = AuthState::LoggingOut;

    AuthController controller(fake);
    ASSERT_EQ(controller.get_auth_state(), AuthState::LoggingOut);

    fake.log_out_called = false;

    controller.logout();

    EXPECT_FALSE(fake.log_out_called);
    EXPECT_EQ(controller.get_auth_state(), AuthState::LoggingOut);
}

// logout: не авторизованы и не LoggingOut — ничего не делает
TEST(AuthControllerTests, LogoutWhenNotAuthorizedDoesNothing) {
    FakeTgClient fake;
    fake.state = AuthState::WaitingPhone;

    AuthController controller(fake);
    ASSERT_FALSE(controller.is_authorized());

    fake.log_out_called = false;

    controller.logout();

    EXPECT_FALSE(fake.log_out_called);
    EXPECT_EQ(controller.get_auth_state(), AuthState::WaitingPhone);
}

// logout: авторизованы — вызывает client.log_out и переходит в LoggingOut
TEST(AuthControllerTests, LogoutWhenAuthorizedCallsClientAndMovesToLoggingOut) {
    FakeTgClient fake;
    fake.state = AuthState::Ready;

    AuthController controller(fake);
    ASSERT_TRUE(controller.is_authorized());

    fake.log_out_called = false;

    controller.logout();

    EXPECT_TRUE(fake.log_out_called);
    EXPECT_EQ(controller.get_auth_state(), AuthState::LoggingOut);
    EXPECT_FALSE(controller.is_authorized());
}

// logout: если клиент кидает — контроллер ставит Error, исключение не пробрасывается
TEST(AuthControllerTests, LogoutClientThrowsSetsErrorButDoesNotRethrow) {
    FakeTgClient fake;
    fake.state = AuthState::Ready;
    fake.throw_in_log_out = true;

    AuthController controller(fake);
    ASSERT_TRUE(controller.is_authorized());

    EXPECT_NO_THROW(controller.logout());
    EXPECT_EQ(controller.get_auth_state(), AuthState::Error);
}
