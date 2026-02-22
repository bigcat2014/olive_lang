/// @file input_buffer_test.cpp
/// @brief Pimento input buffer unit tests
/// @author Logan Thomas

#include <gtest/gtest.h>
#include <spdlog/common.h>
#include <sstream>

#include <pimento/input_buffer.hpp>
#include <pimento/utils.hpp>

namespace pimento::tokenization::testing {

class InputBufferTestFixture : public ::testing::Test
{
public:
    static void SetUpTestSuite()
    {
        pimento::utils::configureLogger(spdlog::level::trace);
        logger = utils::getLogger();
    }

protected:
    static inline auto& logger = utils::getLogger();

    InputBuffer mBuffer;
    std::istringstream mIss;
    std::string mStr;

    void SetUp() override
    {
        mIss    = std::istringstream("Hello, world!");
        mStr    = mIss.str();
        mBuffer = InputBuffer(&mIss);
    }

    void TearDown() override { logger.flush(); }
};

TEST_F(InputBufferTestFixture, peek)
{
    auto value = mBuffer.peek();
    EXPECT_EQ(value, mStr[0]);
    // Peek should not consume
    EXPECT_EQ(value, mBuffer.peek());
}

TEST_F(InputBufferTestFixture, consume)
{
    auto value = mBuffer.consume();
    EXPECT_EQ(value, mStr[0]);

    // Consume should consume the character and advance the buffer
    value = mBuffer.peek();
    EXPECT_EQ(value, mStr[1]);
}

TEST_F(InputBufferTestFixture, advance) {}

TEST_F(InputBufferTestFixture, getTotalBytes)
{
    EXPECT_EQ(mBuffer.getTotalBytes(), mStr.size());
}

TEST_F(InputBufferTestFixture, getTotalChunks)
{
    EXPECT_EQ(mBuffer.getTotalChunks(), 1);
}

TEST_F(InputBufferTestFixture, getOffset)
{
    EXPECT_EQ(mBuffer.getOffset(), 0);
}

TEST_F(InputBufferTestFixture, getCurrentLine)
{
    EXPECT_EQ(mBuffer.getCurrentLine(), 0);
}

TEST_F(InputBufferTestFixture, getCurrentColumn)
{
    EXPECT_EQ(mBuffer.getCurrentColumn(), 0);
}

TEST_F(InputBufferTestFixture, done)
{
    EXPECT_FALSE(mBuffer.done());
}

TEST_F(InputBufferTestFixture, get)
{
    EXPECT_EQ(mBuffer.get(1, 1), "e");
}

}  // namespace pimento::tokenization::testing