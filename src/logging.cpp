#include "logging.hpp"
#include <map>
#include <string>

std::map<std::string, LoggerContextObject> contextLoggers;
namespace PinkCore
{
    Logger& Logging::getLogger()
    {
        static Logger* logger = new Logger({MOD_ID, VERSION, VERSION_LONG}, LoggerOptions(false, true));
        return *logger;
    }

    LoggerContextObject& Logging::getContextLogger(const char* func, const char* file, int line)
    {
        std::string contextString(string_format("%s:%i", file, line));
        std::map<std::string, LoggerContextObject>::iterator it = contextLoggers.find(contextString);
        if (it != contextLoggers.end())
        {
            return it->second;
        }
        contextLoggers.emplace(contextString, getLogger().WithContext(contextString));
        return contextLoggers.find(contextString)->second;
    }
}
