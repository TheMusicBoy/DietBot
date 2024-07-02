#pragma once

#include <generated/config.pb.h>
#include <generated/product.pb.h>
#include <generated/consumer.pb.h>
#include <pqxx/pqxx>

namespace NDietBot {

using TConnectionPtr = std::shared_ptr<pqxx::connection>;

class TClient {
public:
    TClient(const NProto::TDataBaseConfig& config);

    NProto::TConsumer GetConsumer(const std::string& login);
    void UpdateConsumer(const NProto::TConsumer& consumer);
    void CreateConsumer(const NProto::TConsumer& consumer);

    NProto::TProduct GetProduct(const std::string& name);
    void UpdateProduct(const NProto::TProduct& user);
    void CreateProduct(const NProto::TProduct& user);

private:
    TConnectionPtr Connection_;

};

}
