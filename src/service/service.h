#pragma once

#include <db/client.h>

#include <tgbot/tgbot.h>

namespace NDietBot {

class TService {
public:
    TService(NProto::TConfig config);

    std::future<int> Start();

    void InitBot();

private:
    void ProcessStart(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);

    void ProcessBirthdayAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);
    void ProcessWeightAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);
    void ProcessHeightAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);
    void ProcessActivityAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);
    void ProcessPurposeAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);

    void ProcessNewFoodAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);
    void ProcessNewFoodKalAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);
    void ProcessNewFoodProtAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);
    void ProcessNewFoodFatsAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);
    void ProcessNewFoodCarbAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);

    void ProcessFindFoodAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx);

    std::map<int32_t, void(TService::*)(TgBot::Message::Ptr, NProto::TChatInfo, TTransactionPtr)> Processors_;

    TClientPtr Client_;
    NProto::TConfig Config_;
    TgBot::Bot Bot_;
    
    BS::thread_pool ThreadPool_;
};

} // namespace NDietBot
