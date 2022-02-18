#include "Utils/RequirementUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"
#include "Utils/SongUtils.hpp"
#include "logging.hpp"
//#include "CustomTypes/RequirementHandler.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/Button.hpp"

using StandardLevelDetailView = GlobalNamespace::StandardLevelDetailView;

#include <algorithm>

static std::string toLower(std::string in)
{
	std::string output = "";
	for (auto& c : in)
	{
		output += tolower(c);
	}

	return output;
}

static std::string removeSpaces(std::string input)
{
	std::string output = "";
	for (auto c : input)
	{
		if (c == ' ') continue;
		output += c;
	}
	return output;
}

namespace RequirementUtils
{

	std::vector<std::string> installedRequirements = {};
	std::vector<std::string> forcedSuggestions = {};
	std::vector<std::string> currentRequirements = {};
	std::vector<std::string> currentSuggestions = {};

	std::vector<std::string> disablingModIds = {};
	
	FoundRequirementsEvent onFoundRequirementsEvent;
	FoundSuggestionsEvent onFoundSuggestionsEvent;
	
	//void HandleRequirementDetails(StandardLevelDetailView* detailView)
	void HandleRequirementDetails()
	{
		if (installedRequirements.empty()) FindInstalledRequirements();
		currentRequirements.clear();
		currentSuggestions.clear();


		// if custom
		if (SongUtils::SongInfo::get_currentlySelectedIsCustom() && SongUtils::SongInfo::get_currentInfoDatValid())
		{
			auto& doc = SongUtils::GetCurrentInfoDat();
			//INFO("handling requirements for %s", doc["_songName"].GetString());
			rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
			// get the custom data, if it exists
			if (SongUtils::CustomData::GetCurrentCustomData(doc, customData))
			{
				INFO("There was custom data!");
				// there was custom data
				auto requirementsArray = customData.FindMember(u"_requirements");
				if (requirementsArray != customData.MemberEnd())
				{
					INFO("Extracting Requirements");
					SongUtils::CustomData::ExtractRequirements(requirementsArray->value, currentRequirements);
				}

				auto suggestionsArray = customData.FindMember(u"_suggestions");
				if (suggestionsArray != customData.MemberEnd())
				{
					INFO("Extracting Suggestions");
					SongUtils::CustomData::ExtractRequirements(suggestionsArray->value, currentSuggestions);
				}

                RequirementUtils::onFoundRequirements().invoke(RequirementUtils::GetCurrentRequirements());
                RequirementUtils::onFoundSuggestions().invoke(RequirementUtils::GetCurrentSuggestions());
			}
			else
			{
				// there was no custom data
				INFO("There was no custom data!");
			}
		}

	}

	bool AllowPlayerToStart()
	{
		if (disablingModIds.size() > 0) return false;
		if (!SongUtils::SongInfo::get_currentlySelectedIsCustom()) return true;
		// for every required requirement
		for (auto req : currentRequirements)
		{
			// if any is not installed, return false
			if (!GetRequirementInstalled(req) && !GetIsForcedSuggestion(req)) return false;
		}
		// all requirements were installed, we can start!
		return true;
	}

	bool IsAnythingNeeded()
	{
		return !currentRequirements.empty() || !currentSuggestions.empty();
	}

	bool IsAnythingMissing()
	{
		// if the player is not allowed to start
		// a requirement must be missing
		if (!AllowPlayerToStart()) return true;

		// for every suggested suggestion
		for (auto sug : currentSuggestions)
		{
			// if any is not installed, return false
			if (!GetRequirementInstalled(sug)) return true;
		}

		// all requirements/suggestions were installed, nothing is missing!
		return false;
	}

