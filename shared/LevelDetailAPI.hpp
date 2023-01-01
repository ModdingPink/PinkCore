#pragma once
#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "GlobalNamespace/BeatmapCharacteristicSO.hpp"
#include "GlobalNamespace/BeatmapDifficulty.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
#include "GlobalNamespace/IDifficultyBeatmap.hpp"
#include "Contributor.hpp"
#include <optional>
#include <functional>
#include <utility>

namespace PinkCore::API
{

	struct LevelDetails {
		bool dataIsValid = false;
		bool isCustom = false;
		bool isWIP = false;
		GlobalNamespace::BeatmapCharacteristicSO* characteristic;
		GlobalNamespace::BeatmapDifficulty difficulty;
		bool showRotationSpawnLines;
		const char16_t* environmentType;
		int saberCount;
		bool hasCustomColours;
        std::vector<std::string> currentRequirements = {};
		std::vector<std::string> currentSuggestions = {};
		std::vector<std::string> currentWarnings = {};
		std::vector<std::string> currentInformation = {};
        std::vector<Contributor> currentContributors = {};
	};

	/// @brief returns the current loaded maps level details
	/// @return level detail struct
	LevelDetails& GetCurrentMapData();

	/// @brief returns a new leveldetails struct that of the provided info
	/// @return new level detail struct that isnt saved
    /// @param in the infodat json of the new map
    /// @param difficulty the difficulty of the new beatmap
    /// @param characteristic the characteristic of the new beatmap
    /// @param level ipreviewbeatmap level of the new beatmap
    void GetNewMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, PinkCore::API::LevelDetails& levelDetail, GlobalNamespace::IDifficultyBeatmap* difficultyBeatmap);
    void GetNewMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, PinkCore::API::LevelDetails& levelDetail, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic, GlobalNamespace::IPreviewBeatmapLevel* level);
    void GetNewMapData(rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>& in, PinkCore::API::LevelDetails& levelDetail, GlobalNamespace::BeatmapDifficulty difficulty, GlobalNamespace::BeatmapCharacteristicSO* characteristic);


}
#undef pinkcore_id