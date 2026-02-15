/// @file input_buffer_test.cpp
/// @brief Pimento input buffer unit tests
/// @author Logan Thomas

#include <gtest/gtest.h>
#include <sstream>

#include <pimento/input_buffer.hpp>

namespace pimento::tokenization::testing {

class InputBufferTestFixture : public ::testing::Test
{
protected:
    InputBuffer mBuffer;
    std::istringstream mIss;
    std::string mStr;

    void SetUp() override
    {
        mIss    = std::istringstream("Hello, world!");
        mStr    = mIss.str();
        mBuffer = InputBuffer(&mIss);
    }
};

TEST_F(InputBufferTestFixture, peek)
{
    auto value = mBuffer.peek();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), mStr[0]);
    EXPECT_EQ(mIss.str(), mStr);

    // Peek shouuld not consume
    value = mBuffer.peek();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), mStr[0]);
    EXPECT_EQ(mIss.str(), mStr);
}

TEST_F(InputBufferTestFixture, consume)
{
    auto value = mBuffer.consume();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), mStr[0]);
    EXPECT_EQ(mIss.str(), mStr);

    value = mBuffer.peek();

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), mStr[1]);
    EXPECT_EQ(mIss.str(), mStr);
}

TEST_F(InputBufferTestFixture, advance) {}

TEST_F(InputBufferTestFixture, getTotalBytes)
{
    EXPECT_EQ(mBuffer.getTotalBytes(), 13);
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