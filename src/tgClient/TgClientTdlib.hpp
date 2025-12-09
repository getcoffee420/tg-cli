//
// Created by get-coffee on 12/8/25.
//

#ifndef TGCLIENTTDLIB_HPP
#define TGCLIENTTDLIB_HPP
#pragma once

#include "ITgClient.hpp"

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <cstdint>
#include <map>
#include <memory>

namespace td_api = td::td_api;

class TgClientTdlib : public ITgClient {
public:
    TgClientTdlib();
    ~TgClientTdlib() override;

    AuthState check_status() override;

    void enter_phone_number(std::string phone) override;
    void enter_message_code(std::string code) override;
    void log_out() override;

    std::vector<Chat> get_chats(int limit) override;
    std::vector<Message> get_chat_history(std::string chatID, int limit) override;
    void send_message(std::string chatID, std::string message) override;
    void send_tdlib_parameters() override;

private:
    using BaseChat = ITgClient::Chat;
    using BaseMessage = ITgClient::Message;
    using BaseAuthState = ITgClient::AuthState;

    std::unique_ptr<td::ClientManager> client_manager_;
    td::ClientManager::ClientId client_id_{0};

    std::uint64_t next_query_id_{1};
    BaseAuthState auth_state_{BaseAuthState::Error};

    std::map<std::int64_t, BaseChat> chats_;

    std::uint64_t send_query(td_api::object_ptr<td_api::Function> f);

    [[nodiscard]] td_api::object_ptr<td_api::Object> wait_result(std::uint64_t request_id, double timeout_seconds) const;
};


#endif //TGCLIENTTDLIB_HPP
