#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <argparse/argparse.hpp>
#include <filesystem>
#include <iostream>
#include <optional>
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

std::filesystem::path expandUserHome(const std::string& path) {
  auto logger = getLogger();

  if (!path.empty() && path[0] == '~') {
#ifdef _WIN32
    const char* home = std::getenv("USERPROFILE");
#else
    const char* home = std::getenv("HOME");
#endif
    if (!home) {
      throw std::runtime_error("Unable to determine home directory.");
    }

    std::filesystem::path expanded = std::filesystem::path(home);
    logger.debug("Expanded '~' to home: {}", expanded.string());

    if (path == "~") {
      return expanded;
    } else if (path[1] == '/') {
      std::filesystem::path subPath = path.substr(2);  // strip "~/" prefix
      expanded = expanded / subPath;
      logger.debug("Expanded entire path to: {}", expanded.string());
      return expanded;
    } else {
      throw std::runtime_error(
          "Unsupported ~ expansion (e.g. ~username is not supported).");
    }
  }

  return std::filesystem::path(path);
}

std::optional<std::filesystem::path> sanitizePath(const std::string& path) {
  auto logger = getLogger();

  std::filesystem::path resolvedPath = expandUserHome(path);
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