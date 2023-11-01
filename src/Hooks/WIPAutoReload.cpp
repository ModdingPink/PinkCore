#include "beatsaber-hook/shared/utils/logging.hpp"

#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "config.hpp"
#include "hooks.hpp"

#include "GlobalNamespace/LevelCollectionViewController.hpp"

#include "UnityEngine/WaitForSecondsRealtime.hpp"

#include "custom-types/shared/coroutine.hpp"

#include "songloader/shared/API.hpp"

#include <cstdlib>
#include <filesystem>
#include <unordered_map>

std::filesystem::file_time_type
max_last_file_time(std::filesystem::path const &path, bool recursive) {
  std::filesystem::file_time_type lastTime =
      std::filesystem::last_write_time(path);

  for (auto const &file : std::filesystem::directory_iterator(path)) {
    lastTime = std::max(lastTime, file.last_write_time());
  }

  if (recursive) {
    for (auto const &file : std::filesystem::directory_iterator(path)) {
      lastTime = std::max(lastTime, max_last_file_time(path, true));
    }
  }

  return lastTime;
}

custom_types::Helpers::Coroutine wipPathReloader() {
  std::unordered_map<std::string_view, std::filesystem::file_time_type> times;

  std::string wipPath = RuntimeSongLoader::API::GetCustomWIPLevelsPath();

  std::filesystem::file_time_type oldLastTime = max_last_file_time(wipPath, true);

  while (true) {

    auto lastTime = max_last_file_time(wipPath, true);

    // if a file was modified, refresh
    if (lastTime >= oldLastTime) {
      RuntimeSongLoader::API::RefreshSongs();
      oldLastTime = lastTime;
    }

    // try {
    //   // LOG errors, then ignore
    //   // TODO: Just crash if any errors
    //   IL2CPP_CATCH_HANDLER(
    //       if (!std::filesystem::exists(wipPath)) break;

    //       for (auto const &folder
    //            : std::filesystem::directory_iterator(wipPath)) {
    //         auto lastTimeIt = times.find(folder.path().c_str());

    //         if (lastTimeIt == times.end()) {
    //           times.emplace(folder.path().c_str(),
    //                         max_last_file_time(folder.path(), true));
    //           continue;
    //         }

    //         auto &lastTime = lastTimeIt->second;

    //         if (folder.last_write_time() <= lastTime)
    //           continue;

    //         lastTime = max_last_file_time(folder.path(), true);
    //         RuntimeSongLoader::API::RefreshSongs();
    //         break;
    //       })
    // }
    // // ignore
    // catch (std::exception const &e) {
    // } catch (...) {
    // }

    co_yield UnityEngine::WaitForSecondsRealtime::New_ctor(4)->i_IEnumerator();
  }
}

MAKE_AUTO_HOOK_MATCH(
    LevelCollectionViewController_DidActivate,
    &GlobalNamespace::LevelCollectionViewController::DidActivate, void,
    GlobalNamespace::LevelCollectionViewController *self, bool firstActivation,
    bool addedToHierarchy, bool screenSystemEnabling) {
  LevelCollectionViewController_DidActivate(
      self, firstActivation, addedToHierarchy, screenSystemEnabling);

  if (firstActivation) {
    self->StartCoroutine(custom_types::Helpers::new_coro(wipPathReloader));
  }
}