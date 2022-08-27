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
	};

	/// @brief returns whether the currently selected song is custom or not
	/// @return true for custom, false for not custom, nullopt for not installed
	bool get_currentlySelectedIsCustom();

	/// @brief returns whether the currently selected song is wip or not
	/// @return true for wip, false for not wip, nullopt for not installed
	bool get_currentlySelectedIsWIP();
    }

#undef pinkcore_id