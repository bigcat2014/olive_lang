#include <regex>

#include <pimento/utils.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>  // NOLINT(misc-include-cleaner)

namespace pimento::utils {

spdlog::logger& getLogger() noexcept
{
    static const auto SLogger = spdlog::stderr_color_mt("pimento");
    return *SLogger;
}

void configureLogger(spdlog::level::level_enum level) noexcept
{
    getLogger().set_level(level);
}

std::filesystem::path expandVars(const std::string& inputPath) noexcept
{
    auto& logger = getLogger();

    std::string path = inputPath;

    // Expand ~ at the start
    if (!path.empty() && path[0] == '~') {
        const char* home = std::getenv("HOME");
        if (home == nullptr) {
            logger.error("Unable to determine home directory.");
            exit(EXIT_FAILURE);
        }
        logger.debug("Expanded `~` to: {}", home);

        if (path == "~") {
            path = home;
        }
        else if (path[1] == '/') {
            path = std::string(home) + path.substr(1);  // replace "~" prefix
        }
        else {
            logger.error("Unsupported ~ expansion (e.g. ~username is not supported).");
            exit(EXIT_FAILURE);
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
            logger.error("Environment variable not set: ${}", varName);
            exit(EXIT_FAILURE);
        }

        result.append(value);
        searchStart = match[0].second;
    }

    result.append(searchStart, path.cend());
    logger.debug("Expanded path: {}", result);

    return {result};
}

std::filesystem::path sanitizePath(const std::string& inputPath) noexcept
{
    return std::filesystem::canonical(expandVars(inputPath));
}

bool fileExists(const std::filesystem::path& path)
{
    auto& logger = getLogger();

    try {
        if (!std::filesystem::exists(path)) {
            logger.error("File does not exist: {}", path.string());
            return false;
        }

        if (!std::filesystem::is_regular_file(path)) {
            logger.error("Path is not a regular file: {}", path.string());
            return false;
        }

        if (path.extension() != ".oil") {
            logger.warn("File is not a .oil file.");
        }
    } catch (const std::filesystem::filesystem_error& e) {
        logger.error("Filesystem error: {}", e.what());
        return false;
    }

    return true;
}

}  // namespace pimento::utils