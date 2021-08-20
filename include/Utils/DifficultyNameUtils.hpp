#pragma once
#include "modloader/shared/modloader.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/il2cpp-utils.hpp" 
#include "beatsaber-hook/shared/utils/typedefs.h"
#include "GlobalNamespace/BeatmapDifficultyMethods.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include <string>

namespace DifficultyNameUtils
{
	void setDifficultyNameCacheFromDifficulty(GlobalNamespace::BeatmapDifficulty difficulty, std::string name);
	std::string GetDifficultyNameFromDoc(rapidjson::Document& d, GlobalNamespace::BeatmapDifficulty difficulty);
	std::string GetDifficultyNameFromCache(GlobalNamespace::BeatmapDifficulty difficulty);
	void SetDifficultyNameCacheFromArray(::Array<GlobalNamespace::IDifficultyBeatmap*>* difficultyArray);

}