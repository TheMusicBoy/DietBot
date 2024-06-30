#include <gtest/gtest.h>
#include <string>
#include <output/message.pb.h>
#include <google/protobuf/util/json_util.h>

TEST(ProtoTest, MessageToJson) {
    NDietBot::NProtoTest::TTestMessage message;
    message.set_test_field("data");

    std::string output;

    EXPECT_TRUE(google::protobuf::json::MessageToJsonString(message, &output).ok());
    EXPECT_EQ(output, "{\"testField\":\"data\"}");
}
