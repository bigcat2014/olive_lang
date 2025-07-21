//! @file pimento.cpp
//! @brief Pimento executable
//! @author Logan Thomas

#include <argparse/argparse.hpp>
#include <filesystem>
#include <iostream>
#include <optional>
#include <pimento/utils.hpp>
#include <string>

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "unknown"
#endif

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
    pimento::utils::configure_logger(spdlog::level::debug);
  } else if (program.get<bool>("--verbose")) {
    pimento::utils::configure_logger(spdlog::level::info);
  } else {
    pimento::utils::configure_logger(spdlog::level::warn);
  }

  auto& logger = pimento::utils::get_logger();

  std::string fileStr = program.get<std::string>("file");
  logger.debug("Input file path: {}", fileStr);

  auto resolvedPath = pimento::utils::sanitize_path(fileStr);
  if (!resolvedPath.has_value()) {
    return EXIT_FAILURE;
  }

  logger.debug("Sanitized file path: {}", resolvedPath.value().string());

  return EXIT_SUCCESS;
}