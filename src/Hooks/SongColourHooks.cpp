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

MAKE_AUTO_HOOK_MATCH(
	StandardLevelScenesTransitionSetupDataSO_Init,
	&StandardLevelScenesTransitionSetupDataSO::Init,
	void,
	StandardLevelScenesTransitionSetupDataSO* self,
	StringW gameMode,
	GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap,
	GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel,
	GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings,
	GlobalNamespace::ColorScheme* overrideColorScheme,
	GlobalNamespace::ColorScheme* beatmapOverrideColorScheme,
	GlobalNamespace::GameplayModifiers* gameplayModifiers,
	GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings,
	GlobalNamespace::PracticeSettings* practiceSettings,
	StringW backButtonText,
	bool useTestNoteCutSoundEffects,
	bool startPaused,
	GlobalNamespace::BeatmapDataCache* beatmapDataCache,
	System::Nullable_1<::GlobalNamespace::__RecordingToolManager__SetupData> recordingToolData
) {
	if(!SongUtils::SongInfo::isCustom(previewBeatmapLevel)){
		StandardLevelScenesTransitionSetupDataSO_Init(
			self,
			gameMode,
			difficultyBeatmap,
			previewBeatmapLevel,
			overrideEnvironmentSettings,
			overrideColorScheme,
			beatmapOverrideColorScheme,
			gameplayModifiers,
			playerSpecificSettings,
			practiceSettings,
			backButtonText,
			useTestNoteCutSoundEffects,
			startPaused,
			beatmapDataCache,
			recordingToolData
		);
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
				difficultyBeatmap->parentDifficultyBeatmapSet->beatmapCharacteristic->_containsRotationEvents = true;
			}else if(envType == "default"){
				difficultyBeatmap->parentDifficultyBeatmapSet->beatmapCharacteristic->_containsRotationEvents = false;
			}

		}
	}

	StandardLevelScenesTransitionSetupDataSO_Init(
		self,
		gameMode,
		difficultyBeatmap,
		previewBeatmapLevel,
		overrideEnvironmentSettings,
		overrideColorScheme,
		beatmapOverrideColorScheme,
		gameplayModifiers,
		playerSpecificSettings,
		practiceSettings,
		backButtonText,
		useTestNoteCutSoundEffects,
		startPaused,
		beatmapDataCache,
		recordingToolData
	);

	difficultyBeatmap->parentDifficultyBeatmapSet->beatmapCharacteristic->_containsRotationEvents = containRotEvent;
}


MAKE_AUTO_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::Init, void, MultiplayerLevelScenesTransitionSetupDataSO* self, StringW gameMode, IPreviewBeatmapLevel* previewBeatmapLevel, BeatmapDifficulty beatmapDifficulty, BeatmapCharacteristicSO* beatmapCharacteristic, IDifficultyBeatmap* difficultyBeatmap, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects)
{
	if(!difficultyBeatmap || !SongUtils::SongInfo::isCustom(previewBeatmapLevel)){
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
