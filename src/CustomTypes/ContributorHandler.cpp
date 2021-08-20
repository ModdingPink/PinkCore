#include "CustomTypes/ContributorHandler.hpp"
#include "CustomTypes/ContributorElement.hpp"
#include "Utils/SongUtils.hpp"
#include "Utils/ContributorUtils.hpp"
#include "Contributor.hpp"

#include "UnityEngine/GameObject.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "UnityEngine/UI/LayoutRebuilder.hpp"

DEFINE_TYPE(PinkCore::UI, ContributorHandler);

using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace QuestUI;
using namespace QuestUI::BeatSaberUI;

extern Logger& getLogger();

namespace PinkCore::UI
{
	void ContributorHandler::OnEnable()
	{
		ActivateAllElements();
	}

	void ContributorHandler::ActivateAllElements()
	{
		Array<ContributorElement*>* elements = GetComponentsInChildren<ContributorElement*>(true);

		int length = elements->Length();

		for (int i = 0; i < length; i++)
		{
			auto elem = elements->values[i];
			elem->EnableObjectIfIsCurrentContributor();
		}
	}

	void ContributorHandler::GetAllCurrentContributors()
	{
		LoggerContextObject logger = getLogger().WithContext("GetAllCurrentContributors");
		logger.info("Fetching List");

		// get the list of contributors
		auto& contributors = ContributorUtils::GetContributors();

		logger.info("Have %lu contributors", contributors.size());
		// if there are no contributors, everything should just be disabled
		if (contributors.size() == 0)
		{
			Array<ContributorElement*>* elements = GetComponentsInChildren<ContributorElement*>(true);
			int length = elements->Length();

			for (int i = 0; i < length; i++)
			{
				elements->values[i]->get_gameObject()->SetActive(false);
			}
		}
		// if we do have some contributors
		else
		{
			// add each contributor
			for (auto contributor : contributors)
			{
				AddContributor(contributor);
			}
		}
	}

	void ContributorHandler::AddContributor(Contributor& contributor)
	{
		Array<ContributorElement*>* elements = GetComponentsInChildren<ContributorElement*>(true);
		int length = elements->Length();

		for (int i = 0; i < length; i++)
		{
			auto elem = elements->values[i];
			// if we find an element that is the same, return, and dont add a new one
			if (elem->GetIsSame(contributor)) return;
		}
		
		// if we get here that means this was a new contributor
		UnityEngine::UI::HorizontalLayoutGroup* object = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(get_transform());
		// last sibling, so they show up at the end of the list at all times
		object->get_transform()->SetAsLastSibling();
		auto elem = object->get_gameObject()->AddComponent<ContributorElement*>();
		elem->Init(contributor);
	}
}