#include "Utils/RequirementUtils.hpp"
#include "Utils/DifficultyNameUtils.hpp"
#include "Utils/SongUtils.hpp"

#include "UnityEngine/Resources.hpp"
#include "UnityEngine/UI/Button.hpp"

using StandardLevelDetailView = GlobalNamespace::StandardLevelDetailView;

#include <algorithm>

extern Logger& getLogger();

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
	
	//void HandleRequirementDetails(StandardLevelDetailView* detailView)
	void HandleRequirementDetails()
	{
		if (installedRequirements.size() == 0) FindInstalledRequirements();
		currentRequirements.clear();
		currentSuggestions.clear();
		
		// if custom
		if (SongUtils::SongInfo::get_currentlySelectedIsCustom() && SongUtils::SongInfo::get_currentInfoDatValid())
		{
			auto& doc = SongUtils::GetCurrentInfoDat();
			//getLogger().info("handling requirements for %s", doc["_songName"].GetString());
			rapidjson::GenericValue<rapidjson::UTF16<char16_t>> customData;
			// get the custom data, if it exists
			if (SongUtils::CustomData::GetCurrentCustomData(doc, customData))
			{
				getLogger().info("There was custom data!");
				// there was custom data
				auto requirementsArray = customData.FindMember(u"_requirements");
				if (requirementsArray != customData.MemberEnd())
				{
					getLogger().info("Extracting Requirements");
					SongUtils::CustomData::ExtractRequirements(requirementsArray->value, currentRequirements);
				}

				auto suggestionsArray = customData.FindMember(u"_suggestions");
				if (suggestionsArray != customData.MemberEnd())
				{
					getLogger().info("Extracting Suggestions");
					SongUtils::CustomData::ExtractRequirements(suggestionsArray->value, currentSuggestions);
				}

			}
			else
			{
				// there was no custom data
				getLogger().info("There was no custom data!");
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
		return currentRequirements.size() || currentSuggestions.size();
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
		static LoggerContextObject logger = getLogger().WithContext("Install Check");

		// find the req in the suggestions list, if found return true, else return false
		for (auto req : installedRequirements)
		{
			if (req.find(requirement) != std::string::npos)
			{
				logger.info("Match installed %s, %s", req.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	bool GetIsForcedSuggestion(std::string requirement)
	{
		static LoggerContextObject logger = getLogger().WithContext("Forced Suggestion Check");
		// find the req in the suggestions list, if found return true, else return false
		for (auto sug : forcedSuggestions)
		{
			if (sug.find(requirement) != std::string::npos)
			{
				logger.info("Match requirement %s, %s", sug.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	bool GetSongHasRequirement(std::string requirement)
	{
		static LoggerContextObject logger = getLogger().WithContext("Requirement Check");
		// find the req in the suggestions list, if found return true, else return false
		for (auto req : currentRequirements)
		{
			if (req.find(requirement) != std::string::npos)
			{
				logger.info("Match requirement %s, %s", req.c_str(), requirement.c_str());
				return true;
			}
		}
		return false;
	}

	bool GetSongHasSuggestion(std::string requirement)
	{
		static LoggerContextObject logger = getLogger().WithContext("Suggestion Check");
		// find the req in the suggestions list, if found return true, else return false
		for (auto sug : currentSuggestions)
		{
			if (sug.find(requirement) != std::string::npos)
			{
				logger.info("Match Suggestion %s, %s", sug.c_str(), requirement.c_str());
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
				getLogger().info("Found loaded id: %s", mod.second.info.id.c_str());
			}
		}
	}

	void UpdateRequirementHandler(PinkCore::UI::RequirementHandler* handler, bool firstUpdate)
	{
		getLogger().info("Handler ptr: %p", handler);
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

	void UpdatePlayButton()
	{
		if (!SongUtils::SongInfo::get_currentlySelectedIsCustom()) return;
		auto levelViews = UnityEngine::Resources::FindObjectsOfTypeAll<GlobalNamespace::StandardLevelDetailView*>();
		int length = levelViews->Length();
		if (length > 0)
		{
			bool interactable = AllowPlayerToStart();
			if (SongUtils::SongInfo::get_currentlySelectedIsWIP())
			{
				levelViews->values[length - 1]->get_practiceButton()->set_interactable(interactable);
				levelViews->values[length - 1]->get_actionButton()->set_interactable(false);
			}
			else
			{
				levelViews->values[length - 1]->get_practiceButton()->set_interactable(interactable);
				levelViews->values[length - 1]->get_actionButton()->set_interactable(interactable);
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
				getLogger().info("Mod %s is trying to disable the play button again!", id.c_str());
				return;
			}
			else
			{
				getLogger().info("Mod %s is disabling the play button!", id.c_str());
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
				getLogger().info("Mod %s is no longer disabling the play button", id.c_str());
				disablingModIds.erase(itr, itr++);
				return;
			}
			UpdatePlayButton();
		}
	}
}