#pragma once

#include <generated/config.pb.h>
#include <generated/product.pb.h>
#include <generated/consumer.pb.h>
#include <common/thread_pool.h>
#include <pqxx/pqxx>

namespace NDietBot {

using TConnectionPtr = std::shared_ptr<pqxx::connection>;
using TTransactionPtr = std::shared_ptr<pqxx::work>;

class TClient {
public:
    TClient(const NProto::TDataBaseConfig& config);
    TTransactionPtr StartTransaction();

    std::future<NProto::TConsumer> GetConsumer(int64_t id, TTransactionPtr tx);
    std::future<int> UpdateConsumer(const NProto::TConsumer& consumer, TTransactionPtr tx);
    std::future<int> CreateConsumer(const NProto::TConsumer& consumer, TTransactionPtr tx);
    std::future<int> DeleteConsumer(int64_t id, TTransactionPtr tx);

    std::future<NProto::TProduct> GetProduct(const std::string& name, TTransactionPtr tx);
    std::future<int> UpdateProduct(const NProto::TProduct& user, TTransactionPtr tx);
    std::future<int> CreateProduct(const NProto::TProduct& user, TTransactionPtr tx);
    std::future<int> DeleteProduct(const std::string& name, TTransactionPtr tx);

private:
    TConnectionPtr Connection_;
    BS::thread_pool ThreadPool_;

};

using TClientPtr = std::shared_ptr<TClient>;

}
