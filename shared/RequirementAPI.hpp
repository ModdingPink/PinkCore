#pragma once
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include <optional>

#ifndef pinkcore_id
#define pinkcore_id "pinkcore"
#endif

namespace PinkCore::RequirementAPI
{
	using Value16 = rapidjson::GenericValue<rapidjson::UTF16<char16_t>>;
	/// @brief Register a given string as installed
	/// @param identifier the string to register
	/// @return optional bool, true for success, false for failed
	bool RegisterInstalled(std::string identifier);

	/// @brief Removes an identifier as installed
	/// @param identifier the string to remove
	/// @return optional bool, true for success, false for failed
	bool RemoveInstalled(std::string identifier);

	/// @brief Register a given string as a possible suggestion
	/// @param identifier the string to register
	/// @return optional bool, true for success, false for failed
	bool RegisterAsSuggestion(std::string identifier);

	/// @brief Removes an identifier as a suggestion
	/// @param identifier the string to remove
	/// @return optional bool, true for success, false for failed
	bool RemoveSuggestion(std::string identifier);

	/// @brief Gets the currently selected custom data (diff and characteristic wise)
	/// @param value the reference to the value that will be written to, this will be overwritten!
	/// @return false for failure, true for success
	bool GetCurrentlySelectedCustomData(Value16& value);

	/// @brief disables the play button with your mod, allowing the player to not play maps
	/// @param info your mod info, always pass the same modinfo to the method or we'll get weird behaviour
	void DisablePlayButton(const modloader::ModInfo& info);

	/// @brief disables the play button with your mod, allowing the player to play maps again
	/// @param info your mod info, always pass the same modinfo to the method or we'll get weird behaviour
	void EnablePlayButton(const modloader::ModInfo& info);
}

#undef pinkcore_id