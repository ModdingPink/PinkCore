#pragma once
#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

#include <optional>
#include <functional>
#include <utility>

namespace PinkCore::API
{
	/// @brief returns whether the currently selected song is custom or not
	/// @return true for custom, false for not custom, nullopt for not installed
	bool get_currentlySelectedIsCustom();

	/// @brief returns whether the currently selected song is wip or not
	/// @return true for wip, false for not wip, nullopt for not installed
	bool get_currentlySelectedIsWIP();

	using LoadedInfoEvent = UnorderedEventCallback<std::optional<std::reference_wrapper<rapidjson::GenericDocument<rapidjson::UTF16<char16_t>>>>>;
	/// @brief gets a reference to the loaded info event internally used, so other mods can update their data
	/// The passed document pointer CAN be nullptr, this means that the loaded song didn't have an info.dat or was not custom, be sure to check this!
	/// @return optional ref, nullopt means not installed
	LoadedInfoEvent& GetInfoDatLoadedCallbackSafe();
}

#undef pinkcore_id