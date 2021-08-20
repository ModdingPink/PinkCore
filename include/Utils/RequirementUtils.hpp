#pragma once

#include <string>
#include <vector>
#include "GlobalNamespace/StandardLevelDetailView.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"
#include "CustomTypes/RequirementHandler.hpp"

namespace RequirementUtils
{
	/// @brief handle hte requirement details so all backing information is up to date
	//void HandleRequirementDetails(GlobalNamespace::StandardLevelDetailView* detailView);
	void HandleRequirementDetails();

	/// @brief checks if passed requirement string is installed
	/// @param requirement the requirement to check
	/// @return bool true for installed, false for not installed
	bool GetRequirementInstalled(std::string requirement);

	/// @brief checks if passed requirement string is required
	/// @param requirement the requirement to check
	/// @return bool true for required, false for not required
	bool GetSongHasRequirement(std::string requirement);

	/// @brief checks if passed requirement string is suggested
	/// @param requirement the requirement to check
	/// @return bool true for suggested, false for not suggested
	bool GetSongHasSuggestion(std::string requirement);

	/// @brief checks if passed requirement is a forced suggestion, meaning that if it is required it can be downgraded to suggestion
	/// @param requirement the requirement to check
	/// @return bool true for forced Suggestion, false for not forced suggested
	bool GetIsForcedSuggestion(std::string requirement);

	/// @brief finds all the installed requirements, details on how are still unknown, maybe all files/mods/*.so ?
	void FindInstalledRequirements();

	/// @brief whether or not the play button should be enabled
	/// @return bool true for enable, false for disable
	bool AllowPlayerToStart();

	/// @brief whether or not there are any suggestions or requirements at all
	/// @return true for something, false for nothing
	bool IsAnythingNeeded();
	
	/// @brief whether or not anything is not installed
	/// @return true for something missing, false for all installed
	bool IsAnythingMissing();

	/// @brief updates the requirementHandler in a way that makes it properly show the requirements
	/// @param handler the handler to use
	void UpdateRequirementHandler(PinkCore::UI::RequirementHandler* handler, bool firstUpdate);

	/// @brief updates the play button based on if the player is allowed to start
	void UpdatePlayButton();

	namespace ExternalAPI
	{
		/// @brief
		/// @param identifier the identifier to use
		/// @return bool newly added
		bool RegisterInstalled(std::string identifier);

		/// @brief
		/// @param identifier the identifier to use
		/// @return bool removed succesfully
		bool RemoveInstalled(std::string identifier);

		/// @brief
		/// @param identifier the identifier to use
		/// @return bool newly added
		bool RegisterAsSuggestion(std::string identifier);
		
		/// @brief
		/// @param identifier the identifier to use
		/// @return bool removed succesfully
		bool RemoveSuggestion(std::string identifier);
		
		/// @brief adds to the list of play button disabling mod IDs 
		/// @param id mod ID
		void RegisterDisablingModId(std::string id);

		/// @brief removes from the list of play button disabling mod IDs 
		/// @param id mod ID
		void RemoveDisablingModId(std::string id);
	}
}