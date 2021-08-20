#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "Hooks.hpp"
#include "config.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"

#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/BeatmapDifficultySegmentedControlController.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"

#include "UnityEngine/UI/Button.hpp"

MAKE_HOOK_MATCH(BeatmapDifficultyMethods_Name, &GlobalNamespace::BeatmapDifficultyMethods::Name, Il2CppString*, GlobalNamespace::BeatmapDifficulty difficulty) {
	
	if (SongUtils::SongInfo::get_currentlySelectedIsCustom() && config.enableCustomDiffNames) {
		std::string newDifficultyLabel = DifficultyNameUtils::GetDifficultyNameFromCache(difficulty);
		if (newDifficultyLabel != "") {
			return il2cpp_utils::createcsstr(newDifficultyLabel);
		}
	}
	return BeatmapDifficultyMethods_Name(difficulty);
}

MAKE_HOOK_MATCH(BeatmapDifficultySegmentedControlController_SetData, &GlobalNamespace::BeatmapDifficultySegmentedControlController::SetData, void, GlobalNamespace::BeatmapDifficultySegmentedControlController* self, ::Array<GlobalNamespace::IDifficultyBeatmap*>* difficultyBeatmaps, GlobalNamespace::BeatmapDifficulty selectedDifficulty)
{
	SongUtils::SongInfo::set_lastPhysicallySelectedDifficulty(SongUtils::GetDiffFromNumber(selectedDifficulty));

	if (SongUtils::SongInfo::get_currentlySelectedIsCustom()) {
		if (difficultyBeatmaps->values[0] != nullptr) {
			SongUtils::SongInfo::set_lastPhysicallySelectedCharacteristic(to_utf8(csstrtostr(difficultyBeatmaps->values[0]->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->get_serializedName())));
			if (config.enableCustomDiffNames) {
				DifficultyNameUtils::SetDifficultyNameCacheFromArray(difficultyBeatmaps);
			}
		}
	}
	BeatmapDifficultySegmentedControlController_SetData(self, difficultyBeatmaps, selectedDifficulty);
}
void InstallDifficultyNameHooks(Logger& logger)
{
	SIMPLE_INSTALL_HOOK(BeatmapDifficultyMethods_Name);
	SIMPLE_INSTALL_HOOK(BeatmapDifficultySegmentedControlController_SetData);
}

// using a macro to register the method pointer to the class that stores all of the install methods, for automatic execution
PCInstallHooks(InstallDifficultyNameHooks)