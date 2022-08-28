#pragma once
#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include <optional>
#include <functional>
#include <utility>

namespace PinkCore::API
{

	struct LevelDetails {
		bool isCustom;
		bool isWIP;
		GlobalNamespace::BeatmapCharacteristicSO* characteristic;
		GlobalNamespace::BeatmapDifficulty difficulty;
		bool showRotationSpwanLines;
		const char16_t* environmentType;
		int saberCount;
		bool hasCustomColours;
        std::vector<std::string> currentRequirements = {};
	    std::vector<std::string> currentSuggestions = {};
	};

	/// @brief returns the current loaded maps level details
	/// @return level detail struct
	LevelDetails GetCurrrentMapData();

	/// @brief returns a new leveldetails struct that of the provided info
	/// @return new level detail struct that isnt saved
    /// @param in the infodat json of the new map
    /// @param difficulty the difficulty of the new beatmap
    /// @param characteristic the characteristic of the new beatmap
    /// @param level ipreviewbeatmap level of the new beatmap
    LevelDetails GetNewMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic, GlobalNamespace::IPreviewBeatmapLevel* level);


}
#undef pinkcore_id