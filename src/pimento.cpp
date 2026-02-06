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

int main(int argc, char* argv[])
{
    argparse::ArgumentParser program("pimento", PROJECT_VERSION);

    program.add_argument("file").help("Path to the .oil file to compile").required();

    auto& group = program.add_mutually_exclusive_group();
    group.add_argument("-o", "--output")
        .help("Path for assembly output file. Cannot be combined with `--stdout`.")
        .nargs(1)
        .default_value(std::string{"out.asm"});

    group.add_argument("--stdout")
        .default_value(false)
        .implicit_value(true)
        .nargs(0)
        .help("Output to stdout. Cannot be combined with `-o` | `--output`.");

    program.add_argument("--trace").default_value(false).implicit_value(true).nargs(0).help(
        "Enable trace logging. WARNING: This will spam output!");

    program.add_argument("--debug").default_value(false).implicit_value(true).nargs(0).help("Enable debug logging");

    program.add_argument("--verbose").default_value(false).implicit_value(true).nargs(0).help("Enable verbose logging");

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception& err) {
        std::cerr << "Argument parsing error: " << err.what() << '\n';
        std::cerr << program;
        return EXIT_FAILURE;
    }

    std::string inFileStr;
    std::string outFileStr;
    bool toStdout;

    try {
        // Set log level based on flags
        if (program.get<bool>("--trace")) {
            pimento::utils::configureLogger(spdlog::level::trace);
        }
        else if (program.get<bool>("--debug")) {
            pimento::utils::configureLogger(spdlog::level::debug);
        }
        else if (program.get<bool>("--verbose")) {
            pimento::utils::configureLogger(spdlog::level::info);
        }
        else {
            pimento::utils::configureLogger(spdlog::level::warn);
        }

        // Get additional cli arguments
        inFileStr  = program.get<std::string>("file");
        outFileStr = program.get<std::string>("output");
        toStdout   = program.get<bool>("--stdout");
    } catch (const std::exception& err) {
        std::cerr << "Argument retrieval error: " << err.what() << '\n';
        return EXIT_FAILURE;
    }

    auto& logger = pimento::utils::getLogger();

    logger.debug("Input file path: {}", inFileStr);

    auto inputResolvedPath = pimento::utils::sanitizePath(inFileStr);
    if (!inputResolvedPath.has_value()) {
        logger.error("Cannot resolve path {}", inFileStr);
        return EXIT_FAILURE;
    }
    logger.debug("Sanitized input file path: {}", inputResolvedPath.value().string());

    std::ifstream inputFile{inputResolvedPath.value()};
    if (!inputFile) {
        logger.error("Cannot open {}", inputResolvedPath.value().string());
        return EXIT_FAILURE;
    }

    if (toStdout) {
        pimento::generation::Generator generator(inputFile, std::cout);
        generator.generate();
    }
    else {
        logger.debug("Output file path: {}", outFileStr);

        auto outputResolvedPath = pimento::utils::expandVars(outFileStr);
        outputResolvedPath      = std::filesystem::absolute(outputResolvedPath);
        outputResolvedPath      = outputResolvedPath.lexically_normal();
        logger.debug("Sanitized output file path: {}", outputResolvedPath.string());

        std::ofstream outputFile{outputResolvedPath};
        if (!outputFile) {
            logger.error("Cannot open {}", outputResolvedPath.string());
            return EXIT_FAILURE;
        }

        pimento::generation::Generator generator(inputFile, outputFile);
        generator.generate();
    }

    // system("nasm -felf64 out.asm");
    // system("ld -o out out.o");

    return EXIT_SUCCESS;
}