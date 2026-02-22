/// @file utils.hpp
/// @brief Pimento utility functions.
/// @author Logan Thomas

#pragma once

#include <filesystem>
#include <string>

#include <spdlog/spdlog.h>

namespace pimento::utils {

/// @brief Gets the global logger.
/// @return Global logger.
spdlog::logger& getLogger();

/// @brief Configures the logger.
/// @param level The log level to configure the logger with.
void configureLogger(spdlog::level::level_enum level) noexcept;

/// @brief Expands `~` and environment variables in input path.
/// @param inputPath The file path in which to expand the variables.
/// @return The file path with variables expanded.
std::filesystem::path expandVars(const std::string& inputPath);

/// @brief Sanitize the input path.
/// @details Sanitize the input path by expanding `~` and environment variables and producing the canonical path.
/// @param inputPath The file path to sanitize.
/// @return The sanitized path.
std::filesystem::path sanitizePath(const std::string& inputPath) noexcept;

/// @brief Check if a file exists and is a regular file.
/// @details Check if a file exists and is a regular file. Additionally checks the file extension and warns the user if
/// the extension is not the expected `.oil` extension.
/// @param path The file path to check.
/// @return True if the file exists and is a regular file, false otherwise.
bool fileExists(const std::filesystem::path& path);

}  // namespace pimento::utils