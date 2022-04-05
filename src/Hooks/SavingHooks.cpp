#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"
#include "logging.hpp"
#include "beatsaber-hook/shared/config/config-utils.hpp"

#include "GlobalNamespace/FileHelpers.hpp"
#include "UnityEngine/Application.hpp"

using namespace GlobalNamespace;

static std::optional<std::string> alternativeDataPath = std::nullopt;
std::string GetAlternativeDataPath() {
    if(!alternativeDataPath.has_value()) {
        auto path = getDataDir(ID) + "PlayerData";
        mkpath(path);
        alternativeDataPath.emplace(path);
    }

    return *alternativeDataPath;
}

std::optional<std::string> ConvertFilePath(std::string dataDirectory, std::string path) {
    if(((std::string) path).rfind((std::string) dataDirectory, 0) == 0) {
        auto trimmed = path.substr(dataDirectory.length());
        return GetAlternativeDataPath() + trimmed;
    }
    return std::nullopt;
}

MAKE_AUTO_HOOK_MATCH(FileHelpers_SaveToJSONFile, static_cast<void (*)(Il2CppObject*, StringW, StringW, StringW)>
(&FileHelpers::SaveToJSONFile), void, Il2CppObject* obj, StringW filePath, StringW tempFilePath, StringW backupFilePath)
{
    auto dataDir = UnityEngine::Application::get_persistentDataPath();

    auto altFilePath = ConvertFilePath(dataDir, filePath);
    auto altTempFilePath = ConvertFilePath(dataDir, tempFilePath);
    auto altBackupFilePath = ConvertFilePath(dataDir, backupFilePath);

    // If an alternative path is available for this file, we will also save to there
    if(altFilePath.has_value() && altTempFilePath.has_value() && altBackupFilePath.has_value()) {
        PinkCore::Logging::getLogger().info("Triggering additional player data save to %s", altFilePath->c_str());
        FileHelpers_SaveToJSONFile(obj, *altFilePath, *altTempFilePath, *altBackupFilePath);
    }

    // Always save to the original location as well!
    FileHelpers_SaveToJSONFile(obj, filePath, tempFilePath, backupFilePath);
}

MAKE_AUTO_HOOK_MATCH(FileHelpers_LoadJSONFile, &FileHelpers::LoadJSONFile, StringW, StringW filePath, StringW backupFilePath)
{
    auto dataDir = UnityEngine::Application::get_persistentDataPath();

    auto altFilePath = ConvertFilePath(dataDir, filePath);

    std::optional<std::string> altBackupFilePath = std::nullopt;
    bool isBackupAvailable = backupFilePath.operator Il2CppString *();
    if(isBackupAvailable) {
        altBackupFilePath = ConvertFilePath(dataDir, backupFilePath);
    }

    auto originalResult = FileHelpers_LoadJSONFile(filePath, backupFilePath);

    // If an alternative save path is available for this file
    if(altFilePath.has_value() && (altBackupFilePath.has_value() || !isBackupAvailable)) {
        // Attempt to load the file with an alternate path
        PinkCore::Logging::getLogger().info("Attempting player data load from alternative path at %s", altFilePath->c_str());
        auto alternativeResult = FileHelpers_LoadJSONFile(*altFilePath, isBackupAvailable ? StringW(*altBackupFilePath) : StringW(nullptr));

        // If the alternative path did not contain a file or wasn't loadable, we must return the original file
        if(!alternativeResult) {
            PinkCore::Logging::getLogger().info("Player data load from alternative path successful!");
            return originalResult;
        }

        return alternativeResult;
    }

    return originalResult;
}