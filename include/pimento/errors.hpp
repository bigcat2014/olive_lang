//! @file errors.hpp
//! @brief Pimento error handling.
//! @author Logan Thomas

#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

#include <pimento/utils.hpp>

namespace pimento::errors {

enum class ErrorType
{
    INVALID_TOKEN_ERROR,
    SYMBOL_ERROR,
};

//! @brief Static utility class for interacting with error types.
class ErrorTypeUtil
{
public:
    ErrorTypeUtil() = delete;

    //! @brief Get a human-readable string representing the error type.
    //! @param error_type The error type for which to return the string.
    //! @return The human-readable string representation of the error.
    [[nodiscard]] static std::string getErrorTypeStr(ErrorType errorType)
    {
        try {
            return getErrorStrMap().at(errorType);
        } catch (const std::out_of_range&) {
            return "Unknown Error";
        }
    }

private:
    using ErrorStrMap = std::unordered_map<ErrorType, std::string>;

    //! @brief Get the map from error types to human-readable strings.
    [[nodiscard]] static ErrorStrMap getErrorStrMap() noexcept
    {
        // clang-format off
    static const ErrorStrMap TokenStr{
      {ErrorType::INVALID_TOKEN_ERROR, "Invalid Token Error"},
      {ErrorType::SYMBOL_ERROR,        "Symbol Error"}
    };
        // clang-format on

        return TokenStr;
    }
};

struct Error
{
    ErrorType type;
    size_t line;
    size_t column;
    std::string msg;
};

inline void raise(const Error& error)
{
    auto& logger = pimento::utils::getLogger();

    logger.error(
        "Line: {} Column: {}: {}. {}", error.line, error.column, ErrorTypeUtil::getErrorTypeStr(error.type), error.msg);
    exit(EXIT_FAILURE);
}

}  // namespace pimento::errors