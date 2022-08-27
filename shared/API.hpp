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

	using LoadedInfoEvent = UnorderedEventCallback<std::optional<std::reference_wrapper<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>>>;
	/// @brief gets a reference to the loaded info event internally used, so other mods can update their data
	/// The passed document pointer CAN be nullptr, this means that the loaded song didn't have an info.dat or was not custom, be sure to check this!
	/// @return optional ref, nullopt means not installed
	LoadedInfoEvent& GetInfoDatLoadedCallbackSafe();

	using FoundRequirementsEvent = UnorderedEventCallback<const std::vector<std::string>&>;
	/// @brief gets a reference to an even that gets ran when the requirements vector has been determined for the specific map
	FoundRequirementsEvent& GetFoundRequirementCallbackSafe();
	
	using FoundSuggestionsEvent = UnorderedEventCallback<const std::vector<std::string>&>;
	/// @brief gets a reference to an even that gets ran when the suggestions vector has been determined for the specific map
	FoundSuggestionsEvent& GetFoundSuggestionCallbackSafe();
}

#undef pinkcore_id