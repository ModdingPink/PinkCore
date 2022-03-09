#include "beatsaber-hook/shared/utils/logging.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "hooks.hpp"
#include "config.hpp"

#include "Utils/SongUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"

#include "GlobalNamespace/StandardLevelScenesTransitionSetupDataSO.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/OverrideEnvironmentSettings.hpp"
#include "GlobalNamespace/ColorScheme.hpp"
#include "GlobalNamespace/GameplayModifiers.hpp"
#include "GlobalNamespace/PlayerSpecificSettings.hpp"
#include "GlobalNamespace/PracticeSettings.hpp"
#include "GlobalNamespace/EnvironmentInfoSO.hpp"
#include "GlobalNamespace/ColorSchemeSO.hpp"

#include "UnityEngine/UI/Button.hpp"

MAKE_AUTO_HOOK_MATCH(StandardLevelScenesTransitionSetupDataSO_Init, &GlobalNamespace::StandardLevelScenesTransitionSetupDataSO::Init, void, GlobalNamespace::StandardLevelScenesTransitionSetupDataSO* self, StringW gameMode, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap, GlobalNamespace::IPreviewBeatmapLevel* previewBeatmapLevel, GlobalNamespace::OverrideEnvironmentSettings* overrideEnvironmentSettings, GlobalNamespace::ColorScheme* overrideColorScheme, GlobalNamespace::GameplayModifiers* gameplayModifiers, GlobalNamespace::PlayerSpecificSettings* playerSpecificSettings, GlobalNamespace::PracticeSettings* practiceSettings, StringW backButtonText, bool useTestNoteCutSoundEffects, bool startPaused)
{
	if (!SongUtils::SongInfo::get_currentlySelectedIsCustom() || !config.enableCustomSongColours) {
		StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused);
		return;
	}

	GlobalNamespace::ColorScheme* colourScheme;
	if (overrideColorScheme == nullptr) {
		colourScheme = SongUtils::CustomData::getCustomSongColour(previewBeatmapLevel->get_environmentInfo()->colorScheme->colorScheme, false);
	}
	else {
		colourScheme = SongUtils::CustomData::getCustomSongColour(overrideColorScheme, true);
	}

	if (colourScheme == nullptr) {
		StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, overrideColorScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused);
	}
	else {
		StandardLevelScenesTransitionSetupDataSO_Init(self, gameMode, difficultyBeatmap, previewBeatmapLevel, overrideEnvironmentSettings, colourScheme, gameplayModifiers, playerSpecificSettings, practiceSettings, backButtonText, useTestNoteCutSoundEffects, startPaused);
	}
}