	bool GetRequirementInstalled(std::string requirement)
	{
		// find the req in the suggestions list, if found return true, else return false
		for (auto const& req : installedRequirements)
		{
			if (req.find(requirement) != std::string::npos)
			{
				INFO("Match installed %s, %s", req.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	bool GetIsForcedSuggestion(std::string requirement)
	{
		// find the req in the suggestions list, if found return true, else return false
		for (auto sug : forcedSuggestions)
		{
			if (sug.find(requirement) != std::string::npos)
			{
				INFO("Match requirement %s, %s", sug.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	bool GetSongHasRequirement(std::string requirement)
	{
		// find the req in the suggestions list, if found return true, else return false
		for (auto req : currentRequirements)
		{
			if (req.find(requirement) != std::string::npos)
			{
				INFO("Match requirement %s, %s", req.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	bool GetSongHasSuggestion(std::string requirement)
	{
		// find the req in the suggestions list, if found return true, else return false
		for (auto sug : currentSuggestions)
		{
			if (sug.find(requirement) != std::string::npos)
			{
				INFO("Match Suggestion %s, %s", sug.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	void FindInstalledRequirements()
	{
		return;
		for (auto mod : Modloader::getMods())
		{
			// if mod is loaded, put it in the list of installed requirements
			if (mod.second.get_loaded())
			{
				installedRequirements.push_back(mod.second.info.id);
				INFO("Found loaded id: %s", mod.second.info.id.c_str());
			}
		}
	}

	const std::vector<std::string>& GetCurrentRequirements()
	{
		return currentRequirements;
	}

	const std::vector<std::string>& GetCurrentSuggestions()
	{
		return currentSuggestions;
	}

	/*
	void UpdateRequirementHandler(PinkCore::UI::RequirementHandler* handler, bool firstUpdate)
	{
		INFO("Handler ptr: %p", handler);
		if (!handler) return;
		for (auto req : currentRequirements)
		{
			handler->AddID(req);
		}

		for (auto sug : currentSuggestions)
		{
			handler->AddID(sug);
		}

		if (!firstUpdate) handler->CheckAllRequirements();
	}
	*/
	void UpdatePlayButton()
	{
		auto levelViews = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::StandardLevelDetailView*>();
		int length = levelViews.Length();
		if (length > 0)
		{
			bool interactable = AllowPlayerToStart();
                        bool isCustom = SongUtils::SongInfo::get_currentlySelectedIsCustom();
                        bool isWip = SongUtils::SongInfo::get_currentlySelectedIsWIP();
			INFO("interactable: %d, custom: %d, wip: %d", interactable, isCustom, isWip);
            {
                levelViews[length - 1]->get_practiceButton()->set_interactable(interactable);
                levelViews[length - 1]->get_actionButton()->set_interactable(!(isCustom && isWip) && interactable);
            }
		}
	}
	namespace ExternalAPI
	{
		bool RegisterInstalled(std::string identifier)
		{
			auto it = std::find(installedRequirements.begin(), installedRequirements.end(), identifier);

			if (it == installedRequirements.end())
			{
				installedRequirements.push_back(identifier);
				std::sort(installedRequirements.begin(), installedRequirements.end());
				return true;
			}
			else return false;
		}

		bool RemoveInstalled(std::string identifier)
		{
			auto it = std::find(installedRequirements.begin(), installedRequirements.end(), identifier);
			if (it != installedRequirements.end())
			{
				installedRequirements.erase(it, it + 1);
				return true;
			}
			else return false;
		}

		bool RegisterAsSuggestion(std::string identifier)
		{
			auto it = std::find(forcedSuggestions.begin(), forcedSuggestions.end(), identifier);

			if (it == forcedSuggestions.end())
			{
				forcedSuggestions.push_back(identifier);
				std::sort(forcedSuggestions.begin(), forcedSuggestions.end());
				return true;
			}
			else return false;
		}
		
		bool RemoveSuggestion(std::string identifier)
		{
			auto it = std::find(forcedSuggestions.begin(), forcedSuggestions.end(), identifier);
			if (it != forcedSuggestions.end())
			{
				forcedSuggestions.erase(it, it + 1);
				return true;
			}
			else return false;
		}

		

		void RegisterDisablingModId(std::string id)
		{
			auto itr = std::find(disablingModIds.begin(), disablingModIds.end(), id);
			if (itr != disablingModIds.end()) 
			{
				INFO("Mod %s is trying to disable the play button again!", id.c_str());
				return;
			}
			else
			{
				INFO("Mod %s is disabling the play button!", id.c_str());
				disablingModIds.push_back(id);
				std::sort(disablingModIds.begin(), disablingModIds.end());
			}
			UpdatePlayButton();
		}

		void RemoveDisablingModId(std::string id)
		{
			auto itr = std::find(disablingModIds.begin(), disablingModIds.end(), id);
			if (itr != disablingModIds.end()) 
			{
				INFO("Mod %s is no longer disabling the play button", id.c_str());
				disablingModIds.erase(itr, itr++);
				return;
			}
			UpdatePlayButton();
		}
	}

	FoundRequirementsEvent& onFoundRequirements() {
		return onFoundRequirementsEvent;
	}

	FoundSuggestionsEvent& onFoundSuggestions() {
		return onFoundSuggestionsEvent;
	}
}
