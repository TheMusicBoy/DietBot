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

std::future<int> TClient::CreateConsumer(const NProto::TConsumer& consumer, TTransactionPtr tx, int behavior) {
    return ThreadPool_.submit_task([this, consumer, tx, behavior](){
        try {
            std::string conflict = "";
            if (behavior) {
                conflict = behavior == 1 ? "ON CONFLICT DO NOTHING" : R"(
ON CONFLICT (id) DO UPDATE
    purpose = excluded.purpose
    birthday = excluded.birthday
    height = excluded.height
    wieght = excluded.wieght
    activity = excluded.activity
                )";
            }
            auto query = fmt::format(
                R"(INSERT INTO consumer (id, purpose, birthday, height, weight, activity)
VALUES ({0}, {1}, {2}, {3}, {4}, {5}) {6})",
                consumer.id(),
                consumer.purpose(),
                consumer.birthday().seconds(),
                consumer.height(),
                consumer.weight(),
                consumer.activity(),
                conflict
            );
            tx->exec(query);
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to create product.");
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

std::future<std::vector<NProto::TProduct>> TClient::GetProductsLike(const std::string& name, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, name, tx](){
        std::vector<NProto::TProduct> protos;
        try {
            auto query = fmt::format(
                "SELECT * FROM product WHERE name LIKE '%{}%'",
                name
            );
            auto result = tx->query<std::string, float, float, float, float>(query);
            THROW_IF(result.begin() == result.end(), "Products not found (ProductName: {})", name);

            for (auto [name, kalories, protein, fats, carbohydrates] : result) {
                NProto::TProduct p;
                LOG_INFO("Found product {}", name);
                p.set_name(name);
                p.set_kalories(kalories);
                p.set_protein(protein);
                p.set_fats(fats);
                p.set_carbohydrates(carbohydrates);
                protos.push_back(p);
            }
            return protos;
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to get products.");
            return protos;
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

std::future<int> TClient::CreateProduct(const NProto::TProduct& product, TTransactionPtr tx, int behavior) {
    return ThreadPool_.submit_task([this, product, tx, behavior](){
        try {
            std::string conflict = "";
            if (behavior) {
                conflict = behavior == 1 ? "ON CONFLICT DO NOTHING" : R"(
ON CONFLICT (name) DO UPDATE
    kalories = excluded.kalories
    protein = excluded.protein
    fats = excluded.fats
    carbohydrates = excluded.carbohydrates
)";
            }
            auto query = fmt::format(
                R"(INSERT INTO product (name, kalories, protein, fats, carbohydrates)
    VALUES ('{0}', {1}, {2}, {3}, {4}) {5})",
                product.name(),
                product.kalories(),
                product.protein(),
                product.fats(),
                product.carbohydrates(),
                conflict
            );
            tx->exec(query);
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to create product.");
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

////////////////////////////////////////////////////////////////////////////////

std::future<NProto::TChatInfo> TClient::GetChatInfo(int64_t id, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, id, tx](){
        NProto::TChatInfo proto;
        try {
            auto query = fmt::format(
                "SELECT * FROM chat_info WHERE id={}",
                id
            );
            auto result = tx->query<int64_t, int32_t, std::string>(query);
            THROW_IF(result.begin() == result.end(), "Chat info not found (ChatId: {})", id);

            auto [_, status, last_string_id] = *(result.begin());
            proto.set_id(id);
            proto.set_status(NProto::EChatStatus(status));
            proto.set_last_string_id(last_string_id);
            return proto;
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to get chat info.");
            proto.set_id(0);
            return proto;
        }
    });
}

std::future<int> TClient::DeleteChatInfo(int64_t id, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, id, tx](){
        try {
            auto query = fmt::format(
                "DELETE FROM chat_info WHERE id={}",
                id
            );
            tx->exec(query);
            return 0;
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to delete chat_info.");
        }
        return 1;
    });
}

std::future<int> TClient::CreateChatInfo(const NProto::TChatInfo& chatInfo, TTransactionPtr tx, int behavior) {
    return ThreadPool_.submit_task([this, chatInfo, tx, behavior](){
        try {
            std::string conflict = "";
            if (behavior) {
                conflict = behavior == 1 ? "ON CONFLICT DO NOTHING" : "ON CONFLICT (id) DO UPDATE SET status = excluded.status";
            }
            auto query = fmt::format(
                "INSERT INTO chat_info (id, status, last_string_id) VALUES ({0}, {1}, '{2}') {3}",
                chatInfo.id(),
                static_cast<int32_t>(chatInfo.status()),
                chatInfo.last_string_id(),
                conflict
            );
            tx->exec(query);
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to create chat_info.");
            return 1;
        }
        return 0;
    });
}

std::future<int> TClient::UpdateChatInfo(const NProto::TChatInfo& chatInfo, TTransactionPtr tx) {
    return ThreadPool_.submit_task([this, chatInfo, tx](){
        try {
            auto query = fmt::format(
                R"(
UPDATE chat_info
    SET
        status = {1},
        last_string_id = '{2}'
    WHERE
        id = {0}
                )",
                chatInfo.id(),
                static_cast<int32_t>(chatInfo.status()),
                chatInfo.last_string_id()
            );
            tx->exec(query);
        } catch (std::exception& e) {
            ELOG_ERROR(e, "Failed to update chat_info.");
            return 1;
        }
        return 0;
    });
}

} // namespace NDietBot
