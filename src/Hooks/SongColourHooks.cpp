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


std::shared_ptr<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>> levelStartJSONptr;
PinkCore::API::LevelDetails levelStartMapLevelDetails;

MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, &StandardLevelScenesTransitionSetupDataSO::Init, void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, IDifficultyBeatmap* difficultyBeatmap, IPreviewBeatmapLevel* previewBeatmapLevel, OverrideEnvironmentSettings* overrideEnvironmentSettings, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused, ::GlobalNamespace::BeatmapDataCache* beatmapDataCache)
{
	if(!SongUtils::SongInfo::isCustom(previewBeatmapLevel)){ 	
		StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused, beatmapDataCache);
		return; 
	}
	
	bool containRotEvent = difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->containsRotationEvents;

	GlobalNamespace::ColorScheme* newColourScheme = nullptr;
	if(SongUtils::CustomData::GetInfoJson(previewBeatmapLevel, levelStartJSONptr)){
		SongUtils::SongInfo::SetMapData(*levelStartJSONptr, levelStartMapLevelDetails, difficultyBeatmap);
		rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
		if(SongUtils::CustomData::GetCustomDataJsonFromDifficultyAndCharacteristic(*levelStartJSONptr, customData, difficultyBeatmap->get_difficulty(), difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic())){
			if (config.enableCustomSongColours && levelStartMapLevelDetails.hasCustomColours) {
				if (overrideColorScheme == nullptr){
					newColourScheme = SongUtils::CustomData::GetCustomSongColourFromCustomData(previewBeatmapLevel->get_environmentInfo()->colorScheme->colorScheme, false, customData);
				}else{
					newColourScheme = SongUtils::CustomData::GetCustomSongColourFromCustomData(overrideColorScheme, true, customData);
				}
			}
			if(newColourScheme) overrideColorScheme = newColourScheme;

			StringW envType = levelStartMapLevelDetails.environmentType;
			if(envType == "allDirections"){
				difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->containsRotationEvents = true;
			}else if(envType == "default"){
				difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->containsRotationEvents = false;
			}
			
		}
	}

	StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused, beatmapDataCache);
	
	difficultyBeatmap->get_parentDifficultyBeatmapSet()->get_beatmapCharacteristic()->containsRotationEvents = containRotEvent;
}


MAKE_AUTO_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::Init, void, MultiplayerLevelScenesTransitionSetupDataSO* self, StringW gameMode, IPreviewBeatmapLevel* previewBeatmapLevel, BeatmapDifficulty beatmapDifficulty, BeatmapCharacteristicSO* beatmapCharacteristic, IDifficultyBeatmap* difficultyBeatmap, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects)
{
	if(!SongUtils::SongInfo::isCustom(previewBeatmapLevel)){ 
		MultiplayerLevelScenesTransitionSetupDataSO_Init(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects);
		return;
	}

	GlobalNamespace::ColorScheme* newColourScheme = nullptr;
	if(SongUtils::CustomData::GetInfoJson(previewBeatmapLevel, levelStartJSONptr)){
		SongUtils::SongInfo::SetMapData(*levelStartJSONptr, levelStartMapLevelDetails, difficultyBeatmap);
		rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
		if(SongUtils::CustomData::GetCustomDataJsonFromDifficultyAndCharacteristic(*levelStartJSONptr, customData, beatmapDifficulty, beatmapCharacteristic)){
			if (config.enableCustomSongColours && levelStartMapLevelDetails.hasCustomColours) {
				if (overrideColorScheme == nullptr)
					newColourScheme = SongUtils::CustomData::GetCustomSongColourFromCustomData(previewBeatmapLevel->get_environmentInfo()->colorScheme->colorScheme, false, customData);
				else
					newColourScheme = SongUtils::CustomData::GetCustomSongColourFromCustomData(overrideColorScheme, true, customData);
			}
			if(newColourScheme) overrideColorScheme = newColourScheme;
		}
	}
	MultiplayerLevelScenesTransitionSetupDataSO_Init(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects);
}
