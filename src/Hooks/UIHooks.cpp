#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"
#include "logging.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/RequirementUtils.hpp"
#include "Utils/UIUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"

#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "GlobalNamespace/StandardLevelDetailViewController.hpp"
#include "GlobalNamespace/BeatmapDifficultySegmentedControlController.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSegmentedControlController.hpp"
#include "GlobalNamespace/IBeatmapLevelData.hpp"
#include "GlobalNamespace/BeatmapLevelDataExtensions.hpp"
#include "GlobalNamespace/PlayerData.hpp"

#include "UnityEngine/UI/Button.hpp"
#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/Canvas.hpp"
#include "UnityEngine/Sprite.hpp"
#include "HMUI/IconSegmentedControl_DataItem.hpp"
#include "HMUI/ModalView.hpp"
#include "HMUI/Screen.hpp"
#include "Polyglot/Localization.hpp"
#include "sombrero/shared/linq.hpp"
#include "sombrero/shared/linq_functional.hpp"

MAKE_AUTO_HOOK_MATCH(StandardLevelDetailView_SetContent, &GlobalNamespace::StandardLevelDetailView::SetContent, void, GlobalNamespace::StandardLevelDetailView* self, ::GlobalNamespace::IBeatmapLevel* level, GlobalNamespace::BeatmapDifficulty defaultDifficulty, GlobalNamespace::BeatmapCharacteristicSO* defaultBeatmapCharacteristic, GlobalNamespace::PlayerData* playerData)
{
	auto currentSelectedLevel = reinterpret_cast<GlobalNamespace::IPreviewBeatmapLevel*>(level);
	
	//this will only work on a post fix, which we cant do because refresh is in the method which we need to do shit on anyway so AAAAAAAAAAAAAAAAAAA 3:<
	//GlobalNamespace::BeatmapDifficulty difficulty = defaultDifficulty;
	//if(!difficulty) difficulty = self->beatmapDifficultySegmentedControlController->selectedDifficulty;
	//GlobalNamespace::BeatmapCharacteristicSO* characteristic = defaultBeatmapCharacteristic;
	//if(!characteristic) characteristic = self->beatmapCharacteristicSegmentedControlController->selectedBeatmapCharacteristic;
	
	SongUtils::CustomData::HandleGetMapInfoData(currentSelectedLevel);

	//this ensures we get the info dat at the earliest point

	StandardLevelDetailView_SetContent(self, level, defaultDifficulty, defaultBeatmapCharacteristic, playerData);
}


MAKE_AUTO_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &GlobalNamespace::StandardLevelDetailView::RefreshContent, void, GlobalNamespace::StandardLevelDetailView* self)
{
	StandardLevelDetailView_RefreshContent(self);

	auto characteristic = self->selectedDifficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic();

	SongUtils::SongInfo::UpdateMapData(SongUtils::GetCurrentInfoDat(), self->selectedDifficultyBeatmap);

	UIUtils::SetupOrUpdateRequirementsModal(self);

	RequirementUtils::UpdatePlayButton();

	// set data for segmented controllers so they update with the selected level
    // if (self->level != nullptr && self->level->get_beatmapLevelData() != nullptr) {
	// 	self->beatmapCharacteristicSegmentedControlController->SetData(self->level->get_beatmapLevelData()->get_difficultyBeatmapSets(), self->beatmapCharacteristicSegmentedControlController->get_selectedBeatmapCharacteristic());
	// 	self->beatmapDifficultySegmentedControlController->SetData(GlobalNamespace::BeatmapLevelDataExtensions::GetDifficultyBeatmapSet(self->level->get_beatmapLevelData(), self->beatmapCharacteristicSegmentedControlController->get_selectedBeatmapCharacteristic())->get_difficultyBeatmaps(), self->beatmapDifficultySegmentedControlController->get_selectedDifficulty());
    // }
}

MAKE_AUTO_HOOK_MATCH(BeatmapDifficultyMethods_Name, &GlobalNamespace::BeatmapDifficultyMethods::Name, StringW, GlobalNamespace::BeatmapDifficulty difficulty) {
	
	if (SongUtils::SongInfo::get_mapData().isCustom && config.enableCustomDiffNames) {
		StringW newDifficultyLabel = DifficultyNameUtils::GetDifficultyNameFromCache(difficulty);
		if (newDifficultyLabel->get_Length() != 0) {
			return newDifficultyLabel;
		}
	}
	return BeatmapDifficultyMethods_Name(difficulty);
}

MAKE_AUTO_HOOK_MATCH(BeatmapDifficultySegmentedControlController_SetData, &GlobalNamespace::BeatmapDifficultySegmentedControlController::SetData, void, GlobalNamespace::BeatmapDifficultySegmentedControlController* self, System::Collections::Generic::IReadOnlyList_1<GlobalNamespace::IDifficultyBeatmap*>* difficultyBeatmapsList, GlobalNamespace::BeatmapDifficulty selectedDifficulty)
{	

	auto difficultyBeatmaps = ArrayW<GlobalNamespace::IDifficultyBeatmap*>(difficultyBeatmapsList);

	if (SongUtils::SongInfo::get_mapData().isCustom) {
		if (difficultyBeatmaps[0] != nullptr) {
			if (config.enableCustomDiffNames) {
				DifficultyNameUtils::SetDifficultyNameCacheFromArray(difficultyBeatmaps, difficultyBeatmaps[0]->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic());
			}
		}
	}
	BeatmapDifficultySegmentedControlController_SetData(self, difficultyBeatmapsList, selectedDifficulty);
}

