/// @file errors.hpp
/// @brief Pimento error handling.
/// @author Logan Thomas

#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include <pimento/utils.hpp>

namespace pimento::errors {

/// @brief Supported error types.
enum class ErrorType : uint8_t
{
    INVALID_TOKEN_ERROR,
    SYMBOL_ERROR,
};

/// @brief Static utility class for interacting with error types.
class ErrorTypeUtil
{
public:
    ErrorTypeUtil() = delete;

    /// @brief Get a human-readable string representing the error type.
    /// @param errorType The error type for which to return the string.
    /// @return The human-readable string representation of the error.
    [[nodiscard]] static std::string getErrorTypeStr(ErrorType errorType)
    {
        try {
            return getErrorStrMap().at(errorType);
        } catch (const std::out_of_range&) {
            return "Unknown Error";
        }
    }

private:
    /// @brief Type alias for the map between error types and their string representations.
    using ErrorStrMap = std::unordered_map<ErrorType, std::string>;

    /// @brief Get the map from error types to human-readable strings.
    [[nodiscard]] static ErrorStrMap getErrorStrMap() noexcept
    {
        static const ErrorStrMap TokenStr{{ErrorType::INVALID_TOKEN_ERROR, "Invalid Token Error"},
                                          {ErrorType::SYMBOL_ERROR, "Symbol Error"}};

        return TokenStr;
    }
};

/// @brief Error struct representing errors encountered during the compilation process.
struct Error
{
    /// @brief The type of the error.
    ErrorType type;
    /// @brief The line number at which the error occured.
    size_t line;
    /// @brief The column number at which the error occured.
    size_t column;
    /// @brief The error message describing the error.
    std::string msg;
};

/// @brief Raise an error by logging it and exiting the program.
/// @param error The error to raise.
inline void raise(const Error& error)
{
    auto& logger = pimento::utils::getLogger();

    logger.error(
        "Line: {} Column: {}: {}. {}", error.line, error.column, ErrorTypeUtil::getErrorTypeStr(error.type), error.msg);
    exit(EXIT_FAILURE);
}

}  // namespace pimento::errors