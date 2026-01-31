#include <regex>
#include <stdexcept>

#include <pimento/utils.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace pimento::utils {

//! @brief Gets the global logger.
//! @return spdlog::logger& Global logger.
spdlog::logger& getLogger()
{
    static const auto SLogger = spdlog::stderr_color_mt("pimento");
    return *SLogger;
}

//! @brief Configures the logger.
//! @param level spdlog::level::level_enum The log level to configure the logger with.
void configureLogger(spdlog::level::level_enum level)
{
    getLogger().set_level(level);
}

//! @brief Expands `~` and environment variables in input path.
//! @param input_path const std::string& The file path in which to expand the variables.
//! @return std::filesystem::path The file path with variables expanded.
std::filesystem::path expandVars(const std::string& inputPath)
{
    auto& logger = getLogger();

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
        }
        else if (path[1] == '/') {
            path = std::string(home) + path.substr(1);  // replace "~" prefix
        }
        else {
            throw std::runtime_error("Unsupported ~ expansion (e.g. ~username is not supported).");
        }
    }

    // Expand environment variables
    std::regex const envPattern(R"(\$([A-Za-z_][A-Za-z0-9_]*)|\$\{([^}]+)\})");
    std::smatch match;
    std::string result;
    std::string::const_iterator searchStart(path.cbegin());

    while (std::regex_search(searchStart, path.cend(), match, envPattern)) {
        result.append(searchStart, match[0].first);

        std::string const varName = match[1].matched ? match[1].str() : match[2].str();
        const char* value         = std::getenv(varName.c_str());

        if (value == nullptr) {
            throw std::runtime_error("Environment variable not set: $" + varName);
        }

        result.append(value);
        searchStart = match[0].second;
    }

    result.append(searchStart, path.cend());
    logger.debug("Expanded path: {}", result);

    return {result};
}

//! @brief Sanitize the input path.
//!
//! Sanitize the input path by expanding `~` and environment variables, normalizing the path, and ensuring it is a file.
//! Additionally checks the file extension and warns the user if the extension is not the expected `.oil` extension.
//! @param input_path const std::string& The file path to sanitize.
//! @return std::optional<std::filesystem::path> The sanitized path.
std::optional<std::filesystem::path> sanitizePath(const std::string& inputPath)
{
    auto& logger = getLogger();

    std::filesystem::path resolvedPath = expandVars(inputPath);
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

}  // namespace pimento::utils