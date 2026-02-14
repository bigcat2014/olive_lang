/// @file file_stream.hpp
/// @brief Pimento file handler.
/// @author Logan Thomas

#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

namespace pimento::tokenization {
template <typename T>
concept IOStream = std::derived_from<T, std::istream> || std::derived_from<T, std::ostream>;

/// @brief Smart deleter for input and output streams.
/// @details Smart deleter that handles both global streams (cin, cout, cerr) and dynamically allocated streams.
/// Global streams are not deleted, while dynamically allocated streams are properly cleaned up.
/// @tparam StreamType The type of stream (std::istream for input, std::ostream for output).
template <IOStream StreamType>
struct SmartDeleter
{
    /// @brief Custom deleter function.
    void operator()(StreamType* ptr) const noexcept
    {
        if (ptr == nullptr) {
            return;
        }

        // Do not delete global streams (cin, cout, cerr)
        if constexpr (std::is_same_v<StreamType, std::istream>) {
            if (ptr == &std::cin) {
                return;
            }
        }
        else if constexpr (std::is_same_v<StreamType, std::ostream>) {
            if (ptr == &std::cout || ptr == &std::cerr) {
                return;
            }
        }

        // Delete dynamically allocated streams
        delete ptr;
    }
};

/// @brief Traits class to associate stream types with their corresponding file stream types and type names.
/// @tparam StreamType The type of stream (std::istream for input, std::ostream for output).
template <IOStream StreamType>
struct StreamTraits;

/// @brief Specialization of StreamTraits for std::istream.
template <>
struct StreamTraits<std::istream>
{
    /// @brief Type alias for the corresponding file stream type (std::ifstream).
    using FileStreamType = std::ifstream;
};

/// @brief Specialization of StreamTraits for std::ostream.
template <>
struct StreamTraits<std::ostream>
{
    /// @brief Type alias for the corresponding file stream type (std::ofstream).
    using FileStreamType = std::ofstream;
};

template <IOStream StreamType>
class FileStream;

/// @brief Type alias for input file stream.
using InputStream = FileStream<std::istream>;

/// @brief Type alias for output file stream.
using OutputStream = FileStream<std::ostream>;

/// @brief FileStream class template that manages file streams with smart pointers and custom deleters.
/// @tparam StreamType The type of stream (std::istream for input, std::ostream for output).
template <IOStream StreamType>
class FileStream
{
public:
    /// @brief Type alias for the unique pointer to the stream with a smart deleter.
    using StreamPtr = std::unique_ptr<StreamType, SmartDeleter<StreamType>>;
    /// @brief Type alias for the unique pointer to the input stream with a smart deleter.
    using InputStreamPtr = std::unique_ptr<std::istream, SmartDeleter<std::istream>>;
    /// @brief Type alias for the unique pointer to the output stream with a smart deleter.
    using OutputStreamPtr = std::unique_ptr<std::ostream, SmartDeleter<std::ostream>>;

public:
    /// @brief Factory method to create a FileStream from stdin.
    static InputStream makeFromStdin() { return FileStream(InputStreamPtr(&std::cin, SmartDeleter<std::istream>{})); }

    /// @brief Factory method to create a FileStream from stdout.
    static OutputStream makeFromStdout()
    {
        return FileStream(OutputStreamPtr(&std::cout, SmartDeleter<std::ostream>{}));
    }

    /// @brief Factory method to create a FileStream from stderr.
    static OutputStream makeFromStderr()
    {
        return FileStream(OutputStreamPtr(&std::cerr, SmartDeleter<std::ostream>{}));
    }

    /// @brief Factory method to create a FileStream from a file path (input or output).
    /// @param filePath The path to the file to be opened.
    static FileStream<StreamType> makeFromPath(const std::filesystem::path& filePath)
    {
        using FileStreamType = typename StreamTraits<StreamType>::FileStreamType;
        auto* fileStream     = new FileStreamType(filePath);
        if (!fileStream->is_open()) {
            delete fileStream;
            return FileStream(nullptr);
        }
        return FileStream(StreamPtr(fileStream, SmartDeleter<StreamType>{}));
    }

    /// @brief Overloaded bool operator to check if the stream is valid and good for I/O operations.
    explicit operator bool() const noexcept { return mStream && mStream->good(); }

    /// @brief Get a reference to the underlying stream.
    [[nodiscard]] StreamType& getStream() noexcept { return *mStream; }

    /// @brief Get a const reference to the underlying stream.
    [[nodiscard]] const StreamType& getStream() const noexcept { return *mStream; }

private:
    /// @brief Private constructor that takes a unique pointer to the stream with a smart deleter.
    /// @param stream Unique pointer to the stream with a smart deleter.
    explicit FileStream(StreamPtr stream)
        : mStream(std::move(stream))
    {}

private:
    /// @brief Unique pointer to the stream with smart deleter.
    StreamPtr mStream;
};

/// @brief Convenience factory function for creating an InputStream from stdin.
static InputStream makeFileStreamFromStdin()
{
    return InputStream::makeFromStdin();
}

/// @brief Convenience factory function for creating an OutputStream from stdout.
static OutputStream makeFileStreamFromStdout()
{
    return OutputStream::makeFromStdout();
}

/// @brief Convenience factory function for creating an OutputStream from stderr.
static OutputStream makeFileStreamFromStderr()
{
    return OutputStream::makeFromStderr();
}

/// @brief Convenience factory function for creating an InputStream from a file path.
/// @param filePath The path to the input file to be opened.
static InputStream makeInputStreamFromPath(const std::filesystem::path& filePath)
{
    return InputStream::makeFromPath(filePath);
}

/// @brief Convenience factory function for creating an OutputStream from a file path.
/// @param filePath The path to the output file to be opened.
static OutputStream makeOutputStreamFromPath(const std::filesystem::path& filePath)
{
    return OutputStream::makeFromPath(filePath);
}

}  // namespace pimento::tokenization