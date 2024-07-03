#include "service.h"

#include <common/logger.h>
#include <fmt/chrono.h>
#include <common/exception.h>

#include <algorithm>
#include <sstream>

namespace NDietBot {

namespace {

auto Logger = std::make_shared<TLogger>("Service");

}

TService::TService(NProto::TConfig config)
    : Config_(config),
      Client_(std::make_shared<TClient>(config.db_config())),
      Bot_(config.token())
{
    InitBot();
    LOG_INFO("Service initialized.");
}

std::future<int> TService::Start() {
    return ThreadPool_.submit_task([this](){
        try {
            LOG_INFO("Bot username: {}", Bot_.getApi().getMe()->username);
            TgBot::TgLongPoll longPoll(Bot_);
            while (true) {
                LOG_INFO("Long poll started");
                longPoll.start();
            }
        } catch (TgBot::TgException& e) {
            ELOG_ERROR(e, "Stop bot.");
            return 1;
        }
        return 0;
    });
}

void TService::ProcessStart(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    Bot_.getApi().sendMessage(message->chat->id, R"(
Для использования бота используйте следующие команды:
 - /start -- для перенастройки бота;
 - /info -- для проверки информации о себе;
 - /newfood -- для добавления продукта в базу данных;
 - /findfood -- для поиска продукта по названию;
    )");
}

void TService::ProcessBirthdayAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    std::tm tm = {};
    std::istringstream ss(message->text);
    ss >> std::get_time(&tm, "%d-%m-%Y");
    LOG_INFO("Setting date");

    THROW_IF(ss.fail(), "Неверный формат даты, проверьте правильность вписанный данных");

    LOG_INFO("Format is ok");
    auto consumer = Client_->GetConsumer(message->from->id, tx).get();
    THROW_UNLESS(consumer.id(), "Пользователь не был найден.");

    LOG_INFO("Consumer is ok");
    time_t date = mktime(&tm);

    // Во время преобразования форматов, смещается на 1 день :(
    consumer.mutable_birthday()->set_seconds(date + 3600 * 24); 
    THROW_IF(Client_->UpdateConsumer(consumer, tx).get(), "Произошла ошибка при записи данных.");
    LOG_INFO("Consumer updated");
    chat.set_status(NProto::EChatStatus::WeightAsk);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");
    LOG_INFO("Chat updated");

    Bot_.getApi().sendMessage(message->chat->id, "Дата успешно указана!\n\nТеперь укажите вес в полных кг, только цифру.");
}

void TService::ProcessWeightAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    auto consumer = Client_->GetConsumer(message->from->id, tx).get();
    THROW_UNLESS(consumer.id(), "Пользователь не был найден.");

    consumer.set_weight(std::stoi(message->text));
    THROW_IF(Client_->UpdateConsumer(consumer, tx).get(), "Произошла ошибка при записи данных.");
    chat.set_status(NProto::EChatStatus::HeightAsk);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");

    Bot_.getApi().sendMessage(message->chat->id, "Вес успешно указан!\n\nТеперь укажите рост в сантиметрах, только цифру.");
}

void TService::ProcessHeightAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    auto consumer = Client_->GetConsumer(message->from->id, tx).get();
    THROW_UNLESS(consumer.id(), "Пользователь не был найден.");

    consumer.set_height(std::stoi(message->text));
    THROW_IF(Client_->UpdateConsumer(consumer, tx).get(), "Произошла ошибка при записи данных.");
    chat.set_status(NProto::EChatStatus::ActivityAsk);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");

    Bot_.getApi().sendMessage(message->chat->id, "Рост успешно указан!\n\nТеперь расскажите, сколько ккал вы сжигаете в день.");
}

void TService::ProcessActivityAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    auto consumer = Client_->GetConsumer(message->from->id, tx).get();
    THROW_UNLESS(consumer.id(), "Пользователь не был найден.");

    consumer.set_activity(std::stoi(message->text));
    THROW_IF(Client_->UpdateConsumer(consumer, tx).get(), "Произошла ошибка при записи данных.");
    chat.set_status(NProto::EChatStatus::PurposeAsk);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");

    Bot_.getApi().sendMessage(message->chat->id, "Сжигаемые ккал успешно указаны!\n\nТеперь расскажите, какой вес вы хотите иметь.");
}

