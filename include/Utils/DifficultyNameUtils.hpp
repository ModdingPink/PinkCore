#pragma once

#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include <string>

namespace DifficultyNameUtils
{
	void SetDifficultyNameCacheFromDifficulty(GlobalNamespace::BeatmapDifficulty difficulty, std::u16string_view name);
	std::u16string GetDifficultyNameFromDoc(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& d, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);
	std::u16string GetDifficultyNameFromCache(GlobalNamespace::BeatmapDifficulty difficulty);
	void SetDifficultyNameCacheFromArray(ArrayW<GlobalNamespace::IDifficultyBeatmap*>& difficultyArray, GlobalNamespace::BeatmapCharacteristicSO* characteristic);

}
