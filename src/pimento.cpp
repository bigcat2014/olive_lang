//! @file pimento.cpp
//! @brief Pimento executable
//! @author Logan Thomas

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include <argparse/argparse.hpp>
#include <pimento/generator.hpp>
#include <pimento/utils.hpp>

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "unknown"
#endif

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("pimento", PROJECT_VERSION);

  program.add_argument("file")
      .help("Path to the .oil file to compile")
      .required();

  program.add_argument("-o", "--output")
      .help("Path for assembly output file")
      .nargs(1)
      .default_value(std::string{"out.asm"});

  program.add_argument("--trace")
      .default_value(false)
      .implicit_value(true)
      .nargs(0)
      .help("Enable trace logging. WARNING: This will spam output!");

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
  } catch (const std::runtime_error &err) {
    std::cerr << "Argument parsing error: " << err.what() << '\n';
    std::cerr << program;
    return EXIT_FAILURE;
  }

  // Set log level based on flags
  if (program.get<bool>("--trace")) {
    pimento::utils::configure_logger(spdlog::level::trace);
  } else if (program.get<bool>("--debug")) {
    pimento::utils::configure_logger(spdlog::level::debug);
  } else if (program.get<bool>("--verbose")) {
    pimento::utils::configure_logger(spdlog::level::info);
  } else {
    pimento::utils::configure_logger(spdlog::level::warn);
  }

  auto &logger = pimento::utils::get_logger();

  std::string in_file_str = program.get<std::string>("file");
  logger.debug("Input file path: {}", in_file_str);

  auto input_resolved_path = pimento::utils::sanitize_path(in_file_str);
  if (!input_resolved_path.has_value()) {
    return EXIT_FAILURE;
  }
  logger.debug("Sanitized input file path: {}",
               input_resolved_path.value().string());

  std::string out_file_str = program.get<std::string>("output");
  logger.debug("Output file path: {}", out_file_str);

  auto output_resolved_path = pimento::utils::expand_vars(out_file_str);
  output_resolved_path = std::filesystem::absolute(output_resolved_path);
  output_resolved_path = output_resolved_path.lexically_normal();
  logger.debug("Sanitized output file path: {}", output_resolved_path.string());

  std::shared_ptr<std::fstream> input_file =
      std::make_shared<std::fstream>(input_resolved_path.value(), std::ios::in);
  if (!input_file) {
    throw std::runtime_error("cannot open " +
                             input_resolved_path.value().string());
  }
  std::shared_ptr<std::fstream> output_file =
      std::make_shared<std::fstream>(output_resolved_path, std::ios::out);
  if (!output_file) {
    throw std::runtime_error("cannot open " + output_resolved_path.string());
  }

  pimento::generation::Generator generator(input_file, output_file);
  generator.generate();

  // system("nasm -felf64 out.asm");
  // system("ld -o out out.o");

  return EXIT_SUCCESS;
}