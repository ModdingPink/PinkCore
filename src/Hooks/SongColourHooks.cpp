#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/MultiplayerLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/SceneInfo.hpp"
#include "GlobalNamespace/GameplayCoreSceneSetupData.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/ColorSchemeSO.hpp"
#include "GlobalNamespace/IBeatmapLevel.hpp"
#include "GlobalNamespace/IBeatmapLevelData.hpp"
#include "GlobalNamespace/IDifficultyBeatmapSet.hpp"
#include "GlobalNamespace/BeatmapEnvironmentHelper.hpp"

#include "UnityEngine/UI/Button.hpp"

using namespace GlobalNamespace;



MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, &StandardLevelScenesTransitionSetupDataSO::Init, void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, IDifficultyBeatmap* difficultyBeatmap, IPreviewBeatmapLevel* previewBeatmapLevel, OverrideEnvironmentSettings* overrideEnvironmentSettings, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused)
{
	GlobalNamespace::ColorScheme* newColourScheme = nullptr;
	SongUtils::CustomData::HandleGetMapInfoData(previewBeatmapLevel);
	//, difficultyBeatmap->get_difficulty(), difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()
	auto mapData = SongUtils::SongInfo::get_mapData();
	if (config.enableCustomSongColours && mapData.hasCustomColours) {
		if (overrideColorScheme == nullptr)
			newColourScheme = SongUtils::CustomData::GetCustomSongColour(previewBeatmapLevel->get_environmentInfo()->colorScheme->colorScheme, false);
		else
			newColourScheme = SongUtils::CustomData::GetCustomSongColour(overrideColorScheme, true);
	}
	if(newColourScheme) overrideColorScheme = newColourScheme;
	
	bool containRotEvent = difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->containsRotationEvents;

	StringW envType = mapData.environmentType;
	if(envType == "allDirections"){
		difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->containsRotationEvents = true;
	}else if(envType == "default"){
		difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->containsRotationEvents = false;
	}
	
	StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused);
	
	difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->containsRotationEvents = containRotEvent;
}

std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>> multiJSON;

MAKE_AUTO_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::Init, void, MultiplayerLevelScenesTransitionSetupDataSO* self, StringW gameMode, IPreviewBeatmapLevel* previewBeatmapLevel, BeatmapDifficulty beatmapDifficulty, BeatmapCharacteristicSO* beatmapCharacteristic, IDifficultyBeatmap* difficultyBeatmap, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects)
{
	GlobalNamespace::ColorScheme* newColourScheme = nullptr;
	if(SongUtils::CustomData::GetInfoJson(previewBeatmapLevel, multiJSON)){
		SongUtils::SongInfo::UpdateMapData(previewBeatmapLevel, *multiJSON, beatmapDifficulty, beatmapCharacteristic);

		rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;

		SongUtils::CustomData::GetCustomDataJsonFromDifficultyAndCharacteristic(*multiJSON, customData, beatmapDifficulty, beatmapCharacteristic);

		if (config.enableCustomSongColours && SongUtils::SongInfo::get_mapData().hasCustomColours) {
			if (overrideColorScheme == nullptr)
				newColourScheme = SongUtils::CustomData::GetCustomSongColourFromCustomData(previewBeatmapLevel->get_environmentInfo()->colorScheme->colorScheme, false, customData);
			else
				newColourScheme = SongUtils::CustomData::GetCustomSongColourFromCustomData(overrideColorScheme, true, customData);
		}
		if(newColourScheme) overrideColorScheme = newColourScheme;


	}
	MultiplayerLevelScenesTransitionSetupDataSO_Init(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects);
}
