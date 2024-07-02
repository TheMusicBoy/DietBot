#include "client.h"

#include <common/exception.h>
#include <common/logger.h>

#include <google/protobuf/util/json_util.h>
#include <fmt/core.h>

namespace NDietBot {

namespace {

auto Logger = std::make_shared<TLogger>("Client");

}

TClient::TClient(const NProto::TDataBaseConfig& config) {
    try {
        Connection_ = std::make_shared<pqxx::connection>(
            fmt::format("dbname = {} hostaddr = {} port = {} user = {} password = {}",
                config.dbname(),
                config.hostaddr(),
                config.port(),
                config.user(),
                config.password()
            )
        );

        THROW_UNLESS(Connection_->is_open(), "App can't open data base. ");
        LOG_INFO("Opened database successfully: {}", Connection_->dbname());
    } catch (std::exception& e) {
        THROW_ERROR(e, "Something went wrong while start db client. ");
    }
}

////////////////////////////////////////////////////////////////////////////////

NProto::TConsumer TClient::GetConsumer(const std::string& login) {
    try {
        pqxx::work tx(*Connection_);

        auto query = fmt::format(
            R"(
SELECT * FROM consumer WHERE login=\"{}\"
            )",
            login);
        auto [_, purpose, birthday, height, weight, activity] = *tx.query<std::string, int32_t, std::string, int32_t, int32_t, int32_t>(query).begin();
        NProto::TConsumer proto;
        proto.set_login(login);
        proto.set_purpose(purpose);
        proto.mutable_birthday()->ParseFromString(birthday);
        proto.set_height(height);
        proto.set_weight(weight);
        proto.set_activity(activity);
        return proto;
    } catch (std::exception& e) {
        THROW_ERROR(e, "Failed to get consumer.");
    }
}

void TClient::CreateConsumer(const NProto::TConsumer& consumer) {
    try {
        pqxx::work tx(*Connection_);

        std::string birthday;
        consumer.birthday().SerializeToString(&birthday);
        auto query = fmt::format(
            R"(
INSERT INTO consumer (login, purpose, birthday, height, weight, activity)
    VALUES("{0}", {1}, "{2}", {3}, {4}, {5})
            )",
            consumer.login(),
            consumer.purpose(),
            birthday,
            consumer.height(),
            consumer.weight(),
            consumer.activity());
        tx.exec(query);
        tx.commit();
    } catch (std::exception& e) {
        THROW_ERROR(e, "Failed to create consumer.");
    }
}

void TClient::UpdateConsumer(const NProto::TConsumer& consumer) {
    try {
        pqxx::work tx(*Connection_);

        std::string birthday;
        consumer.birthday().SerializeToString(&birthday);
        auto query = fmt::format(
            R"(
UPDATE consumer
    SET
        purpose = {1},
        birthday = "{2}",
        height = {3},
        weight = {4},
        activity = {5}
    WHERE
        login = "{0}"
    VALUES("{0}", {1}, "{2}", {3}, {4}, {5})
            )",
            consumer.login(),
            consumer.purpose(),
            birthday,
            consumer.height(),
            consumer.weight(),
            consumer.activity());
        tx.exec(query);
        tx.commit();
    } catch (std::exception& e) {
        THROW_ERROR(e, "Failed to update consumer.");
    }
}

////////////////////////////////////////////////////////////////////////////////


NProto::TProduct TClient::GetProduct(const std::string& name) {
    try {
        pqxx::work tx(*Connection_);

        auto query = fmt::format(
            R"(
SELECT * FROM product WHERE name=\"{}\"
            )",
            name);
        auto [_, kalories, protein, fats, carbohydrates] = *tx.query<std::string, float, float, float, float>(query).begin();
        NProto::TProduct proto;
        proto.set_name(name);
        proto.set_kalories(kalories);
        proto.set_protein(protein);
        proto.set_fats(fats);
        proto.set_carbohydrates(carbohydrates);
        return proto;
    } catch (std::exception& e) {
        THROW_ERROR(e, "Failed to get product.");
    }
}

void TClient::CreateProduct(const NProto::TProduct& product) {
    try {
        pqxx::work tx(*Connection_);

        auto query = fmt::format(
            R"(
INSERT INTO product (name, kalories, protein, fats, carbohydrates)
    VALUES("{0}", {1}, {2}, {3}, {4})
            )",
            product.name(),
            product.kalories(),
            product.protein(),
            product.fats(),
            product.carbohydrates());
        tx.exec(query);
        tx.commit();
    } catch (std::exception& e) {
        THROW_ERROR(e, "Failed to create product.");
    }
}

void TClient::UpdateProduct(const NProto::TProduct& product) {
    try {
        pqxx::work tx(*Connection_);

        auto query = fmt::format(
            R"(
UPDATE product
    SET
        kalories = {1},
        protein = {2},
        fats = {3},
        carbohydrates = {4}
    WHERE
        name = "{0}"
            )",
            product.name(),
            product.kalories(),
            product.protein(),
            product.fats(),
            product.carbohydrates());
        tx.exec(query);
        tx.commit();
    } catch (std::exception& e) {
        THROW_ERROR(e, "Failed to update product.");
    }
}


} // namespace NDietBot
