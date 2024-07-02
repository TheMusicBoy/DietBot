#include <gtest/gtest.h>
#include <db/client.h>

class QuickTest : public testing::Test {
 protected:
  void SetUp() override { start_time_ = time(nullptr); }

  void TearDown() override {
    const time_t end_time = time(nullptr);

    EXPECT_TRUE(end_time - start_time_ <= 5) << "The test took too long.";
  }

  time_t start_time_;
};

class DbClientTest : public QuickTest {
 protected:
    void SetUp() override {
        QuickTest::SetUp();

        NDietBot::NProto::TDataBaseConfig config;
        config.set_dbname("test_db");
        config.set_hostaddr("127.0.0.1");
        config.set_port("5432");
        config.set_user("diet_bot");
        config.set_password("123456");

        client = std::make_shared<NDietBot::TClient>(config);
    }
    
    std::shared_ptr<NDietBot::TClient> client;
};

TEST_F(DbClientTest, TestConsumer) {
    NDietBot::NProto::TConsumer consumerA, consumerB;
    auto expectEq = [&]() {
        EXPECT_EQ(consumerA.login(), consumerB.login());
        EXPECT_EQ(consumerA.purpose(), consumerB.purpose());
        EXPECT_EQ(consumerA.birthday().seconds(), consumerB.birthday().seconds());
        EXPECT_EQ(consumerA.height(), consumerB.height());
        EXPECT_EQ(consumerA.weight(), consumerB.weight());
        EXPECT_EQ(consumerA.activity(), consumerB.activity());
    };

    consumerA.set_login("test_consumer");
    consumerA.set_purpose(1);
    consumerA.mutable_birthday()->set_seconds(2);
    consumerA.set_height(3);
    consumerA.set_weight(4);
    consumerA.set_activity(5);

    EXPECT_TRUE(!client->CreateConsumer(consumerA).get());
    consumerB = client->GetConsumer("test_consumer").get();
    expectEq();

    consumerA.set_purpose(2);
    consumerA.mutable_birthday()->set_seconds(3);
    consumerA.set_height(4);
    consumerA.set_weight(5);
    consumerA.set_activity(6);

    EXPECT_TRUE(!client->UpdateConsumer(consumerA).get());
    consumerB = client->GetConsumer("test_consumer").get();
    expectEq();

    EXPECT_TRUE(!client->DeleteConsumer("test_consumer").get());
}

TEST_F(DbClientTest, TestProduct) {
    NDietBot::NProto::TProduct productA, productB;
    auto expectEq = [&]() {
        EXPECT_EQ(productA.name(), productB.name());
        EXPECT_EQ(productA.kalories(), productB.kalories());
        EXPECT_EQ(productA.protein(), productB.protein());
        EXPECT_EQ(productA.fats(), productB.fats());
        EXPECT_EQ(productA.carbohydrates(), productB.carbohydrates());
    };

    productA.set_name("test_product");
    productA.set_kalories(1);
    productA.set_protein(2);
    productA.set_fats(3);
    productA.set_carbohydrates(4);

    EXPECT_TRUE(!client->CreateProduct(productA).get());
    productB = client->GetProduct("test_product").get();
    expectEq();

    productA.set_kalories(2);
    productA.set_protein(3);
    productA.set_fats(4);
    productA.set_carbohydrates(5);

    EXPECT_TRUE(!client->UpdateProduct(productA).get());
    productB = client->GetProduct("test_product").get();
    expectEq();

    EXPECT_TRUE(!client->DeleteProduct("test_product").get());
}
