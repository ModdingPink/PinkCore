#include "Utils/ContributorUtils.hpp"
#include "Utils/SongUtils.hpp"
#include "logging.hpp"
#include "LevelDetailAPI.hpp"
//#include "CustomTypes/ContributorHandler.hpp"

using Contributor = PinkCore::Contributor;
namespace ContributorUtils
{

	void EmptyContributors() {
		SongUtils::SongInfo::get_mapData().currentContributors.clear();
	}

	void FetchListOfContributors(PinkCore::API::LevelDetails& levelDetail)
	{
		levelDetail.currentContributors.clear();
		// if current info is not valid, there is no use in trying to read it
		if (SongUtils::CustomData::get_currentInfoDatValid())
		{   
			auto& doc = SongUtils::GetCurrentInfoDat();
			// try to find the custom data, which has the contributors
			auto customDataitr = doc.FindMember(u"_customData");
			if (customDataitr != doc.MemberEnd())
			{
				rapidjson::GenericValue<rapidjson::UTF16<char16_t>>& customData = customDataitr->value;
				// try to find the contributors array
				auto contributorsMemberItr = customData.FindMember(u"_contributors");
				if (contributorsMemberItr != customData.MemberEnd())
				{
					// get the array
					auto contributors = contributorsMemberItr->value.GetArray();

					// add every contributor
					for (auto& contributor : contributors)
					{
						levelDetail.currentContributors.push_back(Contributor(contributor));
					}
				}
			}
		}
	}

	bool GetIsCurrentContributor(Contributor& contributor)
	{
		for (auto& cont : SongUtils::SongInfo::get_mapData().currentContributors)
		{
			// if we find a match, return true
			if (cont == contributor) return true;
		}
		return false;
	}

	bool DidAnyoneWorkOnThis()
	{
		return SongUtils::SongInfo::get_mapData().currentContributors.size() != 0;
	}
	
	/*
	void UpdateContributorHandler(PinkCore::UI::ContributorHandler* handler, bool firstUpdate)
	{
		if (!handler) return;
		handler->GetAllCurrentContributors();
	}
	*/
}