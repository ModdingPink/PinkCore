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
	RequirementUtils::PreHandleRequirements(currentSelectedLevel);

	StandardLevelDetailView_SetContent(self, level, defaultDifficulty, defaultBeatmapCharacteristic, playerData);
}

MAKE_AUTO_HOOK_MATCH(StandardLevelDetailView_RefreshContent, &GlobalNamespace::StandardLevelDetailView::RefreshContent, void, GlobalNamespace::StandardLevelDetailView* self)
{
	StandardLevelDetailView_RefreshContent(self);
	auto beatmapCharacteristicSegmentedControlController = self->beatmapCharacteristicSegmentedControlController;
	auto selectedBeatmapCharacteristic = beatmapCharacteristicSegmentedControlController ? beatmapCharacteristicSegmentedControlController->selectedBeatmapCharacteristic : nullptr;
    SongUtils::CustomData::SetCharacteristicAndDifficulty(self->beatmapDifficultySegmentedControlController->selectedDifficulty, selectedBeatmapCharacteristic);
	RequirementUtils::HandleRequirementDetails();
	ContributorUtils::FetchListOfContributors();
	UIUtils::SetupOrUpdateRequirementsModal(self);
	RequirementUtils::UpdatePlayButton();

	// set data for segmented controllers so they update with the selected level
    // if (self->level != nullptr && self->level->get_beatmapLevelData() != nullptr) {
	// 	self->beatmapCharacteristicSegmentedControlController->SetData(self->level->get_beatmapLevelData()->get_difficultyBeatmapSets(), self->beatmapCharacteristicSegmentedControlController->get_selectedBeatmapCharacteristic());
	// 	self->beatmapDifficultySegmentedControlController->SetData(GlobalNamespace::BeatmapLevelDataExtensions::GetDifficultyBeatmapSet(self->level->get_beatmapLevelData(), self->beatmapCharacteristicSegmentedControlController->get_selectedBeatmapCharacteristic())->get_difficultyBeatmaps(), self->beatmapDifficultySegmentedControlController->get_selectedDifficulty());
    // }
}

MAKE_AUTO_HOOK_MATCH(BeatmapCharacteristicSegmentedControlController_SetData, &GlobalNamespace::BeatmapCharacteristicSegmentedControlController::SetData, void, GlobalNamespace::BeatmapCharacteristicSegmentedControlController* self, System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::IDifficultyBeatmapSet*>* difficultyBeatmapSets, GlobalNamespace::BeatmapCharacteristicSO* selectedBeatmapCharacteristic)
{	
    BeatmapCharacteristicSegmentedControlController_SetData(self, difficultyBeatmapSets,selectedBeatmapCharacteristic);
	if (!SongUtils::SongInfo::get_currentlySelectedIsCustom() || !config.enableCustomCharacteristics) return;
    int i = 0;
    ArrayW<HMUI::IconSegmentedControl::DataItem*> dataItemArray(self->segmentedControl->dataItems->Length());

    for(auto dataItem : self->segmentedControl->dataItems){
        UnityEngine::Sprite* characteristicSprite = nullptr;
        StringW characteristicText = "";
        SongUtils::CustomData::GetCustomCharacteristicItems(self->beatmapCharacteristics->get_Item(i)->serializedName, characteristicSprite, characteristicText);
        if(characteristicText == "") characteristicText = Polyglot::Localization::Get(self->beatmapCharacteristics->get_Item(i)->characteristicNameLocalizationKey);
        if(characteristicSprite == nullptr) characteristicSprite = self->beatmapCharacteristics->get_Item(i)->get_icon();
        dataItemArray[i] = HMUI::IconSegmentedControl::DataItem::New_ctor(characteristicSprite, characteristicText);
        i++;
    }
	int selectedCell = self->segmentedControl->selectedCellNumber;
    self->segmentedControl->SetData(dataItemArray);
    self->segmentedControl->SelectCellWithNumber(selectedCell);
}

MAKE_AUTO_HOOK_MATCH(BeatmapDifficultyMethods_Name, &GlobalNamespace::BeatmapDifficultyMethods::Name, StringW, GlobalNamespace::BeatmapDifficulty difficulty) {
	
	if (SongUtils::SongInfo::get_currentlySelectedIsCustom() && config.enableCustomDiffNames) {
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

MAKE_AUTO_HOOK_MATCH(ModalView_Show, &HMUI::ModalView::Show, void, HMUI::ModalView* self, bool animated, bool moveToCenter, System::Action* finishedCallback)
{
	ModalView_Show(self, animated, moveToCenter, finishedCallback); 
	auto cb = self->blockerGO->get_gameObject()->GetComponent<UnityEngine::Canvas*>();
	auto screen = self->get_transform()->get_parent()->get_gameObject()->GetComponentInParent<HMUI::Screen*>();
	auto canvases = screen->get_gameObject()->GetComponentsInChildren<UnityEngine::Canvas*>();

	int highest = 0;
	for (auto& canvas : canvases) {
		if (canvas->get_sortingLayerID() == cb->get_sortingLayerID()) {
			// if highest lower than current, assign
			if (highest < canvas->get_sortingOrder()) {
				highest = canvas->get_sortingOrder();
			}
		}
	}

	highest ++;
	cb->set_overrideSorting(true);
	cb->set_sortingOrder(highest);

	auto cm = self->get_gameObject()->GetComponent<UnityEngine::Canvas*>();
	cm->set_overrideSorting(true); 
	cm->set_sortingOrder(highest + 1);
}
