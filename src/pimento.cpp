/// @file pimento.cpp
/// @brief Pimento executable
/// @author Logan Thomas

#include <cstdlib>
#include <exception>
#include <iostream>
#include <spdlog/common.h>
#include <string>

#include <argparse/argparse.hpp>
#include <pimento/file_stream.hpp>
#include <pimento/generator.hpp>
#include <pimento/utils.hpp>

#ifndef PROJECT_VERSION
#define PROJECT_VERSION "unknown"
#endif

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program("pimento", PROJECT_VERSION);

    std::string inFileStr;
    std::string outFileStr;
    bool toStdout;
    spdlog::level::level_enum logLevel = spdlog::level::warn;

    try {
        // clang-format off
        program.add_argument("file")
            .required()
            .help("Path to the .oil file to compile")
            .store_into(inFileStr);

        auto& outputGroup = program.add_mutually_exclusive_group();
        outputGroup.add_argument("-o", "--output")
            .default_value(std::string{"out.asm"})
            .nargs(1)
            .help("Path for assembly output file.")
            .store_into(outFileStr);

        outputGroup.add_argument("--stdout")
            .flag()
            .help("Output to stdout.")
            .store_into(toStdout);

        auto& logGroup = program.add_mutually_exclusive_group();
        logGroup.add_argument("--trace")
            .flag()
            .action([&](const auto&) { logLevel = spdlog::level::trace; })
            .help("Enable trace logging. WARNING: This will spam output!");

        logGroup.add_argument("--debug")
            .flag()
            .action([&](const auto&) { logLevel = spdlog::level::debug; })
            .help("Enable debug logging");

        logGroup.add_argument("--verbose")
            .flag()
            .action([&](const auto&) { logLevel = spdlog::level::info; })
            .help("Enable verbose logging");
        // clang-format on

        program.parse_args(argc, argv);

        // Set log level based on flags
        pimento::utils::configureLogger(logLevel);

    } catch (const std::exception& err) {
        std::cerr << "Argument retrieval error: " << err.what() << '\n';
        std::cerr << program << '\n';
        return EXIT_FAILURE;
    }

    auto& logger = pimento::utils::getLogger();
    logger.info("Pimento version {}", PROJECT_VERSION);
    logger.debug("Input file path: {}", inFileStr);

    // Sanitize and validate input file path
    auto inputResolvedPath = pimento::utils::sanitizePath(inFileStr);
    logger.debug("Sanitized input file path: {}", inputResolvedPath.string());
    if (!pimento::utils::fileExists(inputResolvedPath)) {
        logger.error("Input file check failed: {}", inFileStr);
        return EXIT_FAILURE;
    }

    // Open and validate input file stream
    pimento::tokenization::InputStream inputFile = pimento::tokenization::makeInputStreamFromPath(inputResolvedPath);
    if (!inputFile) {
        logger.error("Cannot open {}", inputResolvedPath.string());
        return EXIT_FAILURE;
    }

    // Sanitize output file path, open, and validate output file stream
    auto outputResolvedPath = pimento::utils::sanitizePath(outFileStr);
    pimento::tokenization::OutputStream outputFile
        = toStdout ? pimento::tokenization::makeFileStreamFromStdout()
                   : pimento::tokenization::makeOutputStreamFromPath(outputResolvedPath);
    if (!outputFile) {
        logger.error("Cannot open {}", outputResolvedPath.string());
        return EXIT_FAILURE;
    }

    if (toStdout) {
        logger.info("Outputting to stdout");
    }
    else {
        logger.debug("Output file path: {}", outFileStr);
        logger.debug("Sanitized output file path: {}", outputResolvedPath.string());
    }

    pimento::generation::Generator generator(inputFile.getStream(), outputFile.getStream());
    generator.generate();

    // system("nasm -felf64 out.asm");
    // system("ld -o out out.o");

    return EXIT_SUCCESS;
}