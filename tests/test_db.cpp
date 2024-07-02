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
        EXPECT_EQ(consumerA.id(), consumerB.id());
        EXPECT_EQ(consumerA.purpose(), consumerB.purpose());
        EXPECT_EQ(consumerA.birthday().seconds(), consumerB.birthday().seconds());
        EXPECT_EQ(consumerA.height(), consumerB.height());
        EXPECT_EQ(consumerA.weight(), consumerB.weight());
        EXPECT_EQ(consumerA.activity(), consumerB.activity());
    };

    consumerA.set_id(444);
    consumerA.set_purpose(1);
    consumerA.mutable_birthday()->set_seconds(2);
    consumerA.set_height(3);
    consumerA.set_weight(4);
    consumerA.set_activity(5);

    {
        auto tx = client->StartTransaction();
        EXPECT_TRUE(!client->CreateConsumer(consumerA, tx).get());
        tx->commit();
    }
    {
        auto tx = client->StartTransaction();
        consumerB = client->GetConsumer(consumerA.id(), tx).get();
        tx->commit();
        expectEq();
    }

    consumerA.set_purpose(2);
    consumerA.mutable_birthday()->set_seconds(3);
    consumerA.set_height(4);
    consumerA.set_weight(5);
    consumerA.set_activity(6);

    {
        auto tx = client->StartTransaction();
        EXPECT_TRUE(!client->UpdateConsumer(consumerA, tx).get());
        tx->commit();
    }
    {
        auto tx = client->StartTransaction();
        consumerB = client->GetConsumer(consumerA.id(), tx).get();
        tx->commit();
        expectEq();
    }

    {
        auto tx = client->StartTransaction();
        EXPECT_TRUE(!client->DeleteConsumer(consumerA.id(), tx).get());
        tx->commit();
    }
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

    {
        auto tx = client->StartTransaction();
        EXPECT_TRUE(!client->CreateProduct(productA, tx).get());
        tx->commit();
    }
    {
        auto tx = client->StartTransaction();
        productB = client->GetProduct(productA.name(), tx).get();
        tx->commit();
        expectEq();
    }

    productA.set_kalories(2);
    productA.set_protein(3);
    productA.set_fats(4);
    productA.set_carbohydrates(5);

    {
        auto tx = client->StartTransaction();
        EXPECT_TRUE(!client->UpdateProduct(productA, tx).get());
        tx->commit();
    }
    {
        auto tx = client->StartTransaction();
        productB = client->GetProduct(productA.name(), tx).get();
        tx->commit();
        expectEq();
    }

    {
        auto tx = client->StartTransaction();
        EXPECT_TRUE(!client->DeleteProduct(productA.name(), tx).get());
        tx->commit();
    }
}

TEST_F(DbClientTest, TestChatInfo) {
    NDietBot::NProto::TChatInfo chatInfoA, chatInfoB;
    auto expectEq = [&]() {
        EXPECT_EQ(chatInfoA.id(), chatInfoB.id());
        EXPECT_EQ(chatInfoA.status(), chatInfoB.status());
    };

    chatInfoA.set_id(444);
    chatInfoA.set_status(NDietBot::NProto::EChatStatus::Start);

    {
        auto tx = client->StartTransaction();
        EXPECT_TRUE(!client->CreateChatInfo(chatInfoA, tx).get());
        tx->commit();
    }
    {
        auto tx = client->StartTransaction();
        chatInfoB = client->GetChatInfo(chatInfoA.id(), tx).get();
        tx->commit();
        expectEq();
    }

    chatInfoA.set_status(NDietBot::NProto::EChatStatus::PurposeAsk);

    {
        auto tx = client->StartTransaction();
        EXPECT_TRUE(!client->UpdateChatInfo(chatInfoA, tx).get());
        tx->commit();
    }
    {
        auto tx = client->StartTransaction();
        chatInfoB = client->GetChatInfo(chatInfoA.id(), tx).get();
        tx->commit();
        expectEq();
    }

    {
        auto tx = client->StartTransaction();
        EXPECT_TRUE(!client->DeleteChatInfo(chatInfoA.id(), tx).get());
        tx->commit();
    } }
