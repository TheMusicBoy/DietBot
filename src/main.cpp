#include <fstream>
#include <sstream>
#include <string>
#include <service/service.h>
#include <common/exception.h>
#include <common/logger.h>
#include <google/protobuf/util/json_util.h>

std::string readFile(const std::string& fileName) {
    std::ifstream f(fileName);
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

int main() {
    auto Logger = std::make_shared<NDietBot::TLogger>("Main");

    std::string configStr = readFile("config.txt");

    LOG_INFO("Got config: {}", configStr);

    NDietBot::NProto::TConfig config;
    THROW_UNLESS(google::protobuf::json::JsonStringToMessage(configStr, &config).ok(), "Config has invalid format.");
    
    NDietBot::TService service(config);

    auto process = service.Start();

    THROW_IF(process.get(), "Process returned 1.");
}
