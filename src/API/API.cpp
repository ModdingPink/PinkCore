#include "Utils/RequirementUtils.hpp"
#include "Utils/SongUtils.hpp"
#include "beatsaber-hook/shared/utils/typedefs-wrappers.hpp"
#include "beatsaber-hook/shared/rapidjson/include/rapidjson/document.h"

namespace PinkCore::API
{
	bool get_currentlySelectedIsCustom()
	{
		return SongUtils::SongInfo::get_mapIsCustom();
	}

	bool get_currentlySelectedIsWIP()
	{
		return SongUtils::SongInfo::get_mapIsWIP();
	}

	SongUtils::LoadedInfoEvent& GetInfoDatLoadedCallbackSafe()
	{
		return SongUtils::onLoadedInfo();
	}


	RequirementUtils::FoundRequirementsEvent& GetFoundRequirementCallbackSafe()
	{
		return RequirementUtils::onFoundRequirements();
	}

	RequirementUtils::FoundSuggestionsEvent& GetFoundSuggestionCallbackSafe()
	{
		return RequirementUtils::onFoundSuggestions();
	}
}