void TService::ProcessPurposeAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    auto consumer = Client_->GetConsumer(message->from->id, tx).get();
    THROW_UNLESS(consumer.id(), "Пользователь не был найден.");

    consumer.set_purpose(std::stoi(message->text));
    THROW_IF(Client_->UpdateConsumer(consumer, tx).get(), "Произошла ошибка при записи данных.");
    chat.set_status(NProto::EChatStatus::Start);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");

    Bot_.getApi().sendMessage(message->chat->id, R"(
Цель успешно поставлена!

Теперь вы можете воспользоваться ботом, используя следующие команды:
 - /start -- для перенастройки профиля;
 - /info -- для проверки информации о себе;
    )");
}

void TService::ProcessNewFoodAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    NProto::TProduct product;
    product.set_name(message->text);
    
    THROW_IF(Client_->CreateProduct(product, tx).get(), "Произошла ошибка при добавлении продукта.");

    chat.set_status(NProto::EChatStatus::KaloriesAsk);
    chat.set_last_string_id(message->text);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");

    Bot_.getApi().sendMessage(message->chat->id, "Теперь укажите ккал на 100 грам.");
}

void TService::ProcessNewFoodKalAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    auto product = Client_->GetProduct(chat.last_string_id(), tx).get();
    THROW_IF(product.name().empty(), "Произошла ошибка при добавлении продукта.");

    product.set_kalories(std::stof(message->text));
    THROW_IF(Client_->UpdateProduct(product, tx).get(), "Произошла ошибка при записи данных.");

    chat.set_status(NProto::EChatStatus::FatsAsk);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");

    Bot_.getApi().sendMessage(message->chat->id, "Теперь укажите количество жира на 100 грам.");
}

void TService::ProcessNewFoodFatsAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    auto product = Client_->GetProduct(chat.last_string_id(), tx).get();
    THROW_IF(product.name().empty(), "Произошла ошибка при добавлении продукта.");

    product.set_fats(std::stof(message->text));
    THROW_IF(Client_->UpdateProduct(product, tx).get(), "Произошла ошибка при записи данных.");

    chat.set_status(NProto::EChatStatus::ProteinAsk);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");

    Bot_.getApi().sendMessage(message->chat->id, "Теперь укажите белков на 100 грам.");
}

void TService::ProcessNewFoodProtAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    auto product = Client_->GetProduct(chat.last_string_id(), tx).get();
    THROW_IF(product.name().empty(), "Произошла ошибка при добавлении продукта.");

    product.set_protein(std::stof(message->text));
    THROW_IF(Client_->UpdateProduct(product, tx).get(), "Произошла ошибка при записи данных.");

    chat.set_status(NProto::EChatStatus::CarbohydratesAsk);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");

    Bot_.getApi().sendMessage(message->chat->id, "Теперь укажите углеводов на 100 грам.");
}

void TService::ProcessNewFoodCarbAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    auto product = Client_->GetProduct(chat.last_string_id(), tx).get();
    THROW_IF(product.name().empty(), "Произошла ошибка при добавлении продукта.");

    product.set_carbohydrates(std::stof(message->text));
    THROW_IF(Client_->UpdateProduct(product, tx).get(), "Произошла ошибка при записи данных.");

    chat.set_status(NProto::EChatStatus::Start);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");

    Bot_.getApi().sendMessage(message->chat->id, "Продукт успешно занесен в базу данных!");
}

void TService::ProcessFindFoodAsk(TgBot::Message::Ptr message, NProto::TChatInfo chat, TTransactionPtr tx) {
    auto product = Client_->GetProductsLike(message->text, tx).get();
    std::string result;
    if (product.empty()) {
        result = "Не было найдено продуктов по запросу.";
    } else {
        std::ostringstream ss("По запросу были найдены следующие продукты:");
        for (auto p : product) {
            ss << fmt::format("\n{} -- на 100 гр:\n   {} ккал, {} гр. жира, {} гр. белка, {} гр. углеводов.",
                p.name(), p.kalories(), p.fats(), p.protein(), p.carbohydrates());
        }
        result = ss.str();
    }


    chat.set_status(NProto::EChatStatus::Start);
    THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Произошла ошибка при записи данных.");
    Bot_.getApi().sendMessage(message->chat->id, result);
}

