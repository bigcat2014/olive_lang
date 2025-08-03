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
//! @return spdlog::logger& Global logger.
spdlog::logger &get_logger();

//! @brief Configures the logger.
//! @param level spdlog::level::level_enum The log level to configure the logger
//! with.
void configure_logger(spdlog::level::level_enum level);

//! @brief Expands `~` and environment variables in input path.
//! @param input_path const std::string& The file path in which to expand the
//! variables.
//! @return std::filesystem::path The file path with variables expanded.
std::filesystem::path expand_vars(const std::string &input_path);

//! @brief Sanitize the input path.
//! @details Sanitize the input path by expanding `~` and environment variables,
//! normalizing the path, and ensuring it is a file. Additionally checks the
//! file extension and warns the user if the extension is not the expected
//! `.oil` extension.
//! @param input_path const std::string& The file path to sanitize.
//! @return std::optional<std::filesystem::path> The sanitized path.
std::optional<std::filesystem::path>
sanitize_path(const std::string &input_path);

} // namespace pimento::utils