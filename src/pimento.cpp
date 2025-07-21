#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <argparse/argparse.hpp>
#include <filesystem>
#include <iostream>
#include <optional>
#include <regex>
#include <stdexcept>
#include <string>

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "unknown"
#endif

spdlog::logger& getLogger() {
  static auto logger = spdlog::stderr_color_mt("pimento");
  return *logger;
}

void configureLogger(spdlog::level::level_enum level) {
  getLogger().set_level(level);
}

std::filesystem::path expandVars(const std::string& inputPath) {
  auto logger = getLogger();

  std::string path = inputPath;

  // Expand ~ at the start
  if (!path.empty() && path[0] == '~') {
    const char* home = std::getenv("HOME");
    if (home == nullptr) {
      throw std::runtime_error("Unable to determine home directory.");
    }
    logger.debug("Expanded `~` to: {}", home);

    if (path == "~") {
      path = home;
    } else if (path[1] == '/') {
      path = std::string(home) + path.substr(1);  // replace "~" prefix
    } else {
      throw std::runtime_error(
          "Unsupported ~ expansion (e.g. ~username is not supported).");
    }
  }

  // Expand environment variables
  std::regex envPattern(R"(\$([A-Za-z_][A-Za-z0-9_]*)|\$\{([^}]+)\})");
  std::smatch match;
  std::string result;
  std::string::const_iterator searchStart(path.cbegin());

  while (std::regex_search(searchStart, path.cend(), match, envPattern)) {
    result.append(searchStart, match[0].first);

    std::string varName = match[1].matched ? match[1].str() : match[2].str();
    const char* value = std::getenv(varName.c_str());

    if (!value) {
      throw std::runtime_error("Environment variable not set: $" + varName);
    }

    result.append(value);
    searchStart = match[0].second;
  }

  result.append(searchStart, path.cend());
  logger.debug("Expanded path: {} to {}", inputPath, result);

  return std::filesystem::path(result);
}

std::optional<std::filesystem::path> sanitizePath(const std::string& path) {
  auto logger = getLogger();

  std::filesystem::path resolvedPath = expandVars(path);
  try {
    resolvedPath = std::filesystem::canonical(resolvedPath);

    if (!std::filesystem::exists(resolvedPath)) {
      logger.error("File does not exist: {}", resolvedPath.string());
      return {};
    }

    if (!std::filesystem::is_regular_file(resolvedPath)) {
      logger.error("Path is not a regular file: {}", resolvedPath.string());
      return {};
    }

    if (resolvedPath.extension() != ".oil") {
      logger.warn("File is not a .oil file.");
    }
  } catch (const std::filesystem::filesystem_error& e) {
    logger.error("Filesystem error: {}", e.what());
    return {};
  }

  return resolvedPath;
}

int main(int argc, char* argv[]) {
  argparse::ArgumentParser program("pimento", PROJECT_VERSION);

  program.add_argument("file")
      .help("Path to the .oil file to compile")
      .required();

  program.add_argument("--debug")
      .default_value(false)
      .implicit_value(true)
      .nargs(0)
      .help("Enable debug logging");

  program.add_argument("--verbose")
      .default_value(false)
      .implicit_value(true)
      .nargs(0)
      .help("Enable verbose logging");

  try {
    program.parse_args(argc, argv);
  } catch (const std::runtime_error& err) {
    std::cerr << "Argument parsing error: " << err.what() << '\n';
    std::cerr << program;
    return EXIT_FAILURE;
  }

  // Set log level based on flags
  if (program.get<bool>("--debug")) {
    configureLogger(spdlog::level::debug);
  } else if (program.get<bool>("--verbose")) {
    configureLogger(spdlog::level::info);
  } else {
    configureLogger(spdlog::level::warn);
  }

  auto logger = getLogger();

  std::string fileStr = program.get<std::string>("file");
  logger.debug("Input file path: {}", fileStr);

  auto resolvedPath = sanitizePath(fileStr);
  if (!resolvedPath.has_value()) {
    return EXIT_FAILURE;
  }

  logger.debug("Sanitized input file path: {}", resolvedPath.value().string());

  return EXIT_SUCCESS;
}