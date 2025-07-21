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

std::optional<std::filesystem::path> sanitizePath(const std::string& path) {
  auto logger = getLogger();
  std::filesystem::path resolvedPath;
  try {
    resolvedPath = std::filesystem::canonical(path);

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

  auto logger = getLogger();
  // Set log level based on flags
  if (program.get<bool>("--debug")) {
    logger.set_level(spdlog::level::debug);
  } else if (program.get<bool>("--verbose")) {
    logger.set_level(spdlog::level::info);
  } else {
    logger.set_level(spdlog::level::warn);
  }

  std::string fileStr = program.get<std::string>("file");
  logger.debug("Input file path: {}", fileStr);
  auto resolvedPath = sanitizePath(fileStr);
  if (!resolvedPath.has_value()) {
    return EXIT_FAILURE;
  }

  logger.debug("Sanitized input file path: {}", resolvedPath.value().string());

  return EXIT_SUCCESS;
}