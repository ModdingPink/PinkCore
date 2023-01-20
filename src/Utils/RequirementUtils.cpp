#include "Utils/RequirementUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"
#include "Utils/SongUtils.hpp"
#include "logging.hpp"
//#include "CustomTypes/RequirementHandler.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/Button.hpp"
#include "LevelDetailAPI.hpp"

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

	std::vector<std::string> disablingModIds = {};
	
	FoundRequirementsEvent onFoundRequirementsEvent;
	FoundSuggestionsEvent onFoundSuggestionsEvent;
	

	void EmptyRequirements(PinkCore::API::LevelDetails& levelDetail) {
		levelDetail.currentRequirements.clear();
		levelDetail.currentSuggestions.clear();
		RequirementUtils::onFoundRequirements().invoke(levelDetail.currentRequirements);
        RequirementUtils::onFoundSuggestions().invoke(levelDetail.currentSuggestions);
	}

	void HandleRequirementDetails(PinkCore::API::LevelDetails& levelDetail)
	{
		if (installedRequirements.empty()) FindInstalledRequirements();
		levelDetail.currentRequirements.clear();
		levelDetail.currentSuggestions.clear();


		// if custom
		if (SongUtils::SongInfo::get_mapData().isCustom && SongUtils::CustomData::get_currentInfoDatValid())
		{
			auto& doc = SongUtils::GetCurrentInfoDat();
			//INFO("handling requirements for %s", doc["_songName"].GetString());
			rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
			// get the custom data, if it exists
			if (SongUtils::CustomData::GetCurrentCustomDataJson(doc, customData))
			{
				INFO("There was custom data!");
				// there was custom data
				auto requirementsArray = customData.FindMember(u"_requirements");
				if (requirementsArray != customData.MemberEnd())
				{
					INFO("Extracting Requirements");
					SongUtils::CustomData::ExtractRequirements(requirementsArray->value, levelDetail.currentRequirements);
				}

				auto suggestionsArray = customData.FindMember(u"_suggestions");
				if (suggestionsArray != customData.MemberEnd())
				{
					INFO("Extracting Suggestions");
					SongUtils::CustomData::ExtractRequirements(suggestionsArray->value, levelDetail.currentSuggestions);
				}

				auto warningsArray = customData.FindMember(u"_warnings");
				if (warningsArray != customData.MemberEnd())
				{
					INFO("Extracting Warnings");
					SongUtils::CustomData::ExtractRequirements(warningsArray->value, levelDetail.currentWarnings);
				}

				auto informationArray = customData.FindMember(u"_information");
				if (informationArray != customData.MemberEnd())
				{
					INFO("Extracting Suggestions");
					SongUtils::CustomData::ExtractRequirements(informationArray->value, levelDetail.currentInformation);
				}

                RequirementUtils::onFoundRequirements().invoke(levelDetail.currentRequirements);
                RequirementUtils::onFoundSuggestions().invoke(levelDetail.currentSuggestions);
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
		auto mapData = SongUtils::SongInfo::get_mapData();
		if (!mapData.isCustom) return true;
		// for every required requirement
		for (auto req : mapData.currentRequirements)
		{
			// if any is not installed, return false
			if (!GetRequirementInstalled(req) && !GetIsForcedSuggestion(req)) return false;
		}
		// all requirements were installed, we can start!
		return true;
	}

	bool IsAnythingNeeded()
	{
		auto mapData = SongUtils::SongInfo::get_mapData();
		return !mapData.currentRequirements.empty() || !mapData.currentSuggestions.empty();
	}

	bool IsAnythingMissing()
	{
		// if the player is not allowed to start
		// a requirement must be missing
		if (!AllowPlayerToStart()) return true;

		// for every suggested suggestion
		for (auto sug : SongUtils::SongInfo::get_mapData().currentSuggestions)
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
		for (auto req : SongUtils::SongInfo::get_mapData().currentRequirements)
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
		for (auto sug : SongUtils::SongInfo::get_mapData().currentSuggestions)
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
		auto levelViews = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::StandardLevelDetailView*>().LastOrDefault();
		if (levelViews) {
			bool interactable = AllowPlayerToStart();
            bool isCustom = SongUtils::SongInfo::get_mapData().isCustom;
            bool isWip = SongUtils::SongInfo::get_mapData().isWIP;
			INFO("interactable: %d, custom: %d, wip: %d", interactable, isCustom, isWip);
            {
                levelViews->get_practiceButton()->set_interactable(interactable);
                levelViews->get_actionButton()->set_interactable(!(isCustom && isWip) && interactable);
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
				disablingModIds.erase(itr);
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