void TService::InitBot() {
    Processors_.emplace(NProto::EChatStatus::Start, &TService::ProcessStart);

    Processors_.emplace(NProto::EChatStatus::BirthdayAsk, &TService::ProcessBirthdayAsk);
    Processors_.emplace(NProto::EChatStatus::WeightAsk, &TService::ProcessWeightAsk);
    Processors_.emplace(NProto::EChatStatus::HeightAsk, &TService::ProcessHeightAsk);
    Processors_.emplace(NProto::EChatStatus::ActivityAsk, &TService::ProcessActivityAsk);
    Processors_.emplace(NProto::EChatStatus::PurposeAsk, &TService::ProcessPurposeAsk);

    Processors_.emplace(NProto::EChatStatus::ProductNameAsk, &TService::ProcessNewFoodAsk);
    Processors_.emplace(NProto::EChatStatus::KaloriesAsk, &TService::ProcessNewFoodKalAsk);
    Processors_.emplace(NProto::EChatStatus::FatsAsk, &TService::ProcessNewFoodFatsAsk);
    Processors_.emplace(NProto::EChatStatus::ProteinAsk, &TService::ProcessNewFoodProtAsk);
    Processors_.emplace(NProto::EChatStatus::CarbohydratesAsk, &TService::ProcessNewFoodCarbAsk);

    Processors_.emplace(NProto::EChatStatus::FindFoodAsk, &TService::ProcessFindFoodAsk);

    Bot_.getEvents().onAnyMessage([this](TgBot::Message::Ptr message) {
        try {
            auto tx = Client_->StartTransaction();

            if (message->text.starts_with("/start")) {
                NProto::TConsumer consumer;
                consumer.set_id(message->from->id);
                
                NProto::TChatInfo chat;
                chat.set_id(message->chat->id);
                chat.set_status(NProto::EChatStatus::BirthdayAsk);

                THROW_IF(Client_->CreateConsumer(consumer, tx, 1).get(), "Stop check consumer.");
                THROW_IF(Client_->CreateChatInfo(chat, tx, 2).get(), "Stop update chat info.");
                tx->commit();
                Bot_.getApi().sendMessage(message->chat->id, fmt::format(R"(
Привет, {}!

Начнем настройку профиля. Впишите дату рождения в формате дд-мм-гггг, без пробелов.
                )", message->from->username));
                return;
            }

            if (message->text.starts_with("/info")) {
                auto consumer = Client_->GetConsumer(message->from->id, tx).get();
                auto time = std::chrono::system_clock::from_time_t(consumer.birthday().seconds());
                Bot_.getApi().sendMessage(message->chat->id, fmt::format(R"(
Твоя информация:
 - дата рождения: {:%d-%m-%Y};
 - вес {} кг;
 - рост {} см;
 - кол-во сжигаемых ккал в день: {} ккал;
 - цель: весить {} кг;
                    )",
                    time,
                    consumer.weight(),
                    consumer.height(),
                    consumer.activity(),
                    consumer.purpose()
                 ));
                tx->commit();
                return;
            }

            if (message->text.starts_with("/newfood")) {
                NProto::TChatInfo chat;
                chat.set_id(message->chat->id);
                chat.set_status(NProto::EChatStatus::ProductNameAsk);

                Bot_.getApi().sendMessage(message->chat->id, R"(
Напиши название продукта, который хочешь добавить.
)"
                );
                THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Something went wrong with db.");
                tx->commit();
                return;
            }

            if (message->text.starts_with("/findfood")) {
                NProto::TChatInfo chat;
                chat.set_id(message->chat->id);
                chat.set_status(NProto::EChatStatus::FindFoodAsk);

                Bot_.getApi().sendMessage(message->chat->id, R"(
Напиши примерное название продукта, который ищешь
)"
                );
                THROW_IF(Client_->UpdateChatInfo(chat, tx).get(), "Something went wrong with db.");
                tx->commit();
                return;
            }

            auto chat = Client_->GetChatInfo(message->chat->id, tx).get();
            (this->*(Processors_.at(static_cast<uint32_t>(chat.status()))))(message, chat, tx);
            tx->commit();
        } catch (std::exception& e) {
            Bot_.getApi().sendMessage(
                message->chat->id,
                fmt::format("Произошла ошибка: {}", e.what())
            );
        }
    });
}

} // namespace NDietBot
