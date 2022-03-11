#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
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

MAKE_AUTO_HOOK_MATCH(BeatmapDifficultyMethods_Name, &GlobalNamespace::BeatmapDifficultyMethods::Name, StringW, GlobalNamespace::BeatmapDifficulty difficulty) {
	
	if (SongUtils::SongInfo::get_currentlySelectedIsCustom() && config.enableCustomDiffNames) {
		std::u16string newDifficultyLabel = DifficultyNameUtils::GetDifficultyNameFromCache(difficulty);
		if (!newDifficultyLabel.empty()) {
			return il2cpp_utils::newcsstr(newDifficultyLabel);
		}
	}
	return BeatmapDifficultyMethods_Name(difficulty);
}

MAKE_AUTO_HOOK_MATCH(BeatmapDifficultySegmentedControlController_SetData, &GlobalNamespace::BeatmapDifficultySegmentedControlController::SetData, void, GlobalNamespace::BeatmapDifficultySegmentedControlController* self, System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::IDifficultyBeatmap*>* difficultyBeatmapsList, GlobalNamespace::BeatmapDifficulty selectedDifficulty)
{
	auto difficultyBeatmaps = ArrayW<GlobalNamespace::IDifficultyBeatmap*>(difficultyBeatmapsList);
	SongUtils::SongInfo::set_lastPhysicallySelectedDifficulty(SongUtils::GetDiffFromNumber(selectedDifficulty));

	if (SongUtils::SongInfo::get_currentlySelectedIsCustom()) {
		if (difficultyBeatmaps[0] != nullptr) {
			SongUtils::SongInfo::set_lastPhysicallySelectedCharacteristic(difficultyBeatmaps[0]->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->get_serializedName());
			if (config.enableCustomDiffNames) {
				DifficultyNameUtils::SetDifficultyNameCacheFromArray(difficultyBeatmaps);
			}
		}
	}
	BeatmapDifficultySegmentedControlController_SetData(self, difficultyBeatmapsList, selectedDifficulty);
}