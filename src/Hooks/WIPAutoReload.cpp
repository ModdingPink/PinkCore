#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"

#include "GlobalNamespace/LevelCollectionViewController.hpp"
#include "GlobalNamespace/IBeatmapLevelPack.hpp"
#include "GlobalNamespace/SongPackMask.hpp"
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"

#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/WaitForSecondsRealtime.hpp"

#include "custom-types/shared/coroutine.hpp"

#include "songloader/shared/API.hpp"

#include <cstdlib>
#include <filesystem>
#include <unordered_map>

custom_types::Helpers::Coroutine wipPathReloader() {
    std::unordered_map<std::string_view, std::filesystem::file_time_type> times;

    std::string wipPath = RuntimeSongLoader::API::GetCustomWIPLevelsPath();

    while (true) {
        try {
            // LOG errors, then ignore
            // TODO: Just crash if any errors
            IL2CPP_CATCH_HANDLER(
                    if (!std::filesystem::exists(wipPath)) break;

                    for (auto const &folder: std::filesystem::directory_iterator(wipPath)) {

                        auto lastTimeIt = times.find(folder.path().c_str());

                        if (lastTimeIt == times.end()) {
                            times.emplace(folder.path().c_str(), folder.last_write_time());
                            continue;
                        }

                        auto &lastTime = lastTimeIt->second;

                        if (folder.last_write_time() <= lastTime) continue;

                        lastTime = folder.last_write_time();
                        RuntimeSongLoader::API::RefreshSongs();
                        break;
                    }
            )
        }
        // ignore
        catch (std::exception const& e) {}
        catch (...) {}

        co_yield UnityEngine::WaitForSecondsRealtime::New_ctor(4)->i_IEnumerator();
    }
}

MAKE_AUTO_HOOK_MATCH(LevelCollectionViewController_DidActivate, &GlobalNamespace::LevelCollectionViewController::DidActivate, void, GlobalNamespace::LevelCollectionViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling)
{
    LevelCollectionViewController_DidActivate(self, firstActivation, addedToHierarchy, screenSystemEnabling);


    if (firstActivation) {
        //self->StartCoroutine(custom_types::Helpers::new_coro(wipPathReloader));
    }
}