#include "client.h"

#include <common/exception.h>
#include <common/logger.h>

#include <google/protobuf/util/json_util.h>
#include <fmt/core.h>

namespace NDietBot {

namespace {

auto Logger = std::make_shared<TLogger>("Client");

}

TClient::TClient(const NProto::TDataBaseConfig& config)
    : ThreadPool_(config.num_threads() ? config.num_threads() : 16)
{
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

TTransactionPtr TClient::StartTransaction() {
    return std::make_shared<pqxx::work>(*Connection_);
}

////////////////////////////////////////////////////////////////////////////////

std::future<NProto::TConsumer> TClient::GetConsumer(int64_t id, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, id, tx](){
        NProto::TConsumer proto;
        try {
            auto query = fmt::format(
                "SELECT * FROM consumer WHERE id={}",
                id
            );
            auto result = tx->query<int64_t, int32_t, int64_t, int32_t, int32_t, int32_t>(query);
            THROW_IF(result.begin() == result.end(), "Consumer not found (ConsumerId: {})", id);

            auto [_, purpose, birthday, height, weight, activity] = *(result.begin());
            proto.set_id(id);
            proto.set_purpose(purpose);
            proto.mutable_birthday()->set_seconds(birthday);
            proto.set_height(height);
            proto.set_weight(weight);
            proto.set_activity(activity);
            return proto;
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to get consumer.");
            proto.set_id(0);
            return proto;
        }
    });
}

std::future<int> TClient::DeleteConsumer(int64_t id, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, id, tx](){
        try {
            auto query = fmt::format(
                "DELETE FROM consumer WHERE id={}",
                id
            );
            tx->exec(query);
            return 0;
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to delete consumer.");
        }
        return 1;
    });
}

std::future<int> TClient::CreateConsumer(const NProto::TConsumer& consumer, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, consumer, tx](){
        try {
            auto query = fmt::format(
                R"(
INSERT INTO consumer (id, purpose, birthday, height, weight, activity)
    VALUES ({0}, {1}, {2}, {3}, {4}, {5})
                )",
                consumer.id(),
                consumer.purpose(),
                consumer.birthday().seconds(),
                consumer.height(),
                consumer.weight(),
                consumer.activity()
            );
            tx->exec(query);
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to update product.");
            return 1;
        }
        return 0;
    });
}

std::future<int> TClient::UpdateConsumer(const NProto::TConsumer& consumer, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, consumer, tx](){
        try {
            auto query = fmt::format(
                R"(
UPDATE consumer
    SET
        purpose = {1},
        birthday = {2},
        height = {3},
        weight = {4},
        activity = {5}
    WHERE
        id = {0}
                )",
                consumer.id(),
                consumer.purpose(),
                consumer.birthday().seconds(),
                consumer.height(),
                consumer.weight(),
                consumer.activity()
            );
            tx->exec(query);
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to update product.");
            return 1;
        }
        return 0;
    });
}

////////////////////////////////////////////////////////////////////////////////

std::future<NProto::TProduct> TClient::GetProduct(const std::string& name, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, name, tx](){
        NProto::TProduct proto;
        try {
            auto query = fmt::format(
                "SELECT * FROM product WHERE name='{}'",
                name
            );
            auto result = tx->query<std::string, float, float, float, float>(query);
            THROW_IF(result.begin() == result.end(), "Product not found (ProductName: {})", name);

            auto [_, kalories, protein, fats, carbohydrates] = *(result.begin());
            proto.set_name(name);
            proto.set_kalories(kalories);
            proto.set_protein(protein);
            proto.set_fats(fats);
            proto.set_carbohydrates(carbohydrates);
            return proto;
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to get product.");
            proto.set_name("");
            return proto;
        }
    });
}

std::future<int> TClient::DeleteProduct(const std::string& name, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, name, tx](){
        try {
            auto query = fmt::format(
                "DELETE FROM product WHERE name='{}'",
                name
            );
            tx->exec(query);
            return 0;
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to delete product.");
        }
        return 1;
    });
}

std::future<int> TClient::CreateProduct(const NProto::TProduct& product, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, product, tx](){
        try {
            auto query = fmt::format(
                R"(
INSERT INTO product (name, kalories, protein, fats, carbohydrates)
    VALUES ('{0}', {1}, {2}, {3}, {4})
                )",
                product.name(),
                product.kalories(),
                product.protein(),
                product.fats(),
                product.carbohydrates()
            );
            tx->exec(query);
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to update product.");
            return 1;
        }
        return 0;
    });
}

std::future<int> TClient::UpdateProduct(const NProto::TProduct& product, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, product, tx](){
        try {
            auto query = fmt::format(
                R"(
UPDATE product
    SET
        kalories = {1},
        protein = {2},
        fats = {3},
        carbohydrates = {4}
    WHERE
        name = '{0}'
                )",
                product.name(),
                product.kalories(),
                product.protein(),
                product.fats(),
                product.carbohydrates()
            );
            tx->exec(query);
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to update product.");
            return 1;
        }
        return 0;
    });
}

} // namespace NDietBot
