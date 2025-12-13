//
// Created by get-coffee on 12/8/25.
//

#include "TgClientTdlib.hpp"

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <iostream>
#include <string>
#include <utility>


TgClientTdlib::TgClientTdlib() {
    td::ClientManager::execute(td_api::make_object<td_api::setLogVerbosityLevel>(0));
    client_manager_ = std::make_unique<td::ClientManager>();
    client_id_ = client_manager_->create_client_id();
}

TgClientTdlib::~TgClientTdlib() = default;

std::uint64_t TgClientTdlib::send_query(td_api::object_ptr<td_api::Function> f) {
    auto query_id = next_query_id_++;
    client_manager_->send(client_id_, query_id, std::move(f));
    return query_id;
}

td_api::object_ptr<td_api::Object> TgClientTdlib::wait_result(std::uint64_t request_id, double timeout_seconds) const {
    while (true) {
        auto response = client_manager_->receive(timeout_seconds);
        if (!response.object) {
            return nullptr;
        }
        if (response.request_id == request_id) {
            return std::move(response.object);
        }
    }
}

void TgClientTdlib::send_tdlib_parameters() {
    auto request = td_api::make_object<td_api::setTdlibParameters>();
    request->database_directory_ = "tdlib";
    request->use_message_database_ = true;
    request->use_chat_info_database_ = true;
    request->use_secret_chats_ = false;
    request->api_id_ = std::stoi(API_ID);
    request->api_hash_ = std::string(API_HASH);
    request->system_language_code_ = "en";
    request->device_model_ = "Desktop";
    request->application_version_ = "1.0";
    request->system_version_ = "unknown";

    auto id = send_query(std::move(request));
    (void) wait_result(id, 10.0);
}


ITgClient::AuthState TgClientTdlib::check_status() {
    auto req = td_api::make_object<td_api::getAuthorizationState>();
    auto id = send_query(std::move(req));
    auto res = wait_result(id, 3.0);

    if (res == nullptr) {
        return AuthState::Error;
    }

    switch (res->get_id()) {
        case td_api::authorizationStateReady::ID:
            return AuthState::Ready;
        case td_api::authorizationStateWaitPhoneNumber::ID:
            return AuthState::WaitingPhone;
        case td_api::authorizationStateWaitCode::ID:
            return AuthState::WaitingCode;
        case td_api::authorizationStateWaitPassword::ID:
            return AuthState::WaitingPassword;
        case td_api::authorizationStateWaitTdlibParameters::ID:
          return AuthState::WaitingTdlibParameters;
        default:
            return AuthState::Error;
    }
}

void TgClientTdlib::enter_phone_number(std::string phone) {
    auto req = td_api::make_object<td_api::setAuthenticationPhoneNumber>();
    req->phone_number_ = std::move(phone);
    req->settings_ = nullptr;
    auto id = send_query(std::move(req));
    (void) wait_result(id, 60.0);
}

void TgClientTdlib::enter_message_code(std::string code) {
    auto req = td_api::make_object<td_api::checkAuthenticationCode>();
    req->code_ = std::move(code);
    auto id = send_query(std::move(req));
    (void) wait_result(id, 60.0);
}

void TgClientTdlib::log_out() {
    auto id = send_query(td_api::make_object<td_api::logOut>());
    (void) wait_result(id, 10.0);
}

std::vector<ITgClient::Chat> TgClientTdlib::get_chats(int limit) {
    std::vector<BaseChat> result;

    auto req = td_api::make_object<td_api::getChats>();
    req->limit_ = limit;
    req->chat_list_ = nullptr;

    auto id = send_query(std::move(req));
    auto obj = wait_result(id, 10.0);
    if (!obj) {
        return result;
    }
    if (obj->get_id() == td_api::error::ID) {
        auto err = td::move_tl_object_as<td_api::error>(obj);
        std::cerr << "get_chats error: " << to_string(err) << std::endl;
        return result;
    }

    auto chats = td::move_tl_object_as<td_api::chats>(obj);
    result.reserve(chats->chat_ids_.size());

    for (auto chat_id : chats->chat_ids_) {
        BaseChat chat;
        chat.chatId = std::to_string(chat_id);

        auto it = chats_.find(chat_id);
        if (it != chats_.end()) {
            chat.title = it->second.title;
        } else {
            auto get_chat = td_api::make_object<td_api::getChat>();
            get_chat->chat_id_ = chat_id;
            auto chat_req_id = send_query(std::move(get_chat));
            auto chat_obj = wait_result(chat_req_id, 10.0);
            if (chat_obj && chat_obj->get_id() != td_api::error::ID) {
                auto chat_full = td::move_tl_object_as<td_api::chat>(chat_obj);
                chat.title = chat_full->title_;

                BaseChat cache_entry;
                cache_entry.chatId = chat.chatId;
                cache_entry.title = chat.title;
                chats_[chat_id] = std::move(cache_entry);
            }
        }

        result.push_back(std::move(chat));
    }

    return result;
}

std::vector<ITgClient::Message> TgClientTdlib::get_chat_history(std::string chatID, int limit) {
    std::vector<BaseMessage> result;

    std::int64_t chat_id = 0;
    try {
        chat_id = std::stoll(chatID);
    } catch (...) {
        return result;
    }

    auto req = td_api::make_object<td_api::getChatHistory>();
    req->chat_id_ = chat_id;
    req->from_message_id_ = 0;
    req->offset_ = 0;
    req->limit_ = limit;
    req->only_local_ = false;

    auto id = send_query(std::move(req));
    auto obj = wait_result(id, 10.0);
    if (!obj) {
        return result;
    }
    if (obj->get_id() == td_api::error::ID) {
        auto err = td::move_tl_object_as<td_api::error>(obj);
        std::cerr << "get_chat_history error: " << to_string(err) << std::endl;
        return result;
    }

    auto msgs = td::move_tl_object_as<td_api::messages>(obj);
    result.reserve(msgs->messages_.size());

    for (auto &msg_ptr : msgs->messages_) {
        if (!msg_ptr) {
            continue;
        }

        BaseMessage m;
        m.chatId = chatID;
        m.text.clear();
        m.sender.clear();

        if (msg_ptr->sender_id_->get_id() == td_api::messageSenderUser::ID) {
            auto sender_user =
                static_cast<td_api::messageSenderUser*>(msg_ptr->sender_id_.get());

            m.sender = std::to_string(sender_user->user_id_);
        }
        m.messageID = std::to_string(msg_ptr->id_);

        if (msg_ptr->content_ && msg_ptr->content_->get_id() == td_api::messageText::ID) {
            auto &content = static_cast<td_api::messageText &>(*msg_ptr->content_);
            if (content.text_) {
                m.text = content.text_->text_;
            }
        }

        result.push_back(std::move(m));
    }

    return result;
}

void TgClientTdlib::send_message(std::string chatID, std::string message) {
    std::int64_t chat_id = 0;
    try {
        chat_id = std::stoll(chatID);
    } catch (...) {
        return;
    }

    auto req = td_api::make_object<td_api::sendMessage>();
    req->chat_id_ = chat_id;

    auto content = td_api::make_object<td_api::inputMessageText>();
    content->text_ = td_api::make_object<td_api::formattedText>();
    content->text_->text_ = std::move(message);

    req->input_message_content_ = std::move(content);

    auto id = send_query(std::move(req));
    (void) wait_result(id, 10.0);
}