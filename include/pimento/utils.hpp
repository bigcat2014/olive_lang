//! @file utils.hpp
//! @brief Pimento utility functions.
//! @author Logan Thomas

#pragma once

#include <filesystem>
#include <optional>
#include <string>

#include <spdlog/spdlog.h>

namespace pimento::utils {

//! @brief Gets the global logger.
//! @return Global logger.
spdlog::logger& get_logger();

//! @brief Configures the logger.
//! @param level The log level to configure the logger with.
void configure_logger(spdlog::level::level_enum level);

//! @brief Expands `~` and environment variables in input path.
//! @param input_path The file path in which to expand the variables.
//! @return The file path with variables expanded.
std::filesystem::path expand_vars(const std::string& input_path);

//! @brief Sanitize the input path.
//! @details Sanitize the input path by expanding `~` and environment variables,
//! normalizing the path, and ensuring it is a file. Additionally checks the
//! file extension and warns the user if the extension is not the expected
//! `.oil` extension.
//! @param input_path The file path to sanitize.
//! @return The sanitized path.
std::optional<std::filesystem::path> sanitize_path(const std::string& input_path);

}  // namespace pimento::utils