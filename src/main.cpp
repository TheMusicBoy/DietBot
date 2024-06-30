#include <iostream>
#include <string>
#include <output/config.pb.h>
#include <google/protobuf/util/json_util.h>

int main() {
    NDietBot::NProto::TConfig config;
    config.set_service_name("Some Service");

    std::string output;
    if (!google::protobuf::json::MessageToJsonString(config, &output).ok())
        std::cout << "Something bad!" << std::endl;

    std::cout << output << std::endl;
}
