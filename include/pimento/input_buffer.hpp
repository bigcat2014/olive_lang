/// @file input_buffer.hpp
/// @brief Pimento file buffer.
/// @author Logan Thomas

#pragma once

#include <array>
#include <istream>
#include <string>

namespace pimento::tokenization {

/// @brief Class responsible for buffering input from a stream and providing utility functions for interacting with the
/// buffered input.
class InputBuffer
{
public:
    /// @brief The size in bytes of the internal buffer.
    static constexpr size_t BUFFER_SIZE = 4096;
    /// @brief The size in bytes of the chunks read from the stream.
    static constexpr size_t READ_SIZE = BUFFER_SIZE - 1;

public:
    /// @brief Default constructor for InputBuffer;
    InputBuffer()
        : mDone(true)
    {}

    /// @brief Constructor for the InputBuffer
    /// @param istream The stream of characters to buffer.
    explicit InputBuffer(std::istream* istream);

    /// @brief Peek at the current character in the buffer without consuming it.
    /// @return The current character in the buffer.
    [[nodiscard]] char peek() const noexcept;

    /// @brief Peek at the next character in the buffer without consuming it.
    /// @return The next character in the buffer.
    [[nodiscard]] char peekNext() noexcept;

    /// @brief Get the current character and advance the buffer.
    /// @return The current character in the buffer.
    [[nodiscard]] char consume();

    /// @brief Get the current number of bytes read from input stream.
    /// @return Current number of bytes read from input stream.
    [[nodiscard]] size_t getTotalBytes() const { return mTotalBytes; }

    /// @brief Get the current number of chunks read from input stream.
    /// @return Current number of chunks read from input stream.
    [[nodiscard]] size_t getTotalChunks() const { return mTotalChunks; }

    /// @brief Get the current offset in the input stream.
    /// @return Current offset in the input stream.
    [[nodiscard]] size_t getOffset() const { return mOffset; }

    /// @brief Get the current line of the input stream.
    /// @return The current line.
    [[nodiscard]] size_t getCurrentLine() const { return mLine; }

    /// @brief Get the current column in the current line.
    /// @return The current column in the current line.
    [[nodiscard]] size_t getCurrentColumn() const { return mColumn; }

    /// @brief Whether or not we are done traversing the input stream.
    /// @return True if done, false if there is more data.
    [[nodiscard]] bool done() const { return mDone; }

    /// @brief Get characters from the input stream.
    /// @param offset The offset of the input stream to start at.
    /// @param span The number of characters to get.
    /// @return String of characters from the input stream starting at `offset` and grabbing `span` number of
    /// characters.
    [[nodiscard]] std::string get(size_t offset, size_t span = 1);

private:
    /// @brief Read one chunk of BUFFER_SIZE from the input stream.
    void readChunk();

private:
    /// @brief The input stream to tokenize.
    std::istream* mStream;
    /// @brief The buffer to read characters into.
    std::array<char, BUFFER_SIZE> mBuffer;
    /// @brief The number of characters read into the buffer.
    size_t mNumChars{0};
    /// @brief Total bytes read from the input stream.
    size_t mTotalBytes{0};
    /// @brief Total chunks of size BUFFER_SIZE read from the input stream.
    size_t mTotalChunks{0};
    /// @brief Current index in the buffer.
    size_t mIndex{0};
    /// @brief The current line of the input stream.
    size_t mLine{0};
    /// @brief The current column of the current line of the input stream.
    size_t mColumn{0};
    /// @brief The current offset in the input stream.
    size_t mOffset{0};
    /// @brief Whether or not we are done traversing the input stream.
    bool mDone{false};
};

}  // namespace pimento::tokenization