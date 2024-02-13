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
#include "HMUI/ModalView.hpp"
#include "HMUI/Screen.hpp"
#include "HMUI/SegmentedControl.hpp"
#include "HMUI/IconSegmentedControl.hpp"
#include "Polyglot/Localization.hpp"
#include "GlobalNamespace/GameplayCoreInstaller.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatLineManager.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "Zenject/MonoInstaller.hpp"

MAKE_AUTO_HOOK_MATCH(BeatmapCharacteristicSegmentedControlController_SetData, &GlobalNamespace::BeatmapCharacteristicSegmentedControlController::SetData, void, GlobalNamespace::BeatmapCharacteristicSegmentedControlController* self, System::Collections::Generic::IReadOnlyList_1<::GlobalNamespace::IDifficultyBeatmapSet*>* difficultyBeatmapSets, GlobalNamespace::BeatmapCharacteristicSO* selectedBeatmapCharacteristic)
{
    BeatmapCharacteristicSegmentedControlController_SetData(self, difficultyBeatmapSets,selectedBeatmapCharacteristic);
	if (!SongUtils::SongInfo::get_mapData().isCustom || !config.enableCustomCharacteristics) return;
    int i = 0;
    ArrayW<HMUI::IconSegmentedControl::DataItem*> dataItemArray(self->_segmentedControl->_dataItems.size());

    for(auto dataItem : self->_segmentedControl->_dataItems){
        UnityEngine::Sprite* characteristicSprite = nullptr;
        StringW characteristicText = "";
        SongUtils::CustomData::GetCustomCharacteristicItems(self->_beatmapCharacteristics->get_Item(i), characteristicSprite, characteristicText);
        if(characteristicText == "") characteristicText = Polyglot::Localization::Get(self->_beatmapCharacteristics->get_Item(i)->characteristicNameLocalizationKey);
        if(characteristicSprite == nullptr) characteristicSprite = self->_beatmapCharacteristics->get_Item(i)->get_icon();
        dataItemArray[i] = HMUI::IconSegmentedControl::DataItem::New_ctor(characteristicSprite, characteristicText);
        i++;
    }
	int selectedCell = self->_segmentedControl->selectedCellNumber;
    self->_segmentedControl->SetData(dataItemArray);
    self->_segmentedControl->SelectCellWithNumber(selectedCell);
}

MAKE_AUTO_HOOK_MATCH(GameplayCoreInstaller_InstallBindings, &GlobalNamespace::GameplayCoreInstaller::InstallBindings, void, GlobalNamespace::GameplayCoreInstaller* self)
{
    if (!SongUtils::SongInfo::get_mapData().isCustom) { GameplayCoreInstaller_InstallBindings(self); return; }

    int newSaberNum = SongUtils::SongInfo::get_mapData().saberCount;
    if(newSaberNum == -1) { GameplayCoreInstaller_InstallBindings(self); return; }

	int colourNum = self->_sceneSetupData->difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->numberOfColors;
	self->_sceneSetupData->difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->_numberOfColors = newSaberNum;
	GameplayCoreInstaller_InstallBindings(self);
	self->_sceneSetupData->difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->_numberOfColors = colourNum;

}


MAKE_AUTO_HOOK_MATCH(BeatLineManager_HandleNoteWasSpawned, &GlobalNamespace::BeatLineManager::HandleNoteWasSpawned, void, GlobalNamespace::BeatLineManager* self, GlobalNamespace::NoteController* noteController)
{
    if (!SongUtils::SongInfo::get_mapData().isCustom) { BeatLineManager_HandleNoteWasSpawned(self, noteController); return; }

    if(SongUtils::SongInfo::get_mapData().showRotationSpawnLines) BeatLineManager_HandleNoteWasSpawned(self, noteController);
}
