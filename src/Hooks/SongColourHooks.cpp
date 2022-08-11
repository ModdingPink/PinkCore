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

#include "UnityEngine/UI/Button.hpp"

using namespace GlobalNamespace;

MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, &StandardLevelScenesTransitionSetupDataSO::Init, void, StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, IDifficultyBeatmap* difficultyBeatmap, IPreviewBeatmapLevel* previewBeatmapLevel, OverrideEnvironmentSettings* overrideEnvironmentSettings, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused)
{
	GlobalNamespace::ColorScheme* newColourScheme = nullptr;
	if (SongUtils::SongInfo::get_currentlySelectedIsCustom() && config.enableCustomSongColours) {
		if (overrideColorScheme == nullptr)
			newColourScheme = SongUtils::CustomData::GetCustomSongColour(previewBeatmapLevel->get_environmentInfo()->colorScheme->colorScheme, false);
		else
			newColourScheme = SongUtils::CustomData::GetCustomSongColour(overrideColorScheme, true);
	}
	if(newColourScheme) overrideColorScheme = newColourScheme;

	StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused);
}

MAKE_AUTO_HOOK_MATCH(MultiplayerLevelScenesTransitionSetupDataSO_Init, &MultiplayerLevelScenesTransitionSetupDataSO::Init, void, MultiplayerLevelScenesTransitionSetupDataSO* self, StringW gameMode, IPreviewBeatmapLevel* previewBeatmapLevel, BeatmapDifficulty beatmapDifficulty, BeatmapCharacteristicSO* beatmapCharacteristic, IDifficultyBeatmap* difficultyBeatmap, ColorScheme* overrideColorScheme, GameplayModifiers* gameplayModifiers, PlayerSpecificSettings* playerSpecificSettings, PracticeSettings* practiceSettings, bool useTestNoteCutSoundEffects)
{
	auto& dat = SongUtils::GetCurrentInfoDatPtr();
	GlobalNamespace::ColorScheme* newColourScheme = nullptr;
	if (config.enableCustomSongColours && SongUtils::CustomData::GetInfoJson(previewBeatmapLevel, dat)) {
		if (overrideColorScheme == nullptr)
			newColourScheme = SongUtils::CustomData::GetCustomSongColour(previewBeatmapLevel->get_environmentInfo()->colorScheme->colorScheme, false);
		else
			newColourScheme = SongUtils::CustomData::GetCustomSongColour(overrideColorScheme, true);
	}
	if(newColourScheme) overrideColorScheme = newColourScheme;
	MultiplayerLevelScenesTransitionSetupDataSO_Init(self, gameMode, previewBeatmapLevel, beatmapDifficulty, beatmapCharacteristic, difficultyBeatmap, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, useTestNoteCutSoundEffects);
}
