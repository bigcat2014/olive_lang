//! @file utils.hpp
//! @brief Pimento utility functions.
//! @author Logan Thomas

#pragma once

#include <filesystem>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace pimento::utils {

//! @brief Gets the global logger.
//! @return spdlog::logger& Global logger.
spdlog::logger &get_logger() {
  static auto logger = spdlog::stderr_color_mt("pimento");
  return *logger;
}

//! @brief Configures the logger.
//! @param level spdlog::level::level_enum The log level to configure the logger
//! with.
void configure_logger(spdlog::level::level_enum level) {
  get_logger().set_level(level);
}

//! @brief Expands `~` and environment variables in input path.
//! @param input_path const std::string& The file path in which to expand the
//! variables.
//! @return std::filesystem::path The file path with variables expanded.
std::filesystem::path expand_vars(const std::string &input_path) {
  auto &logger = get_logger();

  std::string path = input_path;

  // Expand ~ at the start
  if (!path.empty() && path[0] == '~') {
    const char *home = std::getenv("HOME");
    if (home == nullptr) {
      throw std::runtime_error("Unable to determine home directory.");
    }
    logger.debug("Expanded `~` to: {}", home);

    if (path == "~") {
      path = home;
    } else if (path[1] == '/') {
      path = std::string(home) + path.substr(1); // replace "~" prefix
    } else {
      throw std::runtime_error(
          "Unsupported ~ expansion (e.g. ~username is not supported).");
    }
  }

  // Expand environment variables
  std::regex env_pattern(R"(\$([A-Za-z_][A-Za-z0-9_]*)|\$\{([^}]+)\})");
  std::smatch match;
  std::string result;
  std::string::const_iterator search_start(path.cbegin());

  while (std::regex_search(search_start, path.cend(), match, env_pattern)) {
    result.append(search_start, match[0].first);

    std::string var_name = match[1].matched ? match[1].str() : match[2].str();
    const char *value = std::getenv(var_name.c_str());

    if (!value) {
      throw std::runtime_error("Environment variable not set: $" + var_name);
    }

    result.append(value);
    search_start = match[0].second;
  }

  result.append(search_start, path.cend());
  logger.debug("Expanded path: {}", result);

  return std::filesystem::path(result);
}

//! @brief Sanitize the input path.
//!
//! Sanitize the input path by expanding `~` and environment variables,
//! normalizing the path, and ensuring it is a file. Additionally checks the
//! file extension and warns the user if the extension is not the expected
//! `.oil` extension.
//! @param input_path const std::string& The file path to sanitize.
//! @return std::optional<std::filesystem::path> The sanitized path.
std::optional<std::filesystem::path>
sanitize_path(const std::string &input_path) {
  auto &logger = get_logger();

  std::filesystem::path resolved_path = expand_vars(input_path);
  try {
    resolved_path = std::filesystem::canonical(resolved_path);

    if (!std::filesystem::exists(resolved_path)) {
      logger.error("File does not exist: {}", resolved_path.string());
      return {};
    }

    if (!std::filesystem::is_regular_file(resolved_path)) {
      logger.error("Path is not a regular file: {}", resolved_path.string());
      return {};
    }

    if (resolved_path.extension() != ".oil") {
      logger.warn("File is not a .oil file.");
    }
  } catch (const std::filesystem::filesystem_error &e) {
    logger.error("Filesystem error: {}", e.what());
    return {};
  }

  return resolved_path;
}
} // namespace pimento::utils