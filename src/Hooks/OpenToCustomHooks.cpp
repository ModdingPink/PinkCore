#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"

#include "GlobalNamespace/LevelFilteringNavigationController.hpp"
#include "GlobalNamespace/IBeatmapLevelPack.hpp"
#include "GlobalNamespace/SongPackMask.hpp"
#include "GlobalNamespace/SelectLevelCategoryViewController.hpp"


#include "UnityEngine/UI/Button.hpp"

#include <cstdlib>
  
MAKE_AUTO_HOOK_MATCH(LevelFilteringNavigationController_Setup, &GlobalNamespace::LevelFilteringNavigationController::Setup, void, GlobalNamespace::LevelFilteringNavigationController* self, GlobalNamespace::SongPackMask songPackMask, GlobalNamespace::IBeatmapLevelPack* levelPackToBeSelectedAfterPresent, GlobalNamespace::SelectLevelCategoryViewController::LevelCategory startLevelCategory, bool hidePacksIfOneOrNone, bool enableCustomLevels)
{
	LevelFilteringNavigationController_Setup(self, songPackMask, levelPackToBeSelectedAfterPresent, startLevelCategory, hidePacksIfOneOrNone, enableCustomLevels);

	if (levelPackToBeSelectedAfterPresent == nullptr && config.openToCustomLevels && enableCustomLevels) {
		self->selectLevelCategoryViewController->Setup(startLevelCategory.CustomSongs, self->enabledLevelCategories);
	}
}