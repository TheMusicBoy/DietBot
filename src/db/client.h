#pragma once

#include <generated/config.pb.h>
#include <generated/product.pb.h>
#include <generated/consumer.pb.h>
#include <common/thread_pool.h>
#include <pqxx/pqxx>

namespace NDietBot {

using TConnectionPtr = std::shared_ptr<pqxx::connection>;

class TClient {
public:
    TClient(const NProto::TDataBaseConfig& config);

    std::future<NProto::TConsumer> GetConsumer(const std::string& login);
    std::future<int> UpdateConsumer(const NProto::TConsumer& consumer);
    std::future<int> CreateConsumer(const NProto::TConsumer& consumer);

    std::future<NProto::TProduct> GetProduct(const std::string& name);
    std::future<int> UpdateProduct(const NProto::TProduct& user);
    std::future<int> CreateProduct(const NProto::TProduct& user);

private:
    TConnectionPtr Connection_;
    BS::thread_pool ThreadPool_;

};

}
